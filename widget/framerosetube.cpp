#include "widget/framerosetube.h"

#include "widget/toastmsg.h"

#include "common/global.h"
#include "common/networkhttp.h"

#include <QJsonArray>


const int HTTP_NETWORK_TRACK_HEART_SET = 99;
const int HTTP_NETWORK_TRACK_INFO = 98;

FrameRoseTube::FrameRoseTube(OptionPopup::TypeMenu p_typeMenu_optionPopup, bool flagSmallUI, QWidget *parent)
    : FrameWidget(Type::RoseTube, parent)
    , flagSmallUI(flagSmallUI)
{
    this->typeMenu_optionPopup = p_typeMenu_optionPopup;

    setInit();
    setUIControl();
}


FrameRoseTube::~FrameRoseTube(){

    this->deleteLater();
}


/**
 * @brief FrameRoseTop20::setInit
 */
void FrameRoseTube::setInit(){

    linker = Linker::getInstance();
    data_RT_Track = new Data_RT_Track(this);
    dataPopup = new DataPopup(this);
}


/**
 * @brief FrameRoseTop20::setUIControl : [override] UI 세팅
 */
void FrameRoseTube::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    this->lb_img = new ImageLabel();
    if(flagSmallUI){
        lb_img->setType(ImageLabel::Type::RoseTube_s);
    }else{
        lb_img->setType(ImageLabel::Type::RoseTube);
    }

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 4;

    QWidget *label_base = new QWidget();
    label_base->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setAlignment(Qt::AlignVCenter);
    this->lb_title->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

    this->lb_artist = new QLabel(label_base);
    this->lb_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_artist->setStyleSheet("font-size:14px;color:#999999;");
    this->lb_artist->setGeometry(0, (this->LABEL_HEIGHT * 2), img_width, this->LABEL_HEIGHT);

    this->lb_num = new QLabel(label_base);
    this->lb_num->setStyleSheet("font-size:14px;color:#999999;");
    this->lb_num->setGeometry(0, (this->LABEL_HEIGHT * 3), img_width, this->LABEL_HEIGHT);

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
 * @brief FrameRoseTop20::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameRoseTube::setData(const QJsonObject &p_jsonObject){

    data_RT_Track->setData(p_jsonObject);
    setPopupData();

    lb_img->setFavorStatus(data_RT_Track->getFavorites() == "1" ? true : false);
    this->dataJsonObj = p_jsonObject;

    if(flagNeedTrackData){
        requestTrackInfo();
    }
}


/**
 * @brief FrameRoseTube::getData : 트랙 데이터 JSONObject 반환
 * @return QJsonObject
 */
QJsonObject FrameRoseTube::getData(){

    return data_RT_Track->getDataJson();
}


void FrameRoseTube::setDataFromCrawling(const QJsonObject &p_jsonObject){

    flagCrawlingData = true;
    data_RT_Track->setDataFromCrawling(p_jsonObject);
    setPopupData();

    if(flagNeedTrackData){
        requestTrackInfo();
    }
}


/**
 * @brief FrameRoseTop20::setPopupData : 옵션 팝업용 데이터 클래스 세팅
 * @details DataAlbum 세팅 이후에 호출
 */
void FrameRoseTube::setPopupData(){

    QString videoID = data_RT_Track->getPlayurl().split("?v=").last();

    dataPopup->setId(videoID);
    dataPopup->setChannelId(data_RT_Track->getChannelId());
    dataPopup->setTitle(data_RT_Track->getTitle());
    dataPopup->setArtist(data_RT_Track->getChannelTitle());
    dataPopup->setChannelName(data_RT_Track->getChannelTitle());
    dataPopup->setData(data_RT_Track->getPlayurl());
    dataPopup->setThumbnailUrl(data_RT_Track->getThumbnail());
    dataPopup->setFavorite_id(data_RT_Track->getNo());
}


