#include "login/dialoglogin.h"
#include "login/dialogjoinagree.h"

#include "common/global.h"
#include "common/rosesettings.h"
#include "common/gscommon.h"
#include "common/linker.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"

#include "data/datauser.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QSettings>


using namespace Dialog;

/**
 * @brief DialogLogin::DialogLogin : 로그인 다이얼로그 .cpp
 * @details 사용자 로그인 다이얼로그
 * @param parent
 */
DialogLogin::DialogLogin(QWidget *parent) : QDialog(parent)
{
    setInit();
    setUIControl();
}


void DialogLogin::setInit(){

    linker = Linker::getInstance();

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}


void DialogLogin::setUIControl(){

    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Log-in"));
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;background-color:transparent;padding-left:70px;");

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setParent(this);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setMargin(0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title,1);
    box_top->addWidget(btn_close);

    QWidget *wg_top = new QWidget;
    wg_top->setObjectName("wg_top");
    wg_top->setLayout(box_top);
    wg_top->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #404040;}");


    // 아이디
    le_id = new QLineEdit;
    le_id->setMaxLength(30);
    le_id->setPlaceholderText(tr("ID"));
    le_id->setFixedHeight(50);
    le_id->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");

    // 패스워드
    le_pw = new QLineEdit;
    le_pw->setMaxLength(30);
    le_pw->setPlaceholderText(tr("password"));
    le_pw->setFixedHeight(50);
    le_pw->setEchoMode(QLineEdit::Password);
    le_pw->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");

    QLabel *lb_autoLogin = new QLabel;
    lb_autoLogin->setText(tr("Stay signed in"));
    lb_autoLogin->setStyleSheet("color:#CCCCCC;font-size:18px;");

    // 자동 로그인
    onOff = new OnOffWidget;
    onOff->setValue(true);
    onOff->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_autoLogin = new QHBoxLayout;
    box_autoLogin->setMargin(0);
    box_autoLogin->setSpacing(20);
    box_autoLogin->addStretch(1);
    box_autoLogin->addWidget(lb_autoLogin,0, Qt::AlignVCenter);
    box_autoLogin->addWidget(onOff,0, Qt::AlignVCenter);

    // 로그인 버튼
    QPushButton *btn_login = new QPushButton;
    btn_login->setText(tr("Log-in"));
    btn_login->setFixedHeight(55);
    btn_login->setStyleSheet("font-size:20px;color:#FFFFFF;background-color:#B18658;border-radius:27px;margin-left:25px;margin-right:25px;");
    btn_login->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *vl_login = new QVBoxLayout;
    vl_login->setSpacing(0);
    vl_login->setContentsMargins(25,45,25,0);
    vl_login->setAlignment(Qt::AlignTop);
    vl_login->addWidget(le_id);
    vl_login->addSpacing(30);
    vl_login->addWidget(le_pw);
    vl_login->addSpacing(17);
    vl_login->addLayout(box_autoLogin);
    vl_login->addSpacing(60);
    vl_login->addWidget(btn_login);

    QWidget *wg_line = new QWidget;
    wg_line->setFixedHeight(1);
    wg_line->setStyleSheet("background-color:#444444;");

    QPushButton *btn_join = new QPushButton;
    btn_join->setText(tr("Sign Up"));
    btn_join->setFixedHeight(55);
    btn_join->setStyleSheet("font-size:20px;color:#CCCCCC;background-color:transparent;border:3px solid #666666;border-radius:27px;margin-left:25px;margin-right:25px;");
    btn_join->setCursor(Qt::PointingHandCursor);

    QPushButton *btn_email = new QPushButton;
    btn_email->setText(tr("Find Email"));
    btn_email->setStyleSheet("font-size:20px;color:#CCCCCC;");
    btn_email->setCursor(Qt::PointingHandCursor);

    QLabel *lb_split = new QLabel;
    lb_split->setText("|");
    lb_split->setStyleSheet("font-size:20px;color:#CCCCCC;");

    QPushButton *btn_findPW = new QPushButton;
    btn_findPW->setText(tr("Find Forgot your password"));
    btn_findPW->setStyleSheet("font-size:20px;color:#CCCCCC;");
    btn_findPW->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_find = new QHBoxLayout;
    box_find->setSpacing(10);
    box_find->setAlignment(Qt::AlignHCenter);
    box_find->addWidget(btn_email);
    box_find->addWidget(lb_split);
    box_find->addWidget(btn_findPW);

    QVBoxLayout *vl_join = new QVBoxLayout;
    vl_join->setMargin(0);
    vl_join->setSpacing(0);
    vl_join->setContentsMargins(25,20,25,0);
    vl_join->setAlignment(Qt::AlignTop);
    vl_join->addWidget(btn_join);
    vl_join->addSpacing(53);
    //vl_join->addLayout(box_find);


    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setMargin(0);
    vl_total->setContentsMargins(1,1,1,1);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(wg_top);
    vl_total->addLayout(vl_login);
    vl_total->addSpacing(20);
    vl_total->addWidget(wg_line);
    vl_total->addLayout(vl_join);


    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(vl_total);
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");
    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setMargin(0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);

    this->setLayout(vboxlayout);


    // 커넥션
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_login, SIGNAL(clicked()), this, SLOT(slot_clickedLogin()));
    connect(btn_join, SIGNAL(clicked()), this, SLOT(slot_clickedJoin()));
    connect(btn_email, SIGNAL(clicked()), this, SLOT(slot_clickedFindId()));
    connect(btn_findPW, SIGNAL(clicked()), this, SLOT(slot_clickedFindId()));

    le_id->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
}


