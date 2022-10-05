#include "common/global.h"
#include "artistinfo.h"
#include "proc_uploadthumbnail.h"

#include <QJsonObject>
#include <QVBoxLayout>
#include <QPainter>
#include <QJsonArray>
#include <QScrollArea>
#include <QScroller>
#include <QSettings>

#include "common/rosesettings.h"
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <common/sqlitehelper.h>
#include <widget/trackinfo.h>
#include <widget/framemusicalbum.h>

const int ARTISTIMG_SIZE = 300;
const int HTTP_PLAY_ALL = 99;
const int HTTP_PLAY_SHUFFLE = 98;



/** ******************************************************************************************************
 * ******************************************************************************************************
 *
 *  ArtistInfo 클래스
 *
 * ******************************************************************************************************
 * ****************************************************************************************************** */
/**
 * @brief ArtistInfo::ArtistInfo 생성자
 * @param parent
 */
ArtistInfo::ArtistInfo(QWidget *parent) : QWidget(parent)
{
    setUIControl();
}


ArtistInfo::~ArtistInfo(){

    this->deleteLater();
}


/**
 * @brief ArtistInfo::setUIControl UI 세팅
 */
void ArtistInfo::setUIControl(){

    linker = Linker::getInstance();

    proc_add= new Proc_AddTackInPlayList(this);
    connect(proc_add, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_add, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");

    // 앨범 이미지
    this->lb_albumImg = new QLabel();
    this->lb_albumImg->setFixedSize(ARTISTIMG_SIZE,ARTISTIMG_SIZE);

    // 가수명 및 정보
    this->lb_artist = new QLabel();
    this->lb_artist->setStyleSheet("color:#FFFFFF;font-size:30px;");
    this->lb_infoText = new QLabel();
    this->lb_infoText->setStyleSheet("color:#E6E6E6;font-size:20px;");
    this->lb_artist->setContentsMargins(18,0,0,0);
    this->lb_infoText->setContentsMargins(18,10,0,0);

    // SNS
    QPushButton *btn_sns_web_ico = GSCommon::getUIBtnImg("sns_web_ico", ":/images/rosehome/icon_share.png");//c220825_2
    btn_sns_web_ico->setFixedSize(60,60);
    btn_sns_web_ico->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_sns_insta_ico = GSCommon::getUIBtnImg("sns_insta_ico", ":/images/sns_insta_ico.png");
    btn_sns_insta_ico->setFixedSize(60,60);
    btn_sns_insta_ico->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_sns_twit_ico = GSCommon::getUIBtnImg("sns_twit_ico", ":/images/sns_twit_ico.png");
    btn_sns_twit_ico->setFixedSize(60,60);
    btn_sns_twit_ico->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_sns_face_ico = GSCommon::getUIBtnImg("sns_face_ico", ":/images/sns_face_ico.png");
    btn_sns_face_ico->setFixedSize(60,60);
    btn_sns_face_ico->setCursor(Qt::PointingHandCursor);
    QHBoxLayout *hl_hart = new QHBoxLayout();
    hl_hart->setContentsMargins(0,0,0,0);
    hl_hart->setSpacing(0);
    hl_hart->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hl_hart->addSpacing(3);
    hl_hart->addWidget(btn_sns_web_ico);
    hl_hart->addWidget(btn_sns_insta_ico);
    hl_hart->addWidget(btn_sns_twit_ico);
    hl_hart->addWidget(btn_sns_face_ico);

    // 전체재생
    QPushButton *btn_icon_list_allplay = new QPushButton();
    //btn_icon_list_allplay->setFixedSize(96,30);
    btn_icon_list_allplay->setText(tr("Play All"));
    //btn_icon_list_allplay->setText(tr("전체재생"));
    QPixmap pixmap_allPlay(":/images/icon_list_allplay.png");
    btn_icon_list_allplay->setIcon(pixmap_allPlay);
    btn_icon_list_allplay->setIconSize(pixmap_allPlay.rect().size());
    btn_icon_list_allplay->setCursor(Qt::PointingHandCursor);
    btn_icon_list_allplay->setStyleSheet("font-size:16px;color:#FFFFFF;");


    QPushButton *btn_icon_list_shuffle = new QPushButton();
    //btn_icon_list_shuffle->setFixedSize(96,30);
    btn_icon_list_shuffle->setText(tr("Shuffle Play"));
    //btn_icon_list_shuffle->setText(tr("셔플재생"));
    QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
    btn_icon_list_shuffle->setIcon(pixmap_shuffle);
    btn_icon_list_shuffle->setIconSize(pixmap_shuffle.rect().size());
    btn_icon_list_shuffle->setCursor(Qt::PointingHandCursor);
    btn_icon_list_shuffle->setStyleSheet("font-size:16px;color:#FFFFFF;");



    QHBoxLayout *hl_playType = new QHBoxLayout();
    hl_playType->setContentsMargins(0,0,0,0);
    hl_playType->setSpacing(0);
    hl_playType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hl_playType->addSpacing(18);
    hl_playType->addWidget(btn_icon_list_allplay);
    hl_playType->addSpacing(30);
    hl_playType->addWidget(btn_icon_list_shuffle);

    QVBoxLayout *vl_albumInfo = new QVBoxLayout();
    vl_albumInfo->setContentsMargins(0,0,0,0);
    vl_albumInfo->setSpacing(0);
    vl_albumInfo->setAlignment(Qt::AlignLeft | Qt::AlignBottom);    // VCenter 가 아님
    //vl_albumInfo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);    // VCenter 가 아님
    vl_albumInfo->addWidget(this->lb_artist, 0, Qt::AlignVCenter);
    vl_albumInfo->addWidget(this->lb_infoText);
    vl_albumInfo->addSpacing(30);
    //vl_albumInfo->addLayout(hl_hart);
    vl_albumInfo->addSpacing(10);
    vl_albumInfo->addLayout(hl_playType);
    vl_albumInfo->addSpacing(20);

    QHBoxLayout *hl_albumInfo = new QHBoxLayout();
    hl_albumInfo->setContentsMargins(0,0,0,0);
    hl_albumInfo->setSpacing(0);
    hl_albumInfo->addWidget(this->lb_albumImg);
    hl_albumInfo->addSpacing(10);
    hl_albumInfo->addLayout(vl_albumInfo);

    // 하단 곡 리스트 / 앨범 리스트
    this->lb_tracks = new QLabel();
    this->lb_tracks->setText(tr("Song"));
    //lb_tracks->setText(tr("곡"));
    this->lb_tracks->setStyleSheet("font-size:20px;color:#FFFFFF;");

    QPushButton *btnViewAll_track = new QPushButton;
    btnViewAll_track->setText(tr("View All"));
    btnViewAll_track->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_track->setCursor(Qt::PointingHandCursor);
    btnViewAll_track->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_ARTIST_TRACKLIST);
    connect(btnViewAll_track, SIGNAL(clicked()), this, SLOT(slot_clickedTrackViewAll()));

    QHBoxLayout *box_track = new QHBoxLayout;
    box_track->setSpacing(0);
    box_track->setContentsMargins(0,0,0,0);
    box_track->setAlignment(Qt::AlignHCenter);
    box_track->addWidget(this->lb_tracks);
    box_track->addStretch();
    box_track->addWidget(btnViewAll_track);

    QWidget *widget_track = new QWidget;
    widget_track->setLayout(box_track);
    widget_track->setFixedHeight(60);
    widget_track->setStyleSheet("background-color:#333333;border:0px;margin-bottom:0px;");

    this->lb_albums = new QLabel();
    this->lb_albums->setText(tr("Album"));
    //lb_albums->setText(tr("앨범"));    
    this->lb_albums->setStyleSheet("font-size:20px;color:#FFFFFF;");

    QPushButton *btnViewAll_albums = new QPushButton;
    btnViewAll_albums->setText(tr("View All"));
    btnViewAll_albums->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_albums->setCursor(Qt::PointingHandCursor);
    btnViewAll_albums->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_ARTIST_ALBUMLIST);
    connect(btnViewAll_albums, SIGNAL(clicked()), this, SLOT(slot_clickedAlbumViewAll()));

    QHBoxLayout *box_albums = new QHBoxLayout;
    box_albums->setSpacing(0);
    box_albums->setContentsMargins(0,0,0,0);
    box_albums->setAlignment(Qt::AlignHCenter);
    box_albums->addWidget(this->lb_albums);
    box_albums->addStretch();
    box_albums->addWidget(btnViewAll_albums);

    QWidget *widget_albums = new QWidget;
    widget_albums->setLayout(box_albums);
    widget_albums->setFixedHeight(60);
    widget_albums->setStyleSheet("background-color:#333333;border:0px;margin-bottom:0px;");

    this->vl_tracks = new QVBoxLayout();
    this->vl_tracks->setContentsMargins(0,0,0,0);
    this->vl_tracks->setSpacing(0);
    this->vl_tracks->setAlignment(Qt::AlignTop);

    /*this->fl_albums = new FlowLayout();
    this->fl_albums->setContentsMargins(0,0,0,0);
    this->fl_albums->setSpacing(0);
    this->fl_albums->setSpacingHV(18, 20);*/

    this->boxAlbums = new QHBoxLayout();
    this->boxAlbums->setContentsMargins(0,0,0,0);
    this->boxAlbums->setSpacing(20);
    this->boxAlbums->setSizeConstraint(QLayout::SetFixedSize);

    QWidget *widgetAlbums = new QWidget();
    widgetAlbums->setLayout(this->boxAlbums);

    QScrollArea *scrollArea_Albums = new QScrollArea();
    scrollArea_Albums->setWidget(widgetAlbums);
    scrollArea_Albums->setWidgetResizable(true);
    scrollArea_Albums->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_Albums->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_Albums->setStyleSheet("background-color:transparent;border:0px;margin-bottom:0px;");
    scrollArea_Albums->setFixedHeight(260);

    QScroller::grabGesture(scrollArea_Albums, QScroller::LeftMouseButtonGesture);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(38,10,28,40);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addLayout(hl_albumInfo);
    vl_total->addSpacing(30);
    //vl_total->addWidget(lb_tracks);
    vl_total->addWidget(widget_track);
    vl_total->addSpacing(10);
    vl_total->addLayout(vl_tracks);
    vl_total->addSpacing(30);
    //vl_total->addWidget(lb_albums);
    vl_total->addWidget(widget_albums);
    vl_total->addSpacing(10);
    //vl_total->addLayout(fl_albums, 9);
    vl_total->addWidget(scrollArea_Albums);
    vl_total->addSpacing(20);

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setLayout(vl_total);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

    // 커넥션
    connect(btn_icon_list_allplay, &QPushButton::clicked, this, &ArtistInfo::slot_playAll);
    connect(btn_icon_list_shuffle, &QPushButton::clicked, this, &ArtistInfo::slot_playShuffle);

}


