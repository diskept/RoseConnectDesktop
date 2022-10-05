#include "AbstractAlbumTrackDetailInfo_RHV.h"

#include "common/global.h"
#include <common/gscommon.h>
#include "widget/framewidget.h"

#include <QJsonObject>
#include <QTime>
#include <QJsonArray>
#include "widget/optionpopup.h"
#include <common/networkhttp.h>


const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";
const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";
const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";

/**
 * @brief 생성자
 * @param parent
 */
AlbumTrackDetailInfo_RHV::AlbumTrackDetailInfo_RHV(QWidget *parent) : QPushButton(parent){
    this->linker = Linker::getInstance();

    this->widget_info_main = new QWidget();
    this->widget_info_main->setFixedSize(1500, 70);
    this->widget_info_main->setObjectName("trackInfo");

    this->label_list_play = GSCommon::getUILabelImg(":/images/icon_list_play.png", this->widget_info_main);
    this->label_list_play->setGeometry(20, 20, 30, 30);
    this->label_list_play->setStyleSheet("background-color:transparent;");

    this->label_track_number = new QLabel(this->widget_info_main);
    this->label_track_number->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:300;");
    this->label_track_number->setGeometry(80, 21, 30, 24);

    this->label_title = new QLabel(this->widget_info_main);
    this->label_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_title->setGeometry(150, 10, 610, 49);

    this->label_resolution = new QLabel(this->widget_info_main);
    this->label_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    this->label_resolution->setAlignment(Qt::AlignCenter);
    this->label_resolution->hide();

    this->label_artist = new QLabel(this->widget_info_main);
    this->label_artist->setWordWrap(true);
    this->label_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_artist->setGeometry(937, 24, 220, 20);

    this->label_duration = new QLabel(this->widget_info_main);
    this->label_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_duration->setGeometry(1181, 24, 100, 20);

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

    connect(this->btn_play_list_add, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_list_fav, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_menu, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
}


AlbumTrackDetailInfo_RHV::~AlbumTrackDetailInfo_RHV(){

    this->deleteLater();
}


void AlbumTrackDetailInfo_RHV::setDataTrackInfo_Music(const QJsonObject &jsonObj){

    QString tmp = "";
    int idx = this->property("index").toInt() + 1;
    if(idx < 100){
        tmp = QString("%1").arg(idx, 2, 10, QLatin1Char('0')).toUpper();
    }
    else{
        tmp = QString("%1").number(idx);
    }

    this->label_track_number->setText(tmp);

    QString tmpMime = ProcJsonEasy::getString(jsonObj, "album_mime");
    int tmpsampleRate = ProcJsonEasy::getInt(jsonObj, "samplerate");
    int tmpbitDepth = ProcJsonEasy::getInt(jsonObj, "bitdepth");

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

    QString title = ProcJsonEasy::getString(jsonObj, "title");
    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 610, this->label_title->font()));

    QString tmpArtist = ProcJsonEasy::getString(jsonObj, "artist");

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_artist->setText(tmpArtist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 220){

        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_artist_line1 = "";

        tmp_split = tmpArtist;
        splitToken = tmp_split.split(" ");

        tmp_artist->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_artist_line1 = splitToken.at(i);
                }
                else{
                    tmp_artist_line1 += " " + splitToken.at(i);
                }
                tmp_artist->setText(tmp_artist_line1);

                if(tmp_artist->sizeHint().width() > 220){
                    tmp_artist->setText("");
                    tmp_artist_line1.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_artist->setText("");
        tmp_artist->setText(tmp_artist_line1);

        artist_width = tmp_artist->sizeHint().width() + 220;

        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(tmpArtist, artist_width, this->label_artist->font()));
        this->label_artist->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_artist->setText(tmpArtist);
        this->label_artist->setGeometry(937, 24, 220, 20);
    }

    if(!mimeType.isEmpty()){
        this->label_resolution->setText(QString("%1").arg(mimeType));
        this->label_resolution->setAlignment(Qt::AlignCenter);

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        tmp_title->setText(this->label_title->text());

        int width = this->label_resolution->sizeHint().width() + 10;
        int left = tmp_title->sizeHint().width();

        if(left > 610){
            left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;
        }
        else{
            left += this->label_title->geometry().left() + 15;
        }

        this->label_resolution->setGeometry(left, 27, width, 16);
        this->label_resolution->show();
    }

    int duration = ProcJsonEasy::getInt(jsonObj, "duration") / 1000;
    QString str_duration = "";

    if(duration > 3600){
        str_duration = QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss");
    }
    else{
        str_duration = QDateTime::fromTime_t(duration).toUTC().toString("mm:ss");
    }

    this->label_duration->setText(str_duration);
}


