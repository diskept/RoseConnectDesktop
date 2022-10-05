#include "widget/framerecentalbum.h"
#include "widget/optionpopup.h"

#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "common/sqlitehelper.h"

#include <QJsonArray>


const int HTTP_NETWORK_ALBUM_FETCH = 0;
const int HTTP_NETWORK_ALBUM_FETCH_MORE = 1;

/**
 * @brief FrameRecentAlbum::FrameRecentAlbum : 음악-홈-최근재생앨범 프레임 위젯
 * @param parent
 * @note 트랙 구조에서 앨범 구조로 변경
 */
FrameRecentAlbum::FrameRecentAlbum(QWidget *parent)
    : FrameWidget(Type::RecentAlbum, parent)
{
    setInit();
    setUIControl();
}

FrameRecentAlbum::FrameRecentAlbum(bool flagIsViewAllSize, QWidget *parent)
    : FrameWidget(Type::RecentAlbum, parent)
    , flagIsViewAllSize(flagIsViewAllSize)
{
    setInit();
    setUIControl();
}

FrameRecentAlbum::~FrameRecentAlbum(){

}

/**
 * @brief FrameWidget::setInit : [override] 초기 세팅
 */
void FrameRecentAlbum::setInit(){
    linker = Linker::getInstance();
    data = new DataAlbum(this);
    dataPopup = new DataPopup(this);
}

/**
 * @brief FrameWidget::setUIControl : [override] UI 세팅
 */
void FrameRecentAlbum::setUIControl(){

    /*
    // 앨범 타이틀
    lb_title = new QLabel;
    lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;margin-top:15px;");
    lb_title->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);

    // 앨범 아티스트
    lb_artist = new QLabel;
    lb_artist->setStyleSheet("font-size:16px;color:#999999;margin-top:5px;");
    lb_artist->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);

    // 앨범 이미지
    lb_img = new ImageLabel;
    lb_img->setType(flagIsViewAllSize ? ImageLabel::Type::MusicCategory : ImageLabel::Type::Album);
    lb_img->setHidden_favorite(true);

    // ---------------------------------------------------------
    //  박스 구성 및 레이아웃 세팅
    // ---------------------------------------------------------
    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    boxLayout->addWidget(lb_title);
    boxLayout->addWidget(lb_artist);

    this->setLayout(boxLayout);*/

    int base_height = 0;
    int img_width = 0;

    QWidget *label_base = new QWidget();

    // 앨범 이미지
    this->lb_img = new ImageLabel();
    this->lb_img->setType(flagIsViewAllSize ? ImageLabel::Type::MusicCategory : ImageLabel::Type::Album);
    this->lb_img->setHidden_favorite(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS * 3;
    label_base->setFixedSize(img_width, base_height);

    // 앨범 타이틀
    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    // 앨범 아티스트
    this->lb_artist = new QLabel(label_base);
    this->lb_artist->setStyleSheet("font-size:16px;color:#999999;");
    this->lb_artist->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);

    // ---------------------------------------------------------
    //  박스 구성 및 레이아웃 세팅
    // ---------------------------------------------------------
    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(this->lb_img);
    boxLayout->addWidget(label_base);
    boxLayout->setAlignment(Qt::AlignTop);

    this->setLayout(boxLayout);

    // ---------------------------------------------------------
    //  Connect
    // ---------------------------------------------------------
    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}

