#include "AbstractPlaylistTrackDetailInfo_RHV.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "widget/framewidget.h"
#include "widget/optionpopup.h"

#include <QJsonObject>
#include <QTime>
#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include "roseHome/ProcCommon_forRosehome.h"//c220824-4

const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";
const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";
const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";

/**
 * @brief 생성자
 * @param parent
 */
PlaylistTrackDetailInfo_RHV::PlaylistTrackDetailInfo_RHV(QWidget *parent) : QPushButton(parent){

    this->linker = Linker::getInstance();

    this->setUIControl_basic();
}


PlaylistTrackDetailInfo_RHV::~PlaylistTrackDetailInfo_RHV(){

    this->fileDownLoader->deleteLater();
    this->deleteLater();
}


void PlaylistTrackDetailInfo_RHV::setUIControl_basic(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &PlaylistTrackDetailInfo_RHV::slot_fileDownload_loadImage);

    this->playlist_type = "";

    this->widget_info_main = new QWidget();
    this->widget_info_main->setFixedSize(1500, 70);
    this->widget_info_main->setObjectName("trackInfo");

    this->label_track_thumbnail = new QLabel(this->widget_info_main);
    this->label_track_thumbnail->setScaledContents(true);
    this->label_track_thumbnail->setGeometry(5, 5, 60, 60);
    this->label_track_thumbnail->setStyleSheet("background-color:transparent;");

    this->label_track_type = new QLabel(this->widget_info_main);
    this->label_track_type->setGeometry(78, 15, 30, 30);
    this->label_track_type->setStyleSheet("background-color:transparent;");
    this->label_track_type->hide();

    this->label_rank = new QLabel(this->widget_info_main);
    this->label_rank->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:18px;font-weight:bold;");
    this->label_rank->setAlignment(Qt::AlignCenter);
    this->label_rank->hide();

    this->label_rank_ico = new QLabel(this->widget_info_main);
    this->label_rank_ico->setAlignment(Qt::AlignCenter);
    this->label_rank_ico->hide();

    this->label_rank_value = new QLabel(this->widget_info_main);
    this->label_rank_value->setAlignment(Qt::AlignCenter);
    this->label_rank_value->hide();

    this->label_adult_certification = new QLabel(this->widget_info_main);
    this->label_adult_certification->setGeometry(120, 15, 30, 30);
    this->label_adult_certification->setStyleSheet("background-color:transparent;");
    this->label_adult_certification->hide();

    this->label_title = new QLabel(this->widget_info_main);
    this->label_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_title->setGeometry(120, 10, 720, 25);

    this->label_artist = new QLabel(this->widget_info_main);
    this->label_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    this->label_artist->setGeometry(120, 34, 720, 25);

    this->label_track_resolution = new QLabel(this->widget_info_main);
    this->label_track_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    this->label_track_resolution->setGeometry(540, 34, 200, 24);
    this->label_track_resolution->hide();

    this->label_track_hires = new QLabel(this->widget_info_main);
    this->label_track_hires->setStyleSheet("background-color:transparent;border:0px");
    this->label_track_hires->hide();

    this->label_album = new QLabel(this->widget_info_main);
    this->label_album->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_album->setAlignment(Qt::AlignVCenter);
    this->label_album->setWordWrap(true);
    this->label_album->setGeometry(937, 24, 220, 20);

    this->label_duration = new QLabel(this->widget_info_main);
    this->label_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_duration->setGeometry(1231, 24, 65, 20);

    this->btn_music_video = GSCommon::getUIBtnImg("btn_music_video", ":/images/list_video_ico.png", this->widget_info_main);
    this->btn_music_video->setGeometry(1300, 17, 50, 36);
    this->btn_music_video->setCursor(Qt::PointingHandCursor);
    this->btn_music_video->setProperty("type", "musicVideo");
    this->btn_music_video->hide();

    this->btn_play_list_add = GSCommon::getUIBtnImg("btn_play_list_add", ":/images/play_list_add_icon.png", this->widget_info_main);
    this->btn_play_list_add->setGeometry(1350, 10, 50, 50);
    this->btn_play_list_add->setCursor(Qt::PointingHandCursor);
    this->btn_play_list_add->setProperty("type", "playlist");

    this->btn_list_fav = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, this->widget_info_main);
    this->btn_list_fav->setGeometry(1400, 6, 50, 58);
    this->btn_list_fav->setCursor(Qt::PointingHandCursor);
    this->btn_list_fav->setProperty("type", "fav");

    this->btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", this->widget_info_main);
    this->btn_menu->setGeometry(1450, 6, 50, 58);
    this->btn_menu->setCursor(Qt::PointingHandCursor);
    this->btn_menu->setProperty("type", "more");

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->addWidget(this->widget_info_main);

    this->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { background-color:#333333; } ");
    this->setFixedHeight(70);
    this->setLayout(vl_total);
    this->setCursor(Qt::PointingHandCursor);
    this->setProperty("type", "playtrack");

    connect(this->btn_music_video, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_play_list_add, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_list_fav, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_menu, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Music(const QJsonObject &jsonObj){

    this->btn_play_list_add->hide();
    this->btn_list_fav->hide();

    QString album = ProcJsonEasy::getString(jsonObj, "album");
    QString thumbnail = ProcJsonEasy::getString(jsonObj, "album_art");
    QString artist = ProcJsonEasy::getString(jsonObj, "artist");
    QString title = ProcJsonEasy::getString(jsonObj, "title");

    QString tmpMime = ProcJsonEasy::getString(jsonObj, "mime_type");
    int tmpsampleRate = ProcJsonEasy::getInt(jsonObj, "samplerate");
    int tmpbitDepth = ProcJsonEasy::getInt(jsonObj, "bitdepth");

    int duration = ProcJsonEasy::getInt(jsonObj, "duration");
    duration = duration / 1000;

    bool flag_fav = ProcJsonEasy::getBool(jsonObj, "favorite");

    if(jsonObj.contains("favorite")){
        if(flag_fav == false){

            this->btn_play_list_add->setGeometry(1400, 10, 50, 50);

            this->btn_list_fav->hide();
        }
    }

    if(!tmpMime.isEmpty()){
        QString tmp_data = tmpMime;
        QStringList splitToken = tmp_data.split("/");

        if(splitToken.size() > 1){
            QString tmp_type = splitToken.at(1);
            if(tmp_type == "x-wav" || tmp_type == "wav"){
                tmpMime = "WAV";
            }
            else if (tmp_type == "wv") {
                tmpMime = "WV";
            }
            else if(tmp_type == "flac"){
                tmpMime = "FLAC";
            }
            else if(tmp_type == "dff" || tmp_type == "dsf"){
                tmpMime = "DSD";
            }
            else if(tmp_type == "mp4"){
                tmpMime = "ALAC";
            }
            else if(tmp_type == "mpeg"){
                tmpMime = "MP3";
            }
            else if(tmp_type == "amr"){
                tmpMime = "AMR";
            }
            else if(tmp_type == "amr-wb"){
                tmpMime = "AWB";
            }
            else if(tmp_type == "x-ms-wma"){
                tmpMime = "WMA";
            }
            else if(tmp_type == "ape"){
                tmpMime = "APE";
            }
            else if(tmp_type == "ogg"){
                tmpMime = "OGG";
            }
            else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                tmpMime = "AAC";
            }
            else if(tmp_type == "x-matroska"){
                tmpMime = "MKA";
            }
            else if(tmp_type == "midi"){
                tmpMime = "MIDI";
            }
            else if(tmp_type == "sp-midi"){
                tmpMime = "SMF";
            }
            else if(tmp_type == "imelody"){
                tmpMime = "IMY";
            }
            else if(tmp_type == "x-mpegurl"){
                tmpMime = "M3U8";
            }
            else if(tmp_type == "x-scpls"){
                tmpMime = "PLS";
            }
            else if(tmp_type == "aiff"){
                tmpMime = "AIFF";
            }
        }
    }

    QString mimeType = "";
    if(tmpMime == "MP3" || tmpMime == "AMR" || tmpMime == "AWB" || tmpMime == "WMA" || tmpMime == "APE" || tmpMime == "OGG" || tmpMime == "AAC" || tmpMime == "MKA"
            || tmpMime == "MIDI" || tmpMime == "SMF" || tmpMime == "IMY" || tmpMime == "M3U8" || tmpMime == "PLS" || tmpMime == "AIFF" || tmpMime == "WV"){

        mimeType = tmpMime;
    }
    else if(tmpMime == "DSD"){
        if(tmpsampleRate == 88200 || tmpsampleRate == 2822400){
            mimeType = tmpMime + " 64";
        }
        else if(tmpsampleRate == 176400 || tmpsampleRate == 5644800){
            mimeType = tmpMime + " 128";
        }
        else if(tmpsampleRate == 352800 || tmpsampleRate == 11289600){
            mimeType = tmpMime + " 256";
        }
        else if(tmpsampleRate == 705600 || tmpsampleRate == 22579200){
            mimeType = tmpMime + " 512";
        }
        else if(tmpsampleRate == 1411200 || tmpsampleRate == 45158400){
            mimeType = tmpMime + " 1024";
        }
        else{
            mimeType = tmpMime;
        }
    }
    else if(tmpMime == "WAV" || tmpMime == "FLAC" || tmpMime == "ALAC"){
        if(tmpbitDepth == 0 || tmpbitDepth == 16){
            mimeType = tmpMime;
        }
        else if(tmpbitDepth > 16){
            mimeType = tmpMime + QString(" %1").arg(tmpbitDepth);
        }
    }

    this->playlist_type = "MUSIC";

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if(!thumbnail.contains("http://")){
        thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + thumbnail;
    }
    this->setImage(thumbnail);

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 680, this->label_title->font()));
        if(this->label_title->text().contains("…")){
            this->label_title->setToolTip(title);//c230321
            this->label_title->setToolTipDuration(2000);//c230321
        }
        this->label_title->setGeometry(80, 10, 720, 25);
    }
    else{
        this->label_title->setText(title);
        this->label_title->setGeometry(80, 10, title_width, 25);
    }

    if(!mimeType.isEmpty()){
        this->label_track_resolution->setText(QString("%1").arg(mimeType));
        this->label_track_resolution->setAlignment(Qt::AlignCenter);

        int width = this->label_track_resolution->sizeHint().width() + 20;
        int left = 0;
        if(title_width < 720){
            left = this->label_title->geometry().left() + title_width + 15;
        }
        else{
            left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;
        }

        this->label_track_resolution->setGeometry(left, 15, width, 16);
        this->label_track_resolution->show();
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 680){
        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
        if(this->label_artist->text().contains("…")){
            this->label_artist->setToolTip(artist);//c230321
            this->label_artist->setToolTipDuration(2000);//c230321
        }
        this->label_artist->setGeometry(80, 34, 680, 25);
    }
    else{
        this->label_artist->setText(artist);
        this->label_artist->setGeometry(80, 34, artist_width, 25);
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(album);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = album;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(album);//c230321
            this->label_album->setToolTipDuration(2000);//c230321
        }
        this->label_album->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_album->setText(album);
        this->label_album->setGeometry(937, 24, 220, 20);
    }

    if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Tidal(const tidal::TrackItemData &data_track){

    QList<tidal::TrackItemData> *list_track = new QList<tidal::TrackItemData>();
    list_track->append(data_track);

    this->label_adult_certification->hide();

    this->playlist_type = "TIDAL";

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/tidal/tidal_def_400.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(list_track->at(0).image);

    QString title_version = "";
    QString title_total = "";

    if(!list_track->at(0).version.isEmpty()){
        title_version = " (" + list_track->at(0).version +")";
    }

    title_total = list_track->at(0).title + title_version;

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title_total);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QString mqa_path = ":/images/tidal/mqa_ico.png";

    QString artist = list_track->at(0).list_artist_name.join(",");
    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(list_track->at(0).adult_certification == true){
        QString adult_certification_path = "";
        if(global.lang == 0){
            adult_certification_path = ":/images/tidal/tidal-e-ico.png";
        }
        else{
            adult_certification_path = ":/images/bugs/ico-19.png";
        }
        QImage img;
        QPixmap *img_adult_certification = new QPixmap();
        if(img.load(adult_certification_path)){
            *img_adult_certification = QPixmap::fromImage(img);
            *img_adult_certification = img_adult_certification->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        this->label_adult_certification->setPixmap(*img_adult_certification);
        this->label_adult_certification->setFixedSize(30, 30);

        this->label_adult_certification->setGeometry(85, 20, 0, 0);
        this->label_adult_certification->show();

        if(list_track->at(0).audioQuality == "HI_RES"){
            title_width += (15 + 36);

            int title_width_resize = 0;
            if(title_width > 680){
                title_width_resize = 680 - (15 + 36);
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title_total);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }
                this->label_title->setGeometry(135, 10, title_width_resize, 25);
            }
            else{
                title_width_resize = title_width - (15 + 36);
                this->label_title->setText(title_total);
                this->label_title->setGeometry(135, 10, title_width_resize, 25);
            }

            QImage img;
            QPixmap *img_hires = new QPixmap();
            if(img.load(mqa_path)){
                *img_hires = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
                *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
            }

            this->label_track_hires->setPixmap(*img_hires);
            this->label_track_hires->setFixedSize(36, 13);

            int left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

            this->label_track_hires->setGeometry(left, 18, 0, 0);
            this->label_track_hires->show();
        }
        else{
            if(title_width > 680){
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 680, this->label_title->font()));
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title_total);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }
                this->label_title->setGeometry(135, 10, 680, 25);
            }
            else{
                this->label_title->setText(title_total);
                this->label_title->setGeometry(135, 10, title_width, 25);
            }
        }

        if(artist_width > 680){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);//c230321
                this->label_artist->setToolTipDuration(2000);//c230321
            }
            this->label_artist->setGeometry(135, 34, 680, 25);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(135, 34, artist_width, 25);
        }
    }
    else{
        if(list_track->at(0).audioQuality == "HI_RES"){
            title_width += (15 + 36);

            int title_width_resize = 0;
            if(title_width > 720){
                title_width_resize = 720 - (15 + 36);
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title_total);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }
                this->label_title->setGeometry(85, 10, title_width_resize, 25);
            }
            else{
                title_width_resize = title_width - (15 + 36);
                this->label_title->setText(title_total);
                this->label_title->setGeometry(85, 10, title_width_resize, 25);
            }

            QImage img;
            QPixmap *img_hires = new QPixmap();
            if(img.load(mqa_path)){
                *img_hires = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
                *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
            }

            this->label_track_hires->setPixmap(*img_hires);
            this->label_track_hires->setFixedSize(36, 13);

            int left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

            this->label_track_hires->setGeometry(left, 18, 0, 0);
            this->label_track_hires->show();
        }
        else{
            if(title_width > 720){
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 720, this->label_title->font()));
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title_total);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }
                this->label_title->setGeometry(85, 10, 720, 25);
            }
            else{
                this->label_title->setText(title_total);
                this->label_title->setGeometry(85, 10, title_width, 25);
            }
        }

        if(artist_width > 680){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);//c230321
                this->label_artist->setToolTipDuration(2000);//c230321
            }
            this->label_artist->setGeometry(85, 34, 680, 25);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(85, 34, artist_width, 25);
        }
    }


    /*if(!list_track->at(0).quality.isEmpty()){
        this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).audioQuality));
        this->label_track_resolution->setAlignment(Qt::AlignCenter);

        int width = this->label_track_resolution->sizeHint().width() + 10;;
        int left = 0;
        if(artist_width < 580){
            left = this->label_artist->geometry().left() + artist_width + 15;
        }
        else{
            left = this->label_artist->geometry().left() + this->label_artist->geometry().width() + 15;
        }

        this->label_track_resolution->setGeometry(left, 40, width, 16);
        this->label_track_resolution->show();
    }*/

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(list_track->at(0).albumName);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = list_track->at(0).albumName;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(list_track->at(0).albumName, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(list_track->at(0).albumName);//c230321
            this->label_album->setToolTipDuration(2000);//c230321
        }
        this->label_album->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_album->setText(list_track->at(0).albumName);
        this->label_album->setGeometry(937, 24, 220, 20);
    }

    if(data_track.duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("mm:ss"));
    }

    if(list_track->at(0).streamReady == false){
        this->label_title->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:normal;");
        this->label_artist->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:300;");
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#666666;font-size:12px;font-weight:300;border:1px solid #666666;border-radius:8px;");
        this->label_album->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");
        this->label_duration->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");

        this->setEnabled(false);
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Bugs(const bugs::TrackItemData &data_track, const QString &type){

    QList<bugs::TrackItemData> *list_track = new QList<bugs::TrackItemData>();
    list_track->append(data_track);

    this->playlist_type = "BUGS";

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/bugs/bugs_def_430.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(list_track->at(0).album_image);

    this->btn_play_list_add->hide();
    this->btn_music_video->setGeometry(1350, 17, 50, 36);

    if(list_track->at(0).list_mv_id.count() > 0){
        this->btn_music_video->show();
    }

    if(type == "Chart"){
        this->label_rank->hide();
        this->label_rank_ico->hide();
        this->label_rank_value->hide();

        QImage img_up;
        QPixmap *img_ranking_up = new QPixmap();
        if(img_up.load(":/images/bugs/ranking_ico1.png")){
            *img_ranking_up = QPixmap::fromImage(img_up);
            *img_ranking_up = img_ranking_up->scaled(10, 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        QImage img_down;
        QPixmap *img_ranking_down = new QPixmap();
        if(img_down.load(":/images/bugs/ranking_ico2.png")){
            *img_ranking_down = QPixmap::fromImage(img_down);
            *img_ranking_down = img_ranking_down->scaled(10, 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        QImage img_same;
        QPixmap *img_ranking_same = new QPixmap();
        if(img_same.load(":/images/bugs/ranking_ico3.png")){
            *img_ranking_same = QPixmap::fromImage(img_same);
            *img_ranking_same = img_ranking_same->scaled(10, 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->label_rank->setText(QString("%1").number(this->property("index").toInt() + 1));
        this->label_rank->setGeometry(72, 10, 60, 20);
        this->label_rank->show();


        if(list_track->at(0).rank_code == "UP"){
            this->label_rank_ico->setPixmap(*img_ranking_up);
            this->label_rank_ico->setGeometry(91, 45, 10, 10);
            this->label_rank_ico->show();

            this->label_rank_value->setText(list_track->at(0).rank_code_value);
            this->label_rank_value->setGeometry(103, 41, 14, 20);
            this->label_rank_value->setStyleSheet("background-color:transparent;color:#FF0000;font-size:14px;font-weight:300;line-height: 2.68;");
            this->label_rank_value->show();

        }
        else if(list_track->at(0).rank_code == "DOWN"){
            this->label_rank_ico->setPixmap(*img_ranking_down);
            this->label_rank_ico->setGeometry(91, 46, 10, 10);
            this->label_rank_ico->setStyleSheet("color:#ACACAC;");
            this->label_rank_ico->show();

            this->label_rank_value->setText(list_track->at(0).rank_code_value);
            this->label_rank_value->setGeometry(103, 41, 14, 20);
            this->label_rank_value->setStyleSheet("background-color:transparent;color:#ACACAC;font-size:14px;font-weight:300;line-height: 2.68;");
            this->label_rank_value->show();
        }
        else{
            this->label_rank_ico->setPixmap(*img_ranking_same);
            this->label_rank_ico->setGeometry(97, 45, 10, 10);
            this->label_rank_ico->setStyleSheet("color:#ACACAC;");
            this->label_rank_ico->show();

            this->label_rank_value->hide();
        }

        this->label_adult_certification->hide();

        QString title_total = list_track->at(0).track_title;

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");

        tmp_title->setText(title_total);

        int title_width = 0;
        title_width = tmp_title->sizeHint().width();

        QString artist = list_track->at(0).list_artist_nm.join(",");

        if(list_track->at(0).adult_yn == true){
            QString adult_yn_path = "";
            if(global.lang == 0){
                adult_yn_path = ":/images/tidal/tidal-e-ico.png";
            }
            else{
                adult_yn_path = ":/images/bugs/ico-19.png";
            }
            QImage img;
            QPixmap *img_adult_yn = new QPixmap();
            if(img.load(adult_yn_path)){
                *img_adult_yn = QPixmap::fromImage(img);
                *img_adult_yn = img_adult_yn->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            this->label_adult_certification->setPixmap(*img_adult_yn);
            this->label_adult_certification->setFixedSize(30, 30);

            this->label_adult_certification->setGeometry(135 , 20, 0, 0);
            this->label_adult_certification->show();

            if(!list_track->at(0).bitrates.isEmpty()){

                this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                this->label_track_resolution->setAlignment(Qt::AlignCenter);

                int width = this->label_track_resolution->sizeHint().width() + 20;
                int left = 0;

                title_width += (width + 15);

                int title_width_resize = 0;
                if(title_width > 630){
                    title_width_resize = 630 - (width + 15);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);//c230321
                        this->label_title->setToolTipDuration(2000);//c230321
                    }
                    this->label_title->setGeometry(190, 10, title_width_resize, 25);

                    left = this->label_title->geometry().left() + title_width + 15;
                }
                else{
                    title_width_resize = title_width - (width + 15);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(190, 10, title_width_resize, 25);

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;
                }

                this->label_track_resolution->setGeometry(left, 15, width, 16);
                this->label_track_resolution->show();
            }
            else{
                if(title_width > 630){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 630, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);//c230321
                        this->label_title->setToolTipDuration(2000);//c230321
                    }
                    this->label_title->setGeometry(140, 10, 630, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(140, 10, title_width, 25);
                }
            }

            QLabel *tmp_artist = new QLabel();
            tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
            tmp_artist->setText(artist);

            int artist_width = 0;
            artist_width = tmp_artist->sizeHint().width();

            if(artist_width > 520){
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 520, this->label_title->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(artist);//c230321
                    this->label_artist->setToolTipDuration(2000);//c230321
                }
                this->label_artist->setGeometry(190, 34, 520, 25);
            }
            else{
                this->label_artist->setText(artist);
                this->label_artist->setGeometry(190, 34, artist_width, 25);
            }
        }
        else{
            if(!list_track->at(0).bitrates.isEmpty()){

                this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                this->label_track_resolution->setAlignment(Qt::AlignCenter);

                int width = this->label_track_resolution->sizeHint().width() + 20;
                int left = 0;

                title_width += (width + 15);

                int title_width_resize = 0;
                if(title_width > 680){
                    title_width_resize = 680 - (width + 15);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);//c230321
                        this->label_title->setToolTipDuration(2000);//c230321
                    }
                    this->label_title->setGeometry(140, 10, title_width_resize, 25);

                    left = this->label_title->geometry().left() + title_width_resize + 15;
                }
                else{
                    title_width_resize = title_width - (width + 15);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(140, 10, title_width_resize, 25);

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;
                }

                this->label_track_resolution->setGeometry(left, 15, width, 16);
                this->label_track_resolution->show();
            }
            else{
                if(title_width > 680){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 680, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);//c230321
                        this->label_title->setToolTipDuration(2000);//c230321
                    }
                    this->label_title->setGeometry(140, 10, 680, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(140, 10, title_width, 25);
                }
            }

            QLabel *tmp_artist = new QLabel();
            tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
            tmp_artist->setText(artist);

            int artist_width = 0;
            artist_width = tmp_artist->sizeHint().width();

            if(artist_width > 520){
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 520, this->label_title->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(artist);//c230321
                    this->label_artist->setToolTipDuration(2000);//c230321
                }
                this->label_artist->setGeometry(140, 34, 520, 25);
            }
            else{
                this->label_artist->setText(artist);
                this->label_artist->setGeometry(140, 34, artist_width, 25);
            }
        }

        QLabel *tmp_album_title = new QLabel();
        tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        tmp_album_title->setText(list_track->at(0).album_title);

        int album_title_width = 0;
        album_title_width = tmp_album_title->sizeHint().width();

        if(album_title_width > 220){
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_album_set = "";

            tmp_split = list_track->at(0).album_title;
            splitToken = tmp_split.split(" ");

            tmp_album_title->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_album_set = splitToken.at(i);
                    }
                    else{
                        tmp_album_set += " " + splitToken.at(i);
                    }
                    tmp_album_title->setText(tmp_album_set);

                    if(tmp_album_title->sizeHint().width() >= 220){
                        tmp_album_title->setText("");
                        tmp_album_set.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_album_title->setText("");
            tmp_album_title->setText(tmp_album_set);

            album_title_width = tmp_album_title->sizeHint().width() + 220;

            this->label_album->setText(GSCommon::getTextCutFromLabelWidth(list_track->at(0).album_title, album_title_width, this->label_album->font()));
            if(this->label_album->text().contains("…")){
                this->label_album->setToolTip(list_track->at(0).album_title);//c230321
                this->label_album->setToolTipDuration(2000);//c230321
            }
            this->label_album->setGeometry(937, 14, 220, 40);
        }
        else{
            this->label_album->setText(list_track->at(0).album_title);
            this->label_album->setGeometry(937, 24, 220, 20);
        }

        if(!list_track->at(0).len.isEmpty()){

            QString duration = "";
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = list_track->at(0).len;
            splitToken = tmp_split.split(":");

            if(splitToken.size() > 0){
                for(int i = 0; i < splitToken.size(); i++){
                    int tmp_number = splitToken.at(i).toInt();

                    if(i == splitToken.size() - 1){
                        duration += QString::number(tmp_number).rightJustified(2, '0');
                    }
                    else{
                        duration += QString::number(tmp_number).rightJustified(2, '0') + ":";
                    }
                }

                this->label_duration->setText(duration);
            }
        }
    }
    else{

        this->label_adult_certification->hide();

        QString title_total = list_track->at(0).track_title;

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        tmp_title->setText(title_total);

        int title_width = 0;
        title_width = tmp_title->sizeHint().width();

        QString artist = list_track->at(0).list_artist_nm.join(",");

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(list_track->at(0).adult_yn == true){
            QString adult_yn_path = "";
            if(global.lang == 0){
                adult_yn_path = ":/images/tidal/tidal-e-ico.png";
            }
            else{
                adult_yn_path = ":/images/bugs/ico-19.png";
            }
            QImage img;
            QPixmap *img_parental_warning = new QPixmap();
            if(img.load(adult_yn_path)){
                *img_parental_warning = QPixmap::fromImage(img);
                *img_parental_warning = img_parental_warning->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            this->label_adult_certification->setPixmap(*img_parental_warning);
            this->label_adult_certification->setFixedSize(30, 30);

            this->label_adult_certification->setGeometry(85, 20, 0, 0);
            this->label_adult_certification->show();

            if(!list_track->at(0).bitrates.isEmpty()){
                this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                this->label_track_resolution->setAlignment(Qt::AlignCenter);

                int width = this->label_track_resolution->sizeHint().width() + 20;
                int left = 0;

                title_width += (width + 15);

                int title_width_resize = 0;
                if(title_width > 680){
                    title_width_resize = 680 - (width + 15);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (width + 15);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }

                if(artist_width > 680){
                    this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
                    if(this->label_artist->text().contains("…")){
                        this->label_artist->setToolTip(artist);
                        this->label_artist->setToolTipDuration(2000);
                    }
                    this->label_artist->setGeometry(135, 34, 680, 25);
                }
                else{
                    this->label_artist->setText(artist);
                    this->label_artist->setGeometry(135, 34, artist_width, 25);
                }

                if(title_width > 680){
                    left = this->label_title->geometry().left() + title_width + 15;
                }
                else{
                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;
                }

                this->label_track_resolution->setGeometry(left, 15, width, 16);
                this->label_track_resolution->show();
            }
            else{
                if(title_width > 680){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 680, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, 680, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width, 25);
                }

                if(artist_width > 680){
                    this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
                    if(this->label_artist->text().contains("…")){
                        this->label_artist->setToolTip(artist);
                        this->label_artist->setToolTipDuration(2000);
                    }
                    this->label_artist->setGeometry(135, 34, 680, 25);
                }
                else{
                    this->label_artist->setText(artist);
                    this->label_artist->setGeometry(135, 34, artist_width, 25);
                }
            }
        }
        else{
            if(!list_track->at(0).bitrates.isEmpty()){
                this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                this->label_track_resolution->setAlignment(Qt::AlignCenter);

                int width = this->label_track_resolution->sizeHint().width() + 20;
                int left = 0;

                title_width += (width + 15);

                int title_width_resize = 0;
                if(title_width > 720){
                    title_width_resize = 720 - (width + 15);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (width + 15);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }

                if(artist_width > 680){
                    this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
                    if(this->label_artist->text().contains("…")){
                        this->label_artist->setToolTip(artist);
                        this->label_artist->setToolTipDuration(2000);
                    }
                    this->label_artist->setGeometry(85, 34, 680, 25);
                }
                else{
                    this->label_artist->setText(artist);
                    this->label_artist->setGeometry(85, 34, artist_width, 25);
                }

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_resolution->setGeometry(left, 15, width, 16);
                this->label_track_resolution->show();
            }
            else{
                if(title_width > 720){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 720, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, 720, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width, 25);
                }

                if(artist_width > 680){
                    this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
                    if(this->label_artist->text().contains("…")){
                        this->label_artist->setToolTip(artist);
                        this->label_artist->setToolTipDuration(2000);
                    }
                    this->label_artist->setGeometry(85, 34, 680, 25);
                }
                else{
                    this->label_artist->setText(artist);
                    this->label_artist->setGeometry(85, 34, artist_width, 25);
                }
            }
        }

        QLabel *tmp_album_title = new QLabel();
        tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        tmp_album_title->setText(list_track->at(0).album_title);

        int album_title_width = 0;
        album_title_width = tmp_album_title->sizeHint().width();

        if(album_title_width > 220){
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_album_set = "";

            tmp_split = list_track->at(0).album_title;
            splitToken = tmp_split.split(" ");

            tmp_album_title->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_album_set = splitToken.at(i);
                    }
                    else{
                        tmp_album_set += " " + splitToken.at(i);
                    }
                    tmp_album_title->setText(tmp_album_set);

                    if(tmp_album_title->sizeHint().width() >= 220){
                        tmp_album_title->setText("");
                        tmp_album_set.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_album_title->setText("");
            tmp_album_title->setText(tmp_album_set);

            album_title_width = tmp_album_title->sizeHint().width() + 220;

            this->label_album->setText(GSCommon::getTextCutFromLabelWidth(list_track->at(0).album_title, album_title_width, this->label_album->font()));
            if(this->label_album->text().contains("…")){
                this->label_album->setToolTip(list_track->at(0).album_title);
                this->label_album->setToolTipDuration(2000);
            }
            this->label_album->setGeometry(937, 14, 220, 40);
        }
        else{
            this->label_album->setText(list_track->at(0).album_title);
            this->label_album->setGeometry(937, 24, 220, 20);
        }

        if(!list_track->at(0).len.isEmpty()){
            //this->label_duration->setText(list_track->at(0).len);

            QString duration = "";
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = list_track->at(0).len;
            splitToken = tmp_split.split(":");

            if(splitToken.size() > 0){
                for(int i = 0; i < splitToken.size(); i++){
                    int tmp_number = splitToken.at(i).toInt();

                    if(i == splitToken.size() - 1){
                        duration += QString::number(tmp_number).rightJustified(2, '0');
                    }
                    else{
                        duration += QString::number(tmp_number).rightJustified(2, '0') + ":";
                    }
                }

                this->label_duration->setText(duration);
            }
        }
    }

    if(list_track->at(0).service_yn == false){
        this->label_title->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:normal;");
        this->label_artist->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:300;");
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#666666;font-size:12px;font-weight:300;border:1px solid #666666;border-radius:8px;");
        this->label_album->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");
        this->label_duration->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");

        this->setEnabled(false);
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Qobuz(const qobuz::TrackItemData &data_track){

    QList<qobuz::TrackItemData> *list_track = new QList<qobuz::TrackItemData>();
    list_track->append(data_track);

    this->playlist_type = "QOBUZ";

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/qobuz/qobuz_default_400.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(list_track->at(0).album_image_thumbnail);

    QString title_work = "";
    QString title_version = "";
    QString title_total = "";

    if(!list_track->at(0).work.isEmpty()){
        title_work = list_track->at(0).work + " - ";
    }

    if(!list_track->at(0).version.isEmpty()){
        title_version = " (" + list_track->at(0).version +")";
    }

    title_total = title_work + list_track->at(0).title + title_version;

    QString hires_path = ":/images/qobuz/hires_ico.png";

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title_total);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QString performer = list_track->at(0).list_artist_name.join(",");
    if(performer.isEmpty()){
        performer = list_track->at(0).performer_name;
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(performer);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    this->label_adult_certification->hide();

    int width = 0;
    int left = 0;
    if(list_track->at(0).parental_warning == true){

        QString parental_warning_path = "";
        if(global.lang == 0){
            parental_warning_path = ":/images/tidal/tidal-e-ico.png";
        }
        else{
            parental_warning_path = ":/images/bugs/ico-19.png";
        }
        QImage img;
        QPixmap *img_parental_warning = new QPixmap();
        if(img.load(parental_warning_path)){
            *img_parental_warning = QPixmap::fromImage(img);
            *img_parental_warning = img_parental_warning->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        this->label_adult_certification->setPixmap(*img_parental_warning);
        this->label_adult_certification->setFixedSize(30, 30);

        this->label_adult_certification->setGeometry(85, 20, 0, 0);
        this->label_adult_certification->show();

        if(list_track->at(0).maximum_sampling_rate > 0){
            this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
            this->label_track_resolution->setAlignment(Qt::AlignCenter);

            width = this->label_track_resolution->sizeHint().width() + 20;

            if(list_track->at(0).hires == true){
                title_width += (15 + width + 15 + 20);

                int title_width_resize = 0;
                if(title_width > 680){
                    title_width_resize = 680 - (15 + width + 15 + 20);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width + 15 + 20);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(20, 20);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 14, 0, 0);
                this->label_track_hires->show();

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15 + 20 + 15;

                this->label_track_resolution->setGeometry(left, 16, width, 16);
                this->label_track_resolution->show();
            }
            else{
                title_width += (15 + width);

                int title_width_resize = 0;
                if(title_width > 680){
                    title_width_resize = 680 - (15 + width);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_resolution->setGeometry(left, 16, width, 16);
                this->label_track_resolution->show();
            }
        }
        else{
            if(list_track->at(0).hires == true){
                title_width += (15 + 20);

                int title_width_resize = 0;
                if(title_width > 680){
                    title_width_resize = 680 - (15 + 20);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 20);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(20, 20);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 14, 0, 0);
                this->label_track_hires->show();
            }
            else{
                if(title_width > 680){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 680, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, 720, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 680){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 680, this->label_title->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(performer);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(135, 34, 680, 25);
        }
        else{
            this->label_artist->setText(performer);
            this->label_artist->setGeometry(135, 34, artist_width, 25);
        }
    }
    else{
        if(list_track->at(0).maximum_sampling_rate > 0){
            this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
            this->label_track_resolution->setAlignment(Qt::AlignCenter);

            width = this->label_track_resolution->sizeHint().width() + 20;

            if(list_track->at(0).hires == true){
                title_width += (15 + width) + 15 + 20;

                int title_width_resize = 0;
                if(title_width > 720){
                    title_width_resize = 720 - (15 + width + 15 + 20);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width + 15 + 20);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(20, 20);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 14, 0, 0);
                this->label_track_hires->show();

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15 + 20 + 15;

                this->label_track_resolution->setGeometry(left, 16, width, 16);
                this->label_track_resolution->show();
            }
            else{
                title_width += (15 + width);

                int title_width_resize = 0;
                if(title_width > 720){
                    title_width_resize = 720 - (15 + width);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_resolution->setGeometry(left, 16, width, 16);
                this->label_track_resolution->show();
            }
        }
        else{
            if(list_track->at(0).hires == true){
                title_width += (15 + 20);

                int title_width_resize = 0;
                if(title_width > 720){
                    title_width_resize = 720 - (15 + 20);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 20);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(20, 20);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 14, 0, 0);
                this->label_track_hires->show();
            }
            else{
                if(title_width > 720){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 720, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, 720, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 680){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 680, this->label_title->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(performer);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(85, 34, 680, 25);
        }
        else{
            this->label_artist->setText(performer);
            this->label_artist->setGeometry(85, 34, artist_width, 25);
        }
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(list_track->at(0).album_title);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = list_track->at(0).album_title;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(list_track->at(0).album_title, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(list_track->at(0).album_title);
            this->label_album->setToolTipDuration(2000);
        }
        this->label_album->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_album->setText(list_track->at(0).album_title);
        this->label_album->setGeometry(937, 24, 220, 20);
    }

    if(list_track->at(0).duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(list_track->at(0).duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(list_track->at(0).duration).toUTC().toString("mm:ss"));
    }

    if(list_track->at(0).streamable == false){
        this->label_title->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:normal;");
        this->label_artist->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:300;");
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#666666;font-size:12px;font-weight:300;border:1px solid #666666;border-radius:8px;");
        this->label_album->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");
        this->label_duration->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");

        this->setEnabled(false);
    }
}


void PlaylistTrackDetailInfo_RHV::setDataAlbumInfo_Qobuz(const qobuz::AlbumItemData &data_album){

    QList<qobuz::AlbumItemData> *list_album = new QList<qobuz::AlbumItemData>();
    list_album->append(data_album);

    this->playlist_type = "QOBUZ";

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/qobuz/qobuz_default_400.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(list_album->at(0).image_thumbnail);

    QString title_version = "";
    QString title_total = "";

    if(!list_album->at(0).version.isEmpty()){
        title_version = " (" + list_album->at(0).version +")";
    }

    title_total = list_album->at(0).title + title_version;

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title_total);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 720, this->label_title->font()));
        if(this->label_title->text().contains("…")){
            this->label_title->setToolTip(title_total);
            this->label_title->setToolTipDuration(2000);
        }
        this->label_title->setGeometry(80, 10, 720, 25);
    }
    else{
        this->label_title->setText(title_total);
        this->label_title->setGeometry(80, 10, title_width, 25);
    }

    QString artist = list_album->at(0).artist_name;
    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 680){
        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->label_title->font()));
        if(this->label_artist->text().contains("…")){
            this->label_artist->setToolTip(artist);
            this->label_artist->setToolTipDuration(2000);
        }
        this->label_artist->setGeometry(80, 34, 680, 25);
    }
    else{
        this->label_artist->setText(artist);
        this->label_artist->setGeometry(80, 34, artist_width, 25);
    }

    QString hires_path = ":/images/qobuz/hires_ico.png";
    if(list_album->at(0).hires == true){

        //QPixmap *img_hires = GSCommon::getUIPixmapImg(hires_path);

        QImage img;
        QPixmap *img_hires = new QPixmap();
        if(img.load(hires_path)){
            *img_hires = QPixmap::fromImage(img);                                        //이미지를 버퍼에 옮긴다.
            *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);                //이미지 사이즈 조절
        }

        this->label_track_hires->setPixmap(*img_hires);
        this->label_track_hires->setFixedSize(20, 20);

        int left = this->label_title->geometry().left() + this->label_title->geometry().width() + 10;

        this->label_track_hires->setGeometry(left, 14, 0, 0);
        this->label_track_hires->show();
    }

    if(list_album->at(0).maximum_sampling_rate > 0){
        this->label_track_resolution->setText(QString("%1 kHz").arg(list_album->at(0).maximum_sampling_rate));
        this->label_track_resolution->setAlignment(Qt::AlignCenter);

        int width = this->label_track_resolution->sizeHint().width() + 10;;
        int left = 0;
        if(artist_width < 580){
            left = this->label_artist->geometry().left() + artist_width + 15;
        }
        else{
            left = this->label_artist->geometry().left() + this->label_artist->geometry().width() + 15;
        }

        this->label_track_resolution->setGeometry(left, 40, width, 16);
        this->label_track_resolution->show();
    }

    if(list_album->at(0).duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(list_album->at(0).duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(list_album->at(0).duration).toUTC().toString("mm:ss"));
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Apple(const QJsonObject &trackInfo){

    this->btn_play_list_add->hide();

    QJsonObject attributes = ProcJsonEasy::getJsonObject(trackInfo, "attributes");

    this->playlist_type = "APPLE_MUSIC";

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/apple/apple_def.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QJsonObject artwork = ProcJsonEasy::getJsonObject(attributes, "artwork");
    QString url = ProcJsonEasy::getString(artwork, "url");
    url.replace("{w}x{h}", "60x60");

    this->setImage(url);

    QString title = ProcJsonEasy::getString(attributes, "name");

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->label_title->font()));
        if(this->label_title->text().contains("…")){
            this->label_title->setToolTip(title);
            this->label_title->setToolTipDuration(2000);
        }
        this->label_title->setGeometry(85, 10, 720, 25);
    }
    else{
        this->label_title->setText(title);
        this->label_title->setGeometry(85, 10, title_width, 25);
    }

    QString performer = ProcJsonEasy::getString(attributes, "artistName");
    if(performer.isEmpty()){
        performer = ProcJsonEasy::getString(attributes, "composerName");
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(performer);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 720){
        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 720, this->label_artist->font()));
        if(this->label_artist->text().contains("…")){
            this->label_artist->setToolTip(performer);
            this->label_artist->setToolTipDuration(2000);
        }
        this->label_artist->setGeometry(85, 34, 720, 25);
    }
    else{
        this->label_artist->setText(performer);
        this->label_artist->setGeometry(85, 34, artist_width, 25);
    }

    QString albumName = ProcJsonEasy::getString(attributes, "albumName");

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(albumName);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = albumName;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(albumName, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(albumName);
            this->label_album->setToolTipDuration(2000);
        }
        this->label_album->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_album->setText(albumName);
        this->label_album->setGeometry(937, 24, 220, 20);
    }

    int duration = ProcJsonEasy::getInt(attributes, "durationInMillis") / 1000;
    if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Rose(const roseHome::TrackItemData &data_track){

    QList<roseHome::TrackItemData> *list_track = new QList<roseHome::TrackItemData>();
    list_track->append(data_track);

    this->btn_play_list_add->hide();

    this->playlist_type = list_track->at(0).type;

    QString default_path = "";
    QString type_path = "";
    if(this->playlist_type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(this->playlist_type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(this->playlist_type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(this->playlist_type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
    }
    else if(this->playlist_type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(this->playlist_type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
    }
    else if(this->playlist_type == "APPLE_MUSIC"){
        default_path = ":/images/apple/apple_def.png";
        type_path = ":/images/rosehome/q_apple_play_ico.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(default_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(list_track->at(0).thumbnail);

    QImage image;
    image.load(type_path);

    QPixmap pixmapIMG = QPixmap(QSize(image.width(), image.height()));
    pixmapIMG.fill(Qt::transparent);

    QPixmap tmp_pixmap;
    tmp_pixmap = tmp_pixmap.fromImage(image);
    tmp_pixmap = tmp_pixmap.scaled(image.width(), image.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter (&pixmapIMG);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path = QPainterPath();
    path.addRoundedRect(0, 0, image.width(), image.height(), 0, 0);

    painter.setClipPath(path);

    if(this->playlist_type == "MUSIC"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(70, 14, image.width(), image.height());
    }
    else if(this->playlist_type == "VIDEO"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(68, 15, image.width(), image.height());
    }
    else if(this->playlist_type == "YOUTUBE"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(68, 15, image.width(), image.height());
    }
    else if(this->playlist_type == "TIDAL"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(66, 15, image.width(), image.height());
    }
    else if(this->playlist_type == "BUGS"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(70, 12, image.width(), image.height());
    }
    else if(this->playlist_type == "QOBUZ"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(70, 13, image.width(), image.height());
    }
    else if(this->playlist_type == "APPLE_MUSIC"){
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(78, 25, image.width(), image.height());
    }
    else{
        painter.drawPixmap(0, 0, tmp_pixmap);
        this->label_track_type->setGeometry(70, 15, image.width(), image.height());
    }

    painter.end();

    this->label_track_type->setStyleSheet("background-color:transparent;");
    this->label_track_type->setPixmap(pixmapIMG);
    this->label_track_type->show();

    QString title_work = "";
    QString title_version = "";
    QString title_total = "";

    if(!list_track->at(0).work.isEmpty()){
        title_work = list_track->at(0).work + " - ";
    }

    if(!list_track->at(0).version.isEmpty()){
        title_version = " (" + list_track->at(0).version +")";
    }

    title_total = title_work + list_track->at(0).title + title_version;

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title_total);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QString performer;
    if(this->playlist_type == "QOBUZ"){
        performer = list_track->at(0).list_artist_name.join(",");
        if(performer.isEmpty()){
            performer = list_track->at(0).performer_name;
        }
    }
    else{
        performer = list_track->at(0).list_artist_name.join(",");
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(performer);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    this->label_adult_certification->hide();
    this->label_track_hires->hide();

    int width = 0;
    int left = 0;
    if(list_track->at(0).adult_certification == true){
        QString adult_certification_path = "";

        if(global.lang == 0){
            adult_certification_path = ":/images/tidal/tidal-e-ico.png";
        }
        else{
            adult_certification_path = ":/images/bugs/ico-19.png";
        }
        QImage img;
        QPixmap *img_adult_certification = new QPixmap();
        if(img.load(adult_certification_path)){
            *img_adult_certification = QPixmap::fromImage(img);
            *img_adult_certification = img_adult_certification->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        this->label_adult_certification->setPixmap(*img_adult_certification);
        this->label_adult_certification->setFixedSize(30, 30);

        this->label_adult_certification->setGeometry(120, 20, 0, 0);
        this->label_adult_certification->show();

        if(list_track->at(0).hires == true){
            QString hires_path = "";

            if(this->playlist_type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 640){
                    title_width_resize = 640 - (15 + 36);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(170, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(170, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(36, 13);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 18, 0, 0);
                this->label_track_hires->show();
            }
            else if(this->playlist_type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width + 15 + 20);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    this->label_track_hires->setPixmap(*img_hires);
                    this->label_track_hires->setFixedSize(20, 20);

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_hires->setGeometry(left, 14, 0, 0);
                    this->label_track_hires->show();

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15 + 20 + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 16, width, 16);
                    this->label_track_resolution->show();
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 640){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 640, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, 640, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(this->playlist_type == "QOBUZ"){

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 640){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, title_width, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width, 25);
                    }
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 640){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 640, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(170, 10, 640, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(170, 10, title_width, 25);
                    }
                }
            }
            else{
                if(title_width > 640){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 640, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(170, 10, 640, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(170, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 680){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 680, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(performer);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(170, 34, 680, 25);
        }
        else{
            this->label_artist->setText(performer);
            this->label_artist->setGeometry(170, 34, artist_width, 25);
        }
    }
    else{
        if(list_track->at(0).hires == true){
            QString hires_path = "";

            if(this->playlist_type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 690){
                    title_width_resize = 690 - (15 + 36);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(120, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(120, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(36, 13);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 18, 0, 0);
                this->label_track_hires->show();
            }
            else if(this->playlist_type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width + 15 + 20);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    this->label_track_hires->setPixmap(*img_hires);
                    this->label_track_hires->setFixedSize(20, 20);

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_hires->setGeometry(left, 14, 0, 0);
                    this->label_track_hires->show();

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15 + 20 + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 16, width, 16);
                    this->label_track_resolution->show();
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 690){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 690, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, 690, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(this->playlist_type == "QOBUZ"){

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 690){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width, 25);
                    }
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 690){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 690, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(120, 10, title_width, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(120, 10, 690, 25);
                    }
                }
            }
            else{
                if(title_width > 690){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 690, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(120, 10, 690, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(120, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 680){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 680, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(performer);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(120, 34, 680, 25);
        }
        else{
            this->label_artist->setText(performer);
            this->label_artist->setGeometry(120, 34, artist_width, 25);
        }
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("");
    tmp_album_title->setText(list_track->at(0).album_title);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = list_track->at(0).album_title;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(list_track->at(0).album_title, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(list_track->at(0).album_title);
            this->label_album->setToolTipDuration(2000);
        }
        this->label_album->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_album->setText(list_track->at(0).album_title);
        this->label_album->setGeometry(937, 24, 220, 20);
    }

    if(this->playlist_type == "APPLE_MUSIC"){
        int duration = data_track.duration / 1000;

        if(data_track.duration == 0){
            this->label_duration->setText("");
        }
        else if(duration >= 3600){
            this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }
    else{
        if(data_track.duration == 0){
            this->label_duration->setText("");
        }
        else if(data_track.duration >= 3600){
            this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("mm:ss"));
        }
    }

    if(list_track->at(0).streamable == false){
        this->label_title->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:normal;");
        this->label_artist->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:300;");
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#666666;font-size:12px;font-weight:300;border:1px solid #666666;border-radius:8px;");
        this->label_album->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");
        this->label_duration->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");

        this->setEnabled(false);
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_RoseMain(const roseHome::TrackItemData &data_track){

    this->widget_info_main->setFixedSize(1415, 70);

    QList<roseHome::TrackItemData> *list_track = new QList<roseHome::TrackItemData>();
    list_track->append(data_track);

    this->btn_play_list_add->hide();

    this->playlist_type = list_track->at(0).type;

    QString default_path = "";
    QString type_path = "";
    if(this->playlist_type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(this->playlist_type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(this->playlist_type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(this->playlist_type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
    }
    else if(this->playlist_type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(this->playlist_type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
    }
    else if(this->playlist_type == "APPLE_MUSIC"){
        default_path = ":/images/apple/apple_def.png";
        type_path = ":/images/rosehome/q_apple_play_ico.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(default_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(list_track->at(0).thumbnail);

    QImage image;
    image.load(type_path);

    QPixmap pixmapIMG = QPixmap(QSize(30, 20));
    pixmapIMG.fill(Qt::transparent);

    QPixmap tmp_pixmap;
    tmp_pixmap = tmp_pixmap.fromImage(image);
    tmp_pixmap = tmp_pixmap.scaled(image.width(), image.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter (&pixmapIMG);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path = QPainterPath();
    path.addRoundedRect(0, 0, image.width(), image.height(), 0, 0);

    painter.setClipPath(path);
    if(this->playlist_type == "MUSIC"){
        painter.drawPixmap(-8, -11, tmp_pixmap);
    }
    else if(this->playlist_type == "VIDEO"){
        painter.drawPixmap(-9, -12, tmp_pixmap);
    }
    else if(this->playlist_type == "YOUTUBE"){
        painter.drawPixmap(-7, -11, tmp_pixmap);
    }
    else if(this->playlist_type == "TIDAL"){
        painter.drawPixmap(-9, -11, tmp_pixmap);
    }
    else if(this->playlist_type == "BUGS"){
        painter.drawPixmap(-7, -13, tmp_pixmap);
    }
    else if(this->playlist_type == "QOBUZ"){
        painter.drawPixmap(-8, -12, tmp_pixmap);
    }
    else if(this->playlist_type == "APPLE_MUSIC"){
        painter.drawPixmap(0, 0, tmp_pixmap);
    }
    else{
        painter.drawPixmap(0, 0, tmp_pixmap);
    }
    painter.end();

    this->label_track_type->setStyleSheet("background-color:transparent;");
    this->label_track_type->setPixmap(pixmapIMG);
    this->label_track_type->setGeometry(80, 36, 30, 20);
    this->label_track_type->hide();

    QString title_work = "";
    QString title_version = "";
    QString title_total = "";
    if(!list_track->at(0).work.isEmpty()){
        title_work = list_track->at(0).work + " - ";
    }

    if(!list_track->at(0).version.isEmpty()){
        title_version = " (" + list_track->at(0).version +")";
    }

    title_total = title_work + list_track->at(0).title + title_version;

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title_total);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QString performer;
    if(this->playlist_type == "QOBUZ"){
        performer = list_track->at(0).list_artist_name.join(",");
        if(performer.isEmpty()){
            performer = list_track->at(0).performer_name;
        }
    }
    else{
        performer = list_track->at(0).list_artist_name.join(",");
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(performer);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    this->label_adult_certification->hide();
    this->label_track_hires->hide();

    int width = 0;
    int left = 0;
    if(list_track->at(0).adult_certification == true){
        QString adult_certification_path = "";

        if(global.lang == 0){
            adult_certification_path = ":/images/tidal/tidal-e-ico.png";
        }
        else{
            adult_certification_path = ":/images/bugs/ico-19.png";
        }
        QImage img;
        QPixmap *img_adult_certification = new QPixmap();
        if(img.load(adult_certification_path)){
            *img_adult_certification = QPixmap::fromImage(img);
            *img_adult_certification = img_adult_certification->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        this->label_adult_certification->setPixmap(*img_adult_certification);
        this->label_adult_certification->setFixedSize(30, 30);

        this->label_adult_certification->setGeometry(85, 18, 0, 0);
        this->label_adult_certification->show();

        if(list_track->at(0).hires == true){
            QString hires_path = "";

            if(this->playlist_type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 650){
                    title_width_resize = 650 - (15 + 36);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(36, 13);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 18, 0, 0);
                this->label_track_hires->show();
            }
            else if(this->playlist_type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 650){
                        title_width_resize = 650 - (15 + width + 15 + 20);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    this->label_track_hires->setPixmap(*img_hires);
                    this->label_track_hires->setFixedSize(20, 20);

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_hires->setGeometry(left, 14, 0, 0);
                    this->label_track_hires->show();

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15 + 20 + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 650){
                        title_width_resize = 650 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 16, width, 16);
                    this->label_track_resolution->show();
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 650){
                        title_width_resize = 650 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 650){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 650, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, 650, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(this->playlist_type == "QOBUZ"){

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 650){
                        title_width_resize = 650 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 650){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, title_width, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width, 25);
                    }
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 650){
                        title_width_resize = 650 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 650){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 650, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(135, 10, 650, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(135, 10, title_width, 25);
                    }
                }
            }
            else{
                if(title_width > 650){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 650, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(135, 10, 650, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(135, 10, title_width, 25);
                }
            }
        }

        this->label_track_type->setGeometry(135, 36, 30, 20);
        this->label_track_type->show();

        if(artist_width > 610){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 610, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(performer);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(165, 34, 610, 25);
        }
        else{
            this->label_artist->setText(performer);
            this->label_artist->setGeometry(165, 34, artist_width, 25);
        }
    }
    else{
        if(list_track->at(0).hires == true){
            QString hires_path = "";

            if(this->playlist_type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 700){
                    title_width_resize = 700 - (15 + 36);
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                this->label_track_hires->setPixmap(*img_hires);
                this->label_track_hires->setFixedSize(36, 13);

                left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                this->label_track_hires->setGeometry(left, 18, 0, 0);
                this->label_track_hires->show();
            }
            else if(this->playlist_type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 700){
                        title_width_resize = 700 - (15 + width + 15 + 20);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    this->label_track_hires->setPixmap(*img_hires);
                    this->label_track_hires->setFixedSize(20, 20);

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_hires->setGeometry(left, 14, 0, 0);
                    this->label_track_hires->show();

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15 + 20 + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 700){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 16, width, 16);
                    this->label_track_resolution->show();
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 700){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 700){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 690, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, 690, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(this->playlist_type == "QOBUZ"){

                if(list_track->at(0).maximum_sampling_rate > 0){
                    this->label_track_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 700){
                        title_width_resize = 690 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 700){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width, 25);
                    }
                }
            }
            else if(this->playlist_type == "BUGS"){

                if(!list_track->at(0).bitrates.isEmpty()){
                    this->label_track_resolution->setText(QString("%1").arg(list_track->at(0).bitrates));
                    this->label_track_resolution->setAlignment(Qt::AlignCenter);

                    width = this->label_track_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 700){
                        title_width_resize = 700 - (15 + width);
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, title_width_resize, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, title_width_resize, 25);
                    }

                    left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;

                    this->label_track_resolution->setGeometry(left, 15, width, 16);
                    this->label_track_resolution->show();
                }
                else{
                    if(title_width > 700){
                        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 700, this->label_title->font()));
                        if(this->label_title->text().contains("…")){
                            this->label_title->setToolTip(title_total);
                            this->label_title->setToolTipDuration(2000);
                        }
                        this->label_title->setGeometry(85, 10, title_width, 25);
                    }
                    else{
                        this->label_title->setText(title_total);
                        this->label_title->setGeometry(85, 10, 690, 25);
                    }
                }
            }
            else{
                if(title_width > 700){
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_total, 700, this->label_title->font()));
                    if(this->label_title->text().contains("…")){
                        this->label_title->setToolTip(title_total);
                        this->label_title->setToolTipDuration(2000);
                    }
                    this->label_title->setGeometry(85, 10, 690, 25);
                }
                else{
                    this->label_title->setText(title_total);
                    this->label_title->setGeometry(85, 10, title_width, 25);
                }
            }
        }

        this->label_track_type->setGeometry(85, 36, 30, 20);
        this->label_track_type->show();

        if(artist_width > 660){
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(performer, 660, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(performer);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(115, 34, 660, 25);
        }
        else{
            this->label_artist->setText(performer);
            this->label_artist->setGeometry(115, 34, artist_width, 25);
        }
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("");
    tmp_album_title->setText(list_track->at(0).album_title);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = list_track->at(0).album_title;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(list_track->at(0).album_title, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(list_track->at(0).album_title);
            this->label_album->setToolTipDuration(2000);
        }
        this->label_album->setGeometry(819, 14, 220, 40);
    }
    else{
        this->label_album->setText(list_track->at(0).album_title);
        this->label_album->setGeometry(819, 24, 220, 20);
    }

    if(this->playlist_type == "APPLE_MUSIC"){
        int duration = data_track.duration / 1000;

        if(data_track.duration == 0){
            this->label_duration->setText("");
        }
        else if(duration >= 3600){
            this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }
    else{
        if(data_track.duration == 0){
            this->label_duration->setText("");
        }
        else if(data_track.duration >= 3600){
            this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("mm:ss"));
        }
    }
    this->label_duration->setGeometry(1150, 24, 65, 20);

    this->btn_list_fav->setGeometry(1290, 6, 50, 58);

    this->btn_menu->setGeometry(1340, 6, 50, 58);

    if(list_track->at(0).streamable == false){
        this->label_title->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:normal;");
        this->label_artist->setStyleSheet("background-color:transparent;color:#666666;font-size:16px;font-weight:300;");
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#666666;font-size:12px;font-weight:300;border:1px solid #666666;border-radius:8px;");
        this->label_album->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");
        this->label_duration->setStyleSheet("background-color:transparent;color:#666666;font-size:14px;font-weight:300;");

        this->setEnabled(false);
    }
}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_RoseListEdit(const QJsonObject &trackInfo){

    QString type = ProcJsonEasy::getString(trackInfo, "type");
    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    QString artist = ProcJsonEasy::getString(trackInfo, "artist");
    int duration = ProcJsonEasy::getInt(trackInfo, "duration");
    bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
    double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

    this->btn_play_list_add->hide();
    this->btn_list_fav->hide();
    this->btn_menu->hide();

    this->playlist_type = type;

    QString default_path = "";
    QString type_path = "";
    QString hires_path = "";
    if(this->playlist_type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(this->playlist_type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(this->playlist_type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(this->playlist_type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
        hires_path = ":/images/tidal/mqa_ico.png";
    }
    else if(this->playlist_type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(this->playlist_type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
        hires_path = ":/images/qobuz/hires_ico.png";
    }
    else if(this->playlist_type == "APPLE_MUSIC"){
        default_path = ":/images/apple/apple_def.png";
        type_path = ":/images/rosehome/home_apple.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(default_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(thumbnail);

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
    this->label_track_type->setPixmap(*pixmap_icon);
    this->label_track_type->setGeometry(68, 9, 49, 43);
    this->label_track_type->setStyleSheet("background-color:transparent;");
    this->label_track_type->show();

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->label_title->font()));
        if(this->label_title->text().contains("…")){
            this->label_title->setToolTip(title);
            this->label_title->setToolTipDuration(2000);
        }
        this->label_title->setGeometry(120, 10, 720, 25);
    }
    else{
        this->label_title->setText(title);
        this->label_title->setGeometry(120, 10, title_width, 25);
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    this->label_artist->setText(artist);

    if(hires == true){

        int hires_width = 0;
        int hires_height = 0;
        if(this->playlist_type == "TIDAL"){
            hires_width = 36;
            hires_height = 13;
        }
        else if(this->playlist_type == "QOBUZ"){
            hires_width = 30;
            hires_height = 30;
        }

        this->label_track_hires = new QLabel(this->widget_info_main);

        QPixmap pixmap(hires_path);
        pixmap = pixmap.scaled(hires_width, hires_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPixmap pixmap_painter = QPixmap(QSize(hires_width, hires_height));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QBrush brush = QBrush(pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, hires_width, hires_height, 2, Qt::RelativeSize);

        this->label_track_hires->setPixmap(pixmap_painter);

        int left = this->label_title->geometry().left() + this->label_title->geometry().width() + 20;
        int top = (70 / 2) - (this->label_track_hires->sizeHint().height() / 2);

        this->label_track_hires->setGeometry(left, top, hires_width, hires_height);
    }

    if(sampling > 0){
        this->label_track_resolution = new QLabel(this->widget_info_main);
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        this->label_track_resolution->setText(QString("%1 kHz").arg(sampling));
        this->label_track_resolution->setAlignment(Qt::AlignCenter);

        int width = this->label_track_resolution->sizeHint().width() + 10;;
        int left = 0;
        if(artist_width > 720){
            left = this->label_artist->geometry().left() + this->label_artist->geometry().width() + 15;
        }
        else{
            left = this->label_artist->geometry().left() + artist_width + 15;
        }

        this->label_track_resolution->setGeometry(left, 40, width, 16);
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(title);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = title;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() >= 220){
                    tmp_album_title->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album_title->setText("");
        tmp_album_title->setText(tmp_album_set);

        album_title_width = tmp_album_title->sizeHint().width() + 220;

        this->label_album->setText(GSCommon::getTextCutFromLabelWidth(title, album_title_width, this->label_album->font()));
        if(this->label_album->text().contains("…")){
            this->label_album->setToolTip(title);
            this->label_album->setToolTipDuration(2000);
        }
        this->label_album->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_album->setText(title);
        this->label_album->setGeometry(937, 24, 220, 20);
    }

    if(this->playlist_type == "APPLE_MUSIC"){
        duration /= 1000;
    }

    if(duration == 0){
        this->label_duration->setText("");
    }
    else if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }

    this->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { } ");
    //this->setCursor(Qt::ArrowCursor);//c0208

}


void PlaylistTrackDetailInfo_RHV::setDataTrackInfo_Video(const QJsonObject &trackInfo){

    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    int duration = ProcJsonEasy::getInt(trackInfo, "duration");
    double fileSize = ProcJsonEasy::getDouble(trackInfo, "fileSize");

    this->playlist_type = "VIDEO";

    this->btn_play_list_add->hide();
    this->btn_list_fav->hide();

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->setImage(thumbnail);

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->label_title->font()));
        if(this->label_title->text().contains("…")){
            this->label_title->setToolTip(title);
            this->label_title->setToolTipDuration(2000);
        }
        this->label_title->setGeometry(80, 10, 720, 25);
    }
    else{
        this->label_title->setText(title);
        this->label_title->setGeometry(80, 10, title_width, 25);
    }

    QString strSize = "";
    if(fileSize > 1000){
        strSize = QString::number(fileSize / 1000, 'f', 1) + " GB";
    }
    else{
        strSize = QString::number(fileSize, 'f', 1) + " MB";
    }

    this->label_artist->setText(strSize);
    this->label_artist->setGeometry(80, 34, 720, 25);

    if(duration == 0){
        this->label_duration->setText("");
    }
    else if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }
}


void PlaylistTrackDetailInfo_RHV::setImage(QString imagePath){

    // 이미지 경로 설정
    if(imagePath.isEmpty() == false){
         this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_image(*this->pixmap_albumImg_default);
    }
}


//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : [Slot]
//
//-----------------------------------------------------------------------------------------------------------------------

void PlaylistTrackDetailInfo_RHV::slot_clickBtn_signal(){
    QString type = sender()->property("type").toString();

    if(type == "playtrack"){
        emit this->clicked(this->property("index").toInt(), PlaylistTrackDetailInfo_RHV::ClickMode::AllBox);
    }
    else if(type == "playlist"){
        emit this->clicked(this->property("index").toInt(), PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn);
    }
    else if(type == "fav"){
        emit this->clicked(this->property("index").toInt(), PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn);
    }
    else if(type == "more"){
        emit this->clicked(this->property("index").toInt(), PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn);
    }
    else if(type == "musicVideo"){
        emit this->clicked(this->property("index").toInt(), PlaylistTrackDetailInfo_RHV::ClickMode::MusicVideoBtn);
    }
}



void PlaylistTrackDetailInfo_RHV::setFavoritesIds(bool p_myFavoriteIds, int p_star){
    this->myFavoriteIds = p_myFavoriteIds;
    this->myStarIds = p_star;

    if(this->myFavoriteIds == true && this->myStarIds > 0){
        switch (this->myStarIds) {
            case 1:
                this->btn_list_fav->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                              ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
            case 2:
                this->btn_list_fav->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                              ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
            case 3:
                this->btn_list_fav->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                              ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
        }

    }
    else{
        this->btn_list_fav->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                      ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
    }
}


bool PlaylistTrackDetailInfo_RHV::getFavoritesIds(){
    return this->myFavoriteIds;
}


int PlaylistTrackDetailInfo_RHV::getFavoritesStars(){
    return this->myStarIds;
}

/**
 * @brief PlayListDetail::loadImage [SLOT] 앨범 이미지 세팅
 */
void PlaylistTrackDetailInfo_RHV::slot_fileDownload_loadImage()
{
    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(60, 60));
        pixmapIMG.fill(Qt::black);
        /*if(this->playlist_type == "VIDEO"){
            pixmapIMG.fill(Qt::transparent);
        }
        else{
            pixmapIMG.fill(Qt::black);
        }*/

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        /*if(this->playlist_type == "VIDEO"){
            tmp_pixmap = tmp_pixmap.scaled(42, 42, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else{
            if(60 > tmp_pixmap.width() && 60 <= tmp_pixmap.height()){
                tmp_pixmap = tmp_pixmap.scaled(tmp_pixmap.width(), 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            else if(60 <= tmp_pixmap.width() && 60 > tmp_pixmap.height()){
                tmp_pixmap = tmp_pixmap.scaled(60, tmp_pixmap.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            else{
                tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }*/

        if(60 > tmp_pixmap.width() && 60 <= tmp_pixmap.height()){
            tmp_pixmap = tmp_pixmap.scaled(tmp_pixmap.width(), 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else if(60 <= tmp_pixmap.width() && 60 > tmp_pixmap.height()){
            tmp_pixmap = tmp_pixmap.scaled(60, tmp_pixmap.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else{
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, 60, 60, 4, 4, Qt::RelativeSize);

        int leftValue = (60 - tmp_pixmap.width()) / 2;
        int topValue = (60 - tmp_pixmap.height()) / 2;

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->label_track_thumbnail->setPixmap(pixmapIMG);        
    }
    else{
        this->paint_image(*this->pixmap_albumImg_default);
    }

    /*if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->paint_image(tmp_pixmap);
        this->resize(this->size());
    }
    else{
        this->paint_image(*this->pixmap_albumImg_default);
    }*/
}


QLabel *PlaylistTrackDetailInfo_RHV::getThumnailLabel(){

    return label_track_thumbnail;
}


/**
 * @brief pixmap 을 label_imageBig 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
 * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
 * @param pixmap
 */
void PlaylistTrackDetailInfo_RHV::paint_image(QPixmap &pixmap){

    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(60, 60));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, 60, 60, 2, Qt::RelativeSize);

    this->label_track_thumbnail->setPixmap(pixmap_painter);
}