void AlbumTrackDetailInfo_RHV::setDataTrackInfo_Tidal(const tidal::TrackItemData &data_track){

    QList<tidal::TrackItemData> *list_track = new QList<tidal::TrackItemData>();
    list_track->append(data_track);

    QString tmp = "";
    if(data_track.visual_type == true){
        int number = this->property("index").toInt() + 1;
        tmp = QString("%1").arg(number, 2, 10, QLatin1Char('0')).toUpper();
    }
    else{
        tmp = QString("%1").arg(list_track->at(0).trackNumber, 2, 10, QLatin1Char('0')).toUpper();
    }
    this->label_track_number->setText(tmp);

    /*this->label_title->setText(list_track->at(0).title);
    this->label_artist->setText(list_track->at(0).list_artist_name.join(","));
    */

    QString title = list_track->at(0).title;
    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 610, this->label_title->font()));

    QString tmpArtist = list_track->at(0).list_artist_name.join(",");

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_artist->setText(tmpArtist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 220){

        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_artist_line1 = "";

        tmp_split = tmpArtist;
        splitToken = tmp_split.split(" ");

        tmp_artist->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_artist_line1 = splitToken.at(i);
                }
                else{
                    tmp_artist_line1 += " " + splitToken.at(i);
                }
                tmp_artist->setText(tmp_artist_line1);

                if(tmp_artist->sizeHint().width() > 220){
                    tmp_artist->setText("");
                    tmp_artist_line1.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_artist->setText("");
        tmp_artist->setText(tmp_artist_line1);

        artist_width = tmp_artist->sizeHint().width() + 220;

        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(tmpArtist, artist_width, this->label_artist->font()));
        this->label_artist->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_artist->setText(tmpArtist);
        this->label_artist->setGeometry(937, 24, 220, 20);
    }

    if(data_track.duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("mm:ss"));
    }

    if(!data_track.streamReady){//c220617
         this->label_title->setStyleSheet(this->label_title->styleSheet().replace("#FFFFFF", "#707070"));
    }
}


