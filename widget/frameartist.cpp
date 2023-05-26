#include "frameartist.h"
#include "common/global.h"
#include "common/sqlitehelper.h"
#include "widget/optionpopup.h"

#include <QJsonArray>

/**
 * @brief FrameArtist::FrameArtist:아티스트 프레임
 * @details 음악-홈 아티스트 목록에 사용되는 위젯<BR>
 * 아티스트 이미지(앨범 이미지와 동일), 아티스트 명, 아티스트의 음원 정보 Show<BR>
 * 마우스 hover 시 하트 / 바로재생 / 더보기 옵션 Show<BR>
 * HOVER ITEM(하트/바로재생/더보기 옵션) 영역 외 클릭 시 아티스트 상세 페이지로 이동<BR>
 * @param parent QWidget
 * @note
 * 마우스 hover 하트 위젯 제거 => API 부재<BR>
 */
FrameArtist::FrameArtist(QWidget *parent)
    : FrameWidget(Type::Artist, parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief FrameArtist::~FrameArtist : 소멸자
 */
FrameArtist::~FrameArtist(){
    qDeleteAll(dataPopupList->begin(), dataPopupList->end());
    dataPopupList->clear();
}

/**
 * @brief FrameArtist::setInit : 초기 세팅
 */
void FrameArtist::setInit(){
    linker = Linker::getInstance();
    data = new DataArtist(this);
    dataPopupList = new QList<DataPopup*>();
}

/**
 * @brief FrameArtist::setUIControl : UI 세팅
 */
void FrameArtist::setUIControl(){
    lb_title = new QLabel;
    //lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;margin-top:15px;");
    lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;margin-top:10px;");
    lb_title->setFixedWidth(IMG_WIDTH);
    lb_title->setAlignment(Qt::AlignCenter);

    lb_album_track = new QLabel;
    //lb_album_track->setStyleSheet("font-size:16px;color:#999999;margin-top:15px;");
    lb_album_track->setStyleSheet("font-size:16px;color:#999999;margin-top:5px;");
    lb_album_track->setFixedWidth(IMG_WIDTH);
    lb_album_track->setAlignment(Qt::AlignCenter);

    lb_img = new ImageLabel;
    lb_img->setType(ImageLabel::Type::Artist);
    lb_img->setHidden_favorite(true);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    boxLayout->addWidget(lb_title);
    boxLayout->addWidget(lb_album_track);

    this->setLayout(boxLayout);

    // connect 연결 :: ImageLabel의 클릭 이벤트 연결
    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}

/**
 * @brief FrameArtist::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameArtist::setData(const QJsonObject &p_jsonObject){
    data->setData(p_jsonObject);
}

/**
 * @brief FrameArtist::selectMusicInfo : 아티스트의 오디오 정보 SELECT FROM DB
 * @details artist_id 값을 이용해 DB 쿼리, 이미 가져온 데이터가 있는 경우 무시한다.
 */
void FrameArtist::selectMusicInfo(){


    if(dataPopupList->size()==0){
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.artist_id="+QString::number(data->getId());

            QVariantList data;
            sqlite->exec(strQuery, data);

            for(int i = 0; i < data.size(); i++){
                //setPopupData(data.at(i).toMap());
                DataPopup *tmp_dataPopup = new DataPopup(this);
                tmp_dataPopup->setJsonData(data.at(i).toJsonObject());
                dataPopupList->append(tmp_dataPopup);
            }
        }

        sqlite->close();
        delete sqlite;
    }
}

/**
 * @brief FrameArtist::setPopupData : 옵션팝업용 데이터 세팅
 * @param p_map
 */
void FrameArtist::setPopupData(QMap<QString, QVariant> p_map){
    Q_UNUSED(p_map);

}

/**
 * @brief FrameWidget::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameArtist::getDataForPopup(){

    QJsonArray jsonArrAlbum;
    for(int i = 0; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data->getArtist());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
    //jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_Artist);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

    return jsonData;
}

/**
 * @brief FrameArtist::getDataForAddInPlaylist : 플레이리스트 담기를 위한 데이터 구성 반환
 * @return
 */
