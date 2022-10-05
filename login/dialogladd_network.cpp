#include "dialogladd_network.h"

#include "widget/toastmsg.h"

#include <common/gscommon.h>
#include <common/global.h>
#include <common/networkhttp.h>
#include <common/networkhttp.h>
#include <widget/toastmsg.h>

#include <QProcess>
#include <QDir>
#include <QShortcut>//c220906_1
#include <QApplication>//c220906_1
#include "common/global.h"//c220907_1
#include "widget/dialogconfirm_macshare.h"


using namespace Dialog;

/**
 * @brief DialogAdd_Network::DialogAdd_Network : 네트워크 폴더 추가 Dialog
 * @param parent
 */
DialogAdd_Network::DialogAdd_Network(QWidget *parent) : QDialog(parent)
{
    setInit();
    setUIControl();
}


/**
 * @brief DialogAdd_Network::setInit : 초기 세팅
 */
void DialogAdd_Network::setInit(){

    linker = Linker::getInstance();

    // --------------------------------------------------------
    //  Set Dialog property
    // --------------------------------------------------------
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
#if defined(Q_OS_WIN)
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT+70);//c220913_1
#elif defined(Q_OS_MAC)//c220913_1
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT+300);//c220913_1
#endif

    this->setStyleSheet("background-color:transparent;");


}

/**
 * @brief DialogAdd_Network::setUIControl : UI 세팅 함수
 */
