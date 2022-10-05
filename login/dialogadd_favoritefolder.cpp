#include "dialogadd_favoritefolder.h"
#include "common/gscommon.h"
#include "common/global.h"
#include "widget/toastmsg.h"
#include "common/networkhttp.h"
#include <QJsonArray>



using namespace  Dialog;

/**
 * @brief DialogAdd_FavoriteFolder::DialogAdd_FavoriteFolder : 폴더-네트워크-즐겨찾기 등록 다이얼로그
 * @param p_dirName QString 디렉토리명(default Favorite name)
 * @param p_path QString smbPath(ID,PW 포함)
 * @param parent QWidget;
 */
DialogAdd_FavoriteFolder::DialogAdd_FavoriteFolder(QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
}
DialogAdd_FavoriteFolder::DialogAdd_FavoriteFolder(QString p_dirName, QString p_path, QWidget *parent)
    : QDialog(parent)
{
    setInit();
    setUIControl();
    setFavoriteData(p_dirName, p_path);
}

/**
 * @brief DialogAdd_FavoriteFolder::setInit : 초기 세팅 함수
 */
void DialogAdd_FavoriteFolder::setInit(){
    // --------------------------------------------------------
    //  Set Dialog property
    // --------------------------------------------------------
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    //this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setFixedWidth(DLG_WIDTH);
    this->setStyleSheet("background-color:transparent;");

    linker = Linker::getInstance();
}

/**
 * @brief DialogAdd_FavoriteFolder::setUIControl : UI 세팅 함수
 */
void DialogAdd_FavoriteFolder::setUIControl(){

    // ------------------------------------------
    // 타이틀 & Close
    // ------------------------------------------
    QLabel *lb_title = new QLabel;
    lb_title->setText(tr("Add Favorites"));// lb_title->setText(tr("즐겨찾기 추가"));
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
    box_top->addWidget(lb_title);

    QWidget *widget_title = new QWidget;
    widget_title->setObjectName("widget_title");
    widget_title->setLayout(box_top);
    widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");


    // ------------------------------------------
    // 즐겨찾기 이름
    // ------------------------------------------
    QLabel *lb_name = new QLabel;
    lb_title->setText(tr("Add Favorites"));// lb_title->setText(tr("즐겨찾기 추가"));
    lb_name->setStyleSheet("color:#FFFFFF;font-size:18px;padding-left:6px;");


    lineEdit_name = new QLineEdit;
    lineEdit_name->setMaxLength(15);
    lineEdit_name->setPlaceholderText(tr("Favorite name")); //lineEdit_name->setPlaceholderText(tr("즐겨찾기 이름"));
    lineEdit_name->setFixedHeight(50);
    lineEdit_name->setTextMargins(23,14,23,14);
    lineEdit_name->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");

    // ------------------------------------------
    // SMB PATH
    // ------------------------------------------
    QLabel *lb_path = new QLabel;
    lb_path->setText(tr("Favorite path"));//lb_path->setText(tr("즐겨찾기 경로"));
    lb_path->setStyleSheet("color:#FFFFFF;font-size:18px;padding-left:6px;");

    lb_favorPath = new QLabel;
    lb_favorPath->setMinimumHeight(50);
    lb_favorPath->setContentsMargins(23,14,23,14);
    lb_favorPath->setStyleSheet("font-size:20px;color:#777777;background-color:#212121;border-radius:2px;border:1px solid #4d4d4d;");
    lb_favorPath->setWordWrap(true);

    // 하단 버튼
    QPushButton *btn_ok = new QPushButton;
    btn_ok->setText(tr("Confirm"));// btn_ok->setText(tr("확인"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    btn_ok->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_ok->setFixedSize(140,40);
    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));// btn_cancel->setText(tr("취소"));
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

    QVBoxLayout *box_contents = new QVBoxLayout;
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(25,25,25,8);
    box_contents->setAlignment(Qt::AlignTop);
    box_contents->addWidget(lb_name);
    box_contents->addSpacing(8);
    box_contents->addWidget(lineEdit_name);
    box_contents->addSpacing(15);
    box_contents->addWidget(lb_path);
    box_contents->addSpacing(8);
    box_contents->addWidget(lb_favorPath);
    box_contents->addSpacing(34);
    box_contents->addLayout(hl_btn);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(widget_title);
    vl_total->addLayout(box_contents);


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
 * @brief DialogAdd_FavoriteFolder::setFavoriteData : 즐겨찾기 데이터 세팅
 * @param p_dirName QString 디렉토리명(default Favorite name)
 * @param p_path QString smbPath(ID,PW 포함)
 */
