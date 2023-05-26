#include "widget/framerosetubeplaylist.h"

#include "widget/optionpopup.h"

#include "common/networkhttp.h"
#include "common/global.h"

#include <QJsonArray>


const int HTTP_NETWORK_TRACK_LIST = 99;

FrameRoseTubePlayList::FrameRoseTubePlayList(OptionPopup::TypeMenu p_typeMenu_optionPopup, bool flagSmallUI,QWidget *parent)
    : FrameWidget(Type::RoseTubePlayList, parent)
    , flagSmallUI(flagSmallUI)
{
    this->typeMenu_optionPopup = p_typeMenu_optionPopup;

    setInit();
    setUIControl();
}


FrameRoseTubePlayList::~FrameRoseTubePlayList(){

    this->deleteLater();
}


/**
 * @brief FrameRoseMyPlay::setInit
 */
void FrameRoseTubePlayList::setInit(){

    linker = Linker::getInstance();
    data_RT_PlayList = new Data_RT_PlayList(this);
    dataTracks = new QList<Data_RT_Track*>();
    dataPopupList = new QList<DataPopup*>();
}


/**
 * @brief FrameWidget::setUIControl : [override] UI 세팅
 */
void FrameRoseTubePlayList::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    this->lb_img = new ImageLabel();
    if(flagSmallUI){
        lb_img->setType(ImageLabel::Type::RoseTubeList_s);
    }else{
        lb_img->setType(ImageLabel::Type::RoseTubeList);
    }

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 3 + 35;

    QWidget *label_base = new QWidget();
    label_base->setFixedSize(img_width, base_height);

    this->lb_img->setHidden_favorite(true);

    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setAlignment(Qt::AlignVCenter);
    this->lb_title->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

    this->label_owner = new QLabel(label_base);
    this->label_owner->setStyleSheet("font-size:14px;color:#999999;");
    this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2), img_width, this->LABEL_HEIGHT);

    QPixmap *img_thmub = GSCommon::getUIPixmapImg(":/images/rosehome/home_like_ico.png");

    this->image_thumb = new QLabel(label_base);
    this->image_thumb->setPixmap(*img_thmub);
    this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
    this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 3), 0, 0);

    this->label_thumb = new QLabel(label_base);
    this->label_thumb->setStyleSheet("font-size:14px;color:#999999;");
    this->label_thumb->setGeometry(40, (this->LABEL_HEIGHT * 3) + 7, img_width - 40, this->LABEL_HEIGHT);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(0, 0, 20, 0);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    boxLayout->addSpacing(10);
    boxLayout->addWidget(label_base);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}


/**
 * @brief FrameRoseMyPlay::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameRoseTubePlayList::setData(const QJsonObject &p_jsonObject){

    data_RT_PlayList->setData(p_jsonObject);

    requestTracks();
}


/**
 * @brief FrameRoseMyPlay::setPopupData : 옵션 팝업용 데이터 클래스 세팅
 * @details DataAlbum 세팅 이후에 호출
 */
void FrameRoseTubePlayList::setPopupData(const int &p_index ){

    Data_RT_Track *data = dataTracks->at(p_index);
    QString videoID = data->getPlayurl().split("?v=").last();
    DataPopup *dataPopup = new DataPopup(this);
    dataPopup->setId(videoID); // 유투브 URL 의 ID 값을 의미함.
    dataPopup->setChannelId(data->getChannelId());
    dataPopup->setTitle(data->getTitle());
    dataPopup->setArtist(data->getChannelTitle());
    dataPopup->setChannelName(data->getChannelTitle());
    dataPopup->setData(data->getPlayurl());
    dataPopup->setThumbnailUrl(data->getThumbnail());

    dataPopupList->append(dataPopup);
}


/**
 * @brief FrameRoseMyPlay::requestTracks : 트랙 정보 요청
 */
void FrameRoseTubePlayList::requestTracks(){

    qDeleteAll(dataTracks->begin(), dataTracks->end());
    dataTracks->clear();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


    /*QUrlQuery params;
    params.addQueryItem("playlistno", QString::number(data_RT_PlayList->getNo()));
    //params.addQueryItem("range_start",QString::number(0));
    //params.addQueryItem("range_end",QString::number(data_RT_PlayList->getTotalcount()));

    network->request(HTTP_NETWORK_TRACK_LIST, QString("%1/playlist/track/fetch?range_start=0&range_end=%2")
                     .arg(global.legacy_v1).arg(data_RT_PlayList->getTotalcount()), params, true,true);*/

    QString url = global.legacy_v1 + QString("/member/playlist/%1").arg(data_RT_PlayList->getNo()) + QString("?&page=0&size=%1").arg(data_RT_PlayList->getTotalcount());
    QJsonObject json;

    network->request(HTTP_NETWORK_TRACK_LIST
                     , url
                     , json
                     , false
                     , true);
}


