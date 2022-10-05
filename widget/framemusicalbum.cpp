#include "framemusicalbum.h"
#include "common/global.h"
#include "common/sqlitehelper.h"
#include "widget/optionpopup.h"

#include <QJsonArray>

FrameMusicAlbum::FrameMusicAlbum(QWidget *parent)
: FrameWidget(Type::MusicAlbum, parent)
{
    setInit();
    setUIControl();
    }

    /**
    * @brief FrameMusicAlbum::setInit : 초기 세팅
    */
    void FrameMusicAlbum::setInit(){
    linker = Linker::getInstance();
    data = new DataAlbum(this);
}

FrameMusicAlbum::~FrameMusicAlbum(){
    this->deleteLater();
}

/**
* @brief FrameMusicAlbum::setUIControl : UI 세팅
*/
void FrameMusicAlbum::setUIControl(){//c220415
    /*lb_title = new QLabel;
    lb_title->setStyleSheet("font-size:14px;color:#FFFFFF;margin-top:10px;");//cheon-210708-album
    lb_title->setFixedWidth(IMG_WIDTH);

    lb_numSongs = new QLabel;
    lb_numSongs->setStyleSheet("font-size:16px;color:#999999;margin-top:5px;");
    lb_numSongs->setFixedWidth(IMG_WIDTH);

    lb_img = new ImageLabel;
    lb_img->setType(ImageLabel::Type::MusicCategory);
    //lb_img->setHidden_favorite(true);
    lb_img->setHidden_favorite(false);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(0,0,0,0);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    boxLayout->addWidget(lb_title);
    boxLayout->addWidget(lb_numSongs);

    this->setLayout(boxLayout);*/

    int base_height = 0;
    int img_width = 0;

    QWidget *label_base = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(ImageLabel::Type::MusicCategory);
    //this->lb_img->setHidden_favorite(true);
    this->lb_img->setHidden_favorite(false);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS * 3;
    label_base->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(IMG_WIDTH);
    //this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    this->lb_numSongs = new QLabel(label_base);
    this->lb_numSongs->setStyleSheet("font-size:16px;color:#999999;");
    this->lb_numSongs->setFixedWidth(IMG_WIDTH);
    //this->lb_numSongs->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);

    this->lb_artist = new QLabel();
    this->lb_artist->setStyleSheet("font-size:16px;color:#999999;");
    this->lb_artist->setFixedWidth(0);

    //this->lb_artist->setText("test");

    vl_box3Layout = new QVBoxLayout;
    vl_box3Layout->setContentsMargins(0,0,0,0);
    vl_box3Layout->addWidget(label_base);

    label_base2 = new QWidget();
    //this->label_base2->setStyleSheet("background-color:transperant;");
    label_base2->setLayout(vl_box3Layout);

    QHBoxLayout *box2Layout = new QHBoxLayout;
    box2Layout->setContentsMargins(0,0,0,0);
    box2Layout->setSpacing(10);
    box2Layout->addWidget(this->lb_artist);
    box2Layout->addWidget(this->lb_numSongs);
    box2Layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
/*
    label_artist_song = new QWidget(label_base);
    label_artist_song->setStyleSheet("font-size:16px;color:#FFFFFF;");
    label_artist_song->setFixedWidth(IMG_WIDTH);
    label_artist_song->setLayout(box2Layout);
    label_artist_song->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), img_width, this->LABEL_HEIGHT);
*/
    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(0,0,0,0);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(this->lb_img);
    boxLayout->addWidget(this->label_base2);
    boxLayout->setAlignment(Qt::AlignTop);

    this->setLayout(boxLayout);

    // ---------------------------------------------------------
    //  Connect
    // ---------------------------------------------------------
    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}


void FrameMusicAlbum::setBackground_label_base(){//c220415


    //this->lb_numSongs->setStyleSheet("border-bottom:1px solid #e5e572;font-size:16px;color:#999999;");
    //this->label_base2->styleSheet().replace("background-color:transperant;","background-color:#3e3e1f;");
    QWidget *line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    line_bottom_1->setStyleSheet("background-color:#e5e572;");
    vl_box3Layout->addWidget(line_bottom_1);
    this->label_base2->setStyleSheet("border-right:3px solid #e5e572; background-color:#3e3e1f;");


}

void FrameMusicAlbum::setBackground_label_end(){//c220415


    //this->lb_numSongs->setStyleSheet("border-bottom:1px solid #e5e572;font-size:16px;color:#999999;");
    //this->label_base2->styleSheet().replace("background-color:transperant;","background-color:#3e3e1f;");
    QWidget *line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    line_bottom_1->setStyleSheet("background-color:#e5e572;");
    vl_box3Layout->addWidget(line_bottom_1);
    this->label_base2->setStyleSheet("border-left:3px solid #e5e572; background-color:#3e3e1f;");


}



