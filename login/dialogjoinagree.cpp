#include "dialogjoin.h"
#include "dialogjoinagree.h"
#include "widget/toastmsg.h"
#include "common/global.h"

#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QRegularExpression>
#include <QTextStream>
#include <qapplication.h>
#include <common/gscommon.h>

using namespace Dialog;

const int DLG_WIDTH = 700;
const int DLG_HEIGHT= 750;

/**
 * @brief DialogJoinAgree::DialogJoinAgree 회원가입 개인정보 동의 다이얼로그 생성자
 * @param parent
 */
DialogJoinAgree::DialogJoinAgree(QWidget *parent) : QDialog(parent)
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
 * @brief DialogJoinAgree::setUIControl UI 생성
 */
void DialogJoinAgree::setUIControl(){

    // 타이틀
    QLabel *lb_title = new QLabel();
    lb_title->setText(tr("Sign Up"));
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


    QLabel *lb_agree_title = new QLabel(tr("[Consent to collection of Personal Information]"));
    lb_agree_title->setContentsMargins(50,37,50,0);
    lb_agree_title->setStyleSheet("font-size:18px;color:#CCCCCC;");


    lb_agree_content = new QTextEdit();
    lb_agree_content->setContentsMargins(50,23,50,15);
    lb_agree_content->setReadOnly(true);
    lb_agree_content->setStyleSheet("font-size:18px;color:#777777;border:none;");

    // 스크롤 가능한 부모 위젯인 QScrollArea를 생성합니다.
    QScrollArea *scrollArea = new QScrollArea();

    // QLabel을 스크롤 가능한 콘텐츠 위젯 안에 배치합니다.
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->addWidget(lb_agree_content);

    // QScrollArea에 스크롤 가능한 콘텐츠 위젯을 설정합니다.
    scrollArea->setWidget(contentWidget);

    // 스크롤 영역의 크기 조정을 자동으로 처리합니다.
    scrollArea->setWidgetResizable(true);

    // 스크롤 영역의 스타일을 설정합니다.

    scrollArea->setStyleSheet("QScrollArea {background-color:#333333;border:none;}"
                              "QScrollBar:vertical {border: none; background-color: transparent; width: 6px; margin: 12px 0px 12px 0px; }"
                              "QScrollBar::handle:vertical {background-color: #b18658; min-height: 60px; border-radius: 3px; }"
                              "QScrollBar::add-line:vertical {height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }"
                              "QScrollBar::sub-line:vertical {height: 0 px; subcontrol-position: top; subcontrol-origin: margin; }"
                              "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none; }");

    QWidget *wg_line = new QWidget;
    wg_line->setFixedHeight(1);
    wg_line->setStyleSheet("background-color:#444444;");

    this->checkbox_agree = new QCheckBox();
    this->checkbox_agree->setText(tr("Consent to collection of Personal Information"));
    this->checkbox_agree->setStyleSheet("font-size:20px;color:#B18658;");
    this->checkbox_agree->setCursor(Qt::PointingHandCursor);

    btn_join = new QPushButton;
    btn_join->setText(tr("Agree and Sign Up"));
    btn_join->setFixedHeight(55);
    btn_join->setStyleSheet("font-size:20px;color:#CCCCCC;background-color:transparent;border:3px solid #666666;border-radius:27px;margin-left:25px;margin-right:25px;");
    btn_join->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *vl_join = new QVBoxLayout;
    vl_join->setMargin(0);
    vl_join->setSpacing(0);
    vl_join->setContentsMargins(25,20,25,0);
    vl_join->setAlignment(Qt::AlignTop);
    vl_join->addSpacing(15);
    vl_join->addWidget(checkbox_agree, 0, Qt::AlignCenter);
    vl_join->addSpacing(30);
    vl_join->addWidget(btn_join);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setMargin(0);
    vl_total->setContentsMargins(1,1,1,1);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(wg_top);
    vl_total->addWidget(lb_agree_title);
    vl_total->addWidget(scrollArea);
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
    connect(btn_join, SIGNAL(clicked()), this, SLOT(slot_clickedJoin()));
    connect(checkbox_agree, &QCheckBox::stateChanged, this, &DialogJoinAgree::slot_clickedCheckBox);
}

/**
 * @brief DialogJoinAgree::showEvent [override] 개인정보 동의서 txt 파일 읽어서 세팅한다.
 */
void DialogJoinAgree::showEvent(QShowEvent *){

    //QString filename = QApplication::applicationDirPath() + "/data/agree.txt";

    QString filename;
    if(global.lang== 0){
        filename = ":/join_agree_en.txt";
    }else{
        filename = ":/join_agree_kr.txt";
    }
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");           // for Korean
    QString myText = in.readAll();
    this->lb_agree_content->setText(myText);
    file.close();
};

/**
 * @brief DialogJoinAgree::slot_clickedCheckBox
 * @param state
 */
void DialogJoinAgree::slot_clickedCheckBox(int state){

    if(state==Qt::Checked){
        btn_join->setText(tr("next"));
        btn_join->setStyleSheet("font-size:20px;color:#FFFFFF;background-color:#B18658;border:0px;border-radius:27px;margin-left:25px;margin-right:25px;");

    }else{
        btn_join->setText(tr("Agree and Sign Up"));
        btn_join->setStyleSheet("font-size:20px;color:#CCCCCC;background-color:transparent;border:3px solid #666666;border-radius:27px;margin-left:25px;margin-right:25px;");
    }
}

/**
 * @brief DialogJoinAgree::slot_clickedJoin [SLOT] 동의하고 회원가입하기 버튼 클릭시
 */
void DialogJoinAgree::slot_clickedJoin(){

    if(this->checkbox_agree->isChecked()==false){
        ToastMsg::show(this, "", "Please consent to collection of Person Information.");
    }else{
        this->close();

        Dialog::DialogJoin dlg;
        dlg.exec();
    }
}