/**
 * @brief DialogLogin::checkUserInput : 사용자 입력값 검사
 * @details ID, PW 유효성 체크
 * [미완성] ID, PW 정규식 요청필요
 * @return
 */
bool DialogLogin::checkUserInput(){

    bool flagOk = true;

    if(le_id->text().trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "",QString(tr("Please enter your ID.")));
    }
    else if(le_pw->text().trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "",QString(tr("Please enter a password.")));
    }

    return flagOk;
}


/**
 * @brief DialogLogin::setResultOfLogin : 로그인 요청결과 처리 함수
 * @param p_jsonData QJsonObject API response
 */
void DialogLogin::setResultOfLogin(const QJsonObject &p_jsonData){

    //QJsonDocument doc(p_jsonData);
    //QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug() << "ROSE USER LOGIN RCV : " << strJson;

    bool flagLogined = false;
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";
    const QString jsonKey_msg = "msg";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool(false)){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){

            flagLogined = true;

            global.user.setDataUser(p_jsonData);

            saveLoginInfo();

            this->requestRoseLogin();

            emit linker->signal_logined();

            ToastMsg::show(this, "", QString(tr("You are logged in.")));
            this->close();

        }
        else if(p_jsonData[jsonKey_message].toString().toLower() == "정상"){

            flagLogined = true;

            global.user.setDataUser(p_jsonData);

            saveLoginInfo();

            this->requestRoseLogin();

            emit linker->signal_logined();

            ToastMsg::show(this, "", QString(tr("You are logged in.")));
            this->close();

        }
    }

    // ERROR 메시지 출력
    if(!flagLogined){
        QString errMsg(tr("You can't logged in."));

        if(p_jsonData.contains(jsonKey_message)){
            errMsg = p_jsonData[jsonKey_message].toString("");
        }

        if(errMsg.isEmpty() && p_jsonData.contains(jsonKey_msg)){
            errMsg = p_jsonData[jsonKey_msg].toString("");
        }

        ToastMsg::show(this, "", errMsg);
    }
}


/**
 * @brief DialogLogin::requestRoseLogin 로그인 성공시에,, 로그 기기에게도 로그인 처리 요청해야한다.
 */
void DialogLogin::requestRoseLogin(){

    // 로즈기기에 사용자 정보 로그인 처리
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    QJsonObject json;
    json.insert("accesskey", global.user.getAccess_token());
    json.insert("email", global.user.getEmail());
    json.insert("userName", global.user.getUsername());
    network->request(HTTP_SET_ROSE_USER_INFO, QString("http://%1:%2/set_rose_user_info")
                 .arg(global.device.getDeviceIP()).arg(global.port), json, true, true);
}


/**
 * @brief DialogLogin::saveLoginInfo : 로그인 정보 저장
 * @details
 */
