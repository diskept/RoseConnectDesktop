#include "framevideo.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "data/datavideo.h"
#include "widget/optionpopup.h"
#include "widget/toastmsg.h"
#include <QJsonArray>
#include <QTextCodec>

const int HTTP_VIDEO_INFO = 99;
const int HTTP_VIDEO_THUMBNAIL = 100;

FrameVideo::FrameVideo(QWidget *parent)
    : FrameWidget(Type::Video, parent)
{
    setInit();
    setUIControl();
}


FrameVideo::~FrameVideo(){

    this->deleteLater();
}

/**
 * @brief FrameVideo::setInit : [override] 초기 세팅
 */
void FrameVideo::setInit(){
    linker = Linker::getInstance();
    data = new DataVideoAlbum(this);
    dataPopupList = new QList<DataPopup*>();
}

/**
 * @brief FrameVideo::setUIControl : [override] UI 세팅
 */
void FrameVideo::setUIControl(){

    int base_height = 0;
    int img_width = 0;

    QWidget *label_base = new QWidget();

    this->lb_img = new ImageLabel();
    this->lb_img->setType(ImageLabel::Type::Video);
    this->lb_img->setHidden_favorite(true);
    this->lb_img->setHidden_more(true);

    img_width = this->lb_img->sizeHint().width();
    base_height = this->LABEL_HEIGHT * 2 + this->SPACE_LABELS * 2;
    label_base->setFixedSize(img_width, base_height);

    this->lb_title = new QLabel(label_base);
    this->lb_title->setWordWrap(true);
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
    this->lb_title->setFixedWidth(img_width);
    this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), img_width, this->LABEL_HEIGHT);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->setContentsMargins(MARGIN,MARGIN,MARGIN,MARGIN);
    boxLayout->setSpacing(0);
    boxLayout->addWidget(lb_img);
    //boxLayout->addWidget(lb_title);
    boxLayout->addWidget(label_base);

    this->setLayout(boxLayout);

    connect(lb_img, SIGNAL(clickedHoverItem(QString)), SLOT(slot_clickedHover(QString)));
    connect(lb_img, SIGNAL(clickedItem()), SLOT(slot_clickedItem()));
}

/**
 * @brief FrameVideo::requestVideo : 비디오 정보 요청
 */
void FrameVideo::requestVideo(){
    dataPopupList->clear();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject jsonParam;
    jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
    jsonParam.insert("filter", data->getAlbum());

    network->request(HTTP_VIDEO_INFO, QString("http://%1:%2/video_group_data")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port)
                     , jsonParam, true);
}

/**
 * @brief FrameVideo::requestVideoThumbnail : 비디오 썸네일 정보 요청
 */
void FrameVideo::requestVideoThumbnail(){

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QStringList strList = data->getVideoFilePath().split("/");
    QString strPath;

    if(strList.count() > 2){
        for(int i = 0; i < strList.count() - 1; i++){
            if(!strList[i].isEmpty()){
                strPath += "/" + strList[i];
            }
        }
    }
    else{
        strPath = strList[0];
    }

    QJsonArray jsonArrVideo;
    QJsonObject jsonVideo;
    jsonVideo.insert("ext", "");
    jsonVideo.insert("id", "");
    jsonVideo.insert("ip", "");
    jsonVideo.insert("isCloud", false);
    jsonVideo.insert("isDir", false);
    jsonVideo.insert("isFile", false);
    jsonVideo.insert("isNetworkAdd", false);
    jsonVideo.insert("isServer", false);
    jsonVideo.insert("isShare", false);
    jsonVideo.insert("isThumbNailGetting", false);
    jsonVideo.insert("isUp", false);
    jsonVideo.insert("name", "");
    jsonVideo.insert("path", strPath);
    jsonVideo.insert("pw", "");
    jsonVideo.insert("rcloneServe", "");
    jsonVideo.insert("smbParamPath", "");
    jsonVideo.insert("thumbnail", "");

    jsonArrVideo.append(jsonVideo);

    QJsonArray jsonArrMediaSrc;
    jsonArrMediaSrc.append("jpeg");
    jsonArrMediaSrc.append("jpg");

    QJsonObject tmp_json;
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    tmp_json.insert("musicType", "10");
    tmp_json.insert("music", jsonArrVideo);
    tmp_json.insert("mediaSrcType",jsonArrMediaSrc);

    network->request(HTTP_VIDEO_THUMBNAIL, QString("http://%1:%2/external_usb")
                     .arg(global.device.getDeviceIP())
                     .arg(global.port)
                     , tmp_json, true);
}

