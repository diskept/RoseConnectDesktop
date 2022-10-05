#include "dialogpoweron.h"
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
DialogPowerOn::DialogPowerOn(QWidget *parent, DlgPowerOnType p_confirmType)
    : QDialog(parent)
{
    this->confirmType = p_confirmType;

    setInit();
    setUIControl();
}

/**
 * @brief DialogConfirm::setInit : 초기 세팅
 */
void DialogPowerOn::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedWidth(DLG_WIDTH);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief DialogConfirm::setUIControl : UI 세팅
 */
void DialogPowerOn::setUIControl(){
    lb_title = new QLabel;
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;background-color:transparent;");
    lb_title->setFixedHeight(70);

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
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
    if(this->confirmType == DlgPowerOnType::OKCancel){
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


    // 확인 버튼
    QPushButton *btn_save = new QPushButton;
    btn_save->setText(tr("Confirm"));
    //btn_save->setText(tr("확인"));
    btn_save->setFixedSize(140,40);
    btn_save->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_save->setCursor(Qt::PointingHandCursor);

    // 취소 버튼
    btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));
    //btn_cancel->setText(tr("취소"));
    btn_cancel->setFixedSize(140,40);
    btn_cancel->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:transparent;border-radius:20px;border:1px solid #4D4D4D;");
    btn_cancel->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_btn = new QHBoxLayout;
    box_btn->setSpacing(30);
    box_btn->setContentsMargins(0,0,0,0);
    box_btn->addStretch(1);
    box_btn->addWidget(btn_save);
    if(this->confirmType == DlgPowerOnType::OKCancel){
    box_btn->addWidget(btn_cancel);
    }
    box_btn->addStretch(1);

    QVBoxLayout *box_contents = new QVBoxLayout;
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(25,25,25,30);
    box_contents->setAlignment(Qt::AlignTop);
    box_contents->addWidget(lb_text);
    box_contents->addLayout(box_btn);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(0,0,0,0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(wg_top);
    box_total->addLayout(box_contents);

    QWidget *wg_total = new QWidget;
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
void DialogPowerOn::setTitle(const QString &p_title){
    lb_title->setText(p_title);
}

/**
 * @brief DialogConfirm::setText : 텍스트 세팅함수
 * @param p_text
 */
void DialogPowerOn::setText(const QString &p_text){
    lb_text->setText(p_text);
}

/**
 * @brief DialogConfirm::setAlertMode : Alert Mode 변환
 */
void DialogPowerOn::setAlertMode(){
    btn_cancel->hide();
}