/**
 * @brief FrameRoseTube::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameRoseTube::getDataForPopup(){

    QJsonArray jsonArrAlbum;
    jsonArrAlbum.append(dataPopup->getJsonData());

    QString tmp_playUrl = data_RT_Track->getPlayurl();

    QJsonObject jsonData_sub;
    jsonData_sub.insert("channel_id", this->data_RT_Track->getChannelId());
    jsonData_sub.insert("title", this->data_RT_Track->getChannelTitle());
    QJsonObject jsonData;
    jsonData.insert(KEY_OP_TITLE_MAIN, data_RT_Track->getTitle());
    jsonData.insert(KEY_OP_TITLE_SUB, data_RT_Track->getChannelTitle());
    //jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_OP_TYPE, this->typeMenu_optionPopup);
    jsonData.insert(KEY_OP_albumImg, data_RT_Track->getThumbnail());
    jsonData.insert(KEY_OP_cntStar, data_RT_Track->getStar());
    jsonData.insert(KEY_OP_ROSETUBE_PLAYURL, tmp_playUrl);
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::RoseTube);
    jsonData.insert(KEY_DATA, jsonData_sub);    // data 추가 (for 구독정보 페이지 이동)
    jsonData.insert(KEY_OP_PLAYLISTNO, data_RT_Track->getPlaylistno());

    return jsonData;
}


/**
 * @brief FrameRoseTop20::setHover : [override] 호버 세팅
 */
void FrameRoseTube::setHover(){

    lb_img->setHover();
}


void FrameRoseTube::setHover2(){

    lb_img->setHover2();
}


/**
 * @brief FrameRoseTube::requestTrackInfo : 트랙 정보 요청
 */
void FrameRoseTube::requestTrackInfo(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playurl", data_RT_Track->getPlayurl());

    network->request(HTTP_NETWORK_TRACK_INFO
                     ,QString("%1/playlist/track/get").arg(global.legacy_v1)
                     , params
                     , true
                     , true);
}


/**
 * @brief FrameRoseTube::requestChangeTrackFavorites
 */
void FrameRoseTube::requestChangeTrackFavorites(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playurl", data_RT_Track->getPlayurl());

    network->request(HTTP_NETWORK_TRACK_HEART_SET
                     , QString("%1/playlist/track/favorites").arg(global.legacy_mod)
                     , params
                     , true
                     , true);
}


/**
 * @brief FrameRoseTube::setResultOfGetTrackInfo : 트랙정보 요청결과 처리
 * @param p_jsonObject QJsonObject response
 * @note Favorite, 별점 추가 전에는 flagOk false 반환,
 */
void FrameRoseTube::setResultOfGetTrackInfo(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_favorites = "favorites";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_favorites)){
            QString tmp_favorite = p_jsonObject[jsonKey_favorites].toString();
            data_RT_Track->setFavorites(tmp_favorite);
            lb_img->setFavorStatus(tmp_favorite=="1"?true:false);
        }
    }
}


/**
 * @brief FrameRoseTube::setResultOfChangeTrackFavorites : 트랙 하트 업데이트 요청결과처리
 * @param p_jsonObject
 */
void FrameRoseTube::setResultOfChangeTrackFavorites(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_result) && p_jsonObject[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
            linker->signal_favoriteTrackChanged();
        }
    }

    if(!flagOk){
        //changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("Unable to change the Favorites setting."));
        //ToastMsg::show(this,"",tr("즐겨찾기 설정을 변경할 수 없습니다."));
    }
}


