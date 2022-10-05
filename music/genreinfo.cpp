#include "genreinfo.h"
#include "proc_uploadthumbnail.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include <common/sqlitehelper.h>
#include "widget/toastmsg.h"
#include <QPainter>

#include "common/rosesettings.h"
#include <QSettings>
#include <QScrollArea>
#include <QScroller>
#include <widget/framemusicalbum.h>

const int COVER_IMG_SIZE = 300;
const int HTTP_PLAY_ALL = 99;
const int HTTP_PLAY_SHUFFLE = 98;
const int HTTP_FAVORITE_GET = 59;
const int HTTP_FAVORITE_ADD = 58;
const int HTTP_FAVORITE_DELETE = 57;

/**
 * @brief GenreInfo::GenreInfo : 음악-분류-장르의 상세 화면
 * @param parent
 */
GenreInfo::GenreInfo(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

GenreInfo::~GenreInfo(){

    this->fileDownLoader->deleteLater();
    this->deleteLater();
}


/**
 * @brief GenreInfo::setInit : 초기 세팅
 */
void GenreInfo::setInit(){
    linker = Linker::getInstance();

    proc_add= new Proc_AddTackInPlayList(this);
    connect(proc_add, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_add, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    delegate = new FileDelegate(this);
    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
}

/**
 * @brief GenreInfo::setUIControl : UI 세팅
 */
void GenreInfo::setUIControl(){
    // 앨범 이미지
    this->lb_albumImg = new QLabel();
    this->lb_albumImg->setFixedSize(COVER_IMG_SIZE,COVER_IMG_SIZE);

    // 장르명 및 정보
    this->lb_genre = new QLabel();
    this->lb_genre->setStyleSheet("color:#FFFFFF;font-size:30px;");
    this->lb_infoText = new QLabel();
    this->lb_infoText->setStyleSheet("color:#E6E6E6;font-size:20px;");
    this->lb_genre->setContentsMargins(18,0,0,0);
    this->lb_infoText->setContentsMargins(18,10,0,0);

    // 전체재생
    QPushButton *btn_icon_list_allplay = new QPushButton();
    btn_icon_list_allplay->setFixedSize(96,30);
    btn_icon_list_allplay->setText(tr("Play All"));
    //btn_icon_list_allplay->setText(tr("전체재생"));
    QPixmap pixmap_allPlay(":/images/icon_list_allplay.png");
    btn_icon_list_allplay->setIcon(pixmap_allPlay);
    btn_icon_list_allplay->setIconSize(pixmap_allPlay.rect().size());
    btn_icon_list_allplay->setCursor(Qt::PointingHandCursor);
    btn_icon_list_allplay->setStyleSheet("font-size:16px;color:#FFFFFF;");


    QPushButton *btn_icon_list_shuffle = new QPushButton();
    btn_icon_list_shuffle->setFixedSize(96,30);
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
    vl_albumInfo->addWidget(this->lb_genre, 0, Qt::AlignVCenter);
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

//---------------------------------------------------------------------------------
    this->lb_albums = new QLabel();
    this->lb_albums->setText(tr("Album"));
    //lb_albums->setText(tr("앨범"));
    this->lb_albums->setStyleSheet("font-size:20px;color:#FFFFFF;");

    QPushButton *btnViewAll_albums = new QPushButton;
    btnViewAll_albums->setText(tr("View All"));
    btnViewAll_albums->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_albums->setCursor(Qt::PointingHandCursor);
    btnViewAll_albums->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_GENRE_ALBUMLIST);
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

    //-----------------------------------------------

    // 하단 곡 리스트 / 앨범 리스트
    QLabel *lb_tracks = new QLabel();
    lb_tracks->setText(tr("Song"));
    //lb_tracks->setText(tr("곡"));
    lb_tracks->setStyleSheet("font-size:20px;color:#FFFFFF;");

    // ------------------------------------------------------
    //  파일 목록 ListWidget
    // ------------------------------------------------------
    listWidget = new CustomListWidget(this);
    listWidget->setItemDelegate(delegate);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("border:none;");

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(38,10,28,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addLayout(hl_albumInfo);
    //vl_total->addSpacing(30);
    //vl_total->addWidget(lb_tracks);
    //vl_total->addSpacing(10);
    //vl_total->addWidget(listWidget);

    //------------------------------------
    vl_total->addSpacing(30);
    //vl_total->addWidget(lb_albums);
    vl_total->addWidget(widget_albums);
    vl_total->addSpacing(20);
    //vl_total->addLayout(fl_albums, 9);
    vl_total->addWidget(scrollArea_Albums);
    vl_total->addSpacing(70);
    //----------------------------------------

    setLayout(vl_total);

    // 커넥션
    connect(btn_icon_list_allplay, &QPushButton::clicked, this, &GenreInfo::slot_playAll);
    connect(btn_icon_list_shuffle, &QPushButton::clicked, this, &GenreInfo::slot_playShuffle);
}

/**
 * @brief GenreInfo::loadImage [SLOT] 앨범 이미지 세팅
 */
void GenreInfo::slot_loadImage()
{
    bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(COVER_IMG_SIZE, COVER_IMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(COVER_IMG_SIZE,COVER_IMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, COVER_IMG_SIZE, COVER_IMG_SIZE, 4, 4);    // border-radius:4px
        this->lb_albumImg->setPixmap(pixmap_painter);
    }
}

/**
 * @brief GenreInfo::slot_playAll [SLOT] 전체재생
 */
void GenreInfo::slot_playAll(){
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
 * @brief GenreInfo::slot_playShuffle [SLOT] 셔플재생
 */
void GenreInfo::slot_playShuffle(){

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
 * @brief GenreInfo::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void GenreInfo::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_FAVORITE_GET){
        setResultOfGetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_FAVORITE_DELETE){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_FAVORITE_ADD){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }

    sender()->deleteLater();
}
/**
 * @brief GenreInfo::setDataAlbumFromDB [앨범] 데이터 가져와서 UI 세팅
 */
void GenreInfo::setDataAlbumFromDB(){

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
        if(this->genre.front() == "'" || this->genre.back() == "'" || this->genre.contains("'")){
            print_debug();
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name=\"%1\")";
        }
        else{
            print_debug();
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name='%1') ";
        }
        qDebug() << "-----------------------------------------this->genre" << this->genre;
        QVariantList data_cnt;
        sqlite->exec(strQuery.arg(this->genre), data_cnt);

        int album_cnt = 0;
        if(data_cnt.size() > 0){
            QJsonObject tmp_data = data_cnt.at(0).toJsonObject();
            album_cnt = tmp_data["album_count"].toInt();
        }
        this->lb_albums->setText(tr("Album") + QString(" (%1)").arg(album_cnt));

        strQuery = "";
        strQuery =  " SELECT AI.album, AI._id AS album_id, AI.genre_name, AI.genre_id, AI.numsongs AS num_of_song, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        if(this->genre.front() == "'" || this->genre.back() == "'" || this->genre.contains("'")){
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name=\"%1\") ORDER BY AI.album LIMIT 15 ";
        }
        else{
            strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE genre_name='%1') ORDER BY AI.album LIMIT 15 ";
        }
        //strQuery += " ORDER BY AI.album LIMIT 15 ";

        QVariantList data;
        sqlite->exec(strQuery.arg(this->genre), data);

        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();
                //QJsonDocument doc(tmp_data);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " GenreInfo::setDataAlbumFromDB---tmp_data = " << strJson << "\n";

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
 * @brief ArtistInfo::setDataGenreInfo 기본정보 데이터 세팅
 * @note [참고] OptionPopup 클래스의 setData 받는 구조와 동일하다
 * @param p_data
 */