/**
 * @brief FrameRecentAlbum::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameRecentAlbum::setData(const QJsonObject &p_jsonObject){

    if(p_jsonObject.contains("clientKey")){
        QString albumKey = ProcJsonEasy::getString(p_jsonObject, "clientKey").toUtf8();

        QJsonObject tmpdata;
        tmpdata.insert("album", ProcJsonEasy::getString(p_jsonObject, "title"));
        tmpdata.insert("favorite", ProcJsonEasy::getBool(p_jsonObject, "favorite") == true ? 1 : 0);
        tmpdata.insert("album_art", ProcJsonEasy::getString(p_jsonObject, "thumbnail"));

        this->data->setData(tmpdata);

        this->recv_albumData = QJsonObject();
        this->recv_albumData = p_jsonObject;
    }
    else if(p_jsonObject.contains("album")){
        QJsonObject album = ProcJsonEasy::getJsonObject(p_jsonObject, "album");

        QString albumKey = ProcJsonEasy::getString(album, "clientKey").toUtf8();

        QJsonObject tmpdata;
        tmpdata.insert("album", ProcJsonEasy::getString(album, "title"));
        tmpdata.insert("favorite", ProcJsonEasy::getBool(album, "favorite") == true ? 1 : 0);
        tmpdata.insert("album_art", ProcJsonEasy::getString(album, "thumbnail"));

        this->data->setData(tmpdata);

        this->recv_albumData = QJsonObject();
        this->recv_albumData = album;
    }
    else{
        this->data->setData(p_jsonObject);
    }


    // ---------------------------------------
    //  API에서 album_art 키가 누락되는 경우가 있음
    // ---------------------------------------
    if(data->getAlbum_art().isEmpty()){
        setAlbumArt();
    }
}


/**
 * @brief FrameWidget::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameRecentAlbum::getDataForPopup(){

    // -------------------------------------------
    //  트랙 구조에서 앨범 구조로 변경
    // -------------------------------------------
    //QJsonArray jsonArrAlbum;
    //jsonArrAlbum.append(dataPopup->getJsonData());

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data->getAlbum());
    jsonData.insert(KEY_OP_TITLE_SUB, data->getArtist());
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_RecentAlbum);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,getDataForPlayMusic());
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
    jsonData.insert("isAlbum", true);

    return jsonData;
}
/**
 * @brief FrameWidget::setHover : [override] 호버 세팅
 */
void FrameRecentAlbum::setHover(){

    lb_img->setHover2();
}

/**
 * @brief FrameRecentAlbum::getAlbumArt : 앨범 커버이미지 세팅
 * @return
 */
void FrameRecentAlbum::setAlbumArt(){
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT _data FROM album_art WHERE album_id='%1'";

        QVariantList dataDB;
        sqlite->exec(strQuery.arg(data->getAlbum_id()), dataDB);
        if(dataDB.size() > 0){
            data->setAlbum_art(dataDB.at(0).toMap()["_data"].toString());
        }
    }

    sqlite->close();
    delete sqlite;
}

/**
 * @brief FrameRecentAlbum::getDataForPlayMusic : 바로듣기에 필요한 데이터 반환
 * @return QJsonArray Track info
 * @note 플레이리스트 담기와 JsonObject 구조 다름
 */
QJsonArray FrameRecentAlbum::getDataForPlayMusic(){

    QJsonArray jsonArray;

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id WHERE A.album_id=%1 ORDER BY A.track ";

        QVariantList dataDB;
        sqlite->exec(strQuery.arg(data->getAlbum_id()), dataDB);
        if(dataDB.size() > 0){
            for(int i = 0 ; i < dataDB.size(); i++){
                DataPopup *tmp_dataPopup = new DataPopup(this);
                tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                jsonArray.append(tmp_dataPopup->getJsonData());
            }
        }
    }

    sqlite->close();
    delete sqlite;

    return jsonArray;
}

/**
 * @brief FrameRecentAlbum::getDataForAddInPlaylist : 플레이리스트 담기에 필요한 데이터 반환
 * @return QJsonArray Track info
 * @note 재생과 JsonObject 구조 다름.
 */