void DialogLogin::saveLoginInfo(){

     QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

     SqliteHelper *sqliteHelper = new SqliteHelper(this);
     QSqlError err = sqliteHelper->addConnectionLocal();

    if(onOff->getCurrentValue()){

        settings->setValue(rosesettings.SETTINGS_AUTO_LOGIN, true);

        if(err.type() == QSqlError::NoError){

            QString tmp_id = le_id->text().trimmed();
            QString tmp_pw = QString("%1").arg(QString(QCryptographicHash::hash(le_pw->text().trimmed().toUtf8(),QCryptographicHash::Sha1).toHex()));
            QString tmp_txt = "";

            QString strQuery_create = "CREATE TABLE IF NOT EXISTS User (";
            strQuery_create += "email VARCHAR(50), pw TEXT, user_name TEXT, token_type TEXT, access_token TEXT, refresh_token TEXT)";
            sqliteHelper->exec(strQuery_create);

            QString strQuery_delete = "DELETE FROM User";
            sqliteHelper->exec(strQuery_delete);

            QString strQuery_insert = "INSERT INTO User (email, pw, user_name, token_type, access_token, refresh_token) ";
            strQuery_insert += " VALUES ";
            strQuery_insert += " ('" + tmp_id + "','" + tmp_pw + "','" + global.user.getUsername() + "','" + global.user.getToken_type() + "','" + global.user.getAccess_token() + "','" + global.user.getRefresh_token() + "'";
            strQuery_insert += ") ";
            sqliteHelper->exec(strQuery_insert);
        }
    }else{
        settings->setValue(rosesettings.SETTINGS_AUTO_LOGIN, false);

        if(err.type() == QSqlError::NoError){

            QString tmp_id = le_id->text().trimmed();
            QString tmp_pw = QString("%1").arg(QString(QCryptographicHash::hash(le_pw->text().trimmed().toUtf8(),QCryptographicHash::Sha1).toHex()));

            QString strQuery_create = "CREATE TABLE IF NOT EXISTS User (";
            strQuery_create += "email VARCHAR(50), pw TEXT, user_name TEXT, token_type TEXT, access_token TEXT, refresh_token TEXT)";

            sqliteHelper->exec(strQuery_create);

            QString strQuery_delete = "DELETE FROM User";
            sqliteHelper->exec(strQuery_delete);
        }


    }

    settings->deleteLater();

    sqliteHelper->close();
    delete sqliteHelper;
}


/**
 * @brief DialogLogin::slot_clickedLogin : [슬롯]-로그인 버튼 클릭
 */
void DialogLogin::slot_clickedLogin(){

    if(checkUserInput()){
        QString tmp_id = le_id->text().trimmed();
        QString tmp_pw = QString("%1").arg(QString(QCryptographicHash::hash(le_pw->text().trimmed().toUtf8(),QCryptographicHash::Sha1).toHex()));

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonData;
        jsonData.insert("email", tmp_id);
        jsonData.insert("password", tmp_pw);

        // roseaudio api change      22/02/2021    fixed jeon
        network->request(HTTP_LOGIN
                         , QString("%1/member/member/login").arg(global.legacy_v1)
                         , jsonData
                         , true);

        /*QJsonObject jsonData;
        jsonData.insert("user_name", tmp_id);
        jsonData.insert("password", tmp_pw);
        network->request(HTTP_LOGIN, QString("%1%2")
                         .arg(global.legacy_v3)
                         .arg("/auth/login"), jsonData, true);*/

    }
}


void DialogLogin::slot_responseHttp(int p_id, QJsonObject p_jsonData){

    if(p_id == HTTP_LOGIN){
        setResultOfLogin(p_jsonData);
    }else if(p_id == HTTP_SET_ROSE_USER_INFO){
        // Nothing..
    }

    sender()->deleteLater();
}


/**
 * @brief DialogLogin::slot_clickedJoin [SLOT] 회원가입 버튼 클릭시
 */
void DialogLogin::slot_clickedJoin(){

    this->close();

    Dialog::DialogJoinAgree dlg;
    dlg.exec();
}


/**
 * @brief DialogLogin::slot_clickedFindId [SLOT] 아이디 찾기 버튼 클릭시
 */
void DialogLogin::slot_clickedFindId(){

    ToastMsg::show(this, "", tr("[B-03] Scenario change scheduled"));
}
