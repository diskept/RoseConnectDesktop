#include "rosetube/ItemPlaylist_rosetube.h"
#include "rosetube/ItemImage_Playlist_rosetube.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QJsonArray>


namespace rosetube {

    const int LABEL_HEIGHT_RT_PLAYLIST = 25;
    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;

    /**
     * @brief
     * @param parent
     */
    ItemPlaylist_rosetube::ItemPlaylist_rosetube(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->flag_btnPlay = flagPlay;
        this->setUIControl();
    }


    ItemPlaylist_rosetube::~ItemPlaylist_rosetube(){

        this->deleteLater();
    }

    /**
     * @brief [override] UI 세팅
     */
    void ItemPlaylist_rosetube::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Playlist_rosetube(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(m_imageSizeMode), this->flag_btnPlay);
        connect(this->itemImage, &ItemImage_Playlist_rosetube::signal_clickedBtn, this, &ItemPlaylist_rosetube::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = LABEL_HEIGHT_RT_PLAYLIST * 2 + this->LABEL_HEIGHT + 35;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);
        this->label_title->setStyleSheet("font-size:16px;color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, (LABEL_HEIGHT_RT_PLAYLIST * 2));

        this->label_owner = new QLabel(label_base);
        this->label_owner->setStyleSheet("font-size:14px;color:#999999;");
        this->label_owner->setGeometry(0, (LABEL_HEIGHT_RT_PLAYLIST * 2), img_width, this->LABEL_HEIGHT);

        QPixmap *img_thmub = GSCommon::getUIPixmapImg(":/images/rosehome/home_like_ico.png");

        this->image_thumb = new QLabel(label_base);
        this->image_thumb->setPixmap(*img_thmub);
        this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
        this->image_thumb->setGeometry(0, (LABEL_HEIGHT_RT_PLAYLIST * 2) + this->LABEL_HEIGHT, 0, 0);

        this->label_thumb = new QLabel(label_base);
        this->label_thumb->setStyleSheet("font-size:14px;color:#999999;");
        this->label_thumb->setGeometry(40, (LABEL_HEIGHT_RT_PLAYLIST * 2) + this->LABEL_HEIGHT + 7, img_width - 40, this->LABEL_HEIGHT);

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 30;
        height += LABEL_HEIGHT_RT_PLAYLIST * 2 + this->LABEL_HEIGHT + 35;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
    }



    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemPlaylist_rosetube::setData(const QJsonObject &jsonObj){

        this->data_playlist = jsonObj;
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemPlaylist_rosetube::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 30;
        height += LABEL_HEIGHT_RT_PLAYLIST * 3 + 35;

        return height;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemPlaylist_rosetube::paintEvent(QPaintEvent *event){
        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            int all_width = 0;

            QString image_bg_path = ProcJsonEasy::getString(this->data_playlist, "thumbnail");
            this->itemImage->setImage(image_bg_path);
            all_width = this->itemImage->sizeHint().width();

            QString title = ProcJsonEasy::getString(this->data_playlist, "title");

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

                this->label_title->setGeometry(0, 0, all_width, (LABEL_HEIGHT_RT_PLAYLIST * 2));
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));

                QString owner = ProcJsonEasy::getString(this->data_playlist, "ownerName");

                QString thumb_count ="";
                thumb_count = QString("%1").number(ProcJsonEasy::getInt(this->data_playlist, "thumbupCount"));
                thumb_count += " | " + QString("%1").number(ProcJsonEasy::getInt(this->data_playlist, "trackCount")) + tr(" Tracks");

                this->label_owner->setAlignment(Qt::AlignVCenter);
                this->label_owner->setText(owner.toUpper());

                this->label_thumb->setAlignment(Qt::AlignVCenter);
                this->label_thumb->setText(thumb_count);

                if(owner.isEmpty()){
                    this->label_owner->hide();
                    this->image_thumb->setGeometry(0, (LABEL_HEIGHT_RT_PLAYLIST * 2) + 5, 0, 0);
                    this->label_thumb->setGeometry(35, (LABEL_HEIGHT_RT_PLAYLIST * 2) + 12, all_width - 40, LABEL_HEIGHT_RT_PLAYLIST);
                }
                else{
                    this->label_owner->setGeometry(0, (LABEL_HEIGHT_RT_PLAYLIST * 2), all_width, LABEL_HEIGHT_RT_PLAYLIST);
                    this->image_thumb->setGeometry(0, (LABEL_HEIGHT_RT_PLAYLIST * 3) + 5, 0, 0);
                    this->label_thumb->setGeometry(35, (LABEL_HEIGHT_RT_PLAYLIST * 3) + 12, all_width - 40, LABEL_HEIGHT_RT_PLAYLIST);
                }
            }
            else {
                this->label_title->setGeometry(0, 0, all_width, LABEL_HEIGHT_RT_PLAYLIST);
                this->label_title->setText(title);

                QString owner = ProcJsonEasy::getString(this->data_playlist, "ownerName");

                QString thumb_count ="";
                thumb_count = QString("%1").number(ProcJsonEasy::getInt(this->data_playlist, "thumbupCount"));
                thumb_count += " | " + QString("%1").number(ProcJsonEasy::getInt(this->data_playlist, "trackCount")) + tr(" Tracks");

                this->label_owner->setAlignment(Qt::AlignVCenter);
                this->label_owner->setText(owner.toUpper());

                this->label_thumb->setAlignment(Qt::AlignVCenter);
                this->label_thumb->setText(thumb_count);

                if(owner.isEmpty()){
                    this->label_owner->hide();
                    this->image_thumb->setGeometry(0, LABEL_HEIGHT_RT_PLAYLIST + 5, 0, 0);
                    this->label_thumb->setGeometry(35, LABEL_HEIGHT_RT_PLAYLIST + 12, all_width - 40, LABEL_HEIGHT_RT_PLAYLIST);
                }
                else{
                    this->label_owner->setGeometry(0, LABEL_HEIGHT_RT_PLAYLIST, all_width, LABEL_HEIGHT_RT_PLAYLIST);
                    this->image_thumb->setGeometry(0, (LABEL_HEIGHT_RT_PLAYLIST * 2) + 5, 0, 0);
                    this->label_thumb->setGeometry(35, (LABEL_HEIGHT_RT_PLAYLIST * 2) + 12, all_width - 40, LABEL_HEIGHT_RT_PLAYLIST);
                }
            }
        }
    }
}
