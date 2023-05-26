#include "bugs/ItemTrackSqure_bugs.h"
#include "bugs/ItemImage_TrackSquare.h"

#include <QJsonArray>
#include "common/gscommon.h"


namespace bugs {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 18;                 ///< 옆에 있는 다른 ItemAlbum 과의 가로 거리 (space)


    /**
     * @brief
     * @param parent
     */
    ItemTrackSqure_bugs::ItemTrackSqure_bugs(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : AbstractItem(index, section, imageSizeMode, parent) {

        this->setUIControl();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemTrackSqure_bugs::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_TrackSquare(this->get_imageWidth(this->m_imageSizeMode));
        connect(this->itemImage, &ItemImage_TrackSquare::signal_clickedBtn, this, &ItemTrackSqure_bugs::slot_itemImage_clickedBtn);

        this->label_mime = new QLabel(itemImage);
        this->label_mime->hide();

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 2.1;text-align: left; color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, this->LABEL_HEIGHT * 2);

        this->label_artist = new QLabel(label_base);
        this->label_artist->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 1.88;text-align: left; color: #999999;");
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
    void ItemTrackSqure_bugs::setData(const bugs::TrackItemData &p_data_album){

        this->data_track = p_data_album;
    }


    /**
     * @brief ItemAlbum_bugs::get_fixedHeight
     * @return
     */
    int ItemTrackSqure_bugs::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        return height;
    }

    int ItemTrackSqure_bugs::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemTrackSqure_bugs::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }

    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemTrackSqure_bugs::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);
        if(this->flagInitDraw == false){

            this->flagInitDraw = true;

            this->itemImage->setImage(this->data_track.album_image);
            //this->itemImage->setFavorite(this->data_track.likes_yn);

            if(this->data_track.bitrates.isEmpty() == false){

                int width_set = 0;
                QString bitrates = this->data_track.bitrates;

                QLabel *tmp_mime = new QLabel();
                tmp_mime->setStyleSheet("background-color:#75000000;font-size:11px;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:10px;");
                tmp_mime->setText(bitrates);

                width_set = tmp_mime->sizeHint().width() + 10;

                this->label_mime->setFixedSize(width_set, 20);
                this->label_mime->setStyleSheet("background-color:#75000000;font-size:11px;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:10px;");
                this->label_mime->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                this->label_mime->setGeometry(10, itemImage->height() - 30, 0, 0);
                this->label_mime->setText(bitrates);
                this->label_mime->show();
            }

            int all_width = this->itemImage->sizeHint().width();

            QString title = this->data_track.track_title;

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
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title);
                    this->label_title->setToolTipDuration(2000);
                }

                this->label_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(this->data_track.list_artist_nm.join(","), all_width, this->label_artist->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(this->data_track.list_artist_nm.join(","));
                    this->label_artist->setToolTipDuration(2000);
                }

            }
            else{
                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                this->label_title->setText(title);

                this->label_artist->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(this->data_track.list_artist_nm.join(","), all_width, this->label_artist->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(this->data_track.list_artist_nm.join(","));
                    this->label_artist->setToolTipDuration(2000);
                }
            }
        }
    }
}