/**
 * @brief FrameRoseTube::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameRoseTube::slot_clickedHover(const QString &p_code){

    if(p_code==HOVER_CODE_FAV){
        requestChangeTrackFavorites();
    }else{
        QJsonObject tmpSend = getDataForPopup();
        tmpSend.insert("rosetubeData", this->dataJsonObj);
        emit linker->signal_clickedHoverItem(p_code, tmpSend);
    }
}


/**
 * @brief FrameRoseTube::slot_clickedItem : [슬롯] 아이템 클릭
 * @see OptionPopup:slot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameRoseTube::slot_clickedItem(){

    emit linker->signal_clickedHoverItem(HOVER_CODE_PLAY, getDataForPopup());
}


void FrameRoseTube::paintEvent(QPaintEvent *event){
    FrameWidget::paintEvent(event);

    if(flagNeedDraw){
        flagNeedDraw = false;

        int all_width = 0;

        QString image_bg_path = "";
        if(data_RT_Track->getThumbnail().isEmpty()){
            image_bg_path = ":/images/def_tube_340.png";
        }
        else{
            image_bg_path = data_RT_Track->getThumbnail();
        }

        this->lb_img->setImageUrl(QString("%1").arg(image_bg_path));
        all_width = this->lb_img->sizeHint().width();

        QString title = this->data_RT_Track->getTitle();

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

            this->lb_artist->setGeometry(0, (LABEL_HEIGHT * 2), all_width, LABEL_HEIGHT);
            this->lb_num->setGeometry(0, (LABEL_HEIGHT * 3), all_width, LABEL_HEIGHT);
        }
        else {
            this->lb_title->setGeometry(0, 0, IMG_WIDTH, this->LABEL_HEIGHT);
            this->lb_title->setText(title);

            this->lb_artist->setGeometry(0, LABEL_HEIGHT, all_width, LABEL_HEIGHT);
            this->lb_num->setGeometry(0, (LABEL_HEIGHT * 2), all_width, LABEL_HEIGHT);
        }

        QString tmpArtist = GSCommon::getTextCutFromLabelWidth(data_RT_Track->getChannelTitle(), all_width, lb_artist->font());

        this->lb_artist->setAlignment(Qt::AlignVCenter);
        this->lb_artist->setText(tmpArtist);

        QString tmpNumber = "";
        if(flagCrawlingData){
            tmpNumber = data_RT_Track->getViewCount();  //tr("View Count") +
            tmpNumber = GSCommon::getTextCutFromLabelWidth(tmpNumber, all_width, lb_artist->font());

            this->lb_num->setAlignment(Qt::AlignVCenter);
            this->lb_num->setText(tmpNumber);
        }else{
            int tmp_viewCount = data_RT_Track->getViewCount().toInt();

            if(tmp_viewCount > 0){
                tmpNumber = QString(tr("View Count") + " %1").arg(QLocale(QLocale::English).toString(tmp_viewCount));
                tmpNumber = GSCommon::getTextCutFromLabelWidth(tmpNumber, all_width, lb_artist->font());

                this->lb_num->setAlignment(Qt::AlignVCenter);
                this->lb_num->setText(tmpNumber);
            }
            else{
                this->lb_num->setText(QString(tr("")));
            }
        }

        if(!data_RT_Track->getDuration().isEmpty()){
            if(flagCrawlingData){
                lb_img->setDuration(data_RT_Track->getDuration());
            }else{
                lb_img->setDuration(data_RT_Track->getConvertDuration());
            }
        }
    }
}


/**
 * @brief RoseTubeHome::slot_responseHttp : [슬롯] Http 요청 결과 처리
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void FrameRoseTube::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){

    if(p_id == HTTP_NETWORK_TRACK_HEART_SET){
        setResultOfChangeTrackFavorites(p_jsonData);
    }
    else if(p_id == HTTP_NETWORK_TRACK_INFO){
        setResultOfGetTrackInfo(p_jsonData);
    }
}


/**
 * @brief FrameRoseTube::setRequestTrackInfoAfterSetData : 데이터 세팅 후 트랙 데이터 요청 유무
 * @note if true, setData 이후 트랙정보 요청
 */
void FrameRoseTube::setRequestTrackInfoAfterSetData(){

    flagNeedTrackData = true;
}
