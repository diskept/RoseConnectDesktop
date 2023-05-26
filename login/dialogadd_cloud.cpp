#include "dialogadd_cloud.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include <QLabel>
#include <QIntValidator>
#include <QStyle>
#include <QJsonObject>

using namespace Dialog;



/**
 * @brief DialogAdd_Cloud::DialogAdd_Cloud : 음악-폴더-클라우드-클라우드 추가 다이얼로그
 * @details FTP, WEBDAV 추가<br>
 * @param p_dlgType
 * @param parent
 */
DialogAdd_Cloud::DialogAdd_Cloud(Dialog::DialogType p_dlgType, QWidget *parent)
    : QDialog(parent)
    , dlgType(p_dlgType)
{
    setInit();
    setUIControl();
}

/**
 * @brief DialogAdd_Cloud::setInit : 초기 세팅
 */
void DialogAdd_Cloud::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief DialogAdd_Cloud::setUIControl : UI 세팅
 */
void DialogAdd_Cloud::setUIControl(){
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(dlgType==DialogType::ADD_FTP ? "FTP" : "Webdav");
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(70);

    // X 버튼
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

    QString tmp_styleLineEdit = "QLineEdit{font-size:20px;color:#FFFFFF;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;}";
    tmp_styleLineEdit += " QLineEdit[text=\"\"]{color:#777777;}";

    // 클라우드 명 입력
    edit_name = new QLineEdit;
    edit_name->setMaxLength(30);
    edit_name->setPlaceholderText("Remote name");
    edit_name->setFixedHeight(50);
    edit_name->setTextMargins(23,10,23,10);
    edit_name->setStyleSheet(tmp_styleLineEdit);

    // 호스트 입력
    edit_host = new QLineEdit;
    edit_host->setMaxLength(50);
    edit_host->setPlaceholderText(dlgType==DialogType::ADD_FTP ? "FTP host to connect to" : "");
    if(dlgType==DialogType::ADD_WEBDAV){
        edit_host->setText("http://");
    }
    edit_host->setFixedSize(410,50);
    edit_host->setTextMargins(23,10,23,10);
    edit_host->setStyleSheet(tmp_styleLineEdit);

    // 포트 입력
    edit_port = new QLineEdit;
    edit_port->setMaxLength(50);
    edit_port->setPlaceholderText("Port");
    edit_port->setText(dlgType==DialogType::ADD_FTP ? "21" : "5005");
    edit_port->setAlignment(Qt::AlignCenter);
    edit_port->setFixedHeight(50);
    edit_port->setTextMargins(23,14,23,14);
    //edit_port->setValidator(new QIntValidator(0,65535,this));  // 0 ~ 65535
    edit_port->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
    //edit_port->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");
    edit_port->setStyleSheet(tmp_styleLineEdit);


    QHBoxLayout *box_hostPort = new QHBoxLayout;
    box_hostPort->setContentsMargins(0,0,0,0);
    box_hostPort->setSpacing(30);
    box_hostPort->addWidget(edit_host);
    box_hostPort->addWidget(edit_port);

    edit_userName = new QLineEdit;
    edit_userName->setMaxLength(50);
    edit_userName->setPlaceholderText(dlgType==DialogType::ADD_FTP ? "FTP Username" : "User name");
    edit_userName->setFixedHeight(50);
    edit_userName->setTextMargins(23,10,23,10);
    edit_userName->setStyleSheet(tmp_styleLineEdit);

    edit_pw = new QLineEdit;
    edit_pw->setMaxLength(30);
    edit_pw->setPlaceholderText(dlgType==DialogType::ADD_FTP ? "FTP Password" : "Password");
    edit_pw->setFixedHeight(50);
    edit_pw->setTextMargins(23,10,23,10);
    edit_pw->setEchoMode(QLineEdit::Password);
    edit_pw->setStyleSheet(tmp_styleLineEdit);

    // 하단 버튼
    QPushButton *btn_ok = new QPushButton;
    btn_ok->setText(tr("Confirm"));//btn_ok->setText(tr("확인"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    btn_ok->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_ok->setFixedSize(140,40);

    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));//btn_cancel->setText(tr("취소"));
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
    vl_editInfo->setSpacing(30);
    vl_editInfo->setAlignment(Qt::AlignTop);
    vl_editInfo->addWidget(edit_name);
    vl_editInfo->addLayout(box_hostPort);
    vl_editInfo->addWidget(edit_userName);
    vl_editInfo->addWidget(edit_pw);
    vl_editInfo->addSpacing(5);
    vl_editInfo->addLayout(hl_btn);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setContentsMargins(0,0,0,0);
    box_total->setSpacing(0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(widget_title);
    box_total->addLayout(vl_editInfo);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(box_total);
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");

    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);


    this->setLayout(vboxlayout);

    // 커넥션
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOk()));

    connect(edit_name, &QLineEdit::textChanged, [=]{ style()->polish(edit_name);});
    connect(edit_host, &QLineEdit::textChanged, [=]{ style()->polish(edit_host);});
    connect(edit_userName, &QLineEdit::textChanged, [=]{ style()->polish(edit_userName);});
    connect(edit_pw, &QLineEdit::textChanged, [=]{ style()->polish(edit_pw);});
    connect(edit_port, &QLineEdit::textChanged, [=]{ style()->polish(edit_port);});
}

