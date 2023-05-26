#include "ItemRecommends_rosehome.h"
#include "ItemImage_Recommends_rosehome.h"

#include <QJsonArray>
#include "common/gscommon.h"

namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;
    const int ITEM_LABEL_HEIGHT = 25;

    /**
     * @brief
     * @param parent
     */
    ItemRecommends_rosehome::ItemRecommends_rosehome(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->setUIControl();
    }

    /**
     * @brief [override] UI 세팅
     */
    void ItemRecommends_rosehome::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Recommends_roseHome(this->get_imageWidth(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Recommends_roseHome::signal_clickedBtn, this, &ItemRecommends_rosehome::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = 10 + (ITEM_LABEL_HEIGHT * 2) + 30 + 10;

        this->label_base = new QWidget();
        this->label_base->setFixedSize(img_width, base_height);
        this->label_base->setStyleSheet("background-color:#333333; border-bottom-left-radius:8px; border-bottom-right-radius:8px;");

        this->label_title = new QLabel(this->label_base);
        this->label_title->setAlignment(Qt::AlignVCenter);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("background-color:transparent; font-size:18px;   font-weight: normal;font-style: normal;line-height: 1.39;text-align: left;color: #FFFFFF;");
        this->label_title->setGeometry(15, 10, (img_width - 30), (ITEM_LABEL_HEIGHT * 2));

        this->label_owner = new QLabel(this->label_base);
        this->label_owner->setAlignment(Qt::AlignVCenter);
        this->label_owner->setStyleSheet("background-color:transparent;font-size: 16px;font-weight: normal;font-style: normal;line-height: 1.56;text-align: left;color: #999999;");
        this->label_owner->setGeometry(15, 10 + (ITEM_LABEL_HEIGHT * 2), (img_width - 30), ITEM_LABEL_HEIGHT);

        QPixmap *img_thmub = GSCommon::getUIPixmapImg(":/images/rosehome/home_like_ico.png");

        this->image_thumb = new QLabel(this->label_base);
        this->image_thumb->setPixmap(*img_thmub);
        this->image_thumb->setFixedSize(30, 30);
        this->image_thumb->setGeometry(15, 10 + (ITEM_LABEL_HEIGHT * 3), 0, 0);

        int width = this->image_thumb->geometry().width();
        this->label_thumb = new QLabel(this->label_base);
        this->label_thumb->setAlignment(Qt::AlignVCenter);
        this->label_thumb->setStyleSheet("background-color:transparent; font-size:14px; font-weight:normal;  font-style: normal;line-height: 1.14; text-align: left;color:#FFFFFF;");
        this->label_thumb->setGeometry(15 + width, 10 + (ITEM_LABEL_HEIGHT * 3), img_width - (30 + width), 30);

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addWidget(this->label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += base_height;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
        this->setStyleSheet("background-color:transparent; border-top-left-radius:8px; border-top-right-radius:8px;");
        //this->setFixedHeight(this->get_fixedHeight());
    }


    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemRecommends_rosehome::setData(const roseHome::RecommendsItemData &p_data){
        this->data_recommends = p_data;
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemRecommends_rosehome::get_fixedHeight(){
        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10 + this->label_title->sizeHint().height() + ITEM_LABEL_HEIGHT + 30 + 10;

        return height;
    }

    int ItemRecommends_rosehome::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemRecommends_rosehome::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemRecommends_rosehome::paintEvent(QPaintEvent *event){
        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            int all_width = 0;
            //QString setHTML_owner = "";
            //QString setHTML_thumb = "";

            QString image_bg_path = "";
            QString image_type = "";
            if(this->data_recommends.thumbnail.isEmpty()){
                if(this->data_recommends.type == "MUSIC"){
                    image_bg_path = ":/images/def_mus_550.png";
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_recommends.type == "VIDEO"){
                    image_bg_path = ":/images/def_video_200x2.png";
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_recommends.type == "YOUTUBE"){
                    image_bg_path = ":/images/def_tube_340";
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_recommends.type == "TIDAL"){
                    image_bg_path = ":/images/tidal/tidal_def_400.png";
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_recommends.type == "BUGS"){
                    image_bg_path = ":/images/bugs/bugs_def_430.png";
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_recommends.type == "QOBUZ"){
                    image_bg_path = ":/images/qobuz/qobuz_default_400.png";
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_recommends.type == "APPLE_MUSIC"){
                    image_bg_path = ":/images/apple/apple_def.png";
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_bg_path = ":/images/rosehome/home_allx2.png";
                    image_type = ":/images/rosehome/home_all.png";
                }
            }
            else{
                image_bg_path = this->data_recommends.thumbnail;

                if(this->data_recommends.type == "MUSIC"){
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_recommends.type == "VIDEO"){
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_recommends.type == "YOUTUBE"){
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_recommends.type == "TIDAL"){
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_recommends.type == "BUGS"){
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_recommends.type == "QOBUZ"){
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_recommends.type == "APPLE_MUSIC"){
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_type = ":/images/rosehome/home_all.png";
                }
            }

            this->itemImage->setTypeImage(image_type);
            this->itemImage->setImage(image_bg_path);
            all_width = this->itemImage->sizeHint().width() - 30;

            QString title = this->data_recommends.title;
            QString owner = this->data_recommends.ownerName;
            QString thumb_count = QString("%1").number(this->data_recommends.thumbupCount) + " | " + QString("%1").number(this->data_recommends.trackCount) + tr(" Tracks");

            qDebug() << owner;
            /*QString owner = GSCommon::getTextCutFromLabelWidth(data_recommends.ownerName, all_width, this->label_owner->font());
            setHTML_owner = QString("<html><head/><body><span style='font-size:16px; font-weight:300; line-height: 3.38; color:#999999;'>%1</span></body></html>").arg("by " + owner);

            QString thumb_count ="";
            thumb_count = QString("%1").number(this->data_recommends.thumbupCount);
            thumb_count += " | " + QString("%1").number(this->data_recommends.trackCount) + tr(" Tracks");
            setHTML_thumb = QString("<html><head/><body><span style='font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;'>%1</span></body></html>").arg(thumb_count);*/

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:20px; font-weight:normal; line-height: 1.25; color:#FFFFFF;");
            tmp_wordwrap->setText(title);

            int title_width = tmp_wordwrap->sizeHint().width();

            int width = this->image_thumb->geometry().width();

            if(title_width > all_width){
                int height = 10 + (ITEM_LABEL_HEIGHT * 3) + 30 + 10;
                this->label_base->setFixedSize(this->itemImage->sizeHint().width(), height);
                height += this->itemImage->sizeHint().height();
                this->setFixedHeight(height);

                this->label_title->setGeometry(15, 10, all_width, (ITEM_LABEL_HEIGHT * 2));
                this->label_owner->setGeometry(15, 10 + (ITEM_LABEL_HEIGHT * 2), all_width, ITEM_LABEL_HEIGHT);
                this->image_thumb->setGeometry(15, 10 + (ITEM_LABEL_HEIGHT * 3), 0, 0);
                this->label_thumb->setGeometry(15 + width, 10 + (ITEM_LABEL_HEIGHT * 3), all_width - (30 + width), 30);

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

                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }

                this->label_owner->setText(GSCommon::getTextCutFromLabelWidth(owner, all_width - 30, this->label_owner->font()));
                if(this->label_owner->text().contains("…")){
                    this->label_owner->setToolTip(owner);//c230321
                    this->label_owner->setToolTipDuration(2000);//c230321
                }

                this->label_thumb->setText(thumb_count);
            }
            else{
                this->label_title->setGeometry(15, 10, all_width, ITEM_LABEL_HEIGHT);
                this->label_owner->setGeometry(15, 10 + ITEM_LABEL_HEIGHT, all_width, ITEM_LABEL_HEIGHT);
                this->image_thumb->setGeometry(15, 10 + (ITEM_LABEL_HEIGHT * 2), 0, 0);
                this->label_thumb->setGeometry(15 + width, 10 + (ITEM_LABEL_HEIGHT * 2), all_width - (30 + width), 30);

                this->label_title->setText(title);
                this->label_owner->setText(owner);
                this->label_thumb->setText(thumb_count);
            }
        }
    }
}
