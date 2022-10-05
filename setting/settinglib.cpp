#include "settinglib.h"

#include <common/gscommon.h>
#include "common/networkhttp.h"
#include "common/global.h"
#include "login/dialog_radio.h"
#include "widget/dialogconfirm.h"

#include <QScrollArea>

const int HTTP_NETWORK_MEDIA_LIBRARY = 99;
const int HTTP_NETWORK_MEDIA_ADD_DEL = 98;
const int HTTP_NETWORK_DB_INIT = 97;

/**
 * @brief SettingLib::SettingLib : 설정-미디어라이브러리
 * @param parent
 * @note 미디어라이브러리 DB 추가 삭제, 초기화, 재스캔 등의 미디어라이브러리 관리 페이지
 */
SettingLib::SettingLib(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief SettingLib::~SettingLib : 소멸자
 */
SettingLib::~SettingLib(){
    clearMediaList();
}

/**
 * @brief SettingLib::setInit : 초기 세팅 함수
 */
void SettingLib::setInit(){
    linker = Linker::getInstance();
    listMedia = new QList<Data_Media_Library*>();
}

/**
 * @brief SettingLib::setUIControl : UI 세팅 함수
 */
void SettingLib::setUIControl()
{
    // 미디어 라이브러리 목록
    box_mediaLibrary = new QVBoxLayout();
    box_mediaLibrary->setContentsMargins(0,0,0,0);
    box_mediaLibrary->setSpacing(0);
    box_mediaLibrary->setAlignment(Qt::AlignTop);

    // 전체
    QVBoxLayout *vl_contents = new QVBoxLayout();
    vl_contents->setContentsMargins(0,0,0,0);
    vl_contents->setSpacing(0);
    vl_contents->setAlignment(Qt::AlignTop);
    vl_contents->addWidget(GSCommon::getUIControlTitleForSetting(tr("Media Library Settings")));
    vl_contents->addWidget(getUIControlOption(0,tr("Get the database again"), ":images/play_refre_icon.png", true));
    vl_contents->addWidget(getUIControlOption(1, tr("DB reset"), tr("reset")));
    vl_contents->addWidget(getUIControlOption(2, tr("Add/Delete"), tr("GO")));
    vl_contents->addSpacing(70);
    vl_contents->addWidget(GSCommon::getUIControlTitleForSetting(tr("Library")));
    vl_contents->addLayout(box_mediaLibrary);

    /*
     *     vl_contents->addWidget(GSCommon::getUIControlTitleForSetting(tr("미디어 라이브러리 설정")));
    vl_contents->addWidget(getUIControlOption(0,tr("데이터베이스 다시 받아오기"), ":images/play_refre_icon.png", true));
    vl_contents->addWidget(getUIControlOption(1, tr("DB 초기화"), tr("초기화")));
    vl_contents->addWidget(getUIControlOption(2, tr("추가/삭제"), "GO"));
    vl_contents->addSpacing(70);
    vl_contents->addWidget(GSCommon::getUIControlTitleForSetting(tr("라이브러리")));
    vl_contents->addLayout(box_mediaLibrary);
    */

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setObjectName("widget_scrollArea");
    widget_scrollArea->setLayout(vl_contents);
    widget_scrollArea->setStyleSheet("#widget_scrollArea { border-left:1px solid #303030; }");

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);
}

/**
 * @brief SettingLib::clearMediaList : QList clear
 */
void SettingLib::clearMediaList(){
    if(listMedia->size() > 0){
        qDeleteAll(listMedia->begin(), listMedia->end());
        listMedia->clear();

        GSCommon::clearLayout(box_mediaLibrary);
    }
}

/**
 * @brief SettingLib::getUIControlOption
 * @param p_title
 * @return
 */
