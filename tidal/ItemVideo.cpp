#include "ItemVideo.h"
#include "ItemImage_Video.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include "common/gscommon.h"


namespace tidal {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;             ///< 옆에 있는 다른 ItemAlbum 과의 가로 거리 (space)


    /**
     * @brief 생성자
     * @param parent
     */
    ItemVideo::ItemVideo(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : AbstractItem(index, section, imageSizeMode, parent) {

        setUIControl();
    }



    /**
     * @brief [override] UI 세팅
     */
    void ItemVideo::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Video(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Video::signal_clickedBtn, this, &ItemVideo::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);
        this->label_title->setStyleSheet("font-size:16px; color:#FFFFFF;");
        this->label_title->setFixedWidth(this->itemImage->sizeHint().width());
        this->label_title->setGeometry(0, 0, img_width, this->LABEL_HEIGHT * 2);

        this->label_creator = new QLabel(label_base);
        this->label_creator->setStyleSheet("font-size:16px; color: #999999;");
        this->label_creator->setFixedWidth(this->itemImage->sizeHint().width());
        this->label_creator->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);


        QVBoxLayout *boxLayout = new QVBoxLayout();
        boxLayout->setContentsMargins(0,0,ITEM_BETWEEN_MARGIN_RIGHT,0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
        //this->setFixedHeight(this->get_fixedHeight());
    }


    /**
     * @brief [override] 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemVideo::setData(const tidal::VideoItemData &p_data_video){

        this->data_video = p_data_video;
    }


    /**
     * @brief ItemVideo::get_fixedHeight
     * @return
     */
    int ItemVideo::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemVideo::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            this->itemImage->setImage(data_video.image);
            qobject_cast<ItemImage_Video*>(this->itemImage)->setDuration(data_video.duration);

            int all_width = this->itemImage->sizeHint().width();

            QString title = data_video.title;

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
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

                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT * 2);
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));

                this->label_creator->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
            }
            else{
                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                this->label_title->setText(title);

                this->label_creator->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
            }

            this->label_creator->setText(GSCommon::getTextCutFromLabelWidth(data_video.list_artist_name.join(", "), all_width, this->label_creator->font()));
        }
    }
}