void DialogAdd_FavoriteFolder::setFavoriteData(QString p_dirName, QString p_path){
    lineEdit_name->setText(p_dirName);
    lb_favorPath->setText(p_path);
}

/**
 * @brief DialogAdd_FavoriteFolder::checkUserInput::사용자 입력값 검사
 * @return bool
 */
bool DialogAdd_FavoriteFolder::checkUserInput(){
    bool flagOk = true;


    if(lineEdit_name->text().trimmed().isEmpty()){
        flagOk = false;
        ToastMsg::show(this, "", QString(tr("Please enter your favorite name.")));
        //ToastMsg::show(this, "", QString(tr("즐겨찾기 이름을 입력해주세요.")));
    }

    return flagOk;
}

/**
 * @brief DialogAdd_FavoriteFolder::requestAddFavoriteFolder : 즐겨찾기 폴더 추가
 */
void DialogAdd_FavoriteFolder::requestAddFavoriteFolder(){
    QJsonObject jsonData;
    jsonData.insert("albumFavName", lineEdit_name->text().trimmed());
    jsonData.insert("albumFavPath", lb_favorPath->text().trimmed());

    QJsonArray jsonArrData;
    jsonArrData.append(jsonData);

    QJsonObject jsonParams;
    jsonParams.insert("data", jsonArrData);

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_ADD_FAVORITE_FOLDER, QString("http://%1:%2/media.album.folder.fav.add")
                        .arg(global.device.getDeviceIP()).arg(global.port), jsonParams, true);
}

/**
 * @brief DialogAdd_FavoriteFolder::setResultOfFavoriteFolder : 즐겨찾기 폴더 추가 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void DialogAdd_FavoriteFolder::setResultOfFavoriteFolder(const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_message = "message";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

                if(p_jsonData.contains(jsonKey_message)&&p_jsonData[jsonKey_message].toString().toLower()!="success"){
                    // 실패
                    ToastMsg::show(this,"",p_jsonData[jsonKey_message].toString());
                }
                else{
                    // 즐겨찾기  추가 시그널 호출 ==> 폴더 화면 갱신
                    emit linker->signal_favoriteFolderChanged();

                    ToastMsg::show(this,"",tr("Favorites folder has been added."));
                    //ToastMsg::show(this,"",tr("즐겨찾기 폴더가 추가되었습니다."));

                    setResult(QDialog::Accepted);
                    this->hide();
                }
            /*}else if(p_jsonData.contains(jsonKey_message)){
                ToastMsg::show(this,"",p_jsonData[jsonKey_message].toString());
            }*/
        }
    }else{
        ToastMsg::show(this,"",tr("Please check the network status."));
        //ToastMsg::show(this,"",tr("네트워크 상태를 확인해주세요."));
    }
}

/**
 * @brief DialogAdd_FavoriteFolder::slot_ok [슬롯] 확인 버튼 클릭시
 */
void DialogAdd_FavoriteFolder::slot_ok(){
    if(checkUserInput()){
        requestAddFavoriteFolder();
    }
}

/**
 * @brief DialogAdd_FavoriteFolder::slot_responseHttp : [슬롯]
 * @param p_id int 요청 아이디
 * @param p_jsonData QJsonObject response
 */
void DialogAdd_FavoriteFolder::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){
    if(p_id == HTTP_ADD_FAVORITE_FOLDER){
        setResultOfFavoriteFolder(p_jsonData);
    }
}
