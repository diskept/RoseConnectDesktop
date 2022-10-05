#include "framefriendplaylist.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"
#include "widget/optionpopup.h"

#include <QJsonArray>

const int HTTP_MUSIC_INFO = 99;

/**
 * @brief FrameFriendPlayList::FrameFriendPlayList : 친구 플레이리스트 프레임
 * @details
 * 사용 API : 친구 플레이리스트 V3_LEGACY_BETA/legacy/v3/playlist/friend
 *         : 플레이리스트 트랙 MOD_LAGACY_BETA/playlist/track/fetch
 *         : 음원 재생은 ROSEIP:PORT/music_network_play 사용
 * @note :
 * #필수 param은 name, path
 * #특이사항 : 로즈 기기 미디러라이브러리에 track path가 올라오지 않은 경우 음원 재생 불가
 *          :
 * @param parent
 */
FrameFriendPlayList::FrameFriendPlayList(QWidget *parent)
    : FrameWidget(Type::FriendPlayList, parent)
{
    setInit();
    setUIControl();

}

FrameFriendPlayList::FrameFriendPlayList(bool flagIsViewAllSize, QWidget *parent)
    : FrameWidget(Type::AddedAlbum, parent)
    , flagIsViewAllSize(flagIsViewAllSize)
{
    setInit();
    setUIControl();
}

/**
 * @brief FrameFriendPlayList::~FrameFriendPlayList:: 소멸자
 */
FrameFriendPlayList::~FrameFriendPlayList(){
    //qDeleteAll(dataPopupList->begin(),dataPopupList->end());
    //dataPopupList->clear();
    playUrlList.clear();
    playTitleList.clear();
}

/**
 * @brief FrameWidget::setInit : [override] 초기 세팅
 */
void FrameFriendPlayList::setInit(){
    linker = Linker::getInstance();
    //data = new DataPlayList(this);

    dataPlayList = new Data_M_PlayList(this);
    //dataPopupList = new QList<DataPopup*>();
}

/**
 * @brief FrameWidget::setUIControl : [override] UI 세팅
 */
void FrameFriendPlayList::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    QWidget *freindplaylist = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(flagIsViewAllSize ? ImageLabel::Type::playListFriend_C :ImageLabel::Type::PlayListFriend);
    this->lb_img->setHidden_favorite(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS * 3;
    freindplaylist->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(freindplaylist);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    this->lb_user = new QLabel(freindplaylist);
    this->lb_user->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_user->setStyleSheet("font-size:16px;color:#999999;");
    this->lb_user->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_user->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    //boxLayout->addWidget(lb_title);
    //boxLayout->addWidget(lb_user);
    boxLayout->addWidget(freindplaylist);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}



/**
 * @brief FrameFriendPlayList::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameFriendPlayList::setData(const QJsonObject &p_jsonObject){

    dataPlayList->setData(p_jsonObject);  // playListNo 만..세팅함..
    requestMusic();
}

void FrameFriendPlayList::requestMusic(){

    playUrlList.clear();
    playTitleList.clear();
    //dataPopupList->clear();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


    if(global.device.getDeviceIP()!="" && global.device.getDeviceID()!=""){
        QJsonObject tmp_json;
        /*tmp_json.insert("local_ip", global.device.getDeviceIP());
        tmp_json.insert("mac_address", global.device.getDeviceID());
        network->request(HTTP_MUSIC_INFO, QString("%1/playlist/friend/%2?page=0&size=20").arg(global.legacy_v3_api).arg(dataPlayList->getNo()), tmp_json, true,true);*/

        network->request(HTTP_MUSIC_INFO, QString("%1/member/playlist/%2?page=0&size=20")
                         .arg(global.legacy_v1).arg(dataPlayList->getNo()), tmp_json, false, true);
    }
}

/**
 * @brief FrameFriendPlayList::setResultOfMusicInfoNew Track 정보 요청 결과 처리
 * @param p_jsonObject
 */
