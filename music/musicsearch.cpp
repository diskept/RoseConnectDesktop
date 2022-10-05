#include "musicsearch.h"
#include "proc_uploadthumbnail.h"

#include <common/filedownloader.h>
#include <common/sqlitehelper.h>
#include <common/global.h>
#include <common/networkhttp.h>
#include <widget/VerticalScrollArea.h>
#include <widget/frameartist.h>
#include <widget/framerecentadd.h>
#include <widget/toastmsg.h>
#include <QLabel>
#include <QPushButton>
#include <QScroller>
#include <QTime>
#include <widget/optionpopup.h>

const int HTTP_FAVORITE_GET = 59;
const int HTTP_FAVORITE_ADD = 58;
const int HTTP_FAVORITE_DELETE = 57;

/**
 * @brief MusicSearch::MusicSearch : 음악 검색 화면
 * @param parent
 * @note 트랙, 아티스트, 앨범등의 검색
 */
MusicSearch::MusicSearch(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief MusicSearch::setInit : 초기 세팅
 */
void MusicSearch::setInit(){

    listDataTrack = new QList<DataPopup*>();

    proc_addPlaylist = new Proc_AddTackInPlayList(this);
    connect(proc_addPlaylist, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_addPlaylist, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    linker = Linker::getInstance();
    delegateTrack = new FileDelegate(this);
    connect(delegateTrack, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
}

/**
 * @brief MusicSearch::setUIControl : UI 세팅
 */
void MusicSearch::setUIControl(){

    QLabel *lb_empty = new QLabel(tr("No results were found for your search."));
    //QLabel *lb_empty = new QLabel(tr("검색결과가 없습니다."));
    lb_empty->setStyleSheet("font-size:18px; color:#FFFFFF;");
    lb_empty->setContentsMargins(0,0,0,0);
    lb_empty->setAlignment(Qt::AlignCenter);

    QVBoxLayout *box_empty = new QVBoxLayout();
    box_empty->setContentsMargins(0,0,0,0);
    box_empty->setSpacing(0);
    box_empty->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    box_empty->addWidget(lb_empty,0, Qt::AlignCenter);

    widgetEmpty = new QWidget();
    widgetEmpty->setLayout(box_empty);
    widgetEmpty->hide();


    boxContents = new QVBoxLayout();
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(0,0,0,0);
    boxContents->setAlignment(Qt::AlignTop);

    QWidget *widgetContents = new QWidget();
    widgetContents->setLayout(boxContents);
    widgetContents->setContentsMargins(22,0,0,22);

    setTrackUI();
    setAlbumUI();
    setArtistUI();

    scrollArea_content = new QScrollArea();
    scrollArea_content->setWidget(widgetContents);
    scrollArea_content->setWidgetResizable(true);
    scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout();
    boxMain->setContentsMargins(0,0,0,0);
    boxMain->addWidget(scrollArea_content);
    boxMain->addWidget(widgetEmpty);
    boxMain->setSizeConstraint(QLayout::SetNoConstraint);

    setLayout(boxMain);
}

/**
 * @brief MusicSearch::setTrackUI : 트랙 UI 세팅
 */
void MusicSearch::setTrackUI(){
    labelSubTitle_1 = new QLabel;//cheon211115-01
    labelSubTitle_1->setText(tr("Track"));
    //labelSubTitle_1->setText(tr("트랙"));
    labelSubTitle_1->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:5px;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_SEARCH_TRACK);

    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,0,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_1);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);
    widget_top->setFixedHeight(22);

    listWidgetTrack = new QListWidget(this);
    listWidgetTrack->setItemDelegate(delegateTrack);
    listWidgetTrack->setCursor(Qt::PointingHandCursor);
    listWidgetTrack->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidgetTrack->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidgetTrack->setStyleSheet("border:none;");
    listWidgetTrack->setFixedHeight(350);


    QVBoxLayout *box_total = new QVBoxLayout();
    box_total->setSpacing(0);
    box_total->setContentsMargins(10,0,28,0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(widget_top);
    box_total->addSpacing(5);
    box_total->addWidget(listWidgetTrack);

    widgetTrack = new QWidget(this);
    widgetTrack->setContentsMargins(0,10,0,30);
    widgetTrack->setLayout(box_total);

    boxContents->addWidget(widgetTrack);
}

/**
 * @brief MusicSearch::setAlbumUI : 앨범 UI 세팅
 */
void MusicSearch::setAlbumUI(){
    labelSubTitle_2 = new QLabel;//cheon211115-01
    labelSubTitle_2->setText(tr("Album"));//cheon211115-01
    //labelSubTitle_2->setText(tr("앨범"));
    labelSubTitle_2->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");//cheon211115-01

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_SEARCH_ALBUM);
    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,28,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_2);//cheon211115-01
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);


    boxAlbum = new QHBoxLayout;
    boxAlbum->setSpacing(20);
    boxAlbum->setContentsMargins(10,10,0,0);
    boxAlbum->setSizeConstraint(QLayout::SetFixedSize);


    QWidget *widgetAlbumList = new QWidget;
    widgetAlbumList->setLayout(boxAlbum);


    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setContentsMargins(0,0,0,0);
    scrollArea->setWidget(widgetAlbumList);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);


    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(0,0,0,5);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(scrollArea);

    widgetAlbum = new QWidget;
    widgetAlbum->setLayout(boxTotal);
    widgetAlbum->setStyleSheet("border:0px;");
    widgetAlbum->setFixedHeight(310);

    boxContents->addWidget(widgetAlbum);
}

