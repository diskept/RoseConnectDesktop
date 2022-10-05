#include "dialogconfirm.h"
#include "common/gscommon.h"

const int DLG_WIDTH = 600;

/**
 * @brief DialogConfirm::DialogConfirm : Confirm Dialog
 * @details 제목, 내용으로 구성
 * @param parent
 * @param p_confirmType
 * @note {@link setAlertMode()} 함수로 Alert 스타일 사용 가능
 *
 */
DialogConfirm::DialogConfirm(QWidget *parent, DlgConfirmType p_confirmType)
    : QDialog(parent)
{
    this->confirmType = p_confirmType;

    setInit();
    setUIControl();
}

/**
 * @brief DialogConfirm::setInit : 초기 세팅
 */
void DialogConfirm::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedWidth(DLG_WIDTH);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief DialogConfirm::setUIControl : UI 세팅
 */
void DialogConfirm::setUIControl(){
    lb_title = new QLabel;
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;background-color:transparent;");
    lb_title->setFixedHeight(70);

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png");
    btn_close->setParent(this);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setCursor(Qt::PointingHandCursor);

    /*
    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title);
    */
    QGridLayout *box_top = new QGridLayout();
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title, 0, 0);
    if(this->confirmType == DlgConfirmType::OKCancel){
    box_top->addWidget(btn_close, 0, 0, Qt::AlignRight);
    }

    QWidget *wg_top = new QWidget;
    wg_top->setObjectName("wg_top");
    wg_top->setLayout(box_top);
    wg_top->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #404040;}");

    lb_text = new QLabel;
    lb_text->setAlignment(Qt::AlignCenter);
    lb_text->setStyleSheet("color:#FFFFFF;font-size:20px;background-color:transparent; line-height:24px;");
    lb_text->setContentsMargins(30,14,30,39);
    lb_text->setWordWrap(true);
    //lb_text->setFixedHeight(100);//c220511


    // 확인 버튼
    QPushButton *btn_save = new QPushButton;
    btn_save->setText(tr("Confirm"));
    //btn_save->setText(tr("확인"));
    btn_save->setObjectName("btn_save");
    btn_save->setFixedSize(140, 40);
    btn_save->setStyleSheet("#btn_save{font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px;} #btn_save:hover{font-size:18px; color:#FFFFFF; background-color:#7D6144; border-radius:20px;}");
    btn_save->setCursor(Qt::PointingHandCursor);

    // 취소 버튼
    btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));
    //btn_cancel->setText(tr("취소"));
    btn_cancel->setObjectName("btn_cancel");
    btn_cancel->setFixedSize(140,40);
    btn_cancel->setStyleSheet("#btn_cancel{font-size:18px; color:#FFFFFF; background-color:#777777; border-radius:20px;} #btn_cancel:hover{font-size:18px; color:#FFFFFF; background-color:#4D4D4D; border-radius:20px;}");
    btn_cancel->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_btn = new QHBoxLayout;
    box_btn->setSpacing(30);
    box_btn->setContentsMargins(0,0,0,0);
    box_btn->addStretch(1);
    box_btn->addWidget(btn_save);
    if(this->confirmType == DlgConfirmType::OKCancel){
    box_btn->addWidget(btn_cancel);
    }
    box_btn->addStretch(1);

    wg_body = new QWidget;
    wg_body->setObjectName("wg_body");
    wg_body->setStyleSheet("#wg_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");


    QVBoxLayout *box_body = new QVBoxLayout;//c220709
    box_body->setSpacing(0);
    box_body->setContentsMargins(0,0,0,0);
    box_body->setAlignment(Qt::AlignTop);
    box_body->addWidget(lb_text);
    box_body->addWidget(wg_body);
    //wg_body->hide();



    QVBoxLayout *box_contents = new QVBoxLayout;
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(25,25,25,30);
    box_contents->setAlignment(Qt::AlignTop);
    box_contents->addLayout(box_body);
    box_contents->addLayout(box_btn);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(0,0,0,0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(wg_top);
    box_total->addLayout(box_contents);

    wg_total = new QWidget;
    wg_total->setObjectName("wg_total");
    wg_total->setLayout(box_total);
    wg_total->setStyleSheet("#wg_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");

    QVBoxLayout *box_wrap = new QVBoxLayout;
    box_wrap->setContentsMargins(0,0,0,0);
    box_wrap->setSpacing(0);
    box_wrap->setAlignment(Qt::AlignTop);
    box_wrap->addWidget(wg_total);

    setLayout(box_wrap);

    // 커넥션
    connect(btn_save, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

/**
 * @brief DialogConfirm::setTitle : 타이틀 세팅 함수
 * @param p_title
 */
void DialogConfirm::setTitle(const QString &p_title){
    lb_title->setText(p_title);
}

/**
 * @brief DialogConfirm::setText : 텍스트 세팅함수
 * @param p_text
 */
void DialogConfirm::setText(const QString &p_text){

    wg_body->hide();
    lb_text->setText(p_text);
    lb_text->show();

}
void DialogConfirm::setLayoutMy(QLayout *a){//c220709
    lb_text->hide();
    wg_body->setLayout(a);
    wg_body->show();

}
void DialogConfirm::setLayoutMy_withText(QLayout *a){//c220804
    //lb_text->hide();
    wg_body->setLayout(a);
    wg_body->show();

}

void DialogConfirm::setTextHeight(const int &h){//c220511
    lb_text->setFixedHeight(h);
}
void DialogConfirm::setAlignment(Qt::Alignment a){//c220511
    lb_text->setAlignment(a);
}


void DialogConfirm::yellow_setStyleSheet(){//cheon211203
    wg_total->setStyleSheet("#wg_total { background-color:rgb(227,184,138);border-radius:4px;border:1px solid #4D4D4D; }");//#333333

}
/**
 * @brief DialogConfirm::setAlertMode : Alert Mode 변환
 */
void DialogConfirm::setAlertMode(){
    btn_cancel->hide();
}
