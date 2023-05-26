#include "rosefm_home.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "widget/framewidget.h"
#include "widget/toastmsg.h"

#include <QJsonArray>
#include <QBoxLayout>
#include <QDial>
#include <QSlider>
#include <QPainter>
#include <QRect>
#include <QDebug>

const int SIZE_DIAL_LENGTH = 216;
const int ROTATE_VOL_DIAL = -124;           //-150;

const int HTTP_ROSEFM_TUNER_GET = 10;
const int HTTP_ROSEFM_CHANNEL_GET = 11;
const int HTTP_ROSEFM_VOLUME_GET = 12;
const int HTTP_ROSEFM_VOLUME_SET = 13;
const int HTTP_ROSEFM_MUTE_SET = 14;
const int HTTP_ROSEFM_POWER_SET = 15;
const int HTTP_ROSEFM_TUNE_FAV = 16;
const int HTTP_ROSEFM_TUNE_AUTO = 17;
const int HTTP_ROSEFM_TUNE_FRE = 18;


typedef struct _tagROSEFM
{
    int CountryID = 0;
    double lastFreq = 0;
    double favorite[6];
    bool isPower = false;
    bool isStreo = false;
    QString title = "";
    QString trackInfo = "";
} ROSEFM;

ROSEFM roseFM_data;



RoseFmHome::RoseFmHome(QWidget *parent) : QWidget(parent)
{
#if defined(Q_OS_WINDOWS)
    //0717 this->mutex.lock();
#endif

    this->setInit();
    this->setUIControl();

#if defined(Q_OS_WINDOWS)
    //0717 this->mutex.unlock();
#endif
}

RoseFmHome::~RoseFmHome(){

}

void RoseFmHome::setLabelText(){
    if(this->flag_power == true){

        this->btn_Power->setStyleSheet(this->btn_Power->styleSheet().replace("fm_btn_off.png", "fm_btn_on.png"));

        this->btn_tunled->setStyleSheet(this->btn_tunled->styleSheet().replace("fm_led_off.png", "fm_led_on_g.png"));
        if(roseFM_data.isStreo == true){
            this->btn_stereo->setStyleSheet(this->btn_stereo->styleSheet().replace("fm_led_off.png", "fm_led_on.png"));
        }
        else if(roseFM_data.isStreo == false){
            this->btn_stereo->setStyleSheet(this->btn_stereo->styleSheet().replace("fm_led_on.png", "fm_led_off.png"));
        }

        for(int ch = 0; ch < list_channelInfo.count(); ch++){
            //qDebug() << roseFM_data.lastFreq << list_channelInfo.at(ch).frequency.toDouble() << (list_channelInfo.at(ch).frequency.toDouble() - 0.3) << (list_channelInfo.at(ch).frequency.toDouble() + 0.3);
            if(list_channelInfo.at(ch).frequency.toDouble() == roseFM_data.lastFreq ||
                    (((list_channelInfo.at(ch).frequency.toDouble() - 0.3) < roseFM_data.lastFreq) && ((list_channelInfo.at(ch).frequency.toDouble() + 0.3) > roseFM_data.lastFreq))){
                roseFM_data.title = list_channelInfo.at(ch).title;
                this->auto_tune_idx = ch;
                if(roseFM_data.isStreo == false){
                    roseFM_data.isStreo = true;
                    this->btn_stereo->setStyleSheet(this->btn_stereo->styleSheet().replace("fm_led_off.png", "fm_led_on.png"));
                }
                break;
            }
        }

        //qDebug() << roseFM_data.lastFreq << roseFM_data.trackInfo << roseFM_data.title;
        if(!roseFM_data.trackInfo.isEmpty() && !roseFM_data.title.isEmpty()){
            this->lb_channel->setText(roseFM_data.trackInfo + " - " + roseFM_data.title);
        }
        else if(roseFM_data.trackInfo.isEmpty()){
            roseFM_data.title = "";
            this->lb_channel->setText("No Channel");
            if(roseFM_data.isStreo == true){
                roseFM_data.isStreo = false;
                this->btn_stereo->setStyleSheet(this->btn_stereo->styleSheet().replace("fm_led_on.png", "fm_led_off.png"));
            }
        }
        else if(roseFM_data.title.isEmpty()){
            roseFM_data.title = "";
            this->lb_channel->setText("No Channel");
            if(roseFM_data.isStreo == true){
                roseFM_data.isStreo = false;
                this->btn_stereo->setStyleSheet(this->btn_stereo->styleSheet().replace("fm_led_on.png", "fm_led_off.png"));
            }
        }

        for(int idx = 0; idx < list_channelInfo.count(); idx++){
           if(list_channelInfo.at(0).frequency.toDouble() > roseFM_data.lastFreq){
               this->auto_tune_idx = 0;
               break;
           }
           else if(list_channelInfo.at(list_channelInfo.count() - 1).frequency.toDouble() <= roseFM_data.lastFreq){
               this->auto_tune_idx = list_channelInfo.count() - 1;
               break;
           }
           else if((idx < list_channelInfo.count()) && (list_channelInfo.at(idx).frequency.toDouble() >= roseFM_data.lastFreq) && (list_channelInfo.at(idx + 1).frequency.toDouble() > roseFM_data.lastFreq)){
               this->auto_tune_idx = idx;
               break;
           }
        }

        for(int i = 0; i < list_countryInfo.count(); i++){
            if(list_countryInfo.at(i).id == roseFM_data.CountryID){
                this->lb_country->setText(list_countryInfo.at(i).name);
                this->lb_country->setStyleSheet("font:bold; font-size:21px; color:#FFFFFF; Text-align:Cnter;");
                this->lb_country->setGeometry(240, 17, 0, 0);
                break;
            }
        }
    }
    else if(this->flag_power == false){

        this->btn_Power->setStyleSheet(this->btn_Power->styleSheet().replace("fm_btn_on.png", "fm_btn_off.png"));

        this->btn_tunled->setStyleSheet(this->btn_tunled->styleSheet().replace("fm_led_on_g.png", "fm_led_off.png"));
        this->btn_stereo->setStyleSheet(this->btn_stereo->styleSheet().replace("fm_led_on.png", "fm_led_off.png"));

        this->lb_channel->setText("");
        this->lb_country->setText("");
    }

    int range = (roseFM_data.lastFreq * 9.6838) - 825.92;
    if(range != this->tun_dial->value()){
        slot_tunDial_Autotune(range);
    }

    char tmp_fav[30];
    sprintf(tmp_fav, "%.1f", roseFM_data.favorite[0]);
    lb_favorite_fre1->setText(tmp_fav);
    sprintf(tmp_fav, "%.1f", roseFM_data.favorite[1]);
    lb_favorite_fre2->setText(tmp_fav);
    sprintf(tmp_fav, "%.1f", roseFM_data.favorite[2]);
    lb_favorite_fre3->setText(tmp_fav);
    sprintf(tmp_fav, "%.1f", roseFM_data.favorite[3]);
    lb_favorite_fre4->setText(tmp_fav);
    sprintf(tmp_fav, "%.1f", roseFM_data.favorite[4]);
    lb_favorite_fre5->setText(tmp_fav);
    sprintf(tmp_fav, "%.1f", roseFM_data.favorite[5]);
    lb_favorite_fre6->setText(tmp_fav);
}

