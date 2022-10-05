#include "login/dialogjoin.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/toastmsg.h"
#include "widget/dialogconfirm.h"

#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QTextStream>
#include <QApplication>


using namespace Dialog;

const int DLG_WIDTH = 700;
const int DLG_HEIGHT= 560;

const int HTTP_CHECK_DUPLICATE_NICKNAME = 99;
const int HTTP_JOIN_MEMBER = 98;


/**
 * @brief DialogJoin::DialogJoin 회원가입 다이얼로그 생성자
 * @param parent
 */
DialogJoin::DialogJoin(QWidget *parent) : QDialog(parent)
{
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    //this->setFixedWidth(DLG_WIDTH);
    this->setStyleSheet("background-color:transparent;");

    setUIControl();
}


/**
 * @brief DialogJoin::setUIControl UI 생성
 */
void DialogJoin::setUIControl(){

    // 타이틀
    QLabel *lb_title = new QLabel();
    lb_title->setText(tr("Sign Up"));//lb_title->setText(tr("회원가입"));
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;background-color:transparent;padding-left:70px;");

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setParent(this);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_top = new QHBoxLayout();
    box_top->setMargin(0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title,1);
    box_top->addWidget(btn_close);

    QWidget *wg_top = new QWidget();
    wg_top->setObjectName("wg_top");
    wg_top->setLayout(box_top);
    wg_top->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #404040;}");

    le_email = new QLineEdit;
    le_email->setMaxLength(50);
    le_email->setPlaceholderText(tr("e-mail"));//le_email->setPlaceholderText(tr("이메일"));
    le_email->setFixedHeight(50);
    le_email->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");
    QLabel *lb_email_comment = new QLabel(tr("A verification email will be sent to the email you entered."));
    //QLabel *lb_email_comment = new QLabel(tr("입력하신 이메일로 인증 메일이 전송됩니다."));
    lb_email_comment->setContentsMargins(25,0,0,0);
    lb_email_comment->setStyleSheet("font-size:18px;color:#CCCCCC;");
    le_nickName = new QLineEdit;
    le_nickName->setMaxLength(50);
    le_nickName->setPlaceholderText(tr("nickname"));//le_nickName->setPlaceholderText(tr("닉네임"));
    le_nickName->setFixedHeight(50);
    le_nickName->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");
    QPushButton *btn_duplicateNickName = new QPushButton(tr("Duplicate check"));
    //QPushButton *btn_duplicateNickName = new QPushButton(tr("중복체크"));
    btn_duplicateNickName->setStyleSheet("color:#B18658;font-size:18px;padding-right:20px;");
    btn_duplicateNickName->setCursor(Qt::PointingHandCursor);
    le_pw = new QLineEdit;
    le_pw->setMaxLength(30);
    le_pw->setPlaceholderText(tr("password"));//le_pw->setPlaceholderText(tr("비밀번호"));
    le_pw->setFixedHeight(50);
    le_pw->setEchoMode(QLineEdit::Password);
    le_pw->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");
    QGridLayout *gl_myInfo = new QGridLayout();
    gl_myInfo->setContentsMargins(0,0,0,0);
    gl_myInfo->setSpacing(0);
    gl_myInfo->addWidget(le_nickName, 0, 0);
    gl_myInfo->addWidget(btn_duplicateNickName, 0, 0, Qt::AlignRight);

    QVBoxLayout *vl_login = new QVBoxLayout;
    vl_login->setSpacing(0);
    vl_login->setContentsMargins(25,45,25,0);
    vl_login->setAlignment(Qt::AlignTop);
    vl_login->addWidget(le_email);
    vl_login->addSpacing(10);
    vl_login->addWidget(lb_email_comment);
    vl_login->addSpacing(46);
    vl_login->addLayout(gl_myInfo);
    vl_login->addSpacing(30);
    vl_login->addWidget(le_pw);

    QPushButton *btn_join = new QPushButton;
    btn_join->setText(tr("Sign Up"));//btn_join->setText(tr("가입하기"));
    btn_join->setFixedHeight(55);
    btn_join->setStyleSheet("font-size:20px;color:#FFFFFF;background-color:#B18658;border-radius:27px;margin-left:25px;margin-right:25px;");
    btn_join->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setMargin(0);
    vl_total->setContentsMargins(1,1,1,1);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(wg_top);
    vl_total->addLayout(vl_login);
    vl_total->addSpacing(45);
    vl_total->addWidget(btn_join);

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
    connect(btn_join, SIGNAL(clicked()), this, SLOT(slot_clickedJoin()));
    connect(btn_duplicateNickName, &QPushButton::clicked, this, &DialogJoin::slot_duplicateNickName);
}


/**
 * @brief DialogJoin::slot_responseHttp Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void DialogJoin::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_CHECK_DUPLICATE_NICKNAME){
        this->setResponseNickNameCheck(p_jsonObject);
    }else if(p_id == HTTP_JOIN_MEMBER){
        this->setResponseJoinMember(p_jsonObject);
    }

    sender()->deleteLater();
}


/**
 * @brief DialogJoin::slot_duplicateNickName [SOT] 중복체크 버튼 클릭시
 */