/**
* @brief FrameMusicAlbum::setData : [override] 데이터 세팅
* @param p_jsonObject 데이터 세팅
*/
void FrameMusicAlbum::setData(const QJsonObject &p_jsonObject){

    data->setData(p_jsonObject);
}


/**
* @brief FrameWidget::getDataForPopup : [override] 데이터 반환
* @return
*/
QJsonObject FrameMusicAlbum::getDataForPopup(){

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data->getAlbum());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
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
void FrameMusicAlbum::setHover(){

    lb_img->setHover();
}


/**
* @brief FrameMusicAlbum::getDataForPlayMusic : 바로듣기에 필요한 데이터 반환
* @return QJsonArray Track info
* @note 플레이리스트 담기와 JsonObject 구조 다름
*/
QJsonArray FrameMusicAlbum::getDataForPlayMusic(){

    QJsonArray jsonArray;

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
    //        strQuery = "SELECT album, album_id, _id AS id, _data AS data, title, artist, duration ";
    //        strQuery += " FROM audio WHERE album_id=%1 ORDER BY track ";
        strQuery += "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        strQuery += " WHERE A.album_id=%1 ORDER BY A.track ";

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
* @brief FrameMusicAlbum::getDataForAddInPlaylist : 플레이리스트 담기에 필요한 데이터 반환
* @return QJsonArray Track info
* @note 재생과 JsonObject 구조 다름.
*/
QJsonArray FrameMusicAlbum::getDataForAddInPlaylist(){
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
void FrameMusicAlbum::slot_clickedHover(const QString &p_code){

    QJsonObject jsonForPopup = getDataForPopup();

    if(p_code == HOVER_CODE_MORE){
        // 플레이리스트 담기에 필요한 데이터 추가
        jsonForPopup.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, getDataForAddInPlaylist());
    }
    emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
}

/**
* @brief FrameMusicAlbum::slot_clickedItem : [슬롯] 아이템 클릭
* @details 앨범 상세보기 페이지로 이동
* 옵션 팝업의 signal OptionPopup:signal_clickedMovePage() 와 동일 구조
* @see OptionPopup:slot_gotoPage()
* @note hover item(하트/재생/더보기)를 제외한 영역 클릭
*/
void FrameMusicAlbum::slot_clickedItem(){
    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);
    emit linker->signal_clickedMovePage(jsonData);
}

void FrameMusicAlbum::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        if(!data->getAlbum_art().isEmpty()){
            //lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(data->getAlbum_art()));

            QString url = "http://%1:%2%3";
            auto completeURL = url.arg(global.device.getDeviceIP(), global.port_img, data->getAlbum_art());
            lb_img->setImageUrl(completeURL);
        }

        if(data->getSection() == "recent_add" || data->getSection() == "album"){
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

        if(data->getAlbum().at(0).unicode() >= 57344 && data->getAlbum().at(0).unicode() <= 63743){
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
            this->lb_numSongs->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), IMG_WIDTH, this->LABEL_HEIGHT);
            lb_title->setText(GSCommon::getTextCutFromLabelWidth(tr("Unknown"), IMG_WIDTH, lb_title->font()));
            lb_numSongs->setText(GSCommon::getTextCutFromLabelWidth(QString("%3 %1 %2").arg(data->getNum_of_song()).arg(QString(tr("Song"))).arg(QString(data->getArtist())), IMG_WIDTH, lb_numSongs->font()));//cheon-210708-album
        }
        else{
            int all_width = IMG_WIDTH;
            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setText(data->getAlbum());
            //qDebug() << tmp_wordwrap->sizeHint().width();

            if(tmp_wordwrap->sizeHint().width() > IMG_WIDTH){
                if(data->getAlbum().contains(" ")){
                    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), all_width, (this->LABEL_HEIGHT * 2));
                    this->lb_numSongs->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 3), all_width, this->LABEL_HEIGHT);

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
                    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
                    this->lb_numSongs->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 3), IMG_WIDTH, this->LABEL_HEIGHT);
                    this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), IMG_WIDTH, lb_title->font()));//cheon-210708-album                    
                }
            }
            else if(tmp_wordwrap->sizeHint().width() == IMG_WIDTH){
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
                this->lb_numSongs->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), IMG_WIDTH, this->LABEL_HEIGHT);
                this->lb_title->setText(data->getAlbum());
            }
            else {
                this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
                this->lb_numSongs->setGeometry(0, this->LABEL_HEIGHT + (this->SPACE_LABELS * 3), IMG_WIDTH, this->LABEL_HEIGHT);
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), IMG_WIDTH, lb_title->font()));//cheon-210708-album
            }

            this->lb_numSongs->setText(GSCommon::getTextCutFromLabelWidth(QString("%3 %1 %2").arg(data->getNum_of_song()).arg(QString(tr("Song"))).arg(QString(data->getArtist())), IMG_WIDTH, lb_numSongs->font()));
        }
    }
}