void FrameFriendPlayList::setResultOfMusicInfoNew(const QJsonObject &p_jsonObject){

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
 * @brief FrameFriendPlayList::setResultOfMusicInfo::Track 정보 요청 결과 처리
 * @details 친구 플레이리스트 ID를 이용해
 * @param p_jsonObject
 */
void FrameFriendPlayList::setResultOfMusicInfo(const QJsonObject &p_jsonObject){
    const QString jsonKey_tracks = "tracks";
    const QString jsonKey_playurl = "playurl";
    const QString jsonKey_title = "title";

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){
        if(p_jsonObject.contains(jsonKey_tracks)){
            QJsonArray jsonArrTack = p_jsonObject[jsonKey_tracks].toArray();

            for(int i = 0 ; i < jsonArrTack.size(); i++){
                QJsonObject jsonTrack = jsonArrTack.at(i).toObject();

                if(jsonTrack.contains(jsonKey_playurl) && jsonTrack.contains(jsonKey_title)){

                    playUrlList.append(jsonTrack[jsonKey_playurl].toString());
                    playTitleList.append(jsonTrack[jsonKey_title].toString());

//                    DataPopup *tmp_dataPopup = new DataPopup(this);
//                    tmp_dataPopup->setJsonData(jsonTrack);
//                    dataPopupList->append(tmp_dataPopup);
                    setPopupData(jsonTrack[jsonKey_title].toString(), jsonTrack[jsonKey_playurl].toString());
                }
            }
        }
    }

    sqlite->close();
    delete sqlite;
}

void FrameFriendPlayList::setPopupData(const QString &p_title, const QString &p_path){

    Q_UNUSED(p_title);
    Q_UNUSED(p_path);
    /*
    DataPopup *dataPopup = new DataPopup(this);
    dataPopup->setTitle(data->getName());
    dataPopup->setArtist(data->getUser_name());     // 옵션 팝업
    dataPopup->setName(p_title);
    dataPopup->setPath(p_path);

    dataPopupList->append(dataPopup);
    */

}

/**
 * @brief VideoDetail::getPlayList : 재생을 위한 트랙 정보를 JSONArray로 반환
 * @param p_index int if < 0 all list
 * @return
 * @note 로즈튜브 재생 관련 필수값 전달받은 내용 없음<br>
 * App 디버깅 내용및 음악 재생 API 참고하여 구성함.
 */
QJsonArray FrameFriendPlayList::getPlayList(int p_index){
    QJsonArray jsonArr;

    if(p_index >=0 ){
        QJsonObject jsonTracks = this->dataPlayList->getJsonDataTrackForOptionPopup(p_index);

        // -----------------------------------------------------------------------
        //  친구 플레이리스트 재생 PARAM APP과 동일하게 변경
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
        jsonTracks.insert("isShare", jsonTracks["ip"].toString()==global.device.getDeviceID() ? false : true);
        jsonArr.append(jsonTracks);

    }else{
        for(int i = 0 ; i < this->dataPlayList->getTrackList().count(); i++){
            QJsonObject jsonTracks = this->dataPlayList->getJsonDataTrackForOptionPopup(i);
            // -----------------------------------------------------------------------
            //  친구 플레이리스트 재생 PARAM APP과 동일하게 변경
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
            jsonTracks.insert("isShare", jsonTracks["ip"].toString()==global.device.getDeviceID() ? false : true);
            jsonArr.append(jsonTracks);
        }
    }
    return jsonArr;
}
/**
 * @brief FrameWidget::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameFriendPlayList::getDataForPopup(){

    /*
    QJsonArray jsonArrAlbum;
    for(int i = 0; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonDataForNetworkPlay());
        //jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }
    */
    QJsonArray jsonArrAlbum = getPlayList(-1);

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, dataPlayList->getTitle());
    jsonData.insert(KEY_OP_TITLE_SUB, dataPlayList->getUsername());
    //jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_FriendPlayList);
    jsonData.insert(KEY_OP_albumImg, dataPlayList->getConverimg());
    jsonData.insert(KEY_OP_cntStar, dataPlayList->getStar());
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
    //jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
    jsonData.insert(KEY_OP_PLAYLISTNO, dataPlayList->getNo());
    jsonData.insert("flagPlayListMy", false); // false : 친구플레이리스트
    //jsonData.insert("flagAdd", data->getIsFavorite() ? false : true);

    return jsonData;
}
/**
 * @brief FrameWidget::setHover : [override] 호버 세팅
 */