QJsonArray FrameArtist::getDataForAddInPlaylist(){
    QJsonArray tmp_jsonArrTrack;
    for(int i = 0; i < dataPopupList->size(); i++){
        tmp_jsonArrTrack.append(convertDataPopupToJsonObject(dataPopupList->at(i)));
    }
    return tmp_jsonArrTrack;
}

/**
 * @brief FrameArtist::convertDataPopupToJsonObject : DataPopup 데이터를 QJsonObject 변환
 * @note 트랙정보를 플레이리스트담기용 QJsonObject로 변환한다.
 * @param p_dataPopup DataPopup 트랙정보 데이터
 * @return QJsonObject
 */
QJsonObject FrameArtist::convertDataPopupToJsonObject(DataPopup *p_dataPopup){
    QJsonObject jsonTrack;

    // 앨범
    jsonTrack.insert("album", p_dataPopup->getAlbum());

    // 아티스트
    QJsonObject jsonArtist;
    jsonArtist.insert("name", p_dataPopup->getArtist());
    QJsonArray jsonArrArtists;
    jsonArrArtists.append(jsonArtist);
    jsonTrack.insert("artists", jsonArrArtists);

    // 클라이언트 아이디 = 오디오 아이디
    jsonTrack.insert("client_id", p_dataPopup->getId());

    // 썸네일
    QJsonObject jsonThumb;
    jsonThumb.insert("url", p_dataPopup->getAlbum_art());

    QJsonArray jsonArrThumbs;
    jsonArrThumbs.append(jsonThumb);
    jsonTrack.insert("thumbnails", jsonArrThumbs);

    // 타이틀
    jsonTrack.insert("title", p_dataPopup->getTitle());

    // 재생 URL
    jsonTrack.insert("url", p_dataPopup->getData());

    jsonTrack.insert("isFavorite", false);
    jsonTrack.insert("isSelected", false);
    jsonTrack.insert("mac_address", global.device.getDeviceID());
    jsonTrack.insert("type","music");


    return jsonTrack;
}

/**
 * @brief FrameArtist::setHover : [override] 호버 세팅
 */
void FrameArtist::setHover(){
    lb_img->setHover();
}

/**
 * @brief FrameArtist::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameArtist::slot_clickedHover(const QString &p_code){
    selectMusicInfo();

    QJsonObject jsonForPopup = getDataForPopup();

    if(p_code==HOVER_CODE_MORE){
       // 플레이리스트 담기에 필요한 데이터 추가
        jsonForPopup.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, getDataForAddInPlaylist());
    }
    emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
}

/**
 * @brief FrameArtist::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 앨범 상세보기 페이지로 이동
 * 옵션 팝업의 signal OptionPopup::signal_clickedMovePage() 와 동일 구조
 * @see OptionPopup::Pslot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameArtist::slot_clickedItem(){
    selectMusicInfo();
    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);
    emit linker->signal_clickedMovePage(jsonData);
}

void FrameArtist::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;
        QString str_album_track = QString("%1").arg(data->getNumber_of_albums()) + tr(" Album ") + QString("%1").arg(data->getNumber_of_tracks()) + tr(" Track");

        if(global.flagConnected){
            lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP(),global.port_img,data->getAlbum_art()));
        }
        lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getArtist(), IMG_WIDTH, lb_title->font()));
        if(lb_title->text().contains("…")){
            lb_title->setToolTip(data->getArtist());//c230321
            lb_title->setToolTipDuration(2000);//c230321
        }
        lb_album_track->setText(GSCommon::getTextCutFromLabelWidth(str_album_track, IMG_WIDTH, lb_album_track->font()));
        if(lb_album_track->text().contains("…")){
            lb_album_track->setToolTip(str_album_track);//c230321
            lb_album_track->setToolTipDuration(2000);//c230321
        }
    }
}
