#include "dialoglogin_network.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include "common/global.h"//c220913_1

#include <QLabel>
#include <QProcess>
#include <QDebug>

using namespace Dialog;

/**
 * @brief DialogLogin_network::DialogLogin_network
 * @param p_ip QString 아이피
 * @param hostName QString 호스트명
 * @param parent
 */
DialogLogin_network::DialogLogin_network(const QString &p_ip, const QString &hostName,QWidget *parent) : QDialog(parent)
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

void DialogLogin_network::setUIControl(){
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Network Log-in"));
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
    lineEdit_id->setPlaceholderText(tr("ID"));
    lineEdit_id->setFixedHeight(50);
    lineEdit_id->setTextMargins(23,10,23,10);
    lineEdit_id->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    // 패스워드
    lineEdit_pw = new QLineEdit;
    lineEdit_pw->setMaxLength(30);
    lineEdit_pw->setPlaceholderText(tr("Password"));
    lineEdit_pw->setFixedHeight(50);
    lineEdit_pw->setTextMargins(23,10,23,10);
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
}

/**
 * @brief DialogLogin_network::checkUserInput:사용자 입력값 검사
 * @details hostName만 확인, ID, PW는 검사하지 않는다.(익명 로그인)
 * @return bool
 */
bool DialogLogin_network::checkUserInput(){
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
 * @brief DialogLogin_network::slot_ok:[슬롯] 확인 버튼 클릭
 * @details 해당 주소에 대하여 로그인 시도
 */
void DialogLogin_network::slot_ok(){//c220913_2
    //QString tem = QString("fddf\"");
    print_debug();
    //qDebug() << tem.toUtf8();
    #if defined(Q_OS_WIN)//maccheon
    if(checkUserInput()){
        // net.exe 이용 smb 연결 삭제
        QProcess *process = new QProcess(this);
        process->setProgram("net.exe");
        process->setArguments(QStringList()<<"use"<< "/delete"<< "\\\\"+ip+"\\");
        process->startDetached();
        process->waitForFinished(-1);

        QStringList strListProcess;
        strListProcess.append("use");
        strListProcess.append("\\\\"+ip+"");
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

        setResult(QDialog::Accepted);
        this->hide();
    }
#elif defined(Q_OS_MAC)//c220908_2
    if(checkUserInput()){
        print_debug();


        //c220913_1  start
        global.standardOutput.clear();
        global.standardOutput.append(ip);
        global.standardOutput.append(lineEdit_id->text());
        global.standardOutput.append(lineEdit_pw->text());
        QProcess* process1 = new QProcess();
        QStringList sArgs;
        //QString cmd = QString("sh -c \"smbutil view //%1:%2@%3/ | awk '$2==\"Disk\"{print $1}'\"").arg(lineEdit_id->text()).arg(lineEdit_pw->text()).arg(ip);
        QString cmd = QString("sh -c \"smbutil view //%1:%2@%3/\"").arg(lineEdit_id->text()).arg(lineEdit_pw->text()).arg(ip);
        //QString cmd = QString::fromLocal8Bit("smbutil view //%1:%2@%3/ | awk '$2==\"Disk\"{print $1}'").arg(lineEdit_id->text()).arg(lineEdit_pw->text()).arg(ip);
        //QString cmd = "smbutil view //" + lineEdit_id->text() + ":" + lineEdit_pw->text() + "@" +  ip + "/ | awk '$2==" +  "\"Disk\"{print $1}" + "'";
        qDebug() << "cmd=" << cmd.toUtf8();
        process1->start(cmd);//c220913_1
        process1->waitForFinished();
        QString standardOutput = process1->readAllStandardOutput();
        QStringList strList = standardOutput.split("\n");
        for(int i = 0; i < strList.size(); i++){
            if(strList.at(i).contains("Disk")){
                qDebug() << strList.at(i).split(" ").first();
                global.standardOutput.append(strList.at(i).split(" ").first());
            }
        }
        qDebug() << "global.standardOutput=" << global.standardOutput;
        qDebug() << process1->readAllStandardError();
        //c220913_1 end


        setResult(QDialog::Accepted);
        this->hide();
    }
#endif
}
