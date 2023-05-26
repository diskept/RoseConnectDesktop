#include "dialog_radio.h"
#include "common/gscommon.h"
#include "common/my_lang.h"
#include <QDebug>

using namespace Dialog;



/**
 * @brief Dialog_Radio::Dialog_Radio : 라디오그룹 다이얼로그
 * @param parent
 */
Dialog_Radio::Dialog_Radio(QWidget *parent)
     : QDialog(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief Dialog_Radio::~Dialog_Radio : 소멸자
 */
Dialog_Radio::~Dialog_Radio(){
    clearRadio();
}

/**
 * @brief Dialog_Radio::setInit : 초기 세팅
 */
void Dialog_Radio::setInit(){
    // ---------------------------------------
    //  다이얼로그 속성 세팅
    // ---------------------------------------
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedWidth(DLG_WIDTH);
    this->setMinimumHeight(DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");

    // ---------------------------------------
    //  Radio 스타일 정의
    // ---------------------------------------
    radio_style = "QRadioButton {font-size:20px;color:#CCCCCC;spacing:15px;padding:15px;border-bottom:1px solid #404040;}";
    radio_style += " QRadioButton::indicator {width:40px;height:40px;}";
    radio_style += " QRadioButton::indicator::unchecked {image: url(:images/icon_che_off.png);}";
    radio_style += " QRadioButton::indicator::checked {image: url(:images/icon_che_on.png);}";

    // ---------------------------------------
    //  QList
    // ---------------------------------------
    listRadio = new QList<QRadioButton*>();
}

/**
 * @brief Dialog_Radio::setUIControl : UI 세팅
 */
void Dialog_Radio::setUIControl(){

    // 타이틀
    lb_title = new QLabel;
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
    box_top->setAlignment(Qt::AlignTop);
    box_top->addWidget(lb_title);

    QWidget *widget_title = new QWidget;
    widget_title->setObjectName("widget_title");
    widget_title->setLayout(box_top);
    widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");

    box_radio = new QVBoxLayout;
    box_radio->setSpacing(0);
    box_radio->setContentsMargins(0,0,0,0);

    // 하단 버튼
    QPushButton *btn_ok = new QPushButton;
    // btn_ok->setText(tr("추가"));
     btn_ok->setText(tr("Add"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    btn_ok->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_ok->setFixedSize(140,40);
    QPushButton *btn_cancel = new QPushButton;
    // btn_cancel->setText(tr("취소"));
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



    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(widget_title);
    vl_total->addLayout(box_radio);
    vl_total->addSpacing(100);
    vl_total->addLayout(hl_btn);


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
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedOk()));
}

/**
 * @brief Dialog_Radio::setTitle : 타이틀 세팅
 * @param p_title
 */
void Dialog_Radio::setTitle(const QString &p_title){
    lb_title->setText(GSCommon::getTextCutFromLabelWidth(p_title, DLG_WIDTH-150, lb_title->font()));
    if(lb_title->text().contains("…")){
        lb_title->setToolTip(p_title);
        lb_title->setToolTipDuration(2000);
    }
}

/**
 * @brief Dialog_Radio::setRadioButtons : 라디오 버튼 세팅
 * @param p_list
 * @note 라디오버튼 clicked 미사용, QList에 담아 selected 확인처리
 */
void Dialog_Radio::setRadioButtons(const QStringList &p_list){

    clearRadio();

    for(int i = 0 ; i < p_list.size(); i++){
        QRadioButton *radio = new QRadioButton;
        radio->setText(p_list.at(i));
        radio->setStyleSheet(radio_style);
        radio->setCursor(Qt::PointingHandCursor);
        radio->setProperty("index", i);

        listRadio->append(radio);
        box_radio->addWidget(radio);
    }
}

/**
 * @brief Dialog_Radio::clearRadio : 라디오 버튼 정보 clear
 */
void Dialog_Radio::clearRadio(){
    if(listRadio->size() > 0){
        qDeleteAll(listRadio->begin(), listRadio->end());
        listRadio->clear();
    }
    GSCommon::clearLayout(box_radio);
}

/**
 * @brief Dialog_Radio::getSelectedIndex : 선택된 라디오버튼 index 반환
 * @return int selectedIndex
 */
int Dialog_Radio::getSelectedIndex() const
{
    int tmp_selectedIndex = -1;

    for(int i = 0 ; i < listRadio->size(); i++){
        if(listRadio->at(i)->isChecked()){
            tmp_selectedIndex = i;
            break;
        }
    }

    return tmp_selectedIndex;
}

/**
 * @brief Dialog_Radio::slot_clickedOk : [슬롯] OK 버튼 클릭
 */
void Dialog_Radio::slot_clickedOk(){
    setResult(QDialog::Accepted);
    hide();
}
