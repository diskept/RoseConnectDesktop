#include "dialogladd_network_local.h"

#include "widget/toastmsg.h"

#include <common/gscommon.h>
#include <common/global.h>
#include <common/networkhttp.h>
#include <common/networkhttp.h>
#include <widget/toastmsg.h>

#include <QProcess>
#include <QDir>



using namespace Dialog;

/**
 * @brief DialogAdd_Network::DialogAdd_Network : 네트워크 폴더 추가 Dialog
 * @param parent
 */
DialogAdd_Network_local::DialogAdd_Network_local(QWidget *parent) : QDialog(parent)
{
    setInit();
    setUIControl();
}


/**
 * @brief DialogAdd_Network::setInit : 초기 세팅
 */
void DialogAdd_Network_local::setInit(){

    linker = Linker::getInstance();

    // --------------------------------------------------------
    //  Set Dialog property
    // --------------------------------------------------------
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");


}

/**
 * @brief DialogAdd_Network::setUIControl : UI 세팅 함수
 */
void DialogAdd_Network_local::setUIControl(){

    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Local Desktop folder"));
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
    lineEdit_http->setPlaceholderText(tr("Network share location"));
    lineEdit_http->setFixedHeight(50);
    lineEdit_http->setTextMargins(23,10,23,10);
    lineEdit_http->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    QLabel *lb_commentHttp = new QLabel();
    lb_commentHttp->setText("ex) //host/share or smb://host/share");
    lb_commentHttp->setStyleSheet("font-size:16px;color:#B18658;");




    // 하단 버튼
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
    vl_editInfo->addWidget(lineEdit_http);
    vl_editInfo->addSpacing(8);
    vl_editInfo->addWidget(lb_commentHttp);

    vl_editInfo->addSpacing(35);
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
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_ok()));
}

/**
 * @brief DialogAdd_Network::setFolderInfo : 추가할 네트워크 폴더 데이터 세팅
 * @param p_host QString
 * @param p_id QString
 * @param p_pw QString
 */
void DialogAdd_Network_local::setFolderInfo(QString p_host, QString p_id, QString p_pw){
    lineEdit_http->setText(p_host);

}

/**
 * @brief DialogLogin_Network::checkUserInput::사용자 입력값 검사
 * @details 공유폴더주소만 확인, ID, PW는 검사하지 않는다.(익명 로그인)
 * @return bool
 */
bool DialogAdd_Network_local::checkUserInput(){
    bool flagOk = true;

    QStringList httpList = lineEdit_http->text().split("/");

    if(lineEdit_http->text().trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please enter the network folder address.")));
    }
    else if(httpList.size() <3){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please check the network folder address.")));
    }

    return flagOk;
}

/**
 * @brief DialogLogin_Network::slot_ok [SLOT] 확인 버튼 클릭시
 */
void DialogAdd_Network_local::slot_ok(){


    if(checkUserInput()){
        QStringList httpList = lineEdit_http->text().split("/");
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
            #endif
        }
    }

}

/**
 * @brief DialogLogin_Network::requestAddNetworkFolder::네트워크 폴더 등록
 */
void DialogAdd_Network_local::requestAddNetworkFolder(const QString &p_ip, const QString &p_path){


    QString tmp_smbPath = "smb://";
    if(lineEdit_id->text().isEmpty()&&lineEdit_pw->text().isEmpty()){
        tmp_smbPath+=QString("%1/").arg(p_ip);
    }else{
        tmp_smbPath+=QString("%1:%2@%3/").arg(lineEdit_id->text()).arg(lineEdit_pw->text()).arg(p_ip);
    }


    QJsonObject jsonFileInfo;
    jsonFileInfo.insert("ext","");
   // jsonFileInfo.insert("id", lineEdit_id->text());
    jsonFileInfo.insert("id", "");
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
    jsonFileInfo.insert("path",p_path.isEmpty() ? "/" : p_path+"/");
   // jsonFileInfo.insert("pw", lineEdit_pw->text());
    jsonFileInfo.insert("pw", "");
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

void DialogAdd_Network_local::slot_qprocessError(QProcess::ProcessError error){
    Q_UNUSED(error);
}

/**
 * @brief DialogLogin_Network::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void DialogAdd_Network_local::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id == HTTP_ADD_NETWORK_FOLDER){
        setResultOfAddNetworkFolder(p_jsonObject);
    }

    sender()->deleteLater();
}

/**
 * @brief DialogLogin_Network::setResultOfAddNetworkFolder : 네트워크 폴더 추가 요청 처리
 * @param p_json QJsonObject response
 */
void DialogAdd_Network_local::setResultOfAddNetworkFolder(const QJsonObject &p_json){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_message = "message";

    if(p_json.contains(jsonKey_flagOk) && p_json[jsonKey_flagOk].toBool()){
        if(p_json.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_json[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

                if(p_json.contains(jsonKey_message)&&p_json[jsonKey_message].toString().toLower()!="success"){
                    // 실패
                    ToastMsg::show(this,"",p_json[jsonKey_message].toString());
                }else{
                    // 네트워크 추가 시그널 호출 ==> 폴더 화면 갱신
                    emit linker->signal_networkFolderAdded();

                    ToastMsg::show(this,"",tr("A network folder has been added."));

                    setResult(QDialog::Accepted);
                    this->hide();
                }
            /*}else if(p_json.contains(jsonKey_message)){
                ToastMsg::show(this,"",p_json[jsonKey_message].toString());
            }*/
        }
    }else{
        ToastMsg::show(this,"",tr("Please check the network staus."));
    }
}
