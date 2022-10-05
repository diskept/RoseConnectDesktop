#include "friendprofileitem.h"
#include "widget/toastmsg.h"
#include "common/global.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>

#include <common/filedownloader.h>
#include <common/gscommon.h>
#include <common/networkhttp.h>

int const WIDGET_HEIGHT = 80;
const int PROFILEIMG_SIZE = 60;
const int PROFILEIMG_SIZE_GRADE = 62;

const int HTTP_GET_FPI_DELETE = 97;
const int HTTP_GET_FPI_REJECT = 96;
const int HTTP_GET_FPI_ACCEPT = 95;
const int HTTP_GET_FPI_REQUEST = 94;
const int HTTP_GET_FREIND_CANCEL = 93;
/**
 * @brief FriendProfileItem::FriendProfileItem 생성자
 * @param p_type
 * @param parent
 */
FriendProfileItem::FriendProfileItem(ProfileBtnType p_type, QWidget *parent) : QWidget(parent)
{
    this->profileBtnType = p_type;
    this->setFixedHeight(WIDGET_HEIGHT);
    this->setUIControl();
}

void FriendProfileItem::setUIControl(){

    // 내 프로필
    lb_profile_grade = new QLabel();
    lb_profile = new QLabel();
    lb_profile_grade->setFixedSize(PROFILEIMG_SIZE_GRADE,PROFILEIMG_SIZE_GRADE);
    lb_profile->setFixedSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE);
    lb_profile_grade->raise();

    this->lb_username = new QLabel();
    this->lb_username->setStyleSheet("font-size:18px;color:#E6E6E6;");
    this->lb_bio = new QLabel();
    this->lb_bio->setStyleSheet("font-size:14px;color:#999999;");
    this->lb_userid = new QLabel();//cheon211018
    this->lb_userid->setStyleSheet("font-size:14px;color:#999999;");//cheon211018

    QVBoxLayout *vl_nickName = new QVBoxLayout();
    vl_nickName->setContentsMargins(0,0,0,0);
    vl_nickName->setSpacing(0);
    vl_nickName->addStretch(1);
    vl_nickName->addWidget(this->lb_username);
    vl_nickName->addWidget(this->lb_bio);
    //vl_nickName->addWidget(this->lb_userid);
    vl_nickName->addStretch(1);

    QPushButton *btn_unconnect = new QPushButton(tr("Delete"));//cheon211103
    btn_unconnect->setFixedSize(74,34);
    btn_unconnect->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_reject = new QPushButton(tr("Reject"));
    btn_reject->setFixedSize(74,34);
    btn_reject->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_accept = new QPushButton(tr("Accept"));
    btn_accept->setFixedSize(74,34);
    btn_accept->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"));
    btn_cancel->setFixedSize(74,34);
    btn_cancel->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_request = new QPushButton(tr("Request"));
    btn_request->setFixedSize(74,34);
    /*
    QPushButton *btn_unconnect = new QPushButton(tr("해제"));
    btn_unconnect->setFixedSize(74,34);
    btn_unconnect->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_reject = new QPushButton(tr("거절"));
    btn_reject->setFixedSize(74,34);
    btn_reject->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_accept = new QPushButton(tr("수락"));
    btn_accept->setFixedSize(74,34);
    btn_accept->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_cancel = new QPushButton(tr("취소"));
    btn_cancel->setFixedSize(74,34);
    btn_cancel->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_request = new QPushButton(tr("신청"));
    btn_request->setFixedSize(74,34);
    */

    btn_request->setCursor(Qt::PointingHandCursor);

    QGridLayout *gl_myInfo = new QGridLayout();
    gl_myInfo->setContentsMargins(0,0,0,0);
    gl_myInfo->setSpacing(0);
    gl_myInfo->addWidget(lb_profile, 0, 0);
    gl_myInfo->addWidget(lb_profile_grade, 0, 0);
    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(33,10,50,10);
    hl_total->setSpacing(0);
    hl_total->addLayout(gl_myInfo);
    hl_total->addSpacing(15);
    hl_total->addLayout(vl_nickName);
    hl_total->addStretch(1);
    switch (profileBtnType) {
    case FriendProfileItem::ProfileBtnType::FriendList :
        hl_total->addWidget(btn_unconnect);
        break;
    case FriendProfileItem::ProfileBtnType::Receive :
        hl_total->addWidget(btn_reject);
        hl_total->addSpacing(22);
        hl_total->addWidget(btn_accept);
        break;
    case FriendProfileItem::ProfileBtnType::Send :
        hl_total->addWidget(btn_cancel);
        break;
    case FriendProfileItem::ProfileBtnType::Find :
        hl_total->addWidget(btn_request);
        break;
    }
    QWidget *widget_friendProfileItem = new QWidget();
    widget_friendProfileItem->setObjectName("widget_friendProfileItem");
    widget_friendProfileItem->setLayout(hl_total);
    widget_friendProfileItem->setStyleSheet("#widget_friendProfileItem { border-bottom:1px solid #2A2A2A; } QPushButton { font-size:15px;color:#E6E6E6;border:2px solid #666666;border-radius:17px; } QPushButton:hover { border:2px solid #B18658;background-color:#B18658;color:#FFFFFF; } ");
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);
    vl->setSpacing(0);
    vl->addWidget(widget_friendProfileItem);
    this->setLayout(vl);

    // 커넥션
    connect(btn_unconnect, &QPushButton::clicked, this, &FriendProfileItem::slot_clickedUnConnect);
    connect(btn_reject, &QPushButton::clicked, this, &FriendProfileItem::slot_clickedReject);
    connect(btn_accept, &QPushButton::clicked, this, &FriendProfileItem::slot_clickedAccept);
    connect(btn_cancel, &QPushButton::clicked, this, &FriendProfileItem::slot_clickedCancel);
    connect(btn_request, &QPushButton::clicked, this, &FriendProfileItem::slot_clickedRequest);
}

