#include "widget/AbstractAddPlaylist_RHV.h"

#include "tidal/Dialog_ChoosePlaylist_forTidal.h"
#include "tidal/ProcCommon.h"

#include "bugs/ConvertData_forBugs.h"

#include "qobuz/Dialog_ChoosePlaylist_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"

#include "widget/dialogconfirm.h"
#include "widget/toastmsg.h"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QSettings>


const QString IMAGE_PATH_DEFAULT = ":/images/rosehome/thum_edit.png";

const QString ALBTAB_STEP_ADDING = "added";
const QString ALBTAB_STEP_EXISTING = "existing";
const QString ALBTAB_STEP_EDITING = "editing";

const int GET_MAX_ITEM_SIZE___ONCE = 20;


AbstractAddPlaylist_RHV::AbstractAddPlaylist_RHV(ContentsType contents_type, ContentsUIType contents_ui_type, QWidget *parent)
    : QWidget(parent)
    , contents_type(contents_type)
    , contents_ui_type(contents_ui_type)
{

    this->linker = Linker::getInstance();

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractAddPlaylist_RHV::slot_fileDownload_loadImage);

    this->tagTypeList << tr("#Classical") << tr("#Pop") << tr("#K-Pop") << tr("#Hip-Hop") << tr("#Jazz") << tr("#Dance") << tr("#R&B")
                      << tr("#Latin") << tr("#Blues") << tr("#Soul") << tr("#Country") << tr("#Indie") << tr("#Rock");

    this->flag_addTracks_tidal = false;
    this->flag_addTracks_qobuz = false;
    this->flag_addTracks_rose = false;

    this->flag_all_enable = false;
    this->flag_freind_enable = false;
    this->flag_private_enable = false;

    this->flag_stream_enable = false;

    global.isMoveModePlaylist = true;

    this->shared_type = "";
    this->playlist_type = "";
}


AbstractAddPlaylist_RHV::~AbstractAddPlaylist_RHV(){

    this->fileDownLoader->disconnect();
    this->fileDownLoader->deleteLater();

    this->deleteLater();
}


void AbstractAddPlaylist_RHV::setData_fromJsonObject(const QJsonObject &jsonObj){

    if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){
        this->str_added = tr("To be added");
        this->str_exsting = tr("Existing");

        if(jsonObj.contains("tracks")){
            this->added_array = QJsonArray();
            this->added_array = ProcJsonEasy::getJsonArray(jsonObj, "tracks");

            this->create_count = this->added_array.count();

            this->str_added += QString(" (%1)").arg(this->added_array.size());
        }
        else{
            str_added += " (0)";
            this->create_count = 0;
        }

        if(jsonObj.contains("playlistInfo")){
            this->playlist_info = QJsonObject();
            this->playlist_info = ProcJsonEasy::getJsonObject(jsonObj, "playlistInfo");

            this->track_count = ProcJsonEasy::getInt(this->playlist_info, "trackCount");
            this->playlist_id = ProcJsonEasy::getInt(this->playlist_info, "id");
            this->shared_type = ProcJsonEasy::getString(this->playlist_info, "share");

            this->str_exsting += QString(" (%1)").arg(this->track_count);
        }
        else{
            this->str_exsting += " (0)";
            this->track_count = 0;
        }

        this->widget_menubar = new QWidget();
        this->widget_menubar->setFixedSize(1550, 50);

        QWidget *widget_line = new QWidget(this->widget_menubar);
        widget_line->setFixedSize(1550, 1);
        widget_line->setGeometry(0, 49, 0, 0);
        widget_line->setStyleSheet("background-color:#333333");

        QJsonArray *p_jsonArray_titlSub = new QJsonArray();
        QJsonObject sub1 { {"name", str_added}, {"code", ALBTAB_STEP_ADDING} };
        p_jsonArray_titlSub->push_back(sub1);

        if(this->track_count > 0){
            QJsonObject sub2 { {"name", str_exsting}, {"code", ALBTAB_STEP_EXISTING} };
            p_jsonArray_titlSub->push_back(sub2);
        }

        this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail, this->widget_menubar);
        this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

        connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));

        this->btn_delete = new QPushButton(this->widget_menubar);
        this->btn_delete->setObjectName("btn_del");
        this->btn_delete->setCursor(Qt::PointingHandCursor);
        this->btn_delete->setStyleSheet("background-color:transparent;");
        //this->btn_delete->setFixedSize(270, 50);            //45, 50
        //this->btn_delete->setGeometry(850, 0, 0, 0);
        this->btn_delete->hide();

        connect(this->btn_delete, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clickedDelete);

        this->lb_delete_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", this->btn_delete);
        this->lb_delete_icon->setFixedSize(50, 50);
        this->lb_delete_icon->setGeometry(0, 0, 0, 0);

        this->lb_delete = new QLabel(this->btn_delete);
        this->lb_delete->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        this->lb_delete->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->lb_delete->setText(tr("Delete Tracks"));
        this->lb_delete->setGeometry(60, 12, this->lb_delete->sizeHint().width(), 26);

        int btn_width = this->lb_delete->sizeHint().width() + 92;

        this->btn_delete->setGeometry(850, 0, btn_width, 50);

        this->contentStep = ALBTAB_STEP_ADDING;
        this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

        this->btn_withStream = new QPushButton(this->widget_menubar);
        this->btn_withStream->setStyleSheet("background-color:transparent;");
        this->btn_withStream->setCursor(Qt::PointingHandCursor);
        this->btn_withStream->setFixedSize(220, 50);//c220511
        this->btn_withStream->setGeometry(1030, 0, 0, 0);
        this->btn_withStream->hide();

        connect(this->btn_withStream, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnStream);

        this->lb_listCheck_icon = GSCommon::getUILabelImg(":/images/rosehome/list_check_off.png", this->btn_withStream);
        this->lb_listCheck_icon->setFixedSize(50, 50);
        this->lb_listCheck_icon->setGeometry(0, 0, 0, 0);

        this->lb_listCheck = new QLabel(this->btn_withStream);
        this->lb_listCheck->setFixedSize(170, 26);//c220511
        this->lb_listCheck->setGeometry(50, 12, 0, 0);
        this->lb_listCheck->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        this->lb_listCheck->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

        this->btn_selectList = new QPushButton(this->widget_menubar);
        this->btn_selectList->setStyleSheet("background-color:transparent;");
        this->btn_selectList->setCursor(Qt::PointingHandCursor);
        this->btn_selectList->setFixedSize(250, 50);
        this->btn_selectList->setGeometry(1250, 0, 0, 0);
        this->btn_selectList->hide();

        connect(this->btn_selectList, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnSelect);

        this->lb_selectList_icon = GSCommon::getUILabelImg(":/images/rosehome/playlist_ico.png", this->btn_selectList);
        this->lb_selectList_icon->setFixedSize(50, 50);
        this->lb_selectList_icon->setGeometry(0, 0, 0, 0);

        this->lb_selectList = new QLabel(this->btn_selectList);
        this->lb_selectList->setFixedSize(200, 26);
        this->lb_selectList->setGeometry(50, 12, 0, 0);
        this->lb_selectList->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        this->lb_selectList->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

        if(this->contents_ui_type == ContentsUIType::Creating){
            this->setUIControl_Creating();
        }
        else if(this->contents_ui_type == ContentsUIType::Adding){
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            this->request_more_trackData();

            this->setUIControl_Adding();
        }
    }
    else if(this->contents_ui_type == ContentsUIType::Editing){
        this->str_editing = tr("");

        this->contentStep = ALBTAB_STEP_EDITING;

        if(jsonObj.contains("playlist_id")){
            this->playlist_id = ProcJsonEasy::getInt(jsonObj, "playlist_id");
        }

        this->flagReqMore_track = false;
        this->flag_lastPage_track = false;
        this->flag_playlist_info = false;

        this->request_more_trackData();

        this->setUIControl_Editing();
    }
}


void AbstractAddPlaylist_RHV::setActivePage(){

    int height = 0;
    if(this->added_array.size() > 0){
        QJsonArray tmpData_arr = QJsonArray();

        if(this->added_array.size() < 4){
            height = 210;

            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(height);
        }
        else{
            height = 70 * this->added_array.size();

            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(height);
        }

        for(int i = 0; i < this->added_array.size(); i++){
            QJsonObject convertData = this->added_array.at(i).toObject();
            QJsonObject tmpData = QJsonObject();

            if(contents_type == ContentsType::Music){
                QString img = ProcJsonEasy::getString(convertData, "album_art");
                QString tmp_mime = ProcJsonEasy::getString(convertData, "album_mime");
                tmp_mime.replace("audio/", "");
                int tmp_sample = ProcJsonEasy::getInt(convertData, "samplerate");
                int tmp_bit = ProcJsonEasy::getInt(convertData, "bitdepth");
                bool tmp_explicit = false;

                QString mime = "";
                if(tmp_mime == "x-wav" || tmp_mime == "wav"){
                    tmp_mime = "WAV";
                }
                else if (tmp_mime == "wv") {
                    tmp_mime = "WV";
                }
                else if(tmp_mime == "flac"){
                    tmp_mime = "FLAC";
                }
                else if(tmp_mime == "dff" || tmp_mime == "dsf"){
                    tmp_mime = "DSD";
                }
                else if(tmp_mime == "mp4"){
                    tmp_mime = "ALAC";
                }
                else if(tmp_mime == "mpeg"){
                    tmp_mime = "MP3";
                }
                else if(tmp_mime == "amr"){
                    tmp_mime = "AMR";
                }
                else if(tmp_mime == "amr-wb"){
                    tmp_mime = "AWB";
                }
                else if(tmp_mime == "x-ms-wma"){
                    tmp_mime = "WMA";
                }
                else if(tmp_mime == "ape"){
                    tmp_mime = "APE";
                }
                else if(tmp_mime == "ogg"){
                    tmp_mime = "OGG";
                }
                else if(tmp_mime == "aac" || tmp_mime == "aac-adts"){
                    tmp_mime = "AAC";
                }
                else if(tmp_mime == "x-matroska"){
                    tmp_mime = "MKA";
                }
                else if(tmp_mime == "midi"){
                    tmp_mime = "MIDI";
                }
                else if(tmp_mime == "sp-midi"){
                    tmp_mime = "SMF";
                }
                else if(tmp_mime == "imelody"){
                    tmp_mime = "IMY";
                }
                else if(tmp_mime == "x-mpegurl"){
                    tmp_mime = "M3U8";
                }
                else if(tmp_mime == "x-scpls"){
                    tmp_mime = "PLS";
                }
                else if(tmp_mime == "aiff"){
                    tmp_mime = "AIFF";
                }

                if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                        || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                    mime = tmp_mime;
                }
                else if(tmp_mime == "DSD"){
                    if(tmp_sample == 88200 || tmp_sample == 2822400){
                        mime = tmp_mime + " 64";
                    }
                    else if(tmp_sample == 176400 || tmp_sample == 5644800){
                        mime = tmp_mime + " 128";
                    }
                    else if(tmp_sample == 352800 || tmp_sample == 11289600){
                        mime = tmp_mime + " 256";
                    }
                    else if(tmp_sample == 705600 || tmp_sample == 22579200){
                        mime = tmp_mime + " 512";
                    }
                    else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                        mime = tmp_mime + " 1024";
                    }
                    else{
                        mime = tmp_mime;
                    }
                }
                else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                    if(tmp_bit == 0 || tmp_bit == 16){
                        mime = tmp_mime;
                    }
                    else if(tmp_bit > 16){
                        mime = tmp_mime + QString(" %1").arg(tmp_bit);
                    }
                }
                else{
                    mime = tmp_mime;
                }

                QString thumbnail = "";
                if(img.startsWith("http")){
                    thumbnail = img;
                }
                else{
                    thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + img;
                }

                if(i == 0){
                    this->setImage(thumbnail);
                }

                QString title = ProcJsonEasy::getString(convertData, "title");
                QString artist = ProcJsonEasy::getString(convertData, "artist");
                QString album = ProcJsonEasy::getString(convertData, "album");
                int duration = ProcJsonEasy::getInt(convertData, "duration");

                tmpData.insert("thumbnail", thumbnail);
                tmpData.insert("title", title);
                tmpData.insert("artist", artist);
                tmpData.insert("album", album);
                tmpData.insert("duration", duration);
                tmpData.insert("bitrates", mime);
                tmpData.insert("explicit", tmp_explicit);
                tmpData.insert("type", "MUSIC");

                tmpData_arr.append(tmpData);
            }
            else if(contents_type == ContentsType::RoseTube){

                QString thumbnail = ProcJsonEasy::getString(convertData, "thumbnailUrl");
                QString title = ProcJsonEasy::getString(convertData, "title");
                bool tmp_explicit = false;

                QString artist = "";
                if(convertData.contains("artists")){
                    QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(convertData, "artists");

                    for(int i = 0; i < tmpArtists.size(); i++){
                        if(i == 0){
                            artist = tmpArtists.at(i).toString();
                        }
                        else{
                            artist += "," + tmpArtists.at(i).toString();
                        }
                    }
                }

                int duration = 0;
                duration = ProcJsonEasy::getInt(convertData, "duration");

                if(duration == 0){
                    QJsonObject tmpData = ProcJsonEasy::getJsonObject(convertData, "data");
                    QJsonObject tmpContents = ProcJsonEasy::getJsonObject(tmpData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(tmpContents, "duration");

                    if(!tmpDuration.isEmpty()){

                        QString time = tmpDuration;
                        QString hour;
                        QString minute;
                        QString second;

                        if(tmpDuration.indexOf("PT") >= 0){
                            time = tmpDuration.right(tmpDuration.size() - 2);
                        }
                        if(time.indexOf("H") >= 0){
                            hour = time.section("H", 0, -2);
                            time = time.mid(time.indexOf("H") + 1);
                        }
                        if(time.indexOf("M") >= 0){
                            minute = time.section("M", 0, -2);
                            time = time.mid(time.indexOf("M") + 1);
                        }
                        if(time.indexOf("S") >= 0){
                            second = time.section("S", 0, -2);
                        }

                        duration = (hour.toInt() * 3600) + (minute.toInt() * 60) + second.toInt();
                    }
                }

                if(i == 0){
                    this->setImage(thumbnail);
                }

                tmpData.insert("thumbnail", thumbnail);
                tmpData.insert("title", title);
                tmpData.insert("artist", artist);
                tmpData.insert("duration", duration);
                tmpData.insert("explicit", tmp_explicit);
                tmpData.insert("type", "YOUTUBE");

                tmpData_arr.append(tmpData);
            }
            else if(contents_type == ContentsType::Tidal){

                QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");

                QString imagePath = ProcJsonEasy::getString(tmpAlbum, "cover");
                QString thumbnail = "";

                if(imagePath.isEmpty()){
                    thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(convertData, "imageId"), 750);
                }
                else{
                    thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(tmpAlbum, "cover"), 750);
                }

                if(i == 0){
                    this->setImage(thumbnail);
                }

                QString artist = "";
                if(convertData.contains("artist")){
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                    artist = ProcJsonEasy::getString(tmpArtist, "name");
                }

                tmpData.insert("thumbnail", thumbnail);
                tmpData.insert("title", ProcJsonEasy::getString(convertData, "title"));
                tmpData.insert("artist", artist);
                tmpData.insert("album", ProcJsonEasy::getString(tmpAlbum, "title"));
                tmpData.insert("duration", ProcJsonEasy::getInt(convertData, "duration"));
                if(ProcJsonEasy::getString(convertData, "audioQuality") == "HI_RES"){
                    tmpData.insert("hires", true);
                }
                else{
                    tmpData.insert("hires", false);
                }
                tmpData.insert("maximum_sampling_rate", 0);
                tmpData.insert("explicit", ProcJsonEasy::getBool(convertData, "explicit"));
                tmpData.insert("type", "TIDAL");

                this->vtrack_id = ProcJsonEasy::getVariant(convertData, "id");

                tmpData_arr.append(tmpData);
            }
            else if(contents_type == ContentsType::Bugs){

                QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");
                QJsonObject albumImg = ProcJsonEasy::getJsonObject(tmpAlbum, "image");

                QString url = ProcJsonEasy::getString(albumImg, "url");
                QString path = ProcJsonEasy::getString(albumImg, "path");
                QString thumbnail = url.replace("https://", "http://");
                thumbnail += QString("%1").arg(static_cast<int>(bugs::ConvertData_forBugs::ImageSize_Square::Size_350x350)) + path;

                if(i == 0){
                    this->setImage(thumbnail);
                }

                QString artist = "";
                if(convertData.contains("artists")){
                    QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(convertData, "artists");

                    for(int i = 0; i < tmpArtists.size(); i++){
                        QJsonObject tmpArtistInfo = tmpArtists.at(i).toObject();
                        if(i == 0){
                            artist = ProcJsonEasy::getString(tmpArtistInfo, "artist_nm");
                        }
                        else{
                            artist += "," + ProcJsonEasy::getString(tmpArtistInfo, "artist_nm");
                        }
                    }
                }

                QString bitrate = "";
                QJsonArray bitrates1 = ProcJsonEasy::getJsonArray(convertData, "bitrates");//cheon210605

                if(bitrates1.contains("flac") == true){
                    bitrate = "FLAC";
                }
                else if(bitrates1.contains("aac320") == true){
                    bitrate = "AAC320";
                }
                else if(bitrates1.contains("320k") == true || bitrates1.contains("mp3") == true){
                    bitrate = "MP3";
                }
                else if(bitrates1.contains("aac") == true){
                    bitrate = "AAC";
                }
                else if(bitrates1.contains("fullhd") == true){
                    bitrate = "FULL HD";
                }
                else if(bitrates1.contains("hd") == true){
                    bitrate = "HD";
                }
                else if(bitrates1.contains("sd") == true){
                    bitrate = "SD";
                }

                tmpData.insert("thumbnail", thumbnail);
                tmpData.insert("title", ProcJsonEasy::getString(convertData, "track_title"));
                tmpData.insert("artist", artist);
                tmpData.insert("album", ProcJsonEasy::getString(tmpAlbum, "title"));
                tmpData.insert("duration", ProcJsonEasy::getString(convertData, "len"));
                tmpData.insert("bitrates", bitrate);
                tmpData.insert("explicit", ProcJsonEasy::getBool(convertData, "adult_yn"));
                tmpData.insert("type", "BUGS");

                tmpData_arr.append(tmpData);
            }
            else if(contents_type == ContentsType::Qobuz){

                QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");
                QJsonObject tmpImage = ProcJsonEasy::getJsonObject(tmpAlbum, "image");
                tmpData.insert("thumbnail", ProcJsonEasy::getString(tmpImage, "thumbnail"));

                if(i == 0){
                    this->setImage(ProcJsonEasy::getString(tmpImage, "large"));
                }

                QString artist = "";
                if(convertData.contains("artist")){
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                    artist = ProcJsonEasy::getString(tmpArtist, "name");
                }
                else{
                    if(tmpAlbum.contains("artist")){
                        QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(tmpAlbum, "artist");
                        artist = ProcJsonEasy::getString(tmpArtist, "name");
                    }
                    else{
                        QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "composer");
                        artist = ProcJsonEasy::getString(tmpArtist, "name");
                    }
                }

                tmpData.insert("title", ProcJsonEasy::getString(convertData, "title"));
                tmpData.insert("artist", artist);
                tmpData.insert("album", ProcJsonEasy::getString(tmpAlbum, "title"));
                tmpData.insert("duration", ProcJsonEasy::getInt(convertData, "duration"));
                tmpData.insert("hires", ProcJsonEasy::getBool(convertData, "hires"));
                tmpData.insert("maximum_sampling_rate", ProcJsonEasy::getDouble(convertData, "maximum_sampling_rate"));
                tmpData.insert("explicit", ProcJsonEasy::getBool(convertData, "parental_warning"));
                tmpData.insert("type", "QOBUZ");

                this->vtrack_id = ProcJsonEasy::getVariant(convertData, "id");

                tmpData_arr.append(tmpData);
            }
        }

        for(int i = 0; i < tmpData_arr.size(); i++){
            QJsonObject trackData = tmpData_arr.at(i).toObject();
            this->setDataTrackInfo_RoseListEdit_Item(trackData, i);
        }
    }
    else{

    }
}