void DialogAdd_Network::setUIControl(){//c220913_1

    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Add network folder"));
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(70);

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setParent(lb_title);
    btn_close->setGeometry(530, 0, 70, 70);
    btn_close->show();

    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title);

    QWidget *widget_title = new QWidget;
    widget_title->setObjectName("widget_title");
    widget_title->setLayout(box_top);
    widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");


    lineEdit_http = new QLineEdit();
    lineEdit_http->setMaxLength(200);
    //lineEdit_http->setPlaceholderText(tr("Network share folder Url path"));//c220918_1
    lineEdit_http->setFixedHeight(50);
    lineEdit_http->setTextMargins(23,14,23,14);
    lineEdit_http->setStyleSheet("font-size:18px;color:#ffffff;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    QLabel *lb_commentHttp = new QLabel();
    lb_commentHttp->setText("ex) //host or ip/sharepath or smb://host or ip/sharesharepath");
    lb_commentHttp->setStyleSheet("font-size:16px;color:#B18658;");

    // 아이디
    lineEdit_id = new QLineEdit;
    lineEdit_id->setMaxLength(30);
    //lineEdit_id->setPlaceholderText(tr("ID"));
    lineEdit_id->setFixedHeight(50);
    lineEdit_id->setTextMargins(23,14,23,14);
    lineEdit_id->setStyleSheet("font-size:18px;color:#ffffff;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    // 패스워드
    lineEdit_pw = new QLineEdit;
    lineEdit_pw->setMaxLength(30);
    //lineEdit_pw->setPlaceholderText(tr("password"));
    lineEdit_pw->setFixedHeight(50);
    lineEdit_pw->setTextMargins(23,14,23,14);
    lineEdit_pw->setEchoMode(QLineEdit::Password);
    lineEdit_pw->setStyleSheet("font-size:18px;color:#ffffff;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    lineEdit_path = new QLineEdit;//
    lineEdit_path->setMaxLength(30);
    //lineEdit_path->setPlaceholderText(tr("Share Folder"));
    lineEdit_path->setFixedHeight(50);
    lineEdit_path->setTextMargins(23,14,23,14);
    //lineEdit_path->setEchoMode(QLineEdit::Password);
    lineEdit_path->setStyleSheet("font-size:18px;color:#ffffff;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    lineEdit_sudopw = new QLineEdit;//
    lineEdit_sudopw->setMaxLength(30);
    //lineEdit_sudopw->setPlaceholderText(tr("desktop user password"));
    lineEdit_sudopw->setFixedHeight(50);
    lineEdit_sudopw->setTextMargins(23,14,23,14);
    lineEdit_sudopw->setEchoMode(QLineEdit::Password);
    lineEdit_sudopw->setStyleSheet("font-size:18px;color:#ffffff;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    lineEdit_username = new QLineEdit;//
    lineEdit_username->setMaxLength(30);
    //lineEdit_username->setPlaceholderText(tr("user name"));
    lineEdit_username->setFixedHeight(55);
    lineEdit_username->setTextMargins(23,14,23,14);
    //lineEdit_username->setEchoMode(QLineEdit::Password);
    lineEdit_username->setStyleSheet("font-size:18px;color:#ffffff;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");


#if defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)//c220913_1
    if(global.shareFolderInfoSel.size()>0){
        lineEdit_http->setText(QString("//%1/%2").arg(global.shareFolderInfoSel.at(0)).arg(global.shareFolderInfoSel.at(3)));
        lineEdit_id->setText(global.shareFolderInfoSel.at(1));
        lineEdit_pw->setText(global.shareFolderInfoSel.at(2));
        lineEdit_path->setText(global.shareFolderInfoSel.at(3));
    }
    global.shareFolderInfoSel.clear();
#endif

    QLabel *lable_search_icon = GSCommon::getUILabelImg(":/images/icon_ser_w.png");//c220906_1
    lable_search_icon->resize(40, 40);

    QLabel *lb_networkFolderSearcch = new QLabel();//c220906_1
    lb_networkFolderSearcch->setText(tr("Add after finding network folder"));
    lb_networkFolderSearcch->setStyleSheet("font-size:16px;color:#cccccc;");

    QLabel *lb_networkFolderUrl = new QLabel();//c220918_1
    lb_networkFolderUrl->setFixedHeight(20);
    lb_networkFolderUrl->setText(tr(" network share folder Url path"));
    lb_networkFolderUrl->setStyleSheet("font-size:16px;color:#cccccc;");
    QLabel *lb_networkFolderID = new QLabel();//
    lb_networkFolderID->setFixedHeight(20);
    lb_networkFolderID->setText(tr(" network host id"));
    lb_networkFolderID->setStyleSheet("font-size:16px;color:#cccccc;");
    QLabel *lb_networkFolderPW = new QLabel();//
    lb_networkFolderPW->setFixedHeight(20);
    lb_networkFolderPW->setText(tr(" network host password"));
    lb_networkFolderPW->setStyleSheet("font-size:16px;color:#cccccc;");
    QLabel *lb_networkFolderPATH = new QLabel();//
    lb_networkFolderPATH->setFixedHeight(20);
    lb_networkFolderPATH->setText(tr(" network share pathname"));
    lb_networkFolderPATH->setStyleSheet("font-size:16px;color:#cccccc;");
    QLabel *lb_networkFolderSUDOPW = new QLabel();//
    lb_networkFolderSUDOPW->setFixedHeight(20);
    lb_networkFolderSUDOPW->setText(tr(" desktop user password"));
    lb_networkFolderSUDOPW->setStyleSheet("font-size:16px;color:#cccccc;");
    QLabel *lb_networkFolderUSERNAME = new QLabel();//
    lb_networkFolderUSERNAME->setFixedHeight(20);
    lb_networkFolderUSERNAME->setText(tr(" desktop user name"));
    lb_networkFolderUSERNAME->setStyleSheet("font-size:16px;color:#cccccc;");
    // 하단 버튼
    btn_search = new QPushButton;//c220906_1
    btn_search->setText(tr("Search"));
    btn_search->setCursor(Qt::PointingHandCursor);
    btn_search->setStyleSheet("font-size:16px;color:#CCCCCC;border:1px solid #777777;border-radius:15px;");
    btn_search->setFixedSize(120,30);

    QHBoxLayout *hl_search = new QHBoxLayout();//c220906_1
    hl_search->setContentsMargins(0,0,0,0);
    hl_search->setSpacing(0);
    hl_search->setAlignment(Qt::AlignLeft);
    hl_search->addWidget(lable_search_icon);
    hl_search->addWidget(lb_networkFolderSearcch);
    hl_search->addSpacing(30);
    hl_search->addWidget(btn_search);

    QPushButton *btn_ok = new QPushButton;
    btn_ok->setText(tr("Confirm"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    btn_ok->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_ok->setFixedSize(140,40);
    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));
    btn_cancel->setCursor(Qt::PointingHandCursor);
    btn_cancel->setStyleSheet("font-size:18px;color:#CCCCCC;border:1px solid #777777;border-radius:20px;");
    btn_cancel->setFixedSize(140,40);
    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->setContentsMargins(0,0,0,0);
    hl_btn->setSpacing(0);
    hl_btn->setAlignment(Qt::AlignCenter);
    hl_btn->addWidget(btn_ok);
    hl_btn->addSpacing(30);
    hl_btn->addWidget(btn_cancel);

    QVBoxLayout *vl_editInfo = new QVBoxLayout;
    vl_editInfo->setContentsMargins(25,25,25,30);
    vl_editInfo->setSpacing(0);
    vl_editInfo->addWidget(lb_networkFolderUrl);//c220918_1
    vl_editInfo->addWidget(lineEdit_http);
    vl_editInfo->addSpacing(0);//c220918_1
    vl_editInfo->addWidget(lb_commentHttp);

    vl_editInfo->addSpacing(10);
#if defined(Q_OS_MAC)//
    vl_editInfo->addLayout(hl_search);//c220907_1
    vl_editInfo->addSpacing(20);//c220907_1
#endif//c220907_1
    vl_editInfo->addWidget(lb_networkFolderID);
    vl_editInfo->addWidget(lineEdit_id);
    vl_editInfo->addSpacing(30);
    vl_editInfo->addWidget(lb_networkFolderPW);
    vl_editInfo->addWidget(lineEdit_pw);
    vl_editInfo->addSpacing(35);//
#if defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)//
    vl_editInfo->addWidget(lb_networkFolderPATH);
    vl_editInfo->addWidget(lineEdit_path);
    vl_editInfo->addSpacing(35);
    vl_editInfo->addWidget(lb_networkFolderUSERNAME);
    vl_editInfo->addWidget(lineEdit_username);

    vl_editInfo->addSpacing(35);
    vl_editInfo->addWidget(lb_networkFolderSUDOPW);//
    vl_editInfo->addWidget(lineEdit_sudopw);//
    vl_editInfo->addSpacing(35);
#endif

    vl_editInfo->addLayout(hl_btn);


    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_title);
    vl_total->addLayout(vl_editInfo);


    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(vl_total);
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");
    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);

    this->setLayout(vboxlayout);

    // 커넥션

    connect(lineEdit_http, SIGNAL(textChanged(QString)), this, SLOT(slot_textChanged(QString)));//
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_ok()));
    connect(btn_search, SIGNAL(clicked()), this, SLOT(slot_folderSearch()));//c220906_1
    connect(this, SIGNAL(signal_process_smb(QString)), this, SLOT(slot_process_smb(QString)));//c220909_1
#if defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)//
    QProcess *process1 = new QProcess();
    process1->start(QString("sh -c \"whoami\""));
    process1->waitForFinished();
    QString whoami = process1->readAllStandardOutput();
    qDebug() << "whoami=" << whoami;
    lineEdit_username->setText(whoami.split("\n").first());
#endif


}



