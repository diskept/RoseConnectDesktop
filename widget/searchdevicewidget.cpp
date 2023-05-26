#include "widget/searchdevicewidget.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"

#include "widget/VerticalScrollArea.h"
#include "widget/clickablewidget.h"
#include "widget/dialogconfirm.h"

#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExpValidator>
#include <QProgressDialog>
#include "widget/toastmsg.h"//c221026


const int MAXIMUM_REQ_COUNT = 255;
const int HTTP_DEVICE_NAME = 99;
const int HTTP_DEVICE_NAME_IP = 98;


/**
 * @brief SearchDeviceWidget::SearchDeviceWidget : 로즈 기기 검색 Widget
 * @param parent
 * @note 아이피 지정 검색, 로컬 아이피 255 전체 검색 지원
 * @note RS150, RS201, RS520 모델만 지원
 */
SearchDeviceWidget::SearchDeviceWidget(QWidget *parent)
    : QWidget(parent)
{
    this->linker = Linker::getInstance();

    this->setInit();
    this->setUIControl();
}


SearchDeviceWidget::~SearchDeviceWidget(){

    this->list_device.clear();

    delete this->findIp;

    this->deleteLater();
}


/**
 * @brief SearchDeviceWidget::setInit : 초기 세팅
 */
void SearchDeviceWidget::setInit(){

    this->selectedIndex = -1;
    this->flagReqUseIP = false;
    this->cntResponse = 0;
    this->cntWaitingResponse = 0;

    this->findIp = new FindIP();
}


/**
 * @brief SearchDeviceWidget::setUIControl : UI 세팅
 */
