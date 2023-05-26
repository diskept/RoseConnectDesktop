#include "framemusicartist.h"
#include "common/global.h"
#include "common/sqlitehelper.h"
#include "widget/optionpopup.h"

#include <QJsonArray>

/**
 * @brief FrameMusicArtist::FrameMusicArtist : 음악-분류-아티스트에서 사용하는 FrameWidget
 * @param parent
 */
FrameMusicArtist::FrameMusicArtist(QWidget *parent)
    : FrameWidget(Type::MusicArtist, parent)
{
    setInit();
    setUIControl();
}


/**
 * @brief FrameMusicArtist::setInit : 초기 세팅
 */
void FrameMusicArtist::setInit(){

    linker = Linker::getInstance();
    data = new DataMusicArtist(this);
    dataPopupList = new QList<DataPopup*>();
}


/**
 * @brief FrameMusicArtist::setUIControl : UI 세팅
 */
void FrameMusicArtist::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    QWidget *label_base = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(ImageLabel::Type::Artist);
    this->lb_img->setHidden_favorite(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 2 + this->SPACE_LABELS;
    label_base->setFixedSize(img_width, base_height);

    this->lb_artist = new QLabel(label_base);
    this->lb_artist->setWordWrap(true);
    this->lb_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_artist->setStyleSheet("font-size:16px;font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#FFFFFF;");
    this->lb_artist->setGeometry(0, 0, img_width, this->LABEL_HEIGHT);
    this->lb_artist->setAlignment(Qt::AlignCenter);

    this->lb_numSongs = new QLabel(label_base);
    this->lb_numSongs->setStyleSheet("font-size:16px; font-weight: 300;font-style: normal;line-height: 1.88;text-align: left; color: #999999;");
    this->lb_numSongs->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);
    this->lb_numSongs->setAlignment(Qt::AlignCenter);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(0, 0, this->MARGIN, 0);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(this->lb_img);
    boxLayout->addSpacing(10);
    boxLayout->addWidget(label_base);
    boxLayout->setAlignment(Qt::AlignTop);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}


/**
 * @brief FrameMusicArtist::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameMusicArtist::setData(const QJsonObject &p_jsonObject){

    data->setData(p_jsonObject);
}


/**
 * @brief FrameMusicArtist::setPopupData : 옵션 팝업용 데이터 클래스 세팅
 * @details DataAlbum 세팅 이후에 호출
 */
void FrameMusicArtist::setPopupData(){

    if(dataPopupList->size()==0){
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.artist_id=" + QString::number(data->getArtist_id());
            QVariantList dataDB;
            sqlite->exec(strQuery, dataDB);

            for(int i = 0; i < dataDB.size(); i++){
                QMap<QString, QVariant> tmp_map = dataDB.at(i).toMap();
                DataPopup *dataPopup = new DataPopup(this);
                dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                dataPopupList->append(dataPopup);
            }
        }

        sqlite->close();
        delete sqlite;
    }
}


/**
 * @brief FrameMusicArtist::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameMusicArtist::getDataForPopup(){

    QJsonArray jsonArrAlbum;
    for(int i = 0; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data->getArtist());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Category_Artist);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

    return jsonData;
}


/**
 * @brief FrameMusicArtist::getDataForAddInPlaylist : 플레이리스트 담기를 위한 데이터 구성 반환
 * @return
 */
QJsonArray FrameMusicArtist::getDataForAddInPlaylist(){

    QJsonArray tmp_jsonArrTrack;
    for(int i = 0; i < dataPopupList->size(); i++){
        tmp_jsonArrTrack.append(convertDataPopupToJsonObject(dataPopupList->at(i)));
    }
    return tmp_jsonArrTrack;
}


/**
 * @brief FrameMusicArtist::convertDataPopupToJsonObject : DataPopup 데이터를 QJsonObject 변환
 * @note 트랙정보를 플레이리스트담기용 QJsonObject로 변환한다.
 * @param p_dataPopup DataPopup 트랙정보 데이터
 * @return QJsonObject
 */
QJsonObject FrameMusicArtist::convertDataPopupToJsonObject(DataPopup *p_dataPopup){

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
 * @brief FrameMusicArtist::setHover : [override] 호버 세팅
 */
void FrameMusicArtist::setHover(){

    lb_img->setHover();
}


/**
 * @brief FrameMusicArtist::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameMusicArtist::slot_clickedHover(const QString &p_code){

    setPopupData();

    QJsonObject jsonForPopup = getDataForPopup();
    if(p_code==HOVER_CODE_MORE){
        // 플레이리스트 담기에 필요한 데이터 추가
        jsonForPopup.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, getDataForAddInPlaylist());
    }

    emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
}


/**
 * @brief FrameMusicArtist::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 앨범 상세보기 페이지로 이동
 * 옵션 팝업의 signal OptionPopup::signal_clickedMovePage() 와 동일 구조
 * @see OptionPopup::Pslot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameMusicArtist::slot_clickedItem(){

    setPopupData();
    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

    emit linker->signal_clickedMovePage(jsonData);
}


void FrameMusicArtist::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        this->lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(data->getAlbum_art()));

        this->lb_artist->setText(GSCommon::getTextCutFromLabelWidth(data->getArtist(), IMG_WIDTH, lb_artist->font()));
        if(this->lb_artist->text().contains("…")){
            this->lb_artist->setToolTip(data->getArtist());
            this->lb_artist->setToolTipDuration(2000);
        }
        this->lb_numSongs->setText(GSCommon::getTextCutFromLabelWidth(QString("%1 %2").arg(data->getNumber_of_albums()).arg(QString(tr("Album"))), IMG_WIDTH, lb_numSongs->font()));
        if(this->lb_numSongs->text().contains("…")){
            this->lb_numSongs->setToolTip(QString("%1 %2").arg(data->getNumber_of_albums()).arg(QString(tr("Album"))));
            this->lb_numSongs->setToolTipDuration(2000);
        }
    }
}