void AlbumTrackDetailInfo_RHV::setDataTrackInfo_Qobuz(const qobuz::TrackItemData &data_track){

    QList<qobuz::TrackItemData> *list_track = new QList<qobuz::TrackItemData>();
    list_track->append(data_track);

    QString tmp = "";
    tmp = QString("%1").arg(list_track->at(0).track_number, 2, 10, QLatin1Char('0')).toUpper();
    this->label_track_number->setText(tmp);

    /*this->label_title->setText(list_track->at(0).title);
    QString tmpArtist = list_track->at(0).list_artist_name.join(",");
    if(tmpArtist.isEmpty()){
        tmpArtist = list_track->at(0).performer_name;
    }
    this->label_artist->setText(tmpArtist);*/

    QString title_version = "";

    if(!list_track->at(0).version.isEmpty()){
        title_version = " (" + list_track->at(0).version +")";
    }

    QString title = list_track->at(0).title + title_version;

    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 610, this->label_title->font()));

    QString tmpArtist = list_track->at(0).list_artist_name.join(",");
    if(tmpArtist.isEmpty()){
        tmpArtist = list_track->at(0).performer_name;
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_artist->setText(tmpArtist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 220){

        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_artist_line1 = "";

        tmp_split = tmpArtist;
        splitToken = tmp_split.split(" ");

        tmp_artist->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_artist_line1 = splitToken.at(i);
                }
                else{
                    tmp_artist_line1 += " " + splitToken.at(i);
                }
                tmp_artist->setText(tmp_artist_line1);

                if(tmp_artist->sizeHint().width() > 220){
                    tmp_artist->setText("");
                    tmp_artist_line1.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_artist->setText("");
        tmp_artist->setText(tmp_artist_line1);

        artist_width = tmp_artist->sizeHint().width() + 220;

        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(tmpArtist, artist_width, this->label_artist->font()));
        this->label_artist->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_artist->setText(tmpArtist);
        this->label_artist->setGeometry(937, 24, 220, 20);
    }

    if(list_track->at(0).maximum_sampling_rate > 0){
        this->label_resolution->setText(QString("%1 kHz").arg(list_track->at(0).maximum_sampling_rate));

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        tmp_title->setText(title);

        int width = this->label_resolution->sizeHint().width() + 20;
        int left = tmp_title->sizeHint().width();

        if(left > 610){
            left = this->label_title->geometry().left() + this->label_title->geometry().width() + 15;
        }
        else{
            left += this->label_title->geometry().left() + 15;
        }

        this->label_resolution->setGeometry(left, 27, width, 16);
        this->label_resolution->show();
    }

    if(data_track.duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("mm:ss"));
    }

    if(!data_track.streamable){//c220617
         this->label_title->setStyleSheet(this->label_title->styleSheet().replace("#FFFFFF", "#707070"));
    }
}


void AlbumTrackDetailInfo_RHV::setDataTrackInfo_Apple(const QJsonObject &data_track){

    this->btn_play_list_add->hide();

    QJsonObject attributes = ProcJsonEasy::getJsonObject(data_track, "attributes");

    int track_number = ProcJsonEasy::getInt(attributes, "trackNumber");

    QString tmp = "";
    tmp = QString("%1").arg(track_number, 2, 10, QLatin1Char('0')).toUpper();
    this->label_track_number->setText(tmp);

    QString title = ProcJsonEasy::getString(attributes, "name");
    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 610, this->label_title->font()));

    QString tmpArtist = ProcJsonEasy::getString(attributes, "artistName");
    if(tmpArtist.isEmpty()){
        tmpArtist = ProcJsonEasy::getString(attributes, "composerName");
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_artist->setText(tmpArtist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 220){

        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_artist_line1 = "";

        tmp_split = tmpArtist;
        splitToken = tmp_split.split(" ");

        tmp_artist->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_artist_line1 = splitToken.at(i);
                }
                else{
                    tmp_artist_line1 += " " + splitToken.at(i);
                }
                tmp_artist->setText(tmp_artist_line1);

                if(tmp_artist->sizeHint().width() > 220){
                    tmp_artist->setText("");
                    tmp_artist_line1.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_artist->setText("");
        tmp_artist->setText(tmp_artist_line1);

        artist_width = tmp_artist->sizeHint().width() + 220;

        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(tmpArtist, artist_width, this->label_artist->font()));
        this->label_artist->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_artist->setText(tmpArtist);
        this->label_artist->setGeometry(937, 24, 220, 20);
    }

    int duration = ProcJsonEasy::getInt(attributes, "durationInMillis") / 1000;
    if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }
}