void GenreInfo::setDataGenreInfo(QJsonObject p_data){

    this->type_optionMenu = OptionPopup::TypeMenu::Music_Etc_AlbumDetailTrack;
    this->albumImg = "";
    this->list_audioInfo = QList<DataPopup*>();
    this->genre = "";
    this->infoText = "";
    this->listWidget->clear();

    int cnt = 0, LMT_CNT = 0;//cheon210525
    //int hwindow_width = global.LmtCnt - 100;//cheon210525
    //cnt = hwindow_width / 220;//cheon210525
    //LMT_CNT = cnt * 10;//cheon210525

    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

    if(p_data.contains(KEY_OP_TYPE)){
        this->type_optionMenu = static_cast<OptionPopup::TypeMenu>(p_data[KEY_OP_TYPE].toInt());
    }
    if(p_data.contains(KEY_OP_albumImg)){
        this->albumImg = p_data[KEY_OP_albumImg].toString();
    }
/*
    if(p_data.contains(KEY_OP_GENRENO)){
        this->genre_id = p_data[KEY_OP_GENRENO].toInt();
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            //strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";


            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art, A.mime_type, A.samplerate, A.bitdepth ";
            strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.genre_id=" + QString::number(this->genre_id);   //QString("%1").arg(this->genre_id);


            //strQuery += " LIMIT 100 OFFSET "+ QString::number(listWidget->count());
            strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(listWidget->count());//cheon210525

            QVariantList dataDB;
            sqlite->exec(strQuery, dataDB);
            print_debug();
            for(int i = 0; i < dataDB.size(); i++){
                QJsonObject tmp_data = dataDB.at(i).toJsonObject();
                QJsonDocument doc(tmp_data);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " GenreInfo::setDataGenreInfo---jsonData_obj = " << strJson << "\n";

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

                QString tmp_strDuration="";
                int tmp_duration = tmp_dataPopup->getDuration();
                if(tmp_duration >=0){
                    QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                    tmp_strDuration = tmp_time.toString("mm:ss");
                }

                QJsonObject json;
                json.insert("index", i);
                json.insert("fileName", tmp_dataPopup->getTitle());
                json.insert("duration", tmp_strDuration);
                json.insert("author", tmp_dataPopup->getArtist());
                json.insert("albumTitle", tmp_dataPopup->getAlbum());
                json.insert("title", tmp_dataPopup->getTitle());
                json.insert("isFile", true);
                json.insert("album_art", tmp_dataPopup->getAlbum_art());
                json.insert("album_mime", snd_mime);

                //QJsonDocument doc(json);
                //QString strJson(doc.toJson(QJsonDocument::Compact));
                //qDebug() << strJson;

                QListWidgetItem *item = new QListWidgetItem;
                item->setData(Qt::UserRole, json);
                item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
                listWidget->addItem(item);

                // ----------------------------------------------------
                //  File Download
                // ----------------------------------------------------
                FileDownloader *fileDownloader = new FileDownloader(this);
                fileDownloader->setProperty("index", i);
                fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_dataPopup->getAlbum_art()));
                connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                // ----------------------------------------------------
                //  하트 정보 요청
                // ----------------------------------------------------
                requestGetTrackFavorites(i);

            }
        }
    }
*/
    /*if(p_data.contains(KEY_OP_DATA)){
        QJsonArray tmp_arr = p_data[KEY_OP_DATA].toArray();
        for(int i=0; i<tmp_arr.count(); i++){
            DataPopup *tmp_dataPopup = new DataPopup();
            tmp_dataPopup->setJsonData(tmp_arr[i].toObject());

            this->list_audioInfo.append(tmp_dataPopup);

            QString tmp_strDuration="";
            int tmp_duration = tmp_dataPopup->getDuration();
            if(tmp_duration >=0){
                QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                tmp_strDuration = tmp_time.toString("mm:ss");
            }

            QJsonObject json;
            json.insert("index", i);
            json.insert("fileName", tmp_dataPopup->getTitle());
            json.insert("duration", tmp_strDuration);
            json.insert("author", tmp_dataPopup->getArtist());
            json.insert("albumTitle", tmp_dataPopup->getAlbum());
            json.insert("title", tmp_dataPopup->getTitle());
            json.insert("isFile", true);
            json.insert("album_art", tmp_dataPopup->getAlbum_art());

            QListWidgetItem *item = new QListWidgetItem;
            item->setData(Qt::UserRole, json);
            item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
            listWidget->addItem(item);

            // ----------------------------------------------------
            //  File Download
            // ----------------------------------------------------
            FileDownloader *fileDownloader = new FileDownloader(this);
            fileDownloader->setProperty("index", i);
            fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_dataPopup->getAlbum_art()));
            connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

            // ----------------------------------------------------
            //  하트 정보 요청
            // ----------------------------------------------------
            requestGetTrackFavorites(i);
        }
    }*/
    if(p_data.contains(KEY_OP_TITLE_MAIN)){
        genre = p_data[KEY_OP_TITLE_MAIN].toString();
    }


    // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
    emit signal_changedMenuName(genre);




    // 이미지 경로 설정
    QString tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->albumImg);
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
    this->lb_genre->setText(this->genre);
    this->lb_infoText->setText(this->infoText);

    //this->setDataTrackFromDB();
    this->setDataAlbumFromDB();

}

