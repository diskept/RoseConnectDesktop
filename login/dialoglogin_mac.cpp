#include "dialoglogin_mac.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "common/global.h"//c220913_1

#include <QLabel>
#include <QProcess>
#include <QDebug>

using namespace Dialog;

/**
 * @brief DialogLogin_mac::DialogLogin_mac
 * @param p_ip QString 아이피
 * @param hostName QString 호스트명
 * @param parent
 */
DialogLogin_mac::DialogLogin_mac(const QString &p_ip, const QString &hostName,QWidget *parent) : QDialog(parent)//c220913_2
  ,ip(p_ip)
  ,hostName(hostName)
{
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");

    setUIControl();
}

void DialogLogin_mac::setUIControl(){
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Desktop Mac Log-in"));
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(70);

    QLabel *lb_hostName = new QLabel();
    lb_hostName->setText(hostName);
    lb_hostName->setStyleSheet("font-size:16px;color:#B18658;");
    lb_hostName->setAlignment(Qt::AlignHCenter);

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


    // 아이디
    lineEdit_id = new QLineEdit;
    lineEdit_id->setMaxLength(30);
    lineEdit_id->setPlaceholderText(tr("User name"));
    lineEdit_id->setFixedHeight(55);
    lineEdit_id->setTextMargins(23,14,23,14);
    lineEdit_id->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    // 패스워드
    lineEdit_pw = new QLineEdit;
    lineEdit_pw->setMaxLength(30);
    lineEdit_pw->setPlaceholderText(tr("Password"));
    lineEdit_pw->setFixedHeight(50);
    lineEdit_pw->setTextMargins(23,14,23,14);
    lineEdit_pw->setEchoMode(QLineEdit::Password);
    lineEdit_pw->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");


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
    vl_editInfo->addWidget(lb_hostName);
    vl_editInfo->addSpacing(20);
    vl_editInfo->addWidget(lineEdit_id);
    vl_editInfo->addSpacing(30);
    vl_editInfo->addWidget(lineEdit_pw);
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

    QProcess *process1 = new QProcess();
    process1->start(QString("sh -c \"whoami\""));//c220913_2
    process1->waitForFinished();
    QString whoami = process1->readAllStandardOutput();
    qDebug() << "whoami=" << whoami;
    lineEdit_id->setText(whoami.split("\n").first());

}

/**
 * @brief DialogLogin_mac::checkUserInput:사용자 입력값 검사
 * @details hostName만 확인, ID, PW는 검사하지 않는다.(익명 로그인)
 * @return bool
 */
bool DialogLogin_mac::checkUserInput(){
    bool flagOk = true;

    if(ip.trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("The network address could not be verified.")));
    }
    else if(hostName.trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("The network address could not be verified.")));
    }

    return flagOk;
}

/**
 * @brief DialogLogin_mac::slot_ok:[슬롯] 확인 버튼 클릭
 * @details 해당 주소에 대하여 로그인 시도
 */
void DialogLogin_mac::slot_ok(){
    //QString tem = QString("fddf\"");
    print_debug();
    //qDebug() << tem.toUtf8();
#if defined(Q_OS_WIN)//maccheon
    if(checkUserInput()){


        setResult(QDialog::Accepted);
        this->hide();
    }
#elif defined(Q_OS_MAC)//c220913_2
    if(checkUserInput()){
        print_debug();
        QProcess* process = new QProcess();
        process->start(QString("sh -c \"echo %1 | sudo -S ls /\"").arg(lineEdit_pw->text()));//c220913_1
        process->waitForFinished();
        QString std_out = process->readAllStandardOutput();
        if(std_out.isEmpty()){
            qDebug() << "The password input is incorrect.";
            global.window_activate_flag = true;//
            ToastMsg::show(this,"",tr("The password input is incorrect."), 4000);
            return;
        }

        global.userPassword = lineEdit_pw->text();
        setResult(QDialog::Accepted);
        this->hide();
    }
#endif
}