/**
 * @brief FrameVideo::setData : [override] 데이터 세팅
 * @param p_jsonObject 데이터 세팅
 */
void FrameVideo::setData(const QJsonObject &p_jsonObject){

    data->setData(p_jsonObject);

    QLabel *tmp_wordwrap = new QLabel();
    tmp_wordwrap->setText(data->getAlbum());
    //qDebug() << tmp_wordwrap->sizeHint().width();

    if(tmp_wordwrap->sizeHint().width() > IMG_WIDTH){
        if(data->getAlbum().contains(" ")){
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, (this->LABEL_HEIGHT * 2));

            tmp_wordwrap->clear();
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
            tmp_wordwrap->setWordWrap(true);
            tmp_wordwrap->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), (IMG_WIDTH * 2)-30, this->lb_title->font()));
            if(tmp_wordwrap->text().contains("…")){
                tmp_wordwrap->setToolTip(data->getAlbum());
                tmp_wordwrap->setToolTipDuration(2000);
            }

            if(tmp_wordwrap->sizeHint().height() > this->LABEL_HEIGHT * 2){
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), (IMG_WIDTH * 2)-80, this->lb_title->font()));
                if(this->lb_title->text().contains("…")){
                    this->lb_title->setToolTip(data->getAlbum());//c230321
                    this->lb_title->setToolTipDuration(2000);//c230321
                }
            }
            else{
                this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), (IMG_WIDTH * 2)-30, this->lb_title->font()));
                if(this->lb_title->text().contains("…")){
                    this->lb_title->setToolTip(data->getAlbum());//c230321
                    this->lb_title->setToolTipDuration(2000);//c230321
                }
            }
        }
        else{
            this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
            this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), IMG_WIDTH, lb_title->font()));//cheon-210708-album
            if(this->lb_title->text().contains("…")){
                this->lb_title->setToolTip(data->getAlbum());//c230321
                this->lb_title->setToolTipDuration(2000);//c230321
            }
        }
    }
    else if(tmp_wordwrap->sizeHint().width() == IMG_WIDTH){
        this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
        this->lb_title->setText(data->getAlbum());
    }
    else {
        this->lb_title->setGeometry(0, (this->SPACE_LABELS * 2), IMG_WIDTH, this->LABEL_HEIGHT);
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(data->getAlbum(), IMG_WIDTH, lb_title->font()));//cheon-210708-album
        if(this->lb_title->text().contains("…")){
            this->lb_title->setToolTip(data->getAlbum());//c230321
            this->lb_title->setToolTipDuration(2000);//c230321
        }
    }

    requestVideoThumbnail();
    requestVideo();
}

void FrameVideo::setResultOfVideoInfo(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";
    const QString jsonKey_data = "data";
    const QString jsonKey_mArr = "mArr";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

                if(p_jsonObject.contains(jsonKey_data)){
                    QJsonObject jsonData = p_jsonObject[jsonKey_data].toObject();

                    if(jsonData.contains(jsonKey_mArr)){
                        jsonArrVideos = jsonData[jsonKey_mArr].toArray();

                        for(int i = 0 ; i < jsonArrVideos.size(); i++){
                            setPopupData(jsonArrVideos.at(i).toObject());
                        }
                    }
                }
            //}
        }
    }
}

void FrameVideo::setResultofVideoThumbnail(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";

    QString coverImg;
    QString coverPath;
    QStringList strList;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        QJsonArray tmp_Array = ProcJsonEasy::getJsonArray(p_jsonObject, "externalArr");
        QJsonObject tmp_Obj = tmp_Array.at(0).toObject();

        coverImg = ProcJsonEasy::getString(tmp_Obj, "name");
        coverPath = ProcJsonEasy::getString(tmp_Obj, "path");

        if(!coverImg.isEmpty()){
            strList = coverImg.split(".");

            if(strList[strList.count()-1] == "jpg"){
                lb_img->setImageUrl(QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(coverPath));
                data->setCoverImg(coverPath);
            }
        }
    }
}