/**
 * @brief ArtistInfo::loadImage [SLOT] 앨범 이미지 세팅
 */
void ArtistInfo::slot_loadImage()
{
    bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(ARTISTIMG_SIZE, ARTISTIMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(ARTISTIMG_SIZE,ARTISTIMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, ARTISTIMG_SIZE, ARTISTIMG_SIZE, 150, 150);    // border-radius:4px
        this->lb_albumImg->setPixmap(pixmap_painter);
    }

    this->fileDownLoader->deleteLater();
}

/**
 * @brief ArtistInfo::slot_playAll [SLOT] 전체재생
 */
void ArtistInfo::slot_playAll(){
    QJsonArray array_music;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        array_music.append(this->list_audioInfo.at(i)->getJsonData());
    }
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 0);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    network->request(HTTP_PLAY_ALL, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);

}
/**
 * @brief ArtistInfo::slot_playShuffle [SLOT] 셔플재생
 */
void ArtistInfo::slot_playShuffle(){

    QJsonArray array_music;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        array_music.append(this->list_audioInfo.at(i)->getJsonData());
    }
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 1);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    network->request(HTTP_PLAY_SHUFFLE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
}


/**
 * @brief ArtistInfo::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void ArtistInfo::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    Q_UNUSED(p_id);
    Q_UNUSED(p_jsonObject);

    sender()->deleteLater();
}

/**
 * @brief ArtistInfo::setDataArtistInfo 기본정보 데이터 세팅
 * @note [참고] OptionPopup 클래스의 setData 받는 구조와 동일하다
 * @param p_data
 */