void AbstractAddPlaylist_RHV::setUIControl_Creating(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->widget_info = new QWidget();
    this->widget_info->setFixedSize(1550, 315);
    this->widget_info->setGeometry(0, 0, 0, 0);
    this->widget_info->setContentsMargins(0, 20, 0, 50);

    this->lb_image = new QLabel(this->widget_info);
    this->lb_image->setStyleSheet("background-color:#333");
    this->lb_image->setFixedSize(315, 315);
    this->lb_image->setGeometry(0, 0, 0, 0);

    this->setImage("");

    this->widget_info_title = new QWidget(this->widget_info);
    this->widget_info_title->setStyleSheet("background-color:#333");
    this->widget_info_title->setFixedSize(1125, 50);
    this->widget_info_title->setGeometry(375, 10, 0, 0);

    this->le_title = new QLineEdit(this->widget_info_title);
    this->le_title->setProperty("type", "title");
    this->le_title->setMaxLength(51);
    this->le_title->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_title->setPlaceholderText(tr("Playlist Title"));
    this->le_title->setGeometry(22, 12, 800, 26);

    connect(this->le_title, &QLineEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_title_text);

    this->lb_title = new QLabel(this->widget_info_title);
    this->lb_title->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_title->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_title->setText("0/50 " + tr("Charaters"));
    this->lb_title->setGeometry(900, 12, 200, 26);


    this->widget_info_content = new QWidget(this->widget_info);
    this->widget_info_content->setStyleSheet("background-color:#333");
    this->widget_info_content->setFixedSize(1125, 77);
    this->widget_info_content->setGeometry(375, 75, 0, 0);

    this->te_content = new QTextEdit(this->widget_info_content);
    this->te_content->setProperty("type", "contents");
    this->te_content->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;line-height:2pt");
    this->te_content->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->te_content->setPlaceholderText(tr("Please enter\nPlaylist introduction."));
    this->te_content->setGeometry(22, 9, 900, 52);

    connect(this->te_content, &QTextEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_content_text);

    this->lb_content = new QLabel(this->widget_info_content);
    this->lb_content->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_content->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_content->setText("0/500 " + tr("Charaters"));
    this->lb_content->setGeometry(900, 30, 200, 26);


    int pub_left = 0;
    int pub_width = 0;

    this->lb_public_set = new QLabel(widget_info);
    this->lb_public_set->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_set->setText(tr("Public"));

    pub_width = this->lb_public_set->sizeHint().width();
    this->lb_public_set->setGeometry(375, 174, pub_width, 26);

    this->btn_all = GSCommon::getUIBtnImg("btnAll", ":/images/icon_che_off.png", widget_info);
    this->btn_all->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_set->geometry().left() + lb_public_set->geometry().width() + 25;
    this->btn_all->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_all, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_all = new QLabel(widget_info);
    this->lb_public_all->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_all->setText(tr("ALL"));

    pub_left = this->btn_all->geometry().left() + this->btn_all->geometry().width();
    pub_width = this->lb_public_all->sizeHint().width();
    this->lb_public_all->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_freind = GSCommon::getUIBtnImg("btnFreind", ":/images/icon_che_off.png", widget_info);
    this->btn_freind->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_all->geometry().left() + lb_public_all->geometry().width() + 50;
    this->btn_freind->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_freind, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_freind = new QLabel(widget_info);
    this->lb_public_freind->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_freind->setText(tr("FRIEND"));

    pub_left = this->btn_freind->geometry().left() + this->btn_freind->geometry().width();
    pub_width = this->lb_public_freind->sizeHint().width();
    this->lb_public_freind->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_private = GSCommon::getUIBtnImg("btnPrivate", ":/images/icon_che_off.png", widget_info);
    this->btn_private->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_freind->geometry().left() + lb_public_freind->geometry().width() + 50;
    this->btn_private->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_private, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_private = new QLabel(widget_info);
    this->lb_public_private->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_private->setText(tr("PRIVATE"));

    pub_left = this->btn_private->geometry().left() + this->btn_private->geometry().width();
    pub_width = this->lb_public_private->sizeHint().width();
    this->lb_public_private->setGeometry(pub_left, 174, pub_width, 26);

    if(this->contents_type == ContentsType::Music || this->playlist_type == "MUSIC"){
        this->btn_all->setEnabled(true);

        this->flag_all_enable = false;
        this->flag_freind_enable = true;
        this->flag_private_enable = false;

        this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else{
        this->flag_all_enable = true;
        this->flag_freind_enable = false;
        this->flag_private_enable = false;

        this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }


    this->widget_info_tag = new QWidget(this->widget_info);
    this->widget_info_tag->setStyleSheet("background-color:#333");
    this->widget_info_tag->setFixedSize(1125, 50);
    this->widget_info_tag->setGeometry(375, 220, 0, 0);

    this->le_tag = new QLineEdit(this->widget_info_tag);
    this->le_tag->setProperty("type", "tag");
    this->le_tag->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_tag->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_tag->setPlaceholderText(tr("#Please enter your tags."));
    this->le_tag->setGeometry(22, 12, 1000, 26);

    pub_left = 0;
    pub_width = 0;
    for(int i = 0; i < this->tagTypeList.size(); i++){
        this->btn_tag[i] = new QPushButton(widget_info);
        this->btn_tag[i]->setProperty("idx", i);
        this->btn_tag[i]->setFixedHeight(30);
        this->btn_tag[i]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
        this->btn_tag[i]->setCursor(Qt::PointingHandCursor);
        this->btn_tag[i]->setText(this->tagTypeList[i].replace("&", "&&"));

        if(pub_left == 0 && pub_width == 0){
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(375, 280, pub_width, 30);
        }
        else{
            pub_left = this->btn_tag[i - 1]->geometry().left() + this->btn_tag[i - 1]->geometry().width() + 10;
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(pub_left, 280, pub_width, 30);
        }

        connect(this->btn_tag[i], &QPushButton::pressed, this, &AbstractAddPlaylist_RHV::slot_btnTags_pressed);
        connect(this->btn_tag[i], &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnTags_clicked);
    }

    if(contents_type == ContentsType::Qobuz || contents_type == ContentsType::Tidal){

        if(contents_type == ContentsType::Qobuz){
            this->lb_listCheck->setText(tr("Save also to QOBUZ"));
        }
        else if(contents_type == ContentsType::Tidal){
            this->lb_listCheck->setText(tr("Save also to TIDAL"));
        }

        this->lb_selectList->setText(tr("Select Playlist"));

        this->btn_withStream->show();
        this->btn_selectList->show();
        this->btn_selectList->setEnabled(false);

        this->btn_delete->setGeometry(850, 0, this->btn_delete->geometry().width(), 50);
    }
    else{
        this->btn_delete->setGeometry((1550 - 24 - this->btn_delete->geometry().width()), 0, this->btn_delete->geometry().width(), 50);
    }

    this->lb_delete->setText(tr("Delete Tracks"));
    this->lb_delete->setEnabled(true);
    this->lb_delete->show();

    this->btn_delete->show();

    this->listWidget_adding = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_adding->setCursor(Qt::PointingHandCursor);
    this->listWidget_adding->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_adding->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_adding->setStyleSheet("background-color:transparent;");

    connect(this->listWidget_adding, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));
    connect(this->linker, SIGNAL(signal_dragDrop_playlist()), this, SLOT(slot_dragAndDropLoop()));

    this->vl_Adding = new QVBoxLayout();
    this->vl_Adding->setContentsMargins(5, 0, 5, 0);
    this->vl_Adding->setSpacing(0);
    this->vl_Adding->setAlignment(Qt::AlignTop);
    this->vl_Adding->addWidget(this->listWidget_adding);

    this->widget_Adding = new QWidget();
    this->widget_Adding->setLayout(this->vl_Adding);

    this->listWidget_existing = new QListWidget(this);
    this->listWidget_existing->setCursor(Qt::PointingHandCursor);
    this->listWidget_existing->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_existing->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_existing->setStyleSheet("background-color:transparent;");

    this->vl_Existing = new QVBoxLayout();
    this->vl_Existing->setContentsMargins(5, 0, 5, 0);
    this->vl_Existing->setSpacing(0);
    this->vl_Existing->setAlignment(Qt::AlignTop);
    this->vl_Existing->addWidget(this->listWidget_existing);

    this->widget_Existing = new QWidget();
    this->widget_Existing->setLayout(this->vl_Existing);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->widget_Adding);
    this->stackedwidget->addWidget(this->widget_Existing);

    this->widget_button = new QWidget();
    this->widget_button->setFixedSize(1550, 120);

    this->btn_confirm = new QPushButton(this->widget_button);
    this->btn_confirm->setText(tr("Confirm"));
    this->btn_confirm->setStyleSheet("QPushButton{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;} "
                                     "QPushButton:hover{background:#7d6144; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #7d6144; border-radius:20px;}");
    this->btn_confirm->setCursor(Qt::PointingHandCursor);
    this->btn_confirm->setFixedSize(140, 40);
    this->btn_confirm->setGeometry(590, 30, 0, 0);

    connect(this->btn_confirm, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnConfirm);

    this->btn_cancel = new QPushButton(this->widget_button);
    this->btn_cancel->setText(tr("Cancel"));
    this->btn_cancel->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                     "QPushButton:hover{background:#777777; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777; border-radius:20px;}");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(760, 30, 0, 0);

    connect(this->btn_cancel, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnCancel);

    this->vl_addPlaylist = new QVBoxLayout();
    this->vl_addPlaylist->setAlignment(Qt::AlignTop);
    this->vl_addPlaylist->addWidget(this->widget_info);
    this->vl_addPlaylist->addSpacing(45);
    this->vl_addPlaylist->addWidget(this->widget_menubar);
    this->vl_addPlaylist->addWidget(this->stackedwidget);
    this->vl_addPlaylist->addSpacing(30);
    this->vl_addPlaylist->addWidget(this->widget_button);

    this->setLayout(this->vl_addPlaylist);
}


