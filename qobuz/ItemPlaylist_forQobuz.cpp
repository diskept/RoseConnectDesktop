#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/ItemImage_Playlist_forQobuz.h"

#include "common/gscommon.h"


namespace qobuz {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 19;             ///< 옆에 있는 다른 ItemAlbum 과의 가로 거리 (space)


    /**
     * @brief 생성자
     * @param parent
     */
    ItemPlaylist_qobuz::ItemPlaylist_qobuz(int index, int section, ImageSizeMode imageSizeMode, QWidget *parent)
        : AbstractItem(index, section, imageSizeMode, parent) {

        setUIControl();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemPlaylist_qobuz::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Playlist_qobuz(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(m_imageSizeMode));
        connect(this->itemImage, &ItemImage_Playlist_qobuz::signal_clickedBtn, this, &ItemPlaylist_qobuz::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        QPixmap *img_hires = GSCommon::getUIPixmapImg(":/images/qobuz/hires_ico.png");
        this->label_hires = new QLabel(label_base);
        this->label_hires->setPixmap(*img_hires);
        this->label_hires->setFixedSize(this->label_hires->pixmap()->width(), this->label_hires->pixmap()->height());
        this->label_hires->setGeometry(354, 0, 0, 0);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 2.1;text-align: left; color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

        this->label_creator = new QLabel(label_base);
        this->label_creator->setStyleSheet("font-size:16px; color: #999999;");
        this->label_creator->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);


        QVBoxLayout *boxLayout = new QVBoxLayout();
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        int height = this->itemImage->sizeHint().height();
        height += 32;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        this->setLayout(boxLayout);
        this->setFixedHeight(height);
        //this->setFixedHeight(this->get_fixedHeight());
    }


    /**
     * @brief [override] 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemPlaylist_qobuz::setData(const qobuz::PlaylistItemData &p_data_playList){

        this->data_playlist = p_data_playList;
    }


    /**
     * @brief ItemPlaylist::get_fixedHeight
     * @return
     */
    int ItemPlaylist_qobuz::get_fixedHeight(){

        int height = this->itemImage->sizeHint().height();
        height += 32;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        return height;
    }

    int ItemPlaylist_qobuz::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemPlaylist_qobuz::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemPlaylist_qobuz::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            this->itemImage->setImage(data_playlist.image);
            int all_width = this->itemImage->sizeHint().width();

            QString title = data_playlist.title;

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");
            tmp_wordwrap->setText(title);

            int title_width = tmp_wordwrap->sizeHint().width();

            if(data_playlist.hires == true){

                all_width = (all_width - ICON_HEIGHT - 8);
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
                        this->label_title->setToolTip(title);//c230321
                        this->label_title->setToolTipDuration(2000);//c230321
                    }

                    this->label_creator->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                }
                else{
                    this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                    this->label_title->setText(title);

                    this->label_creator->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                }

                this->label_creator->setText(GSCommon::getTextCutFromLabelWidth(data_playlist.ownerName, all_width, this->label_creator->font()));
                if(this->label_creator->text().contains("…")){
                    this->label_creator->setToolTip(data_playlist.ownerName);//c230321
                    this->label_creator->setToolTipDuration(2000);//c230321
                }
                this->label_hires->show();
            }
            else{

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
                        this->label_title->setToolTip(title);//c230321
                        this->label_title->setToolTipDuration(2000);//c230321
                    }

                    this->label_creator->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                }
                else{
                    this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                    this->label_title->setText(title);

                    this->label_creator->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                }

                this->label_creator->setText(GSCommon::getTextCutFromLabelWidth(data_playlist.ownerName, all_width, this->label_creator->font()));
                if(this->label_creator->text().contains("…")){
                    this->label_creator->setToolTip(data_playlist.ownerName);//c230321
                    this->label_creator->setToolTipDuration(2000);//c230321
                }
                this->label_hires->hide();
            }
        }
    }

}