void AlbumTrackDetailInfo_RHV::setDataTrackInfo_CDplay(const QJsonObject &jsonObj){//c220724

    this->btn_play_list_add->hide();//c220724
    this->btn_list_fav->hide();//c220724
    this->btn_menu->hide();

    QString tmp = "";
    int idx = this->property("index").toInt() + 1;
    if(idx < 100){
        tmp = QString("%1").arg(idx, 2, 10, QLatin1Char('0')).toUpper();
    }
    else{
        tmp = QString("%1").number(idx);
    }

    this->label_track_number->setText(tmp);

    QString title = ProcJsonEasy::getString(jsonObj, "mTitle");
    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 610, this->label_title->font()));

    QString tmpArtist = ProcJsonEasy::getString(jsonObj, "mArtist");

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_artist->setText(tmpArtist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    if(artist_width > 220){

        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_artist_line1 = "";

        tmp_split = tmpArtist;
        splitToken = tmp_split.split(" ");

        tmp_artist->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_artist_line1 = splitToken.at(i);
                }
                else{
                    tmp_artist_line1 += " " + splitToken.at(i);
                }
                tmp_artist->setText(tmp_artist_line1);

                if(tmp_artist->sizeHint().width() > 220){
                    tmp_artist->setText("");
                    tmp_artist_line1.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_artist->setText("");
        tmp_artist->setText(tmp_artist_line1);

        artist_width = tmp_artist->sizeHint().width() + 220;

        this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(tmpArtist, artist_width, this->label_artist->font()));
        this->label_artist->setGeometry(937, 14, 220, 40);
    }
    else{
        this->label_artist->setText(tmpArtist);
        this->label_artist->setGeometry(937, 24, 220, 20);
    }

    QString str_duration = ProcJsonEasy::getString(jsonObj, "mRunningTime");

    QStringList duration = str_duration.split(".");

    if(duration.count() > 1){
        this->label_duration->setText(duration[0]);
    }
    else{
        this->label_duration->setText(str_duration);
    }
}


void AlbumTrackDetailInfo_RHV::slot_clickBtn_signal(){
    QString type = sender()->property("type").toString();

    if(type == "playtrack"){
        emit this->clicked(this->property("index").toInt(), ClickMode::AllBox);
    }
    else if(type == "playlist"){
        emit this->clicked(this->property("index").toInt(), ClickMode::AddCollectionBtn);
    }
    else if(type == "fav"){
        emit this->clicked(this->property("index").toInt(), ClickMode::FavBtn);
    }
    else if(type == "more"){
        emit this->clicked(this->property("index").toInt(), ClickMode::MoreBtn);
    }
}


