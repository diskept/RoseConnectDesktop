#include "roseRadio/ItemSquare.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>


namespace roseRadio {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 17;
    const int ITEM_TITLE_HEIGHT = 29;
    const int ITEM_RESOLUTIION_HEIGHT = 24;
    const int ITEM_SPACE_LABEL = 5;


    QString ICON_PATH___square_favorite_off = ":/images/radio/fav_ico0.png";
    QString ICON_PATH___square_favorite_on = ":/images/radio/fav_ico1.png";

    /**
     * @brief
     * @param parent
     */
    ItemSquare::ItemSquare(int index, int section, tidal::AbstractItem::ImageSizeMode imageSizeMode, QWidget *parent)
        : QWidget(parent)
    {

        this->windex = index;
        this->wsection = section;

        this->imageSize = imageSizeMode;

        this->setUIControl();
    }


    ItemSquare::~ItemSquare(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemSquare::setUIControl(){

        int base_height = 0;
        int img_width = 0;

        if(this->imageSize == tidal::AbstractItem::ImageSizeMode::Square_284x284){
            img_width = 284;
        }
        else if(this->imageSize == tidal::AbstractItem::ImageSizeMode::Square_200x200){
            img_width = 200;
        }
        base_height = 15 + ITEM_TITLE_HEIGHT * 2 + 14;

        this->itemImage = new roseRadio::ItemImageRoseRadio(img_width);
        connect(this->itemImage, &roseRadio::ItemImageRoseRadio::roseRadio_clicked, this, &ItemSquare::slot_clickImg_signal);

        QWidget *label_base = new QWidget();
        label_base->setFixedSize(img_width, base_height);

        this->label_title = new QLabel(label_base);
        this->label_title->setWordWrap(true);
        this->label_title->setStyleSheet("font-size:20px;  font-weight: normal;font-style: normal;line-height: 1.25;text-align: left; color:#FFFFFF;");
        this->label_title->setGeometry(0, 15, img_width - 60, ITEM_TITLE_HEIGHT * 2);

        this->label_name = new QLabel(label_base);
        this->label_name->setStyleSheet("font-size:16px; font-weight:300; line-height:3.38; color: #999999;");
        this->label_name->setGeometry(0, 56, img_width, 24);
        this->label_name->hide();

        this->label_resolution = new QLabel(label_base);
        this->label_resolution->setStyleSheet("font-size:16px;   font-weight: 300;font-style: normal;line-height: 1.56;text-align: left; color: #999999;");
        this->label_resolution->setGeometry(0, ITEM_TITLE_HEIGHT * 2 + ITEM_SPACE_LABEL, img_width - 60, ITEM_RESOLUTIION_HEIGHT);
        this->label_resolution->hide();

        this->btn_favorite = GSCommon::getUIBtnImg("btn_favorite", ICON_PATH___square_favorite_off, label_base);
        this->btn_favorite->setGeometry(img_width - 50, 0, 50, 58);
        this->btn_favorite->setCursor(Qt::PointingHandCursor);
        this->btn_favorite->setProperty("type", "fav");

        connect(this->btn_favorite, SIGNAL(clicked()), SLOT(slot_clickBtn_signal()));

        QVBoxLayout *boxLayout = new QVBoxLayout;
        boxLayout->setSpacing(0);
        boxLayout->setAlignment(Qt::AlignTop);
        boxLayout->addWidget(this->itemImage);
        boxLayout->addWidget(label_base);

        if(img_width > 200){
            boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT + 3, 0);
        }
        else{
            boxLayout->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        }

        this->setLayout(boxLayout);
    }


    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemSquare::setData(const QJsonObject &p_data){

        this->data_channel = p_data;

        QString image_onAir = ":/images/radio/onair_ico_off.png";

        if(this->data_channel.value("online").toBool()){
            image_onAir = ":/images/radio/onair_ico.png";
        }

        this->itemImage->setTypeOnAir(image_onAir);

        QString image_bg_path = ProcJsonEasy::getString(this->data_channel, "imageUrl");

        if(image_bg_path.isEmpty()){
            image_bg_path = ":/images/def_mus_300.png";
        }

        this->itemImage->setImage(image_bg_path);

        int all_width = this->itemImage->sizeHint().width() - 60;

        QString title = ProcJsonEasy::getString(this->data_channel, "title");
        QString name = "";

        QJsonArray tmpGenres = ProcJsonEasy::getJsonArray(this->data_channel, "genres");
        QString genres = ProcJsonEasy::getString(tmpGenres.at(0).toObject(), "name");

        QJsonArray tmpRegions = ProcJsonEasy::getJsonArray(this->data_channel, "regions");
        QString regions = ProcJsonEasy::getString(tmpRegions.at(0).toObject(), "name");

        this->flag_fav = ProcJsonEasy::getBool(this->data_channel, "favorite");

        if(!genres.isNull() && !regions.isNull()){
            name = genres + " · " + regions;
        }
        else if(!genres.isNull() && regions.isNull()){
            name = genres;
        }
        else if(genres.isNull() && !regions.isNull()){
            name = regions;
        }

        QLabel *tmp_wordwrap = new QLabel();
        tmp_wordwrap->setStyleSheet("font-size:20px; line-height:1.25; color:#FFFFFF;");
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

            this->label_title->setGeometry(0, 15, all_width, ITEM_TITLE_HEIGHT * 2);
            this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, title_width, this->label_title->font()));
            if(this->label_title->text().contains("…")){
                this->label_title->setToolTip(title);//c230321
                this->label_title->setToolTipDuration(2000);//c230321
            }

            //this->label_name->setGeometry(0, (ITEM_TITLE_HEIGHT * 2) + ITEM_SPACE_LABEL, all_width, ITEM_RESOLUTIION_HEIGHT);
            //this->label_name->setText(GSCommon::getTextCutFromLabelWidth(name, all_width, this->label_name->font()));

            if(this->flag_fav){
                this->btn_favorite->setStyleSheet("#btn_favorite { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___square_favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");
            }
        }
        else{
            this->label_title->setGeometry(0, 15, all_width, ITEM_TITLE_HEIGHT);
            this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, all_width, this->label_title->font()));
            if(this->label_title->text().contains("…")){
                this->label_title->setToolTip(title);//c230321
                this->label_title->setToolTipDuration(2000);//c230321
            }

            //this->label_name->setGeometry(0, ITEM_TITLE_HEIGHT + ITEM_SPACE_LABEL, all_width, ITEM_RESOLUTIION_HEIGHT);
            //this->label_name->setText(GSCommon::getTextCutFromLabelWidth(name, all_width, this->label_name->font()));

            if(this->flag_fav){
                this->btn_favorite->setStyleSheet("#btn_favorite { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___square_favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");
            }
        }
    }


    void ItemSquare::setFavorite(const bool flag){

        this->flag_fav = flag;

        if(this->flag_fav == true){
            this->btn_favorite->setStyleSheet("#btn_favorite { background-color:rgba(0,0,0,0); background-image: url('" +
                                              ICON_PATH___square_favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");
        }
        else if(this->flag_fav == false){
            this->btn_favorite->setStyleSheet("#btn_favorite { background-color:rgba(0,0,0,0); background-image: url('" +
                                              ICON_PATH___square_favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
        }
    }


    void ItemSquare::slot_clickBtn_signal(){

        QString type = sender()->property("type").toString();

        if(type == "fav"){
            if(this->flag_fav == true){
                emit this->square_clicked(this->wsection, this->windex, roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete);
            }
            else if(this->flag_fav == false){
                emit this->square_clicked(this->wsection, this->windex, roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add);
            }
        }
    }


    void ItemSquare::slot_clickImg_signal(const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio clickMode){

        if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox){
            emit this->square_clicked(this->wsection, this->windex, roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox);
        }
        else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){
            emit this->square_clicked(this->wsection, this->windex, roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn);
        }

    }
}