void ArtistInfo::setDataArtistInfo(QJsonObject p_data){

    // 초기화
    this->type_optionMenu = OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack;
    this->albumImg = "";
    this->list_audioInfo = QList<DataPopup*>();
    //this->list_albumInfo = QList<DataPopup*>();
    this->artist = "";
    this->infoText = "";

    // 데이터 세팅
    if(p_data.contains(KEY_OP_TYPE)){
        //this->type_optionMenu = p_data[KEY_OP_TYPE].toInt();
        this->type_optionMenu = static_cast<OptionPopup::TypeMenu>(p_data[KEY_OP_TYPE].toInt());
    }
    if(p_data.contains(KEY_OP_albumImg)){
        this->albumImg = p_data[KEY_OP_albumImg].toString();
    }
    if(p_data.contains(KEY_OP_DATA)){
        QJsonArray tmp_arr = p_data[KEY_OP_DATA].toArray();
        for(int i=0; i<tmp_arr.count(); i++){
            DataPopup *tmp_dataPopup = new DataPopup();
            tmp_dataPopup->setJsonData(tmp_arr[i].toObject());

            this->list_audioInfo.append(tmp_dataPopup);
        }
    }
    if(this->list_audioInfo.count()>0){
        // 첫번째 audioInfo 를 옵션 팝업 화면에 보여질 데이터로 사용한다.
        this->artist = this->list_audioInfo.first()->getArtist();
    }

    // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
    emit signal_changedMenuName(this->artist);


    // 이미지 경로 설정    
    QString tmp_imgURL = "";
    if(this->albumImg.contains("http://")){
        tmp_imgURL = this->albumImg;
    }
    else{
        tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->albumImg);
    }

    if(this->type_optionMenu==OptionPopup::TypeMenu::Music_Home_MyPlayList
            || this->type_optionMenu==OptionPopup::TypeMenu::Music_Home_FriendPlayList
            || this->type_optionMenu==OptionPopup::TypeMenu::Music_Etc_PlayListDetailTrack_My
            ){
        tmp_imgURL = this->albumImg;
    }
    this->lb_albumImg->setPixmap(*this->pixmap_albumImg_default);
    if(this->albumImg!=""){
        // 앨범이미지 다운받아서 세팅 : loadImage() 참고
        this->fileDownLoader->setImageURL(QUrl(tmp_imgURL));
    }

    // UI에 데이터 세팅
    this->lb_artist->setText(this->artist);
    this->lb_infoText->setText(this->infoText);

    this->setDataTrackFromDB();
    this->setDataAlbumFromDB();
}