void AlbumTrackDetailInfo_RHV::setFavoritesIds(bool p_myFavoriteIds, int p_star){
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


bool AlbumTrackDetailInfo_RHV::getFavoritesIds(){
    return this->myFavoriteIds;
}


int AlbumTrackDetailInfo_RHV::getFavoritesStars(){
    return this->myStarIds;
}





AlbumTrackDetailInfoCredit_RHV::AlbumTrackDetailInfoCredit_RHV(QWidget *parent) : QPushButton(parent){
    this->linker = Linker::getInstance();

    this->widget_info_main = new QWidget();
    this->widget_info_main->setFixedSize(1500, 116);
    this->widget_info_main->setObjectName("trackInfo");

    this->label_list_play = GSCommon::getUILabelImg(":/images/icon_list_play.png", this->widget_info_main);
    this->label_list_play->setGeometry(20, 43, 30, 30);
    this->label_list_play->setStyleSheet("background-color:transparent;");

    this->label_track_number = new QLabel(this->widget_info_main);
    this->label_track_number->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:300;");
    this->label_track_number->setGeometry(80, 44, 17, 24);

    this->label_title = new QLabel(this->widget_info_main);
    this->label_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_title->setGeometry(150, 15, 710, 25);

    this->label_composer = new QLabel(this->widget_info_main);
    this->label_composer->setTextFormat(Qt::RichText);
    this->label_composer->setWordWrap(true);
    this->label_composer->setGeometry(150, 43, 710, 53);
    this->label_composer->setStyleSheet("background-color:transparent;");

    this->label_artist = new QLabel(this->widget_info_main);
    this->label_artist->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_artist->setGeometry(937, 48, 218, 20);

    this->label_duration = new QLabel(this->widget_info_main);
    this->label_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_duration->setGeometry(1181, 48, 43, 20);

    this->btn_play_list_add = GSCommon::getUIBtnImg("btn_play_list_add", ":/images/play_list_add_icon.png", this->widget_info_main);
    this->btn_play_list_add->setGeometry(1350, 33, 50, 50);
    this->btn_play_list_add->setCursor(Qt::PointingHandCursor);
    this->btn_play_list_add->setProperty("type", "playlist");

    this->btn_list_fav = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, this->widget_info_main);
    this->btn_list_fav->setGeometry(1400, 29, 50, 58);
    this->btn_list_fav->setCursor(Qt::PointingHandCursor);
    this->btn_list_fav->setProperty("type", "fav");

    this->btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", this->widget_info_main);
    this->btn_menu->setGeometry(1450, 29, 50, 58);
    this->btn_menu->setCursor(Qt::PointingHandCursor);
    this->btn_menu->setProperty("type", "more");

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->addWidget(this->widget_info_main);

    this->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { background-color:#333333; } ");
    this->setFixedHeight(116);
    this->setLayout(vl_total);
    this->setCursor(Qt::PointingHandCursor);
    this->setProperty("type", "playtrack");

    connect(this->btn_play_list_add, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_list_fav, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this->btn_menu, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
    connect(this, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));
}


AlbumTrackDetailInfoCredit_RHV::~AlbumTrackDetailInfoCredit_RHV(){

}


void AlbumTrackDetailInfoCredit_RHV::setDataTrackInfo_Qobuz(const qobuz::TrackItemData &data_track){
    QList<qobuz::TrackItemData> *list_track = new QList<qobuz::TrackItemData>();
    list_track->append(data_track);

    QString tmp = "";
    tmp = QString("%1").arg(list_track->at(0).track_number, 2, 10, QLatin1Char('0')).toUpper();
    this->label_track_number->setText(tmp);
    this->label_title->setText(list_track->at(0).title);
    tmp = list_track->at(0).performers;
    this->label_composer->setText(QString("<html><head/><body><span style='font-size:13px; font-weight:300; color:#FFFFFF;'><p style='line-height:1.2'>%1</p></span></body></html>").arg(tmp.replace(" - ", "<br>")));
    if(!data_track.streamable){//c220617
         this->label_title->setStyleSheet(this->label_title->styleSheet().replace("#FFFFFF", "#707070"));
    }
    this->label_artist->setText(list_track->at(0).list_artist_name.join(","));
    if(data_track.duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(data_track.duration).toUTC().toString("mm:ss"));
    }
}


void AlbumTrackDetailInfoCredit_RHV::slot_clickBtn_signal(){
    QString type = sender()->property("type").toString();

    if(type == "playtrack"){
        emit this->clicked(this->property("index").toInt(), AlbumTrackDetailInfo_RHV::ClickMode::AllBox);
    }
    else if(type == "playlist"){
        emit this->clicked(this->property("index").toInt(), AlbumTrackDetailInfo_RHV::ClickMode::AddCollectionBtn);
    }
    else if(type == "fav"){
        emit this->clicked(this->property("index").toInt(), AlbumTrackDetailInfo_RHV::ClickMode::FavBtn);
    }
    else if(type == "more"){
        emit this->clicked(this->property("index").toInt(), AlbumTrackDetailInfo_RHV::ClickMode::MoreBtn);
    }
}

void AlbumTrackDetailInfoCredit_RHV::setFavoritesIds(bool p_myFavoriteIds, int p_star){
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


bool AlbumTrackDetailInfoCredit_RHV::getFavoritesIds(){
    return this->myFavoriteIds;
}


int AlbumTrackDetailInfoCredit_RHV::getFavoritesStars(){
    return this->myStarIds;
}