void SearchDeviceWidget::setUIControl(){

    this->widget_search_device = new QWidget();
    this->widget_search_device->setFixedSize(400, 350);
    this->widget_search_device->setStyleSheet("background-color:#4D4D4D; border-radius:10px;");

    QLabel *lb_search_title = new QLabel(this->widget_search_device);
    lb_search_title->setFixedSize(400, 50);
    lb_search_title->setGeometry(0, 0, 0, 0);
    lb_search_title->setStyleSheet("background-color:#4D4D4D; border-top-left-radius:10px; border-top-right-radius:10px; font-size:18px; color:#E6E6E6; padding-left:16px;");
    lb_search_title->setText(tr("Connectable Device"));

    QString btnStyle = "QPushButton{font-size:14px; color:#8E8E8E; padding-left:0px; background-color:transparent; border:1px solid #8E8E8E; border-radius:10px;}";
    btnStyle += "QPushButton:hover{font-size:14px; color:#777777; padding-left:0px; background-color:#777777; border:1px solid #777777; border-radius:10px;}";

    this->btn_searchWidget_adjust = new QPushButton(lb_search_title);
    this->btn_searchWidget_adjust->setFixedSize(100, 30);
    this->btn_searchWidget_adjust->setGeometry(290, 10, 0, 0);
    this->btn_searchWidget_adjust->setStyleSheet(btnStyle);
    this->btn_searchWidget_adjust->setCursor(Qt::PointingHandCursor);

    connect(this->btn_searchWidget_adjust, SIGNAL(clicked()), SLOT(slot_searchWidget_adjust()));

    this->lb_searchWidget_adjust = new QLabel(this->btn_searchWidget_adjust);
    this->lb_searchWidget_adjust->setFixedSize(100, 30);
    this->lb_searchWidget_adjust->setGeometry(0, 0, 0, 0);
    this->lb_searchWidget_adjust->setStyleSheet("font-size:14px; color:#FFFFFF; padding-left:0px; background-color:transparent;");
    this->lb_searchWidget_adjust->setAlignment(Qt::AlignCenter);
    this->lb_searchWidget_adjust->setText(tr("Pull up"));

    this->flag_adjust = false;

    this->widget_device_info = new QWidget(this->widget_search_device);
    this->widget_device_info->setFixedSize(398, 300);
    this->widget_device_info->setGeometry(1, 49, 0, 0);
    this->widget_device_info->setStyleSheet("background:#333333; border-top-right-radius:0px; border-top-left-radius:0px;");


    // -------------------------------------------------
    // Search Bar
    // -------------------------------------------------
    QWidget *widget_search = new QWidget(this->widget_device_info);
    widget_search->setFixedSize(398, 50);
    widget_search->setGeometry(0, 0, 0, 0);
    widget_search->setStyleSheet("background:transparent; border-radius:0px; border-bottom:1px solid #4D4D4D;");

    QPushButton *btn_search_ico = GSCommon::getUIBtnImg("btn_search", ":/images/icon_ser_w.png", widget_search);
    btn_search_ico->setGeometry(0, 0, 50, 50);

    this->le_search_ip = new QLineEdit(widget_search);
    this->le_search_ip->setMaxLength(15);
    this->le_search_ip->setValidator(new QRegExpValidator(QRegExp("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"), this->le_search_ip));
    this->le_search_ip->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    this->le_search_ip->setStyleSheet("font-size:16px; color:#FFFFFF; border:0px; margin-bottom:2px; background-color:transparent;");
    this->le_search_ip->setPlaceholderText(tr("Search rose device IP"));
    this->le_search_ip->setGeometry(50, 0, 298, 50);

    QPalette palette = this->le_search_ip->palette();
    palette.setColor(QPalette::PlaceholderText, Qt::lightGray);
    this->le_search_ip->setPalette(palette);

    this->btn_refresh_ico = GSCommon::getUIBtnImg("btn_refresh", ":/images/icon_refresh.png", widget_search);
    this->btn_refresh_ico->setCursor(Qt::PointingHandCursor);
    this->btn_refresh_ico->setGeometry(348, 0, 50, 50);

    connect(this->le_search_ip, SIGNAL(returnPressed()), btn_refresh_ico, SIGNAL(clicked()));
    connect(this->btn_refresh_ico, SIGNAL(clicked()), SLOT(searchDeviceIP()));

    this->progress_searchBar = new QProgressBar(widget_search);
    this->progress_searchBar->setStyleSheet("QProgressBar {font-size:16px;color:#FFFFFF;background-color:#4d4d4d;border:2px solid #666666;padding-top:2px;padding-right:0px;} QProgressBar::chunk{ background-color:#B18658; }");
    this->progress_searchBar->setAlignment(Qt::AlignCenter);
    this->progress_searchBar->setMaximum(254);
    this->progress_searchBar->setValue(0);
    this->progress_searchBar->setGeometry(0, 10, 398, 30);
    this->progress_searchBar->hide();


    // -------------------------------------------------
    // 조회 결과 Device 선택 박스
    // -------------------------------------------------
    this->vbox_searchDevice = new QVBoxLayout();
    this->vbox_searchDevice->setContentsMargins(0 ,0, 0, 0);
    this->vbox_searchDevice->setSpacing(0);
    this->vbox_searchDevice->setAlignment(Qt::AlignTop);

    QWidget *widget_searchDevice = new QWidget();
    widget_searchDevice->setLayout(this->vbox_searchDevice);
    widget_searchDevice->setMinimumSize(398, 125);
    widget_searchDevice->setStyleSheet("background-color:transparent;");

    VerticalScrollArea *scrollArea_search = new VerticalScrollArea();
    scrollArea_search->setObjectName("scrollArea");
    scrollArea_search->setWidget(widget_searchDevice);
    scrollArea_search->setWidgetResizable(false);
    scrollArea_search->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_search->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_search->setStyleSheet("#scrollArea {background-color:transparent; border:0px;}");


    // -------------------------------------------------
    //  검색진행 TEXT
    // -------------------------------------------------
    this->lb_search_status = new QLabel();
    this->lb_search_status->setText(tr("Click the search icon to search the device IP."));
    this->lb_search_status->setStyleSheet("background-color:transparent; font-size:16px; color:#FFFFFF;");
    this->lb_search_status->setWordWrap(true);
    this->lb_search_status->setAlignment(Qt::AlignCenter);
    this->lb_search_status->setFixedSize(398, 125);


    // -------------------------------------------------
    //  검색진행 & 검색결과박스 스택
    // -------------------------------------------------
    this->stackWidget_search = new QStackedWidget(this->widget_device_info);
    this->stackWidget_search->setFixedSize(398, 250);
    this->stackWidget_search->setGeometry(0, 50, 0, 0);
    this->stackWidget_search->addWidget(this->lb_search_status);
    this->stackWidget_search->addWidget(scrollArea_search);

    QVBoxLayout *vbox_search_device = new QVBoxLayout();
    GSCommon::clearLayout(vbox_search_device);
    vbox_search_device->setContentsMargins(0, 0, 0, 0);
    vbox_search_device->addWidget(this->widget_search_device);

    this->setLayout(vbox_search_device);






    //this->searchDeviceIP();

    //buttonTimer->start(); //c230405
}