/**
 * @brief ArtistInfo::setDataTrackFromDB [트랙] 데이터 가져와서 UI 세팅
 */
void ArtistInfo::setDataTrackFromDB(){

    // 초기화
    GSCommon::clearLayout(this->vl_tracks);
    this->list_audioInfo = QList<DataPopup*>();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){

//        QString strQuery = "";
//        strQuery = "SELECT A.title, A.album, A.album_id, A.audio_id AS id, A._data AS data, A.artist, A.artist_id, A.duration, ";
//        strQuery += " (SELECT _data FROM album_art WHERE album_id=A.album_id) AS album_art ";
//        strQuery += " FROM audio AS A LEFT JOIN album_info AS AI ON A.album_id=AI._id ";
//        strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track ";

        QString strQuery = "";
        strQuery = " SELECT count(A.album) AS track_count ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        if(this->artist.front() == "'" || this->artist.back() == "'"){
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='\"%1\"') ORDER BY A.track ";
        }
        else{
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track ";
        }

        QVariantList data_cnt;
        sqlite->exec(strQuery.arg(this->artist), data_cnt);

        int track_cnt = 0;
        if(data_cnt.size() > 0){
            QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
            track_cnt = tmp_data["track_count"].toInt();
        }
        this->lb_tracks->setText(tr("Song") + QString(" (%1)").arg(track_cnt));

        strQuery = "";
        //strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art , A.mime_type, A.samplerate, A.bitdepth ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id";
        if(this->artist.front() == "'" || this->artist.back() == "'"){
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist= '\"%1\"') ORDER BY A.track LIMIT 5 ";
        }
        else{
            strQuery += " WHERE A.artist_id=(SELECT artist_id from artists where artist='%1') ORDER BY A.track LIMIT 5 ";
        }

        QVariantList data;
        sqlite->exec(strQuery.arg(this->artist), data);

        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_type = "";

                QString tmp_mime = "";
                int tmp_sample = 0;
                int tmp_bit = 0;
                QString snd_mime = "";

                tmp_sample = tmp_data["samplerate"].toInt();
                tmp_bit = tmp_data["bitdepth"].toInt();

                tmp_split = tmp_data["mime_type"].toString();
                splitToken = tmp_split.split("/");

                if(splitToken.size() > 1){
                    tmp_type = splitToken.at(1);

                    if(tmp_type == "x-wav" || tmp_type == "wav"){
                        tmp_mime = "WAV";
                    }
                    else if (tmp_type == "wv") {
                        tmp_mime = "WV";
                    }
                    else if(tmp_type == "flac"){
                        tmp_mime = "FLAC";
                    }
                    else if(tmp_type == "dff" || tmp_type == "dsf"){
                        tmp_mime = "DSD";
                    }
                    else if(tmp_type == "mp4"){
                        tmp_mime = "ALAC";
                    }
                    else if(tmp_type == "mpeg"){
                        tmp_mime = "MP3";
                    }
                    else if(tmp_type == "amr"){
                        tmp_mime = "AMR";
                    }
                    else if(tmp_type == "amr-wb"){
                        tmp_mime = "AWB";
                    }
                    else if(tmp_type == "x-ms-wma"){
                        tmp_mime = "WMA";
                    }
                    else if(tmp_type == "ape"){
                        tmp_mime = "APE";
                    }
                    else if(tmp_type == "ogg"){
                        tmp_mime = "OGG";
                    }
                    else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                        tmp_mime = "AAC";
                    }
                    else if(tmp_type == "x-matroska"){
                        tmp_mime = "MKA";
                    }
                    else if(tmp_type == "midi"){
                        tmp_mime = "MIDI";
                    }
                    else if(tmp_type == "sp-midi"){
                        tmp_mime = "SMF";
                    }
                    else if(tmp_type == "imelody"){
                        tmp_mime = "IMY";
                    }
                    else if(tmp_type == "x-mpegurl"){
                        tmp_mime = "M3U8";
                    }
                    else if(tmp_type == "x-scpls"){
                        tmp_mime = "PLS";
                    }
                    else if(tmp_type == "aiff"){
                        tmp_mime = "AIFF";
                    }
                }

                if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                        || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                    snd_mime = tmp_mime;
                }
                else if(tmp_mime == "DSD"){
                    if(tmp_sample == 88200 || tmp_sample == 2822400){
                        snd_mime = tmp_mime + " 64";
                    }
                    else if(tmp_sample == 176400 || tmp_sample == 5644800){
                        snd_mime = tmp_mime + " 128";
                    }
                    else if(tmp_sample == 352800 || tmp_sample == 11289600){
                        snd_mime = tmp_mime + " 256";
                    }
                    else if(tmp_sample == 705600 || tmp_sample == 22579200){
                        snd_mime = tmp_mime + " 512";
                    }
                    else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                        snd_mime = tmp_mime + " 1024";
                    }
                    else{
                        snd_mime = tmp_mime;
                    }
                }
                else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                    if(tmp_bit == 0 || tmp_bit == 16){
                        snd_mime = tmp_mime;
                    }
                    else if(tmp_bit > 16){
                        snd_mime = tmp_mime + QString(" %1").arg(tmp_bit);
                    }
                }
                tmp_data.insert("album_mime", snd_mime);

                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(tmp_data);                
                this->list_audioInfo.append(tmp_dataPopup);

                QString tmp_albumImg = "";
                if(tmp_data.contains("album_art")){
                    tmp_albumImg = tmp_data["album_art"].toString();
                }
                TrackInfo *tmp_trackInfo = new TrackInfo(OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                tmp_trackInfo->setProperty("index", i);
                tmp_trackInfo->setDataTrackInfo(this->list_audioInfo.at(i), tmp_albumImg);

                connect(tmp_trackInfo, SIGNAL(signal_clickedAddPlaylist()), this, SLOT(slot_playlistClickedTrack()));

                this->vl_tracks->addWidget(tmp_trackInfo);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

/**
 * @brief ArtistInfo::setDataAlbumFromDB [앨범] 데이터 가져와서 UI 세팅
 */
void ArtistInfo::setDataAlbumFromDB(){

    // 초기화
    //GSCommon::clearLayout(this->fl_albums);
    GSCommon::clearLayout(this->boxAlbums);
    //this->list_albumInfo = QList<DataPopup*>();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT count(AI.album) AS album_count ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        if(this->artist.front() == "'" || this->artist.back() == "'" || this->artist.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\")";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ";
        }

        QVariantList data_cnt;
        sqlite->exec(strQuery.arg(this->artist), data_cnt);

        int album_cnt = 0;
        if(data_cnt.size() > 0){
            QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
            album_cnt = tmp_data["album_count"].toInt();
        }
        this->lb_albums->setText(tr("Album") + QString(" (%1)").arg(album_cnt));

        strQuery = "";
        strQuery =  " SELECT AI.album, AI._id AS album_id, AI.artist, AI.artist_id, AI.numsongs AS num_of_song, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        //strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1')";
        if(this->artist.front() == "'" || this->artist.back() == "'" || this->artist.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\") ORDER BY AI.album LIMIT 15 ";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ORDER BY AI.album LIMIT 15 ";
        }
        //strQuery += " ORDER BY AI.album LIMIT 15 ";

        QVariantList data;
        sqlite->exec(strQuery.arg(this->artist), data);

        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();

                FrameMusicAlbum *tmp_album = new FrameMusicAlbum(this);
                tmp_album->setHover();
                tmp_album->setData(tmp_data);

                //this->fl_albums->addWidget(tmp_album);
                this->boxAlbums->addWidget(tmp_album);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

/**
 * @brief ArtistInfo::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray ArtistInfo::getJsonArr_TrackData(const int &p_index){
    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < list_audioInfo.size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }



    return jsonArray;
}

/**
 * @brief ArtistInfo::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject ArtistInfo::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < list_audioInfo.size()){
        DataPopup *dataPopup = list_audioInfo.at(p_index);

        // 앨범
        jsonTrack.insert("album", dataPopup->getAlbum());

        // 아티스트
        QJsonObject jsonArtist;
        jsonArtist.insert("name", dataPopup->getArtist());
        QJsonArray jsonArrArtists;
        jsonArrArtists.append(jsonArtist);
        jsonTrack.insert("artists", jsonArrArtists);

        // 클라이언트 아이디 = 오디오 아이디
        jsonTrack.insert("client_id", dataPopup->getId());

        // 썸네일
        QJsonObject jsonThumb;
        jsonThumb.insert("url", albumImg);

        QJsonArray jsonArrThumbs;
        jsonArrThumbs.append(jsonThumb);
        jsonTrack.insert("thumbnails", jsonArrThumbs);

        // 타이틀
        jsonTrack.insert("title", dataPopup->getTitle());

        // 재생 URL
        jsonTrack.insert("url", dataPopup->getData());

        jsonTrack.insert("isFavorite", false);
        jsonTrack.insert("isSelected", false);
        jsonTrack.insert("mac_address", global.device.getDeviceID());
        jsonTrack.insert("type","music");
    }


    return jsonTrack;
}

/**
 * @brief ArtistInfo::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void ArtistInfo::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_add->requestAddPlayListTrack(p_playlistno, getJsonArr_TrackData(tmp_index));

        // ------------------------------------------------
        //  썸네일 업로드
        // ------------------------------------------------
        if(tmp_index >=0 && tmp_index < list_audioInfo.size()){
            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(list_audioInfo.at(tmp_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, list_audioInfo.at(tmp_index)->getData());
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
        }

    }else{
        proc_add->showDlgOfNewPlayList();
    }


}

/**
 * @brief ArtistInfo::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void ArtistInfo::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    QString tmp_playUrl;
    if(tmp_index <=0){
        tmp_playUrl = list_audioInfo.at(0)->getData();
    }else{
        tmp_playUrl = list_audioInfo.at(tmp_index)->getData();
    }

    proc_add->requestNewPlaylistTrack(*pixmap_albumImg, tmp_playUrl, p_name, getJsonArr_TrackData(tmp_index));
}

/**
 * @brief ArtistInfo::slot_clickedViewAll() [SLOT] View All 버튼 클릭시
 */
void ArtistInfo::slot_clickedTrackViewAll(){
    QJsonObject jsonData;
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_VA_ARTIST_TRACKLIST);
    jsonData.insert("artist", this->artist);
    emit linker->signal_clickedMovePage(jsonData);
}

void ArtistInfo::slot_clickedAlbumViewAll(){
    QJsonObject jsonData;
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_VA_ARTIST_ALBUMLIST);
    jsonData.insert("artist", this->artist);
    emit linker->signal_clickedMovePage(jsonData);
}

/**
 * @brief ArtistInfo::slot_playlistClickedTrack : [슬롯]트랙 플레이리스트 담기 클릭
 */
void ArtistInfo::slot_playlistClickedTrack(){

    int tmp_index = sender()->property("index").toInt();
    proc_add->setProperty("index",tmp_index);
    proc_add->showDlgOfPlayList();
}