/**
 * @brief DialogAdd_Cloud::checkUserInput : 사용자 입력값 검사 함수
 * @note empty 검사만 한다. regex 확정 시 regex 검사 추가 필요.<br>
 * 익명 연결, 사용자 이름, 패스워드 검사는 하지 않는다.
 * @return bool if true, ok
 */
bool DialogAdd_Cloud::checkUserInput(){
    bool flagOk = true;

    if(edit_name->text().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", tr("Please enter the cloud name."));
       // ToastMsg::show(this, "", tr("클라우드 명을 입력해주세요."));
    }
    else if(edit_host->text().isEmpty()){
        flagOk = false;
        if(dlgType==DialogType::ADD_FTP){
            ToastMsg::show(this, "", tr("Please enter the FTP host name."));
            // ToastMsg::show(this, "", tr("FTP 호스트명을 입력해주세요."));
        }else{
            ToastMsg::show(this, "", tr("Please enter the URL."));
            //ToastMsg::show(this, "", tr("URL을 입력해주세요."));
        }
    }
    else if(edit_port->text().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", tr("Please enter the port."));
        //ToastMsg::show(this, "", tr("포트를 입력해주세요."));

    }

    return flagOk;
}

/**
 * @brief DialogAdd_Cloud::slot_clickedOk : [슬롯] 확인 버튼 클릭
 */
void DialogAdd_Cloud::slot_clickedOk(){
    if(checkUserInput()){
        requestAddCloud();
    }
}

/**
 * @brief DialogAdd_Cloud::requestAddCloud : 클라우드 추가 요청
 * @note type 정보, APP로 확인:: FTP(16), Webdav(28)
 */
void DialogAdd_Cloud::requestAddCloud(){

    QJsonObject jsonCloud;
    jsonCloud.insert("port", edit_port->text().trimmed());
    jsonCloud.insert("pw", edit_pw->text().trimmed());
    jsonCloud.insert("remoteName", edit_name->text().trimmed());
    jsonCloud.insert("type", dlgType==DialogType::ADD_FTP ? 16 : 28);
    jsonCloud.insert("url", edit_host->text().trimmed());
    jsonCloud.insert("userName", edit_userName->text().trimmed());

    QJsonObject jsonData;
    jsonData.insert("data", jsonCloud);


    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_ADD_CLOUD, QString("http://%1:%2/set_rclone_info")
                        .arg(global.device.getDeviceIP()).arg(global.port), jsonData, true);
}

/**
 * @brief DialogAdd_Cloud::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void DialogAdd_Cloud::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id == HTTP_NETWORK_ADD_CLOUD){
        setResultOfAddCloud(p_jsonObject);
    }

    sender()->deleteLater();
}


/**
 * @brief DialogAdd_Cloud::setResultOfAddCloud : 클라우드 추가 요청 결과 처리
 * @param p_json
 */
void DialogAdd_Cloud::setResultOfAddCloud(const QJsonObject &p_json){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";

    if(p_json.contains(jsonKey_flagOk) && p_json[jsonKey_flagOk].toBool()){
        if(p_json.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_json[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                setResult(QDialog::Accepted);
                this->hide();
            //}
        }
    }
}