/**
 * @brief FriendProfileItem::setDataProfileItem 데이터 세팅
 * @param p_jsonObject
 */
void FriendProfileItem::setDataProfileItem(QJsonObject p_jsonObject){//cheon211018

    //print_debug();
    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "\nFriendProfileItem::setDataProfileItem--strJson= " << strJson;


    QString tmp_username = "";
    int tmp_userid = 0;
    QString tmp_bio = "";
    QString tmp_imgPath = "";
    if(p_jsonObject.contains("name")){
        tmp_username = p_jsonObject["name"].toString();
    }
    if(p_jsonObject.contains("id")){
        tmp_userid = p_jsonObject["id"].toInt();
    }
    if( this->profileBtnType==ProfileBtnType::Receive ){
        //print_debug();
        if(p_jsonObject.contains("stranger")){
            tmp_username = p_jsonObject["stranger"].toString();
        }
    }else if( this->profileBtnType==ProfileBtnType::Send ){
        //print_debug();
        if(p_jsonObject.contains("friendname")){
            tmp_username = p_jsonObject["friendname"].toString();
        }
    }
    if(p_jsonObject.contains("title")){
        tmp_bio = p_jsonObject["title"].toString();
    }
    if(p_jsonObject.contains("profileImage")){
        tmp_imgPath = p_jsonObject["profileImage"].toString();
    }
    //qDebug() << "tmp_username = " << tmp_username;
    this->lb_username->setText(tmp_username);
    this->lb_userid->setText(QString("%1").arg(tmp_userid));
    //this->lb_username->setText(QString("%1").arg(tmp_userid));
    this->lb_bio->setText(tmp_username);

    if(tmp_bio==""){
        this->lb_bio->setVisible(false);
    }else{
        this->lb_bio->setVisible(true);
    }

    // 기본 5등급부터 시작.. output에 등급 key 없음..
    // 그래서, 현재 항상 무조건 5등급의 기본이미지 세팅
    this->lb_profile->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_image_2.png"));
    this->lb_profile_grade->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_outline_2.png"));

    if(tmp_imgPath!=""){
        FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
        connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
        tmp_fileDownLoader->setImageURL(QUrl(tmp_imgPath));
    }
/*
    QString tmp_username = "";
    QString tmp_bio = "";
    QString tmp_imgPath = "";
    if(p_jsonObject.contains("username")){
        tmp_username = p_jsonObject["username"].toString();
    }
    if( this->profileBtnType==ProfileBtnType::Receive ){
        if(p_jsonObject.contains("stranger")){
            tmp_username = p_jsonObject["stranger"].toString();
        }
    }else if( this->profileBtnType==ProfileBtnType::Send ){
        if(p_jsonObject.contains("friendname")){
            tmp_username = p_jsonObject["friendname"].toString();
        }
    }
    if(p_jsonObject.contains("bio")){
        tmp_bio = p_jsonObject["bio"].toString();
    }
    if(p_jsonObject.contains("main_profileimg")){
        tmp_imgPath = p_jsonObject["main_profileimg"].toString();
    }

    this->lb_username->setText(tmp_username);
    this->lb_bio->setText(tmp_bio);

    if(tmp_bio==""){
        this->lb_bio->setVisible(false);
    }else{
        this->lb_bio->setVisible(true);
    }

    // 기본 5등급부터 시작.. output에 등급 key 없음..
    // 그래서, 현재 항상 무조건 5등급의 기본이미지 세팅
    this->lb_profile->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_image_2.png"));
    this->lb_profile_grade->setPixmap(*GSCommon::getUIPixmapImg(":/images/setting/LV_VIP_outline_2.png"));

    if(tmp_imgPath!=""){
        FileDownloader *tmp_fileDownLoader = new FileDownloader(this);
        connect(tmp_fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImageMainProfile()));
        tmp_fileDownLoader->setImageURL(QUrl(tmp_imgPath));
    }
    */
}


