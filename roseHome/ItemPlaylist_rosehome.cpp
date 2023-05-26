#include "ItemPlaylist_rosehome.h"
#include "ItemImage_Playlist_rosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/global.h"
#include "common/gscommon.h"

namespace roseHome {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;

    const int HTTP_RECOMMENDS = 0;
    const int HTTP_FAVORITE_PLAYLIST = 1;
    const int HTTP_RECENTLY_PLAYLIST = 2;
    const int HTTP_MY_PLAYLIST = 5;
    const int HTTP_NEW_PLAYLIST = 6;
    const int HTTP_POP_PLAYLIST = 7;
    const int HTTP_USERPICK_PLAYLIST = 8;//cheon211015
    const int HTTP_DUDUO_PLAYLIST = 9;

    /**
     * @brief
     * @param parent
     */
    ItemPlaylist_rosehome::ItemPlaylist_rosehome(int index, int section, ImageSizeMode imageSizeMode, int type,  bool flagPlay, QWidget *parent)
        : tidal::AbstractItem(index, section, imageSizeMode, parent) {

        this->playlist_type = type;
        this->flag_btnPlay = flagPlay;
        this->setUIControl();
    }


    ItemPlaylist_rosehome::~ItemPlaylist_rosehome(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemPlaylist_rosehome::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        this->itemImage = new ItemImage_Playlist_roseHome(this->get_imageWidth(this->m_imageSizeMode), this->get_imageHeight(m_imageSizeMode), this->flag_btnPlay);
        connect(this->itemImage, &ItemImage_Playlist_roseHome::signal_clickedBtn, this, &ItemPlaylist_rosehome::slot_itemImage_clickedBtn);

        img_width = this->itemImage->sizeHint().width();
        if(this->playlist_type == HTTP_RECENTLY_PLAYLIST || this->playlist_type == HTTP_FAVORITE_PLAYLIST){
            base_height = this->LABEL_HEIGHT * 2 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }
        else if(this->playlist_type == HTTP_MY_PLAYLIST || this->playlist_type == HTTP_USERPICK_PLAYLIST){
            base_height = this->LABEL_HEIGHT * 2 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }
        else if(this->playlist_type == HTTP_RECOMMENDS || this->playlist_type == HTTP_NEW_PLAYLIST || this->playlist_type == HTTP_POP_PLAYLIST || this->playlist_type == HTTP_DUDUO_PLAYLIST){
            base_height = this->LABEL_HEIGHT * 3 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }else if(this->playlist_type >= 1000){
            base_height = this->LABEL_HEIGHT * 2 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setStyleSheet("font-size:16px;  font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#FFFFFF;");
        this->label_title->setWordWrap(true);
        this->label_title->setGeometry(0, 0, img_width, (this->LABEL_HEIGHT * 2));

        QPixmap *img_thmub = GSCommon::getUIPixmapImg(":/images/rosehome/home_like_ico.png");

        if(this->playlist_type == HTTP_RECENTLY_PLAYLIST){
            this->label_owner = new QLabel(label_base);
            this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);

            this->image_thumb = new QLabel(label_base);
            this->image_thumb->setPixmap(*img_thmub);
            this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 0, 0);

            this->label_thumb = new QLabel(label_base);
            this->label_thumb->setGeometry(40, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS) + 7, img_width - 40, this->LABEL_HEIGHT);
        }
        if(this->playlist_type == HTTP_FAVORITE_PLAYLIST){
            this->label_owner = new QLabel(label_base);
            this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);

            this->label_share = new QLabel(label_base);
            this->label_share->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 60, this->LABEL_HEIGHT);

            this->image_thumb = new QLabel(label_base);
            this->image_thumb->setPixmap(*img_thmub);
            this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 0, 0);

            this->label_thumb = new QLabel(label_base);
            this->label_thumb->setGeometry(40, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width - 40, this->LABEL_HEIGHT);
        }
        else if(this->playlist_type == HTTP_MY_PLAYLIST || this->playlist_type == HTTP_USERPICK_PLAYLIST){//cheon211015
            this->image_thumb = new QLabel(label_base);
            this->image_thumb->setPixmap(*img_thmub);
            this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2), 0, 0);

            this->label_thumb = new QLabel(label_base);
            this->label_thumb->setGeometry(40, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2) + 7, img_width - 40, this->LABEL_HEIGHT);
        }
        else if(this->playlist_type == HTTP_RECOMMENDS || this->playlist_type == HTTP_NEW_PLAYLIST || this->playlist_type == HTTP_POP_PLAYLIST || this->playlist_type == HTTP_DUDUO_PLAYLIST){
            this->label_owner = new QLabel(label_base);
            this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, img_width, this->LABEL_HEIGHT);

            this->image_thumb = new QLabel(label_base);
            this->image_thumb->setPixmap(*img_thmub);
            this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2), 0, 0);

            this->label_thumb = new QLabel(label_base);
            this->label_thumb->setGeometry(40, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2) + 7, img_width - 40, this->LABEL_HEIGHT);
        }else if(this->playlist_type >= 1000){
            this->image_thumb = new QLabel(label_base);
            this->image_thumb->setPixmap(*img_thmub);
            this->image_thumb->setFixedSize(image_thumb->pixmap()->width(), image_thumb->pixmap()->height());
            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2), 0, 0);

            this->label_thumb = new QLabel(label_base);
            this->label_thumb->setGeometry(40, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2) + 7, img_width - 40, this->LABEL_HEIGHT);
        }

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        boxLayout->setSpacing(0);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addSpacing(10);
        boxLayout->addWidget(label_base);
        boxLayout->setAlignment(Qt::AlignTop);

        this->setLayout(boxLayout);
        this->setFixedHeight(this->get_fixedHeight());
    }



    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemPlaylist_rosehome::setData(const roseHome::PlaylistItemData &p_data){
        this->data_playlist = p_data;
    }


    /**
     * @brief ItemAlbum::get_fixedHeight
     * @return
     */
    int ItemPlaylist_rosehome::get_fixedHeight(){
        int height = this->get_imageHeight(this->m_imageSizeMode);
        height += 30;
        if(this->playlist_type == HTTP_RECENTLY_PLAYLIST || this->playlist_type == HTTP_FAVORITE_PLAYLIST){
            height += this->LABEL_HEIGHT * 2 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }
        else if(this->playlist_type == HTTP_MY_PLAYLIST || this->playlist_type == HTTP_USERPICK_PLAYLIST){//cheon211015
            height += this->LABEL_HEIGHT * 2 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }
        else if(this->playlist_type == HTTP_RECOMMENDS || this->playlist_type == HTTP_NEW_PLAYLIST || this->playlist_type == HTTP_POP_PLAYLIST || this->playlist_type == HTTP_DUDUO_PLAYLIST){
            height += this->LABEL_HEIGHT * 3 + this->ICON_HEIGHT + this->SPACE_LABELS * 2;
        }
        else if(this->playlist_type >= 1000){
            height += this->LABEL_HEIGHT * 2 + this->ICON_HEIGHT + this->SPACE_LABELS;
        }

        return height;
    }

    int ItemPlaylist_rosehome::get_fixedWidth(){

        int width = this->get_imageWidth(this->m_imageSizeMode);

        return width;
    }


    int ItemPlaylist_rosehome::get_rightMargin(){

        return ITEM_BETWEEN_MARGIN_RIGHT;
    }


    /**
     * @brief 페인트 이벤트 처리
     * @param event
     */
    void ItemPlaylist_rosehome::paintEvent(QPaintEvent *event){
        AbstractItem::paintEvent(event);

        if(this->flagInitDraw == false){
            this->flagInitDraw = true;

            QString image_bg_path = "";
            QString image_type = "";
            if(this->data_playlist.thumbnail.isEmpty()){
                if(this->data_playlist.type == "MUSIC"){
                    image_bg_path = ":/images/def_mus_300.png";
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_playlist.type == "VIDEO"){
                    image_bg_path = ":/images/def_mus_300.png";
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_playlist.type == "YOUTUBE"){
                    image_bg_path = ":/images/def_mus_300.png";
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_playlist.type == "TIDAL"){
                    image_bg_path = ":/images/tidal/tidal_def_400.png";
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_playlist.type == "BUGS"){
                    image_bg_path = ":/images/bugs/bugs_def_430.png";
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_playlist.type == "QOBUZ"){
                    image_bg_path = ":/images/qobuz/qobuz_default_400.png";
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_playlist.type == "APPLE_MUSIC"){
                    image_bg_path = ":/images/apple/apple_def.png";
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_bg_path = ":/images/def_mus_300.png";
                    image_type = ":/images/rosehome/home_all.png";
                }
            }
            else{
                image_bg_path = this->data_playlist.thumbnail;

                if(this->data_playlist.type == "MUSIC"){
                    image_type = ":/images/rosehome/home_music.png";
                }
                else if(this->data_playlist.type == "VIDEO"){
                    image_type = ":/images/rosehome/home_video.png";
                }
                else if(this->data_playlist.type == "YOUTUBE"){
                    image_type = ":/images/rosehome/home_tube.png";
                }
                else if(this->data_playlist.type == "TIDAL"){
                    image_type = ":/images/rosehome/home_tidal.png";
                }
                else if(this->data_playlist.type == "BUGS"){
                    image_type = ":/images/rosehome/home_bugs.png";
                }
                else if(this->data_playlist.type == "QOBUZ"){
                    image_type = ":/images/rosehome/home_qobuz.png";
                }
                else if(this->data_playlist.type == "APPLE_MUSIC"){
                    image_type = ":/images/rosehome/home_apple.png";
                }
                else{
                    image_type = ":/images/rosehome/home_all.png";
                }
            }

            this->itemImage->setTypeImage(image_type);
            this->itemImage->setImage(image_bg_path);
            int all_width = this->itemImage->sizeHint().width();

            bool flagOwner = (this->data_playlist.ownerName == "tidal" || this->data_playlist.ownerName == "bugs" ||     //this->data_playlist.ownerName == "music" ||
                               this->data_playlist.ownerName == "apple-music" ||this->data_playlist.ownerName == "qobuz") == true ? true : false;

            QString setHTML_owner = "";
            QString setHTML_thumb = "";
            QString setHTML_share = "";

            QString title = this->data_playlist.title;

            QLabel *tmp_wordwrap = new QLabel();
            tmp_wordwrap->setText(title);
            tmp_wordwrap->setStyleSheet("font-size:16px; color:#FFFFFF;");

            int title_width = tmp_wordwrap->sizeHint().width();

            if(title_width > all_width){

                this->label_title->setGeometry(0, 0, all_width, (this->LABEL_HEIGHT * 2));

                if(this->playlist_type == HTTP_RECENTLY_PLAYLIST){
                    if(flagOwner == true){
                        this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    }
                    else{
                        this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 0, 0);
                        this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);
                    }
                }
                else if(this->playlist_type == HTTP_FAVORITE_PLAYLIST){
                    if(flagOwner == true){
                        this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    }
                    else{
                        if(this->data_playlist.share == "PRIVATE"){
                            QLabel *tmpShare = new QLabel();
                            tmpShare->setStyleSheet("font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;");
                            tmpShare->setText(tr("PRIVATE"));

                            int tmpShare_width = tmpShare->sizeHint().width();

                            this->label_share->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, tmpShare_width, this->LABEL_HEIGHT);
                            this->label_thumb->setGeometry(tmpShare_width + 10, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width - tmpShare_width - 10, this->LABEL_HEIGHT);
                        }
                        else{
                            this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 0, 0);
                            this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);
                        }
                    }
                }
                else if(this->playlist_type == HTTP_MY_PLAYLIST || this->playlist_type == HTTP_USERPICK_PLAYLIST){//cheon211015
                    this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 0, 0);
                    this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);
                }
                else if(this->playlist_type == HTTP_RECOMMENDS || this->playlist_type == HTTP_NEW_PLAYLIST || this->playlist_type == HTTP_POP_PLAYLIST || this->playlist_type == HTTP_DUDUO_PLAYLIST){
                    this->label_owner->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2), 0, 0);
                    this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 3) + (this->SPACE_LABELS * 2) + 7, all_width - 40, this->LABEL_HEIGHT);

                }else if(this->playlist_type >= 1000){
                    this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS, 0, 0);
                    this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 2) + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);
                }

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

                this->label_title->setTextFormat(Qt::RichText);
                this->label_title->setAlignment(Qt::AlignVCenter);
                this->label_title->setWordWrap(true);
                this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));
                if(this->label_title->text().contains("…")){
                    this->label_title->setToolTip(title);//c230321
                    this->label_title->setToolTipDuration(2000);//c230321
                }
            }
            else{

                this->label_title->setGeometry(0, 0, all_width, this->LABEL_HEIGHT);

                if(this->playlist_type == HTTP_RECENTLY_PLAYLIST){
                    //this->label_owner->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    if(flagOwner == true){
                        this->label_owner->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    }
                    else{
                        this->image_thumb->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, 0, 0);
                        this->label_thumb->setGeometry(35, this->LABEL_HEIGHT + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);
                    }
                }
                else if(this->playlist_type == HTTP_FAVORITE_PLAYLIST){
                    if(flagOwner == true){
                        this->label_owner->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    }
                    else{
                        if(this->data_playlist.share == "PRIVATE"){
                            QLabel *tmpShare = new QLabel();
                            tmpShare->setStyleSheet("font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;");
                            tmpShare->setText(tr("PRIVATE"));

                            int tmpShare_width = tmpShare->sizeHint().width();

                            this->label_share->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, tmpShare_width, this->LABEL_HEIGHT);
                            this->label_thumb->setGeometry(tmpShare_width + 10, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width - tmpShare_width - 10, this->LABEL_HEIGHT);
                        }
                        else{
                            this->image_thumb->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, 0, 0);
                            this->label_thumb->setGeometry(35, this->LABEL_HEIGHT + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);
                        }
                    }
                }
                else if(this->playlist_type == HTTP_MY_PLAYLIST || this->playlist_type == HTTP_USERPICK_PLAYLIST){//cheon211015
                    this->image_thumb->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, 0, 0);
                    this->label_thumb->setGeometry(35, this->LABEL_HEIGHT + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);

                }
                else if(this->playlist_type == HTTP_RECOMMENDS || this->playlist_type == HTTP_NEW_PLAYLIST || this->playlist_type == HTTP_POP_PLAYLIST || this->playlist_type == HTTP_DUDUO_PLAYLIST){
                    this->label_owner->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, all_width, this->LABEL_HEIGHT);
                    this->image_thumb->setGeometry(0, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 2), 0, 0);
                    this->label_thumb->setGeometry(35, (this->LABEL_HEIGHT * 2) + (this->SPACE_LABELS * 2) + 7, all_width - 40, this->LABEL_HEIGHT);
                }else if(this->playlist_type >= 1000){
                    this->image_thumb->setGeometry(0, this->LABEL_HEIGHT + this->SPACE_LABELS, 0, 0);
                    this->label_thumb->setGeometry(35, this->LABEL_HEIGHT + this->SPACE_LABELS + 7, all_width - 40, this->LABEL_HEIGHT);

                }

                this->label_title->setTextFormat(Qt::RichText);
                this->label_title->setAlignment(Qt::AlignVCenter);
                this->label_title->setWordWrap(true);
                this->label_title->setText(title);
            }

            QString owner_name ="";
            if(flagOwner == true){

                QString ownerName = "";
                if(this->data_playlist.ownerName == "tidal"){
                    ownerName = "Tidal";
                }
                else if(this->data_playlist.ownerName == "bugs"){
                    ownerName = "Bugs";
                }
                else if(this->data_playlist.ownerName == "qobuz"){
                    ownerName = "Qobuz";
                }
                else if(this->data_playlist.ownerName == "apple-music"){
                    ownerName = "AppleMusic";
                }

                owner_name = "by " + ownerName;
            }
            else{
                owner_name = this->data_playlist.ownerName;
            }

            setHTML_owner = QString("<html><head/><body><span style='font-size:16px; font-weight:300; line-height: 1.88; color:#CCCCCC;'>%1</span></body></html>").arg(owner_name);

            QString thumb_count ="";
            thumb_count = QString("%1").number(this->data_playlist.thumbupCount);
            thumb_count += " | " + QString("%1").number(this->data_playlist.trackCount) + tr(" Tracks");
            setHTML_thumb = QString("<html><head/><body><span style='font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;'>%1</span></body></html>").arg(thumb_count);

            setHTML_share = QString("<html><head/><body><span style='font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;'>%1</span></body></html>").arg(tr("PRIVATE"));

            if(this->playlist_type == HTTP_RECENTLY_PLAYLIST){
                if(flagOwner == true){
                    this->label_owner->setTextFormat(Qt::RichText);
                    this->label_owner->setAlignment(Qt::AlignVCenter);
                    this->label_owner->setText(setHTML_owner);
                    this->image_thumb->hide();
                    this->label_thumb->hide();
                }
                else{
                    this->label_thumb->setTextFormat(Qt::RichText);
                    this->label_thumb->setAlignment(Qt::AlignVCenter);
                    this->label_thumb->setText(setHTML_thumb);
                    this->label_owner->hide();
                }
            }
            else if(this->playlist_type == HTTP_FAVORITE_PLAYLIST){
                if(flagOwner == true){
                    this->label_owner->setTextFormat(Qt::RichText);
                    this->label_owner->setAlignment(Qt::AlignVCenter);
                    this->label_owner->setText(setHTML_owner);
                    this->label_share->hide();
                    this->image_thumb->hide();
                    this->label_thumb->hide();
                }
                else{
                    if(this->data_playlist.share == "PRIVATE"){
                        this->label_share->setTextFormat(Qt::RichText);
                        this->label_share->setAlignment(Qt::AlignVCenter);
                        this->label_share->setText(setHTML_share);
                        this->label_thumb->setTextFormat(Qt::RichText);
                        this->label_thumb->setAlignment(Qt::AlignVCenter);
                        this->label_thumb->setText(setHTML_thumb);
                        this->image_thumb->hide();
                        this->label_owner->hide();
                    }
                    else{
                        this->label_thumb->setTextFormat(Qt::RichText);
                        this->label_thumb->setAlignment(Qt::AlignVCenter);
                        this->label_thumb->setText(setHTML_thumb);
                        this->label_share->hide();
                        this->label_owner->hide();
                    }
                }
            }
            else if(this->playlist_type == HTTP_MY_PLAYLIST || this->playlist_type == HTTP_USERPICK_PLAYLIST){//cheon211015
                this->label_thumb->setTextFormat(Qt::RichText);
                this->label_thumb->setAlignment(Qt::AlignVCenter);
                this->label_thumb->setText(setHTML_thumb);

            }
            else if(this->playlist_type == HTTP_RECOMMENDS || this->playlist_type == HTTP_NEW_PLAYLIST || this->playlist_type == HTTP_POP_PLAYLIST || this->playlist_type == HTTP_DUDUO_PLAYLIST){
                this->label_owner->setTextFormat(Qt::RichText);
                this->label_owner->setAlignment(Qt::AlignVCenter);
                this->label_owner->setText(setHTML_owner);

                this->label_thumb->setTextFormat(Qt::RichText);
                this->label_thumb->setAlignment(Qt::AlignVCenter);
                this->label_thumb->setText(setHTML_thumb);
            }
            else if(this->playlist_type >= 1000){
                this->label_thumb->setTextFormat(Qt::RichText);
                this->label_thumb->setAlignment(Qt::AlignVCenter);
                this->label_thumb->setText(setHTML_thumb);

            }
        }
    }
}
