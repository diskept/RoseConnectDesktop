#include "searchdevicewidget.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"
#include "widget/VerticalScrollArea.h"
#include "widget/clickablewidget.h"
#include "common/global.h"

#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRegExpValidator>
#include <QProgressDialog>

const int MAXIMUM_REQ_COUNT = 255;
const int HTTP_DEVICE_NAME = 99;
const int HTTP_DEVICE_NAME_IP = 98;
const int MAX_CNT = 255;
int forlast = 255;
/**
 * @brief SearchDeviceWidget::SearchDeviceWidget : 로즈 기기 검색 Widget
 * @param parent
 * @note 아이피 지정 검색, 로컬 아이피 255 전체 검색 지원
 * @note RS150, RS201 모델만 지원
 */
SearchDeviceWidget::SearchDeviceWidget(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

SearchDeviceWidget::~SearchDeviceWidget(){
    list_device.clear();
    list_checkImg.clear();

    delete findIp;
}

/**
 * @brief SearchDeviceWidget::setInit : 초기 세팅
 */
void SearchDeviceWidget::setInit(){
    selectedIndex = -1;
    flagReqUseIP = false;
    cntResponse = 0;
    cntWaitingResponse = 0;
    findIp = new FindIP;

}

/**
 * @brief SearchDeviceWidget::setUIControl : UI 세팅
 */
void SearchDeviceWidget::setUIControl(){
    // 라벨
    QLabel *lb_device = new QLabel;
    lb_device->setText(tr("Connectable Device"));
    //lb_device->setText(tr("연결 가능한 기기"));
    lb_device->setStyleSheet("font-size:16px;color:#FFFFFF;background-color:#4d4d4d;border:2px solid #4d4d4d;padding-left:10px;");
    //lb_device->setFixedHeight(50);  //cheon210812-remote
    lb_device->setFixedHeight(45);  //cheon210812-remote

    searchProgressBar->setStyleSheet("QProgressBar {font-size:12px;color:#FFFFFF;background-color:#4d4d4d;border:2px solid #666666;padding-top:2px;padding-right:0px;} QProgressBar::chunk{ background-color:#B18658; }");//cheon05_progress_bar
    searchProgressBar->setFixedHeight(20);
    searchProgressBar->setAlignment(Qt::AlignVCenter);

    searchProgressBar->setMaximum(254);         // 최초 최대값 설정-> 자동으로 진행 상항을 퍼센트로 보여준다.
    searchProgressBar->setValue(0);             // 현재 QProgressBar위젯의 설정값을 할당할 수 있다.


    // -------------------------------------------------
    // Search Bar
    // -------------------------------------------------
    QPushButton *btn_search = GSCommon::getUIBtnImg("btn_search", ":/images/icon_ser_w.png");
    btn_search->setFixedSize(QSize(50,50));

    le_ip = new QLineEdit;
    le_ip->setMaxLength(15);
    le_ip->setValidator(new QRegExpValidator(QRegExp("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"), le_ip));
    le_ip->setPlaceholderText(tr("Search IP"));
    //le_ip->setPlaceholderText(tr("IP검색"));
    le_ip->setStyleSheet("font-size:16px;color:#FFFFFF;border:0px;margin-bottom:2px;background-color:transparent");
    le_ip->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QPushButton *btn_refresh = GSCommon::getUIBtnImg("btn_refresh", ":/images/icon_refresh.png");
    btn_refresh->setFixedSize(QSize(50,50));
    btn_refresh->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *box_search = new QHBoxLayout;
    box_search->setSpacing(0);
    box_search->setContentsMargins(0,0,0,0);
    box_search->addWidget(btn_search);
    box_search->addWidget(le_ip);
    box_search->addWidget(btn_refresh);

    QWidget *wg_searchGroup = new QWidget;
    wg_searchGroup->setObjectName("wg_searchGroup");
    wg_searchGroup->setLayout(box_search);
    wg_searchGroup->setStyleSheet("#wg_searchGroup {border-bottom:2px solid #4d4d4d;}");


    // -------------------------------------------------
    // 조회 결과 Device 선택 박스
    // -------------------------------------------------
    boxDeviceList = new QVBoxLayout;
    boxDeviceList->setContentsMargins(0,0,0,0);
    boxDeviceList->setSpacing(0);

    QWidget *widgetBoxList= new QWidget;
    widgetBoxList->setLayout(boxDeviceList);
    //widgetBoxList->setMinimumHeight(215);//cheon210812-remote
    widgetBoxList->setMinimumHeight(400);//c220623
    widgetBoxList->setStyleSheet("background-color:transparent");

    VerticalScrollArea *scrollArea = new VerticalScrollArea;
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widgetBoxList);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea {border:0px;border-bottom-left-radius:5px;border-bottom-right-radius:5px;}");


    // -------------------------------------------------
    //  검색진행 TEXT
    // -------------------------------------------------
    lb_status = new QLabel();
    lb_status->setText(tr("Click the search icon to search the device IP."));       //cheon04_src
    //lb_status->setText(tr("Looking for a device."));
    //lb_status->setText(tr("기기를 찾는중입니다."));
    lb_status->setAlignment(Qt::AlignCenter);
    lb_status->setStyleSheet("font-size:16px;color:#FFFFFF;");
    //lb_status->setGeometry(0,0,400,250);//cheon210812-remote
    lb_status->setGeometry(0,0,400,215);//cheon210812-remote


    // -------------------------------------------------
    //  검색진행 & 검색결과박스 스택
    // -------------------------------------------------
    stackDeviceList = new QStackedWidget;
    stackDeviceList->addWidget(lb_status);
    stackDeviceList->addWidget(scrollArea);
    //stackDeviceList->show();


    // -------------------------------------------------
    // Total 박스 구성 및 세팅
    // -------------------------------------------------
    QVBoxLayout *box_searchTotal = new QVBoxLayout;
    box_searchTotal->setSpacing(0);
    box_searchTotal->setContentsMargins(2,2,2,2);
    box_searchTotal->addWidget(lb_device);
    box_searchTotal->addWidget(wg_searchGroup);
    box_searchTotal->addWidget(searchProgressBar);      //cheon05_progress
    box_searchTotal->addWidget(stackDeviceList);


    QWidget *widgetSearchTotal = new QWidget;
    widgetSearchTotal->setContentsMargins(0,0,0,0);
    widgetSearchTotal->setObjectName("widgetSearchTotal");
    widgetSearchTotal->setLayout(box_searchTotal);
    widgetSearchTotal->setFixedWidth(410);//cheon210812-remote
    widgetSearchTotal->setStyleSheet("#widgetSearchTotal {border:2px solid #4d4d4d;border-radius:5px}");


    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setSpacing(0);
    boxMain->setContentsMargins(0,0,0,0);
    boxMain->addWidget(widgetSearchTotal);

    this->setLayout(boxMain);

    // connect
    connect(btn_refresh, SIGNAL(clicked()), SLOT(searchDeviceIP()));
    connect(le_ip, SIGNAL(returnPressed()), btn_refresh, SIGNAL(clicked()));

}