QWidget* SettingLib::getUIControlOption(int p_index, QString p_title, QString p_btn, bool flagIsImageBtn){

    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    QPushButton *btn = new QPushButton();
    if(flagIsImageBtn){
        QPixmap pixmap(p_btn);
        btn->setIcon(pixmap);
        btn->setIconSize(pixmap.rect().size());
    }else{
        btn->setText(p_btn);
        btn->setFixedSize(74,34);
        btn->setStyleSheet("border:2px solid #666666;border-radius:17px;color:#E6E6E6;font-size:15px;");
    }
    btn->setContentsMargins(0,0,0,0);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setProperty("index", p_index);

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,0,47,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(btn, 0, Qt::AlignRight);

    QWidget *widget_total = new QWidget();
    widget_total->setContentsMargins(0,0,0,0);
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setFixedHeight(70);

    widget_total->setLayout(hl_lb);

    // 커넥션
    connect(btn, SIGNAL(clicked()), this, SLOT(slot_clickedBtn()));
    return widget_total;
}

/**
 * @brief SettingLib::getMedaiNamePath : 미디어 목록 타이틀(name, path 구성) 반환
 * @param p_name QString media library name (ex ip 192.168.10.2)
 * @param p_path QString media library path (ex path share/music1)
 * @return
 * @note API에서 name, path 가 동일한 값을 반환하는 경우 있음<br>
 * App 처럼 smb 프로토콜 제외 하 name만 사용하는것으로 처리<br>
 */
QString SettingLib::getMedaiNamePath(bool p_flagIsServer, QString p_name, QString p_path){
    QString tmp_namePath;
    if(p_name != p_path && p_flagIsServer){
        tmp_namePath = "smb:"+p_name+"/"+p_path;
    }else{
        tmp_namePath = p_path;
    }

    return tmp_namePath;
}

/**
 * @brief SettingLib::getUIControlMediaLibrary : 미디어 라이브러리 목록 UI 세팅
 * @param p_index
 * @return
 */
QWidget* SettingLib::getUIControlMediaLibrary(const int &p_index){

    QWidget *widget_total = new QWidget();

    if(p_index >=0 && p_index < listMedia->count()){
        QString tmp_titleStyle;
        if(listMedia->at(p_index)->getDbStatus() >=0){
            tmp_titleStyle = "color:#E5E5E4;font-size:18px;";
        }else{
            tmp_titleStyle = "color:#666666;font-size:18px;";
        }

        // name, path
        QString tmp_path = getMedaiNamePath(listMedia->at(p_index)->getIsServer(), listMedia->at(p_index)->getName(), listMedia->at(p_index)->getPath());

        QLabel *lb_path = new QLabel();
        lb_path->setText(GSCommon::getTextCutFromLabelWidth(tmp_path, this->width()-155, lb_path->font()));
        lb_path->setStyleSheet(tmp_titleStyle);

        QLabel *lb_dbStatus = new QLabel(listMedia->at(p_index)->getStrDbStatus());
        lb_dbStatus->setStyleSheet("color:#666666;font-size:17px;");

        QVBoxLayout *box_info = new QVBoxLayout();
        box_info->setContentsMargins(0,0,0,0);
        box_info->setAlignment(Qt::AlignVCenter);
        box_info->setSpacing(0);
        box_info->addWidget(lb_path);
        box_info->addSpacing(7);
        box_info->addWidget(lb_dbStatus);

        QPushButton *btn_more = GSCommon::getUIBtnImg("btn_more", ":images/icon_menu.png");
        btn_more->setProperty("index", p_index);
        btn_more->setCursor(Qt::PointingHandCursor);

        QHBoxLayout *hl_lb = new QHBoxLayout();
        hl_lb->setContentsMargins(33,0,55,0);
        hl_lb->setSpacing(40);
        hl_lb->addLayout(box_info, Qt::AlignVCenter);
        hl_lb->addWidget(btn_more,0, Qt::AlignRight);


        widget_total->setObjectName("widget_total");
        widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
        widget_total->setFixedHeight(70);

        widget_total->setLayout(hl_lb);

        // 커넥션
        connect(btn_more, SIGNAL(clicked()), this, SLOT(slot_clickedMediaLibraryMore()));
    }

    return widget_total;
}