void FrameFriendPlayList::setHover(){

    lb_img->setHover();

}

/**
 * @brief FrameMyPlayList::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameFriendPlayList::slot_clickedHover(const QString &p_code){
    QJsonObject jsonData = getDataForPopup();
    emit linker->signal_clickedHoverItem(p_code, jsonData);

    /*
    if(p_code==HOVER_CODE_FAV){

    }
    else if(p_code==HOVER_CODE_MORE){

    }
    else if(p_code==HOVER_CODE_PLAY){
        // 옵션 팝업 공용화 시키지 않고 바로 재생
        QJsonArray jsonArrPlay;
        for(int i= 0 ; i < playUrlList.size(); i++){
            QJsonObject tmp_jsonMusic;
            tmp_jsonMusic.insert("ext","");
            tmp_jsonMusic.insert("id","");
            tmp_jsonMusic.insert("ip","");
            tmp_jsonMusic.insert("isCloud",false);
            tmp_jsonMusic.insert("isDir",false);
            tmp_jsonMusic.insert("isFile",true);
            tmp_jsonMusic.insert("isNetworkAdd", false);
            tmp_jsonMusic.insert("isServer",true);
            tmp_jsonMusic.insert("isThumbNailGetting", false);
            tmp_jsonMusic.insert("isUp",false);
            tmp_jsonMusic.insert("name",playTitleList.at(i));
            tmp_jsonMusic.insert("path",playUrlList.at(i));
            tmp_jsonMusic.insert("pw","");
            tmp_jsonMusic.insert("smbParamPath","");
            tmp_jsonMusic.insert("thumbnail","");

            jsonArrPlay.append(tmp_jsonMusic);
        }

        QJsonObject tmp_json;
        tmp_json.insert("musicPlayType",15);
        tmp_json.insert("music", jsonArrPlay);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(11, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);

    }
    */

}
/**
 * @brief FrameFriendPlayList::slot_clickedItem : [슬롯] 아이템 클릭
 * @see OptionPopup:slot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameFriendPlayList::slot_clickedItem(){

    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);
    emit linker->signal_clickedMovePage(jsonData);
}

/**
 * @brief FrameMyPlayList::slot_responseHttp : [슬롯] Http 요청
 * @param p_id
 * @param p_jsonData
 */
void FrameFriendPlayList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){
    if(p_id == HTTP_MUSIC_INFO){
        //setResultOfMusicInfo(p_jsonData); // 기존 playlist/track/fetch API 호출하는 경우 결과 세팅하는 함수
        setResultOfMusicInfoNew(p_jsonData);
    }
}

void FrameFriendPlayList::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;
        if(global.flagConnected){
            lb_img->setImageUrl(QString("%1").arg(dataPlayList->getConverimg()));
        }

        int all_width = flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH;
        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setText(dataPlayList->getTitle());
        //qDebug() << tmp_wordwrap->sizeHint().width();

        if(tmp_wordwrap->sizeHint().width() > (all_width)){
            if(dataPlayList->getTitle().contains(" ")){
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, (this->LABEL_HEIGHT * 2));
                this->lb_user->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), all_width * 2, lb_title->font()));//cheon-210708-album

                tmp_wordwrap->clear();
                tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
                tmp_wordwrap->setWordWrap(true);
                tmp_wordwrap->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), (all_width * 2)-30, this->lb_title->font()));

                if(tmp_wordwrap->sizeHint().height() > this->LABEL_HEIGHT * 2){
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), (all_width * 2)-80, this->lb_title->font()));
                }
                else{
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), (all_width * 2)-30, this->lb_title->font()));
                }
            }
            else{
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
                this->lb_user->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), all_width, lb_title->font()));//cheon-210708-album
            }
        }
        else if(tmp_wordwrap->sizeHint().width() == all_width){
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
            this->lb_user->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(dataPlayList->getTitle());
        }
        else {
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, this->LABEL_HEIGHT);
            this->lb_user->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getTitle(), all_width, lb_title->font()));//cheon-210708-album
        }

        this->lb_user->setText(GSCommon::getTextCutFromLabelWidth(dataPlayList->getUsername(), all_width, lb_user->font()));
    }
}