/**
 * @brief MusicSearch::setArtist : 아티스트 UI 세팅
 */
void MusicSearch::setArtistUI(){
    labelSubTitle_3 = new QLabel;//cheon211115-01
    labelSubTitle_3->setText(tr("Artist"));//cheon211115-01
    //labelSubTitle_3->setText(tr("아티스트"));
    labelSubTitle_3->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");//cheon211115-01

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_SEARCH_ARTIST);
    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,28,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_3);//cheon211115-01
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);


    boxArtist = new QHBoxLayout;
    boxArtist->setSpacing(20);
    boxArtist->setContentsMargins(10,0,0,0);
    boxArtist->setSizeConstraint(QLayout::SetFixedSize);


    QWidget *widgetAlbumList = new QWidget;
    widgetAlbumList->setLayout(boxArtist);


    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setContentsMargins(0,0,0,0);
    scrollArea->setWidget(widgetAlbumList);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);


    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(0,0,0,5);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(scrollArea);

    widgetArtist = new QWidget;
    widgetArtist->setContentsMargins(0,15,0,0);
    widgetArtist->setLayout(boxTotal);
    widgetArtist->setStyleSheet("border:0px;");
    widgetArtist->setFixedHeight(300);

    boxContents->addWidget(widgetArtist);
}
/**
 * @brief MusicSearch::search : 검색
 * @param p_text
 */
void MusicSearch::search(const QString &p_text){//cheon211008

    QString tmpText = "";
    if(p_text.isEmpty()){
        tmpText = global.search_text;
    }
    else{
        global.search_text = p_text;//searchtextcopy
        tmpText = p_text;
    }

    strSearchWord = tmpText;
print_debug();
    emit signal_changedMenuName("'" + strSearchWord + "' " + tr("Search Result"),"Music");//cheon211008
    //emit signal_changedMenuName("'"+p_text+"' "+tr("검색결과"));

    setTrack();
    setAlbum();
    setArtist();

    showEmptyLabel(!checkSearchResult());
}


void MusicSearch::musicSearch(const QString &p_text){//cheon211008

    QString tmpText = "";
    if(p_text.isEmpty()){
        tmpText = global.search_text;
    }
    else{
        global.search_text = p_text;//searchtextcopy
        tmpText = p_text;
    }

    strSearchWord = tmpText;
print_debug();
    emit signal_changedMenuName("'" + strSearchWord + "' " + tr("Search Result"),"Music");//cheon211008
    //emit signal_changedMenuName("'"+p_text+"' "+tr("검색결과"));

    setTrack();
    setAlbum();
    setArtist();

    showEmptyLabel(!checkSearchResult());
}
/**
 * @brief MusicSearch::showEmptyLabel : empty 라벨 Show
 * @param bool if true show.
 */
