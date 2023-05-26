#include "roseHome/ItemArtist_rosehome.h"
#include "roseHome/ItemImage_Artist_rosehome.h"

#include "common/gscommon.h"


namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;         ///< 옆에 있는 다른 ItemArtist 과의 가로 거리 (space)

    /**
     * @brief
     * @param parent
     */
    ItemArtist_rosehome::ItemArtist_rosehome(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : AbstractItem(index, section, imageSizeMode, parent) {

        this->setUIControl();
    }


    ItemArtist_rosehome::~ItemArtist_rosehome(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemArtist_rosehome::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Artist_roseHome(this->get_imageWidth(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Artist_roseHome::signal_clickedBtn, this, &ItemArtist_rosehome::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 2  + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_artist = new QLabel(label_base);
        this->label_artist->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 1.25;text-align: center; color: #ffffff;");
        this->label_artist->setAlignment(Qt::AlignCenter);
        this->label_artist->setGeometry(0, 0, img_width, this->LABEL_HEIGHT);

        this->label_info = new QLabel(label_base);
        this->label_info->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 1.25;text-align: center; color: #999999;");
        this->label_info->setAlignment(Qt::AlignCenter);
        this->label_info->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
    }


    /**
     * @brief [override] 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemArtist_rosehome::setData(const roseHome::ArtistItemData &p_data_artist){

        this->data_artist = p_data_artist;
    }


    /**
     * @brief ItemArtist::get_fixedHeight
     * @return
     */
    int ItemArtist_rosehome::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS;

        return height;
    }

    int ItemArtist_rosehome::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemArtist_rosehome::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemArtist_rosehome::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            if(!data_artist.type.isEmpty()){
                int all_width = this->itemImage->sizeHint().width();

                QString image_type = "";
                if(this->data_artist.type == "MUSIC"){
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_artist.type == "VIDEO"){
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_artist.type == "YOUTUBE"){
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_artist.type == "TIDAL"){
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_artist.type == "BUGS"){
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_artist.type == "QOBUZ"){
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_artist.type == "APPLE_MUSIC"){
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_type = ":/images/rosehome/home_all.png";
                }

                this->itemImage->setTypeImage(image_type);
                this->itemImage->setImage(data_artist.thumbnail);

                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(data_artist.name, all_width, label_artist->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(data_artist.name);//c230321
                    this->label_artist->setToolTipDuration(2000);//c230321
                }
            }
            else{
                int all_width = this->itemImage->sizeHint().width();
                this->itemImage->setImage(data_artist.imageUrl);
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(data_artist.name, all_width, label_artist->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(data_artist.name);//c230321
                    this->label_artist->setToolTipDuration(2000);//c230321
                }

                QString info = "";
                if(this->data_artist.albums_count > 0 || this->data_artist.tracks_count > 0){
                    info = QString("%1").arg(this->data_artist.albums_count) + tr(" Albums ") + QString("%1").arg(this->data_artist.tracks_count) + tr(" Tracks");
                }
                this->label_info->setText(info);
            }
        }
    }
}
