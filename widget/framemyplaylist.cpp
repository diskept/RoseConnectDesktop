#include "framemyplaylist.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"
#include "widget/optionpopup.h"

#include <QJsonArray>

const int HTTP_MUSIC_INFO = 99;

/**
 * @brief FrameMyPlayList::FrameMyPlayList : MY 플레이리스트 프레임
 * @details My 플레이리스트와 구조 동일
 *
 * @note
 * 사용 API : MY 플레이리스트 V3_LEGACY_BETA/legacy/v3/playlist
 *         : 플레이리스트 트랙 MOD_LAGACY_BETA/playlist/track/fetch
 * @param parent
 */
FrameMyPlayList::FrameMyPlayList(QWidget *parent)
    : FrameWidget(Type::MyPlayList, parent)
{
    setInit();
    setUIControl();
}

FrameMyPlayList::FrameMyPlayList(bool flagIsViewAllSize, QWidget *parent)
    : FrameWidget(Type::AddedAlbum, parent)
    , flagIsViewAllSize(flagIsViewAllSize)
{
    setInit();
    setUIControl();
}

/**
 * @brief FrameMyPlayList::~FrameMyPlayList : 소멸자
 */
FrameMyPlayList::~FrameMyPlayList(){
    //dataPopupList->clear();
}

/**
 * @brief FrameWidget::setInit : [override] 초기 세팅
 */
void FrameMyPlayList::setInit(){
    linker = Linker::getInstance();
    //data = new DataPlayList(this);
    dataPlayList = new Data_M_PlayList(this);
    //dataPopupList = new QList<DataPopup*>();
}

/**
 * @brief FrameWidget::setUIControl : [override] UI 세팅
 */
void FrameMyPlayList::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    QWidget *Myplaylist = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(flagIsViewAllSize ? ImageLabel::Type::playListMy_C :ImageLabel::Type::PlayListMy);
    this->lb_img->setHidden_favorite(true);
    this->lb_img->setGeometry(0, 0, flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH, flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 2 + this->SPACE_LABELS * 2;
    Myplaylist->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(Myplaylist);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    //boxLayout->addWidget(lb_title);
    boxLayout->addWidget(Myplaylist);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}



/**
 * @brief FrameMyPlayList::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameMyPlayList::setData(const QJsonObject &p_jsonObject){

    dataPlayList->setData(p_jsonObject);  // playListNo 세팅함..
    lb_img->setFavorStatus(dataPlayList->getFavorites());
    requestMusic();
}

void FrameMyPlayList::requestMusic(){

    listPlayUrl.clear();
    //dataPopupList->clear();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(global.device.getDeviceIP()!="" && global.device.getDeviceID()!=""){
        QJsonObject tmp_json;
        tmp_json.insert("local_ip", global.device.getDeviceIP());
        tmp_json.insert("mac_address", global.device.getDeviceID());
        network->request(HTTP_MUSIC_INFO, QString("%1/playlist/%2?page=0&size=20").arg(global.legacy_v3_api).arg(dataPlayList->getNo()), tmp_json, true,true);
    }
}

/**
 * @brief FrameMyPlayList::setResultOfMusicInfoNew Track 정보 요청 결과 처리
 * @param p_jsonObject
 */
void FrameMyPlayList::setResultOfMusicInfoNew(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains("playlist")){
            QJsonObject tmp_data = p_jsonObject["playlist"].toObject();
            tmp_data.insert("isFavorite", true);    // 항상 true 로 강제 세팅. 현재 API 는 false 로 오는것이 존재함.
            dataPlayList->setData(tmp_data);
        }
    }
}
/**
 * @brief FrameMyPlayList::setResultOfMusicInfo : 플레이리스트 track 정보요청 결과 처리
 * @param p_jsonObject
 */
void FrameMyPlayList::setResultOfMusicInfo(const QJsonObject &p_jsonObject){
    const QString jsonKey_tracks = "tracks";
    const QString jsonKey_playurl = "playurl";

    if(p_jsonObject.contains(jsonKey_tracks)){
        QJsonArray jsonArrTack = p_jsonObject[jsonKey_tracks].toArray();

        for(int i = 0 ; i < jsonArrTack.size(); i++){
            QJsonObject jsonTrack = jsonArrTack.at(i).toObject();

            if(jsonTrack.contains(jsonKey_playurl)){
                QString playurl = jsonTrack[jsonKey_playurl].toString();

                listPlayUrl.append(playurl);
            }
        }
    }
}

/**
 * @brief FrameMyPlayList::getTrackInfoFromDB : Track 정보 가져오기 FROM DB
 */