void MusicSearch::showEmptyLabel(bool p_flagShow){
    if(p_flagShow){
        widgetEmpty->setVisible(true);
        scrollArea_content->setVisible(false);
    }else{
        widgetEmpty->setVisible(false);
        scrollArea_content->setVisible(true);
    }
}

/**
 * @brief MusicSearch::checkSearchResult : 검색결과 확인
 * @return bool if true, has data
 */
bool MusicSearch::checkSearchResult(){
    bool tmp_flagHasData = false;

    if(listWidgetTrack->count() > 0){
        tmp_flagHasData = true;
    }
    else if(boxAlbum->count() > 0){
        tmp_flagHasData = true;
    }
    else if(boxArtist->count() > 0){
        tmp_flagHasData = true;
    }

    return tmp_flagHasData;
}

/**
 * @brief MusicSearch::setTrack : 트랙 데이터 세팅
 */
void MusicSearch::setTrack(){

    listDataTrack->clear();
    listWidgetTrack->clear();

    QJsonArray jsonArrTrack = searchTrack();
    if(jsonArrTrack.count() > 0){//cheon211115-01
        labelSubTitle_1->setText(QString("Track (%1)").arg(jsonArrTrack.count()));
    }
    for(int i = 0 ; i < jsonArrTrack.count(); i++){

        QJsonObject jsonTrack = jsonArrTrack.at(i).toObject();

        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_type = "";

        QString tmp_mime = "";
        int tmp_sample = 0;
        int tmp_bit = 0;
        QString snd_mime = "";

        tmp_sample = jsonTrack["samplerate"].toInt();
        tmp_bit = jsonTrack["bitdepth"].toInt();

        tmp_split = jsonTrack["mime_type"].toString();
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
        jsonTrack.insert("album_mime", snd_mime);

        DataPopup *tmp_dataPopup = new DataPopup(this);
        tmp_dataPopup->setJsonData(jsonTrack);
        listDataTrack->append(tmp_dataPopup);

        QString tmp_albumImg = "";
        if(jsonTrack.contains("album_art")){
            tmp_albumImg = jsonTrack["album_art"].toString();
        }

        QString tmp_strDuration="";
        int tmp_duration = jsonTrack["duration"].toInt();
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
        json.insert("album_art", tmp_albumImg);
        json.insert("album_mime", snd_mime);

        QListWidgetItem *item = new QListWidgetItem;
        item->setData(Qt::UserRole, json);
        item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
        listWidgetTrack->addItem(item);

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", i);
        fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

        requestGetTrackFavorites(i);
    }

    if(listWidgetTrack->count() > 0){
        listWidgetTrack->setFixedHeight(listWidgetTrack->count()*70);
        //widgetTrack->setFixedHeight(445-(5-listWidgetTrack->count())*70);
        widgetTrack->show();
    }else{
        widgetTrack->hide();
    }
}

/**
 * @brief MusicSearch::setAlbum : 앨범 데이터 세팅
 */
void MusicSearch::setAlbum(){
    GSCommon::clearLayout(boxAlbum);

    QJsonArray jsonArrAlbum = searchAlbum();
    if(jsonArrAlbum.count() > 0){//cheon211115-01
        labelSubTitle_2->setText(QString("Album (%1)").arg(jsonArrAlbum.count()));
    }
    for(int i = 0 ; i < jsonArrAlbum.count(); i++){
        QJsonObject jsonAlbum = jsonArrAlbum.at(i).toObject();

        FrameRecentAdd *tmp_widget = new FrameRecentAdd;
        tmp_widget->setData(jsonAlbum);
        tmp_widget->setHover();
        boxAlbum->addWidget(tmp_widget);
    }

    if(boxAlbum->count() > 0){
        widgetAlbum->show();
    }else{
        widgetAlbum->hide();
    }
}

/**
 * @brief MusicSearch::setArtist : 아티스트 데이터 세팅
 */