void SearchDeviceWidget::slot_searchWidget_adjust(){

    if(this->flag_adjust == false){

        this->lb_searchWidget_adjust->setText(tr("Pull down"));

        this->widget_search_device->setFixedSize(400, 670);
        this->widget_device_info->setFixedSize(398, 620);
        this->stackWidget_search->setFixedSize(398, 570);

        this->flag_adjust = true;
    }
    else{

        this->lb_searchWidget_adjust->setText(tr("Pull up"));

        this->widget_search_device->setFixedSize(400, 350);
        this->widget_device_info->setFixedSize(398, 300);
        this->stackWidget_search->setFixedSize(398, 250);

        this->flag_adjust = false;
    }

    emit signal_searchWidget_adjust(this->flag_adjust);
}


/**
 * @brief SearchDeviceWidget::clearDeviceList : 장치 목록 clear
 * @details 장치목록 UI 및 관련 데이터 초기화
 */
void SearchDeviceWidget::clearDeviceList(){

    this->selectedIndex = -1;
    this->list_device.clear();
    this->cntResponse = 0;

    GSCommon::clearLayout(this->vbox_searchDevice);
}


/**
 * @brief IntroRegister::searchDevice : 로즈 장치 검색
 * @details 사용자가 입력한 아이피가 존재한다면 해당 아이피만 검색
 * 사용자가 입력한 아이피가 없다면 내 아이피 확인 후 전체 검색
 * 전체 검색의 경우 요청수를 제한한다. 응답 대기중인 수가 250건 이상이면 요청 불가
 * @param p_ip
 */
void SearchDeviceWidget::searchDevice(QString p_ip){

    this->cntWaitingResponse = 0;   //c211213
    this->le_search_ip->hide();
    this->btn_refresh_ico->hide();
    this->progress_searchBar->show();

    // 전체 검색
    if(p_ip == ""){
        this->flagReqUseIP = false;

        if(this->cntWaitingResponse <= MAXIMUM_REQ_COUNT){

            this->showStatusMsg(QString(tr("Looking for a device.")));
            this->clearDeviceList();

            QString ipPrefix;
            QStringList list_ipNumber = QString(this->findIp->getMyIP()).split(".");

            if(list_ipNumber.size() == 4){
                ipPrefix = QString("%1.%2.%3.")
                        .arg(list_ipNumber.at(0))
                        .arg(list_ipNumber.at(1))
                        .arg(list_ipNumber.at(2));

                for(int i = 0 ; i < 255; i++){
                    disconnect(sereachConID[i]);
                }
                print_debug();
                if(buttonTimer==nullptr){//c230405
                    buttonTimer = new QTimer(this);
                    buttonTimer->setInterval(1000);
                    connect(buttonTimer, &QTimer::timeout, this, [=](){//

                        print_debug();

                        if(sectTerm < 5){
                            this->progress_searchBar->setValue(254*((sectTerm)/5.0));//
                            qDebug() << "254*((iii)/5.0)=" << 254*((sectTerm)/5.0);
                        }
                        if(sectTerm == 5){
                            this->progress_searchBar->setValue(254);//
                        }
                        if(sectTerm > 6){
                            buttonTimer->stop();
                            for(int i = 0 ; i < 255; i++){//c230428_1
                                disconnect(sereachConID[i]);
                            }
                            this->progress_searchBar->hide();
                            this->le_search_ip->show();
                            this->btn_refresh_ico->show();
                            sectTerm = 0;
                            this->progress_searchBar->setValue(sectTerm);//
                        }
                        sectTerm++;

                    });
                }
                if(buttonTimer!=nullptr){//c230405
                    buttonTimer->start();
                }
                for(int i = 0 ; i < 255; i++){
                    //QThread::msleep(100);
                    for(quint64 i = 0; i < 3000000; i++){

                    }
                    //this->progress_searchBar->setValue(i);//c230403

                    NetworkHttp *network = new NetworkHttp(this);
                    this->sereachConID[i] = connect(network, SIGNAL(response(int,QJsonObject)), SLOT(responseHttp(int,QJsonObject)));//c211213


                    network->request(HTTP_DEVICE_NAME, QString("http://%1%2:9283/device_name")
                                     .arg(ipPrefix)
                                     .arg(QString::number(i+1)), QJsonObject(), true);
                }


                this->cntWaitingResponse += 255;

print_debug();

                //buttonTimer->start(); // disable button for 1 second


            }
        }
        else{

        }
    }
    else{

        this->showStatusMsg(QString(tr("Looking for a device.")));
        this->clearDeviceList();

        QRegExp rx("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

        if(rx.exactMatch(p_ip)){
            this->flagReqUseIP = true;

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(responseHttp(int,QJsonObject)));

            network->request(HTTP_DEVICE_NAME_IP, QString("http://%1:9283/device_name")
                             .arg(p_ip), QJsonObject(), true);
        }
        else{
            this->showStatusMsg(QString(tr("IP address is incorrect.")));
        }
    }
}


