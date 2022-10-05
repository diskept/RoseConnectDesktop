#include "ItemArtist_bugs.h"
#include "ItemImage_Artist_bugs.h"

#include <QJsonArray>
#include "common/gscommon.h"


namespace bugs {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;         ///< 옆에 있는 다른 ItemArtist 과의 가로 거리 (space)

    /**
     * @brief
     * @param parent
     */
    ItemArtist_bugs::ItemArtist_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : AbstractItem(index, section, imageSizeMode, parent) {

        this->setUIControl();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemArtist_bugs::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Artist_bugs(this->get_imageWidth(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Artist_bugs::signal_clickedBtn, this, &ItemArtist_bugs::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 2;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_artist = new QLabel(label_base);
        this->label_artist->setStyleSheet("font-size:16px; color: #FFFFFF;");
        this->label_artist->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        this->label_artist->setWordWrap(true);
        this->label_artist->setGeometry(0, 0, img_width, this->LABEL_HEIGHT * 2);

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
     * @brief [override] 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemArtist_bugs::setData(const bugs::ArtistItemData &p_data_artist){

        this->data_artist = p_data_artist;
    }


    /**
     * @brief ItemArtist::get_fixedHeight
     * @return
     */
    int ItemArtist_bugs::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += this->get_spaceImageAndTitle(this->m_imageSizeMode);
        height += this->LABEL_HEIGHT;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemArtist_bugs::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            this->itemImage->setImage(this->data_artist.image);

            int all_width = this->itemImage->sizeHint().width();

            QString artist = this->data_artist.artist_nm;
            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
            tmp_wordwrap->setText(artist);

            int title_width = tmp_wordwrap->sizeHint().width();

            if(title_width > all_width){

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_title_line1 = "";

                tmp_split = artist;
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

                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, title_width, label_artist->font()));
            }
            else{
                this->label_artist->setText(artist);
            }
        }
    }
}