/**
 * @brief FrameVideo::setPopupData : 옵션 팝업용 데이터 클래스 세팅
 * @details DataAlbum 세팅 이후에 호출
 */
void FrameVideo::setPopupData(const QJsonObject &p_json){

    const QString jsonKey_id = "id";
    const QString jsonKey_videoDuration = "videoDuration";
    const QString jsonKey_videoFilePath = "videoFilePath";
    const QString jsonKey_videoTitle = "videoTitle";

    DataPopup *dataPopup = new DataPopup(this);

    if(p_json.contains(jsonKey_id)){
        dataPopup->setId(QString::number(p_json[jsonKey_id].toInt(0)));
    }
    if(p_json.contains(jsonKey_videoDuration)){
        dataPopup->setDuration(p_json[jsonKey_videoDuration].toInt(0));
    }
    if(p_json.contains(jsonKey_videoTitle)){
        dataPopup->setTitle(p_json[jsonKey_videoTitle].toString(""));
        dataPopup->setAlbum(dataPopup->getTitle());
    }
    if(p_json.contains(jsonKey_videoFilePath)){
        dataPopup->setFilePath(p_json[jsonKey_videoFilePath].toString(""));
        dataPopup->setData(p_json[jsonKey_videoFilePath].toString(""));
    }

    dataPopupList->append(dataPopup);
}

/**
 * @brief FrameVideo::getDataForPopup : [override] 데이터 반환
 * @return
 */
QJsonObject FrameVideo::getDataForPopup(){

    QJsonArray jsonArrAlbum;
    for(int i =0; i < dataPopupList->size(); i++){
        jsonArrAlbum.append(dataPopupList->at(i)->getJsonData());
    }

    QJsonObject jsonData;
    //jsonData.insert(KEY_OP_TYPE, type);
    jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Video);
    jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Video);
    jsonData.insert(KEY_OP_albumImg, "");
    jsonData.insert(KEY_OP_cntStar, 0);
    jsonData.insert(KEY_OP_DATA,jsonArrAlbum);

    return jsonData;
}
/**
 * @brief FrameVideo::setHover : [override] 호버 세팅
 */
void FrameVideo::setHover(){

    lb_img->setHover();
}

/**
 * @brief FrameVideo::slot_clickedHover : [슬롯] Hover 아이템 클릭
 * @param p_code
 */
void FrameVideo::slot_clickedHover(const QString &p_code){
    if(dataPopupList->count()>0){
        emit linker->signal_clickedHoverItem(p_code, getDataForPopup());
    }else{
        ToastMsg::show(this,"",tr("Video Information is temporarily unavailable."));
        //ToastMsg::show(this,"",tr("일시적으로 비디오 정보를 확인할 수 없습니다."));
    }

}

/**
 * @brief FrameVideo::slot_responseHttp : [슬롯] http 요청
 * @param p_id int 요청 아이디
 * @param p_jsonData QjsonObject response
 */
void FrameVideo::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){
    if(p_id == HTTP_VIDEO_INFO){
        setResultOfVideoInfo(p_jsonData);
    }
    else if(p_id == HTTP_VIDEO_THUMBNAIL){
        setResultofVideoThumbnail(p_jsonData);
    }
}

/**
 * @brief FrameVideo::slot_clickedItem : [슬롯] 아이템 클릭
 * @details 비디오 상세보기 페이지로 이동
 * 옵션 팝업의 signal OptionPopup:signal_clickedMovePage() 와 동일 구조
 * @see OptionPopup:slot_gotoPage()
 * @note hover item(하트/재생/더보기)를 제외한 영역 클릭
 */
void FrameVideo::slot_clickedItem(){

    QJsonObject jsonVideo;
    jsonVideo.insert("coverImg", data->getVideoCoverImg());
    jsonVideo.insert("folderName", data->getAlbum());
    jsonVideo.insert("arr_video", jsonArrVideos);

    if(dataPopupList->count() > 0){
        QJsonObject jsonData = getDataForPopup();
        jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_VIDEOINFO);
        jsonData.insert("video_info", jsonVideo);

        emit linker->signal_clickedMovePage(jsonData);
    }else{
        ToastMsg::show(this,"",tr("Video Information is temporarily unavailable."));
        //ToastMsg::show(this,"",tr("일시적으로 비디오 정보를 확인할 수 없습니다."));
    }
}