void RoseFmHome::slot_responseHttp(const int &p_id, const QJsonObject &jsonObject){
    /*QJsonDocument doc(jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << strJson;*/

    switch(p_id){
    case HTTP_ROSEFM_TUNER_GET:
        if(jsonObject.contains("data")){
            QString tmpData = ProcJsonEasy::getString(jsonObject, "data");

            QJsonDocument doc = QJsonDocument::fromJson(tmpData.toUtf8());
            QJsonObject tmpObj = doc.object();

            //QString strJson(doc.toJson(QJsonDocument::Compact));
            //qDebug() << strJson;

            roseFM_data.CountryID = ProcJsonEasy::getInt(tmpObj, "countryId");
            roseFM_data.trackInfo = ProcJsonEasy::getString(tmpObj, "trackInfo");
            roseFM_data.lastFreq = ProcJsonEasy::getDouble(tmpObj, "lastHz");
            roseFM_data.isPower = ProcJsonEasy::getBool(tmpObj, "isPowerOn");
            roseFM_data.isStreo = ProcJsonEasy::getBool(tmpObj, "isStereo");

            QJsonArray tmpArr = ProcJsonEasy::getJsonArray(tmpObj, "favArr");
            for (int i = 0; i < tmpArr.count(); i++){
                roseFM_data.favorite[i] = tmpArr.at(i).toDouble();
            }

            this->flag_power = roseFM_data.isPower;
        }

        if(list_channelInfo.count() == 0){
            request_getChannel_info();
        }
        setLabelText();
        break;

    case HTTP_ROSEFM_CHANNEL_GET:
        global.user_forRoseFM.setChannelInfo(jsonObject);
        global.user_forRoseFM.getChannelInfo(list_channelInfo);

        setLabelText();
        break;

    case HTTP_ROSEFM_VOLUME_GET:
        if(jsonObject.contains("volumeValue")){
            this->volum_val = jsonObject["volumeValue"].toInt();
            slot_volDial_changed(this->volum_val);
        }
        break;

    case HTTP_ROSEFM_VOLUME_SET:
        break;

    case HTTP_ROSEFM_MUTE_SET:
        break;

    case HTTP_ROSEFM_POWER_SET:
        if(this->flag_power == true){
            this->tun_dial->setEnabled(true);
            this->vol_dial->setEnabled(true);
            this->btn_Mute->setEnabled(true);

            request_getTune_info();
        }
        else if(this->flag_power == false){
            this->tun_dial->setEnabled(false);
            this->vol_dial->setEnabled(false);
            this->btn_Mute->setEnabled(false);

            setLabelText();
        }
        break;
    case HTTP_ROSEFM_TUNE_FAV:
        /*if(this->flag_power == true){
            setLabelText();
        }*/
        break;
    case HTTP_ROSEFM_TUNE_AUTO:
        /*if(this->flag_power == true){
            roseFM_data.lastFreq = list_channelInfo.at(this->auto_tune_idx).frequency.toDouble();
            roseFM_data.isStreo = false;
            roseFM_data.trackInfo = "";
        }
        setLabelText();*/
        break;
    case HTTP_ROSEFM_TUNE_FRE:
        if(this->flag_power == true && this->flag_reload == true){
            this->flag_reload = false;
            setLabelText();
        }
        break;
    }
}

void RoseFmHome::slot_channel_changed(const QJsonObject &p_jsonObject){
    /*QJsonDocument doc(p_jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << strJson;*/

    if(p_jsonObject.contains("data")){
        QJsonObject tmpObj = ProcJsonEasy::getJsonObject(p_jsonObject, "data");

        QString tmp_title = ProcJsonEasy::getString(tmpObj, "titleName");
        QString tmp_trackInfo = ProcJsonEasy::getString(tmpObj, "trackInfo");;

        if(roseFM_data.trackInfo.isEmpty() || tmp_title != roseFM_data.title){
            roseFM_data.title = tmp_title;
            roseFM_data.trackInfo = tmp_trackInfo;            
            roseFM_data.isStreo = true;

            for(int ch = 0; ch < list_channelInfo.count(); ch++){
                if(list_channelInfo.at(ch).title == tmp_title){
                    roseFM_data.lastFreq = list_channelInfo.at(ch).frequency.toDouble();
                    this->auto_tune_idx = ch;
                    break;
                }
            }

            setLabelText();
        }
    }
}