/**
 * @brief SearchDeviceWidget::clearDeviceList : 장치 목록 clear
 * @details 장치목록 UI 및 관련 데이터 초기화
 */
void SearchDeviceWidget::clearDeviceList(){
    selectedIndex = -1;
    list_device.clear();
    list_checkImg.clear();
    cntResponse = 0;
    GSCommon::clearLayout(boxDeviceList);
}

/**
 * @brief IntroRegister::searchDevice : 로즈 장치 검색
 * @details 사용자가 입력한 아이피가 존재한다면 해당 아이피만 검색
 * 사용자가 입력한 아이피가 없다면 내 아이피 확인 후 전체 검색
 * 전체 검색의 경우 요청수를 제한한다. 응답 대기중인 수가 250건 이상이면 요청 불가
 * @param p_ip
 */
void SearchDeviceWidget::searchDevice(QString p_ip){


    print_debug();
    cntWaitingResponse = 0;//c211213
    if(p_ip==""){   // 전체 검색
        flagReqUseIP = false;

        if(cntWaitingResponse <= MAXIMUM_REQ_COUNT){
            showStatusMsg(QString(tr("Looking for a device.")));
            //showStatusMsg(QString(tr("기기를 찾는중입니다.")));
            clearDeviceList();

            QString ipPrefix;
            QStringList list_ipNumber = QString(findIp->getMyIP()).split(".");

            if(list_ipNumber.size()==4){
                ipPrefix = QString("%1.%2.%3.")
                        .arg(list_ipNumber.at(0))
                        .arg(list_ipNumber.at(1))
                        .arg(list_ipNumber.at(2));
                for(int i = 0 ; i < 255; i++){//c211213
                    disconnect(sereachConID[i]);
                }
                print_debug();
                for(int i = 0 ; i < 255; i++){
                    //QThread::msleep(10);
                    searchProgressBar->setValue(i);
                    NetworkHttp *network = new NetworkHttp(this);
                    sereachConID[i] = connect(network, SIGNAL(response(int, QJsonObject)), SLOT(responseHttp(int, QJsonObject)));//c211213


                    network->request(HTTP_DEVICE_NAME, QString("http://%1%2:9283/device_name")
                                     .arg(ipPrefix)
                                     .arg(QString::number(i+1)), QJsonObject(), true);
                }
                cntWaitingResponse += 255;
            }
        }else{
        }
    }else{
        showStatusMsg(QString(tr("Looking for a device.")));
        //showStatusMsg(QString(tr("기기를 찾는중입니다.")));
        clearDeviceList();

        QRegExp rx("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        if(rx.exactMatch(p_ip)){
            flagReqUseIP = true;

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int, QJsonObject)), SLOT(responseHttp(int, QJsonObject)));

            network->request(HTTP_DEVICE_NAME_IP, QString("http://%1:9283/device_name")
                             .arg(p_ip), QJsonObject(), true);
        }else{
            showStatusMsg(QString(tr("IP address is incorrect.")));
            //showStatusMsg(QString(tr("IP주소가 올바르지 않습니다.")));
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

        if(!flagReqUseIP){
            cntResponse++;
            //qDebug() << "------------------------ cntResponse : " << cntResponse;       //cheon05_progress_bar

            cntWaitingResponse--;
            setResultOfDeviceList(p_jsonObject);

            if(cntResponse==255){

                if(boxDeviceList->count()==0){
                    showStatusMsg(QString(tr("Device not found.")));
                    //showStatusMsg(QString(tr("기기를 찾을 수 없습니다.")));
                }else{
                    searchProgressBar->setValue(cntResponse);
                }
            }
        }
    }else if(p_id == HTTP_DEVICE_NAME_IP){

        if(flagReqUseIP){
            setResultOfDeviceList(p_jsonObject);

            if(boxDeviceList->count()==0){
                showStatusMsg(QString(tr("Device not found.")));
                //showStatusMsg(QString(tr("기기를 찾을 수 없습니다.")));
            }
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
void SearchDeviceWidget::appendDeviceWidget(const int &p_index, const QString &p_deviceName, const QString &p_deviceIP, const QString &p_deviceType){

    stackDeviceList->setCurrentIndex(1);

    QLabel *lb_img = new QLabel;
    lb_img->setObjectName("deviceImg");
    lb_img->setScaledContents(true);
    lb_img->setAlignment(Qt::AlignCenter);
    lb_img->setStyleSheet("background-color:transparent;");
    lb_img->setContentsMargins(5,5,5,5);
    lb_img->setFixedSize(QSize(200,65));

    QLabel *lb_name = new QLabel;
    lb_name->setText(p_deviceName);
    lb_name->setStyleSheet("font-size:20px;color:#FFFFFF");

    QLabel *lb_type = new QLabel;
    lb_type->setObjectName("deviceType");
    lb_type->setText(p_deviceType);
    lb_type->setStyleSheet("font-size:15px;color:#919191");

    QLabel *lb_ip = new QLabel;
    lb_ip->setText(p_deviceIP);
    lb_ip->setStyleSheet("font-size:15px;color:#919191");

    QVBoxLayout *box_info = new QVBoxLayout;
    box_info->setSpacing(5);
    box_info->addStretch(1);
    box_info->addWidget(lb_name,0);
    box_info->addWidget(lb_type,0);
    box_info->addWidget(lb_ip,0);
    box_info->addStretch(1);

    QWidget *wg_info = new QWidget;
    wg_info->setLayout(box_info);
    wg_info->setStyleSheet("background-color:transparent;");

    QHBoxLayout *box_total = new QHBoxLayout;
    box_total->setSpacing(0);
    box_total->setContentsMargins(0,0,0,0);
    box_total->addWidget(lb_img,6, Qt::AlignVCenter);
    box_total->addWidget(wg_info,5);

    ClickableWidget *wg_total = new ClickableWidget;
    wg_total->setObjectName("wg_deviceTotal");
    wg_total->setLayout(box_total);
    wg_total->setCursor(Qt::PointingHandCursor);
    wg_total->setProperty("index", p_index);

    QLabel *lb_check = new QLabel(wg_total);
    lb_check->setObjectName("lb_check");
    lb_check->setPixmap(*GSCommon::getUIPixmapImg(":/images/rose_check.png"));
    lb_check->setGeometry(360,0,40,40);
    lb_check->setStyleSheet("background-color:transparent;");


    QString tmp_deviceImg_on = ":/images/rs201_on_s.png";
    QString tmp_deviceImg_off = ":/images/rs201_off_s.png";

    if(p_deviceType == "RS150" || p_deviceType == "RS150B"){
        tmp_deviceImg_on = ":/images/rs150_on_s.png";
        tmp_deviceImg_off = ":/images/rs150_off_s.png";
    }

    if(p_deviceType == "RS250"){
        tmp_deviceImg_on = ":/images/rs250_on_s.png";
        tmp_deviceImg_off = ":/images/rs250_off_s.png";
    }

    if(global.device.getDeviceIP() == p_deviceIP && global.flagConnected){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(tmp_deviceImg_on));
        lb_check->show();

        selectedIndex = p_index;
    }else{
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(tmp_deviceImg_off));
        lb_check->hide();
    }
    list_checkImg.append(lb_check);



    if(boxDeviceList->count() > 0){
        wg_total->setStyleSheet("#wg_deviceTotal {border-top:2px solid #4d4d4d;} #wg_deviceTotal:hover{background-color:#444444;}");
    }else{
        wg_total->setStyleSheet("#wg_deviceTotal:hover{background-color:#444444;}");
    }

    boxDeviceList->addWidget(wg_total);
    boxDeviceList->parentWidget()->setFixedSize(400,125*boxDeviceList->count());



    connect(wg_total, SIGNAL(clickedWidget()), SLOT(slot_selectedDevice()));
}

