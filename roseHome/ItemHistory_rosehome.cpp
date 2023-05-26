#include "roseHome/ItemHistory_rosehome.h"
#include "roseHome/ItemImage_Album_rosehome.h"

#include "common/gscommon.h"

#include <QJsonArray>


namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;

    /**
     * @brief
     * @param parent
     */
    ItemHistory_rosehome::ItemHistory_rosehome(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->flag_btnPlay = flagPlay;
        this->setUIControl();
    }


    ItemHistory_rosehome::~ItemHistory_rosehome(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemHistory_rosehome::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Album_roseHome(this->get_imageWidth(this->m_imageSizeMode), this->flag_btnPlay);
        connect(this->itemImage, &ItemImage_Album_roseHome::signal_clickedBtn, this, &ItemHistory_rosehome::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, this->LABEL_HEIGHT);

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += this->LABEL_HEIGHT;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
    }



    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemHistory_rosehome::setData(const roseHome::HistoryItemData &p_data){

        this->data_history = p_data;
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemHistory_rosehome::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += this->LABEL_HEIGHT;

        return height;
    }

    int ItemHistory_rosehome::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemHistory_rosehome::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemHistory_rosehome::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            int all_width = 0;

            QString title = "";
            QString image_bg_path = "";

            QDate chageDate = QDate::fromString(this->data_history.yearMonth, "yyyyMM");

            if("My Most Listened" == this->data_history.yearMonth){

                image_bg_path = ":/images/History/history_most.png";
                title = this->data_history.yearMonth;
            }
            else{

                QString strDate = chageDate.toString("MM");

                switch (strDate.toInt()){
                    case 1:
                        image_bg_path = ":/images/History/history_01jan.png";
                        break;
                    case 2:
                        image_bg_path = ":/images/History/history_02feb.png";
                        break;
                    case 3:
                        image_bg_path = ":/images/History/history_03mar.png";
                        break;
                    case 4:
                        image_bg_path = ":/images/History/history_04apr.png";
                        break;
                    case 5:
                        image_bg_path = ":/images/History/history_05may.png";
                        break;
                    case 6:
                        image_bg_path = ":/images/History/history_06jun.png";
                        break;
                    case 7:
                        image_bg_path = ":/images/History/history_07jul.png";
                        break;
                    case 8:
                        image_bg_path = ":/images/History/history_08aug.png";
                        break;
                    case 9:
                        image_bg_path = ":/images/History/history_09sep.png";
                        break;
                    case 10:
                        image_bg_path = ":/images/History/history_10oct.png";
                        break;
                    case 11:
                        image_bg_path = ":/images/History/history_11nov.png";
                        break;
                    case 12:
                        image_bg_path = ":/images/History/history_12dec.png";
                        break;
                    default:
                        image_bg_path = ":/images/History/history_most.png";
                        break;
                }

                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                title = QLocale().toString(chageDate, "MMMM yyyy");

                QLocale::setDefault(QLocale::system());
            }

            this->itemImage->setImage(image_bg_path);
            all_width = this->itemImage->sizeHint().width();

            this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
            this->label_title->setText(title);
        }
    }
}