void FrameMyPlayList::getTrackInfoFromDB(){

    /*
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){
        //QString strQuery = " SELECT DISTINCT * FROM audio WHERE _data IN ('"+listPlayUrl.join("','")+"')";
        QString strQuery = "";
        strQuery += " SELECT DISTINCT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        strQuery += " WHERE A._data IN ('"+listPlayUrl.join("','")+"') ";
        QVariantList data;
        sqlite->exec(strQuery, data);

        dataPopupList->clear();
        for(int i = 0 ; i < listPlayUrl.size(); i++){
            for(int k = 0; k < data.size(); k++){
                //if(listPlayUrl.at(i)==data.at(k).toMap()["_data"].toString()){
                if(listPlayUrl.at(i)==data.at(k).toMap()["data"].toString()){
                    DataPopup *tmp_dataPopup = new DataPopup(this);
                    tmp_dataPopup->setJsonData(data.at(i).toJsonObject());
                    dataPopupList->append(tmp_dataPopup);
                    //setPopupData(data.at(k).toMap());
                    //data.removeAt(k);
                    break;
                }
            }
        }
    }
    sqlite->close();
    delete sqlite;
    */
}

/**
 * @brief FrameMyPlayList::setPopupData::팝업용 데이터 세팅
 * @param p_map
 */
void FrameMyPlayList::setPopupData(QMap<QString, QVariant> p_map){
    Q_UNUSED(p_map);
    /*
    DataPopup *dataPopup = new DataPopup(this);

    dataPopup->setId(p_map["_id"].toString());
    dataPopup->setData(p_map["_data"].toString());
    dataPopup->setAlbum(p_map["album"].toString());
    dataPopup->setTitle(p_map["title"].toString());
    dataPopup->setArtist(p_map["artist"].toString());
    dataPopup->setAlbum_id(p_map["album_id"].toInt());

    dataPopupList->append(dataPopup);
    */
}

/**
 * @brief FrameMyPlayList::getPlayList : 재생을 위한 트랙 정보를 JSONArray로 반환
 * @param p_index int if < 0 all list
 * @return
 * @note 로즈튜브 재생 관련 필수값 전달받은 내용 없음<br>
 * App 디버깅 내용및 음악 재생 API 참고하여 구성함.
 */
QJsonArray FrameMyPlayList::getPlayList(int p_index){
    QJsonArray jsonArr;

    if(p_index >= 0){
        QJsonObject jsonTracks = this->dataPlayList->getJsonDataTrackForOptionPopup(p_index);
        // -----------------------------------------------------------------------
        //  마이 플레이리스트 재생 PARAM APP과 동일하게 변경
        //  기존 name : 음원 타이틀,  변경 :: 파일명
        //  기존 path : /mnt/~~~~/파일명, 변경 :: http://127.0.0.1:7080/mnt/~~~~/파일명
        // -----------------------------------------------------------------------
        QStringList tmp_strListPath = jsonTracks["path"].toString().split("/");
        QString tmp_path;
        for(int i= 0 ; i < tmp_strListPath.count(); i++){
            if(i < tmp_strListPath.count()-1){
                tmp_path += tmp_strListPath.at(i)+"/";
            }
        }
        jsonTracks.insert("ip", "?"+jsonTracks["ip"].toString());
        jsonTracks.insert("name", tmp_strListPath.last());
        jsonTracks.insert("path", "http://127.0.0.1:7080"+tmp_path);
        jsonArr.append(jsonTracks);
    }
    else{
        for(int i = 0 ; i < this->dataPlayList->getTrackList().count(); i++){
            QJsonObject jsonTracks = this->dataPlayList->getJsonDataTrackForOptionPopup(i);
            // -----------------------------------------------------------------------
            //  마이 플레이리스트 재생 PARAM APP과 동일하게 변경
            //  기존 name : 음원 타이틀,  변경 :: 파일명
            //  기존 path : /mnt/~~~~/파일명, 변경 :: http://127.0.0.1:7080/mnt/~~~~/파일명
            // -----------------------------------------------------------------------
            QStringList tmp_strListPath = jsonTracks["path"].toString().split("/");
            QString tmp_path;
            for(int i= 0 ; i < tmp_strListPath.count(); i++){
                if(i < tmp_strListPath.count()-1){
                    tmp_path += tmp_strListPath.at(i)+"/";
                }
            }
            jsonTracks.insert("ip", "?"+jsonTracks["ip"].toString());
            jsonTracks.insert("name", tmp_strListPath.last());
            jsonTracks.insert("path", "http://127.0.0.1:7080"+tmp_path);
            jsonArr.append(jsonTracks);
        }
    }
    return jsonArr;
}