void AbstractAddPlaylist_RHV::setUIControl_Adding(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->widget_info = new QWidget();
    this->widget_info->setFixedSize(1550, 315);
    this->widget_info->setGeometry(0, 0, 0, 0);
    this->widget_info->setContentsMargins(0, 20, 0, 50);

    this->lb_image = new QLabel(this->widget_info);
    this->lb_image->setStyleSheet("background-color:#333");
    this->lb_image->setFixedSize(315, 315);
    this->lb_image->setGeometry(0, 0, 0, 0);

    this->setImage("");

    this->widget_info_title = new QWidget(this->widget_info);
    this->widget_info_title->setStyleSheet("background-color:#333");
    this->widget_info_title->setFixedSize(1125, 50);
    this->widget_info_title->setGeometry(375, 10, 0, 0);

    QString tmp_title = ProcJsonEasy::getString(this->playlist_info, "title");

    this->le_title = new QLineEdit(this->widget_info_title);
    this->le_title->setProperty("type", "title");
    this->le_title->setMaxLength(51);
    this->le_title->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_title->setText(tmp_title);
    this->le_title->setGeometry(22, 12, 800, 26);

    connect(this->le_title, &QLineEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_title_text);

    this->lb_title = new QLabel(this->widget_info_title);
    this->lb_title->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_title->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_title->setText(QString("%1/50 ").arg(tmp_title.size()) + tr("Charaters"));
    this->lb_title->setGeometry(900, 12, 200, 26);


    this->widget_info_content = new QWidget(this->widget_info);
    this->widget_info_content->setStyleSheet("background-color:#333");
    this->widget_info_content->setFixedSize(1125, 77);
    this->widget_info_content->setGeometry(375, 75, 0, 0);

    QString tmp_content = ProcJsonEasy::getString(this->playlist_info, "comment");

    this->te_content = new QTextEdit(this->widget_info_content);
    this->te_content->setProperty("type", "contents");
    this->te_content->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;line-height:2pt");
    this->te_content->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->te_content->setText(tmp_content);
    this->te_content->setGeometry(22, 9, 900, 52);

    connect(this->te_content, &QTextEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_content_text);

    this->lb_content = new QLabel(this->widget_info_content);
    this->lb_content->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_content->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_content->setText(QString("%1/500 ").arg(tmp_content.size()) + tr("Charaters"));
    this->lb_content->setGeometry(900, 30, 200, 26);


    int pub_left = 0;
    int pub_width = 0;

    this->lb_public_set = new QLabel(widget_info);
    this->lb_public_set->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_set->setText(tr("Public"));

    pub_width = this->lb_public_set->sizeHint().width();
    this->lb_public_set->setGeometry(375, 174, pub_width, 26);

    this->btn_all = GSCommon::getUIBtnImg("btnAll", ":/images/icon_che_off.png", widget_info);
    this->btn_all->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_set->geometry().left() + lb_public_set->geometry().width() + 25;
    this->btn_all->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_all, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_all = new QLabel(widget_info);
    this->lb_public_all->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_all->setText(tr("ALL"));

    pub_left = this->btn_all->geometry().left() + this->btn_all->geometry().width();
    pub_width = this->lb_public_all->sizeHint().width();
    this->lb_public_all->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_freind = GSCommon::getUIBtnImg("btnFreind", ":/images/icon_che_off.png", widget_info);
    this->btn_freind->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_all->geometry().left() + lb_public_all->geometry().width() + 50;
    this->btn_freind->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_freind, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_freind = new QLabel(widget_info);
    this->lb_public_freind->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_freind->setText(tr("FRIEND"));

    pub_left = this->btn_freind->geometry().left() + this->btn_freind->geometry().width();
    pub_width = this->lb_public_freind->sizeHint().width();
    this->lb_public_freind->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_private = GSCommon::getUIBtnImg("btnPrivate", ":/images/icon_che_off.png", widget_info);
    this->btn_private->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_freind->geometry().left() + lb_public_freind->geometry().width() + 50;
    this->btn_private->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_private, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_private = new QLabel(widget_info);
    this->lb_public_private->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_private->setText(tr("PRIVATE"));

    pub_left = this->btn_private->geometry().left() + this->btn_private->geometry().width();
    pub_width = this->lb_public_private->sizeHint().width();
    this->lb_public_private->setGeometry(pub_left, 174, pub_width, 26);


    if(this->contents_type == ContentsType::Music || this->playlist_type == "MUSIC"){
        this->btn_all->setEnabled(false);
    }

    if(this->shared_type == "ALL"){
        this->flag_all_enable = true;
        this->flag_freind_enable = false;
        this->flag_private_enable = false;

        this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else if(this->shared_type == "FRIEND"){
        this->flag_all_enable = false;
        this->flag_freind_enable = true;
        this->flag_private_enable = false;

        this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else{
        this->flag_all_enable = false;
        this->flag_freind_enable = false;
        this->flag_private_enable = true;

        this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }


    this->widget_info_tag = new QWidget(this->widget_info);
    this->widget_info_tag->setStyleSheet("background-color:#333");
    this->widget_info_tag->setFixedSize(1125, 50);
    this->widget_info_tag->setGeometry(375, 220, 0, 0);

    this->le_tag = new QLineEdit(this->widget_info_tag);
    this->le_tag->setProperty("type", "tag");
    this->le_tag->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_tag->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_tag->setPlaceholderText(tr("#Please enter your tags."));
    this->le_tag->setGeometry(22, 12, 1000, 26);

    QList<QVariant> list_tags = ProcJsonEasy::getJsonArray(this->playlist_info, "tags").toVariantList();

    QString tags = "";
    for(int i = 0; i < list_tags.size(); i++){
        if(!list_tags.at(i).toString().isEmpty()){
            if(i == 0){
                tags = "#" + list_tags.at(i).toString();
            }
            else{
                tags += ",#" + list_tags.at(i).toString();
            }
        }
    }

    this->le_tag->setText(tags);

    pub_left = 0;
    pub_width = 0;
    for(int i = 0; i < this->tagTypeList.size(); i++){
        this->btn_tag[i] = new QPushButton(widget_info);
        this->btn_tag[i]->setProperty("idx", i);
        this->btn_tag[i]->setFixedHeight(30);
        this->btn_tag[i]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
        this->btn_tag[i]->setCursor(Qt::PointingHandCursor);
        this->btn_tag[i]->setText(this->tagTypeList[i].replace("&", "&&"));

        if(pub_left == 0 && pub_width == 0){
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(375, 280, pub_width, 30);
        }
        else{
            pub_left = this->btn_tag[i - 1]->geometry().left() + this->btn_tag[i - 1]->geometry().width() + 10;
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(pub_left, 280, pub_width, 30);
        }

        connect(this->btn_tag[i], &QPushButton::pressed, this, &AbstractAddPlaylist_RHV::slot_btnTags_pressed);
        connect(this->btn_tag[i], &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnTags_clicked);
    }

    if(contents_type == ContentsType::Qobuz || contents_type == ContentsType::Tidal){

        if(contents_type == ContentsType::Qobuz){
            this->lb_listCheck->setText(tr("Save also to QOBUZ"));
        }
        else if(contents_type == ContentsType::Tidal){
            this->lb_listCheck->setText(tr("Save also to TIDAL"));
        }

        this->lb_selectList->setText(tr("Select Playlist"));

        this->btn_withStream->show();
        this->btn_selectList->show();
        this->btn_selectList->setEnabled(false);

        this->btn_delete->setGeometry(850, 0, this->btn_delete->geometry().width(), 50);
    }
    else{
        this->btn_delete->setGeometry((1550 - 24 - this->btn_delete->geometry().width()), 0, this->btn_delete->geometry().width(), 50);
    }

    this->lb_delete->setText(tr("Delete Tracks"));
    this->lb_delete->setEnabled(true);
    this->lb_delete->show();

    this->btn_delete->show();

    this->listWidget_adding = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_adding->setCursor(Qt::PointingHandCursor);
    this->listWidget_adding->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_adding->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_adding->setStyleSheet("background-color:transparent;");

    connect(this->listWidget_adding, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem*)));
    connect(this->linker, SIGNAL(signal_dragDrop_playlist()), this, SLOT(slot_dragAndDropLoop()));

    this->vl_Adding = new QVBoxLayout();
    this->vl_Adding->setContentsMargins(5, 0, 5, 0);
    this->vl_Adding->setSpacing(0);
    this->vl_Adding->setAlignment(Qt::AlignTop);
    this->vl_Adding->addWidget(this->listWidget_adding);

    this->widget_Adding = new QWidget();
    this->widget_Adding->setLayout(this->vl_Adding);

    this->listWidget_existing = new QListWidget(this);
    this->listWidget_existing->setCursor(Qt::PointingHandCursor);
    this->listWidget_existing->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_existing->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_existing->setStyleSheet("background-color:transparent;");


    this->vl_Existing = new QVBoxLayout();
    this->vl_Existing->setContentsMargins(5, 0, 5, 0);
    this->vl_Existing->setSpacing(0);
    this->vl_Existing->setAlignment(Qt::AlignTop);
    this->vl_Existing->addWidget(this->listWidget_existing);

    this->widget_Existing = new QWidget();
    this->widget_Existing->setLayout(this->vl_Existing);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->widget_Adding);
    this->stackedwidget->addWidget(this->widget_Existing);

    this->widget_button = new QWidget();
    this->widget_button->setFixedSize(1550, 120);

    this->btn_confirm = new QPushButton(this->widget_button);
    this->btn_confirm->setText(tr("Confirm"));
    this->btn_confirm->setStyleSheet("QPushButton{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;} "
                                     "QPushButton:hover{background:#7d6144; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #7d6144; border-radius:20px;}");
    this->btn_confirm->setCursor(Qt::PointingHandCursor);
    this->btn_confirm->setFixedSize(140, 40);
    this->btn_confirm->setGeometry(590, 30, 0, 0);

    connect(this->btn_confirm, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnConfirm);

    this->btn_cancel = new QPushButton(this->widget_button);
    this->btn_cancel->setText(tr("Cancel"));
    this->btn_cancel->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                     "QPushButton:hover{background:#777777; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777; border-radius:20px;}");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(760, 30, 0, 0);

    connect(this->btn_cancel, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnCancel);

    this->vl_existPlaylist = new QVBoxLayout();
    this->vl_existPlaylist->setAlignment(Qt::AlignTop);
    this->vl_existPlaylist->addWidget(this->widget_info);
    this->vl_existPlaylist->addSpacing(45);
    this->vl_existPlaylist->addWidget(this->widget_menubar);
    this->vl_existPlaylist->addWidget(this->stackedwidget);
    this->vl_existPlaylist->addSpacing(30);
    this->vl_existPlaylist->addWidget(this->widget_button);

    this->setLayout(this->vl_existPlaylist);
}