/**
 * @brief IntroRegister::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void SearchDeviceWidget::responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "SearchDeviceWidget::responseHttp---" << strJson;//cheon210831-network
    if(p_id == HTTP_DEVICE_NAME){

        if(!this->flagReqUseIP){
            this->cntResponse++;

            //this->progress_searchBar->setValue(this->cntResponse);//c230403
            this->cntWaitingResponse--;
            this->setResultOfDeviceList(p_jsonObject);

            if(this->cntResponse == 255){

                if(this->vbox_searchDevice->count() == 0){
                    this->showStatusMsg(QString(tr("Device not found.")));
                }
                else{
                    //this->progress_searchBar->setValue(this->cntResponse);//c230403
                    //qDebug() << "[Debug]" << __FUNCTION__ << " line::" << __LINE__ << this->cntResponse;
                }

                //this->progress_searchBar->hide();
                //this->le_search_ip->show();
                //this->btn_refresh_ico->show();
            }
            //c230405
            //this->progress_searchBar->hide();
            //this->le_search_ip->show();
            //this->btn_refresh_ico->show();

        }
    }
    else if(p_id == HTTP_DEVICE_NAME_IP){

        if(this->flagReqUseIP){
            this->setResultOfDeviceList(p_jsonObject);

            if(this->vbox_searchDevice->count() == 0){
                this->showStatusMsg(QString(tr("Device not found.")));
            }

            this->progress_searchBar->hide();
            this->le_search_ip->show();
            this->btn_refresh_ico->show();
        }
    }

    sender()->deleteLater();
}


/**
 * @brief IntroRegister::appendDeviceWidget : Device 선택 UI 세팅
 * @param p_deviceName QString&
 * @param p_deviceIP QString&
 * @param p_deviceType QString&
 */