void DialogAdd_Network::slot_textChanged(QString str){//c220913_2
    print_debug();
    qDebug() <<"str="<< str;
    QStringList httpList = str.split("/");
    if(httpList.size() >= 3){
        QString tmp_ip = httpList.at(2);
        QStringList pathList;

        if(httpList.size() >3){
            for(int i = 3; i < httpList.size(); i++){
                if(!httpList.at(i).isEmpty()){
                    pathList.append(httpList.at(i));
                }
            }
        }

#if defined(Q_OS_WIN)
        QString tmp_path = pathList.join("/");
        qDebug() << "tmp_path=" << tmp_path;
#elif defined(Q_OS_MAC)
        QString tmp_path = pathList.join("-");
        qDebug() << "tmp_path=" << tmp_path;
#endif
        lineEdit_path->setText(tmp_path);

    }

}

void DialogAdd_Network::slot_folderSearch(){//c220906_1
    print_debug();
#if defined(Q_OS_WIN)
    this->hide();
    emit linker->signal_searchFolder_clicked();
#elif defined(Q_OS_MAC)
    this->hide();
    emit linker->signal_searchFolder_clicked();
    //QString cmd = QString("open %1").arg("/System/Library/CoreServices/Finder.app");
    //QProcess::startDetached(cmd);


    //QTimer::singleShot(4000, this, SLOT(slotShortcutFinderSMB()));
#endif




}

