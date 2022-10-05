#include "video/ItemWidget_video.h"
#include "video/ItemImage_Widget_video.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"


namespace video {

    const int HTTP_VIDEO_THUMBNAIL = 1;
    const int HTTP_VIDEO_INFO = 2;

    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;

    /**
     * @brief
     * @param parent
     */
    ItemWidget_video::ItemWidget_video(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->flag_btnPlay = flagPlay;
        this->setUIControl();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemWidget_video::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Widget_video(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(this->m_imageSizeMode), this->flag_btnPlay);
        connect(this->itemImage, &ItemImage_Widget_video::signal_clickedBtn, this, &ItemWidget_video::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 2;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:16px; color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += this->LABEL_HEIGHT * 2;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
    }


    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemWidget_video::setData(const QJsonObject &p_data){

        this->data_video = p_data;

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QString tmpPath = ProcJsonEasy::getString(this->data_video, "videoFilePath");
        QStringList strList = tmpPath.split("/");
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

        QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/external_usb").arg(global.port);
        network->request(HTTP_VIDEO_THUMBNAIL, url, tmp_json, true);
    }


    QJsonObject ItemWidget_video::get_videoInfo(){

        return this->video_Obj;
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemWidget_video::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += this->LABEL_HEIGHT * 2;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemWidget_video::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            int all_width = 0;
            int title_width = 0;

            QString title = ProcJsonEasy::getString(this->data_video, "album");

            all_width = this->itemImage->sizeHint().width();

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
            tmp_wordwrap->setText(title);

            title_width = tmp_wordwrap->sizeHint().width();

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

                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT * 2);

                if(title_width < 300){
                    this->label_title->setText(title);
                }
                else{
                    this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));
                }
            }
            else{
                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                this->label_title->setText(title);
            }
        }
    }


    void ItemWidget_video::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        const QString jsonKey_flagOk = "flagOk";

        if(p_id == HTTP_VIDEO_THUMBNAIL){
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

                    if(strList[strList.count() - 1] == "jpeg" || strList[strList.count() - 1] == "jpg"){
                        this->img_path = QString("http://%1:").arg(global.device.getDeviceIP()) + global.port_img + coverPath;
                        this->itemImage->setImage(this->img_path);
                    }
                }

                NetworkHttp *network_info = new NetworkHttp(this);
                connect(network_info, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QJsonObject jsonParam;
                jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
                jsonParam.insert("filter", ProcJsonEasy::getString(this->data_video, "album"));

                QString url = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1/video_group_data").arg(global.port);
                network_info->request(HTTP_VIDEO_INFO, url, jsonParam, true);
            }
        }
        else if(p_id == HTTP_VIDEO_INFO){
            const QString jsonKey_data = "data";
            const QString jsonKey_mArr = "mArr";

            QJsonArray jsonArrVideos;

            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
                if(p_jsonObject.contains(jsonKey_data)){
                    QJsonObject jsonData = p_jsonObject[jsonKey_data].toObject();

                    if(jsonData.contains(jsonKey_mArr)){
                        jsonArrVideos = jsonData[jsonKey_mArr].toArray();
                    }

                    this->video_Obj.insert("coverImg", this->img_path);
                    this->video_Obj.insert("folderName", ProcJsonEasy::getString(this->data_video, "album"));
                    this->video_Obj.insert("arr_video", jsonArrVideos);
                }
            }
        }

        sender()->deleteLater();
    }
}