void SearchDeviceWidget::appendDeviceWidget(const int &p_index, const QString &p_deviceName, const QString &p_deviceIP, const QString &p_deviceType, const QString &p_deviceVersion){

    QString tmp_deviceImg_on = "";
    QString tmp_deviceImg_off = "";
    QString tmp_deviceImg = "";

    if(p_deviceType == "RS130"){//c230427
        tmp_deviceImg_on = ":/images/rs130_on_s.png";
        tmp_deviceImg_off = ":/images/rs130_off_s.png";
    }
    else if(p_deviceType == "RS150" || p_deviceType == "RS150B"){
        tmp_deviceImg_on = ":/images/rs150_on_s.png";
        tmp_deviceImg_off = ":/images/rs150_off_s.png";
    }
    else if(p_deviceType == "RS201"){
        tmp_deviceImg_on = ":/images/rs201_on_s.png";
        tmp_deviceImg_off = ":/images/rs201_off_s.png";
    }
    else if(p_deviceType == "RS250" || p_deviceType == "RS250A"){
        tmp_deviceImg_on = ":/images/rs250_on_s.png";
        tmp_deviceImg_off = ":/images/rs250_off_s.png";
    }
    else if(p_deviceType == "RS520"){
        tmp_deviceImg_on = ":/images/rs520_on_s.png";
        tmp_deviceImg_off = ":/images/rs520_off_s.png";
    }

    //if(global.device.getDeviceIP() == p_deviceIP && global.flagConnected){
    if(global.device.getDeviceIP() == p_deviceIP){
        tmp_deviceImg = tmp_deviceImg_on;
        this->selectedIndex = p_index;
    }
    else{
        tmp_deviceImg = tmp_deviceImg_off;
    }

    ClickableWidget *widget_device_info = new ClickableWidget();
    widget_device_info->setObjectName("widget_device_info");
    widget_device_info->setProperty("index", p_index);
    widget_device_info->setFixedSize(398, 125);
    widget_device_info->setCursor(Qt::PointingHandCursor);
    widget_device_info->setStyleSheet(global.device.getDeviceIP() == p_deviceIP ?
                                          "#widget_device_info {border-top:2px solid #4d4d4d;} #widget_device_info:hover{background-color:#444444;}" :
                                          "#widget_device_info {border-top:2px solid #4d4d4d; background-color:transparent;} #widget_device_info:hover{background-color:#444444;}");

    QLabel *lb_device_img = GSCommon::getUILabelImg(tmp_deviceImg, widget_device_info);
    lb_device_img->setObjectName("deviceImg");
    lb_device_img->setFixedSize(200, 65);
    lb_device_img->setGeometry(15, 30, 0, 0);
    lb_device_img->setStyleSheet("background-color:transparent;");
    lb_device_img->setScaledContents(true);

    QLabel *lb_device_name = new QLabel(widget_device_info);
    lb_device_name->setFixedSize(140, 26);
    lb_device_name->setGeometry(235, 22, 0, 0);
    lb_device_name->setStyleSheet("background-color:transparent; font-size:20px; color:#E6E6E6");
    lb_device_name->setAlignment(Qt::AlignLeft);
    lb_device_name->setText(p_deviceName);

    QLabel *lb_device_type = new QLabel(widget_device_info);
    lb_device_type->setObjectName("deviceType");
    lb_device_type->setFixedSize(140, 17);
    lb_device_type->setGeometry(235, 50, 0, 0);
    lb_device_type->setStyleSheet("background-color:transparent; font-size:15px; color:#919191");
    lb_device_type->setAlignment(Qt::AlignLeft);
    lb_device_type->setText(p_deviceType);

    QLabel *lb_device_version = new QLabel(widget_device_info);
    lb_device_version->setFixedSize(140, 17);
    lb_device_version->setGeometry(235, 70, 0, 0);
    lb_device_version->setStyleSheet("background-color:transparent; font-size:15px; color:#919191");
    lb_device_version->setAlignment(Qt::AlignLeft);
    lb_device_version->setText("Rose OS " + p_deviceVersion);

    QLabel *lb_device_ip = new QLabel(widget_device_info);
    lb_device_ip->setFixedSize(140, 17);
    lb_device_ip->setGeometry(235, 90, 0, 0);
    lb_device_ip->setStyleSheet("background-color:transparent; font-size:15px; color:#919191");
    lb_device_ip->setAlignment(Qt::AlignLeft);
    lb_device_ip->setText(p_deviceIP);

    QLabel *lb_device_check = GSCommon::getUILabelImg(":/images/rose_check.png", widget_device_info);
    lb_device_check->setObjectName("deviceCheck");
    lb_device_check->setFixedSize(40, 40);
    lb_device_check->setGeometry(358, 0, 0, 0);
    lb_device_check->setStyleSheet("background-color:transparent;");

    if(global.device.getDeviceIP() == p_deviceIP){
        lb_device_check->show();
    }
    else{
        lb_device_check->hide();
    }

    this->vbox_searchDevice->addWidget(widget_device_info);
    this->vbox_searchDevice->parentWidget()->setFixedSize(398, 125 * this->vbox_searchDevice->count());

    this->stackWidget_search->setCurrentIndex(1);

    connect(widget_device_info, SIGNAL(clickedWidget()), SLOT(slot_selectedDevice()));
}


/**
 * @brief IntroRegister::showNotFoundText : 기기를 찾을 수 없다는 라벨 Show
 */
void SearchDeviceWidget::showStatusMsg(QString p_msg){

    this->lb_search_status->setText(p_msg);
    this->stackWidget_search->setCurrentIndex(0);
}


