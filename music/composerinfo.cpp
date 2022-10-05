#include "composerinfo.h"
#include "proc_uploadthumbnail.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"
#include "widget/toastmsg.h"

#include <QPainter>
#include <QSettings>

const int COVER_IMG_SIZE = 300;
const int HTTP_PLAY_ALL = 99;
const int HTTP_PLAY_SHUFFLE = 98;
const int HTTP_FAVORITE_GET = 59;
const int HTTP_FAVORITE_ADD = 58;
const int HTTP_FAVORITE_DELETE = 57;

/**
 * @brief ComposerInfo::ComposerInfo : 음악-분류-작곡가의 상세 화면
 * @param parent
 */
ComposerInfo::ComposerInfo(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}


ComposerInfo::~ComposerInfo(){

    this->fileDownLoader->deleteLater();
    this->deleteLater();
}


/**
 * @brief ComposerInfo::setInit : 초기 세팅
 */
void ComposerInfo::setInit(){
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
 * @brief ComposerInfo::setUIControl : UI 세팅
 */
void ComposerInfo::setUIControl(){
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


    // 하단 곡 리스트 / 앨범 리스트
    lb_tracks = new QLabel();
    lb_tracks->setText(tr("Song"));
    //lb_tracks->setText(tr("곡"));
    lb_tracks->setStyleSheet("font-size:20px;color:#FFFFFF;");

    QHBoxLayout *box_albums = new QHBoxLayout;
    box_albums->setSpacing(0);
    box_albums->setContentsMargins(0,0,0,0);
    box_albums->setAlignment(Qt::AlignLeft);
    box_albums->addWidget(lb_tracks);


    QWidget *widget_albums = new QWidget;
    widget_albums->setLayout(box_albums);
    widget_albums->setFixedHeight(60);
    widget_albums->setStyleSheet("background-color:#333333;border:0px;margin-bottom:0px;");

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
    vl_total->addSpacing(30);
    vl_total->addWidget(widget_albums);
    vl_total->addSpacing(10);
    vl_total->addWidget(listWidget);

    setLayout(vl_total);

    // 커넥션
    connect(btn_icon_list_allplay, &QPushButton::clicked, this, &ComposerInfo::slot_playAll);
    connect(btn_icon_list_shuffle, &QPushButton::clicked, this, &ComposerInfo::slot_playShuffle);
}

/**
 * @brief ComposerInfo::loadImage [SLOT] 앨범 이미지 세팅
 */
void ComposerInfo::slot_loadImage()
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
 * @brief ComposerInfo::slot_playAll [SLOT] 전체재생
 */
void ComposerInfo::slot_playAll(){
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
 * @brief ComposerInfo::slot_playShuffle [SLOT] 셔플재생
 */
void ComposerInfo::slot_playShuffle(){

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
 * @brief ComposerInfo::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void ComposerInfo::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

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
 * @brief ComposerInfo::setDataComposerInfo 기본정보 데이터 세팅
 * @note [참고] OptionPopup 클래스의 setData 받는 구조와 동일하다
 * @param p_data
 */
void ComposerInfo::setDataComposerInfo(QJsonObject p_data){

    this->type_optionMenu = OptionPopup::TypeMenu::Music_Etc_AlbumDetailTrack;
    this->albumImg = "";
    this->list_audioInfo = QList<DataPopup*>();
    this->composer = "";
    this->infoText = "";
    this->listWidget->clear();

    if(p_data.contains(KEY_OP_TYPE)){
        this->type_optionMenu = static_cast<OptionPopup::TypeMenu>(p_data[KEY_OP_TYPE].toInt());
    }
    if(p_data.contains(KEY_OP_albumImg)){
        this->albumImg = p_data[KEY_OP_albumImg].toString();
    }
    if(p_data.contains(KEY_OP_DATA)){
        QJsonArray tmp_arr = p_data[KEY_OP_DATA].toArray();
        lb_tracks->setText(QString(tr("Song(%1)")).arg(tmp_arr.count()));
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
    }
    if(p_data.contains(KEY_OP_TITLE_MAIN)){
        composer = p_data[KEY_OP_TITLE_MAIN].toString();
    }


    // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
    emit signal_changedMenuName(composer);


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
    this->lb_genre->setText(this->composer);
    this->lb_infoText->setText(this->infoText);

}


/**
 * @brief ComposerInfo::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void ComposerInfo::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){


    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = p_flagFavor;
    item->setData(Qt::UserRole, map);
}

/**
 * @brief ComposerInfo::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void ComposerInfo::changeTrackFavoritesUI(const int &p_index){

    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = !map["favorites"].toBool();
    item->setData(Qt::UserRole, map);
}

/**
 * @brief ComposerInfo::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_index
 */
void ComposerInfo::requestGetTrackFavorites(const int &p_index){

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
 * @brief ComposerInfo::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void ComposerInfo::requestSetTrackFavorites(const int &p_index){
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
 * @brief ComposerInfo::getJsonObject_OptionPopup : 옵션팝업에 사용할 데이터 반환
 * @param p_index
 * @return
 */
QJsonObject ComposerInfo::getJsonObject_OptionPopup(const int &p_index){

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
 * @brief ComposerInfo::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray ComposerInfo::getJsonArr_TrackData(const int &p_index){
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
 * @brief ComposerInfo::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject ComposerInfo::getJsonObject_TrackData(const int &p_index){

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
 * @brief ComposerInfo::requestShowOptionPopup : 옵션팝업 Show
 * @param p_index
 */
void ComposerInfo::requestShowOptionPopup(const int &p_index){
    emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, getJsonObject_OptionPopup(p_index));
}


/**
 * @brief ComposerInfo::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void ComposerInfo::setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_idx, tmp_favorite);
    }
}

/**
 * @brief ComposerInfo::setResultOfSetFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void ComposerInfo::setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData){
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
 * @brief ComposerInfo::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void ComposerInfo::slot_playlistClicked(const int &p_playlistno){

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
 * @brief ComposerInfo::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void ComposerInfo::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    proc_add->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
    proc_add->requestNewPlaylistTrack(p_name);
}


/**
 * @brief ComposerInfo::slot_delegateClicked
 */
void ComposerInfo::slot_delegateClicked(const int &p_index, const int &p_btnType){

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
 * @brief ComposerInfo::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void ComposerInfo::slot_thumbnailDownloaded(){

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