/**
 * @brief GenreInfo::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void GenreInfo::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){


    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = p_flagFavor;
    item->setData(Qt::UserRole, map);
}

/**
 * @brief GenreInfo::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void GenreInfo::changeTrackFavoritesUI(const int &p_index){

    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = !map["favorites"].toBool();
    item->setData(Qt::UserRole, map);
}

/**
 * @brief GenreInfo::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_index
 */
void GenreInfo::requestGetTrackFavorites(const int &p_index){

    if(!global.user.getAccess_token().isEmpty()){
        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",list_audioInfo.at(p_index)->getData());

        NetworkHttp *network = new NetworkHttp;
        network->setProperty("index", p_index);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);
    }
}

/**
 * @brief GenreInfo::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void GenreInfo::requestSetTrackFavorites(const int &p_index){
    if(!global.user.getAccess_token().isEmpty()){

        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        bool tmp_isFavor = map["favorites"].toBool();

        QJsonArray tmp_jsonArrTrack;
        QJsonObject tmp_jsonTrack = getJsonObject_TrackData(p_index);
        tmp_jsonTrack.insert("mac_address", global.device.getDeviceID());
        tmp_jsonTrack.insert("type", "music");
        tmp_jsonTrack.insert("url",list_audioInfo.at(p_index)->getData());
        tmp_jsonTrack.insert("isFavorite", tmp_isFavor);
        tmp_jsonArrTrack.append(tmp_jsonTrack);

        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",list_audioInfo.at(p_index)->getData());
        tmp_json.insert("favorite", tmp_isFavor);
        tmp_json.insert("tracks", tmp_jsonArrTrack);

        if(tmp_isFavor){
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_ADD,  QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);

            // ------------------------------------------------
            //  썸네일 업로드
            // ------------------------------------------------
            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(list_audioInfo.at(p_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, list_audioInfo.at(p_index)->getData());
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
        }else{
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);
        }
    }
}

/**
 * @brief GenreInfo::getJsonObject_OptionPopup : 옵션팝업에 사용할 데이터 반환
 * @param p_index
 * @return
 */