QJsonArray FrameRecentAlbum::getDataForAddInPlaylist(){
    QJsonArray jsonArray;

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT album, album_id, _id AS id, _data AS data, title, artist, duration ";
        strQuery += " FROM audio WHERE album_id=%1 ORDER BY track ";

        QVariantList dataDB;
        sqlite->exec(strQuery.arg(data->getAlbum_id()), dataDB);
        if(dataDB.size() > 0){
            for(int i = 0 ; i < dataDB.size(); i++){
                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                QJsonObject jsonTrack;
                // 앨범
                jsonTrack.insert("album", tmp_dataPopup->getAlbum());

                // 아티스트
                QJsonObject jsonArtist;
                jsonArtist.insert("name", tmp_dataPopup->getArtist());
                QJsonArray jsonArrArtists;
                jsonArrArtists.append(jsonArtist);
                jsonTrack.insert("artists", jsonArrArtists);

                // 클라이언트 아이디 = 오디오 아이디
                jsonTrack.insert("client_id", tmp_dataPopup->getId());

                // 썸네일
                QJsonObject jsonThumb;
                jsonThumb.insert("url", data->getAlbum_art());

                QJsonArray jsonArrThumbs;
                jsonArrThumbs.append(jsonThumb);
                jsonTrack.insert("thumbnails", jsonArrThumbs);

                // 타이틀
                jsonTrack.insert("title", tmp_dataPopup->getTitle());

                // 재생 URL
                jsonTrack.insert("url", tmp_dataPopup->getData());

                jsonTrack.insert("isFavorite", false);
                jsonTrack.insert("isSelected", false);
                jsonTrack.insert("mac_address", global.device.getDeviceID());
                jsonTrack.insert("type","music");

                jsonArray.append(jsonTrack);

            }
        }
    }

    sqlite->close();
    delete sqlite;

    return  jsonArray;
}

/**
 * @brief FrameRecentAlbum::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameRecentAlbum::slot_clickedHover(const QString &p_code){

    QJsonObject jsonForPopup = getDataForPopup();

    if(p_code == HOVER_CODE_MORE){
        jsonForPopup.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, getDataForAddInPlaylist());

        emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
    }
    else if(p_code == HOVER_CODE_PLAY){

        QJsonObject tmpJson = ProcJsonEasy::getJsonObject(this->recv_albumData, "data");
        this->album_id = ProcJsonEasy::getInt(this->recv_albumData, "id");

        // Playlsit 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject tmp_json;
        network->request(HTTP_NETWORK_ALBUM_FETCH, QString("%1/member/album/%2?page=%3&size=20")
                         .arg(global.legacy_v1).arg(this->album_id).arg(this->pageCNT), tmp_json, false, true);
    }
    else{
        emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
    }
}


void FrameRecentAlbum::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";

    if(p_id == HTTP_NETWORK_ALBUM_FETCH){
        if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
            if(p_jsonData.contains("album")){
                QJsonObject album_obj = ProcJsonEasy::getJsonObject(p_jsonData, "album");

                track_count = ProcJsonEasy::getInt(album_obj, "trackCount");

                if(album_obj.contains("tracks")){
                    this->jsonArr_Play = QJsonArray();
                    //this->jsonArr_Play = ProcJsonEasy::getJsonArray(album_obj, "tracks");
                    ProcJsonEasy::mergeJsonArray(this->jsonArr_Play, ProcJsonEasy::getJsonArray(album_obj, "tracks"));
                }

                if(track_count > this->jsonArr_Play.size()){
                    this->pageCNT++;
                    NetworkHttp *network = new NetworkHttp(this);
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QJsonObject tmp_json;
                    network->request(HTTP_NETWORK_ALBUM_FETCH_MORE, QString("%1/member/album/%2?page=%3&size=20")
                                     .arg(global.legacy_v1).arg(this->album_id).arg(this->pageCNT), tmp_json, false, true);
                }
                else{
                    // Rose Play 요청
                    roseHome::AlbumItemData data_album;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byAlbum(data_album, this->jsonArr_Play, 0, OptMorePopup::ClickMode::Play_RightNow,  roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
                }
            }
        }
    }
    else if(p_id == HTTP_NETWORK_ALBUM_FETCH_MORE){
        if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
            if(p_jsonData.contains("album")){
                QJsonObject album_obj = ProcJsonEasy::getJsonObject(p_jsonData, "album");

                track_count = ProcJsonEasy::getInt(album_obj, "trackCount");

                if(album_obj.contains("tracks")){
                    //this->jsonArr_Play = ProcJsonEasy::getJsonArray(album_obj, "tracks");
                    ProcJsonEasy::mergeJsonArray(this->jsonArr_Play, ProcJsonEasy::getJsonArray(album_obj, "tracks"));
                }

                if(track_count > this->jsonArr_Play.size()){
                    this->pageCNT++;
                    NetworkHttp *network = new NetworkHttp(this);
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QJsonObject tmp_json;
                    network->request(HTTP_NETWORK_ALBUM_FETCH_MORE, QString("%1/member/album/%2?page=%3&size=20")
                                     .arg(global.legacy_v1).arg(this->album_id).arg(this->pageCNT), tmp_json, false, true);
                }
                else{
                    // Rose Play 요청
                    roseHome::AlbumItemData data_album;
                    roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byAlbum(data_album, this->jsonArr_Play, 0, OptMorePopup::ClickMode::Play_RightNow,  roseHome::ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
                }
            }
        }
    }

    sender()->deleteLater();
}


/**
 * @brief FrameRecentAlbum::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 앨범 상세보기 페이지로 이동
 * 옵션 팝업의 signal OptionPopup:signal_clickedMovePage() 와 동일 구조
 * @see OptionPopup:slot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameRecentAlbum::slot_clickedItem(){
    /*QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);
    emit linker->signal_clickedMovePage(jsonData);*/

    QJsonObject tmpJson = ProcJsonEasy::getJsonObject(this->recv_albumData, "data");

    QJsonObject jsonData;
    jsonData.insert("id", ProcJsonEasy::getInt(this->recv_albumData, "id"));
    jsonData.insert("star", ProcJsonEasy::getInt(this->recv_albumData, "star"));
    jsonData.insert("type", ProcJsonEasy::getString(this->recv_albumData, "type"));
    jsonData.insert("title", ProcJsonEasy::getString(this->recv_albumData, "title"));
    jsonData.insert("thumbnail", ProcJsonEasy::getString(this->recv_albumData, "thumbnail"));
    jsonData.insert("playTime", ProcJsonEasy::getString(this->recv_albumData, "playTime"));
    jsonData.insert("clientKey", ProcJsonEasy::getString(this->recv_albumData, "clientKey"));
    jsonData.insert("trackId", ProcJsonEasy::getInt(tmpJson, "trackId"));
    jsonData.insert("macAddress", ProcJsonEasy::getString(tmpJson, "macAddress"));

    jsonData.insert("favorite", ProcJsonEasy::getBool(tmpJson, "favorite"));

    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTALBUMINFO);

    emit linker->signal_clickedMovePage(jsonData);
}