/**
 * @brief FriendProfileItem::slot_loadImageMainProfile [SLOT] 프로필 메인 이미지 세팅
 */
void FriendProfileItem::slot_loadImageMainProfile(){

    FileDownloader *tmp_fileDownloader = qobject_cast<FileDownloader*>(sender());
    QPixmap *pixmap_albumImg = new QPixmap();
    bool flagLoad = pixmap_albumImg->loadFromData(tmp_fileDownloader->downloadedData());
    if(flagLoad){
        QPixmap tmp_pixmap = pixmap_albumImg->scaled(PROFILEIMG_SIZE, PROFILEIMG_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // KeepAspectRatio

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(PROFILEIMG_SIZE,PROFILEIMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, PROFILEIMG_SIZE, PROFILEIMG_SIZE, PROFILEIMG_SIZE/2, PROFILEIMG_SIZE/2);    // border-radius:4px
        this->lb_profile->setPixmap(pixmap_painter);
    }

    tmp_fileDownloader->deleteLater();
}

/**
 * @brief FriendProfileItem::slot_clickedUnConnect [SLOT] 해제 버튼 클릭시...친구삭제
 */
void FriendProfileItem::slot_clickedUnConnect(){//cheon211023

/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_DELETE, QString("%1/user/friend/delete")
                     .arg(global.legacy_mod_api), params, true,true);
*/

    QJsonObject json;
    QString memberId = this->lb_userid->text();

    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    network->requestDelete(HTTP_GET_FPI_DELETE, QString("%1/member/friend?memberId=%2").arg(global.legacy_v1).arg(memberId), json, true);

}

/**
 * @brief FriendProfileItem::slot_clickedReject [SLOT] 거절 버튼 클릭시
 */
void FriendProfileItem::slot_clickedReject(){//cheon211018
print_debug();
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_REJECT, QString("%1/user/friend/deny")
                     .arg(global.legacy_mod_api), params, true,true);
                     */

    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    QString a = this->lb_userid->text();
    qDebug()<< "this->lb_username->text()--" << this->lb_username->text();
    bool convertOK;
    unsigned long int n = a.toULong(&convertOK);
    int i = n;
    QUrlQuery params;
    QJsonObject json;
    QJsonObject jsonmember;
    jsonmember.insert("id", i);
    json.insert("member", jsonmember);
    print_debug();
    QJsonDocument doc(json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "FriendProfileItem::slot_clickedAccept--json= " << json;

    //params.addQueryItem("memberId",i);
    //params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_REJECT, QString("%1/member/friend/deny")
                     .arg(global.legacy_v1), json, true,true);

}
/**
 * @brief FriendProfileItem::slot_clickedAccept [SLOT] 수락 버튼 클릭시
 */
void FriendProfileItem::slot_clickedAccept(){//cheon211018
print_debug();
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_ACCEPT, QString("%1/user/friend/accept")
                     .arg(global.legacy_mod_api), params, true,true);
                     */
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    QString a = this->lb_userid->text();
    qDebug()<< "this->lb_username->text()--" << this->lb_username->text();
    bool convertOK;
    unsigned long int n = a.toULong(&convertOK);
    int i = n;
    QUrlQuery params;
    QJsonObject json;
    QJsonObject jsonmember;
    jsonmember.insert("id", i);
    json.insert("member", jsonmember);
    print_debug();
    QJsonDocument doc(json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "FriendProfileItem::slot_clickedAccept--json= " << json;

    //params.addQueryItem("memberId",i);
    //params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_ACCEPT, QString("%1/member/friend/approve")
                     .arg(global.legacy_v1), json, true,true);

}
/**
 * @brief FriendProfileItem::slot_clickedCancel [SLOT] 취소 버튼 클릭시
 */