void MusicSearch::setArtist(){

    GSCommon::clearLayout(boxArtist);

    QJsonArray jsonArrArtist = searchArtist();
    if(jsonArrArtist.count() > 0){//cheon211115-01
        labelSubTitle_3->setText(QString("Artist (%1)").arg(jsonArrArtist.count()));
    }
    for(int i = 0 ; i < jsonArrArtist.count(); i++){
        QJsonObject jsonArtist = jsonArrArtist.at(i).toObject();

        FrameArtist *tmp_widget = new FrameArtist;
        tmp_widget->setData(jsonArtist);
        tmp_widget->setHover();
        boxArtist->addWidget(tmp_widget);
    }

    if(boxArtist->count() > 0){
        widgetArtist->show();
    }else{
        widgetArtist->hide();
    }
}

/**
 * @brief MusicSearch::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void MusicSearch::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){

    if(p_index < listWidgetTrack->count()){
        QListWidgetItem *item = listWidgetTrack->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        map["favorites"] = p_flagFavor;
        item->setData(Qt::UserRole, map);
    }
}

/**
 * @brief MusicSearch::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void MusicSearch::changeTrackFavoritesUI(const int &p_index){

    QListWidgetItem *item = listWidgetTrack->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = !map["favorites"].toBool();
    item->setData(Qt::UserRole, map);
}

/**
 * @brief MusicSearch::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray MusicSearch::getJsonArr_TrackData(const int &p_index){
    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < listDataTrack->size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }

    return jsonArray;
}

/**
 * @brief MusicSearch::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject MusicSearch::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < listDataTrack->size()){
        DataPopup *dataPopup = listDataTrack->at(p_index);

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
 * @brief MusicSearch::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_index
 */
void MusicSearch::requestGetTrackFavorites(const int &p_index){

    if(!global.user.getAccess_token().isEmpty()){
        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",listDataTrack->at(p_index)->getData());

        NetworkHttp *network = new NetworkHttp;
        network->setProperty("index", p_index);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);
    }
}

/**
 * @brief MusicSearch::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void MusicSearch::requestSetTrackFavorites(const int &p_index){
    if(!global.user.getAccess_token().isEmpty()){

        QListWidgetItem *item = listWidgetTrack->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        bool tmp_isFavor = map["favorites"].toBool();

        QJsonArray tmp_jsonArrTrack;
        QJsonObject tmp_jsonTrack = getJsonObject_TrackData(p_index);
        tmp_jsonTrack.insert("mac_address", global.device.getDeviceID());
        tmp_jsonTrack.insert("type", "music");
        tmp_jsonTrack.insert("url",listDataTrack->at(p_index)->getData());
        tmp_jsonTrack.insert("isFavorite", tmp_isFavor);
        tmp_jsonArrTrack.append(tmp_jsonTrack);

        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",listDataTrack->at(p_index)->getData());
        tmp_json.insert("favorite", tmp_isFavor);
        tmp_json.insert("tracks", tmp_jsonArrTrack);

        if(tmp_isFavor){
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_ADD,  QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);

            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(listDataTrack->at(p_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, listDataTrack->at(p_index)->getData());
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
 * @brief MusicSearch::getJsonObject_OptionPopup : 옵션팝업에 사용할 데이터 반환
 * @param p_index
 * @return
 */
QJsonObject MusicSearch::getJsonObject_OptionPopup(const int &p_index){
    DataPopup *data = listDataTrack->at(p_index);

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

/**
 * @brief MusicSearch::requestPlayMusic : 음악 재생
 * @param p_index
 */
void MusicSearch::requestPlayMusic(const int &p_index){
    emit linker->signal_clickedHoverItem(HOVER_CODE_PLAY, getJsonObject_OptionPopup(p_index));
}

/**
 * @brief MusicSearch::requestShowOptionPopup : 옵션팝업 Show
 * @param p_index
 */
void MusicSearch::requestShowOptionPopup(const int &p_index){
    emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, getJsonObject_OptionPopup(p_index));
}

