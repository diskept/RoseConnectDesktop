#include "widget/framemusicgenre.h"

#include "common/global.h"
#include "common/sqlitehelper.h"

#include "widget/optionpopup.h"

#include <QJsonArray>


FrameMusicGenre::FrameMusicGenre(QWidget *parent)
    : FrameWidget(Type::MusicGenre, parent)
{
    setInit();
    setUIControl();
}


/**
 * @brief FrameAlbum::setInit : 초기 세팅
 */
void FrameMusicGenre::setInit(){

    linker = Linker::getInstance();
    data = new DataMusicGenre(this);
    dataPopupList = new QList<DataPopup*>();
}


/**
 * @brief FrameAlbum::setUIControl : UI 세팅
 */
void FrameMusicGenre::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    QWidget *label_base = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(ImageLabel::Type::MusicCategory);
    this->lb_img->setHidden_favorite(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS;
    label_base->setFixedSize(img_width, base_height);

    this->lb_genre = new QLabel(label_base);
    this->lb_genre->setWordWrap(true);
    this->lb_genre->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_genre->setStyleSheet("font-size:16px;font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#FFFFFF;");
    this->lb_genre->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

    this->lb_numSongs = new QLabel(label_base);
    this->lb_numSongs->setStyleSheet("font-size:16px; font-weight: 300;font-style: normal;line-height: 1.88;text-align: left; color: #999999;");
    this->lb_numSongs->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);

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
 * @brief FrameAlbum::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameMusicGenre::setData(const QJsonObject &p_jsonObject){

    data->setData(p_jsonObject);
}


/**
 * @brief FrameAlbum::setPopupData : 옵션 팝업용 데이터 클래스 세팅
 * @details DataAlbum 세팅 이후에 호출
 */
void FrameMusicGenre::setPopupData(){

    if(dataPopupList->size()==0){
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
            strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.genre_id=" + QString::number(data->getGenre_id());

            QVariantList dataDB;
            sqlite->exec(strQuery, dataDB);
            for(int i = 0; i < dataDB.size(); i++){

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
 * @brief FrameWidget::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameMusicGenre::getDataForPopup(){

    QJsonArray jsonArrAlbum;
    for(int i = 0; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data->getName());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Category_Genre);
    jsonData.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_OP_GENRENO, data->getGenre_id());
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

    return jsonData;
}


/**
 * @brief FrameMusicGenre::getDataForAddInPlaylist : 플레이리스트 담기를 위한 데이터 구성 반환
 * @return
 */
QJsonArray FrameMusicGenre::getDataForAddInPlaylist(){

    QJsonArray tmp_jsonArrTrack;
    for(int i = 0; i < dataPopupList->size(); i++){
        tmp_jsonArrTrack.append(convertDataPopupToJsonObject(dataPopupList->at(i)));
    }
    return tmp_jsonArrTrack;
}


/**
 * @brief FrameMusicGenre::convertDataPopupToJsonObject : DataPopup 데이터를 QJsonObject 변환
 * @note 트랙정보를 플레이리스트담기용 QJsonObject로 변환한다.
 * @param p_dataPopup DataPopup 트랙정보 데이터
 * @return QJsonObject
 */
QJsonObject FrameMusicGenre::convertDataPopupToJsonObject(DataPopup *p_dataPopup){

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
 * @brief FrameWidget::setHover : [override] 호버 세팅
 */
void FrameMusicGenre::setHover(){

    lb_img->setHover3();
}


/**
 * @brief FrameRecentAlbum::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameMusicGenre::slot_clickedHover(const QString &p_code){
    /*setPopupData();

    QJsonObject jsonForPopup = getDataForPopup();
    if(p_code==HOVER_CODE_MORE){
        // 플레이리스트 담기에 필요한 데이터 추가
        jsonForPopup.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, getDataForAddInPlaylist());
    }*/

    QJsonObject jsonForPopup;
    jsonForPopup.insert(KEY_OP_TITLE_MAIN, data->getName());
    jsonForPopup.insert(KEY_OP_TITLE_SUB, "");
    jsonForPopup.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Category_Genre);
    jsonForPopup.insert(KEY_OP_albumImg, data->getAlbum_art());
    jsonForPopup.insert(KEY_OP_cntStar, 0);
    jsonForPopup.insert(KEY_OP_GENRENO, data->getGenre_id());
    jsonForPopup.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);

    emit linker->signal_clickedHoverItem(p_code, jsonForPopup);
}


/**
 * @brief FrameMusicArtist::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 앨범 상세보기 페이지로 이동
 * 옵션 팝업의 signal OptionPopup::signal_clickedMovePage() 와 동일 구조
 * @see OptionPopup::Pslot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameMusicGenre::slot_clickedItem(){

    setPopupData();

    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_GENREINFO);

    emit linker->signal_clickedMovePage(jsonData);
}


void FrameMusicGenre::paintEvent(QPaintEvent *event){

    FrameWidget::paintEvent(event);

    if(flagNeedDraw == true){
        flagNeedDraw = false;

        int all_width = 0;

        this->lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(data->getAlbum_art()));

        all_width = IMG_WIDTH;

        QString title = data->getName();

        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
        tmp_wordwrap->setText(title);

        int title_width = tmp_wordwrap->sizeHint().width();

        if(title_width > all_width){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_title_line1 = "";

            tmp_split = title;
            splitToken = tmp_split.split(" ");

            tmp_wordwrap->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_title_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_title_line1 += " " + splitToken.at(i);
                    }
                    tmp_wordwrap->setText(tmp_title_line1);

                    if(tmp_wordwrap->sizeHint().width() > all_width){
                        tmp_wordwrap->setText("");
                        tmp_title_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_wordwrap->setText("");
            tmp_wordwrap->setText(tmp_title_line1);

            title_width = tmp_wordwrap->sizeHint().width() + all_width;

            this->lb_genre->setGeometry(0, 0, all_width, this->LABEL_HEIGHT * 2);
            this->lb_genre->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->lb_genre->font()));
            if(this->lb_genre->text().contains("…")){
                this->lb_genre->setToolTip(title);
                this->lb_genre->setToolTipDuration(2000);
            }

            this->lb_numSongs->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
        }
        else{
            this->lb_genre->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
            this->lb_genre->setText(title);

            this->lb_numSongs->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
        }

        this->lb_numSongs->setText(GSCommon::getTextCutFromLabelWidth(QString("%1 %2").arg(data->getNumber_of_album()).arg(QString(tr("Album"))), all_width, lb_numSongs->font()));
        if(this->lb_numSongs->text().contains("…")){
            this->lb_numSongs->setToolTip(QString("%1 %2").arg(data->getNumber_of_album()).arg(QString(tr("Album"))));
            this->lb_numSongs->setToolTipDuration(2000);
        }
    }
}
