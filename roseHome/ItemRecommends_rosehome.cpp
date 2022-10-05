#include "ItemRecommends_rosehome.h"
#include "ItemImage_Recommends_rosehome.h"

#include <QJsonArray>
#include "common/gscommon.h"

namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;
    const int ITEM_LABEL_HEIGHT = 25;
    const int ITEM_LABEL_SPACE = 5;

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
        base_height = 35 + (ITEM_LABEL_HEIGHT * 3) + 30;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);
        label_base->setStyleSheet("background-color:#333333");

        this->label_title = new QLabel(label_base);
        this->label_title->setTextFormat(Qt::RichText);
        this->label_title->setAlignment(Qt::AlignVCenter);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:20px; font-weight:normal; line-height: 1.25; color:#FFFFFF;");
        this->label_title->setGeometry(15, 15, (img_width - 30), (ITEM_LABEL_HEIGHT * 2));

        this->label_owner = new QLabel(label_base);
        this->label_owner->setTextFormat(Qt::RichText);
        this->label_owner->setAlignment(Qt::AlignVCenter);
        this->label_owner->setStyleSheet("font-size:16px; font-weight:300; line-height: 3.38; color:#999999;");
        this->label_owner->setGeometry(15, (ITEM_LABEL_HEIGHT * 2) + ITEM_LABEL_SPACE + 15, (img_width - 30), (ITEM_LABEL_HEIGHT - 5));

        QPixmap *img_thmub = GSCommon::getUIPixmapImg(":/images/rosehome/home_like_ico.png");

        this->image_thumb = new QLabel(label_base);
        this->image_thumb->setPixmap(*img_thmub);
        this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
        this->image_thumb->setGeometry(15, (ITEM_LABEL_HEIGHT * 3) + 15, 0, 0);

        int width = this->image_thumb->geometry().width();
        int top = this->image_thumb->geometry().bottom();
        this->label_thumb = new QLabel(label_base);
        this->label_thumb->setTextFormat(Qt::RichText);
        this->label_thumb->setAlignment(Qt::AlignVCenter);
        this->label_thumb->setStyleSheet("font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;");
        this->label_thumb->setGeometry(20 + width, (top - ITEM_LABEL_HEIGHT - 10) + 15, img_width - (35 + width), (ITEM_LABEL_HEIGHT - 5));

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 35;
        height += (ITEM_LABEL_HEIGHT * 3) + width;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
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
        height += this->get_spaceImageAndTitle(this->m_imageSizeMode);
        height += 105;

        return height;
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
            QString setHTML_owner = "";
            QString setHTML_thumb = "";

            QString image_bg_path = "";
            QString image_type = "";
            if(this->data_recommends.thumbnail.isEmpty()){
                if(this->data_recommends.type == "MUSIC"){
                    image_bg_path = ":/images/def_mus_300.png";
                    image_type = ":/images/rosehome/home_musicx2.png";
                }
                else if(this->data_recommends.type == "VIDEO"){
                    image_bg_path = ":/images/rosehome/def_mus_300.png";
                    image_type = ":/images/rosehome/home_videox2.png";
                }
                else if(this->data_recommends.type == "YOUTUBE"){
                    image_bg_path = ":/images/rosehome/def_mus_300.png";
                    image_type = ":/images/rosehome/home_tubex2.png";
                }
                else if(this->data_recommends.type == "TIDAL"){
                    image_bg_path = ":/images/rosehome/tidal/tidal_def_400.png";
                    image_type = ":/images/rosehome/home_tidalx2.png";
                }
                else if(this->data_recommends.type == "BUGS"){
                    image_bg_path = ":/images/rosehome/bugs/bugs_def_430.png";
                    image_type = ":/images/rosehome/home_bugsx2.png";
                }
                else if(this->data_recommends.type == "QOBUZ"){
                    image_bg_path = ":/images/rosehome/qobuz/qobuz_default_400.png";
                    image_type = ":/images/rosehome/home_qobuzx2.png";
                }
                else if(this->data_recommends.type == "APPLE_MUSIC"){
                    image_bg_path = ":/images/apple/apple_def.png";
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_bg_path = ":/images/rosehome/home_allx2.png";
                    image_type = ":/images/rosehome/home_allx2.png";
                }
            }
            else{
                image_bg_path = this->data_recommends.thumbnail;

                if(this->data_recommends.type == "MUSIC"){
                    image_type = ":/images/rosehome/home_musicx2.png";
                }
                else if(this->data_recommends.type == "VIDEO"){
                    image_type = ":/images/rosehome/home_videox2.png";
                }
                else if(this->data_recommends.type == "YOUTUBE"){
                    image_type = ":/images/rosehome/home_tubex2.png";
                }
                else if(this->data_recommends.type == "TIDAL"){
                    image_type = ":/images/rosehome/home_tidalx2.png";
                }
                else if(this->data_recommends.type == "BUGS"){
                    image_type = ":/images/rosehome/home_bugsx2.png";
                }
                else if(this->data_recommends.type == "QOBUZ"){
                    image_type = ":/images/rosehome/home_qobuzx2.png";
                }
                else if(this->data_recommends.type == "APPLE_MUSIC"){
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_type = ":/images/rosehome/home_allx2.png";
                }
            }

            this->itemImage->setTypeImage(image_type);
            this->itemImage->setImage(image_bg_path);
            all_width = this->itemImage->sizeHint().width() - 30;

            QString title = this->data_recommends.title;

            QString owner = GSCommon::getTextCutFromLabelWidth(data_recommends.ownerName, all_width, this->label_owner->font());
            setHTML_owner = QString("<html><head/><body><span style='font-size:16px; font-weight:300; line-height: 3.38; color:#999999;'>%1</span></body></html>").arg("by " + owner);

            QString thumb_count ="";
            thumb_count = QString("%1").number(this->data_recommends.thumbupCount);
            thumb_count += " | " + QString("%1").number(this->data_recommends.trackCount) + tr(" Tracks");
            setHTML_thumb = QString("<html><head/><body><span style='font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;'>%1</span></body></html>").arg(thumb_count);

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:20px; font-weight:normal; line-height: 1.25; color:#FFFFFF;");
            tmp_wordwrap->setText(title);

            int title_width = tmp_wordwrap->sizeHint().width();

            int width = this->image_thumb->geometry().width();

            if(title_width > all_width){
                this->label_title->setGeometry(15, 15, all_width, (ITEM_LABEL_HEIGHT * 2));
                this->label_owner->setGeometry(15, (ITEM_LABEL_HEIGHT * 2) + ITEM_LABEL_SPACE + 15, all_width, (ITEM_LABEL_HEIGHT - 5));
                this->image_thumb->setGeometry(15, (ITEM_LABEL_HEIGHT * 3) + ITEM_LABEL_SPACE + 15, 0, 0);
                this->label_thumb->setGeometry(20 + width, (ITEM_LABEL_HEIGHT * 3) + (ITEM_LABEL_SPACE * 2) + 15, all_width - (40 + width), (ITEM_LABEL_HEIGHT - 5));

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
                this->label_owner->setText(setHTML_owner);
                this->label_thumb->setText(setHTML_thumb);
            }
            else{
                this->label_title->setGeometry(15, 15, all_width, ITEM_LABEL_HEIGHT);
                this->label_owner->setGeometry(15, (ITEM_LABEL_HEIGHT + ITEM_LABEL_SPACE) + 15, all_width, (ITEM_LABEL_HEIGHT - 5));
                this->image_thumb->setGeometry(15, (ITEM_LABEL_HEIGHT * 2) + ITEM_LABEL_SPACE + 15, 0, 0);
                this->label_thumb->setGeometry(20 + width, (ITEM_LABEL_HEIGHT * 2) + (ITEM_LABEL_SPACE * 2) + 15, all_width - (40 + width), (ITEM_LABEL_HEIGHT - 5));

                this->label_title->setText(title);
                this->label_owner->setText(setHTML_owner);
                this->label_thumb->setText(setHTML_thumb);
            }
        }
    }
}