/**
 * @brief IntroRegister::setResultOfDeviceList : Device Data 세팅
 * @param p_jsonObject QJsonObject& Device 정보
 */
void SearchDeviceWidget::setResultOfDeviceList(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_data = "data";
    const QString jsonKey_deviceType = "deviceType";

    QString p_deviceType = global.device.getDeviceType();

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){

        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by diskept
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
            if(p_jsonObject.contains(jsonKey_data)){
                QJsonObject jsonData = p_jsonObject[jsonKey_data].toObject();

                DataDevice *data = new DataDevice(this);

                data->setData(jsonData);

                //qDebug() << "data->getDeviceType() : " << data->getDeviceType();
                if(data->getDeviceType() == "RS350"  || data->getDeviceType() == "RS301" ){//rs301, rs350_skip

                }
                else{

                    bool flagHasAlready = false;

                    for(int i = 0; i < this->list_device.size(); i++){
                        if(this->list_device.at(i)->getDeviceIP() == data->getDeviceIP()){
                            flagHasAlready = true;
                            break;
                        }
                    }

                    if(!flagHasAlready){
                        this->list_device.append(data);
                        this->appendDeviceWidget(this->list_device.size()-1, data->getDeviceName(), data->getDeviceIP(), data->getDeviceType(), data->getDeviceVersion());
                    }
                }

            }
            //}
        }
    }

}


/**
 * @brief IntroRegister::selectedDevice : [슬롯] 장치 선택
 */
void SearchDeviceWidget::slot_selectedDevice(){//c220625

    print_debug();

    int tmp_index = sender()->property("index").toInt();

    if(this->selectedIndex >= 0){
        print_debug();

        QLabel *lb_img = this->vbox_searchDevice->itemAt(selectedIndex)->widget()->findChild<QLabel*>("deviceImg");
        QLabel *lb_type = this->vbox_searchDevice->itemAt(selectedIndex)->widget()->findChild<QLabel*>("deviceType");
        QLabel *lb_check = this->vbox_searchDevice->itemAt(selectedIndex)->widget()->findChild<QLabel*>("deviceCheck");

        if(lb_type->text()=="RS130"){//c230427
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs130_off_s.png"));
        }
        else if(lb_type->text()=="RS150" || lb_type->text() == "RS150B"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs150_off_s.png"));
        }
        if(lb_type->text()=="RS201"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs201_off_s.png"));
        }
        else if(lb_type->text()=="RS250" || lb_type->text()=="RS250A"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs250_off_s.png"));
        }
        else if(lb_type->text()=="RS520"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs520_off_s.png"));
        }

        lb_check->hide();
        //emit signal_selectedDevice();//c220623
    }

    // 체크 상태 변경 : 기기 이미지 변경
    QLabel *lb_img = sender()->findChild<QLabel*>("deviceImg");
    QLabel *lb_type = sender()->findChild<QLabel*>("deviceType");
    QLabel *lb_check = sender()->findChild<QLabel*>("deviceCheck");

    if(lb_type->text()=="RS130"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs130_on_s.png"));
    }
    else if(lb_type->text()=="RS150" || lb_type->text() == "RS150B"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs150_on_s.png"));
    }
    else if(lb_type->text()=="RS201"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs201_on_s.png"));
    }
    else if(lb_type->text()=="RS250" || lb_type->text() == "RS250A"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs250_on_s.png"));
    }
    else if(lb_type->text()=="RS520"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs520_on_s.png"));
    }

    // 체크 상태 변경 : 체크 이미지 show
    lb_check->show();

    global.signal_selectedDevice_flag = false;//c220319

    // 선택된 기기가 다른 경우에만 시그널 발생
    if(this->selectedIndex != tmp_index){
        QString selectedIP = list_device.at(tmp_index)->getDeviceIP();

        global.signal_selectedDevice_flag = true;//c220319
        emit linker->signal_searchBarFocusChanged(false);//c220804

        DialogConfirm *selectedDevice_dlgConfirm = new DialogConfirm(this);//c220625
        print_debug();
        selectedDevice_dlgConfirm->setAlertMode();
        //selectedDevice_dlgConfirm->setProperty("flagShown", false);
        selectedDevice_dlgConfirm->setTitle(tr("Selected device notice"));
        selectedDevice_dlgConfirm->setText(QString(tr("%1: %2\n\nA different device was selected for changing the rose device. \nAre you sure you want to change it?")).arg(list_device.at(tmp_index)->getDeviceName()).arg(list_device.at(tmp_index)->getDeviceIP()));//c220609
        selectedDevice_dlgConfirm->setTextHeight(200);

        int result = selectedDevice_dlgConfirm->exec();
        print_debug();
        selectedDevice_dlgConfirm->raise();
        if(result == QDialog::Accepted){


            emit signal_selectedDevice(selectedIP);
            print_debug();//c230416
            emit linker->signal_connected();//c220901_1
            this->selectedIndex = tmp_index;

            if(this->flag_adjust == true){
                this->flag_adjust = false;
                this->slot_searchWidget_adjust();
            }

            QTimer::singleShot(4000,this,[=](){emit linker->signal_checknetwork();});//c230424
            //QTimer::singleShot(4000,this,[=](){emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));});//c230423
            //emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::RoseHome));//c230423
            //ToastMsg::show(this,"", tr("DB is downloading from music folder."), 2000, 0);//c230423

        }else if(result == QDialog::Rejected){
            lb_check->hide();
            print_debug();
        }

    }
    sender()->deleteLater();//c230423
}