void DialogJoin::slot_duplicateNickName(){

    QString tmp_nickName = this->le_nickName->text();
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QString url = global.legacy_v1 + "/member/member/check/name?name=" + tmp_nickName;
    QJsonObject json;

    network->request(HTTP_CHECK_DUPLICATE_NICKNAME
                     , url
                     , json
                     , false
                     , true);
}


/**
 * @brief DialogJoin::slot_clickedJoin [SLOT] 가입하기 버튼 클릭시
 */
void DialogJoin::slot_clickedJoin(){

    if(this->le_email->text()==""){
        ToastMsg::show(this, "", tr("Please check your email."));
        //ToastMsg::show(this, "", "이메일을 입력해주세요.");
    }else if(this->le_nickName->text()==""){
        ToastMsg::show(this, "", tr("Please check your nickname."));
        // ToastMsg::show(this, "", "닉네임을 입력해주세요.");
    }else if(this->flagDuplicateNickName == false){
        ToastMsg::show(this, "", tr("Please check your nickname Duplication."));
        // ToastMsg::show(this, "", "닉네임 중복체크 해주세요.");
    }else if(this->le_pw->text().length() < 5){
        ToastMsg::show(this, "", tr("Please enter your password at least 5 digits."));
        //ToastMsg::show(this, "", "비밀번호를 5자리 이상 입력해주세요.");
    }else{
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString tmp_pw = QString("%1").arg(QString(QCryptographicHash::hash(le_pw->text().trimmed().toUtf8(),QCryptographicHash::Sha1).toHex()));
        QJsonObject json_member;
        json_member.insert("email", this->le_email->text());
        json_member.insert("name", this->le_nickName->text());
        json_member.insert("password", tmp_pw);
        json_member.insert("shareType", "ALL");

        QJsonObject json;
        json.insert("member", json_member);
        json.insert("roseId", global.device.getDeviceID());
        json.insert("result", false);

        QString url = global.legacy_v1 + "/member/member";

        network->request(HTTP_JOIN_MEMBER
                         , url
                         , json
                         , true
                         , true);
    }

    /*QString tmp_pw = QString("%1").arg(QString(QCryptographicHash::hash(le_pw->text().trimmed().toUtf8(),QCryptographicHash::Sha1).toHex()));
    QJsonObject tmp_json_member;
    tmp_json_member.insert("username", this->le_nickName->text());
    tmp_json_member.insert("email", this->le_email->text());
    tmp_json_member.insert("pw", tmp_pw);
    QJsonObject json;
    json.insert("member", tmp_json_member);
    network->request(HTTP_JOIN_MEMBER, QString("http://beta.api.roseaudio.kr/v1/member")
                     , json, true);*/
}

/**
 * @brief DialogJoin::setResponseNickNameCheck 해당 json으로 nickName 중복체크 체크
 * @param p_jsonObject
 */
void DialogJoin::setResponseNickNameCheck(QJsonObject p_jsonObject){

    this->flagDuplicateNickName = false;

    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
    }

    if(flagOk == false){
        QString message = p_jsonObject["message"].toString();

        if(message == "이름 중복"){
            ToastMsg::show(this, "", tr("This nickanme is already using."));
            //ToastMsg::show(this, "", tr("이미 사용중인 닉네임입니다."));
        }
        else{
            ToastMsg::show(this, "", tr("Please try again.(SMCN)"));
            //ToastMsg::show(this, "", tr("다시 시도해주세요.(SMCN)"));
        }
    }
    else{
        this->flagDuplicateNickName = true;
        ToastMsg::show(this, "", tr("Availiable nickname."));
        //ToastMsg::show(this, "", tr("사용 가능한 닉네임입니다."));
    }
}


/**
 * @brief DialogJoin::setResponseEmailCheck 해당 json으로 nickName 중복체크 체크
 * @param p_jsonObject
 */
void DialogJoin::setResponseJoinMember(QJsonObject p_jsonObject){

    bool flagOk = false;
    if(p_jsonObject.contains("flagOk")){
        flagOk = p_jsonObject["flagOk"].toBool();
    }

    if(flagOk==false){
        QString message = p_jsonObject["message"].toString();

        if(message == "이메일 중복"){
            ToastMsg::show(this, "", tr("This e-mail is already using."));
            //ToastMsg::show(this, "", tr("이미 사용중인 닉네임입니다."));
        }
        else{
            ToastMsg::show(this, "", tr("Please try again.(SMCN)"));
            //ToastMsg::show(this, "", tr("다시 시도해주세요.(SMCN)"));
        }
    }
    else{
        DialogConfirm *dlg = new DialogConfirm(this, DialogConfirm::DlgConfirmType::OK);
        dlg->setTitle(tr("notice"));//dlg->setTitle(tr("알림"));
        dlg->setText(tr("Membership registration is completed by verifying the email you entered and verifying your email."));
        //dlg->setText(tr("입력하신 이메일을 확인하여 이메일 인증을하면 회원가입이 완료됩니다."));
        if(dlg->exec()==QDialog::Accepted){
            this->close();
        }
    }
}
