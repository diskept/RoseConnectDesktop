#include "dialogeditradio.h"
#include "common/gscommon.h"
#include "common/global.h"
#include "widget/toastmsg.h"
#include "common/networkhttp.h"

#include <QLabel>
#include <QRegExpValidator>



using namespace Dialog;

/**
 * @brief DialogEditRadio::DialogEditRadio
 * : 라디오 > 사용자채널 > 방송 채널 추가 다이얼로그 .cpp
 * @param parent
 */
DialogEditRadio::DialogEditRadio(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief DialogEditRadio::setInit : 초기 세팅
 */
void DialogEditRadio::setInit(){
    linker = Linker::getInstance();

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief DialogEditRadio::setUIControl : UI 세팅
 */
void DialogEditRadio::setUIControl(){

    // 타이틀
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Add Broadcast Channel"));
    //lb_title->setText(tr("방송 채널 추가"));
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;background-color:transparent;padding-left:70px;");

    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setParent(this);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_top = new QHBoxLayout;
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title,1);
    box_top->addWidget(btn_close);

    QWidget *wg_top = new QWidget;
    wg_top->setObjectName("wg_top");
    wg_top->setLayout(box_top);
    wg_top->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #404040;}");

    // 채널 이름
    QLabel *lb_name = new QLabel;
    lb_name->setText(tr("Channel Name"));
    //lb_name->setText(tr("채널이름"));
    lb_name->setStyleSheet("color:#FFFFFF;font-size:18px;padding-left:6px;");

    le_name = new QLineEdit;
    le_name->setMaxLength(30);
    le_name->setPlaceholderText(tr("Channel Name"));
    //le_name->setPlaceholderText(tr("채널이름"));
    le_name->setFixedHeight(50);
    le_name->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");

    // 채널 URL
    QLabel *lb_url = new QLabel;
    lb_url->setText(tr("Channel URL"));
    //lb_url->setText(tr("채널 URL"));
    lb_url->setStyleSheet("color:#FFFFFF;font-size:18px;padding-left:6px;");

    le_url = new QLineEdit;
    le_url->setMaxLength(100);
    le_url->setPlaceholderText("http://xxx.xxx");
    le_url->setFixedHeight(50);
    le_url->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;padding-left:23px;");



    // 확인 버튼
    QPushButton *btn_save = new QPushButton;
    btn_save->setText(tr("Confirm"));
    //btn_save->setText(tr("확인"));
    btn_save->setFixedSize(140,40);
    btn_save->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_save->setCursor(Qt::PointingHandCursor);

    // 취소 버튼
    QPushButton *btn_cancel = new QPushButton;
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
    box_btn->addWidget(btn_cancel);
    box_btn->addStretch(1);


    QVBoxLayout *box_contents = new QVBoxLayout;
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(25,25,25,30);
    box_contents->setAlignment(Qt::AlignTop);
    box_contents->addWidget(lb_name);
    box_contents->addSpacing(8);
    box_contents->addWidget(le_name);
    box_contents->addSpacing(15);
    box_contents->addWidget(lb_url);
    box_contents->addSpacing(8);
    box_contents->addWidget(le_url);
    box_contents->addSpacing(34);
    box_contents->addLayout(box_btn);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(0,0,0,0);
    box_total->addWidget(wg_top);
    box_total->addLayout(box_contents);

    QWidget *wg_total = new QWidget;
    wg_total->setObjectName("wg_total");
    wg_total->setLayout(box_total);
    wg_total->setStyleSheet("#wg_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");

    QVBoxLayout *box_wrap = new QVBoxLayout;
    box_wrap->setContentsMargins(0,0,0,0);
    box_wrap->setSpacing(0);
    box_wrap->addWidget(wg_total);

    setLayout(box_wrap);

    // 커넥션
    connect(btn_save, SIGNAL(clicked()), this, SLOT(slot_clickedSave()));
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
}



/**
 * @brief DialogEditRadio::checkUserInput : 사용자 입력값 검사
 * @details 채널이름, 채널URL 확인
 * @return
 */
bool DialogEditRadio::checkUserInput(){
    bool flagOk = true;

    QRegExpValidator validator(QRegExp("^(https?):\\/\\/([^:\\/\\s]+)(:([^\\/]*))?((\\/[^\\s/\\/]+)*)?(\\/)?([^#\\s\\?]*)(\\?([^#\\s]*))?(#(\\w*))?$"));
    QString tmp_url = le_url->text().trimmed();
    int pos = 0;
    if(le_name->text().trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please enter the channel name.")));
        //ToastMsg::show(this, "", QString(tr("채널명을 입력해주세요.")));
    }
    else if(le_url->text().trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please enter channel Url.")));
        //ToastMsg::show(this, "", QString(tr("채널 Url을 입력해주세요.")));
    }
    else if(validator.validate(tmp_url,pos)!= validator.Acceptable){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please check the channel Url.")));
        //ToastMsg::show(this, "", QString(tr("채널 Url을 확인해주세요.")));
    }

    return flagOk;
}

/**
 * @brief DialogEditRadio::requestAddChannel : 채널 추가
 */
void DialogEditRadio::requestAddChannel(){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


    QJsonObject json;
    json.insert("channelAddNm", le_name->text().trimmed());
    json.insert("channelAddUrl", le_url->text().trimmed());

    network->request(HTTP_ADD_CHANNEL, QString("http://%1:%2/set.radio.channel.item.add")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port)
                     , json, true);
}

/**
 * @brief DialogEditRadio::setResultOfAddChannel : 채널추가 요청 결과 처리
 * @param p_jsonData
 */
void DialogEditRadio::setResultOfAddChannel(const QJsonObject &p_jsonData){

    bool flagOk = false;

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_message = "message";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){

        if(p_jsonData.contains(jsonKey_status)){
            flagOk = true;
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

                /// 이미 사용 중인 채널 이름이라 저장이 불가능한 경우에도 outs=ok 반환함.
                /// message 처리 필요함
                if(jsonStatus.contains(jsonKey_message) && jsonStatus[jsonKey_message].toString() == "이미 사용 중인 채널 이름입니다."){
                    ToastMsg::show(this, "", jsonStatus[jsonKey_message].toString());
                }else{
                    setResult(QDialog::Accepted);
                    this->hide();
                }
            /*}
            else{
                if(jsonStatus.contains(jsonKey_message)){
                    ToastMsg::show(this, "", jsonStatus[jsonKey_message].toString());
                }
            }*/
        }
    }

    if(!flagOk){
         ToastMsg::show(this, "", QString(tr("Unable to add radio user channel.")));
         //ToastMsg::show(this, "", QString(tr("라디오 사용자채널을 추가할 수 없습니다.")));
    }
}

/**
 * @brief DialogEditRadio::slot_clickedSave : [슬롯] 확인버튼 클릭
 */
void DialogEditRadio::slot_clickedSave(){

    if(checkUserInput()){
        requestAddChannel();
    }
}

/**
 * @brief DialogEditRadio::slot_responseHttp : [슬롯] Http 요청 결과
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void DialogEditRadio::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id==HTTP_ADD_CHANNEL){
        setResultOfAddChannel(p_jsonObject);
    }
}