QJsonObject GenreInfo::getJsonObject_OptionPopup(const int &p_index){

    if(p_index >=0 && p_index < list_audioInfo.size()){
        DataPopup *data = list_audioInfo.at(p_index);

        QJsonObject tmp_data = data->getJsonData();
        QJsonArray tmp_array;
        tmp_array.append(tmp_data);

        QJsonObject tmp_jsonObject;
        tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, data->getTitle());
        tmp_jsonObject.insert(KEY_OP_TITLE_SUB, data->getArtist());
        tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);       // 강제로 넣어주는 경우 : AbstractMainContain 페이지가 아닌 다른 페이지에서 옵션팝업을 띄우는 경우
        tmp_jsonObject.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Category_Track);
        tmp_jsonObject.insert(KEY_OP_albumImg, data->getAlbum_art());
        tmp_jsonObject.insert(KEY_OP_cntStar, 0);
        tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
        tmp_jsonObject.insert("isAlbum", false);

        return tmp_jsonObject;
    }

    return  QJsonObject();
}

/**
 * @brief GenreInfo::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray GenreInfo::getJsonArr_TrackData(const int &p_index){
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
 * @brief GenreInfo::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject GenreInfo::getJsonObject_TrackData(const int &p_index){

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
        jsonThumb.insert("url", dataPopup->getAlbum_art());

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
 * @brief GenreInfo::requestShowOptionPopup : 옵션팝업 Show
 * @param p_index
 */
void GenreInfo::requestShowOptionPopup(const int &p_index){
    emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, getJsonObject_OptionPopup(p_index));
}


/**
 * @brief GenreInfo::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void GenreInfo::setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_idx, tmp_favorite);
    }
}

/**
 * @brief GenreInfo::setResultOfSetFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void GenreInfo::setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool tmp_flagOk = false;

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            tmp_flagOk = true;
        }
    }

    if(!tmp_flagOk){
        changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("You cannot change your favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}


/**
 * @brief GenreInfo::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void GenreInfo::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_add->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
        proc_add->requestAddPlayListTrack(p_playlistno);

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
void GenreInfo::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    proc_add->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
    proc_add->requestNewPlaylistTrack(p_name);
}


/**
 * @brief GenreInfo::slot_delegateClicked
 */
void GenreInfo::slot_delegateClicked(const int &p_index, const int &p_btnType){

    if(p_btnType == FileDelegate::BtnType::etc){
        emit linker->signal_clickedHoverItem(HOVER_CODE_PLAY, getJsonObject_OptionPopup(p_index));
    }
    else if(p_btnType == FileDelegate::BtnType::playListAdd){
        // 플레이리스트 담기
        proc_add->setProperty("index", p_index);
        proc_add->showDlgOfPlayList();
    }
    else if(p_btnType == FileDelegate::BtnType::trackHeart){
        // 하트 클릭
        changeTrackFavoritesUI(p_index);
        requestSetTrackFavorites(p_index);
    }
    else if(p_btnType == FileDelegate::BtnType::more){
        // 더보기 클릭
        requestShowOptionPopup(p_index);
    }

}

/**
 * @brief GenreInfo::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void GenreInfo::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index>=0 && tmp_index<listWidget->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}


void GenreInfo::slot_clickedAlbumViewAll(){
    QJsonObject jsonData;
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_VA_GENRE_ALBUMLIST);
    jsonData.insert("genre", this->genre);
    emit linker->signal_clickedMovePage(jsonData);
}

/**PAGECODE_OP_GENREINFO
 * @brief MusicList_Group::wheelEvent : 휠 이벤트
 * @param event
 */