/**
 * @brief MusicSearch::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void MusicSearch::setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_idx, tmp_favorite);
    }
}

/**
 * @brief MusicSearch::setResultOfSetFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void MusicSearch::setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData){
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
 * @brief MusicSearch::searchTrack : 트랙 조회
 * @return
 */
QJsonArray MusicSearch::searchTrack(){

    QJsonArray jsonArr;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        //strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data ";
        strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";
        strQuery += " WHERE artist LIKE '%"+strSearchWord+"%' OR title LIKE '%"+strSearchWord+"%'";
        strQuery += " ORDER BY A.title ";
        strQuery += " LIMIT 5";

        QVariantList data;
        sqlite->exec(strQuery, data);

        for(int i = 0 ; i < data.size(); i++){
            jsonArr.append(data.at(i).toJsonObject());
        }

    }

    sqlite->close();
    delete sqlite;

    return jsonArr;
}

/**
 * @brief MusicSearch::searchAlbum : 앨범 조회
 * @return
 */
QJsonArray MusicSearch::searchAlbum(){
    QJsonArray jsonArr;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        //strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AA._data AS album_art  ";
        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate AS sample_rate, AI.bitdepth AS bit_depth, AA._data AS album_art  ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        strQuery += " WHERE AI.album LIKE '%"+strSearchWord+"%' OR AI.artist LIKE '%"+strSearchWord+"%'";
        strQuery += " ORDER BY AI.album ";
        strQuery += " LIMIT 10";

        QVariantList data;
        sqlite->exec(strQuery, data);

        for(int i = 0 ; i < data.size(); i++){
            QJsonObject jsonData = data.at(i).toJsonObject();
            jsonData.insert("section", "search_album");

            jsonArr.append(jsonData);
        }

    }

    sqlite->close();
    delete sqlite;

    return jsonArr;
}

/**
 * @brief MusicSearch::searchArtist : 아티스트 조회
 * @return
 */
QJsonArray MusicSearch::searchArtist(){

    QJsonArray jsonArr;

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){

        QString strQuery = "";
        //strQuery = " SELECT AI._id, AI.artist, AI.number_of_tracks, ";
        strQuery = " SELECT AI._id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
        strQuery += " WHERE AI.artist LIKE '%"+strSearchWord+"%'";
        strQuery += " GROUP BY AI._id ";
        strQuery += " LIMIT 10";

        QVariantList data;
        sqlite->exec(strQuery, data);

        for(int i = 0 ; i < data.size(); i++){
            jsonArr.append(data.at(i).toJsonObject());
        }
    }
    sqlite->close();
    delete sqlite;

    return jsonArr;
}

/**
 * @brief MusicSearch::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void MusicSearch::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index < listWidgetTrack->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            listWidgetTrack->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}

/**
 * @brief MusicSearch::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicSearch::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

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
 * @brief MusicSearch::slot_delegateClicked
 */
void MusicSearch::slot_delegateClicked(const int &p_index, const int &p_btnType){

    if(p_btnType == FileDelegate::BtnType::etc){
        requestPlayMusic(p_index);
    }
    else if(p_btnType == FileDelegate::BtnType::playListAdd){
        // 플레이리스트 담기
        proc_addPlaylist->setProperty("index", p_index);
        proc_addPlaylist->showDlgOfPlayList();
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
 * @brief MusicSearch::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void MusicSearch::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
        proc_addPlaylist->requestAddPlayListTrack(p_playlistno);

        if(tmp_index >=0 && tmp_index < listDataTrack->size()){
            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(listDataTrack->at(tmp_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, listDataTrack->at(tmp_index)->getData());
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
        }

    }else{
        proc_addPlaylist->showDlgOfNewPlayList();
    }
}

/**
 * @brief MusicSearch::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void MusicSearch::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
    proc_addPlaylist->requestNewPlaylistTrack(p_name);
}

/**
 * @brief MusicSearch::slot_clickedViewAll : [슬롯] View All 클릭
 */
void MusicSearch::slot_clickedViewAll(){
    emit this->signal_clickedViewAll(strSearchWord, sender()->property(KEY_VIEWALL_MODE.toStdString().c_str()).toString());
}
