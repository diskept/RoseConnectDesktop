#include "AbstractItemImage.h"
//#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace tidal {

    const int BTN_IMAGE_SIZE = 60;
    const int HEIGHT_TOP_SEVERAL_BG = 20;
    const char* KEY_CODE = "mode";

    const int TYPE_IMAGE_WIDTH = 84;
    const int TYPE_IMAGE_HEIGHT = 80;


    const QString imgPath_topBG_large = ":/images/playlist_bg_qobuz.png";
    const QString imgPath_topBG_medium = ":/images/playlist_video_bg.png";
    const QString imgPath_topBG_basic = ":/images/playlist_bg.png";
    const QString imgPath_topBG_small = ":/images/playlist_bg_s.png";

    /**
     * @brief AbstractItemImage::AbstractItemImage
     */
    AbstractItemImage::AbstractItemImage(QWidget *parent) : QWidget(parent) {

        // 기본값
        this->fixed_imgWidth = 200;
        this->fixed_imgHeight = 200;
        this->fixed_cornerRadius = 3;

        this->flagUseTopBG = false;
        this->flagFavorite = false;
        this->flagUseHover = false;

        this->flag_type_image = false;
        this->flag_type_cache = false;
        this->flag_cache_show = false;
        this->flag_type_mime = false;
        this->flag_type_duration = false;
        this->flag_type_onAir = false;


        this->fileDownLoader = new FileDownloader(this);
        connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractItemImage::slot_fileDownload_loadImage);
    }

    AbstractItemImage::~AbstractItemImage(){

        this->deleteLater();
    }

    /**
     * @brief 기본 UI를 세팅한다. AbstractItemImage 에서는 호출하지 않고, 자식 클래스의 생성자에서 호출해야하는 구조임
     */
    void AbstractItemImage::setUIControl(){

        // 본 이미지
        this->label_imageBig = new QLabel();
        this->label_imageBig->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight);
        this->label_imageBig->setScaledContents(true);
        this->label_imageBig->setAlignment(Qt::AlignCenter);

        this->label_imageType = new QLabel(this->label_imageBig);
        this->label_imageType->hide();

        this->label_imageCache = new QLabel(this->label_imageBig);
        this->label_imageCache->hide();

        this->label_imageMime = new QLabel(this->label_imageBig);
        this->label_imageMime->hide();

        this->label_imageDuration = new QLabel(this->label_imageBig);
        this->label_imageDuration->hide();

        this->label_imageOnAir = new QLabel(this->label_imageBig);
        this->label_imageOnAir->hide();

        // 여러겹 표현
        this->label_topListBG = new QLabel();
        this->label_topListBG->setAlignment(Qt::AlignCenter);
        this->label_topListBG->setPixmap(QPixmap(imgPath_topBG_basic));
        //this->label_topListBG->setFixedWidth(QPixmap(imgPath_topBG_basic).width());
        this->label_topListBG->setFixedHeight(HEIGHT_TOP_SEVERAL_BG);
        this->label_topListBG->setVisible(false);       // 기본값

        this->vBoxLayout_total = new QVBoxLayout;
        this->vBoxLayout_total->setSpacing(0);
        this->vBoxLayout_total->setContentsMargins(0, 0, 0 ,0);
        this->vBoxLayout_total->addWidget(this->label_topListBG);
        this->vBoxLayout_total->addWidget(this->label_imageBig);



        this->setLayout(this->vBoxLayout_total);
        this->setContentsMargins(0, 0, 0, 0);
        this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight);


        //------------------------------------------------------------------------

        // 같은 처리를 위해서 list에 담음
        QList<AbstractItemImage::BtnClickMode> list_btnMode {
                        AbstractItemImage::BtnClickMode::Favorite_toAdd
                        , AbstractItemImage::BtnClickMode::Favorite_toAddx2
                        , AbstractItemImage::BtnClickMode::Favorite_toAddx3
                        , AbstractItemImage::BtnClickMode::Favorite_toDelete
                        , AbstractItemImage::BtnClickMode::Play_RightNow
                        , AbstractItemImage::BtnClickMode::More_showPopupMenu
        };

        QList<QString> list_btn_imgPath {
                    ":/images/album_fav_icon_off.png"
                    , ":/images/album_fav_icon_on.png"
                    , ":/images/rosehome/heart_ico2.png"
                    , ":/images/rosehome/heart_ico3.png"
                    , ":/images/album_play_icon.png"
                    , ":/images/album_more_icon.png"
        };
        QList<QString> list_btn_imgPath_forPress {
                    ":/images/album_fav_icon_off_h.png"
                    , ":/images/album_fav_icon_on_h.png"
                    , ":/images/rosehome/heart_ico2.png"
                    , ":/images/rosehome/heart_ico3.png"
                    , ":/images/album_play_icon_h.png"
                    , ":/images/album_more_icon_h.png"
        };

        this->btn_fav_toAdd = new QPushButton();
        this->btn_fav_toAddx2 = new QPushButton();
        this->btn_fav_toAddx3 = new QPushButton();
        this->btn_fav_toDelete = new QPushButton();
        this->btn_play = new QPushButton();
        this->btn_more = new QPushButton();
        QList<QPushButton*> list_btn {
                    this->btn_fav_toAdd
                    , this->btn_fav_toAddx2
                    , this->btn_fav_toAddx3
                    , this->btn_fav_toDelete
                    , this->btn_play
                    , this->btn_more
        };


        QHBoxLayout *hBox_btns = new QHBoxLayout;
        hBox_btns->setSpacing(0);
        hBox_btns->setContentsMargins(0, 0, 0, 0);
        for(int i = 0; i < list_btn.length(); i++){
            QString tmp_btnStyle = this->getStr_btnStyle(i, list_btn_imgPath.at(i), list_btn_imgPath_forPress.at(i));

            list_btn.at(i)->setObjectName(QString("btn_%1").arg(i));
            list_btn.at(i)->setFixedSize(QSize(BTN_IMAGE_SIZE, BTN_IMAGE_SIZE));
            list_btn.at(i)->setProperty(KEY_CODE, list_btnMode.at(i));
            list_btn.at(i)->setStyleSheet(tmp_btnStyle);
            list_btn.at(i)->setCursor(Qt::PointingHandCursor);
            connect(list_btn.at(i), &QPushButton::clicked, this, &AbstractItemImage::slot_clickedBtnItem);
            hBox_btns->addWidget(list_btn.at(i));
        }

        // 기본값으로 하트ON은 가린다.
        this->btn_fav_toDelete->setVisible(false);


        // hBox_btns 를 가운데로 정렬하기 위해 Widget, Layout 을 중간에 하나씩 더 끼운다.
        QWidget *tmp_widget_forBtn = new QWidget();
        tmp_widget_forBtn->setContentsMargins(0, 0, 0, 0);
        tmp_widget_forBtn->setLayout(hBox_btns);

        QHBoxLayout *tmp_hBox_forBtn = new QHBoxLayout();
        tmp_hBox_forBtn->setSpacing(0);
        tmp_hBox_forBtn->setContentsMargins(0, 0, 0, 0);
        tmp_hBox_forBtn->addWidget(tmp_widget_forBtn, 0, Qt::AlignCenter | Qt::AlignVCenter);



        //------------------------------------------------------------------------

        // AllBox용 push button 세팅
        this->btn_allBox = new QPushButton(this->label_imageBig);
        this->btn_allBox->setContentsMargins(0, 0, 0, 0);
        this->btn_allBox->setObjectName("btn_allBox");
        this->btn_allBox->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight);
        this->btn_allBox->setStyleSheet( this->getStr_hovorBtnStyle("btn_allBox") );
        this->btn_allBox->setAutoFillBackground(true);
        this->btn_allBox->setVisible(false);
        this->btn_allBox->setAttribute(Qt::WA_Hover);
        this->btn_allBox->setMouseTracking(true);
        this->btn_allBox->setLayout(tmp_hBox_forBtn);
        this->btn_allBox->setProperty(KEY_CODE, AbstractItemImage::BtnClickMode::AllBox);

        connect(this->btn_allBox, &QPushButton::clicked, this, &AbstractItemImage::slot_clickedBtnItem);

    }


    /**
     * @brief Hover allBox 버튼에 대한 스타일 text 반환. corner 적용됨
     * @param objName
     * @return
     */
    QString AbstractItemImage::getStr_hovorBtnStyle(const QString objName){
        return QString("#%1 { background-color:#B3000000; border-radius:%2px; }").arg(objName).arg(this->fixed_cornerRadius);
    }


    /**
     * @brief AbstractItemImage::getStr_btnStyle
     * @param index
     * @param btn_imgPath
     * @param btn_imgPath_forPress
     * @return
     */
    QString AbstractItemImage::getStr_btnStyle(const int index, QString btn_imgPath, QString btn_imgPath_forPress){
        QString tmp_btnStyle = " #btn_%1 { "
                               "    background-image:url('%2'); background-repeat:no-repeat; "
                               "    background-position:center; background-color:transparent; "
                               " } "
                               " #btn_%1:pressed { "
                               "    background-image:url('%3'); background-repeat:no-repeat; "
                               "    background-position:center;background-color:transparent; "
                               " } "
                               ;

        return  tmp_btnStyle.arg(index).arg(btn_imgPath).arg(btn_imgPath_forPress);
    }



    /**
     * @brief [Slot] 버튼 클릭에 대한 처리. custom signal 발생
     */
    void AbstractItemImage::slot_clickedBtnItem(){
        BtnClickMode sender_clickMode = sender()->property(KEY_CODE).value<AbstractItemImage::BtnClickMode>();

        if(sender_clickMode==BtnClickMode::Favorite_toAdd){
            // show / hide 교체
            this->btn_fav_toAdd->setVisible(false);
            this->btn_fav_toAddx2->setVisible(true);
            this->btn_fav_toAddx3->setVisible(false);
            this->btn_fav_toDelete->setVisible(false);
        }
        else if(sender_clickMode==BtnClickMode::Favorite_toAddx2){
            // show / hide 교체
            this->btn_fav_toAdd->setVisible(false);
            this->btn_fav_toAddx2->setVisible(false);
            this->btn_fav_toAddx3->setVisible(true);
            this->btn_fav_toDelete->setVisible(false);
        }
        else if(sender_clickMode==BtnClickMode::Favorite_toAddx3){
            // show / hide 교체
            this->btn_fav_toAdd->setVisible(false);
            this->btn_fav_toAddx2->setVisible(false);
            this->btn_fav_toAddx3->setVisible(false);
            this->btn_fav_toDelete->setVisible(true);
        }
        else if(sender_clickMode==BtnClickMode::Favorite_toDelete){
            // show / hide 교체
            this->btn_fav_toAdd->setVisible(true);
            this->btn_fav_toAddx2->setVisible(false);
            this->btn_fav_toAddx3->setVisible(false);
            this->btn_fav_toDelete->setVisible(false);
        }
        else{
            this->applyHoverOff();
        }

        emit this->signal_clickedBtn(sender_clickMode);
    }



    /**
     * @brief AbstractItemImage::slot_fileDownload_loadImage
     */
    void AbstractItemImage::slot_fileDownload_loadImage(){

        /// freezing 이유로 QImage 사용
        QImage image;
        bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

        if(flagLoad){
            QPixmap pixmapIMG = QPixmap(QSize(this->fixed_imgWidth, this->fixed_imgHeight));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(this->fixed_imgWidth, this->fixed_imgHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path;
            path.addRoundedRect(0, 0, this->fixed_imgWidth, this->fixed_imgHeight, this->fixed_cornerRadius, this->fixed_cornerRadius);
            painter.setClipPath(path);

            int leftValue = 0;
            int topValue = 0;

            if(tmp_pixmap.width() > this->fixed_imgWidth){
                leftValue = (this->fixed_imgWidth - tmp_pixmap.width()) / 2;
            }

            if(tmp_pixmap.height() > this->fixed_imgHeight){
                topValue = (this->fixed_imgHeight - tmp_pixmap.height()) / 2;
            }

            painter.drawPixmap(leftValue, topValue, tmp_pixmap);
            painter.end();

            this->label_imageBig->setPixmap(pixmapIMG);

            if(this->flag_type_image){
                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(this->type_image_path)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }

                this->label_imageType->setPixmap(*img_type);
                this->label_imageType->setFixedSize(img.width(), img.height());
                this->label_imageType->setGeometry(0, 0, 0, 0);
                this->label_imageType->show();
            }

            if(this->flag_type_cache){
                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(this->type_image_cache)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }

                this->label_imageCache->setPixmap(*img_type);
                this->label_imageCache->setFixedSize(img.width(), img.height());
                this->label_imageCache->setGeometry(0, 0, 0, 0);

                if(this->flag_cache_show == true){
                    this->label_imageCache->show();
                }
                else if(this->flag_cache_show == false){
                    this->label_imageCache->hide();
                }
            }

            if(this->flag_type_mime){
                int width_set = 0;

                QLabel *tmp_mime = new QLabel();
                tmp_mime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:10px;");
                tmp_mime->setText(this->type_image_mime);

                width_set = tmp_mime->sizeHint().width() + 10;

                this->label_imageMime->setFixedSize(width_set, 20);
                this->label_imageMime->setGeometry(10, this->label_imageBig->sizeHint().height() - 30, 0, 0);
                this->label_imageMime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:2px solid #FFFFFF;border-radius:10px;");
                this->label_imageMime->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                this->label_imageMime->setText(this->type_image_mime);
                this->label_imageMime->show();
            }

            if(this->flag_type_duration){

                this->label_imageDuration->setFixedSize(80,30);
                this->label_imageDuration->setGeometry(0, this->label_imageBig->sizeHint().height() - 30, 0, 0);
                this->label_imageDuration->setStyleSheet("background-color:#99000000;color:#FFFFFF;font-size:18px;border-bottom-left-radius:3px;");
                this->label_imageDuration->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                this->label_imageDuration->setText(this->type_image_duration);
                this->label_imageDuration->show();
            }

            if(this->flag_type_onAir){

                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(this->type_image_onAir)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }

                this->label_imageOnAir->setPixmap(*img_type);
                this->label_imageOnAir->setFixedSize(img.width(), img.height());
                this->label_imageOnAir->setGeometry(5, this->label_imageBig->sizeHint().height() - 23, 0, 0);
                this->label_imageOnAir->show();
            }
        }
        else{
            if(this->flag_type_image){
                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(this->type_image_path)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }

                this->label_imageType->setPixmap(*img_type);
                this->label_imageType->setFixedSize(img.width(), img.height());
                this->label_imageType->setGeometry(0, 0, 0, 0);
                this->label_imageType->show();
            }

            if(this->flag_type_cache){
                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(this->type_image_cache)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }

                this->label_imageCache->setPixmap(*img_type);
                this->label_imageCache->setFixedSize(img.width(), img.height());
                this->label_imageCache->setGeometry(0, 0, 0, 0);

                if(this->flag_cache_show == true){
                    this->label_imageCache->show();
                }
                else if(this->flag_cache_show == false){
                    this->label_imageCache->hide();
                }
            }

            if(this->flag_type_mime){
                int width_set = 0;

                QLabel *tmp_mime = new QLabel();
                tmp_mime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:10px;");
                tmp_mime->setText(this->type_image_mime);

                width_set = tmp_mime->sizeHint().width() + 10;

                this->label_imageMime->setFixedSize(width_set, 20);
                this->label_imageMime->setGeometry(10, this->label_imageBig->sizeHint().height() - 30, 0, 0);
                this->label_imageMime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:2px solid #FFFFFF;border-radius:10px;");
                this->label_imageMime->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                this->label_imageMime->setText(this->type_image_mime);
                this->label_imageMime->show();
            }

            if(this->flag_type_duration){

                this->label_imageDuration->setFixedSize(80,30);
                this->label_imageDuration->setGeometry(0, this->label_imageBig->sizeHint().height() - 30, 0, 0);
                this->label_imageDuration->setStyleSheet("background-color:#99000000;color:#FFFFFF;font-size:18px;border-bottom-left-radius:3px;");
                this->label_imageDuration->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
                this->label_imageDuration->setText(this->type_image_duration);
                this->label_imageDuration->show();
            }

            if(this->flag_type_onAir){

                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(this->type_image_onAir)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }

                this->label_imageOnAir->setPixmap(*img_type);
                this->label_imageOnAir->setFixedSize(img.width(), img.height());
                this->label_imageOnAir->setGeometry(5, this->label_imageBig->sizeHint().height() - 23, 0, 0);
                this->label_imageOnAir->show();
            }
        }

        this->fileDownLoader->deleteLater();
    }


    /**
     * @brief AbstractItemImage::setImage
     * @param imagePath
     */
    void AbstractItemImage::setImage(QString imagePath){

        if(imagePath.isEmpty()){
            // 기본 이미지로 세팅
            QPixmap pixmap(this->getImagePath_default());
            pixmap = pixmap.scaled(this->fixed_imgWidth, this->fixed_imgHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            this->paint_imageBig(pixmap);
        }
        else{
            if(imagePath.contains(":/images")){
                QPixmap pixmap(imagePath);
                pixmap = pixmap.scaled(this->fixed_imgWidth, this->fixed_imgHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                this->paint_imageBig(pixmap);
            }
            else{
                this->fileDownLoader->setImageURL(imagePath);
            }
        }
    }


    /**
     * @brief pixmap 을 label_image 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
     * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
     * @param pixmap
     */
    void AbstractItemImage::paint_imageBig(QPixmap &pixmap){
        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(this->fixed_imgWidth, this->fixed_imgHeight));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, this->fixed_imgWidth, this->fixed_imgHeight, this->fixed_cornerRadius, this->fixed_cornerRadius);
        this->label_imageBig->setPixmap(pixmap_painter);

        if(this->flag_type_image){
            QImage img;
            QPixmap *img_type = new QPixmap();
            if(img.load(this->type_image_path)){
                *img_type = QPixmap::fromImage(img);
                *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            this->label_imageType->setPixmap(*img_type);
            this->label_imageType->setFixedSize(img.width(), img.height());
            this->label_imageType->setGeometry(0, 0, 0, 0);
            this->label_imageType->show();
        }

        if(this->flag_type_cache){
            QImage img;
            QPixmap *img_type = new QPixmap();
            if(img.load(this->type_image_cache)){
                *img_type = QPixmap::fromImage(img);
                *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            this->label_imageCache->setPixmap(*img_type);
            this->label_imageCache->setFixedSize(img.width(), img.height());
            this->label_imageCache->setGeometry(0, 0, 0, 0);

            if(this->flag_cache_show == true){
                this->label_imageCache->show();
            }
            else if(this->flag_cache_show == false){
                this->label_imageCache->hide();
            }
        }

        if(this->flag_type_mime){
            int width_set = 0;

            QLabel *tmp_mime = new QLabel();
            tmp_mime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:10px;");
            tmp_mime->setText(this->type_image_mime);

            width_set = tmp_mime->sizeHint().width() + 10;

            this->label_imageMime->setFixedSize(width_set, 20);
            this->label_imageMime->setGeometry(10, this->label_imageBig->sizeHint().height() - 30, 0, 0);
            this->label_imageMime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:2px solid #FFFFFF;border-radius:10px;");
            this->label_imageMime->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            this->label_imageMime->setText(this->type_image_mime);
            this->label_imageMime->show();
        }

        if(this->flag_type_duration){

            this->label_imageDuration->setFixedSize(80,30);
            this->label_imageDuration->setGeometry(0, this->label_imageBig->sizeHint().height() - 30, 0, 0);
            this->label_imageDuration->setStyleSheet("background-color:#99000000;color:#FFFFFF;font-size:18px;border-bottom-left-radius:3px;");
            this->label_imageDuration->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
            this->label_imageDuration->setText(this->type_image_duration);
            this->label_imageDuration->show();
        }

        if(this->flag_type_onAir){

            QImage img;
            QPixmap *img_type = new QPixmap();
            if(img.load(this->type_image_onAir)){
                *img_type = QPixmap::fromImage(img);
                *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            this->label_imageOnAir->setPixmap(*img_type);
            this->label_imageOnAir->setFixedSize(img.width(), img.height());
            this->label_imageOnAir->setGeometry(5, this->label_imageBig->sizeHint().height() - 23, 0, 0);
            this->label_imageOnAir->show();
        }
    }


    void AbstractItemImage::setTypeImage(QString imagePath){
        this->flag_type_image = true;
        this->type_image_path = imagePath;
    }

    void AbstractItemImage::setTypeCache(QString imagePath){
        this->flag_type_cache = true;
        this->type_image_cache = imagePath;
    }

    void AbstractItemImage::setCacheShow(bool flag){

        this->flag_cache_show = flag;

        if(this->flag_cache_show == true){
            this->label_imageCache->show();
        }
        else if(this->flag_cache_show == false){
            this->label_imageCache->hide();
        }
    }

    void AbstractItemImage::setTypeMime(QString mime){
        this->flag_type_mime = true;
        this->type_image_mime = mime;
    }

    void AbstractItemImage::setTypeDuration(QString duration){
        this->flag_type_duration = true;
        this->type_image_duration = duration;
    }

    void AbstractItemImage::setTypeOnAir(QString imagePath){
        this->flag_type_onAir = true;
        this->type_image_onAir = imagePath;
    }


//    /**
//     * @brief 이미지 가로 사이즈 반환
//     * @return
//     */
//    int AbstractItemImage::get_imageWidth(){
//        return this->fixed_imgWidth;
//    }

//    /**
//     * @brief 이미지 전체 세로 사이즈 반환
//     * @return
//     */
//    int AbstractItemImage::get_imageHeight(){
//        if(this->label_topListBG->isVisible()){
//            return this->fixed_imgHeight + HEIGHT_TOP_SEVERAL_BG;
//        }
//        else{
//            return this->fixed_imgHeight;
//        }

//    }

    /**
     * @brief AbstractItemImage::sizeHint
     * @return
     */
    QSize AbstractItemImage::sizeHint() const{
        if(this->flagUseTopBG){
            return QSize(this->fixed_imgWidth, this->fixed_imgHeight + HEIGHT_TOP_SEVERAL_BG);
        }
        else{
            return QSize(this->fixed_imgWidth, this->fixed_imgHeight);
        }
    }

    /**
     * @brief 기본 이미지 반환. 200*200 사이즈
     * @return
     */
    QString AbstractItemImage::getImagePath_default(){
        return ":/images/def_mus_200.png";
    }


    /**
     * @brief AbstractItemImage::show_topListBG
     * @param flagShow
     */
    void AbstractItemImage::show_topListBG(bool flagShow){
        if(flagShow){
            this->flagUseTopBG = true;

            //print_tidal_func();

            if(this->fixed_imgWidth >= 360)
            {
                // large 적용
                this->label_topListBG->setPixmap(QPixmap(imgPath_topBG_large));
                this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight + 22);
            }
            else if(this->fixed_imgWidth < 360 && this->fixed_imgWidth >= 260){
                // medium 적용
                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(imgPath_topBG_medium)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(this->fixed_imgWidth - 40, img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }

                this->label_topListBG->setPixmap(*img_type);
                this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight + 20);
            }
            else if(this->fixed_imgWidth < 260 && this->fixed_imgWidth > 200){
                // medium 적용
                QImage img;
                QPixmap *img_type = new QPixmap();
                if(img.load(imgPath_topBG_medium)){
                    *img_type = QPixmap::fromImage(img);
                    *img_type = img_type->scaled(this->fixed_imgWidth - 32, img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                }

                this->label_topListBG->setPixmap(*img_type);
                this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight + 20);
            }
            else if(this->fixed_imgWidth == 200){
                // normal 적용
                this->label_topListBG->setPixmap(QPixmap(imgPath_topBG_basic));
                this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight + 20);
            }
            else{
                // small 적용
                this->label_topListBG->setPixmap(QPixmap(imgPath_topBG_small));
                this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight + 18);
            }

            this->label_topListBG->setVisible(true);

        }
        else{
            this->flagUseTopBG = false;
            this->label_topListBG->setVisible(false);
            this->setFixedSize(this->fixed_imgWidth, this->fixed_imgHeight);
        }
    }

    /**
     * @brief 좋아요 버튼 사용여부 세팅
     * @param flagUse
     */
    void AbstractItemImage::setUseBtn_favorite(bool flagUse){
        this->flagUseFavorite = flagUse;
        if(flagUse){
            if(this->flagFavorite){
                this->btn_fav_toAdd->setVisible(false);
                this->btn_fav_toDelete->setVisible(true);
            }
            else{
                this->btn_fav_toAdd->setVisible(true);
                this->btn_fav_toDelete->setVisible(false);
            }
        }
        else{
            this->btn_fav_toAdd->setVisible(false);
            this->btn_fav_toAddx2->setVisible(false);
            this->btn_fav_toAddx3->setVisible(false);
            this->btn_fav_toDelete->setVisible(false);
        }
    }


    void AbstractItemImage::setUseBtn_favoriteRose(bool flagUse, int favCnt){

        this->flagUseFavorite = flagUse;
        this->UseFavoriteRoseCnt = favCnt;
        if(flagUse){
            if(favCnt == 0){
                this->btn_fav_toAdd->setVisible(true);
                this->btn_fav_toAddx2->setVisible(false);
                this->btn_fav_toAddx3->setVisible(false);
                this->btn_fav_toDelete->setVisible(false);
            }
            else if(favCnt == 1){
                this->btn_fav_toAdd->setVisible(false);
                this->btn_fav_toAddx2->setVisible(true);
                this->btn_fav_toAddx3->setVisible(false);
                this->btn_fav_toDelete->setVisible(false);
            }
            else if(favCnt == 2){
                this->btn_fav_toAdd->setVisible(false);
                this->btn_fav_toAddx2->setVisible(false);
                this->btn_fav_toAddx3->setVisible(true);
                this->btn_fav_toDelete->setVisible(false);
            }
            else if(favCnt == 3){
                this->btn_fav_toAdd->setVisible(false);
                this->btn_fav_toAddx2->setVisible(false);
                this->btn_fav_toAddx3->setVisible(false);
                this->btn_fav_toDelete->setVisible(true);
            }
        }
        else{
            this->btn_fav_toAdd->setVisible(false);
            this->btn_fav_toAddx2->setVisible(false);
            this->btn_fav_toAddx3->setVisible(false);
            this->btn_fav_toDelete->setVisible(false);
        }
    }


    /**
     * @brief Favorite 초기값 설정
     * @param flagFavorite
     */
    void AbstractItemImage::setFavorite(bool flagFavorite){
        if(this->flagUseFavorite){
            this->flagFavorite = flagFavorite;
            if(this->flagFavorite){
                this->btn_fav_toAdd->setVisible(false);
                this->btn_fav_toDelete->setVisible(true);
            }
            else{
                this->btn_fav_toAdd->setVisible(true);
                this->btn_fav_toDelete->setVisible(false);
            }
        }
    }


    /**
     * @brief 재생 버튼 사용여부 세팅
     * @param flagUse
     */
    void AbstractItemImage::setUseBtn_play(bool flagUse){
        if(flagUse){
            this->btn_play->setVisible(true);
        }
        else{
            this->btn_play->setVisible(false);
        }
    }


    /**
     * @brief 더보기 버튼 사용여부 세팅
     * @param flagUse
     */
    void AbstractItemImage::setUseBtn_more(bool flagUse){
        if(flagUse){
            this->btn_more->setVisible(true);
        }
        else{
            this->btn_more->setVisible(false);
        }
    }


    /**
     * @brief AbstractItemImage::applyHoverOn
     */
    void AbstractItemImage::applyHoverOn(){
        if(this->flagUseHover){
            this->btn_allBox->setVisible(true);
            this->btn_allBox->setCursor(QCursor(Qt::PointingHandCursor));
        }
    }


    /**
     * @brief AbstractItemImage::applyHoverOff
     */
    void AbstractItemImage::applyHoverOff(){
        if(this->flagUseHover){
            this->btn_allBox->setVisible(false);
            this->btn_allBox->setCursor(QCursor(Qt::ArrowCursor));
        }
    }

    void AbstractItemImage::applyHoverOn2(){
        this->setUseBtn_favorite(false);
        this->btn_play->setVisible(this->flagUseHover2);
        this->btn_more->setVisible(this->flagUseHover2);

        this->btn_allBox->setVisible(true);
        this->btn_allBox->setCursor(QCursor(Qt::PointingHandCursor));
    }

    void AbstractItemImage::applyHoverOff2(){
        this->setUseBtn_favorite(false);
        this->btn_play->setVisible(false);
        this->btn_more->setVisible(false);

        this->btn_allBox->setVisible(false);
        this->btn_allBox->setCursor(QCursor(Qt::ArrowCursor));
    }

    void AbstractItemImage::applyHoverOnRose(){
        if(this->flagUseHoverRose){
            this->btn_allBox->setVisible(true);
            this->btn_allBox->setCursor(QCursor(Qt::PointingHandCursor));
        }
    }

    void AbstractItemImage::applyHoverOffRose(){
        if(this->flagUseHoverRose){
            this->btn_allBox->setVisible(false);
            this->btn_allBox->setCursor(QCursor(Qt::ArrowCursor));
        }
    }


    /**
     * @brief AbstractItemImage::setUseHover
     * @param flagUse
     */
    void AbstractItemImage::setUseHover(bool flagUse){
        this->flagUseHover = flagUse;
    }

    void AbstractItemImage::setUseHover2(bool flagUse){
        this->flagUseHover2 = flagUse;
    }

    void AbstractItemImage::setUseHoverRose(bool flagUse){
        this->flagUseHoverRose = flagUse;
    }

    /**
     * @brief ImageLabel::enterEvent : 마우스 Enter 이벤트 슬롯
     * @param event QEvent
     */
    void AbstractItemImage::enterEvent(QEvent *event){
        Q_UNUSED(event);
        if(this->flagUseHover == true){
            this->applyHoverOn();
        }
        else if(this->flagUseHoverRose == true){
            this->applyHoverOnRose();
        }
        else{
            this->applyHoverOn2();
        }
    }


    /**
     * @brief ImageLabel::leaveEvent : 마우스 Leave 이벤트 슬롯
     * @param event QEvent
     */
    void AbstractItemImage::leaveEvent(QEvent *event){
        Q_UNUSED(event);
        if(this->flagUseHover == true){
            this->applyHoverOff();
        }
        else if(this->flagUseHoverRose == true){
            this->applyHoverOffRose();
        }
        else{
            this->applyHoverOff2();
        }
    }

}