/**
 * @brief IntroRegister::searchDeviceIP : [슬롯] 새로고침 클릭
 * @details IP 값을 이용해 연결가능한 장치 정보를 가져온다.
 */
void SearchDeviceWidget::searchDeviceIP(){
    ToastMsg::delay(this,"", tr("delay"), 4000);//c230403
    print_debug();
    this->searchDevice(this->le_search_ip->text());
}


/**
 * @brief SearchDeviceWidget::flagSelectedDevice : 기기 선택 상태값 반환
 * @return bool, if true 선택됨.
 */
bool SearchDeviceWidget::getFlagSelected(){

    if(this->selectedIndex >= 0){
        return true;
    }

    return false;
}


/**
 * @brief SearchDeviceWidget::getFlagHasDeviceList : 기기 목록 보유 여부
 * @return bool if true, has list
 */
bool SearchDeviceWidget::getFlagHasDeviceList(){

    if(this->vbox_searchDevice->count() > 0){
        return true;
    }

    return false;
}


/**
 * @brief SearchDeviceWidget::saveCurrDeviceData : 기기 정보 DB 저장
 */
bool SearchDeviceWidget::saveCurrDeviceData(){

    if(this->selectedIndex >= 0){
        DataDevice *device = this->list_device.at(this->selectedIndex);

        SqliteHelper *sqliteHelper = new SqliteHelper(this);
        QSqlError err = sqliteHelper->addConnectionLocal();

        if(err.type() == QSqlError::NoError){

            QString strQuery_create = "CREATE TABLE IF NOT EXISTS Device (";
            strQuery_create += "deviceID VARCHAR(20), deviceIP VARCHAR(20), deviceName VARCHAR(20), deviceRoseToken VARCHAR(30), deviceType VARCHAR(20),";
            strQuery_create += "deviceVersion VARCHAR(20), isDbScanning integer, musicPlaytype integer, dbFileSize integer)";

            sqliteHelper->exec(strQuery_create);

            QString strQuery_delete = "DELETE FROM Device";
            sqliteHelper->exec(strQuery_delete);

            QString strQuery_insert = "INSERT INTO Device (deviceID, deviceIP, deviceName, deviceRoseToken, deviceType, deviceVersion, isDBScanning, musicPlaytype, dbFileSize) ";
            strQuery_insert += " VALUES ";
            strQuery_insert += " ('"+device->getDeviceID()+"','"+device->getDeviceIP()+"','"+device->getDeviceName()+"',";
            strQuery_insert += "  '"+device->getDeviceRoseToken()+"','"+device->getDeviceType()+"','"+device->getDeviceVersion()+"',";
            strQuery_insert += "  "+QString::number(device->getIsDbScanning())+","+QString::number(device->getMusicPlayType())+",";
            strQuery_insert += "  "+QString::number(device->getDbFileSize());
            strQuery_insert += "  )";

            sqliteHelper->exec(strQuery_insert);
        }

        sqliteHelper->close();
        delete sqliteHelper;

        print_debug();

        return true;
    }

    return false;
}