void AbstractAddPlaylist_RHV::setUIControl_Editing(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->widget_info = new QWidget();
    this->widget_info->setFixedSize(1550, 451);
    this->widget_info->setGeometry(0, 0, 0, 0);
    this->widget_info->setContentsMargins(0, 20, 0, 50);

    QLabel *lb_infoTitle = new QLabel(this->widget_info);
    lb_infoTitle->setFixedHeight(36);
    lb_infoTitle->setStyleSheet("background-color:transparent;font-size:24px;font-weight:bold;color:#FFF;");
    lb_infoTitle->setText(tr("Edit content"));

    int width = lb_infoTitle->sizeHint().width();
    lb_infoTitle->setGeometry(0, 0, width, 0);

    QLabel *lb_infoExplan = new QLabel(this->widget_info);
    lb_infoExplan->setFixedHeight(18);
    lb_infoExplan->setStyleSheet("background-color:transparent;font-size:16px;font-weight:normal;color:#777;");
    lb_infoExplan->setText(tr("* It is not with like editing the list, and when saved, it is immediately reflected in the playlist."));

    width = lb_infoExplan->sizeHint().width();
    int left = lb_infoTitle->sizeHint().width() + 10;
    lb_infoExplan->setGeometry(left, 12, width, 0);

    this->lb_image = new QLabel(this->widget_info);
    this->lb_image->setStyleSheet("background-color:#333");
    this->lb_image->setFixedSize(315, 315);
    this->lb_image->setGeometry(0, 56, 0, 0);

    this->setImage("");

    this->widget_info_title = new QWidget(this->widget_info);
    this->widget_info_title->setStyleSheet("background-color:#333");
    this->widget_info_title->setFixedSize(1125, 50);
    this->widget_info_title->setGeometry(375, 66, 0, 0);

    QString tmp_title = ProcJsonEasy::getString(this->playlist_info, "title");

    this->le_title = new QLineEdit(this->widget_info_title);
    this->le_title->setProperty("type", "title");
    this->le_title->setMaxLength(51);
    this->le_title->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_title->setText(tmp_title);
    this->le_title->setGeometry(22, 12, 800, 26);

    connect(this->le_title, &QLineEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_title_text);

    this->lb_title = new QLabel(this->widget_info_title);
    this->lb_title->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_title->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_title->setText(QString("%1/50 ").arg(tmp_title.size()) + tr("Charaters"));
    this->lb_title->setGeometry(900, 12, 200, 26);


    this->widget_info_content = new QWidget(this->widget_info);
    this->widget_info_content->setStyleSheet("background-color:#333");
    this->widget_info_content->setFixedSize(1125, 77);
    this->widget_info_content->setGeometry(375, 131, 0, 0);

    QString tmp_content = ProcJsonEasy::getString(this->playlist_info, "comment");

    this->te_content = new QTextEdit(this->widget_info_content);
    this->te_content->setProperty("type", "contents");
    this->te_content->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;line-height:2pt");
    this->te_content->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->te_content->setText(tmp_content);
    this->te_content->setGeometry(22, 9, 900, 52);

    connect(this->te_content, &QTextEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_content_text);

    this->lb_content = new QLabel(this->widget_info_content);
    this->lb_content->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_content->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_content->setText(QString("%1/500 ").arg(tmp_content.size()) + tr("Charaters"));
    this->lb_content->setGeometry(900, 30, 200, 26);


    int pub_left = 0;
    int pub_width = 0;

    this->lb_public_set = new QLabel(this->widget_info);
    this->lb_public_set->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_set->setText(tr("Public"));

    pub_width = this->lb_public_set->sizeHint().width();
    this->lb_public_set->setGeometry(375, 230, pub_width, 26);

    this->btn_all = GSCommon::getUIBtnImg("btnAll", ":/images/icon_che_off.png", widget_info);
    this->btn_all->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_set->geometry().left() + lb_public_set->geometry().width() + 25;
    this->btn_all->setGeometry(pub_left, 223, 40, 40);

    connect(this->btn_all, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_all = new QLabel(widget_info);
    this->lb_public_all->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_all->setText(tr("ALL"));

    pub_left = this->btn_all->geometry().left() + this->btn_all->geometry().width();
    pub_width = this->lb_public_all->sizeHint().width();
    this->lb_public_all->setGeometry(pub_left, 230, pub_width, 26);

    this->btn_freind = GSCommon::getUIBtnImg("btnFreind", ":/images/icon_che_off.png", widget_info);
    this->btn_freind->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_all->geometry().left() + lb_public_all->geometry().width() + 50;
    this->btn_freind->setGeometry(pub_left, 223, 40, 40);

    connect(this->btn_freind, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_freind = new QLabel(widget_info);
    this->lb_public_freind->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_freind->setText(tr("FRIEND"));

    pub_left = this->btn_freind->geometry().left() + this->btn_freind->geometry().width();
    pub_width = this->lb_public_freind->sizeHint().width();
    this->lb_public_freind->setGeometry(pub_left, 230, pub_width, 26);

    this->btn_private = GSCommon::getUIBtnImg("btnPrivate", ":/images/icon_che_off.png", widget_info);
    this->btn_private->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_freind->geometry().left() + lb_public_freind->geometry().width() + 50;
    this->btn_private->setGeometry(pub_left, 223, 40, 40);

    connect(this->btn_private, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_private = new QLabel(widget_info);
    this->lb_public_private->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_private->setText(tr("PRIVATE"));

    pub_left = this->btn_private->geometry().left() + this->btn_private->geometry().width();
    pub_width = this->lb_public_private->sizeHint().width();
    this->lb_public_private->setGeometry(pub_left, 230, pub_width, 26);

    if(this->contents_type == ContentsType::Music || this->playlist_type == "MUSIC"){
        this->btn_all->setEnabled(false);
    }

    this->widget_info_tag = new QWidget(this->widget_info);
    this->widget_info_tag->setStyleSheet("background-color:#333");
    this->widget_info_tag->setFixedSize(1125, 50);
    this->widget_info_tag->setGeometry(375, 276, 0, 0);

    this->le_tag = new QLineEdit(this->widget_info_tag);
    this->le_tag->setProperty("type", "tag");
    this->le_tag->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_tag->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_tag->setPlaceholderText(tr("#Please enter your tags."));
    this->le_tag->setGeometry(22, 12, 1000, 26);

    QList<QVariant> list_tags = ProcJsonEasy::getJsonArray(this->playlist_info, "tags").toVariantList();

    QString tags = "";
    for(int i = 0; i < list_tags.size(); i++){
        if(!list_tags.at(i).toString().isEmpty()){
            if(i == 0){
                tags = "#" + list_tags.at(i).toString();
            }
            else{
                tags += ",#" + list_tags.at(i).toString();
            }
        }
    }

    this->le_tag->setText(tags);

    pub_left = 0;
    pub_width = 0;
    for(int i = 0; i < this->tagTypeList.size(); i++){
        this->btn_tag[i] = new QPushButton(widget_info);
        this->btn_tag[i]->setProperty("idx", i);
        this->btn_tag[i]->setFixedHeight(30);
        this->btn_tag[i]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
        this->btn_tag[i]->setCursor(Qt::PointingHandCursor);
        this->btn_tag[i]->setText(this->tagTypeList[i].replace("&", "&&"));

        if(pub_left == 0 && pub_width == 0){
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(375, 336, pub_width, 30);
        }
        else{
            pub_left = this->btn_tag[i - 1]->geometry().left() + this->btn_tag[i - 1]->geometry().width() + 10;
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(pub_left, 336, pub_width, 30);
        }

        connect(this->btn_tag[i], &QPushButton::pressed, this, &AbstractAddPlaylist_RHV::slot_btnTags_pressed);
        connect(this->btn_tag[i], &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnTags_clicked);
    }

    this->btn_edit_confirm = new QPushButton(this->widget_info);
    this->btn_edit_confirm->setText(tr("Save Editing Content"));
    this->btn_edit_confirm->setStyleSheet("QPushButton{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658;border-radius:20px;} "
                                     "QPushButton:hover{background:#7d6144; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #7d6144; border-radius:20px;}");
    this->btn_edit_confirm->setCursor(Qt::PointingHandCursor);
    this->btn_edit_confirm->setFixedSize(260, 40);
    this->btn_edit_confirm->setGeometry(621, 411, 0, 0);

    connect(this->btn_edit_confirm, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnEditConfirm_clicked);


    QWidget *widget_track = new QWidget();
    widget_track->setFixedSize(1550, 100);
    widget_track->setStyleSheet("background-color:transparent;");

    QLabel *lb_trackInfo = new QLabel(widget_track);
    lb_trackInfo->setFixedHeight(36);
    lb_trackInfo->setStyleSheet("background-color:transparent;font-size:24px;font-weight:bold;color:#FFF;");
    lb_trackInfo->setText(tr("Edit list"));

    width = lb_trackInfo->sizeHint().width();
    lb_trackInfo->setGeometry(0, 0, width, 0);

    QLabel *lb_trackExplan = new QLabel(widget_track);
    lb_trackExplan->setFixedHeight(18);
    lb_trackExplan->setStyleSheet("background-color:transparent;font-size:16px;font-weight:normal;color:#777;");
    lb_trackExplan->setText(tr("* It is not saved with editing the content, and when saved, it is immediately reflected in the playlist."));

    width = lb_trackExplan->sizeHint().width();
    left = lb_trackInfo->sizeHint().width() + 10;
    lb_trackExplan->setGeometry(left, 12, width, 0);

    QWidget *widget_track_editBtn = new QWidget(widget_track);
    widget_track_editBtn->setFixedSize(1550, 50);
    widget_track_editBtn->setStyleSheet("background-color:transparent; background-color:transparent; border-top:1px solid #CCC; border-bottom:1px solid #CCC;");
    widget_track_editBtn->setGeometry(0, 50, 0, 0);

    this->checkbox_selectAll = new QCheckBox(widget_track_editBtn);
    this->checkbox_selectAll->setFixedSize(50, 50);
    this->checkbox_selectAll->setGeometry(0, 0, 0, 0);
    this->checkbox_selectAll->setCursor(Qt::PointingHandCursor);
    this->checkbox_selectAll->setStyleSheet("QCheckBox::indicator {width:50px; height:50px;}"
                                      "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                      "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

    connect(this->checkbox_selectAll, &QCheckBox::stateChanged, this, &AbstractAddPlaylist_RHV::slot_clickedSelected);

    QPushButton *btn_selectAll = new QPushButton(widget_track_editBtn);
    btn_selectAll->setFixedHeight(18);
    btn_selectAll->setStyleSheet("background-color:transparent;border:0px;font-size:16px;font-weight:normal;color:#b18658;");
    btn_selectAll->setCursor(Qt::PointingHandCursor);
    btn_selectAll->setText(tr("Select All"));
    btn_selectAll->setGeometry(50, 16, btn_selectAll->sizeHint().width(), 0);

    connect(btn_selectAll, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnSelectAll);

    QPushButton *btn_deleteAll = new QPushButton(widget_track_editBtn);
    btn_deleteAll->setFixedHeight(18);
    btn_deleteAll->setStyleSheet("background-color:transparent;border:0px;font-size:16px;font-weight:normal;color:#b18658;");
    btn_deleteAll->setCursor(Qt::PointingHandCursor);
    btn_deleteAll->setText(tr("Delete"));
    btn_deleteAll->setGeometry((1550 - 24 - btn_deleteAll->sizeHint().width()), 16, btn_deleteAll->sizeHint().width(), 0);

    connect(btn_deleteAll, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clickedDelete);

    this->listWidget_editing = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_editing->setCursor(Qt::PointingHandCursor);
    this->listWidget_editing->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_editing->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_editing->setStyleSheet("background-color:transparent;");

    connect(this->listWidget_editing, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));
    connect(this->linker, SIGNAL(signal_dragDrop_playlist()), this, SLOT(slot_dragAndDropLoop()));

    this->vl_Editing = new QVBoxLayout();
    this->vl_Editing->setContentsMargins(5, 0, 5, 0);
    this->vl_Editing->setSpacing(0);
    this->vl_Editing->setAlignment(Qt::AlignTop);
    this->vl_Editing->addWidget(widget_track);
    this->vl_Editing->addWidget(this->listWidget_editing);

    this->widget_Editing = new QWidget();
    this->widget_Editing->setLayout(this->vl_Editing);

    this->widget_button = new QWidget();
    this->widget_button->setFixedSize(1550, 120);

    this->btn_confirm = new QPushButton(this->widget_button);
    this->btn_confirm->setText(tr("Confirm"));
    this->btn_confirm->setStyleSheet("QPushButton{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;} "
                                     "QPushButton:hover{background:#7d6144; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #7d6144; border-radius:20px;}");
    this->btn_confirm->setCursor(Qt::PointingHandCursor);
    this->btn_confirm->setFixedSize(140, 40);
    this->btn_confirm->setGeometry(590, 30, 0, 0);

    connect(this->btn_confirm, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnConfirm);

    this->btn_cancel = new QPushButton(this->widget_button);
    this->btn_cancel->setText(tr("Cancel"));
    this->btn_cancel->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777; border-radius:20px;} "
                                     "QPushButton:hover{background:#777777; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777; border-radius:20px;}");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(760, 30, 0, 0);

    connect(this->btn_cancel, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnCancel);

    this->vl_editPlaylist = new QVBoxLayout();
    this->vl_editPlaylist->setAlignment(Qt::AlignTop);
    this->vl_editPlaylist->addWidget(this->widget_info);
    this->vl_editPlaylist->addSpacing(45);
    this->vl_editPlaylist->addWidget(this->widget_Editing);
    this->vl_editPlaylist->addSpacing(30);
    this->vl_editPlaylist->addWidget(this->widget_button);

    this->setLayout(this->vl_editPlaylist);
}


void AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item(const QJsonObject &trackInfo, const int &p_index){

    QString type = ProcJsonEasy::getString(trackInfo, "type");
    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    QString artist = ProcJsonEasy::getString(trackInfo, "artist");
    QString album  = ProcJsonEasy::getString(trackInfo, "album");
    QString bitrates = ProcJsonEasy::getString(trackInfo, "bitrates");

    int duration = ProcJsonEasy::getInt(trackInfo, "duration");

    double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

    bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
    bool explicit_flag = ProcJsonEasy::getBool(trackInfo, "explicit");

    QWidget *widget_track_info = new QWidget();
    widget_track_info->setFixedSize(1550, 70);
    widget_track_info->setObjectName("widget_track_info");
    widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                        "#widget_track_info:hover{background-color:#333333;} ");

    this->checkbox_Selected[p_index] = new QCheckBox(widget_track_info);
    this->checkbox_Selected[p_index]->setFixedSize(50, 50);
    this->checkbox_Selected[p_index]->setGeometry(0, 10, 0, 0);
    this->checkbox_Selected[p_index]->setProperty("type", "ADD");
    this->checkbox_Selected[p_index]->setProperty("index", p_index);
    this->checkbox_Selected[p_index]->setStyleSheet("QCheckBox::indicator {width:50px; height:50px;}"
                                                    "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                    "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

    QString default_path = "";
    QString type_path = "";
    QString hires_path = "";
    if(type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
        hires_path = ":/images/tidal/mqa_ico.png";
    }
    else if(type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
        hires_path = ":/images/qobuz/hires_ico.png";
    }
    else if(type == "APPLE_MUSIC"){
        default_path = ":/images/apple/apple_def.png";
        type_path = ":/images/rosehome/home_apple.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    QImage img_album;
    QPixmap *pixmap_image = new QPixmap();
    if(img_album.load(default_path)){
        *pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_image = pixmap_image->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_trackImg_adding[p_index] = new QLabel(widget_track_info);
    this->lb_trackImg_adding[p_index]->setFixedSize(60, 60);
    this->lb_trackImg_adding[p_index]->setGeometry(60, 5, 0, 0);
    this->lb_trackImg_adding[p_index]->setStyleSheet("background-color:transparent;");
    this->lb_trackImg_adding[p_index]->setPixmap(*pixmap_image);

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", p_index);
    fileDownloader->setImageURL(thumbnail);
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_adding()));

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
    *pixmap_icon = pixmap_icon->scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *lb_track_type = new QLabel(widget_track_info);
    lb_track_type->setPixmap(*pixmap_icon);
    lb_track_type->setGeometry(128, 15, 40, 40);
    lb_track_type->setStyleSheet("background-color:transparent;");

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QLabel *lb_title = new QLabel(widget_track_info);
    lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");

    QLabel *lb_hires = new QLabel(widget_track_info);
    lb_hires->setStyleSheet("background-color:transparent;border:0px");
    lb_hires->hide();

    QLabel *lb_resolution = new QLabel(widget_track_info);
    lb_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    lb_resolution->hide();

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    QLabel *lb_artist = new QLabel(widget_track_info);
    lb_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");

    int width = 0;
    int left = 0;
    if(explicit_flag == true){
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

        QLabel *lb_explicit = new QLabel(widget_track_info);
        lb_explicit->setPixmap(*img_adult_certification);
        lb_explicit->setFixedSize(30, 30);
        lb_explicit->setGeometry(180, 20, 0, 0);

        if(hires == true){
            QString hires_path = "";

            if(type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 670){
                    title_width_resize = 670 - (15 + 36);
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(230, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    lb_title->setText(title);
                    lb_title->setGeometry(230, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                lb_hires->setPixmap(*img_hires);
                lb_hires->setFixedSize(36, 13);

                left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                lb_hires->setGeometry(left, 18, 0, 0);
                lb_hires->show();
            }
            else if(type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 670){
                        title_width_resize = 670 - (15 + width + 15 + 20);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    lb_hires->setPixmap(*img_hires);
                    lb_hires->setFixedSize(20, 20);

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_hires->setGeometry(left, 14, 0, 0);
                    lb_hires->show();

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15 + 20 + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 16, width, 16);
                    lb_resolution->show();
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, 640, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(type == "QOBUZ"){

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, 640, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                }
            }
            else{
                if(title_width > 640){
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                    if(this->lb_title->text().contains("…")){
                        this->lb_title->setToolTip(title);
                        this->lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(230, 10, 640, 25);
                }
                else{
                    lb_title->setText(title);
                    lb_title->setGeometry(230, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 600){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 600, lb_artist->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(title);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(230, 34, 600, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(230, 34, artist_width, 25);
        }
    }
    else{
        if(hires == true){
            QString hires_path = "";

            if(type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 690){
                    title_width_resize = 690 - (15 + 36);
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(180, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    lb_title->setText(title);
                    lb_title->setGeometry(180, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                lb_hires->setPixmap(*img_hires);
                lb_hires->setFixedSize(36, 13);

                left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                lb_hires->setGeometry(left, 18, 0, 0);
                lb_hires->show();
            }
            else if(type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width + 15 + 20);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    lb_hires->setPixmap(*img_hires);
                    lb_hires->setFixedSize(20, 20);

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_hires->setGeometry(left, 14, 0, 0);
                    lb_hires->show();

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15 + 20 + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 16, width, 16);
                    lb_resolution->show();
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, 690, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(type == "QOBUZ"){

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);

                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, 690, 25);
                    }
                }
            }
            else{
                if(title_width > 690){
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(180, 10, 690, 25);
                }
                else{
                    lb_title->setText(title);
                    lb_title->setGeometry(180, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 650){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 650, lb_artist->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(artist);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(180, 34, 650, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(180, 34, artist_width, 25);
        }
    }

    QLabel *tmp_album = new QLabel();
    tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
    tmp_album->setText(album);

    int album_title_width = 0;
    album_title_width = tmp_album->sizeHint().width();

    QLabel *lb_album = new QLabel(widget_track_info);
    lb_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
    lb_album->setWordWrap(true);
    lb_album->setGeometry(997, 14, 220, 40);

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = album;
        splitToken = tmp_split.split(" ");

        tmp_album->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album->setText(tmp_album_set);

                if(tmp_album->sizeHint().width() >= 220){
                    tmp_album->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album->setText("");
        tmp_album->setText(tmp_album_set);

        album_title_width = tmp_album->sizeHint().width() + 220;

        QString strAlbum = album;
        QLabel *tmpAlbum = new QLabel();
        tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

        int tmpAlbum_width = tmpAlbum->sizeHint().width();

        if(tmpAlbum_width >= 220){
            lb_album->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, lb_album->font()));
            if(lb_album->text().contains("…")){
                lb_album->setToolTip(album);
                lb_album->setToolTipDuration(2000);
            }
        }
        else{
            lb_album->setText(album);
        }
        lb_album->setGeometry(997, 14, 220, 40);
    }
    else{
        lb_album->setText(album);
        lb_album->setGeometry(997, 24, 220, 20);
    }

    QLabel *lb_duration = new QLabel(widget_track_info);
    lb_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    lb_duration->setGeometry(1291, 24, 65, 20);

    if(type == "MUSIC"){
        int tmpDduration = duration / 1000;
        QString str_duration = "";

        if(tmpDduration > 3600){
            str_duration = QDateTime::fromTime_t(tmpDduration).toUTC().toString("hh:mm:ss");
        }
        else{
            str_duration = QDateTime::fromTime_t(tmpDduration).toUTC().toString("mm:ss");
        }

        lb_duration->setText(str_duration);
    }
    else if(type == "BUGS"){
        QString tmpDduration = ProcJsonEasy::getString(trackInfo, "duration");

        lb_duration->setText(tmpDduration);
    }
    else if(type == "APPLE_MUSIC"){
        int tmpDduration = duration / 1000;

        if(duration == 0){
            lb_duration->setText("");
        }
        else if(tmpDduration >= 3600){
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }
    else{
        if(duration == 0){
            lb_duration->setText("");
        }
        else if(duration >= 3600){
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }

    int left_pos = lb_duration->geometry().left() + lb_duration->sizeHint().width() + 110;

    QPixmap *pixmap_list = GSCommon::getUIPixmapImg(":/images/change_list_ico_b.png");
    *pixmap_list = pixmap_list->scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *lb_change_list = new QLabel(widget_track_info);
    lb_change_list->setPixmap(*pixmap_list);
    lb_change_list->setGeometry(left_pos, 10, 50, 50);
    lb_change_list->setStyleSheet("background-color:transparent;");

    QListWidgetItem *item = new QListWidgetItem();
    item->setWhatsThis(QString("TYPE:ADD,IDX:%1").arg(p_index));
    item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
    item->setSizeHint(QSize(90, 70));

    this->listWidget_adding->addItem(item);

    this->listWidget_adding->setItemWidget(item, widget_track_info);
}


void AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item_exist(const QJsonObject &trackInfo, const int &p_index){

    QString type = ProcJsonEasy::getString(trackInfo, "type");
    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    QString artist = ProcJsonEasy::getString(trackInfo, "artist");
    QString album  = ProcJsonEasy::getString(trackInfo, "album");
    QString bitrates = ProcJsonEasy::getString(trackInfo, "bitrates");

    int duration = ProcJsonEasy::getInt(trackInfo, "duration");

    double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

    bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
    bool explicit_flag = ProcJsonEasy::getBool(trackInfo, "explicit");

    QWidget *widget_track_info = new QWidget();
    widget_track_info->setFixedSize(1550, 70);
    widget_track_info->setObjectName("widget_track_info");
    widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                        "#widget_track_info:hover{background-color:#333333;} ");

    QString default_path = "";
    QString type_path = "";
    QString hires_path = "";
    if(type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
        hires_path = ":/images/tidal/mqa_ico.png";
    }
    else if(type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
        hires_path = ":/images/qobuz/hires_ico.png";
    }
    else if(type == "APPLE_MUSIC"){
        default_path = ":/images/apple/apple_def.png";
        type_path = ":/images/rosehome/home_apple.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    QImage img_album;
    QPixmap *pixmap_image = new QPixmap();
    if(img_album.load(default_path)){
        *pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_image = pixmap_image->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_trackImg_existing[p_index] = new QLabel(widget_track_info);
    this->lb_trackImg_existing[p_index]->setFixedSize(60, 60);
    this->lb_trackImg_existing[p_index]->setGeometry(5, 5, 0, 0);
    this->lb_trackImg_existing[p_index]->setStyleSheet("background-color:transparent;");
    this->lb_trackImg_existing[p_index]->setPixmap(*pixmap_image);

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", p_index);
    fileDownloader->setImageURL(thumbnail);
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_existing()));

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
    *pixmap_icon = pixmap_icon->scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *lb_track_type = new QLabel(widget_track_info);
    lb_track_type->setPixmap(*pixmap_icon);
    lb_track_type->setGeometry(68, 15, 40, 40);
    lb_track_type->setStyleSheet("background-color:transparent;");

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QLabel *lb_title = new QLabel(widget_track_info);
    lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");

    QLabel *lb_hires = new QLabel(widget_track_info);
    lb_hires->setStyleSheet("background-color:transparent;border:0px");
    lb_hires->hide();

    QLabel *lb_resolution = new QLabel(widget_track_info);
    lb_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    lb_resolution->hide();

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    QLabel *lb_artist = new QLabel(widget_track_info);
    lb_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");

    int width = 0;
    int left = 0;
    if(explicit_flag == true){
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

        QLabel *lb_explicit = new QLabel(widget_track_info);
        lb_explicit->setPixmap(*img_adult_certification);
        lb_explicit->setFixedSize(30, 30);
        lb_explicit->setGeometry(130, 20, 0, 0);

        if(hires == true){
            QString hires_path = "";

            if(type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 670){
                    title_width_resize = 670 - (15 + 36);
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(180, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    lb_title->setText(title);
                    lb_title->setGeometry(180, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                lb_hires->setPixmap(*img_hires);
                lb_hires->setFixedSize(36, 13);

                left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                lb_hires->setGeometry(left, 18, 0, 0);
                lb_hires->show();
            }
            else if(type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 670){
                        title_width_resize = 670 - (15 + width + 15 + 20);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    lb_hires->setPixmap(*img_hires);
                    lb_hires->setFixedSize(20, 20);

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_hires->setGeometry(left, 14, 0, 0);
                    lb_hires->show();

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15 + 20 + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 16, width, 16);
                    lb_resolution->show();
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, 640, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(type == "QOBUZ"){

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, 640, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
            else{
                if(title_width > 640){
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(180, 10, 640, 25);
                }
                else{
                    lb_title->setText(title);
                    lb_title->setGeometry(180, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 600){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 600, lb_artist->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(artist);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(180, 34, 600, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(180, 34, artist_width, 25);
        }
    }
    else{
        if(hires == true){
            QString hires_path = "";

            if(type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 690){
                    title_width_resize = 690 - (15 + 36);
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(120, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    lb_title->setText(title);
                    lb_title->setGeometry(120, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                lb_hires->setPixmap(*img_hires);
                lb_hires->setFixedSize(36, 13);

                left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                lb_hires->setGeometry(left, 18, 0, 0);
                lb_hires->show();
            }
            else if(type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width + 15 + 20);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    lb_hires->setPixmap(*img_hires);
                    lb_hires->setFixedSize(20, 20);

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_hires->setGeometry(left, 14, 0, 0);
                    lb_hires->show();

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15 + 20 + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 16, width, 16);
                    lb_resolution->show();
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, 690, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(type == "QOBUZ"){

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width, 25);
                    }
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(120, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(120, 10, 690, 25);
                    }
                }
            }
            else{
                if(title_width > 690){
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(120, 10, 690, 25);
                }
                else{
                    lb_title->setText(title);
                    lb_title->setGeometry(120, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 650){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 650, lb_artist->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(artist);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(120, 34, 650, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(120, 34, artist_width, 25);
        }
    }

    QLabel *tmp_album = new QLabel();
    tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
    tmp_album->setText(album);

    int album_title_width = 0;
    album_title_width = tmp_album->sizeHint().width();

    QLabel *lb_album = new QLabel(widget_track_info);
    lb_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
    lb_album->setWordWrap(true);
    lb_album->setGeometry(937, 14, 220, 40);

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = album;
        splitToken = tmp_split.split(" ");

        tmp_album->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album->setText(tmp_album_set);

                if(tmp_album->sizeHint().width() >= 220){
                    tmp_album->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album->setText("");
        tmp_album->setText(tmp_album_set);

        album_title_width = tmp_album->sizeHint().width() + 220;

        QString strAlbum = album;
        QLabel *tmpAlbum = new QLabel();
        tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

        int tmpAlbum_width = tmpAlbum->sizeHint().width();

        if(tmpAlbum_width >= 220){
            lb_album->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, lb_album->font()));
            if(lb_album->text().contains("…")){
                lb_album->setToolTip(album);
                lb_album->setToolTipDuration(2000);
            }
        }
        else{
            lb_album->setText(album);
        }
        lb_album->setGeometry(937, 14, 220, 40);
    }
    else{
        lb_album->setText(album);
        lb_album->setGeometry(937, 24, 220, 20);
    }

    QLabel *lb_duration = new QLabel(widget_track_info);
    lb_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    lb_duration->setGeometry(1231, 24, 65, 20);

    if(type == "BUGS"){
        QString tmpDduration = ProcJsonEasy::getString(trackInfo, "duration");

        lb_duration->setText(tmpDduration);
    }
    else if(type == "APPLE_MUSIC"){
        int tmpDduration = duration / 1000;

        if(duration == 0){
            lb_duration->setText("");
        }
        else if(tmpDduration >= 3600){
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }
    else{
        if(duration == 0){
            lb_duration->setText("");
        }
        else if(duration >= 3600){
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }

    /*int left_pos = lb_duration->geometry().left() + lb_duration->sizeHint().width() + 140;

    QPixmap *pixmap_list = GSCommon::getUIPixmapImg(":/images/change_list_ico_b.png");
    *pixmap_list = pixmap_list->scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *lb_change_list = new QLabel(widget_track_info);
    lb_change_list->setPixmap(*pixmap_list);
    lb_change_list->setGeometry(left_pos, 10, 50, 50);
    lb_change_list->setStyleSheet("background-color:transparent;");*/

    QListWidgetItem *item = new QListWidgetItem();
    item->setWhatsThis(QString("TYPE:EXIST,IDX:%1").arg(p_index));
    //item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
    //item->setFlags(Qt::ItemIsEnabled);
    item->setSizeHint(QSize(90, 70));

    this->listWidget_existing->addItem(item);

    this->listWidget_existing->setItemWidget(item, widget_track_info);
}


void AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item_edit(const QJsonObject &trackInfo, const int &p_index){

    QString type = ProcJsonEasy::getString(trackInfo, "type");
    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    QString artist = ProcJsonEasy::getString(trackInfo, "artist");
    QString album  = ProcJsonEasy::getString(trackInfo, "album");
    QString bitrates = ProcJsonEasy::getString(trackInfo, "bitrates");

    int duration = ProcJsonEasy::getInt(trackInfo, "duration");

    double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

    bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
    bool explicit_flag = ProcJsonEasy::getBool(trackInfo, "explicit");

    QWidget *widget_track_info = new QWidget();
    widget_track_info->setFixedSize(1550, 70);
    widget_track_info->setObjectName("widget_track_info");
    widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                        "#widget_track_info:hover{background-color:#333333;} ");

    this->checkbox_Selected[p_index] = new QCheckBox(widget_track_info);
    this->checkbox_Selected[p_index]->setFixedSize(50, 50);
    this->checkbox_Selected[p_index]->setGeometry(0, 10, 0, 0);
    this->checkbox_Selected[p_index]->setProperty("type", "ADD");
    this->checkbox_Selected[p_index]->setProperty("index", p_index);
    this->checkbox_Selected[p_index]->setStyleSheet("QCheckBox::indicator {width:50px; height:50px;}"
                                                    "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                    "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

    QString default_path = "";
    QString type_path = "";
    QString hires_path = "";
    if(type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
        hires_path = ":/images/tidal/mqa_ico.png";
    }
    else if(type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
        hires_path = ":/images/qobuz/hires_ico.png";
    }
    else if(type == "APPLE_MUSIC"){
        default_path = ":/images/apple/apple_def.png";
        type_path = ":/images/rosehome/home_apple.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    QImage img_album;
    QPixmap *pixmap_image = new QPixmap();
    if(img_album.load(default_path)){
        *pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
        *pixmap_image = pixmap_image->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    this->lb_trackImg_editing[p_index] = new QLabel(widget_track_info);
    this->lb_trackImg_editing[p_index]->setFixedSize(60, 60);
    this->lb_trackImg_editing[p_index]->setGeometry(60, 5, 0, 0);
    this->lb_trackImg_editing[p_index]->setStyleSheet("background-color:transparent;");
    this->lb_trackImg_editing[p_index]->setPixmap(*pixmap_image);

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("index", p_index);
    fileDownloader->setImageURL(thumbnail);
    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_editing()));

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
    *pixmap_icon = pixmap_icon->scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *lb_track_type = new QLabel(widget_track_info);
    lb_track_type->setPixmap(*pixmap_icon);
    lb_track_type->setGeometry(128, 15, 40, 40);
    lb_track_type->setStyleSheet("background-color:transparent;");

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    QLabel *lb_title = new QLabel(widget_track_info);
    lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");

    QLabel *lb_hires = new QLabel(widget_track_info);
    lb_hires->setStyleSheet("background-color:transparent;border:0px");
    lb_hires->hide();

    QLabel *lb_resolution = new QLabel(widget_track_info);
    lb_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    lb_resolution->hide();

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    QLabel *lb_artist = new QLabel(widget_track_info);
    lb_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");

    int width = 0;
    int left = 0;
    if(explicit_flag == true){
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

        QLabel *lb_explicit = new QLabel(widget_track_info);
        lb_explicit->setPixmap(*img_adult_certification);
        lb_explicit->setFixedSize(30, 30);
        lb_explicit->setGeometry(180, 20, 0, 0);

        if(hires == true){
            QString hires_path = "";

            if(type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 670){
                    title_width_resize = 670 - (15 + 36);
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(230, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    lb_title->setText(title);
                    lb_title->setGeometry(230, 10, title_width_resize, 25);
                }

                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                lb_hires->setPixmap(*img_hires);
                lb_hires->setFixedSize(36, 13);

                left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                lb_hires->setGeometry(left, 18, 0, 0);
                lb_hires->show();
            }
            else if(type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 670){
                        title_width_resize = 670 - (15 + width + 15 + 20);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    lb_hires->setPixmap(*img_hires);
                    lb_hires->setFixedSize(20, 20);

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_hires->setGeometry(left, 14, 0, 0);
                    lb_hires->show();

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15 + 20 + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 16, width, 16);
                    lb_resolution->show();
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, 640, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(type == "QOBUZ"){

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 640){
                        title_width_resize = 640 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 640){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(230, 10, 640, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(230, 10, title_width, 25);
                    }
                }
            }
            else{
                if(title_width > 640){
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 640, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(230, 10, 640, 25);
                }
                else{
                    lb_title->setText(title);
                    lb_title->setGeometry(230, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 600){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 600, lb_artist->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(artist);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(230, 34, 600, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(230, 34, artist_width, 25);
        }
    }
    else{
        if(hires == true){
            QString hires_path = "";

            if(type == "TIDAL"){

                hires_path = ":/images/tidal/mqa_ico.png";

                title_width += (15 + 36);

                int title_width_resize = 0;
                if(title_width > 690){
                    title_width_resize = 690 - (15 + 36);
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(180, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + 36);
                    lb_title->setText(title);
                    lb_title->setGeometry(180, 10, title_width_resize, 25);
                }
                QImage img;
                QPixmap *img_hires = new QPixmap();
                if(img.load(hires_path)){
                    *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                    *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                }

                lb_hires->setPixmap(*img_hires);
                lb_hires->setFixedSize(36, 13);

                left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                lb_hires->setGeometry(left, 18, 0, 0);
                lb_hires->show();
            }
            else if(type == "QOBUZ"){

                hires_path = ":/images/qobuz/hires_ico.png";

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width + 15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width + 15 + 20);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width + 15 + 20);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    QImage img;
                    QPixmap *img_hires = new QPixmap();
                    if(img.load(hires_path)){
                        *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                        *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                    }

                    lb_hires->setPixmap(*img_hires);
                    lb_hires->setFixedSize(20, 20);

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_hires->setGeometry(left, 14, 0, 0);
                    lb_hires->show();

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15 + 20 + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    title_width += (15 + 20);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 16, width, 16);
                    lb_resolution->show();
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, 690, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
        }
        else{
            if(type == "QOBUZ"){

                if(sampling > 0){
                    lb_resolution->setText(QString("%1 kHz").arg(sampling));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                }
            }
            else if(type == "BUGS"){

                if(!bitrates.isEmpty()){
                    lb_resolution->setText(QString("%1").arg(bitrates));
                    lb_resolution->setAlignment(Qt::AlignCenter);

                    width = lb_resolution->sizeHint().width() + 20;

                    title_width += (15 + width);

                    int title_width_resize = 0;
                    if(title_width > 690){
                        title_width_resize = 690 - (15 + width);
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }
                    else{
                        title_width_resize = title_width - (15 + width);
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, title_width_resize, 25);
                    }

                    left = lb_title->geometry().left() + lb_title->geometry().width() + 15;

                    lb_resolution->setGeometry(left, 15, width, 16);
                    lb_resolution->show();
                }
                else{
                    if(title_width > 690){
                        lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                        if(lb_title->text().contains("…")){
                            lb_title->setToolTip(title);
                            lb_title->setToolTipDuration(2000);
                        }
                        lb_title->setGeometry(180, 10, title_width, 25);
                    }
                    else{
                        lb_title->setText(title);
                        lb_title->setGeometry(180, 10, 690, 25);
                    }
                }
            }
            else{
                if(title_width > 690){
                    lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 690, lb_title->font()));
                    if(lb_title->text().contains("…")){
                        lb_title->setToolTip(title);
                        lb_title->setToolTipDuration(2000);
                    }
                    lb_title->setGeometry(180, 10, 690, 25);
                }
                else{
                    lb_title->setText(title);
                    lb_title->setGeometry(180, 10, title_width, 25);
                }
            }
        }

        if(artist_width > 650){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 650, lb_artist->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(artist);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(180, 34, 650, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(180, 34, artist_width, 25);
        }
    }

    QLabel *tmp_album = new QLabel();
    tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
    tmp_album->setText(album);

    int album_title_width = 0;
    album_title_width = tmp_album->sizeHint().width();

    QLabel *lb_album = new QLabel(widget_track_info);
    lb_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
    lb_album->setWordWrap(true);
    lb_album->setGeometry(997, 14, 220, 40);

    if(album_title_width > 220){
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = album;
        splitToken = tmp_split.split(" ");

        tmp_album->setText("");
        int i = 0;
        if(splitToken.size() > 1){

            for(i = 0; i < splitToken.count(); i++){
                if(i == 0){
                    tmp_album_set = splitToken.at(i);
                }
                else{
                    tmp_album_set += " " + splitToken.at(i);
                }
                tmp_album->setText(tmp_album_set);

                if(tmp_album->sizeHint().width() >= 220){
                    tmp_album->setText("");
                    tmp_album_set.replace(splitToken.at(i), "");
                    break;
                }
            }
        }

        tmp_album->setText("");
        tmp_album->setText(tmp_album_set);

        album_title_width = tmp_album->sizeHint().width() + 220;

        QString strAlbum = album;
        QLabel *tmpAlbum = new QLabel();
        tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

        int tmpAlbum_width = tmpAlbum->sizeHint().width();

        if(tmpAlbum_width >= 220){
            lb_album->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, lb_album->font()));
            if(lb_album->text().contains("…")){
                lb_album->setToolTip(album);
                lb_album->setToolTipDuration(2000);
            }
        }
        else{
            lb_album->setText(album);
        }
        lb_album->setGeometry(997, 14, 220, 40);
    }
    else{
        lb_album->setText(album);
        lb_album->setGeometry(997, 24, 220, 20);
    }

    QLabel *lb_duration = new QLabel(widget_track_info);
    lb_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    lb_duration->setGeometry(1291, 24, 65, 20);

    if(type == "BUGS"){
        QString tmpDduration = ProcJsonEasy::getString(trackInfo, "duration");

        lb_duration->setText(tmpDduration);
    }
    else if(type == "APPLE_MUSIC"){
        int tmpDduration = duration / 1000;

        if(duration == 0){
            lb_duration->setText("");
        }
        else if(tmpDduration >= 3600){
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }
    else{
        if(duration == 0){
            lb_duration->setText("");
        }
        else if(duration >= 3600){
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            lb_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }
    }

    //int left_pos = lb_duration->geometry().left() + lb_duration->sizeHint().width() + 110;

    QPixmap *pixmap_list = GSCommon::getUIPixmapImg(":/images/change_list_ico_b.png");
    *pixmap_list = pixmap_list->scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *lb_change_list = new QLabel(widget_track_info);
    lb_change_list->setPixmap(*pixmap_list);
    lb_change_list->setGeometry(1410, 10, 50, 50);
    lb_change_list->setStyleSheet("background-color:transparent;");

    int left_pos = lb_change_list->geometry().left() - lb_change_list->sizeHint().width();

    QPushButton *btn_change_delete = new QPushButton(widget_track_info);
    btn_change_delete->setProperty("type", "editing");
    btn_change_delete->setProperty("idx", p_index);
    btn_change_delete->setCursor(Qt::PointingHandCursor);
    btn_change_delete->setGeometry(left_pos, 10, 50, 50);
    btn_change_delete->setStyleSheet("background-color:transparent; background-image: url(:/images/playlist/fol_del_ico.png); background-repeat:no-repeat; background-position:center center;");

    connect(btn_change_delete, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnDelete);

    QListWidgetItem *item = new QListWidgetItem();
    item->setWhatsThis(QString("TYPE:EDIT,IDX:%1").arg(p_index));
    item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
    item->setSizeHint(QSize(90, 70));

    this->listWidget_editing->addItem(item);

    this->listWidget_editing->setItemWidget(item, widget_track_info);
}


void AbstractAddPlaylist_RHV::slot_changedSubTabUI(const QJsonObject &p_data){

    QString tmp_step = p_data[KEY_PAGE_CODE].toString();

    this->contentStep = tmp_step;

    // Tab 타이틀 변경
    this->menubar->setSelectedSubMenuNoSignal(contentStep);

    int height = 0;
    if(this->contentStep == ALBTAB_STEP_ADDING){
        this->btn_delete->show();
        this->btn_delete->setEnabled(true);//c0217
        if(this->added_array.size() < 4){
            height = 210;
        }
        else{
            //height = 70 * this->added_array.size();
            height = 70 * this->create_count;
        }

        this->stackedwidget->setCurrentIndex(0);
        this->stackedwidget->setFixedHeight(height);
    }
    else if(this->contentStep == ALBTAB_STEP_EXISTING){
        this->btn_delete->hide();
        //this->btn_del->setEnabled(false);//c0217
        if(this->existing_array.size() < 4){
            height = 210;
        }
        else{
            //height = 70 * this->existing_array.count();
            height = 70 * this->track_count;
        }

        this->stackedwidget->setCurrentIndex(1);
        this->stackedwidget->setFixedHeight(height);
    }
}


void AbstractAddPlaylist_RHV::request_more_trackData(){

    if(!this->flagReqMore_track && !this->flag_lastPage_track){

        if(this->track_drawCnt == 0){
            this->pageCNT = 0;
        }
        else{
            this->pageCNT++;
        }

        roseHome::ProcCommon *proc_playlist = new roseHome::ProcCommon(this);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_playlist, this, &AbstractAddPlaylist_RHV::slot_applyResult_playlist);
        connect(proc_playlist, &roseHome::ProcCommon::completeReq_list_tracks, this, &AbstractAddPlaylist_RHV::slot_applyResult_playlist_tracks);
        proc_playlist->request_rose_get_playlist(this->playlist_id, this->pageCNT, GET_MAX_ITEM_SIZE___ONCE);
    }
}


void AbstractAddPlaylist_RHV::slot_applyResult_playlist(const roseHome::PlaylistItemData &playlist_data){

    if(this->flag_playlist_info == false){
        this->track_count = playlist_data.trackCount;
        this->shared_type = playlist_data.share;

        QJsonArray tags;
        for(int i = 0; i < playlist_data.tags.count(); i++){
            tags.append(playlist_data.tags.at(i));
        }

        this->playlist_info = QJsonObject();
        this->playlist_info.insert("clientKey", playlist_data.clientKey);
        this->playlist_info.insert("comment", playlist_data.comment);
        this->playlist_info.insert("duration", playlist_data.duration);
        this->playlist_info.insert("favorite", playlist_data.favorite);
        this->playlist_info.insert("favoriteTrack", playlist_data.favoriteTrack);
        this->playlist_info.insert("id", playlist_data.id);
        this->playlist_info.insert("isRose", playlist_data.isRose);
        this->playlist_info.insert("macAddress", playlist_data.macAddress);
        this->playlist_info.insert("ownerId", playlist_data.ownerId);
        this->playlist_info.insert("ownerName", playlist_data.ownerName);
        this->playlist_info.insert("playTime", playlist_data.playTime);
        this->playlist_info.insert("registDateTime", playlist_data.registDateTime);
        this->playlist_info.insert("share", playlist_data.share);
        this->playlist_info.insert("sort", playlist_data.sort);
        this->playlist_info.insert("star", playlist_data.star);
        this->playlist_info.insert("tags", tags);
        this->playlist_info.insert("thumbnail", playlist_data.thumbnail);
        this->playlist_info.insert("thumbup", playlist_data.thumbup);
        this->playlist_info.insert("thumbupCount", playlist_data.thumbupCount);
        this->playlist_info.insert("title", playlist_data.title);
        this->playlist_info.insert("track_count", playlist_data.trackCount);
        this->playlist_info.insert("trackId", playlist_data.trackId);
        this->playlist_info.insert("type", playlist_data.type);

        this->shared_type = playlist_data.share.toUpper();
        this->playlist_type = playlist_data.type;

        if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){

        }
        else if(this->contents_ui_type == ContentsUIType::Editing){
            int height = this->track_count * 70;
            this->listWidget_editing->setFixedHeight(height);

            this->setImage(playlist_data.thumbnail);

            QString tmpTitle = playlist_data.title;
            this->le_title->setText(tmpTitle);
            this->lb_title->setText(QString("%1/50 ").arg(tmpTitle.size()) + tr("Charaters"));

            QString tmpContent = playlist_data.comment;
            this->te_content->setText(tmpContent);
            this->lb_content->setText(QString("%1/500 ").arg(tmpContent.size()) + tr("Charaters"));

            if(this->contents_type == ContentsType::Music || this->playlist_type == "MUSIC"){
                this->btn_all->setEnabled(false);
            }

            if(this->shared_type == "ALL"){
                this->flag_all_enable = true;
                this->flag_freind_enable = false;
                this->flag_private_enable = false;

                this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
            }
            else if(this->shared_type == "FRIEND"){
                this->flag_all_enable = false;
                this->flag_freind_enable = true;
                this->flag_private_enable = false;

                this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
            }
            else{
                this->flag_all_enable = false;
                this->flag_freind_enable = false;
                this->flag_private_enable = true;

                this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
            }

            QString tags = "";
            for(int i = 0; i < playlist_data.tags.size(); i++){
                if(!playlist_data.tags.at(i).isEmpty()){
                    if(i == 0){
                        tags = "#" + playlist_data.tags.at(i);
                    }
                    else{
                        tags += ",#" + playlist_data.tags.at(i);
                    }
                }
            }

            this->le_tag->setText(tags);
        }

        this->flag_playlist_info = true;
    }
}


void AbstractAddPlaylist_RHV::slot_applyResult_playlist_tracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

    Q_UNUSED(list_data);

    this->flag_lastPage_track = flag_lastPage;
    this->flagReqMore_track = false;

    if(this->pageCNT == 0){
        this->track_drawCnt = 0;

        if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){
            this->existing_array = QJsonArray();
        }
        else if(this->contents_ui_type == ContentsUIType::Editing){
            this->editing_array = QJsonArray();
        }
    }

    int max_count = 0;
    if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){
        ProcJsonEasy::mergeJsonArray(this->existing_array, jsonArr_dataToPlay);
        max_count = this->existing_array.size();
    }
    else if(this->contents_ui_type == ContentsUIType::Editing){
        ProcJsonEasy::mergeJsonArray(this->editing_array, jsonArr_dataToPlay);
        max_count = this->editing_array.size();
    }

    QJsonArray tmpData_arr = QJsonArray();
    for(int i = this->track_drawCnt; i < max_count; i++){
        QJsonObject tmpObj;
        if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){
            tmpObj = this->existing_array.at(i).toObject();
        }
        else if(this->contents_ui_type == ContentsUIType::Editing){
            tmpObj = this->editing_array.at(i).toObject();
        }
        QJsonObject convertData = ProcJsonEasy::getJsonObject(tmpObj, "data");
        QJsonObject tmpData = QJsonObject();

        QString convertType = ProcJsonEasy::getString(tmpObj, "type");

        if(convertType == "MUSIC"){

            QString thumbnail = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");

            QString artist = "";
            if(tmpObj.contains("artists")){
                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(tmpObj, "artists");

                for(int i = 0; i < tmpArtists.count(); i++){

                    if(i == 0){
                        artist = tmpArtists.at(i).toString();
                    }
                    else{
                        artist += "," + tmpArtists.at(i).toString();
                    }
                }
            }

            tmpData.insert("thumbnail", thumbnail);
            tmpData.insert("title", ProcJsonEasy::getString(tmpObj, "title"));
            tmpData.insert("artist", artist);
            tmpData.insert("album", "");
            tmpData.insert("duration", ProcJsonEasy::getInt(tmpObj, "duration"));
            tmpData.insert("explicit", false);
            tmpData.insert("type", "MUSIC");

            tmpData_arr.append(tmpData);
        }
        else if(convertType == "YOUTUBE"){

            QString artist = "";
            if(tmpObj.contains("snippet")){
                QJsonObject snippet_obj = ProcJsonEasy::getJsonObject(tmpObj, "snippet");

                artist = ProcJsonEasy::getString(snippet_obj, "channelTitle");
            }
            else{
                if(tmpObj.contains("artists")){
                    QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(tmpObj, "artists");

                    for(int i = 0; i < tmpArtists.size(); i++){
                        if(i == 0){
                            artist = tmpArtists.at(i).toString();
                        }
                        else{
                            artist += "," + tmpArtists.at(i).toString();
                        }
                    }
                }
            }

            tmpData.insert("thumbnail", ProcJsonEasy::getString(tmpObj, "thumbnailUrl"));
            tmpData.insert("title", ProcJsonEasy::getString(tmpObj, "title"));
            tmpData.insert("artist", artist);
            tmpData.insert("duration", ProcJsonEasy::getInt(tmpObj, "duration"));
            tmpData.insert("explicit", false);
            tmpData.insert("type", "YOUTUBE");

            tmpData_arr.append(tmpData);
        }
        else if(convertType == "TIDAL"){

            QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");

            QString imagePath = ProcJsonEasy::getString(tmpAlbum, "cover");
            QString thumbnail = "";

            if(imagePath.isEmpty()){
                thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(convertData, "imageId"), 750);
            }
            else{
                thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(tmpAlbum, "cover"), 750);
            }

            QString artist = "";
            if(convertData.contains("artist")){
                QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                artist = ProcJsonEasy::getString(tmpArtist, "name");
            }

            tmpData.insert("thumbnail", thumbnail);
            tmpData.insert("title", ProcJsonEasy::getString(convertData, "title"));
            tmpData.insert("artist", artist);
            tmpData.insert("album", ProcJsonEasy::getString(tmpAlbum, "title"));
            tmpData.insert("duration", ProcJsonEasy::getInt(convertData, "duration"));
            if(ProcJsonEasy::getString(convertData, "audioQuality") == "HI_RES"){
                tmpData.insert("hires", true);
            }
            else{
                tmpData.insert("hires", false);
            }
            tmpData.insert("maximum_sampling_rate", 0);
            tmpData.insert("explicit", ProcJsonEasy::getBool(convertData, "explicit"));
            tmpData.insert("type", "TIDAL");

            this->vtrack_id = ProcJsonEasy::getVariant(convertData, "id");

            tmpData_arr.append(tmpData);
        }
        else if(convertType == "BUGS"){

            QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");
            QJsonObject albumImg = ProcJsonEasy::getJsonObject(tmpAlbum, "image");

            QString url = ProcJsonEasy::getString(albumImg, "url");
            QString path = ProcJsonEasy::getString(albumImg, "path");
            QString thumbnail = url.replace("https://", "http://");
            thumbnail += QString("%1").arg(static_cast<int>(bugs::ConvertData_forBugs::ImageSize_Square::Size_350x350)) + path;

            if(i == 0){
                this->setImage(thumbnail);
            }

            QString artist = "";
            if(convertData.contains("artists")){
                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(convertData, "artists");

                for(int i = 0; i < tmpArtists.size(); i++){
                    QJsonObject tmpArtistInfo = tmpArtists.at(i).toObject();
                    if(i == 0){
                        artist = ProcJsonEasy::getString(tmpArtistInfo, "artist_nm");
                    }
                    else{
                        artist += "," + ProcJsonEasy::getString(tmpArtistInfo, "artist_nm");
                    }
                }
            }

            QString bitrate = "";
            QJsonArray bitrates1 = ProcJsonEasy::getJsonArray(convertData, "bitrates");//cheon210605

            if(bitrates1.contains("flac") == true){
                bitrate = "FLAC";
            }
            else if(bitrates1.contains("aac320") == true){
                bitrate = "AAC320";
            }
            else if(bitrates1.contains("320k") == true || bitrates1.contains("mp3") == true){
                bitrate = "MP3";
            }
            else if(bitrates1.contains("aac") == true){
                bitrate = "AAC";
            }
            else if(bitrates1.contains("fullhd") == true){
                bitrate = "FULL HD";
            }
            else if(bitrates1.contains("hd") == true){
                bitrate = "HD";
            }
            else if(bitrates1.contains("sd") == true){
                bitrate = "SD";
            }

            tmpData.insert("thumbnail", thumbnail);
            tmpData.insert("title", ProcJsonEasy::getString(convertData, "track_title"));
            tmpData.insert("artist", artist);
            tmpData.insert("album", ProcJsonEasy::getString(tmpAlbum, "title"));
            tmpData.insert("duration", ProcJsonEasy::getString(convertData, "len"));
            tmpData.insert("bitrates", bitrate);
            tmpData.insert("explicit", ProcJsonEasy::getBool(convertData, "adult_yn"));
            tmpData.insert("type", "BUGS");

            tmpData_arr.append(tmpData);
        }
        else if(convertType == "QOBUZ"){

            QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");
            QJsonObject tmpImage = ProcJsonEasy::getJsonObject(tmpAlbum, "image");
            tmpData.insert("thumbnail", ProcJsonEasy::getString(tmpImage, "thumbnail"));

            QString artist = "";
            if(convertData.contains("artist")){
                QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                artist = ProcJsonEasy::getString(tmpArtist, "name");
            }
            else{
                if(tmpAlbum.contains("artist")){
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(tmpAlbum, "artist");
                    artist = ProcJsonEasy::getString(tmpArtist, "name");
                }
                else{
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "composer");
                    artist = ProcJsonEasy::getString(tmpArtist, "name");
                }
            }

            tmpData.insert("type", "QOBUZ");
            tmpData.insert("title", ProcJsonEasy::getString(convertData, "title"));
            tmpData.insert("artist", artist);
            tmpData.insert("album", ProcJsonEasy::getString(tmpAlbum, "title"));
            tmpData.insert("duration", ProcJsonEasy::getInt(convertData, "duration"));
            tmpData.insert("hires", ProcJsonEasy::getBool(convertData, "hires"));
            tmpData.insert("maximum_sampling_rate", ProcJsonEasy::getDouble(convertData, "maximum_sampling_rate"));
            tmpData.insert("explicit", ProcJsonEasy::getBool(convertData, "parental_warning"));

            this->vtrack_id = ProcJsonEasy::getVariant(convertData, "id");

            tmpData_arr.append(tmpData);
        }
    }

    for(int i = this->track_drawCnt; i < max_count; i++){
        //qDebug() << "idx : " << this->track_drawCnt << " total count : " <<  this->existing_array.count() << "real idx" << i - this->track_drawCnt;
        QJsonObject trackData = tmpData_arr.at(i - this->track_drawCnt).toObject();
        if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){
            this->setDataTrackInfo_RoseListEdit_Item_exist(trackData, i);
        }
        else if(this->contents_ui_type == ContentsUIType::Editing){
            this->setDataTrackInfo_RoseListEdit_Item_edit(trackData, i);
        }

    }

    this->track_drawCnt += tmpData_arr.count();

    this->request_more_trackData();
}


void AbstractAddPlaylist_RHV::setChange_title_text(const QString &title){

    QString tmp_title = title;

    if(title.length() > 50){
        tmp_title = tmp_title.chopped(1);
        this->le_title->setText(tmp_title);

        ToastMsg::show(this, "", tr("The title can be up to 50 characters in length."));
    }

    QString tmpText = QString("%1/50" + tr("Charaters")).arg(tmp_title.length());
    this->lb_title->setText(tmpText);
}


void AbstractAddPlaylist_RHV::setChange_content_text(){

    QString tmp_title = this->te_content->toPlainText();

    if(this->te_content->toPlainText().size() > 500){

        tmp_title = tmp_title.chopped(1);
        this->te_content->setText(tmp_title);

        ToastMsg::show(this, "", tr("The comment can be up to 500 characters."));
    }

    QString tmpText = QString("%1/500" + tr("Charaters")).arg(tmp_title.length());
    this->lb_content->setText(tmpText);
}


void AbstractAddPlaylist_RHV::slot_btnPublic_clicked(){

    QString obj_name = sender()->objectName();

    if((this->contents_type == ContentsType::Music  || this->playlist_type == "MUSIC") && obj_name == "btnAll"){

    }
    else{
        this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_on.png", "icon_che_off.png"));
        this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_on.png", "icon_che_off.png"));
        this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_on.png", "icon_che_off.png"));

        this->flag_all_enable = false;
        this->flag_freind_enable = false;
        this->flag_private_enable = false;

        if(obj_name == "btnAll"){
            this->flag_all_enable = true;
            this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
        }
        else if(obj_name == "btnFreind"){
            this->flag_freind_enable = true;
            this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
        }
        else if(obj_name == "btnPrivate"){
            this->flag_private_enable = true;
            this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
        }
    }
}

void AbstractAddPlaylist_RHV::slot_btnTags_pressed(){

    int idx = sender()->property("idx").toInt();

    this->btn_tag[idx]->setStyleSheet("background-color:#BE905F;color:#FFF;font-size:16px;font-weight:300;border:1px solid #BE905F;border-radius:15px;");

    QString tag_find = this->le_tag->text();
    if(!tag_find.contains(this->tagTypeList[idx])){
        if(tag_find.isEmpty()){
            tag_find += this->tagTypeList[idx];
            this->le_tag->setText(tag_find);
        }
        else{
            tag_find = tag_find + ", " + this->tagTypeList[idx];
            this->le_tag->setText(tag_find);
        }
    }
}


void AbstractAddPlaylist_RHV::slot_btnTags_clicked(){

    int idx = sender()->property("idx").toInt();

    this->btn_tag[idx]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
}


void AbstractAddPlaylist_RHV::slot_btnEditConfirm_clicked(){

    QJsonArray tags;
    QString str_tags = this->le_tag->text().replace("#", "").replace(" ", "");

    QStringList strList = str_tags.split(",");
    for(int i = 0 ; i < strList.count(); i++){
        tags.append(strList.at(i));
    }

    QString share = "PRIVATE";
    if(this->flag_all_enable == true){
        share = "ALL";
    }
    else if(this->flag_freind_enable == true){
        share = "FRIEND";
    }
    else if(this->flag_private_enable == true){
        share = "PRIVATE";
    }

    QJsonObject playlist;
    playlist.insert("clientKey", ProcJsonEasy::getString(this->playlist_info, "clientKey"));
    playlist.insert("comment", this->te_content->toPlainText());
    playlist.insert("favorite", ProcJsonEasy::getBool(this->playlist_info, "favorite"));
    playlist.insert("favoriteTrack", ProcJsonEasy::getBool(this->playlist_info, "favoriteTrack"));
    playlist.insert("id", ProcJsonEasy::getInt(this->playlist_info, "id"));
    playlist.insert("isRose", true);
    playlist.insert("ownerId", ProcJsonEasy::getInt(this->playlist_info, "ownerId"));
    playlist.insert("share", share);
    playlist.insert("star", ProcJsonEasy::getInt(this->playlist_info, "star"));
    playlist.insert("tags", tags);
    playlist.insert("thumbup", ProcJsonEasy::getBool(this->playlist_info, "thumbup"));
    playlist.insert("thumbupCount", ProcJsonEasy::getInt(this->playlist_info, "thumbupCount"));
    playlist.insert("title", this->le_title->text());
    playlist.insert("trackCount", 0);
    playlist.insert("type", ProcJsonEasy::getString(this->playlist_info, "type"));

    QJsonObject json;
    json.insert("playlist", playlist);

    roseHome::ProcCommon *procRose = new roseHome::ProcCommon(this);
    connect(procRose, &roseHome::ProcCommon::completeReq_editInfo_playlist, this, &AbstractAddPlaylist_RHV::slot_editInfo_rosePlaylist);
    procRose->request_rose_editInfo_myPlaylist(json);
}


void AbstractAddPlaylist_RHV::slot_clickedDelete(){

    if(this->contentStep == ALBTAB_STEP_ADDING){

        if(this->tracksDelete_DialogConfirm()){
            int startIdx = this->listWidget_adding->count() - 1;

            for(int i = startIdx; i >= 0; i--){

                QString splitIndex = this->listWidget_adding->item(i)->whatsThis();
                QStringList listIndex = splitIndex.split(",");
                QStringList strIndex = listIndex.at(1).split(":");
                int idx = strIndex.at(1).toInt();

                if(this->checkbox_Selected[idx]->checkState() == 2){
                    delete this->listWidget_adding->takeItem(i);
                }
            }

            int height = 70 * this->listWidget_adding->count();
            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(height);
        }
    }
    else if(this->contentStep == ALBTAB_STEP_EXISTING){

    }
    else if(this->contentStep == ALBTAB_STEP_EDITING){

        if(this->tracksDelete_DialogConfirm()){
            int startIdx = this->listWidget_editing->count() - 1;

            for(int i = startIdx; i >= 0; i--){

                QString splitIndex = this->listWidget_editing->item(i)->whatsThis();
                QStringList listIndex = splitIndex.split(",");
                QStringList strIndex = listIndex.at(1).split(":");
                int idx = strIndex.at(1).toInt();

                if(this->checkbox_Selected[idx]->checkState() == 2){
                    delete this->listWidget_editing->takeItem(i);
                }
            }

            int height = 70 * this->listWidget_editing->count();
            this->listWidget_editing->setFixedHeight(height);
        }
    }
}


void AbstractAddPlaylist_RHV::slot_clickedSelected(int state){

    if(state == Qt::Checked){
        for(int i = 0; i < this->listWidget_editing->count(); i++){
            QString splitIndex = this->listWidget_editing->item(i)->whatsThis();
            QStringList listIndex = splitIndex.split(",");
            QStringList strIndex = listIndex.at(1).split(":");
            int idx = strIndex.at(1).toInt();

            this->checkbox_Selected[idx]->setCheckState(Qt::Checked);
        }
    }
    else{
        for(int i = 0; i < this->listWidget_editing->count(); i++){
            QString splitIndex = this->listWidget_editing->item(i)->whatsThis();
            QStringList listIndex = splitIndex.split(",");
            QStringList strIndex = listIndex.at(1).split(":");
            int idx = strIndex.at(1).toInt();

            this->checkbox_Selected[idx]->setCheckState(Qt::Unchecked);
        }
    }
}


void AbstractAddPlaylist_RHV::slot_clicked_btnSelectAll(){

    if(this->checkbox_selectAll->checkState() == Qt::Checked){
        this->checkbox_selectAll->setCheckState(Qt::Unchecked);
    }
    else if(this->checkbox_selectAll->checkState() == Qt::Unchecked){
        this->checkbox_selectAll->setCheckState(Qt::Checked);
    }
}


void AbstractAddPlaylist_RHV::slot_clicked_btnDelete(){

    QString selectType = sender()->property("type").toString();
    int selectIdx = sender()->property("idx").toInt();

    if(selectType == "adding"){

        if(this->tracksDelete_DialogConfirm()){
            int startIdx = this->listWidget_adding->count() - 1;

            for(int i = startIdx; i >= 0; i--){

                QString splitIndex = this->listWidget_adding->item(i)->whatsThis();
                QStringList listIndex = splitIndex.split(",");
                QStringList strIndex = listIndex.at(1).split(":");
                int idx = strIndex.at(1).toInt();

                if(selectIdx == idx){
                    delete this->listWidget_adding->takeItem(i);
                }
            }

            int height = 70 * this->listWidget_adding->count();
            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(height);
        }
    }
    else if(selectType == "existing"){

    }
    else if(selectType == "editing"){

        if(this->tracksDelete_DialogConfirm()){
            int startIdx = this->listWidget_editing->count() - 1;

            for(int i = startIdx; i >= 0; i--){

                QString splitIndex = this->listWidget_editing->item(i)->whatsThis();
                QStringList listIndex = splitIndex.split(",");
                QStringList strIndex = listIndex.at(1).split(":");
                int idx = strIndex.at(1).toInt();

                if(selectIdx == idx){
                    delete this->listWidget_editing->takeItem(i);
                }
            }

            int height = 70 * this->listWidget_editing->count();
            this->listWidget_editing->setFixedHeight(height);
        }
    }
}


void AbstractAddPlaylist_RHV::slot_clicked_btnStream(){

    if(this->flag_stream_enable == false){
        this->flag_stream_enable = true;

        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_on.png");
        this->lb_listCheck_icon->setPixmap(*pixmap_icon);
        this->lb_listCheck_icon->resize(pixmap_icon->width(), pixmap_icon->height());

        this->btn_selectList->setEnabled(this->flag_stream_enable);
    }
    else if(this->flag_stream_enable == true){
        this->flag_stream_enable = false;

        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_off.png");
        this->lb_listCheck_icon->setPixmap(*pixmap_icon);
        this->lb_listCheck_icon->resize(pixmap_icon->width(), pixmap_icon->height());

        this->btn_selectList->setEnabled(this->flag_stream_enable);
    }

}


void AbstractAddPlaylist_RHV::slot_clicked_btnSelect(){

    if(this->flag_stream_enable == true){

        if(contents_type == ContentsType::Qobuz){
            qobuz::Dialog_ChoosePlaylist_forQobuz *dialog_chooslPlaylist = new qobuz::Dialog_ChoosePlaylist_forQobuz(this);
            connect(dialog_chooslPlaylist, &qobuz::Dialog_ChoosePlaylist_forQobuz::signal_choosePlaylist_forQOBUZ, this, &AbstractAddPlaylist_RHV::slot_selected_playlist);

            int result = dialog_chooslPlaylist->exec();

            if(result == QDialog::Accepted){
                delete dialog_chooslPlaylist;
            }
        }
        else if(contents_type == ContentsType::Tidal){
            tidal::Dialog_ChoosePlaylist_forTidal *dialog_chooslPlaylist = new tidal::Dialog_ChoosePlaylist_forTidal(this);
            connect(dialog_chooslPlaylist, &tidal::Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forTIDAL, this, &AbstractAddPlaylist_RHV::slot_selected_playlist);

            int result = dialog_chooslPlaylist->exec();

            if(result == QDialog::Accepted){
                delete dialog_chooslPlaylist;
            }
        }
    }
}


void AbstractAddPlaylist_RHV::slot_selected_playlist(QString playlist_id_choose, QString playlist_title, QString playlist_description){

    if(playlist_id_choose != 0){
        this->selected_playlist_id = playlist_id_choose;
        this->selected_playlist_name = playlist_title;
        this->selected_playlist_description = playlist_description;

        if(!this->selected_playlist_name.isEmpty()){
            QLabel *tmp_name = new QLabel();
            tmp_name->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            tmp_name->setText(this->selected_playlist_name);

            if(tmp_name->geometry().width() > 190){
                this->lb_selectList->setText(GSCommon::getTextCutFromLabelWidth(this->selected_playlist_name, 180, this->lb_selectList->font()));
                if(this->lb_selectList->text().contains("…")){
                    this->lb_selectList->setToolTip(this->selected_playlist_name);
                    this->lb_selectList->setToolTipDuration(2000);
                }
            }
            else{
                this->lb_selectList->setText(this->selected_playlist_name);
            }
        }
    }
    else{
        this->selected_playlist_name = playlist_title;
        this->selected_playlist_description = playlist_description;

        if(!this->selected_playlist_name.isEmpty()){
            QLabel *tmp_name = new QLabel();
            tmp_name->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            tmp_name->setText(this->selected_playlist_name);

            if(tmp_name->geometry().width() > 190){
                this->lb_selectList->setText(GSCommon::getTextCutFromLabelWidth(this->selected_playlist_name, 180, this->lb_selectList->font()));
                if(this->lb_selectList->text().contains("…")){
                    this->lb_selectList->setToolTip(this->selected_playlist_name);
                    this->lb_selectList->setToolTipDuration(2000);
                }
            }
            else{
                this->lb_selectList->setText(this->selected_playlist_name);
            }

            if(contents_type == ContentsType::Qobuz){

                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                connect(procQobuz, &qobuz::ProcCommon::completeReq_create_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_create_qobuzPlaylist);
                procQobuz->request_qobuz_create_myPlaylist(this->selected_playlist_name, this->selected_playlist_description);
            }
            else if(contents_type == ContentsType::Tidal){

                tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
                connect(procTidal, &tidal::ProcCommon::completeReq_create_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_create_tidalPlaylist);
                procTidal->request_tidal_create_myPlaylist(global.user_forTidal.getUserId(), this->selected_playlist_name);
            }
        }
    }
}


void AbstractAddPlaylist_RHV::slot_create_qobuzPlaylist(const int playlist_id){

    if(playlist_id > 0){
        this->selected_playlist_id = QString("%1").number(playlist_id);
    }
}


void AbstractAddPlaylist_RHV::slot_create_tidalPlaylist(const QString uuid){

    if(!uuid.isEmpty()){
        this->selected_playlist_id = uuid;
    }
}


void AbstractAddPlaylist_RHV::slot_addTracks_qobuzPlaylist(const int tracks_count){

    if(tracks_count > 0){
        this->flag_addTracks_qobuz = true;
    }

    if(this->flag_addTracks_rose == true){
        emit linker->signal_addPlaylistFinish();
    }
}


void AbstractAddPlaylist_RHV::slot_addTracks_tidalPlaylist(const bool flag){

    if(flag == true){
        this->flag_addTracks_tidal = true;
    }

    if(this->flag_addTracks_rose == true){
        emit linker->signal_addPlaylistFinish();
    }
}


void AbstractAddPlaylist_RHV::slot_create_rosePlaylist(const QJsonObject &p_jsonData){

    int rcv_playlist_id = ProcJsonEasy::getInt(p_jsonData, "id");

    this->snd_track_array = QJsonArray();

    if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){

        for(int i = 0; i < this->listWidget_adding->count(); i++){
            //qDebug() << "confirm check ->" << this->listWidget_adding->count() << this->listWidget_adding->item(i)->whatsThis();

            QString splitIndex = this->listWidget_adding->item(i)->whatsThis();
            QStringList listIndex = splitIndex.split(",");
            QStringList strIndex = listIndex.at(1).split(":");
            int idx = strIndex.at(1).toInt();

            this->snd_track_array.append(this->added_array.at(idx).toObject());
        }

        if(contents_type == ContentsType::Tidal){

            for(int i = 0; i < this->snd_track_array.count(); i++){

                QJsonObject tmpObj =  this->snd_track_array.at(i).toObject();
                this->selected_track_ids += QString("%1,").arg(ProcJsonEasy::getInt(tmpObj, "id"));
            }

            if(this->flag_stream_enable == true){
                tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
                connect(procTidal, &tidal::ProcCommon::completeReq_addTracks_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_addTracks_tidalPlaylist);
                procTidal->request_tidal_addTracks_myPlaylist(this->selected_playlist_id, this->selected_track_ids);
            }
        }
        else if(contents_type == ContentsType::Qobuz){

            for(int i = 0; i < this->snd_track_array.count(); i++){

                QJsonObject tmpObj =  this->snd_track_array.at(i).toObject();
                this->selected_track_ids += QString("%1,").arg(ProcJsonEasy::getInt(tmpObj, "id"));
            }

            if(this->flag_stream_enable == true){
                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                connect(procQobuz, &qobuz::ProcCommon::completeReq_addTracks_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_addTracks_qobuzPlaylist);
                procQobuz->request_qobuz_addTracks_myPlaylist(this->selected_playlist_id, this->selected_track_ids);
            }
        }
    }
    else if(this->contents_ui_type == ContentsUIType::Editing){

        for(int i = 0; i < this->listWidget_editing->count(); i++){
            //qDebug() << "confirm check ->" << this->listWidget_adding->count() << this->listWidget_adding->item(i)->whatsThis();

            QString splitIndex = this->listWidget_editing->item(i)->whatsThis();
            QStringList listIndex = splitIndex.split(",");
            QStringList strIndex = listIndex.at(1).split(":");
            int idx = strIndex.at(1).toInt();

            this->snd_track_array.append(this->editing_array.at(idx).toObject());
        }
    }

    if(p_jsonData.contains("id")){

        QJsonArray tracks;

        if(this->contents_ui_type == ContentsUIType::Creating || this->contents_ui_type == ContentsUIType::Adding){
            for(int i = 0; i < this->snd_track_array.size(); i++){
                QJsonObject tmp_data = this->snd_track_array.at(i).toObject();

                QString clientKey = "";
                QString playUrl = "";
                QString thumbnail = "";
                QString title = "";
                QString type = "";
                int duration = 0;

                QJsonArray albums;
                QJsonArray artists;

                if(contents_type == ContentsType::Music){
                    albums.append(ProcJsonEasy::getString(tmp_data, "album"));
                    artists.append(ProcJsonEasy::getString(tmp_data, "artist"));
                    clientKey = ProcJsonEasy::getString(tmp_data, "id");
                    duration = ProcJsonEasy::getInt(tmp_data, "duration");
                    playUrl = ProcJsonEasy::getString(tmp_data, "data");
                    thumbnail = ProcJsonEasy::getString(tmp_data, "album_art");
                    title = ProcJsonEasy::getString(tmp_data, "title");
                    type = "MUSIC";
                }
                else if(contents_type == ContentsType::RoseTube){
                    clientKey = ProcJsonEasy::getString(tmp_data, "clientKey");
                    duration = ProcJsonEasy::getInt(tmp_data, "duration");
                    playUrl = ProcJsonEasy::getString(tmp_data, "clientKey");
                    title = ProcJsonEasy::getString(tmp_data, "title");
                    type = "YOUTUBE";
                }
                else if(contents_type == ContentsType::Tidal){
                    QJsonObject tmp_album = ProcJsonEasy::getJsonObject(tmp_data, "album");

                    clientKey = QString("%1").arg(ProcJsonEasy::getInt(tmp_data, "id"));
                    duration = ProcJsonEasy::getInt(tmp_data, "duration");
                    playUrl = clientKey;
                    thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(tmp_album, "cover"), 320);
                    title = ProcJsonEasy::getString(tmp_data, "title");
                    type = "TIDAL";
                }
                else if(contents_type == ContentsType::Bugs){
                    QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(tmp_data, "album");
                    QJsonObject albumImg = ProcJsonEasy::getJsonObject(tmpAlbum, "image");

                    QString url = ProcJsonEasy::getString(albumImg, "url");
                    QString path = ProcJsonEasy::getString(albumImg, "path");
                    thumbnail = url.replace("https://", "http://");
                    thumbnail += QString("%1").arg(static_cast<int>(bugs::ConvertData_forBugs::ImageSize_Square::Size_350x350)) + path;

                    QString tmpDur = ProcJsonEasy::getString(tmp_data, "len");
                    QStringList strDur = tmpDur.split(":");

                    if(strDur.count() > 2){
                        duration = (strDur.at(0).toInt() * 3600) + (strDur.at(1).toInt() * 60) + strDur.at(2).toInt();
                    }
                    else if(strDur.count() > 1){
                        duration = (strDur.at(0).toInt() * 60) + strDur.at(1).toInt();
                    }
                    else{
                        duration = strDur.at(0).toInt();
                    }

                    clientKey = QString("%1").arg(ProcJsonEasy::getInt(tmp_data, "track_id"));
                    playUrl = clientKey;
                    title = ProcJsonEasy::getString(tmp_data, "track_title");
                    type = "BUGS";
                }
                else if(contents_type == ContentsType::Qobuz){
                    QJsonObject tmp_album = ProcJsonEasy::getJsonObject(tmp_data, "album");
                    QJsonObject tmp_image = ProcJsonEasy::getJsonObject(tmp_album, "image");

                    clientKey = QString("%1").arg(ProcJsonEasy::getInt(tmp_data, "id"));
                    duration = ProcJsonEasy::getInt(tmp_data, "duration");
                    thumbnail = ProcJsonEasy::getString(tmp_image, "large");
                    playUrl = clientKey;
                    title = ProcJsonEasy::getString(tmp_data, "title");
                    type = "QOBUZ";
                }
                else{

                }

                QJsonObject tmp_track = QJsonObject();
                if(contents_type == ContentsType::Music){
                    QJsonObject data;
                    data.insert("macAddress", global.device.getDeviceID());
                    data.insert("trackId", 0);

                    tmp_track.insert("albums", albums);
                    tmp_track.insert("artists", artists);
                    tmp_track.insert("clientKey", clientKey);
                    tmp_track.insert("data", data);
                    tmp_track.insert("duration", duration);
                    tmp_track.insert("favorite", false);
                    tmp_track.insert("ownerId", 0);
                    tmp_track.insert("playUrl", playUrl);
                    tmp_track.insert("sort", 0);
                    tmp_track.insert("star", 0);
                    tmp_track.insert("thumbnailUrl", thumbnail);
                    tmp_track.insert("title", title);
                    tmp_track.insert("type", type);
                }
                else if(contents_type == ContentsType::RoseTube){
                    tmp_track.insert("clientKey", clientKey);
                    tmp_track.insert("duration", duration);
                    tmp_track.insert("favorite", false);
                    tmp_track.insert("ownerId", 0);
                    tmp_track.insert("playUrl", playUrl);
                    tmp_track.insert("sort", 0);
                    tmp_track.insert("star", 0);
                    tmp_track.insert("title", title);
                    tmp_track.insert("type", type);
                }
                else{
                    tmp_track.insert("clientKey", clientKey);
                    tmp_track.insert("data", tmp_data);
                    tmp_track.insert("duration", duration);
                    tmp_track.insert("favorite", false);
                    tmp_track.insert("ownerId", 0);
                    tmp_track.insert("playUrl", playUrl);
                    tmp_track.insert("sort", 0);
                    tmp_track.insert("star", 0);
                    tmp_track.insert("thumbnailUrl", thumbnail);
                    tmp_track.insert("title", title);
                    tmp_track.insert("type", type);
                }

                tracks.append(tmp_track);
            }
        }
        else if(this->contents_ui_type == ContentsUIType::Editing){
            tracks = this->snd_track_array;
        }

        QJsonObject json;
        json.insert("favorite", false);
        json.insert("favoriteTrack", false);
        json.insert("id", rcv_playlist_id);
        json.insert("isRose", true);
        json.insert("ownerId", 0);
        json.insert("star", 0);
        json.insert("thumbup", false);
        json.insert("thumbupCount", 0);
        json.insert("trackCount", 0);
        json.insert("tracks", tracks);

        roseHome::ProcCommon *procRose = new roseHome::ProcCommon(this);
        connect(procRose, &roseHome::ProcCommon::completeReq_addTrack_playlist, this, &AbstractAddPlaylist_RHV::slot_addTracks_rosePlaylist);
        procRose->request_rose_addTrack_myPlaylist(json);
    }
}


void AbstractAddPlaylist_RHV::slot_addTracks_rosePlaylist(const QJsonObject &p_jsonData){

    if(p_jsonData.contains("code") && (p_jsonData["code"].toString() == "200")){
        this->flag_addTracks_rose = true;
    }

    if(this->flag_addTracks_tidal == true){
        emit linker->signal_addPlaylistFinish();
    }
    else if(this->flag_addTracks_qobuz == true){
        emit linker->signal_addPlaylistFinish();
    }
    else if(this->flag_stream_enable == false){
        emit linker->signal_addPlaylistFinish();
    }
}


void AbstractAddPlaylist_RHV::slot_editInfo_rosePlaylist(const QJsonObject &p_jsonData){

    if(p_jsonData.contains("flagOk") && p_jsonData.find("flagOk")->toBool()){
        qDebug() << p_jsonData.find("flagOk")->toBool();
    }
}


void AbstractAddPlaylist_RHV::slot_editTracks_rosePlaylist(const QJsonObject &p_jsonData){

    if(p_jsonData.contains("flagOk") && p_jsonData.find("flagOk")->toBool()){

        if(this->listWidget_editing->count() > 0){

            QJsonObject json;
            json.insert("id", this->playlist_id);
            this->slot_create_rosePlaylist(json);
        }
        else{

        }
    }
}


void AbstractAddPlaylist_RHV::slot_dragAndDropLoop(){

    this->drop_cnt_flag++;
}


void AbstractAddPlaylist_RHV::slot_listwidgetItemPressed(QListWidgetItem *item){  //cheon210916-total

    QString tmpStr = item->whatsThis();

    QStringList split_type = tmpStr.split(",");
    QStringList split_widget = split_type.at(0).split(":");

    QString type = split_widget.at(1);

    int tmp_index = 0;
    if(type == "ADD"){
        tmp_index = this->listWidget_adding->row(item);
    }
    else if(type == "EXIST"){

    }
    else if(type == "EDIT"){
        tmp_index= this->listWidget_editing->row(item);
    }

    qDebug() << "tmp_index=" << tmp_index;
}


void AbstractAddPlaylist_RHV::slot_clicked_btnConfirm(){

    if(this->le_title->text().trimmed().isEmpty()){
        ToastMsg::show(this, "", tr("Please enter the title."));
    }
    else{

        QString type = "";

        if(contents_type == ContentsType::Music){

            type = "MUSIC";
        }
        else if(contents_type == ContentsType::RoseTube){

            type = "YOUTUBE";
        }
        else if(contents_type == ContentsType::Tidal){

            /*if(this->flag_stream_enable == true){
                tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
                connect(procTidal, &tidal::ProcCommon::completeReq_addTracks_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_addTracks_tidalPlaylist);
                procTidal->request_tidal_addTracks_myPlaylist(this->selected_playlist_id, this->selected_track_ids);
            }*/

            type = "TIDAL";
        }
        else if(contents_type == ContentsType::Bugs){

            type = "BUGS";
        }
        else if(contents_type == ContentsType::Qobuz){

            /*if(this->flag_stream_enable == true){
                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                connect(procQobuz, &qobuz::ProcCommon::completeReq_addTracks_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_addTracks_qobuzPlaylist);
                procQobuz->request_qobuz_addTracks_myPlaylist(this->selected_playlist_id, this->selected_track_ids);
            }*/

            type = "QOBUZ";
        }


        if(contents_ui_type == ContentsUIType::Creating){

            QString clientKey = QString("%1").arg(QDateTime::currentMSecsSinceEpoch());

            QJsonArray tags;
            QString str_tags = this->le_tag->text().replace("#", "").replace(" ", "");

            QStringList strList = str_tags.split(",");
            for(int i = 0 ; i < strList.count(); i++){
                tags.append(strList.at(i));
            }

            QString share = "PRIVATE";
            if(this->flag_all_enable == true){
                share = "ALL";
            }
            else if(this->flag_freind_enable == true){
                share = "FRIEND";
            }
            else if(this->flag_private_enable == true){
                share = "PRIVATE";
            }

            QJsonObject data;
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject json;
            json.insert("clientKey", clientKey);
            json.insert("comment", this->te_content->toPlainText());
            json.insert("data", data);
            json.insert("favorite", false);
            json.insert("favoriteTrack", false);
            json.insert("isRose", true);
            json.insert("ownerId", 0);
            json.insert("share", share);
            json.insert("star", 0);
            json.insert("tags", tags);
            json.insert("thumbup", false);
            json.insert("thumbupCount", 0);
            json.insert("title", this->le_title->text());
            json.insert("trackCount", 0);
            json.insert("type", type);

            roseHome::ProcCommon *procRose = new roseHome::ProcCommon(this);
            connect(procRose, &roseHome::ProcCommon::completeReq_create_playlist, this, &AbstractAddPlaylist_RHV::slot_create_rosePlaylist);
            procRose->request_rose_create_myPlaylist(json);
        }
        else if(contents_ui_type == ContentsUIType::Adding){

            QJsonObject json;
            json.insert("id", this->playlist_id);
            this->slot_create_rosePlaylist(json);
        }
        else if(contents_ui_type == ContentsUIType::Editing){

            QJsonObject json;
            json.insert("favorite", false);
            json.insert("favoriteTrack", false);
            json.insert("id", this->playlist_id);
            json.insert("isRose", true);
            json.insert("ownerId", 0);
            json.insert("star", 0);
            json.insert("thumbup", false);
            json.insert("thumbupCount", 0);
            json.insert("trackCount", 0);
            json.insert("tracks", this->editing_array);

            roseHome::ProcCommon *procRose = new roseHome::ProcCommon(this);
            connect(procRose, &roseHome::ProcCommon::completeReq_editTrack_playlist, this, &AbstractAddPlaylist_RHV::slot_editTracks_rosePlaylist);
            procRose->request_rose_editTrack_myPlaylist(json);
        }
    }
}


void AbstractAddPlaylist_RHV::slot_clicked_btnCancel(){

    emit linker->signal_addPlaylistFinish();
}


bool AbstractAddPlaylist_RHV::tracksDelete_DialogConfirm(){

    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("PlayList Delete Notice"));
    dlg->setText(tr("Are you sure you want to delete your selected playlist's tracks?"));
    dlg->setFixedSize(600, 550);

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
    int left = global.left_mainwindow + ((latestWidth - dlg->sizeHint().width()) / 2);
    int top = global.top_mainwindow + ((latestHeight - dlg->sizeHint().height()) / 2);
    dlg->setGeometry(left, top, 0, 0);

    int result = dlg->exec();

    if(result==QDialog::Accepted){
        return result;
    }

    delete dlg;

    return result;
}


void AbstractAddPlaylist_RHV::slot_thumbnailDownload_adding(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index >= 0 && tmp_index < this->added_array.size()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        if(flagLoaded){
            QPixmap pixmapIMG = QPixmap(QSize(60, 60));
            pixmapIMG.fill(Qt::black);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 60, 60, 2, 2);

            int leftValue = (60 - tmp_pixmap.width()) / 2;
            int topValue = (60 - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_trackImg_adding[tmp_index]->setPixmap(pixmapIMG);
        }
    }

    fileDownloader->deleteLater();
}


