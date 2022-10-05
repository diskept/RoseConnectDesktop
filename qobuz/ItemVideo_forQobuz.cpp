#include "ItemVideo_forQobuz.h"
#include "ItemImage_Video_forQobuz.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include "common/gscommon.h"


namespace qobuz {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;             ///< 옆에 있는 다른 ItemAlbum 과의 가로 거리 (space)


    /**
     * @brief 생성자
     * @param parent
     */
    ItemVideo_qobuz::ItemVideo_qobuz(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        setUIControl();
    }



    /**
     * @brief [override] UI 세팅
     */
    void ItemVideo_qobuz::setUIControl(){

        this->itemImage = new ItemImage_Video_qobuz(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Video_qobuz::signal_clickedBtn, this, &ItemVideo_qobuz::slot_itemImage_clickedBtn);

        label_title = new QLabel();
        label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy

        label_title->setContentsMargins(0,0,0,0);
        label_title->setStyleSheet("font-size:16px; color:#FFFFFF;");
        label_title->setFixedWidth(this->itemImage->sizeHint().width());
        label_title->setFixedHeight(this->LABEL_HEIGHT*2);

        label_creator = new QLabel();
        label_creator->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy

        label_creator->setContentsMargins(0,0,0,0);
        label_creator->setStyleSheet("font-size:16px; color: #999999;");
        label_creator->setFixedWidth(this->itemImage->sizeHint().width());
        label_creator->setFixedHeight(this->LABEL_HEIGHT);


        QVBoxLayout *boxLayout = new QVBoxLayout();
        boxLayout->setContentsMargins(0,0,ITEM_BETWEEN_MARGIN_RIGHT,0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->setSpacing(this->get_spaceImageAndTitle(this->m_imageSizeMode));
        boxLayout->addWidget(label_title);
        boxLayout->setSpacing(this->SPACE_LABELS);
        boxLayout->addWidget(label_creator);
        boxLayout->setAlignment(Qt::AlignTop);

        this->setLayout(boxLayout);
        this->setFixedHeight(this->get_fixedHeight()+this->LABEL_HEIGHT);

    }


    /**
     * @brief [override] 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemVideo_qobuz::setData(const qobuz::VideoItemData &p_data_video){
        this->data_video = p_data_video;
    }


    /**
     * @brief ItemVideo::get_fixedHeight
     * @return
     */
    int ItemVideo_qobuz::get_fixedHeight(){
        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += this->get_spaceImageAndTitle(this->m_imageSizeMode);
        height += this->LABEL_HEIGHT + this->SPACE_LABELS + this->LABEL_HEIGHT;
        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemVideo_qobuz::paintEvent(QPaintEvent *event){
        AbstractItem::paintEvent(event);
        if(this->flagInitDraw == false){
            this->flagInitDraw = true;
            this->itemImage->setImage(data_video.image);
            int all_width = this->itemImage->sizeHint().width();

            qobject_cast<ItemImage_Video_qobuz*>(this->itemImage)->setDuration(data_video.duration);
            //label_title->setText(GSCommon::getTextCutFromLabelWidth(data_video.title, all_width, label_title->font()));
            //qDebug()<<"all_width"<<all_width;
            int LABEL_HEIGHT = 18;
            int SPACE_LABELS = 5;
            if(data_video.title.toStdString().size() > all_width/8.3){//cheon21071620-titleline2

                //this->label_title->setFixedHeight(LABEL_HEIGHT*2);
                this->label_title->setGeometry(0, SPACE_LABELS, all_width, (LABEL_HEIGHT *2));
                int a= data_video.title.toStdString().size()-data_video.title.size();//cheon21071620-titleline2
                int b;
                a >= 2 ? b =all_width/8.3-a : b = all_width/8.3;//cheon21071620-titleline2
                //qDebug() << " b =" << b <<" a = " << a;//cheon21071620-titleline2
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(data_video.title.insert(b, '\n'), (all_width - 20) * 2, label_title->font()));//cheon21071620-titleline2

            }
            else{//cheon21071620-titleline2
                //lb_title->setGeometry(0, this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                this->label_title->setGeometry(0, SPACE_LABELS, all_width, (LABEL_HEIGHT * 1));
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(data_video.title, all_width, label_title->font()));//cheon21071620-titleline2

            }

            label_creator->setText(GSCommon::getTextCutFromLabelWidth(data_video.list_artist_name.join(", "), all_width, label_creator->font()));
        }
    }

}