/**
 * @brief SettingLib::showDlgOfDBInit : DB 초기화 Dialog Show
 */
void SettingLib::showDlgOfDBInit(){
    DialogConfirm *dlgConfirm = new DialogConfirm(this);

    dlgConfirm->setText(tr("Rose will reboot when DB is reset.\nAre you sure you want to reset?"));
    //dlgConfirm->setText(tr("DB 초기화시 로즈가 재시작됩니다.\n초기화 하시겠습니까?"));
    dlgConfirm->setTitle(tr("DB reset"));
    //dlgConfirm->setTitle(tr("DB 초기화"));
    int tmp_result = dlgConfirm->exec();

    if(tmp_result == QDialog::Accepted){
        requestDBInit();
    }

    delete dlgConfirm;
}

/**
 * @brief SettingLib::showDlgOfDbReReceive : 데이터베이스 다시 받아오기 Dialog Show
 */
void SettingLib::showDlgOfDbReReceive(){
    DialogConfirm *dlgConfirm = new DialogConfirm(this);
    dlgConfirm->setText(tr("Get the database again"));
    //dlgConfirm->setText(tr("데이터 베이스 다시 받아오기"));
    dlgConfirm->setTitle(tr("DB reset"));
    //dlgConfirm->setTitle(tr("DB 초기화"));
    int tmp_result = dlgConfirm->exec();

    if(tmp_result == QDialog::Accepted){
        global.device.setIsDbScanning(true);
        emit linker->signal_requestDownloadDB();
    }

    delete dlgConfirm;
}

/**
 * @brief SettingLib::showDlgOfMediaRemove : 미디어 삭제 Dialog Show
 */
void SettingLib::showDlgOfMediaRemove(const int &p_index){
    // DB 삭제
    if(p_index >=0 && p_index < listMedia->count()){
        DialogConfirm *dlgConfirm = new DialogConfirm(this);
        dlgConfirm->setTitle(listMedia->at(p_index)->getPath());
        dlgConfirm->setText(tr("The DB in the sub-path will be deleted together.\nAre you sure you want to delete it?"));
        //dlgConfirm->setText(tr("하위 경로에 포함된 DB는 함께 삭제 됩니다.\n삭제하시겠습니까?"));
        int tmp_result = dlgConfirm->exec();

        if(tmp_result == QDialog::Accepted){
            requestMediaAdd_Del(p_index,true);
        }

        delete dlgConfirm;
    }
}

/**
 * @brief SettingLib::requestDBInit : DB 초기화 요청
 */
void SettingLib::requestDBInit(){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("roseToken", global.device.getDeviceRoseToken());

    QString tmp_url = QString("http://%1:%2/media_library_db_init").arg(global.device.getDeviceIP()).arg(global.port);
    network->request(HTTP_NETWORK_DB_INIT, tmp_url, json, true);
}

/**
 * @brief SettingLib::requestMediaAdd_Del : 미디어 라이브러리 스캔 또는 삭제 요청
 * @param p_index int index
 * @param flagScan bool if true delete, false scan
 */
void SettingLib::requestMediaAdd_Del(const int &p_index, bool flagDel){

    if(p_index >=0 && p_index < listMedia->count()){
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        json.insert("isDelete", flagDel);
        json.insert("mediaFileInfo", listMedia->at(p_index)->getJsonData());
        json.insert("roseToken", global.device.getDeviceRoseToken());

        QString tmp_url = QString("http://%1:%2/media_library_add_delete").arg(global.device.getDeviceIP()).arg(global.port);
        network->request(HTTP_NETWORK_MEDIA_ADD_DEL, tmp_url, json, true);
    }
}

/**
 * @brief SettingLib::requestMediaLibrary : 미디어 라이브러리 요청
 */
