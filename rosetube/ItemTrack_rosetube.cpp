#include "rosetube/ItemTrack_rosetube.h"
#include "rosetube/ItemImage_Track_rosetube.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QJsonArray>


namespace rosetube {

    const int LABEL_HEIGHT_RT_TRACK = 25;
    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;

    /**
     * @brief
     * @param parent
     */
    ItemTrack_rosetube::ItemTrack_rosetube(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->flag_btnPlay = flagPlay;
        this->setUIControl();
    }


    ItemTrack_rosetube::~ItemTrack_rosetube(){

        this->deleteLater();
    }

    /**
     * @brief [override] UI 세팅
     */
    void ItemTrack_rosetube::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Track_rosetube(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(m_imageSizeMode), this->flag_btnPlay);
        connect(this->itemImage, &ItemImage_Track_rosetube::signal_clickedBtn, this, &ItemTrack_rosetube::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = LABEL_HEIGHT_RT_TRACK * 2 + this->LABEL_HEIGHT * 2;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);
        this->label_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
        this->label_title->setAlignment(Qt::AlignVCenter);
        this->label_title->setGeometry(0, 0, img_width, (LABEL_HEIGHT_RT_TRACK * 2));

        this->label_artist = new QLabel(label_base);
        this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);
        this->label_artist->setStyleSheet("font-size:14px;color:#999999;");
        this->label_artist->setAlignment(Qt::AlignVCenter);
        this->label_artist->setGeometry(0, (LABEL_HEIGHT_RT_TRACK * 2), img_width, this->LABEL_HEIGHT);

        this->label_number = new QLabel(label_base);
        this->label_number->setStyleSheet("font-size:14px;color:#999999;");
        this->label_number->setGeometry(0, (LABEL_HEIGHT_RT_TRACK * 2) + this->LABEL_HEIGHT, img_width, this->LABEL_HEIGHT);

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += LABEL_HEIGHT_RT_TRACK * 2 + this->LABEL_HEIGHT * 2;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
    }



    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemTrack_rosetube::setData(const QJsonObject &jsonObj){

        this->data_track = jsonObj;
    }


    void ItemTrack_rosetube::setCacheImgShow(bool flag){

        this->itemImage->setCacheShow(flag);
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemTrack_rosetube::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += LABEL_HEIGHT_RT_TRACK * 2 + this->LABEL_HEIGHT * 2;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemTrack_rosetube::paintEvent(QPaintEvent *event){
        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            int all_width = 0;

            QString image_bg_path = ProcJsonEasy::getString(this->data_track, "thumbnailUrl");
            QString image_cache = ":/images/rosetube/cache_lock_ico.png";

            this->itemImage->setTypeCache(image_cache);
            this->itemImage->setImage(image_bg_path);
            all_width = this->itemImage->sizeHint().width();

            QString title = ProcJsonEasy::getString(this->data_track, "title");

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

                this->label_title->setGeometry(0, 0, all_width, (LABEL_HEIGHT_RT_TRACK * 2));
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));

                this->label_artist->setGeometry(0, (LABEL_HEIGHT_RT_TRACK * 2), all_width, this->LABEL_HEIGHT);
                this->label_number->setGeometry(0, (LABEL_HEIGHT_RT_TRACK * 2) + this->LABEL_HEIGHT, all_width, this->LABEL_HEIGHT);
            }
            else {
                this->label_title->setGeometry(0, 0, all_width, LABEL_HEIGHT_RT_TRACK);
                this->label_title->setText(title);

                this->label_artist->setGeometry(0, LABEL_HEIGHT_RT_TRACK, all_width, this->LABEL_HEIGHT);
                this->label_number->setGeometry(0, LABEL_HEIGHT_RT_TRACK + this->LABEL_HEIGHT, all_width, this->LABEL_HEIGHT);
            }

            QJsonObject data;
            QString tmpArtist = "";
            if(this->data_track.contains("data")){
                data = ProcJsonEasy::getJsonObject(this->data_track, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                tmpArtist = ProcJsonEasy::getString(snippet, "channelTitle");
            }
            else{
                if(this->data_track.contains("channelTitle")){
                    tmpArtist = ProcJsonEasy::getString(this->data_track, "channelTitle");
                }
                else if(this->data_track.contains("channelName")){
                    tmpArtist = ProcJsonEasy::getString(this->data_track, "channelName");
                }
            }

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(tmpArtist, all_width, label_artist->font()));

            QString tmpview = "";
            if(this->data_track.contains("data")){
                QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(data, "trackStatistic");
                tmpview = ProcJsonEasy::getString(trackStatistic, "viewCount");

                if(tmpview.toInt() > 0){
                    QString tmpNumber = QString(tr("View Count") +" %1").arg(QLocale(QLocale::English).toString(tmpview.toInt()));
                    this->label_number->setText(GSCommon::getTextCutFromLabelWidth(tmpNumber, all_width, label_number->font()));
                }
            }
            else{
                int viewCNT = ProcJsonEasy::getInt(this->data_track, "viewCount");

                if(viewCNT > 0){
                    QString tmpNumber = QString(tr("View Count") +" %1").arg(QLocale(QLocale::English).toString(viewCNT));
                    this->label_number->setText(GSCommon::getTextCutFromLabelWidth(tmpNumber, all_width, label_number->font()));
                }
            }

            QString tmpDuration = "";
            QString setDuration = "";
            if(this->data_track.contains("data")){
                QJsonObject detailContents = ProcJsonEasy::getJsonObject(data, "detailContents");
                tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                if(!tmpDuration.isEmpty()){

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            setDuration = "00:" + second.rightJustified(2, '0');
                        }
                        else{
                            setDuration = minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        setDuration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    this->itemImage->setTypeDuration(setDuration);
                }
            }
            else{
                int time = ProcJsonEasy::getInt(this->data_track, "duration");

                if(time == 0){

                    QString tmpDur = ProcJsonEasy::getString(this->data_track, "duration");
                    int tmpTime = tmpDur.toInt();

                    if(tmpTime > 0){
                        if(time >= 3600){
                            setDuration = QDateTime::fromTime_t(tmpTime).toUTC().toString("hh:mm:ss");
                        }
                        else{
                            setDuration = QDateTime::fromTime_t(tmpTime).toUTC().toString("mm:ss");
                        }
                    }
                    else{
                        if(tmpDur.isEmpty()){
                            setDuration = "";
                        }
                        else{
                            QString tmp_data = tmpDur;
                            QStringList splitToken = tmp_data.split(":");

                            if(splitToken.size() > 1){
                                QString tmpMin = splitToken.at(0);

                                if(tmpMin.length() < 2){
                                    setDuration = tmpMin.rightJustified(2, '0') + ":" + splitToken.at(1);
                                }
                                else{
                                    setDuration = tmpDur;
                                }
                            }
                        }
                    }
                }
                else if(time >= 3600){
                     setDuration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                }
                else{
                    setDuration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                }
            }

            if(setDuration.length() > 0){
                this->itemImage->setTypeDuration(setDuration);
            }

            if(this->data_track.value("flag_lock").toBool()){
                this->itemImage->setCacheShow(true);
            }
            else{
                this->itemImage->setCacheShow(false);
            }
        }
    }
}