void RoseFmHome::get_rose_data(){

    global.user_forRoseFM.getCountryInfo(list_countryInfo);

    if(global.device.getDeviceIP().isEmpty() == false){
        NetworkHttp *netVol = new NetworkHttp(this);
        connect(netVol, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        json.insert("volumeType", "volume_get");
        netVol->request(HTTP_ROSEFM_VOLUME_GET, QString("http://%1:%2/volume").arg(global.device.getDeviceIP()).arg(global.port), json, true);

        request_getTune_info();

        if(roseFM_data.isPower == true){
            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QString param = "true";
            network->request(HTTP_ROSEFM_POWER_SET, QString("http://%1:%2/set.fmtuner.play_power_on_off").arg(global.device.getDeviceIP()).arg(global.port), param, true);
        }
    }
}

void RoseFmHome::request_getChannel_info(){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    //==channel info==//
    //https://api.roseaudio.kr/v1/radio/member/channel/classic?countryId=1&page=0&pageSize=100
    QUrlQuery params;
    params.addQueryItem("countryId", QString("%1").arg(roseFM_data.CountryID));
    params.addQueryItem("page", "0");
    params.addQueryItem("pageSize", "100");
    network->request(HTTP_ROSEFM_CHANNEL_GET, QString("%1/channel/classic").arg(global.rosefm_member), params, false, false);
}

void RoseFmHome::request_getTune_info(){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    network->request(HTTP_ROSEFM_TUNER_GET, QString("http://%1:%2/get.fmtuner.data").arg(global.device.getDeviceIP()).arg(global.port), json, true);
}

void RoseFmHome::request_favTune_set(int num){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QString param = QString::number(num);
    network->request(HTTP_ROSEFM_TUNE_FAV, QString("http://%1:%2/set.fmtuner.play_fav").arg(global.device.getDeviceIP()).arg(global.port), param, true);
}

void RoseFmHome::request_autTune_set(int ch_num){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QString param = QString::number(ch_num);
    network->request(HTTP_ROSEFM_TUNE_AUTO, QString("http://%1:%2/set.fmtuner.play").arg(global.device.getDeviceIP()).arg(global.port), param, true);
}

void RoseFmHome::request_frqTune_set(double frequency){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QString param = "";

    if(frequency >= 1001 && frequency <= 1005){
        param = QString::number(frequency);
        network->request(HTTP_ROSEFM_TUNE_FRE, QString("http://%1:%2/set.fmtuner.play").arg(global.device.getDeviceIP()).arg(global.port), param, true);
        this->flag_reload = true;
    }
    else{
        char tmp_fav[30];
        sprintf(tmp_fav, "%.1f", frequency);
        network->request(HTTP_ROSEFM_TUNE_FRE, QString("http://%1:%2/set.fmtuner.play_hz").arg(global.device.getDeviceIP()).arg(global.port), tmp_fav, true);
        this->flag_reload = true;
    }
}

void RoseFmHome::setInit(){
    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_rosefmChannelChanged(QJsonObject)), SLOT(slot_channel_changed(QJsonObject)));

    vol_pixmap = GSCommon::getUIPixmapImg(":/images/roseFM/fm_volume.png");

    tmp_imgvol = vol_pixmap->scaled(SIZE_DIAL_LENGTH, SIZE_DIAL_LENGTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QMatrix tmp_rotvol;
    tmp_rotvol.rotate(ROTATE_VOL_DIAL);
    tmp_imgvol = tmp_imgvol.transformed(tmp_rotvol);

    tun_pixmap = GSCommon::getUIPixmapImg(":/images/roseFM/fm_tune.png");

    tmp_imgtun = tun_pixmap->scaled(SIZE_DIAL_LENGTH, SIZE_DIAL_LENGTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QMatrix tmp_rottun;
    tmp_rottun.rotate(ROTATE_VOL_DIAL);
    tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);
}

