#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemImage_Album_rosehome.h"

#include "common/gscommon.h"

#include <QJsonArray>


namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;

    /**
     * @brief
     * @param parent
     */
    ItemAlbum_rosehome::ItemAlbum_rosehome(int index, int section, ImageSizeMode imageSizeMode, bool flagPlay, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->flag_setData = false;
        this->flag_btnPlay = flagPlay;
        this->setUIControl();
    }


    ItemAlbum_rosehome::~ItemAlbum_rosehome(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemAlbum_rosehome::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Album_roseHome(this->get_imageWidth(this->m_imageSizeMode), this->flag_btnPlay);
        connect(this->itemImage, &ItemImage_Album_roseHome::signal_clickedBtn, this, &ItemAlbum_rosehome::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        base_height = this->LABEL_HEIGHT * 3 + this->SPACE_LABELS;

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:16px;font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#FFFFFF;");
        this->label_title->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

        this->label_artist = new QLabel(label_base);
        this->label_artist->setContentsMargins(0,0,0,0);
        this->label_artist->setStyleSheet("font-size:16px; font-weight: 300;font-style: normal;line-height: 1.88;text-align: left; color: #999999;");
        this->label_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);
        this->label_artist->hide();

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
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
    }



    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemAlbum_rosehome::setData(const roseHome::AlbumItemData &p_data){

        this->data_album = p_data;
        this->flag_setData = true;
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemAlbum_rosehome::get_fixedHeight(){

        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 10;
        height += (this->LABEL_HEIGHT * 3) + this->SPACE_LABELS;

        return height;
    }

    int ItemAlbum_rosehome::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemAlbum_rosehome::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }

    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemAlbum_rosehome::paintEvent(QPaintEvent *event){

        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false && this->flag_setData == true){
            this->flagInitDraw = true;

            int all_width = 0;

            QString image_bg_path = "";
            QString image_type = "";
            if(this->data_album.thumbnail.isEmpty()){
                if(this->data_album.type == "MUSIC"){
                    image_bg_path = ":/images/def_mus_550.png";
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_album.type == "VIDEO"){
                    image_bg_path = ":/images/def_video_200x2.png";
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_album.type == "YOUTUBE"){
                    image_bg_path = ":/images/def_tube_340.png";
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_album.type == "TIDAL"){
                    image_bg_path = ":/images/tidal/tidal_def_400.png";
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_album.type == "BUGS"){
                    image_bg_path = ":/images/bugs/bugs_def_430.png";
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_album.type == "QOBUZ"){
                    image_bg_path = ":/images/qobuz/qobuz_default_400.png";
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_album.type == "APPLE_MUSIC"){
                    image_bg_path = ":/images/apple/apple_def.png";
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_bg_path = ":/images/rosehome/home_allx2.png";
                    image_type = ":/images/rosehome/home_all.png";
                }
            }
            else{
                image_bg_path = this->data_album.thumbnail;

                if(this->data_album.type == "MUSIC"){
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_album.type == "VIDEO"){
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_album.type == "YOUTUBE"){
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_album.type == "TIDAL"){
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_album.type == "BUGS"){
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_album.type == "QOBUZ"){
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_album.type == "APPLE_MUSIC"){
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_type = ":/images/rosehome/home_all.png";
                }
            }

            if(this->data_album.mime.isEmpty()){
                this->itemImage->setTypeImage(image_type);
            }
            else{
                this->itemImage->setTypeMime(this->data_album.mime);
            }

            this->itemImage->setImage(image_bg_path);
            all_width = this->itemImage->sizeHint().width();

            QString title = this->data_album.title;
            //QString title_ori = "";
            if(!title.isEmpty() && title.left(1) == " "){
                //title_ori = title;
                title = title.remove(0, 1);
                //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << title_ori << title;
            }
            if(!title.isEmpty() && title.right(1) == " "){
                //title_ori = title;
                title.chop(1);
                //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << title_ori << title;
            }

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
                //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << title << title_width << this->label_title->geometry();
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }

                this->label_artist->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);

            }
            else{
                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);
                this->label_title->setText(title);

                this->label_artist->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
            }

            if(!this->data_album.artist.isEmpty()){
                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(this->data_album.artist, all_width, this->label_artist->font()));
                this->label_artist->show();
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(this->data_album.artist);//c230321
                    this->label_artist->setToolTipDuration(2000);//c230321
                }
            }
        }
    }
}