/**
 * @brief FrameMyPlayList::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameMyPlayList::getDataForPopup(){

    /*
    QJsonArray jsonArrAlbum;
    for(int i = 0; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }
    */
    QJsonArray jsonArrAlbum = getPlayList(-1);

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, dataPlayList->getTitle());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
    //jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_MyPlayList);
    jsonData.insert(KEY_OP_albumImg, dataPlayList->getConverimg());
    jsonData.insert(KEY_OP_cntStar, dataPlayList->getStar());
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
    jsonData.insert(KEY_OP_PLAYLISTNO, dataPlayList->getNo());
    jsonData.insert("flagPlayListMy", true); // false : 친구플레이리스트
    //jsonData.insert("flagAdd", data->getIsFavorite() ? false : true);

    return jsonData;
}
/**
 * @brief FrameWidget::setHover : [override] 호버 세팅
 */
void FrameMyPlayList::setHover(){
    lb_img->setHover();
}

/**
 * @brief FrameMyPlayList::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameMyPlayList::slot_clickedHover(const QString &p_code){

    QJsonObject jsonData = getDataForPopup();
    emit linker->signal_clickedHoverItem(p_code, jsonData);

    /*
    if(p_code==HOVER_CODE_PLAY || p_code==HOVER_CODE_MORE){
        //getTrackInfoFromDB();
        //emit linker->signal_clickedHoverItem(p_code, getDataForPopup());
    }
    else{

        //jsonData.insert(KEY_OP_PLAYLISTNO, data->getId());
        //jsonData.insert("flagAdd", data->getIsFavorite() ? false : true);
        //emit linker->signal_clickedHoverItem(p_code, jsonData);
    }
    */
}
/**
 * @brief FrameMyPlayList::slot_clickedItem : [슬롯] 아이템 클릭
 * @see OptionPopup:slot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameMyPlayList::slot_clickedItem(){

    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);
    emit linker->signal_clickedMovePage(jsonData);
}

/**
 * @brief FrameMyPlayList::slot_responseHttp : [슬롯] Http 요청
 * @param p_id
 * @param p_jsonData
 */
void FrameMyPlayList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){
    if(p_id == HTTP_MUSIC_INFO){
        //setResultOfMusicInfo(p_jsonData);
        setResultOfMusicInfoNew(p_jsonData);
    }
}

void FrameMyPlayList::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;
        if(global.flagConnected){
            lb_img->setImageUrl(QString("%1").arg(dataPlayList->getConverimg()));
        }

        //lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH, lb_title->font()));

        int all_width = flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH;
        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setText(dataPlayList->getTitle());
        //qDebug() << tmp_wordwrap->sizeHint().width();

        if(tmp_wordwrap->sizeHint().width() > all_width){
            if(dataPlayList->getTitle().contains(" ")){
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, (this->LABEL_HEIGHT * 2));

                tmp_wordwrap->clear();
                tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
                tmp_wordwrap->setWordWrap(true);
                tmp_wordwrap->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), (all_width * 2)-30, this->lb_title->font()));
                if(tmp_wordwrap->text().contains("…")){
                    tmp_wordwrap->setToolTip(dataPlayList->getTitle());
                    tmp_wordwrap->setToolTipDuration(2000);
                }

                if(tmp_wordwrap->sizeHint().height() > this->LABEL_HEIGHT * 2){
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), (all_width * 2)-80, this->lb_title->font()));
                    if(this->lb_title->text().contains("…")){
                        this->lb_title->setToolTip(dataPlayList->getTitle());
                        this->lb_title->setToolTipDuration(2000);
                    }
                }
                else{
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), (all_width * 2)-30, this->lb_title->font()));
                    if(this->lb_title->text().contains("…")){
                        this->lb_title->setToolTip(dataPlayList->getTitle());
                        this->lb_title->setToolTipDuration(2000);
                    }
                }
            }
            else{
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), all_width, lb_title->font()));
                if(this->lb_title->text().contains("…")){
                    this->lb_title->setToolTip(dataPlayList->getTitle());
                    this->lb_title->setToolTipDuration(2000);
                }
            }
        }
        else if(tmp_wordwrap->sizeHint().width() == all_width){
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(dataPlayList->getTitle());
        }
        else {
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), all_width, lb_title->font()));//cheon-210708-album
            if(this->lb_title->text().contains("…")){
                this->lb_title->setToolTip(dataPlayList->getTitle());
                this->lb_title->setToolTipDuration(2000);
            }
        }
    }
}