void RoseFmHome::setUIControl(){

    dlgHelp = new DialogRoseFmHelp(this);
    dlgHelp->setTitle(tr("NOTICE"));
    dlgHelp->setText(tr("As Internet radio with an analog design, it provides user convenience with various functions.\nYou can enjoy various channels by adjusting the TUNE wheel left and right, and you can press and hold the Favorites button to register the current channel as a favorite.\nYou can play or pause the radio with the ON/OFF button.\nYou can also enjoy Internet radio and get information on the channel you are listening to through VUMeter."));
    //dlgHelp->setText(tr("아날로그 디자인의 인터넷 라디오로써 다양한 기능으로 사용자의 편의를 제공합니다.\nTUNE휠을 좌우로 컨트롤하여 다양한 채널을 감상할 수 있으며 즐겨찾기 버튼을 길게 눌러 현재의 채널을 즐겨찾기 등록할 수 있습니다.\nOn/Off 버튼을 이용하여 Play/Pause기능을 제공합니다.\n또한 감상중인 채널에 대한 정보 및 VUMeter를 통해 인터넷 라디오를 즐기실 수 있습니다."));

    QWidget *sliderTWidget = new QWidget();
    sliderTWidget->setObjectName("freWidget");
    sliderTWidget->setStyleSheet("#freWidget {background-image: url(:/images/roseFM/fm_bg1.png);}");
    sliderTWidget->setFixedSize(1350, 230);

    QWidget *sliderWidget = new QWidget(sliderTWidget);
    sliderWidget->setObjectName("sliderwidget");
    sliderWidget->setStyleSheet("#sliderwidget {background-image: url(:/images/roseFM/fm_frame1.png);}");
    sliderWidget->setGeometry(61, 25, 1176, 181);

    this->fre_slider = new QSlider(Qt::Horizontal, sliderWidget);
    this->fre_slider->setStyleSheet("QSlider::handle:horizontal{image: url(:/images/roseFM/fm_ needle.png);}"
                            "QSlider::groove:horizontal{image: url(:/images/roseFM/fm_fre_kr.png); Qt::AlignCenter}"
                            "QSlider::groove:horizontal{background-image: url(:/images/roseFM/fm_fre_on_bg.png);}");
    this->fre_slider->setMaximum(248);
    this->fre_slider->setMinimum(0);
    this->fre_slider->setTickInterval(10);
    this->fre_slider->setSingleStep(1);
    this->fre_slider->setValue(0);
    this->fre_slider->setGeometry(19, 17, 1138, 146);
    this->fre_slider->setDisabled(true);

    QWidget *blinkLine1 = new QWidget();
    blinkLine1->setObjectName("blinkLine1");
    blinkLine1->setStyleSheet("#blinkLine1 {background-image: url(:/images/roseFM/fm_line1.png);}");
    blinkLine1->setFixedSize(1350, 5);

    //===================================================================================================//

    QWidget *tuningTWidget = new QWidget();
    tuningTWidget->setObjectName("tuningTWidget");
    tuningTWidget->setStyleSheet("#tuningTWidget {background-image: url(:/images/roseFM/fm_bg1.png);}");
    tuningTWidget->setFixedSize(1350, 200);

    QWidget *tuningWidget = new QWidget(tuningTWidget);
    tuningWidget->setObjectName("tuningWidget");
    tuningWidget->setStyleSheet("#tuningWidget {background-image: url(:/images/roseFM/fm_frame2.png);}");
    tuningWidget->setGeometry(61, 25, 1176, 150);

    QWidget *tunDialWidget = new QWidget(tuningWidget);
    tunDialWidget->setObjectName("tunDialWidget");
    tunDialWidget->setStyleSheet("#tunDialWidget {background-image: url(:/images/roseFM/fm_tuning.png);}");
    tunDialWidget->setGeometry(18, 19, 261, 108);

    QWidget *channelWidget = new QWidget(tuningWidget);
    channelWidget->setObjectName("channelWidget");
    channelWidget->setStyleSheet("#channelWidget {background-image: url(:/images/roseFM/fm_ch_bg.png);}");
    channelWidget->setGeometry(296, 19, 862, 56);

    this->lb_channel = new QLabel(channelWidget);
    this->lb_channel->setStyleSheet("font:bold; font-size:20px; color:#FFFFFF; Text-align:left;");
    this->lb_channel->setFixedSize(800, 25);
    this->lb_channel->setFixedHeight(25);
    this->lb_channel->setGeometry(20, 16, 0, 0);
    //this->lb_channel->setText("mp3. 16bit 44.1kHz 2ch - KBS Classic FM");

    QWidget *locationWidget = new QWidget(tuningWidget);
    locationWidget->setGeometry(296, 80, 862, 60);

    this->btn_tunled =  GSCommon::getUIBtnImg("countryButton", ":/images/roseFM/fm_led_off.png", locationWidget);
    this->btn_tunled->setGeometry(64, 18, 25, 25);
    this->btn_tunled->setEnabled(false);

    QLabel *lb_tun = new QLabel(locationWidget);
    lb_tun->setStyleSheet("font-size:15px; color:#aaaaaa;");
    lb_tun->setText("TUNING");
    lb_tun->setGeometry(104, 18, 60, 18);

    this->btn_stereo =  GSCommon::getUIBtnImg("countryButton", ":/images/roseFM/fm_led_off.png", locationWidget);
    this->btn_stereo->setGeometry(221, 18, 25, 25);
    this->btn_stereo->setEnabled(false);

    QLabel *lb_stereo = new QLabel(locationWidget);
    lb_stereo->setStyleSheet("font-size:15px; color:#aaaaaa;");
    lb_stereo->setText("STEREO");
    lb_stereo->setGeometry(261, 18, 60, 18);

    QWidget *countryWidget = new QWidget(locationWidget);
    countryWidget->setObjectName("countryWidget");
    countryWidget->setStyleSheet("#countryWidget {background-image: url(:/images/roseFM/fm_country_bg.png);}");
    countryWidget->setGeometry(403, 0, 461, 56);

    this->btn_country =  GSCommon::getUIBtnImg("countryButton", ":/images/roseFM/fm_country_btn.png", countryWidget);
    this->btn_country->setCursor(Qt::PointingHandCursor);
    this->btn_country->setGeometry(8, 7, 96, 47);

    this->lb_country = new QLabel(countryWidget);
    this->lb_country->setFixedSize(350, 24);
    //this->lb_country->setStyleSheet("font:bold; font-size:21px; color:#FFFFFF; Text-align:Center;");
    //this->lb_country->setGeometry(180, 17, 0, 0);
    //this->lb_country->setText("대한민국 - 서울특별시");

    QWidget *blinkLine2 = new QWidget();
    blinkLine2->setObjectName("blinkLine2");
    blinkLine2->setStyleSheet("#blinkLine2 {background-image: url(:/images/roseFM/fm_line1.png);}");
    blinkLine2->setFixedSize(1350, 5);

    //===================================================================================================//

    QWidget *dialTWidget = new QWidget();
    dialTWidget->setObjectName("dialTWidget");
    dialTWidget->setStyleSheet("#dialTWidget {background-image: url(:/images/roseFM/fm_bg1.png);}");
    dialTWidget->setFixedSize(1350, 330);

    QWidget *volDialWidget = new QWidget(dialTWidget);
    volDialWidget->setGeometry(62, 30, 260, 270);

    QPixmap *vol_notches = GSCommon::getUIPixmapImg(":/images/roseFM/fm_volume_img.png");

    QLabel *notches_base = new QLabel(volDialWidget);
    notches_base->setGeometry(2, 0, 254, 270);

    QLabel *lb_vol_notches = new QLabel(notches_base);
    lb_vol_notches->setPixmap(*vol_notches);
    lb_vol_notches->setFixedSize(lb_vol_notches->pixmap()->width(), lb_vol_notches->pixmap()->height());

    this->vol_dial = new QDial(notches_base);
    this->vol_dial->setCursor(Qt::PointingHandCursor);
    this->vol_dial->setFixedSize(230, 230);
    this->vol_dial->setSingleStep(1);
    this->vol_dial->setMaximum(99);
    this->vol_dial->setMinimum(0);
    this->vol_dial->setValue(0);
    this->vol_dial->setGeometry(18, 31, 0, 0);
    //this->vol_dial->setStyleSheet("background-color: #");

    this->lb_vol_dial = new QLabel(vol_dial);
    this->lb_vol_dial->setPixmap(tmp_imgvol);
    this->lb_vol_dial->setGeometry(-40, -40, lb_vol_dial->pixmap()->width(), lb_vol_dial->pixmap()->height());
    this->lb_vol_dial->setAlignment(Qt::AlignCenter);

    QLabel *vol_dial_min = new QLabel(volDialWidget);
    vol_dial_min->setStyleSheet("font-size:14px; color:#aaaaaa; Text-align:Left;");
    vol_dial_min->setText("MIN");
    vol_dial_min->setGeometry(0, 212, 50, 16);

    QLabel *vol_dial_max = new QLabel(volDialWidget);
    vol_dial_max->setStyleSheet("font-size:14px; color:#aaaaaa; Text-align:Right;");
    vol_dial_max->setText("MAX");
    vol_dial_max->setGeometry(226, 212, 50, 16);

    QLabel *vol_dial_name = new QLabel(volDialWidget);
    vol_dial_name->setStyleSheet("font-size:16px; color:#aaaaaa; Text-align:Center;");
    vol_dial_name->setText("VOLUME");
    vol_dial_name->setGeometry(95, 252, 100, 19);

    QWidget *blinkWiget1 = new QWidget(dialTWidget);
    blinkWiget1->setObjectName("blinkWiget1");
    blinkWiget1->setStyleSheet("#blinkWiget1 {background-image: url(:/images/roseFM/fm_line2.png);}");
    blinkWiget1->setGeometry(370, 20, 5, 282);

    QWidget *freButtonWidget = new QWidget(dialTWidget);
    freButtonWidget->setGeometry(390, 20, 590, 330);

    QLabel *lb_pwr_on = new QLabel(freButtonWidget);
    lb_pwr_on->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Left;");
    lb_pwr_on->setText("ON");
    lb_pwr_on->setGeometry(30, 0, 30, 19);

    QLabel *img_pwr_on = GSCommon::getUILabelImg(":/images/roseFM/fm_line3.png", freButtonWidget);
    img_pwr_on->setGeometry(60, 7, 50, 28);

    QLabel *img_pwr_off = GSCommon::getUILabelImg(":/images/roseFM/fm_line4.png", freButtonWidget);
    img_pwr_off->setGeometry(145, 7, 50, 28);

    QLabel *lb_pwr_off = new QLabel(freButtonWidget);
    lb_pwr_off->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Right;");
    lb_pwr_off->setText("OFF");
    lb_pwr_off->setGeometry(200, 0, 30, 19);

    this->btn_Power = GSCommon::getUIBtnImg("btn_Power", ":/images/roseFM/fm_btn_off.png", freButtonWidget);
    this->btn_Power->setCursor(Qt::PointingHandCursor);
    this->btn_Power->setGeometry(68, 41, 119, 54);

    QLabel *lb_pwr_name = new QLabel(freButtonWidget);
    lb_pwr_name->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    lb_pwr_name->setText("POWER");
    lb_pwr_name->setGeometry(98, 95, 60, 19);

    QLabel *lb_mute_on = new QLabel(freButtonWidget);
    lb_mute_on->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Left;");
    lb_mute_on->setText("ON");
    lb_mute_on->setGeometry(340, 0, 30, 19);

    QLabel *img_mute_on = GSCommon::getUILabelImg(":/images/roseFM/fm_line3.png", freButtonWidget);
    img_mute_on->setGeometry(370, 7, 50, 28);

    QLabel *img_mute_off = GSCommon::getUILabelImg(":/images/roseFM/fm_line4.png", freButtonWidget);
    img_mute_off->setGeometry(455, 7, 50, 28);

    QLabel *lb_mute_off = new QLabel(freButtonWidget);
    lb_mute_off->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Right;");
    lb_mute_off->setText("OFF");
    lb_mute_off->setGeometry(510, 0, 30, 19);

    this->btn_Mute = GSCommon::getUIBtnImg("btn_Mute", ":/images/roseFM/fm_btn_off.png", freButtonWidget);
    this->btn_Mute->setCursor(Qt::PointingHandCursor);
    this->btn_Mute->setGeometry(378, 41, 119, 54);

    QLabel *lb_mute_name = new QLabel(freButtonWidget);
    lb_mute_name->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    lb_mute_name->setText("MUTE");
    lb_mute_name->setGeometry(418, 95, 60, 19);

    this->btn_Help = GSCommon::getUIBtnImg("btn_Help", ":/images/roseFM/help_ico.png", freButtonWidget);
    this->btn_Help->setCursor(Qt::PointingHandCursor);
    this->btn_Help->setGeometry(520, 100, 66, 53);

    QImage img;
    QPixmap *pixmap_icon = new QPixmap();
    img.load(":/images/roseFM/fm_line5.png");
    *pixmap_icon = QPixmap::fromImage(img);
    *pixmap_icon = pixmap_icon->scaled(485, 34, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *img_favorite = new QLabel(freButtonWidget);
    img_favorite->setPixmap(*pixmap_icon);
    img_favorite->setGeometry(40, 153, 485, 34);

    QLabel *lb_favorite = new QLabel(freButtonWidget);
    lb_favorite->setStyleSheet("font-size:16px; color:#aaaaaa; Text-align:Center;");
    lb_favorite->setText("FAVORITE");
    lb_favorite->setGeometry(245, 145, 160, 19);

    this->btn_favorite_fre1 = GSCommon::getUIBtnImg("btn_favorite_fre1", ":/images/roseFM/fm_fav_btn.png", freButtonWidget);
    this->btn_favorite_fre1->setCursor(Qt::PointingHandCursor);
    this->btn_favorite_fre1->setGeometry(10, 190, 67, 67);

    this->lb_favorite_fre1 = new QLabel(freButtonWidget);
    this->lb_favorite_fre1->setFixedSize(50, 19);
    this->lb_favorite_fre1->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    //this->lb_favorite_fre1->setText("89.0");
    this->lb_favorite_fre1->setGeometry(27, 265, 0, 0);

    this->btn_favorite_fre2 = GSCommon::getUIBtnImg("btn_favorite_fre2", ":/images/roseFM/fm_fav_btn.png", freButtonWidget);
    this->btn_favorite_fre2->setCursor(Qt::PointingHandCursor);
    this->btn_favorite_fre2->setGeometry(102, 190, 67, 67);

    this->lb_favorite_fre2 = new QLabel(freButtonWidget);
    this->lb_favorite_fre2->setFixedSize(50, 19);
    this->lb_favorite_fre2->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    //this->lb_favorite_fre2->setText("91.9");
    this->lb_favorite_fre2->setGeometry(121, 265, 0, 0);

    this->btn_favorite_fre3 = GSCommon::getUIBtnImg("btn_favorite_fre3", ":/images/roseFM/fm_fav_btn.png", freButtonWidget);
    this->btn_favorite_fre3->setCursor(Qt::PointingHandCursor);
    this->btn_favorite_fre3->setGeometry(199, 190, 67, 67);

    this->lb_favorite_fre3 = new QLabel(freButtonWidget);
    this->lb_favorite_fre3->setFixedSize(50, 19);
    this->lb_favorite_fre3->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    //this->lb_favorite_fre3->setText("93.0");
    this->lb_favorite_fre3->setGeometry(218, 265, 0, 0);

    this->btn_favorite_fre4 = GSCommon::getUIBtnImg("btn_favorite_fre4", ":/images/roseFM/fm_fav_btn.png", freButtonWidget);
    this->btn_favorite_fre4->setCursor(Qt::PointingHandCursor);
    this->btn_favorite_fre4->setGeometry(296, 190, 67, 67);

    this->lb_favorite_fre4 = new QLabel(freButtonWidget);
    this->lb_favorite_fre4->setFixedSize(50, 19);
    this->lb_favorite_fre4->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    //this->lb_favorite_fre4->setText("95.0");
    this->lb_favorite_fre4->setGeometry(315, 265, 0, 0);

    this->btn_favorite_fre5 = GSCommon::getUIBtnImg("btn_favorite_fre5", ":/images/roseFM/fm_fav_btn.png", freButtonWidget);
    this->btn_favorite_fre5->setCursor(Qt::PointingHandCursor);
    this->btn_favorite_fre5->setGeometry(393, 190, 67, 67);

    this->lb_favorite_fre5 = new QLabel(freButtonWidget);
    this->lb_favorite_fre5->setFixedSize(50, 19);
    this->lb_favorite_fre5->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    //this->lb_favorite_fre5->setText("98.0");
    this->lb_favorite_fre5->setGeometry(412, 265, 0, 0);

    this->btn_favorite_fre6 = GSCommon::getUIBtnImg("btn_favorite_fre6", ":/images/roseFM/fm_fav_btn.png", freButtonWidget);
    this->btn_favorite_fre6->setCursor(Qt::PointingHandCursor);
    this->btn_favorite_fre6->setGeometry(490, 190, 67, 67);

    this->lb_favorite_fre6 = new QLabel(freButtonWidget);
    this->lb_favorite_fre6->setFixedSize(50, 19);
    this->lb_favorite_fre6->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    //this->lb_favorite_fre6->setText("103.3");
    this->lb_favorite_fre6->setGeometry(509, 265, 0, 0);

    QWidget *blinkWiget2 = new QWidget(dialTWidget);
    blinkWiget2->setObjectName("blinkWiget1");
    blinkWiget2->setStyleSheet("#blinkWiget1 {background-image: url(:/images/roseFM/fm_line2.png);}");
    blinkWiget2->setGeometry(980, 20, 5, 282);

    QWidget *tuneDialWidget = new QWidget(dialTWidget);
    tuneDialWidget->setGeometry(1010, 30, 260, 270);

    this->tun_dial = new QDial(tuneDialWidget);
    this->tun_dial->setCursor(Qt::PointingHandCursor);
    this->tun_dial->setFixedSize(230, 230);
    this->tun_dial->setSingleStep(1);
    this->tun_dial->setMaximum(280);
    this->tun_dial->setMinimum(0);
    this->tun_dial->setValue(0);
    this->tun_dial->setGeometry(18, 31, 0, 0);
    //this->vol_dial->setStyleSheet("background-color: #");

    this->lb_tun_dial = new QLabel(tun_dial);
    this->lb_tun_dial->setPixmap(tmp_imgtun);
    this->lb_tun_dial->setGeometry(-40, -40, lb_tun_dial->pixmap()->width(), lb_tun_dial->pixmap()->height());

    QLabel *tun_dial_name = new QLabel(tuneDialWidget);
    tun_dial_name->setStyleSheet("font-size:17px; color:#aaaaaa; Text-align:Center;");
    tun_dial_name->setText("TUNE");
    tun_dial_name->setGeometry(110, 252, 100, 19);

    this->btn_Back = GSCommon::getUIBtnImg("btn_Back", ":/images/roseFM/fm_back.png", tuneDialWidget);
    this->btn_Back->setCursor(Qt::PointingHandCursor);
    this->btn_Back->setGeometry(28, 252, 53, 22);

    this->btn_Next = GSCommon::getUIBtnImg("btn_Back", ":/images/roseFM/fm_next.png", tuneDialWidget);
    this->btn_Next->setCursor(Qt::PointingHandCursor);
    this->btn_Next->setGeometry(180, 252, 53, 22);

    //===================================================================================================//

    QVBoxLayout *rosefmLayout = new QVBoxLayout();
    rosefmLayout->setContentsMargins(165, 0, 165, 0);
    rosefmLayout->addWidget(sliderTWidget);
    rosefmLayout->addWidget(blinkLine1);
    rosefmLayout->addWidget(tuningTWidget);
    rosefmLayout->addWidget(blinkLine2);
    rosefmLayout->addWidget(dialTWidget);
    rosefmLayout->addStretch(0);

    this->setLayout(rosefmLayout);

    if(roseFM_data.isPower == false)
    {
        this->tun_dial->setEnabled(false);
        this->vol_dial->setEnabled(false);
        this->btn_Mute->setEnabled(false);

        setLabelText();
    }

    if(global.enable_section_left == true){
        global.enable_section_left = false;
    }

    connect(btn_country, &QPushButton::clicked, this, &RoseFmHome::slot_btnCountry_clicked);
    connect(btn_country, &QPushButton::pressed, this, &RoseFmHome::slot_btnCountry_pressed);
    connect(btn_Help, &QPushButton::clicked, this, &RoseFmHome::slot_btnHelp_changed);

    connect(btn_Power, &QPushButton::clicked, this, &RoseFmHome::slot_btnPower_changed);
    connect(btn_Mute, &QPushButton::clicked, this, &RoseFmHome::slot_btnMute_changed);

    connect(btn_Back, &QPushButton::clicked, this, &RoseFmHome::slot_btnBack_clicked);
    connect(btn_Next, &QPushButton::clicked, this, &RoseFmHome::slot_btnNext_clicked);

    connect(btn_Back, &QPushButton::pressed, this, &RoseFmHome::slot_btnBack_pressed);
    connect(btn_Next, &QPushButton::pressed, this, &RoseFmHome::slot_btnNext_pressed);

    connect(btn_favorite_fre1, &QPushButton::clicked, this, &RoseFmHome::slot_btnfre1_pressed);
    connect(btn_favorite_fre2, &QPushButton::clicked, this, &RoseFmHome::slot_btnfre2_pressed);
    connect(btn_favorite_fre3, &QPushButton::clicked, this, &RoseFmHome::slot_btnfre3_pressed);
    connect(btn_favorite_fre4, &QPushButton::clicked, this, &RoseFmHome::slot_btnfre4_pressed);
    connect(btn_favorite_fre5, &QPushButton::clicked, this, &RoseFmHome::slot_btnfre5_pressed);
    connect(btn_favorite_fre6, &QPushButton::clicked, this, &RoseFmHome::slot_btnfre6_pressed);

    connect(btn_favorite_fre1, &QPushButton::pressed, this, &RoseFmHome::slot_btnfre1_clicked);
    connect(btn_favorite_fre2, &QPushButton::pressed, this, &RoseFmHome::slot_btnfre2_clicked);
    connect(btn_favorite_fre3, &QPushButton::pressed, this, &RoseFmHome::slot_btnfre3_clicked);
    connect(btn_favorite_fre4, &QPushButton::pressed, this, &RoseFmHome::slot_btnfre4_clicked);
    connect(btn_favorite_fre5, &QPushButton::pressed, this, &RoseFmHome::slot_btnfre5_clicked);
    connect(btn_favorite_fre6, &QPushButton::pressed, this, &RoseFmHome::slot_btnfre6_clicked);

    connect(vol_dial, &QDial::valueChanged, this, &RoseFmHome::slot_volDial_changed);
    connect(tun_dial, &QDial::valueChanged, this, &RoseFmHome::slot_tunDial_changed);
}

void RoseFmHome::slot_btnPower_changed(){
    this->flag_power = !this->flag_power;
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(this->flag_power == true){
        this->btn_Power->setStyleSheet(this->btn_Power->styleSheet().replace("fm_btn_off.png", "fm_btn_on.png"));
        QString param = "true";
        network->request(HTTP_ROSEFM_POWER_SET, QString("http://%1:%2/set.fmtuner.play_power_on_off").arg(global.device.getDeviceIP()).arg(global.port), param, true);
    }
    else{
        this->btn_Power->setStyleSheet(this->btn_Power->styleSheet().replace("fm_btn_on.png", "fm_btn_off.png"));
        QString param = "false";
        network->request(HTTP_ROSEFM_POWER_SET, QString("http://%1:%2/set.fmtuner.play_power_on_off").arg(global.device.getDeviceIP()).arg(global.port), param, true);
    }    
}

void RoseFmHome::slot_btnMute_changed(){
    this->flag_mute = !this->flag_mute;
    if(this->flag_mute == true){
        this->btn_Mute->setStyleSheet(this->btn_Mute->styleSheet().replace("fm_btn_off.png", "fm_btn_on.png"));
    }
    else{
        this->btn_Mute->setStyleSheet(this->btn_Mute->styleSheet().replace("fm_btn_on.png", "fm_btn_off.png"));
    }

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("barControl","remote_bar_order.mute");
    json.insert("value","-1");
    json.insert("roseToken", global.device.getDeviceRoseToken());
    network->request(HTTP_ROSEFM_MUTE_SET, QString("http://%1:%2/remote_bar_order").arg(global.device.getDeviceIP()).arg(global.port), json, true);
}

void RoseFmHome::slot_btnCountry_clicked(){
    this->btn_country->setStyleSheet(this->btn_country->styleSheet().replace("fm_country_btn_press.png", "fm_country_btn.png"));
}

void RoseFmHome::slot_btnCountry_pressed(){
    this->btn_country->setStyleSheet(this->btn_country->styleSheet().replace("fm_country_btn.png", "fm_country_btn_press.png"));
}

void RoseFmHome::slot_btnHelp_changed(){
    if(dlgHelp->isVisible() == false){
        dlgHelp->exec();
    }
    //dlgHelp->show();
}


void RoseFmHome::slot_btnBack_clicked(){
    this->btn_Back->setStyleSheet(this->btn_Back->styleSheet().replace("fm_back_press.png", "fm_back.png"));
}

void RoseFmHome::slot_btnNext_clicked(){
    this->btn_Next->setStyleSheet(this->btn_Next->styleSheet().replace("fm_next_press.png", "fm_next.png"));
}

void RoseFmHome::slot_btnBack_pressed(){
    if(auto_tune_idx == 0){
        if(list_channelInfo.at(0).frequency.toDouble() != roseFM_data.lastFreq){
            request_autTune_set(this->auto_tune_idx);
        }
    }
    else{
        this->auto_tune_idx -= 1;
        request_autTune_set(this->auto_tune_idx);
    }
    this->btn_Back->setStyleSheet(this->btn_Back->styleSheet().replace("fm_back.png", "fm_back_press.png"));
}

void RoseFmHome::slot_btnNext_pressed(){
    if(auto_tune_idx == list_channelInfo.count() - 1){
        if(list_channelInfo.at(list_channelInfo.count() - 1).frequency.toDouble() != roseFM_data.lastFreq){
            request_autTune_set(this->auto_tune_idx);
        }
    }
    else{
        this->auto_tune_idx += 1;
        request_autTune_set(this->auto_tune_idx);
    }
    this->btn_Next->setStyleSheet(this->btn_Next->styleSheet().replace("fm_next.png", "fm_next_press.png"));
}


void RoseFmHome::slot_btnfre1_clicked(){
    this->btn_favorite_fre1->setStyleSheet(this->btn_favorite_fre1->styleSheet().replace("fm_fav_btn.png", "fm_fav_btn_press.png"));
}

void RoseFmHome::slot_btnfre2_clicked(){
    this->btn_favorite_fre2->setStyleSheet(this->btn_favorite_fre2->styleSheet().replace("fm_fav_btn.png", "fm_fav_btn_press.png"));
}

void RoseFmHome::slot_btnfre3_clicked(){
    this->btn_favorite_fre3->setStyleSheet(this->btn_favorite_fre3->styleSheet().replace("fm_fav_btn.png", "fm_fav_btn_press.png"));
}

void RoseFmHome::slot_btnfre4_clicked(){
    this->btn_favorite_fre4->setStyleSheet(this->btn_favorite_fre4->styleSheet().replace("fm_fav_btn.png", "fm_fav_btn_press.png"));
}

void RoseFmHome::slot_btnfre5_clicked(){
    this->btn_favorite_fre5->setStyleSheet(this->btn_favorite_fre5->styleSheet().replace("fm_fav_btn.png", "fm_fav_btn_press.png"));
}

void RoseFmHome::slot_btnfre6_clicked(){
    this->btn_favorite_fre6->setStyleSheet(this->btn_favorite_fre6->styleSheet().replace("fm_fav_btn.png", "fm_fav_btn_press.png"));
}


void RoseFmHome::slot_btnfre1_pressed(){
    if(roseFM_data.isPower == true){
        request_favTune_set(0);
        roseFM_data.lastFreq = roseFM_data.favorite[0];
    }

    this->btn_favorite_fre1->setStyleSheet(this->btn_favorite_fre1->styleSheet().replace("fm_fav_btn_press.png", "fm_fav_btn.png"));
}

void RoseFmHome::slot_btnfre2_pressed(){
    if(roseFM_data.isPower == true){
        request_favTune_set(1);
        roseFM_data.lastFreq = roseFM_data.favorite[1];
    }

    this->btn_favorite_fre2->setStyleSheet(this->btn_favorite_fre2->styleSheet().replace("fm_fav_btn_press.png", "fm_fav_btn.png"));
}

void RoseFmHome::slot_btnfre3_pressed(){
    if(roseFM_data.isPower == true){
        request_favTune_set(2);
        roseFM_data.lastFreq = roseFM_data.favorite[2];
    }

    this->btn_favorite_fre3->setStyleSheet(this->btn_favorite_fre3->styleSheet().replace("fm_fav_btn_press.png", "fm_fav_btn.png"));
}

void RoseFmHome::slot_btnfre4_pressed(){
    if(roseFM_data.isPower == true){
        request_favTune_set(3);
        roseFM_data.lastFreq = roseFM_data.favorite[3];
    }

    this->btn_favorite_fre4->setStyleSheet(this->btn_favorite_fre4->styleSheet().replace("fm_fav_btn_press.png", "fm_fav_btn.png"));
}

void RoseFmHome::slot_btnfre5_pressed(){
    if(roseFM_data.isPower == true){
        request_favTune_set(4);
        roseFM_data.lastFreq = roseFM_data.favorite[4];
    }

    this->btn_favorite_fre5->setStyleSheet(this->btn_favorite_fre5->styleSheet().replace("fm_fav_btn_press.png", "fm_fav_btn.png"));
}

void RoseFmHome::slot_btnfre6_pressed(){
    if(roseFM_data.isPower == true){
        request_favTune_set(5);
        roseFM_data.lastFreq = roseFM_data.favorite[5];
    }

    this->btn_favorite_fre6->setStyleSheet(this->btn_favorite_fre6->styleSheet().replace("fm_fav_btn_press.png", "fm_fav_btn.png"));
}

void RoseFmHome::slot_volDial_changed(int value){

    tmp_imgvol = vol_pixmap->scaled(SIZE_DIAL_LENGTH, SIZE_DIAL_LENGTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QMatrix tmp_rotvol;

    tmp_rotvol.rotate(ROTATE_VOL_DIAL + (value * 2.45));
    tmp_imgvol = tmp_imgvol.transformed(tmp_rotvol);

    lb_vol_dial->setPixmap(tmp_imgvol);
    lb_vol_dial->setAlignment(Qt::AlignCenter);

    this->volum_val = value;

    //qDebug() << "VOL :" << value << "value :" << this->volum_val << ",degree :" << ROTATE_VOL_DIAL + (value * 0.5);

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("volumeType", "volume_set");
    json.insert("volumeValue", this->volum_val);
    network->request(HTTP_ROSEFM_VOLUME_SET, QString("http://%1:%2/volume").arg(global.device.getDeviceIP()).arg(global.port), json, true);
}

void RoseFmHome::slot_tunDial_changed(int value){

    //qDebug() << "dial rotate Manual" << tun_val_before << value;

    tmp_imgtun = tun_pixmap->scaled(SIZE_DIAL_LENGTH, SIZE_DIAL_LENGTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QMatrix tmp_rottun;

    if(tun_val_before == 0){
        tmp_rottun.rotate(ROTATE_VOL_DIAL + (value * 2.45));
        tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

        lb_tun_dial->setPixmap(tmp_imgtun);
        lb_tun_dial->setAlignment(Qt::AlignCenter);

        this->fre_slider->setValue(value);
        tun_val_before = value;
        return;
    }

    if(value <= 9){
        tmp_rottun.rotate(ROTATE_VOL_DIAL + (9 * 2.45));
        tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

        lb_tun_dial->setPixmap(tmp_imgtun);
        lb_tun_dial->setAlignment(Qt::AlignCenter);

        this->fre_slider->setValue(9);
        this->tun_dial->setValue(9);
        tun_val_before = 9;
    }
    else if(value >= 230){
        tmp_rottun.rotate(ROTATE_VOL_DIAL + (230 * 2.45));
        tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

        lb_tun_dial->setPixmap(tmp_imgtun);
        lb_tun_dial->setAlignment(Qt::AlignCenter);

        this->fre_slider->setValue(230);
        this->tun_dial->setValue(230);
        tun_val_before = 230;
    }
    else if(value > 9 && value < 230){
        if((tun_val_before - value <= 5 || value - tun_val_before <= 5)){
            tmp_rottun.rotate(ROTATE_VOL_DIAL + (value * 2.45));
            tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

            lb_tun_dial->setPixmap(tmp_imgtun);
            lb_tun_dial->setAlignment(Qt::AlignCenter);

            this->fre_slider->setValue(value);

            double frequency_clac = 0.1032 * value + 85.299;

            bool channel_flag = false;
            for(int ch = 0; ch < list_channelInfo.count(); ch++){
                if(list_channelInfo.at(ch).frequency.toDouble() == frequency_clac ||
                        (((list_channelInfo.at(ch).frequency.toDouble() - 0.3) < roseFM_data.lastFreq) && ((list_channelInfo.at(ch).frequency.toDouble() + 0.3) > roseFM_data.lastFreq))){
                    channel_flag = true;
                    auto_tune_idx = ch;
                    break;
                }
            }

            if(channel_flag == true){
                fre_noise_value = 1001;

                roseFM_data.lastFreq = frequency_clac;
                roseFM_data.title = "";
                roseFM_data.trackInfo = "";
                roseFM_data.isStreo = false;
                request_frqTune_set(roseFM_data.lastFreq);
            }
            else if(channel_flag == false){
                if(fre_noise_value < 1005){
                    fre_noise_value++;
                }
                else if(fre_noise_value == 1005){
                    fre_noise_value = 1005;
                }

                roseFM_data.lastFreq = frequency_clac;
                roseFM_data.title = "";
                roseFM_data.trackInfo = "";
                roseFM_data.isStreo = false;
                request_frqTune_set(fre_noise_value);
            }
        }
        /*else{
            if(tun_val_before > value){
                for(int i = value; i <= tun_val_before; i++){
                    tmp_rottun.rotate(ROTATE_VOL_DIAL + (i * 2.45));
                    tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

                    lb_tun_dial->setPixmap(tmp_imgtun);
                    lb_tun_dial->setAlignment(Qt::AlignCenter);

                    this->fre_slider->setValue(i);
                }
            }
            else if(tun_val_before < value){
                for(int i = value; i <= tun_val_before; i--){
                    tmp_rottun.rotate(ROTATE_VOL_DIAL + (i * 2.45));
                    tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

                    lb_tun_dial->setPixmap(tmp_imgtun);
                    lb_tun_dial->setAlignment(Qt::AlignCenter);

                    this->fre_slider->setValue(i);
                }
            }
        }*/

        tun_val_before = value;
    }
}

void RoseFmHome::slot_tunDial_Autotune(int value){

    //qDebug() << "dial rotate Auto" << tun_val_before << value;

    tmp_imgtun = tun_pixmap->scaled(SIZE_DIAL_LENGTH, SIZE_DIAL_LENGTH, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QMatrix tmp_rottun;

    if(tun_val_before == 0){
        tmp_rottun.rotate(ROTATE_VOL_DIAL + (value * 2.45));
        tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

        lb_tun_dial->setPixmap(tmp_imgtun);
        lb_tun_dial->setAlignment(Qt::AlignCenter);

        this->fre_slider->setValue(value);
        tun_val_before = value;

        return;
    }
    else if(value > 9 && value < 230){
        tmp_rottun.rotate(ROTATE_VOL_DIAL + (value * 2.45));
        tmp_imgtun = tmp_imgtun.transformed(tmp_rottun);

        lb_tun_dial->setPixmap(tmp_imgtun);
        lb_tun_dial->setAlignment(Qt::AlignCenter);

        int i = 0;
        if(tun_val_before < value){
            for(i = tun_val_before; i <= value; i++){
                this->fre_slider->setValue(i);
            }
        }
        else if(tun_val_before > value){
            for(i = tun_val_before; i >= value; i--){
                this->fre_slider->setValue(i);
            }
        }

        tun_val_before = value;
    }
}