/**
 * @brief FrameRecentAlbum::paintEvent : 페인트 이벤트
 * @details 이미지, 라벨명, 아티스트 세팅
 * @param event
 */
void FrameRecentAlbum::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        if(global.flagConnected){
            if(data->getAlbum_art().contains("https")){
                 lb_img->setImageUrl(data->getAlbum_art());
            }
            else{
                lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(data->getAlbum_art()));
            }
        }

        int all_width = flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH;
        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setText(data->getAlbum());
        //qDebug() << tmp_wordwrap->sizeHint().width();

        if(tmp_wordwrap->sizeHint().width() > (all_width)){
            if(data->getAlbum().contains(" ")){
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, (this->LABEL_HEIGHT * 2));
                this->lb_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);

                tmp_wordwrap->clear();
                tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
                tmp_wordwrap->setWordWrap(true);
                tmp_wordwrap->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), (all_width * 2)-30, this->lb_title->font()));

                if(tmp_wordwrap->sizeHint().height() > this->LABEL_HEIGHT * 2){
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), (all_width * 2)-80, this->lb_title->font()));
                }
                else{
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), (all_width * 2)-30, this->lb_title->font()));
                }
            }
            else{
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
                this->lb_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), all_width, lb_title->font()));//cheon-210708-album
            }
        }
        else if(tmp_wordwrap->sizeHint().width() == (all_width)){
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
            this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(data->getAlbum());
        }
        else {
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
            this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), all_width, lb_title->font()));//cheon-210708-album
        }

        this->lb_artist->setText(GSCommon::getTextCutFromLabelWidth(data->getArtist(), all_width, lb_artist->font()));
    }
}