void GenreInfo::wheelEvent(QWheelEvent *event){

#if defined(Q_OS_WINDOWS)//cheon0525
    // 더 가져오기 실행
    if(event->delta() < 0){


        /*
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            //strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art, A.mime_type, A.samplerate, A.bitdepth ";
            strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.genre_id=" + QString::number(this->genre_id); //QString("%1").arg(this->genre_id);
            strQuery += " LIMIT 100 OFFSET "+ QString::number(listWidget->count());

            QVariantList dataDB;
            sqlite->exec(strQuery, dataDB);
            for(int i = 0; i < dataDB.size(); i++){
                QJsonObject tmp_data = dataDB.at(i).toJsonObject();

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

                QString tmp_strDuration="";
                int tmp_duration = tmp_dataPopup->getDuration();
                if(tmp_duration >=0){
                    QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                    tmp_strDuration = tmp_time.toString("mm:ss");
                }

                QJsonObject json;
                json.insert("index", i);
                json.insert("fileName", tmp_dataPopup->getTitle());
                json.insert("duration", tmp_strDuration);
                json.insert("author", tmp_dataPopup->getArtist());
                json.insert("albumTitle", tmp_dataPopup->getAlbum());
                json.insert("title", tmp_dataPopup->getTitle());
                json.insert("isFile", true);
                json.insert("album_art", tmp_dataPopup->getAlbum_art());
                json.insert("album_mime", snd_mime);

                //QJsonDocument doc(json);
                //QString strJson(doc.toJson(QJsonDocument::Compact));
                //qDebug() << strJson;

                QListWidgetItem *item = new QListWidgetItem;
                item->setData(Qt::UserRole, json);
                item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
                listWidget->addItem(item);

                // ----------------------------------------------------
                //  File Download
                // ----------------------------------------------------
                FileDownloader *fileDownloader = new FileDownloader(this);
                fileDownloader->setProperty("index", i);
                fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_dataPopup->getAlbum_art()));
                connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                // ----------------------------------------------------
                //  하트 정보 요청
                // ----------------------------------------------------
                requestGetTrackFavorites(i);

            }
        }
        */
        this->setDataAlbumFromDB();
    }

#elif defined(Q_OS_MAC)//cheon0525
    // 더 가져오기 실행
     qDebug() << "event->angleDelta().ry() : " << event->angleDelta().ry();
    if(event->angleDelta().ry() <= -120){//cheon0525
        /*
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.genre_id=" + QString::number(this->genre_id); //QString("%1").arg(this->genre_id);
            strQuery += " LIMIT 100 OFFSET "+ QString::number(listWidget->count());

            QVariantList dataDB;
            sqlite->exec(strQuery, dataDB);
            this->listWidget->clear();
            for(int i = 0; i < dataDB.size(); i++){
                QJsonObject tmp_data = dataDB.at(i).toJsonObject();

                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(tmp_data);

                this->list_audioInfo.append(tmp_dataPopup);

                QString tmp_strDuration="";
                int tmp_duration = tmp_dataPopup->getDuration();
                if(tmp_duration >=0){
                    QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                    tmp_strDuration = tmp_time.toString("mm:ss");
                }

                QJsonObject json;
                json.insert("index", i);
                json.insert("fileName", tmp_dataPopup->getTitle());
                json.insert("duration", tmp_strDuration);
                json.insert("author", tmp_dataPopup->getArtist());
                json.insert("albumTitle", tmp_dataPopup->getAlbum());
                json.insert("title", tmp_dataPopup->getTitle());
                json.insert("isFile", true);
                json.insert("album_art", tmp_dataPopup->getAlbum_art());

                //QJsonDocument doc(json);
                //QString strJson(doc.toJson(QJsonDocument::Compact));
                //qDebug() << strJson;

                QListWidgetItem *item = new QListWidgetItem;
                item->setData(Qt::UserRole, json);
                item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
                listWidget->addItem(item);

                // ----------------------------------------------------
                //  File Download
                // ----------------------------------------------------
                FileDownloader *fileDownloader = new FileDownloader(this);
                fileDownloader->setProperty("index", i);
                fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_dataPopup->getAlbum_art()));
                connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                // ----------------------------------------------------
                //  하트 정보 요청
                // ----------------------------------------------------
                requestGetTrackFavorites(i);

            }
        }
        */
        this->setDataAlbumFromDB();
    }
#endif

}