void DialogAdd_Network::slotShortcutFinderSMB(){//c220906_1
    print_debug();
    //QShortcut *keyCtrlK = new QShortcut(this); // Initialize the object
    //keyCtrlK->setKey(Qt::CTRL + Qt::Key_K); // Set the key code
    //keyCtrlK->setKey("Ctrl+K");
    // connect handler to keypress
    //connect(keyCtrlK, SIGNAL(activated()), this, SLOT(slotShortcutCtrlD()));
    //keyCtrlK->setEnabled(true);
    //emit keyCtrlK->activated();

    //QKeyEvent key(QEvent::KeyPress, Qt::Key_K, Qt::ControlModifier);
    print_debug();
    //QApplication::sendEvent(btn_search, &key);
    //QApplication::sendEvent(this, &key);
    //this->setEnabled(true);
    //btn_search->grabKeyborad() ;
    qDebug() << "QApplication::activeWindow()=" << QApplication::activeWindow();
    //qDebug() << "QApplication::activeWindow()->windowTitle()=" << QApplication::activeWindow()->windowTitle();
    QKeyEvent *key = new QKeyEvent (QEvent::KeyPress, Qt::Key_K, Qt::ControlModifier);
    QCoreApplication::postEvent (QApplication::activeWindow(), key);
    print_debug();
    if (key->isAccepted()) {
          qDebug()<<"everything is ok";
    } else {
          qDebug()<<"something wrong";
    }

}

/**
 * @brief DialogAdd_Network::setFolderInfo : 추가할 네트워크 폴더 데이터 세팅
 * @param p_host QString
 * @param p_id QString
 * @param p_pw QString
 */
void DialogAdd_Network::setFolderInfo(QString p_host, QString p_id, QString p_pw){
    lineEdit_http->setText(p_host);
    lineEdit_id->setText(p_id);
    lineEdit_pw->setText(p_pw);
}

/**
 * @brief DialogLogin_Network::checkUserInput::사용자 입력값 검사
 * @details 공유폴더주소만 확인, ID, PW는 검사하지 않는다.(익명 로그인)
 * @return bool
 */
bool DialogAdd_Network::checkUserInput(){
    bool flagOk = true;

    QStringList httpList = lineEdit_http->text().split("/");
    print_debug();
    qDebug()  << "httpList.size()=" << httpList.size();
    qDebug()  << "httpList=" << httpList;

    if(lineEdit_http->text().trimmed().isEmpty()){
        print_debug();
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please enter the network folder address.")));
    }
    else if(httpList.size() < 3){
        print_debug();
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please check the network folder address.")));
    }
#if defined(Q_OS_MAC)//c220909_1
    else if(httpList.size() == 3){
        print_debug();
        flagOk = false;
        global.window_activate_flag = true;//c220907_2
        ToastMsg::show(this,"",tr("The Share Path information is incorrect."), 4000);
    }
#endif
#if defined(Q_OS_WINDOWS)

#endif


    return flagOk;
}

/**
 * @brief DialogLogin_Network::slot_ok [SLOT] 확인 버튼 클릭시
 */
