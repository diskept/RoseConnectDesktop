#include "settingtrackoption.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"
#include "common/global.h"

#include <QScrollArea>
#include <QSettings>
#include <QComboBox>

const int HTTP_GET_PLAYTYPE = 0;
const int HTTP_SET_PLAYTYPE = 1;

/**
 * @brief SettingTrackOption::SettingCountry 생성자
 * @param parent
 */
SettingTrackOption::SettingTrackOption(QWidget *parent) : QWidget(parent)
{
    linker = Linker::getInstance();     //c220407
    connect(linker, SIGNAL(signal_httpservermsg_music_play_type_change()), this, SLOT(slot_getSettingInfo()));  //c220407

    this->setUIControl();
}


/**
 * @brief SettingTrackOption::setUIControl 기본 UI 세팅
 */
void SettingTrackOption::setUIControl()
{
    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Audio Settings")));//c220511
    vl_total->addWidget(this->getUIControlOption(tr("Music Play Options")));//c220521

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setObjectName("widget_scrollArea");
    widget_scrollArea->setLayout(vl_total);
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
 * @brief SettingTrackOption::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* SettingTrackOption::getUIControlOption(QString p_title){//c220430

    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    this->list_keyValue.append(1);
    this->list_keyValue.append(2);
    this->list_keyValue.append(3);
    this->list_keyValue.append(4);
    this->list_keyValue.append(5);
    this->list_keyValue.append(6);
    this->list_keyValue.append(7);

    this->comboBox_pod = new QComboBox();
    this->comboBox_pod->setContentsMargins(0,0,0,0);
    this->comboBox_pod->setObjectName("comboBox_playback");
    this->comboBox_pod->addItem(tr("One song Add to end of queue"), 1);                ///< 큐 끝에 추가            :: 13
    this->comboBox_pod->addItem(tr("One song Clear queue and add"), 2);                ///< 큐 비우고 추가          :: 14
    this->comboBox_pod->addItem(tr("One song Add and play immediately"), 3);               ///< 바로 듣기 추가          :: 15
    this->comboBox_pod->addItem(tr("One song Add to play next"), 4);   ///< 현재 재생곡 다음 추가   :: 16
    this->comboBox_pod->addItem(tr("Play from here"), 5);                               ///< 여기부터 재생           :: 17
    this->comboBox_pod->addItem(tr("Empty the queue and play from here"), 6);               ///< 큐 비우고 여기부터 재생 :: 18
    this->comboBox_pod->addItem(tr("Add from here to the end of the queue"), 7);                ///< 여기부터 큐 끝에 추가   :: 19

    this->comboBox_pod->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );
    int max_width = 0;
    for(int i = 0; i < this->comboBox_pod->maxVisibleItems(); i++)
    {
        QLabel *tmpText = new QLabel();
        tmpText->setStyleSheet("font-size:18px;color:#FFFFFF;");
        tmpText->setText(comboBox_pod->itemText(i));

        int tmp_width = tmpText->sizeHint().width();
        if(tmp_width > max_width)
        {
            max_width = tmp_width;
        }
    }
    this->comboBox_pod->setFixedWidth(max_width + 60);

    /*
    "QComboBox::item { font-size:18px;color:red; background-color:#333333; }"

    " QListView::item { color:red; } "
    " QListView::item:selected { border: 1px solid #B18658;background-color:#B18658;  }"
    " QListView::item:hover { background-color:#B18658; } "
    "QListView::item:selected:active { background-color:#B18658; } "

    "#comboBox_pod:item { color:#FFFFFF;height:35px; } "
    "#comboBox_pod:item:selected { background-color:#B18658; } "
    "#comboBox_pod:item::hover { background-color:#B18658; } "
    */

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,33,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(this->comboBox_pod);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);

    // 커넥션
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &SettingCountry::slot_changedIndexPod);
    connect(this->comboBox_pod, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexPod(int)));

    //timer = new QTimer(this);
    //timer->setInterval(5000); // 1초
    //connect(timer, SIGNAL(timeout()), SLOT(slot_getSettingInfo()));


    return widget_total;
}


