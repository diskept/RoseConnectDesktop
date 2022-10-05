#include "dialog_newplaylist.h"
#include "common/gscommon.h"
#include "widget/toastmsg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QStyle>

using namespace Dialog;


/**
 * @brief Dialog_NewPlaylist::Dialog_NewPlaylist : 새 플레이리스트 추가 다이얼로그
 * @param parent
 */
Dialog_NewPlaylist::Dialog_NewPlaylist(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief Dialog_NewPlaylist::setInit : 초기 세팅
 */
void Dialog_NewPlaylist::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief Dialog_NewPlaylist::setUIControl : UI 세팅
 */
void Dialog_NewPlaylist::setUIControl(){
    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Add a New Playlist"));//lb_title->setText(tr("새 플레이리스트 추가"));
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

    edit_name = new QLineEdit;
    edit_name->setMaxLength(30);
    edit_name->setPlaceholderText(tr("Playlist Name"));// edit_name->setPlaceholderText(tr("플레이리스트 이름"));
    edit_name->setFixedHeight(50);
    edit_name->setTextMargins(23,14,23,14);
    edit_name->setStyleSheet(tmp_styleLineEdit);

    // -----------------------------------------
    //  하단 버튼 (확인,취소)
    // -----------------------------------------
    QPushButton *btn_ok = new QPushButton;
    btn_ok->setText(tr("Confirm"));//btn_ok->setText(tr("확인"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    //btn_ok->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_ok->setStyleSheet("QPushButton{background:transparent; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                          "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
    btn_ok->setFixedSize(140,40);

    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));//btn_cancel->setText(tr("취소"));
    btn_cancel->setCursor(Qt::PointingHandCursor);
    //btn_cancel->setStyleSheet("font-size:18px;color:#CCCCCC;border:1px solid #777777;border-radius:20px;");
    btn_cancel->setStyleSheet("QPushButton{background:transparent; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                              "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
    btn_cancel->setFixedSize(140,40);

    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->setContentsMargins(0,0,0,0);
    hl_btn->setSpacing(0);
    hl_btn->setAlignment(Qt::AlignCenter);
    hl_btn->addWidget(btn_ok);
    hl_btn->addSpacing(30);
    hl_btn->addWidget(btn_cancel);


    // -----------------------------------------
    //  박스 구성 및 레이아웃 세팅
    // -----------------------------------------
    QVBoxLayout *vl_editInfo = new QVBoxLayout;
    vl_editInfo->setContentsMargins(25,25,25,30);
    vl_editInfo->setSpacing(30);
    vl_editInfo->setAlignment(Qt::AlignTop);
    vl_editInfo->addWidget(edit_name);
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

    // -----------------------------------------
    //  Connect 세팅
    // -----------------------------------------
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOk()));
    connect(edit_name, &QLineEdit::textChanged, [=]{ style()->polish(edit_name);});
}

/**
 * @brief Dialog_NewPlaylist::checkUserInput : 사용자 입력값 검사 함수
 * @note empty 검사만 한다. regex 확정 시 regex 검사 추가 필요.<br>
 * @return bool if true, ok
 */
bool Dialog_NewPlaylist::checkUserInput(){
    bool flagOk = true;

    if(edit_name->text().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", tr("Please enter a playlist name."));
        //ToastMsg::show(this, "", tr("플레이리스트 이름을 입력해주세요."));
    }

    return flagOk;
}


/**
 * @brief Dialog_NewPlaylist::slot_clickedOk : [슬롯] 확인 버튼 클릭
 */
void Dialog_NewPlaylist::slot_clickedOk(){
    if(checkUserInput()){
        setResult(QDialog::Accepted);
        hide();
    }
}