void DialogAdd_Network::slot_ok(){//c220913_1

    print_debug();
    if(checkUserInput()){
        QStringList httpList = lineEdit_http->text().split("/");
        //c220908_1  이하 6줄
#if defined(Q_OS_MAC)
        if(httpList.size() >= 3){//c220909_1
            print_debug();
#endif
#if defined(Q_OS_WINDOWS)
            if(httpList.size() >= 3){
#endif

                QString tmp_ip = httpList.at(2);
                QStringList pathList;

                if(httpList.size() >3){
                    for(int i = 3; i < httpList.size(); i++){
                        if(!httpList.at(i).isEmpty()){
                            pathList.append(httpList.at(i));
                        }
                    }
                }

                QString tmp_path = pathList.join("/");


#if defined(Q_OS_WINDOWS)

                QString tmp_win_sep = "";
                if(!tmp_path.isEmpty()){
                    tmp_win_sep = "\\";
                }

                // net.exe 이용 smb 연결 삭제
                QProcess *process = new QProcess(this);
                process->setProgram("net.exe");

                // 강제 /delete  오류 구문 실행
                // :: 특정 Window에서 net use 목록이 없음에도 네트워크 폴더 등록시 동일한 사용자 오류 발생(1219)
                // :: 강제 /delete 오류 발생후 등록시 정상적으로 등록되는 현상있음.
                //process->setArguments(QStringList()<<"use"<<"/delete");
                //process->startDetached();
                //process->waitForFinished(-1);


                process->setArguments(QStringList()<<"use"<<"/delete"<<"\\\\"+tmp_ip+tmp_win_sep+tmp_path.replace("/","\\"));
                process->startDetached();
                process->waitForFinished(-1);

                QStringList strListProcess;
                strListProcess.append("use");
                strListProcess.append("\\\\"+tmp_ip+tmp_win_sep+tmp_path.replace("/","\\"));
                if(lineEdit_pw->text().trimmed().length() > 0){
                    strListProcess.append(lineEdit_pw->text());
                }
                if(lineEdit_id->text().trimmed().length() > 0){
                    strListProcess.append("/user:"+lineEdit_id->text());
                }
                strListProcess.append("/persistent:no");

                // net.exe 이용 smb 연결 추가
                process->setArguments(strListProcess);
                process->startDetached();
                process->waitForFinished(-1);

                process->close();
                delete process;

                requestAddNetworkFolder(tmp_ip, tmp_path.replace("\\","/"));
#elif defined(Q_OS_MAC)
                // 맥 관련 SMB 기술
                print_debug();
                global.userPassword = lineEdit_sudopw->text();
                QProcess* process = new QProcess();
                QStringList sArgs;
                QFileInfo check_file("/Volumes/"+tmp_ip+"-"+lineEdit_path->text());
                //QFileInfo check_file("//"+p_ip+"/"+p_path);
                if(!check_file.exists()){
                    print_debug();
                    /*//---------------------
                    QProcess *process = new QProcess();
                    process->setProgram("/bin/sh");

                    QStringList strListProcess;
                    QString cmd_argu = QString("-c \"mkdir /Volumes/%1\"").arg(lineEdit_path->text());
                    qDebug() << "cmd_argu=" << cmd_argu;
                    strListProcess.append(cmd_argu);
                    // net.exe 이용 smb 연결 추가
                    process->setArguments(strListProcess);
                    bool bb = process->startDetached();
                    qDebug() << "bb=" << bb;
                    process->waitForFinished(-1);
                    //----------------------*/

                    if(lineEdit_sudopw->text().isEmpty()){
                        qDebug() << "Enter your Mac user password.";
                        global.window_activate_flag = true;//c220907_2
                        ToastMsg::show(this,"",tr("Enter your Mac user password."), 4000);
                    }else{
                        process->start(QString("sh -c \"echo %1 | sudo -S ls /\"").arg(lineEdit_sudopw->text()));//c220913_1
                        process->waitForFinished();
                        QString std_out = process->readAllStandardOutput();
                        if(std_out.isEmpty()){
                            qDebug() << "The password input is incorrect.";
                            global.window_activate_flag = true;//c220907_2
                            ToastMsg::show(this,"",tr("The password input is incorrect."), 4000);
                            return;
                        }

                        process->start(QString("sh -c \"echo %2 | sudo -S umount /Volumes/%1\"").arg(tmp_ip+"-"+lineEdit_path->text()).arg(lineEdit_sudopw->text()));//c220913_1
                        process->waitForFinished();
                        process->start(QString("sh -c \"echo %2 | sudo -S mkdir /Volumes/%1\"").arg(tmp_ip+"-"+lineEdit_path->text()).arg(lineEdit_sudopw->text()));//c220913_1
                        qDebug() << "path= " << QString("sh -c \"echo %2 | sudo -S mkdir /Volumes/%1\"").arg(tmp_ip+"-"+lineEdit_path->text()).arg(lineEdit_sudopw->text());
                        process->waitForFinished();
                        process->start(QString("sh -c \"echo %2 | sudo -S chmod 700 /Volumes/%1\"").arg(tmp_ip+"-"+lineEdit_path->text()).arg(lineEdit_sudopw->text()));//c220913_1
                        process->waitForFinished();
                        process->start(QString("sh -c \"echo %2 | sudo -S chown -R %3 /Volumes/%1\"").arg(tmp_ip+"-"+lineEdit_path->text()).arg(lineEdit_sudopw->text()).arg(lineEdit_username->text()));//c220913_1
                        process->waitForFinished();
                        process->start(QString("sh -c \"mount_smbfs //%1:%2@%3/%4 /Volumes/%5\"").arg(lineEdit_id->text()).arg(lineEdit_pw->text()).arg(tmp_ip).arg(lineEdit_path->text()).arg(tmp_ip+"-"+lineEdit_path->text()));//c220913_1
                        process->waitForFinished();
                        QString standardOutput = process->readAllStandardOutput();
                        qDebug() << standardOutput;
                        qDebug() << process->readAllStandardError();

                        requestAddNetworkFolder(tmp_ip, lineEdit_path->text().replace("\\","/"));//cheon210831-network
                    }

                }else{
                    qDebug() << "The folder name already exists.";
                    global.window_activate_flag = true;//c220907_2
                    ToastMsg::show(this,"",tr("The folder name already exists."), 4000);
                }




#endif
            }
        }

    }


    /**
 * @brief DialogLogin_Network::requestAddNetworkFolder::네트워크 폴더 등록
 */
    void DialogAdd_Network::requestAddNetworkFolder(const QString &p_ip, const QString &p_path){//c220913_1

        print_debug();
        qDebug() << "p_ip=" << p_ip;
        qDebug() << "p_path=" << p_path;
#if defined(Q_OS_MAC)//c220909_1
        if(p_path.isEmpty()){
            print_debug();
            global.window_activate_flag = true;//c220907_2
            ToastMsg::show(this,"",tr("The Share Path information is incorrect."), 4000);
            return;
        }
#endif
#if defined(Q_OS_WINDOWS)
#endif
        QString tmp_smbPath = "smb://";
    if(lineEdit_id->text().isEmpty()&&lineEdit_pw->text().isEmpty()){
        tmp_smbPath+=QString("%1/").arg(p_ip);
    }else{
        tmp_smbPath+=QString("%1:%2@%3/").arg(lineEdit_id->text()).arg(lineEdit_pw->text()).arg(p_ip);
    }


    QJsonObject jsonFileInfo;
    jsonFileInfo.insert("ext","");
    jsonFileInfo.insert("id", lineEdit_id->text());
    jsonFileInfo.insert("ip", p_ip);
    jsonFileInfo.insert("isCloud", false);
    jsonFileInfo.insert("isDir",true);
    jsonFileInfo.insert("isFile", false);
    jsonFileInfo.insert("isServer",true);
    jsonFileInfo.insert("isNetworkAdd",false);
    jsonFileInfo.insert("isShare",false);
    jsonFileInfo.insert("isUp",false);
    jsonFileInfo.insert("isThumbNailGetting",false);
    jsonFileInfo.insert("name","//"+p_ip);
    //jsonFileInfo.insert("path",p_path.isEmpty() ? "/" : p_ip+"-"+p_path+"/");//c220913_2
    jsonFileInfo.insert("path",p_path.isEmpty() ? "/" : p_path+"/");//c220913_2
    jsonFileInfo.insert("pw", lineEdit_pw->text());
    jsonFileInfo.insert("rcloneServe","");
    jsonFileInfo.insert("smbParamPath",tmp_smbPath);
    jsonFileInfo.insert("thumbnail","");

    QJsonObject jsonData;
    jsonData.insert("orderstate","DIRECT");
    jsonData.insert("roseToken",global.device.getDeviceRoseToken());
    jsonData.insert("fileInfo",jsonFileInfo);

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    network->request(HTTP_ADD_NETWORK_FOLDER, QString("http://%1:%2/remote_network_share")
                        .arg(global.device.getDeviceIP()).arg(global.port), jsonData, true);

}

void DialogAdd_Network::slot_qprocessError(QProcess::ProcessError error){
    Q_UNUSED(error);
}

/**
 * @brief DialogLogin_Network::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void DialogAdd_Network::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    print_debug();
    qDebug() << "p_jsonObject=" << p_jsonObject;
    if(p_id == HTTP_ADD_NETWORK_FOLDER){
        print_debug();
        setResultOfAddNetworkFolder(p_jsonObject);
    }

    sender()->deleteLater();
}

void DialogAdd_Network::dialog_comfirmNetworkfolder_forFinder()//c220907_2
{
    /*
     * Finder에서 이동(Go) -> 서버의 연결(Connect to Server..)을 눌러 창을 띄우고 (단축키 : Command + K)
서버주소 란에 smb://[주소]를 입력한다.
예를들어 서버 주소가 google.com/smb 라면 다음과 같이 입력하면 된다.
주소 예 : smb://google.com/smb
출처: https://reysion.tistory.com/17 [미로의 노트:티스토리]
*/
    DialogConfirm_MacShare *dlgConfirmFinder = new DialogConfirm_MacShare(this);//c220913_1
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

void DialogAdd_Network::slot_process_smb(QString argument){//c220909_1
    print_debug();
    QProcess *process = new QProcess(this);
    process->setProgram("open");

    QStringList strListProcess;
    strListProcess.append(argument);
    // net.exe 이용 smb 연결 추가
    process->setArguments(strListProcess);
    bool bb = process->startDetached();
    qDebug() << "bb=" << bb;
    process->waitForFinished(-1);
}

/**
 * @brief DialogLogin_Network::setResultOfAddNetworkFolder : 네트워크 폴더 추가 요청 처리
 * @param p_json QJsonObject response
 */
void DialogAdd_Network::setResultOfAddNetworkFolder(const QJsonObject &p_json){//c220909_1

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_message = "message";

    if(p_json.contains(jsonKey_flagOk) && p_json[jsonKey_flagOk].toBool()){
        QJsonObject jsonFileInfo = p_json["fileInfo"].toObject();
#if defined(Q_OS_WIN)
        QString argument = QString("%1").arg(jsonFileInfo["smbParamPath"].toString());//c220907_1
#endif
#if defined(Q_OS_MAC)
        //QString argument = QString("%1%2").arg(jsonFileInfo["smbParamPath"].toString()).arg(jsonFileInfo["path"].toString());//c220907_1
        QString argument = QString("%1").arg(jsonFileInfo["smbParamPath"].toString());//c220907_1
#endif
        global.smbParamPath_cmd = argument;
        if(p_json.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_json[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                print_debug();
                qDebug() << p_json[jsonKey_message].toString().toLower();
                if(p_json.contains(jsonKey_message)&&(p_json[jsonKey_message].toString().toLower()!="success")){
                    print_debug();
#if defined(Q_OS_MAC)
                    //dialog_comfirmNetworkfolder_forFinder();//c220913_2
                    global.window_activate_flag = true;//c220907_2
                    if((p_json[jsonKey_message].toString().toLower() == "network error")){
                        ToastMsg::show(this,"",tr("The IP address information is incorrect."), 4000);
                    }else{
                        ToastMsg::show(this,"",p_json[jsonKey_message].toString(), 4000);
                    }
                    global.window_activate_flag = false;

                    // 실패
                    emit signal_process_smb(argument);//c220909_1


#endif
#if defined(Q_OS_WIN)//c220909_1
                    global.window_activate_flag = true;//c220907_2
                    if((p_json[jsonKey_message].toString().toLower()=="network error")){
                        ToastMsg::show(this,"",tr("The IP address information is incorrect."), 4000);
                    }else{
                        ToastMsg::show(this,"",p_json[jsonKey_message].toString(), 4000);
                    }
                    global.window_activate_flag = false;
#endif
                    qDebug() << "global.smbParamPath_cmd = " << global.smbParamPath_cmd;//c220907_1
                    global.smbParamPath_cmd = "";



                }else{
                    print_debug();
                    // 네트워크 추가 시그널 호출 ==> 폴더 화면 갱신
                    emit linker->signal_networkFolderAdded();//c220909_3
                    global.window_activate_flag = true;//c220907_2
                    ToastMsg::show(this,"",tr("A network folder has been added."));

                    setResult(QDialog::Accepted);
                    this->hide();
                }

            }else if(p_json.contains(jsonKey_message)){
                global.window_activate_flag = true;//c220907_2
                ToastMsg::show(this,"",p_json[jsonKey_message].toString());
            }
        }
    }else{
        global.window_activate_flag = true;//c220907_2
        ToastMsg::show(this,"",tr("Please check the network staus."));
    }
}