void FriendProfileItem::slot_clickedCancel(){//cheon211023

/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FREIND_CANCEL, QString("%1/user/friend/request/cancle")
                     .arg(global.legacy_mod_api), params, true,true);

    */

    QJsonObject json;
    QString memberId = this->lb_userid->text();

    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    network->requestDelete(HTTP_GET_FPI_DELETE, QString("%1/member/friend/request?memberId=%2").arg(global.legacy_v1).arg(memberId), json, true);


}

/**
 * @brief FriendProfileItem::slot_clickedRequest [SLOT] 신청 버튼 클릭시
 */
void FriendProfileItem::slot_clickedRequest(){//cheon211018//cheon211018
print_debug();
/*
    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_REQUEST, QString("%1/user/friend/request")
                     .arg(global.legacy_mod_api), params, true,true);
                     */

    NetworkHttp *network = new NetworkHttp();
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    QString a = this->lb_userid->text();
    qDebug()<< "this->lb_username->text()--" << this->lb_username->text();
    bool convertOK;
    unsigned long int n = a.toULong(&convertOK);
    int i = n;
    QUrlQuery params;
    QJsonObject json;
    QJsonObject jsonmember;
    jsonmember.insert("id", i);
    json.insert("member", jsonmember);
    print_debug();
    QJsonDocument doc(json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "FriendProfileItem::slot_clickedAccept--json= " << json;

    //params.addQueryItem("memberId",i);
    //params.addQueryItem("friendname",this->lb_username->text());
    network->request(HTTP_GET_FPI_REQUEST, QString("%1/member/friend/request")
                     .arg(global.legacy_v1), json, true,true);
}


/**
 * @brief FriendProfileItem::slot_responseHttp Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void FriendProfileItem::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){//cheon211023

    print_debug();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "FriendProfileItem::slot_responseHttp--p_jsonObject= " << strJson;

    QString tmp_status = "";
    bool flagOk = false;
    QString tmp_msg = "";
    switch (p_id) {
    case HTTP_GET_FPI_DELETE :
        print_debug();
        tmp_status = "";
        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }
        if(flagOk==true){
            emit signal_removeItem();
            ToastMsg::show(this, "", tr("You have canceled a friend."));
            //ToastMsg::show(this, "", tr("친구를 해제하였습니다."));
        }else{
            ToastMsg::show(this, "", tr("You have failed to cancel a friend."));
        }
        break;
    case HTTP_GET_FPI_REJECT :
        print_debug();
        tmp_status = "";
        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }
        if(flagOk==true){
            emit signal_removeItem();
            ToastMsg::show(this, "", tr("The person has denied your friend request."));
            //ToastMsg::show(this, "", tr("친구요청을 거절하였습니다."));
        }else{
            ToastMsg::show(this, "", tr("You have failed to deny your friend request."));
        }
        break;
    case HTTP_GET_FPI_ACCEPT:
        print_debug();
        tmp_status = "";
        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }
        if(flagOk==true){
            emit signal_removeItem();
            ToastMsg::show(this, "", tr("The person has accepted your friend request."));
            //ToastMsg::show(this, "", tr("친구요청을 수락하였습니다."));
        }else{
            ToastMsg::show(this, "", tr("You have failed to accepte your friend request."));
        }
        break;
    case HTTP_GET_FPI_REQUEST:
        print_debug();
        tmp_status = "";
        flagOk = false;
        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }
        if(p_jsonObject.contains("message")){
            tmp_msg = p_jsonObject["message"].toString();
        }
        if(flagOk==true){
            if(tmp_msg!=""){
                ToastMsg::show(this, "", tmp_msg+" - It has already been requested.");  // 이미 요청 되었습니다.
                //ToastMsg::show(this, "", tmp_msg);  // 이미 요청 되었습니다.
            }else{
                ToastMsg::show(this, "", tr("Friend Request sent."));
                //ToastMsg::show(this, "", tr("친구요청을 보냈습니다."));
            }
        }
        break;
    case HTTP_GET_FREIND_CANCEL:
        print_debug();
        tmp_status = "";

        if(p_jsonObject.contains("flagOk")){
            flagOk = p_jsonObject["flagOk"].toBool();
        }
        if(flagOk==true){
            print_debug();
            emit signal_removeItem();
            ToastMsg::show(this, "", tr("Friend Request canceled."));
            //ToastMsg::show(this, "", tr("요청을 취소하였습니다."));
        }else{
            ToastMsg::show(this, "", tr("You have failed to cancel friend Request."));
        }
        break;
    }

    sender()->deleteLater();
}