void SettingLib::requestMediaLibrary(){

    // clear
    clearMediaList();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("music", QJsonArray());
    json.insert("musicType", 10);
    json.insert("roseToken", global.device.getDeviceRoseToken());

    QString tmp_url = QString("http://%1:%2/media_library").arg(global.device.getDeviceIP()).arg(global.port);
    network->request(HTTP_NETWORK_MEDIA_LIBRARY, tmp_url, json, true);
}

/**
 * @brief SettingLib::setResultOfMediaLibrary : 미디어 라이브러리 요청결과 처리
 * @param p_jsonObject
 */
void SettingLib::setResultOfMediaLibrary(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_mediaLibraryArr = "mediaLibraryArr";

    if(p_jsonObject.contains(jsonKey_flagOk)&&p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();

            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs)&&jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                if(p_jsonObject.contains(jsonKey_mediaLibraryArr)){
                    QJsonArray jsonArrMedia = p_jsonObject[jsonKey_mediaLibraryArr].toArray();

                    for(int i = 0 ; i < jsonArrMedia.size(); i++){
                        listMedia->append(new Data_Media_Library(jsonArrMedia.at(i).toObject()));
                        box_mediaLibrary->addWidget(getUIControlMediaLibrary(i));
                    }
                }
            //}
        }
    }
}

/**
 * @brief SettingLib::slot_clickedBtn : [슬롯] 버튼 클릭
 */
void SettingLib::slot_clickedBtn(){
    int tmp_index = sender()->property("index").toInt();

    if(tmp_index==0){
        showDlgOfDbReReceive();
    }
    else if(tmp_index==1){
        showDlgOfDBInit();
    }
    else if(tmp_index==2){
        // 현재 선택된 메뉴가 다른 경우에만 메뉴 변경되었다고 시그널 알림
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_S_SUB_MEDIALIB;
        emit changedSubMenu(tmp_data);
    }
}

/**
 * @brief SettingLib::slot_clickedMediaLibraryMore : [슬롯] 미디어라이브러리 더보기 버튼 클릭
 * @note DB 재스캔, 삭제 기능 제공
 */
void SettingLib::slot_clickedMediaLibraryMore(){
    int tmp_index = sender()->property("index").toInt();

    if(tmp_index >=0 && tmp_index < listMedia->count()){
        QString tmp_title = getMedaiNamePath(listMedia->at(tmp_index)->getIsServer(),listMedia->at(tmp_index)->getName(), listMedia->at(tmp_index)->getPath());

        Dialog::Dialog_Radio *dlg = new Dialog::Dialog_Radio(this);
        dlg->setTitle(tmp_title);
        dlg->setRadioButtons(QStringList() << tr("DB Scan") << tr("DB Delete"));
        //dlg->setRadioButtons(QStringList() << tr("DB 스캔") << tr("DB 삭제"));
        int tmp_result = dlg->exec();

        if(tmp_result == QDialog::Accepted){
            int tmp_selectedIndex = dlg->getSelectedIndex();

            if(tmp_selectedIndex==0){
                // DB 스캔
                requestMediaAdd_Del(tmp_index,false);
            }else if(tmp_selectedIndex==1){
                showDlgOfMediaRemove(tmp_index);

            }
        }

        delete dlg;
    }
}

/**
 * @brief SettingVU::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SettingLib::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    if(p_id == HTTP_NETWORK_MEDIA_LIBRARY){
        setResultOfMediaLibrary(p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_MEDIA_ADD_DEL){

        // -----------------------------------------------
        //  미디어 목록 요청과 output 같음.
        //  but, windows smb DB 스캔후 미디어 목록 요청시 API 버그 있음
        //       :: windows smb 미디어 라이브러리와 name만 다른 신규 미디어 목록이 반환됨
        //       :: 반환된 신규 미디어 목록의 name은 path와 동일값으로 반환됨
        // -----------------------------------------------
        clearMediaList();
        setResultOfMediaLibrary(p_jsonObject);

        // App와 동일하게 목록 다시 요청
        requestMediaLibrary();
    }
    else if(p_id == HTTP_NETWORK_DB_INIT){
        // nothing
    }
}