void AbstractAddPlaylist_RHV::slot_thumbnailDownload_existing(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index >= 0 && tmp_index < this->existing_array.size()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        if(flagLoaded){
            QPixmap pixmapIMG = QPixmap(QSize(60, 60));
            pixmapIMG.fill(Qt::black);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 60, 60, 2, 2);

            int leftValue = (60 - tmp_pixmap.width()) / 2;
            int topValue = (60 - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_trackImg_existing[tmp_index]->setPixmap(pixmapIMG);
        }
    }

    fileDownloader->deleteLater();
}


void AbstractAddPlaylist_RHV::slot_thumbnailDownload_editing(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index >= 0 && tmp_index < this->editing_array.size()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        if(flagLoaded){
            QPixmap pixmapIMG = QPixmap(QSize(60, 60));
            pixmapIMG.fill(Qt::black);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 60, 60, 2, 2);

            int leftValue = (60 - tmp_pixmap.width()) / 2;
            int topValue = (60 - tmp_pixmap.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->lb_trackImg_editing[tmp_index]->setPixmap(pixmapIMG);
        }
    }

    fileDownloader->deleteLater();
}


void AbstractAddPlaylist_RHV::slot_fileDownload_loadImage(){

    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(315, 315));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(315, 315, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, 315, 315, 8, 8);

        int leftValue = (315 - tmp_pixmap.width()) / 2;
        int topValue = (315 - tmp_pixmap.height()) / 2;

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_image->setPixmap(pixmapIMG);
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }
}


void AbstractAddPlaylist_RHV::paint_imageBig(QPixmap &pixmap){

    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(315, 315));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path = QPainterPath();
    path.addRoundedRect(0, 0, 315, 315, 2, Qt::RelativeSize);

    painter.setClipPath(path);
    painter.drawPixmap(116, 116, pixmap);
    painter.end();

    this->lb_image->setPixmap(pixmap_painter);
}


void AbstractAddPlaylist_RHV::setImage(QString imagePath){

    // 이미지 경로 설정
    if(imagePath.isEmpty() == false){
         this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }
}
