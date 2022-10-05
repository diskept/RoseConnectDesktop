#include "bugs/ItemAlbum_bugs.h"
#include "bugs/ItemImage_Album_bugs.h"

#include "common/gscommon.h"


namespace bugs {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;                 ///< 옆에 있는 다른 ItemAlbum 과의 가로 거리 (space)


    /**
     * @brief
     * @param parent
     */
    ItemAlbum_bugs::ItemAlbum_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->setUIControl();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemAlbum_bugs::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Album_bugs(this->get_imageWidth(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Album_bugs::signal_clickedBtn, this, &ItemAlbum_bugs::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:16px; color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, this->LABEL_HEIGHT * 2);

        this->label_artist = new QLabel(label_base);
        this->label_artist->setStyleSheet("font-size:16px; color: #999999;");
        this->label_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->setSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
        //this->setFixedHeight(this->get_fixedHeight()+this->LABEL_HEIGHT);
    }


    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemAlbum_bugs::setData(const bugs::AlbumItemData &p_data_album){

        this->data_album = p_data_album;
    }


    /**
     * @brief ItemAlbum_bugs::get_fixedHeight
     * @return
     */
    int ItemAlbum_bugs::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemAlbum_bugs::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            this->itemImage->setImage(this->data_album.album_image);
            //this->itemImage->setFavorite(this->data_album.likes_yn);
            int all_width = this->itemImage->sizeHint().width();

            QString title_ver = "";

            /*if(this->data_album.version.isEmpty()){
                title_ver = this->data_album.title;
            }
            else{
                title_ver = this->data_album.title + QString(" (%1)").arg(this->data_album.version);
            }*/
            title_ver = this->data_album.title;

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
            tmp_wordwrap->setText(title_ver);

            int title_width = tmp_wordwrap->sizeHint().width();


            if(title_width > all_width){

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_title_line1 = "";

                tmp_split = title_ver;
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
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title_ver, title_width, this->label_title->font()));

                this->label_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(this->data_album.list_artist_nm.join(","), all_width, this->label_artist->font()));
            }
            else{
                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                this->label_title->setText(title_ver);

                this->label_artist->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(this->data_album.list_artist_nm.join(","), all_width, this->label_artist->font()));
            }
        }
    }
}