void SettingTrackOption::slot_getSettingInfo(){//c220407

    int tmp_index = 0;
    switch (global.device.getMusicPlayType()) {
        case 13:
            tmp_index = 0;
            break;
        case 12:
            tmp_index = 1;
            break;
        case 15:
            tmp_index = 2;
            break;
        case 16:
            tmp_index = 3;
            break;
        case 17:
            tmp_index = 4;
            break;
        case 18:
            tmp_index = 5;
            break;
        case 19:
            tmp_index = 6;
            break;
    }

    this->comboBox_pod->setCurrentIndex(tmp_index);

    /*if(tmp_index != this->comboBox_pod->currentIndex()){

        this->comboBox_pod->setCurrentIndex(tmp_index);

        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        settings->setValue(rosesettings.SETTINGS_TRACK_OPTION, tmp_index);
    }*/
}


/**
 * @brief SettingTrackOption::getSettingInfo 설정정보 가져와서 세팅
 */
void SettingTrackOption::getSettingInfo(){

    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int tmp_podcast = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

    // 콤보박스 현재 index 세팅
    int tmp_index = 0;
    for(int i = 0; i < this->list_keyValue.count(); i++){
        if(this->list_keyValue.at(i) == tmp_podcast){
            tmp_index = i;
            break;
        }
    }
    this->comboBox_pod->setCurrentIndex(tmp_index);*/

    if(global.device.getMusicPlayType() == 0){
        global.device.setMusicPlayType(17);
    }

    QJsonObject json;

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_GET_PLAYTYPE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("get.music.play.type"), json, true, true);
}


void SettingTrackOption::slot_responseHttp(const int &p_id, const QJsonObject &jsonObj){

    int tmp_index = 0;

    switch(p_id){
        case HTTP_GET_PLAYTYPE:
            if(jsonObj.contains("flagOk") && jsonObj["flagOk"].toBool()){
                if(jsonObj.contains("data")){

                    QJsonObject data = ProcJsonEasy::getJsonObject(jsonObj, "data");
                    int playType = ProcJsonEasy::getInt(data, "musicPlaytype");

                    switch (playType) {
                        case 13:
                            tmp_index = 0;
                            break;
                        case 12:
                            tmp_index = 1;
                            break;
                        case 15:
                            tmp_index = 2;
                            break;
                        case 16:
                            tmp_index = 3;
                            break;
                        case 17:
                            tmp_index = 4;
                            break;
                        case 18:
                            tmp_index = 5;
                            break;
                        case 19:
                            tmp_index = 6;
                            break;
                    }
                }
                this->comboBox_pod->setCurrentIndex(tmp_index);
                print_debug();
                qDebug() << "tmp_index = " << tmp_index;
            }
            break;

        case HTTP_SET_PLAYTYPE:
            if(jsonObj.contains("flagOk") && jsonObj["flagOk"].toBool()){
                if(jsonObj.contains("data")){

                    QJsonObject data = ProcJsonEasy::getJsonObject(jsonObj, "data");
                    int playType = ProcJsonEasy::getInt(data, "musicPlaytype");

                    global.device.setMusicPlayType(playType);
                }
            }
            break;
    }

    sender()->deleteLater();
}


/**
 * @brief SettingTrackOption::slot_changedIndexPod [SLOT] 팟캐스트 콤보박스 index 변경시
 * @param index
 */
void SettingTrackOption::slot_changedIndexPod(int index){
//print_debug();
    /*Q_UNUSED(index);
    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    settings->setValue(rosesettings.SETTINGS_TRACK_OPTION, this->comboBox_pod->currentData());*/

    int playType = 0;

    switch (index) {
        case 0:
            playType = 13;
            break;
        case 1:
            playType = 12;
            break;
        case 2:
            playType = 15;
            break;
        case 3:
            playType = 16;
            break;
        case 4:
            playType = 17;
            break;
        case 5:
            playType = 18;
            break;
        case 6:
            playType = 19;
            break;
    }

    if(playType != global.device.getMusicPlayType()){

        //{"dbFileSize":0,"isDbScanning":false,"musicPlaytype":17}
        QJsonObject json;
        json.insert("dbFileSize", 0);
        json.insert("isDbScanning", false);
        json.insert("musicPlaytype", playType);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_PLAYTYPE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("set.music.play.type"), json, true, true);
    }

    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int tmp_podcast = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

    if(tmp_podcast != index){
        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        settings->setValue(rosesettings.SETTINGS_TRACK_OPTION, index);

        //{"dbFileSize":0,"isDbScanning":false,"musicPlaytype":17}
        QJsonObject json;
        json.insert("dbFileSize", 0);
        json.insert("isDbScanning", false);
        json.insert("musicPlaytype", playType);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_SET_PLAYTYPE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("set.music.play.type"), json, true, true);
    }*/
}

