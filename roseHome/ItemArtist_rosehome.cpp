#include "roseHome/ItemArtist_rosehome.h"
#include "roseHome/ItemImage_Artist_rosehome.h"

#include "common/gscommon.h"


namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;         ///< 옆에 있는 다른 ItemArtist 과의 가로 거리 (space)

    /**
     * @brief
     * @param parent
     */
    ItemArtist_roseHome::ItemArtist_roseHome(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : AbstractItem(index, section, imageSizeMode, parent) {

        this->setUIControl();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemArtist_roseHome::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Artist_roseHome(this->get_imageWidth(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Artist_roseHome::signal_clickedBtn, this, &ItemArtist_roseHome::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 2  + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_artist = new QLabel(label_base);
        this->label_artist->setStyleSheet("font-size:16px; color: #FFFFFF;");
        this->label_artist->setAlignment(Qt::AlignCenter);
        this->label_artist->setGeometry(0, 0, img_width, this->LABEL_HEIGHT);

        this->label_info = new QLabel(label_base);
        this->label_info->setStyleSheet("font-size:16px; color: #999999;");
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
    void ItemArtist_roseHome::setData(const roseHome::ArtistItemData &p_data_artist){

        this->data_artist = p_data_artist;
    }


    /**
     * @brief ItemArtist::get_fixedHeight
     * @return
     */
    int ItemArtist_roseHome::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += this->get_spaceImageAndTitle(this->m_imageSizeMode);
        height += this->LABEL_HEIGHT;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemArtist_roseHome::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            int all_width = this->itemImage->sizeHint().width();
            this->itemImage->setImage(data_artist.imageUrl);
            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(data_artist.name, all_width, label_artist->font()));

            QString info = "";
            if(this->data_artist.albums_count > 0 || this->data_artist.tracks_count > 0){
                info = QString("%1").arg(this->data_artist.albums_count) + tr(" Albums ") + QString("%1").arg(this->data_artist.tracks_count) + tr(" Tracks");
            }
            this->label_info->setText(info);
        }
    }
}
