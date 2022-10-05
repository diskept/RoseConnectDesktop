#include "framerecentadd.h"
#include "common/global.h"
#include "common/sqlitehelper.h"
#include "widget/optionpopup.h"

#include <QJsonArray>

FrameRecentAdd::FrameRecentAdd(QWidget *parent)
    : FrameWidget(Type::AddedAlbum,  parent)
{
    setInit();
    setUIControl();

}

FrameRecentAdd::FrameRecentAdd(bool flagIsViewAllSize, QWidget *parent)
    : FrameWidget(Type::AddedAlbum, parent)
    , flagIsViewAllSize(flagIsViewAllSize)
{
    setInit();
    setUIControl();
}

FrameRecentAdd::~FrameRecentAdd(){

    this->deleteLater();
}

/**
 * @brief FrameWidget::setInit : [override] 초기 세팅
 */
void FrameRecentAdd::setInit(){

    linker = Linker::getInstance();
    data = new DataAlbum(this);
    dataPopup = new DataPopup(this);
}

/**
 * @brief FrameWidget::setUIControl : [override] UI 세팅
 */
void FrameRecentAdd::setUIControl(){
    /*lb_title = new QLabel;
    lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;margin-top:15px;");
    lb_title->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);

    lb_artist = new QLabel;
    lb_artist->setStyleSheet("font-size:16px;color:#999999;margin-top:5px;");
    lb_artist->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);

    lb_img = new ImageLabel;
    lb_img->setType(flagIsViewAllSize ? ImageLabel::Type::MusicCategory : ImageLabel::Type::Album);
    lb_img->setHidden_favorite(true);


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

    this->lb_img = new ImageLabel();
    this->lb_img->setType(flagIsViewAllSize ? ImageLabel::Type::MusicCategory : ImageLabel::Type::Album);
    this->lb_img->setHidden_favorite(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS * 3;
    label_base->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    this->lb_artist = new QLabel(label_base);
    this->lb_artist->setStyleSheet("font-size:16px;color:#999999;");
    this->lb_artist->setFixedWidth(flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH);
    this->lb_artist->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(this->lb_img);
    boxLayout->addWidget(label_base);
    boxLayout->setAlignment(Qt::AlignTop);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}

/**
 * @brief FrameRecentAlbum::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameRecentAdd::setData(const QJsonObject &p_jsonObject){

    data->setData(p_jsonObject);

    lb_img->setFavorStatus(data->getFavorite() > 0 ? true : false);
    //setPopupData();


}

/**
 * @brief FrameRecentAlbum::setPopupData : 옵션 팝업용 데이터 클래스 세팅
 * @details DataAlbum 세팅 이후에 호출
 */
void FrameRecentAdd::setPopupData(){
    /*
    dataPopup->setId(data->getId());
    dataPopup->setAlbum(data->getAlbum());
    dataPopup->setTitle(data->getTitle());
    dataPopup->setArtist(data->getArtist());
    dataPopup->setAlbum_id(data->getAlbum_id());
    dataPopup->setData(data->getData());
    */
}


/**
 * @brief FrameWidget::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameRecentAdd::getDataForPopup(){

    // -------------------------------------------
    //  트랙 구조에서 앨범 구조로 변경
    // -------------------------------------------
    //QJsonArray jsonArrAlbum;
    //jsonArrAlbum.append(dataPopup->getJsonData());

    QJsonObject jsonData;
    /*jsonData.insert(KEY_OP_TITLE_MAIN, data->getAlbum());
    jsonData.insert(KEY_OP_TITLE_SUB, data->getArtist());
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Home_AddedAlbum);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,getDataForPlayMusic());
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
    jsonData.insert("isAlbum", true);*/

    jsonData.insert(KEY_OP_TITLE_MAIN, data->getAlbum());
    jsonData.insert(KEY_OP_TITLE_SUB, data->getArtist());
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Album);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_MIME, data->getMimeType());
    jsonData.insert(KEY_OP_ARTIST, data->getArtist());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA, getDataForPlayMusic());
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
    jsonData.insert("op_sampleRate", this->data->getSamplerate());
    jsonData.insert("op_bitDepth", this->data->getBitdepth());
    jsonData.insert("isAlbum", true);

    return jsonData;
}
/**
 * @brief FrameWidget::setHover : [override] 호버 세팅
 */
void FrameRecentAdd::setHover(){


    lb_img->setHover();
}

/**
 * @brief FrameRecentAdd::getDataForPlayMusic : 바로듣기에 필요한 데이터 반환
 * @return QJsonArray Track info
 * @note 플레이리스트 담기와 JsonObject 구조 다름
 */
QJsonArray FrameRecentAdd::getDataForPlayMusic(){

    QJsonArray jsonArray;

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art  ";
        strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id WHERE A.album_id=%1 ORDER BY A.track ";

        QVariantList dataDB;
        sqlite->exec(strQuery.arg(data->getAlbum_id()), dataDB);
        if(dataDB.size() > 0){
            for(int i = 0; i < dataDB.size(); i++){
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
 * @brief FrameRecentAdd::getDataForAddInPlaylist : 플레이리스트 담기에 필요한 데이터 반환
 * @return QJsonArray Track info
 * @note 재생과 JsonObject 구조 다름.
 */
QJsonArray FrameRecentAdd::getDataForAddInPlaylist(){
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
void FrameRecentAdd::slot_clickedHover(const QString &p_code){

    QJsonObject jsonForPopup = getDataForPopup();

    if(p_code == HOVER_CODE_MORE){
        jsonForPopup.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, getDataForAddInPlaylist());
    }

    emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
}

/**
 * @brief FrameRecentAdd::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 앨범 상세보기 페이지로 이동
 * 옵션 팝업의 signal OptionPopup::signal_clickedMovePage() 와 동일 구조
 * @see OptionPopup::Pslot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameRecentAdd::slot_clickedItem(){

    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);
    emit linker->signal_clickedMovePage(jsonData);
}

void FrameRecentAdd::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        if(global.flagConnected){
            lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(data->getAlbum_art()));
        }

        if(data->getSection() == "recent_add" || data->getSection() == "album" || data->getSection() == "search_album"){
            QString tmp_mime = data->getMimeType();
            int tmp_sample = data->getSamplerate();
            int tmp_bit = data->getBitdepth();
            QString snd_mime = "";

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

            lb_img->setMimeType(snd_mime);
        }

        int all_width = flagIsViewAllSize ? IMG_WIDTH_S : IMG_WIDTH;
        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setText(data->getAlbum());

        if(tmp_wordwrap->sizeHint().width() > all_width){
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
        else if(tmp_wordwrap->sizeHint().width() == all_width){
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