/**
 * @brief FrameRoseMyPlay::setResultOfTracks : 트랙정보 요청 결과 처리
 * @param p_jsonObject QJsonObject response
 */
void FrameRoseTubePlayList::setResultOfTracks(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playlist = "playlist";
    const QString jsonKey_tracks = "tracks";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_playlist)){
            QJsonObject jsonPlaylist = ProcJsonEasy::getJsonObject(p_jsonObject, jsonKey_playlist);
            QJsonArray jsonArrTack = ProcJsonEasy::getJsonArray(jsonPlaylist, jsonKey_tracks);

            for(int i = 0 ; i < jsonArrTack.size(); i++){

                Data_RT_Track *data = new Data_RT_Track;
                data->setData(jsonArrTack.at(i).toObject());
                dataTracks->append(data);

                setPopupData(i);
            }
        }
    }
}


/**
 * @brief FrameRoseMyPlay::slot_responseHttp : [슬롯] Http 요청
 * @param p_id
 * @param p_jsonData
 */
void FrameRoseTubePlayList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){

    if(p_id == HTTP_NETWORK_TRACK_LIST){
        setResultOfTracks(p_jsonData);
    }
}


/**
 * @brief FrameRoseMyPlay::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameRoseTubePlayList::getDataForPopup(){

    QJsonArray jsonArrAlbum;
    for(int i = 0 ; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }

    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data_RT_PlayList->getTitle());
    jsonData.insert(KEY_OP_TITLE_SUB, "");
    //jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_OP_TYPE, this->typeMenu_optionPopup);
    jsonData.insert(KEY_OP_albumImg, data_RT_PlayList->getConverimg());
    jsonData.insert(KEY_OP_cntStar, data_RT_PlayList->getStar());
    jsonData.insert(KEY_OP_ROSETUBE_PLAYURL, "");
    jsonData.insert(KEY_OP_PLAYLISTNO, data_RT_PlayList->getNo());
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::RoseTubeList);

    return jsonData;
}


/**
 * @brief FrameRoseMyPlay::setHover : [override] 호버 세팅
 */
void FrameRoseTubePlayList::setHover(){

    lb_img->setHover();
}


/**
 * @brief FrameRoseMyPlay::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameRoseTubePlayList::slot_clickedHover(const QString &p_code){

    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);
    jsonData.insert("playList_info", data_RT_PlayList->getData());

    emit linker->signal_clickedHoverItem(p_code, jsonData);
}


/**
 * @brief FrameRoseTube::slot_clickedItem : [슬롯] 아이템 클릭
 * @see OptionPopup:slot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameRoseTubePlayList::slot_clickedItem(){

    QJsonObject jsonData = getDataForPopup();
    jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_PLAYLISTINFO);
    jsonData.insert("playList_info", data_RT_PlayList->getData());

    emit linker->signal_clickedMovePage(jsonData);
}


//const int LABEL_HEIGHT = 18;
//const int SPACE_LABELS = 5;
void FrameRoseTubePlayList::paintEvent(QPaintEvent *event){
    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        int all_width = 0;

        QString image_bg_path = "";
        if(data_RT_PlayList->getConverimg().isEmpty()){
            image_bg_path = ":/images/def_tube_340.png";
        }
        else{
            image_bg_path = data_RT_PlayList->getConverimg();
        }

        this->lb_img->setImageUrl(QString("%1").arg(image_bg_path));
        all_width = this->lb_img->sizeHint().width();

        QString title = this->data_RT_PlayList->getTitle();

        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setStyleSheet("font-size:16px;color:#FFFFFF;");
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

            this->lb_title->setGeometry(0, 0, all_width, (LABEL_HEIGHT * 2));
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->lb_title->font()));
            if(this->lb_title->text().contains("…")){
                this->lb_title->setToolTip(title);//c230321
                this->lb_title->setToolTipDuration(2000);//c230321
            }

            this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2), all_width, this->LABEL_HEIGHT);
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 3) + 5, 0, 0);
            this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 3) + 12, all_width - 40, this->LABEL_HEIGHT);
        }
        else {
            this->lb_title->setGeometry(0, 0, IMG_WIDTH, this->LABEL_HEIGHT);
            this->lb_title->setText(title);

            this->label_owner->setGeometry(0, this->LABEL_HEIGHT, all_width, this->LABEL_HEIGHT);
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + 5, 0, 0);
            this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 2) + 12, all_width - 40, this->LABEL_HEIGHT);
        }

        QString thumb_count ="";
        thumb_count = QString("%1").number(this->data_RT_PlayList->getThumupCount());
        thumb_count += " | " + QString("%1").number(this->data_RT_PlayList->getTotalcount()) + tr(" Tracks");

        this->label_owner->setAlignment(Qt::AlignVCenter);
        this->label_owner->setText(this->data_RT_PlayList->getUsername());

        this->label_thumb->setAlignment(Qt::AlignVCenter);
        this->label_thumb->setText(thumb_count);
    }
}