/**
 * @brief IntroRegister::showNotFoundText : 기기를 찾을 수 없다는 라벨 Show
 */
void SearchDeviceWidget::showStatusMsg(QString p_msg){
    lb_status->setText(p_msg);
    stackDeviceList->setCurrentIndex(0);
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
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
            if(p_jsonObject.contains(jsonKey_data)){
                QJsonObject jsonData = p_jsonObject[jsonKey_data].toObject();

                DataDevice *data = new DataDevice(this);

                data->setData(jsonData);

                //qDebug() << "data->getDeviceType() : " << data->getDeviceType();
                if(data->getDeviceType() == "RS350"  || data->getDeviceType() == "RS301" ){//rs301, rs350_skip
                    ;
                }else{

                    bool flagHasAlready = false;
                    for(int i = 0; i < list_device.size(); i++){
                        if(list_device.at(i)->getDeviceIP() == data->getDeviceIP()){
                            flagHasAlready = true;
                            break;
                        }
                    }

                    if(!flagHasAlready){
                        list_device.append(data);
                        appendDeviceWidget(list_device.size()-1, data->getDeviceName(), data->getDeviceIP(), data->getDeviceType());
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
void SearchDeviceWidget::slot_selectedDevice(){

    int tmp_index = sender()->property("index").toInt();

    if(selectedIndex >=0){
        print_debug();
        emit signal_selectedDevice();//c220623
        QLabel *lb_img = boxDeviceList->itemAt(selectedIndex)->widget()->findChild<QLabel*>("deviceImg");
        QLabel *lb_type = boxDeviceList->itemAt(selectedIndex)->widget()->findChild<QLabel*>("deviceType");

        if(lb_type->text()=="RS201"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs201_off_s.png"));

        }else if(lb_type->text()=="RS150" || lb_type->text() == "RS150B"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs150_off_s.png"));
        }else if(lb_type->text()=="RS250"){
            lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs250_off_s.png"));
        }

        QLabel *lb_check = list_checkImg.at(selectedIndex);
        lb_check->hide();

    }

    // 체크 상태 변경 : 기기 이미지 변경
    QLabel *lb_img = sender()->findChild<QLabel*>("deviceImg");
    QLabel *lb_type = sender()->findChild<QLabel*>("deviceType");

    if(lb_type->text()=="RS201"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs201_on_s.png"));
    }else if(lb_type->text()=="RS150" || lb_type->text() == "RS150B"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs150_on_s.png"));
    }else if(lb_type->text()=="RS250"){
        lb_img->setPixmap(*GSCommon::getUIPixmapImg(":/images/rs250_on_s.png"));
    }

    // 체크 상태 변경 : 체크 이미지 show
    QLabel *lb_check = list_checkImg.at(tmp_index);
    lb_check->show();

    global.signal_selectedDevice_flag = false;//c220319
    // 선택된 기기가 다른 경우에만 시그널 발생
    if(selectedIndex != tmp_index){
        QString selectedIP = list_device.at(tmp_index)->getDeviceIP();
        global.signal_selectedDevice_flag = true;//c220319
        emit signal_selectedDevice(selectedIP);

    }

    selectedIndex = tmp_index;

}

/**
 * @brief IntroRegister::searchDeviceIP : [슬롯] 새로고침 클릭
 * @details IP 값을 이용해 연결가능한 장치 정보를 가져온다.
 */
void SearchDeviceWidget::searchDeviceIP(){
    print_debug();
    emit signal_searchDeviceBtnClick();//c220623
    print_debug();
    searchDevice(le_ip->text());
}

/**
 * @brief SearchDeviceWidget::flagSelectedDevice : 기기 선택 상태값 반환
 * @return bool, if true 선택됨.
 */
bool SearchDeviceWidget::getFlagSelected(){
    if(selectedIndex >=0){
        return true;
    }

    return false;
}

/**
 * @brief SearchDeviceWidget::getFlagHasDeviceList : 기기 목록 보유 여부
 * @return bool if true, has list
 */
bool SearchDeviceWidget::getFlagHasDeviceList(){
    if(boxDeviceList->count() > 0){
        return true;
    }
    return false;
}

/**
 * @brief SearchDeviceWidget::saveCurrDeviceData : 기기 정보 DB 저장
 */
bool SearchDeviceWidget::saveCurrDeviceData(){

    if(selectedIndex >=0){
        DataDevice *device = list_device.at(selectedIndex);

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

        return true;
    }

    return false;
}

