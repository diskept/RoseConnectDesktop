#include "roseRadio/ItemImage_RoseRadio.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>


namespace roseRadio {

    /**
     * @brief
     * @param parent
     */
    ItemImageRoseRadio::ItemImageRoseRadio(int imageSize, QWidget *parent)
        : QWidget(parent)
    {
        this->image_size = imageSize;

        this->setUIControl();
    }


    ItemImageRoseRadio::~ItemImageRoseRadio(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemImageRoseRadio::setUIControl(){

        // 파일다운로드 객체 생성 및 커넥트
        this->pixmap_albumImg = new QPixmap();
        this->fileDownLoader = new FileDownloader(this);
        connect(this->fileDownLoader, &FileDownloader::downloaded, this, &ItemImageRoseRadio::slot_fileDownload_loadImage);

        this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
        *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        this->label_image = new QLabel();
        this->label_image->setFixedSize(this->image_size, this->image_size);
        this->label_image->setScaledContents(true);
        this->label_image->setStyleSheet("background-color: tranparent; border-radius:4px;");
        this->label_image->setAlignment(Qt::AlignCenter);

        this->label_imageOnAir = new QLabel(this->label_image);

        this->btn_allBox = new QPushButton(this->label_image);
        this->btn_allBox->setContentsMargins(0, 0, 0, 0);
        this->btn_allBox->setObjectName("btn_allBox");
        this->btn_allBox->setFixedSize(this->image_size, this->image_size);
        this->btn_allBox->setStyleSheet("#btn_allBox { background-color:#B3000000; border-radius:4px; }");
        this->btn_allBox->setProperty("type", "allBox");
        this->btn_allBox->setAutoFillBackground(true);
        this->btn_allBox->setVisible(false);
        this->btn_allBox->setAttribute(Qt::WA_Hover);
        this->btn_allBox->setMouseTracking(true);

        QString tmp_btnStyle = " #btn_play { "
                               "    background-image:url(':/images/album_play_icon.png'); background-repeat:no-repeat; "
                               "    background-position:center; background-color:transparent; "
                               " } "
                               " #btn_play:pressed { "
                               "    background-image:url(':/images/album_play_icon_h.png'); background-repeat:no-repeat; "
                               "    background-position:center;background-color:transparent; "
                               " } "
                               ;

        int position = (this->image_size - 60) / 2;
        this->btn_play = new QPushButton(this->btn_allBox);
        this->btn_play->setObjectName("btn_play");
        this->btn_play->setFixedSize(QSize(60, 60));
        this->btn_play->setProperty("type", "play");
        this->btn_play->setStyleSheet(tmp_btnStyle);
        this->btn_play->setCursor(Qt::PointingHandCursor);
        this->btn_play->setGeometry(position, position, 0, 0);

        connect(this->btn_play, &QPushButton::clicked, this, &ItemImageRoseRadio::slot_clickBtn_signal);
        connect(this->btn_allBox, &QPushButton::clicked, this, &ItemImageRoseRadio::slot_clickBtn_signal);

        this->vBox_image = new QVBoxLayout();
        this->vBox_image->setSpacing(0);
        this->vBox_image->setContentsMargins(0, 0, 0 ,0);
        this->vBox_image->addWidget(this->label_image);



        this->setLayout(this->vBox_image);
        this->setContentsMargins(0, 0, 0, 0);
        this->setFixedSize(this->image_size, this->image_size);
    }


    void ItemImageRoseRadio::setImage(QString imagePath){

        // 이미지 경로 설정
        if(imagePath.isEmpty() == false){
             this->fileDownLoader->setImageURL(QUrl(imagePath));
        }
        else{
            this->paint_image(*this->pixmap_albumImg_default);
        }
    }


    void ItemImageRoseRadio::setTypeOnAir(QString imagePath){

        QImage img;
        QPixmap *img_type = new QPixmap();
        if(img.load(imagePath)){
            *img_type = QPixmap::fromImage(img);
            *img_type = img_type->scaled(img.width(), img.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->label_imageOnAir->setPixmap(*img_type);
        this->label_imageOnAir->setFixedSize(img.width(), img.height());
        this->label_imageOnAir->setGeometry(5, this->image_size - 25, 0, 0);
    }


    void ItemImageRoseRadio::slot_fileDownload_loadImage()
    {
        QImage image;
        bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

        if(flagLoad){
            QPixmap pixmapIMG = QPixmap(QSize(this->image_size, this->image_size));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(this->image_size, this->image_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, this->image_size, this->image_size, 4, 4);
            painter.setClipPath(path);
            painter.drawPixmap(0, 0, tmp_pixmap);
            painter.end();

            this->label_image->setPixmap(pixmapIMG);
        }
        else{
            this->paint_image(*this->pixmap_albumImg_default);
        }

        this->fileDownLoader->deleteLater();
    }


    void ItemImageRoseRadio::slot_clickBtn_signal(){

        QString type = sender()->property("type").toString();

        if(type == "allBox"){
            emit this->roseRadio_clicked(roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox);
        }
        else if(type == "play"){
            emit this->roseRadio_clicked(roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn);
        }
    }


    void ItemImageRoseRadio::paint_image(QPixmap &pixmap){

        // 빈 Pixmap
        QPixmap pixmapIMG = QPixmap(QSize(this->image_size, this->image_size));
        pixmapIMG.fill(Qt::transparent);

        pixmap = pixmap.scaled(this->image_size, this->image_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, this->image_size, this->image_size, 4, 4);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, pixmap);
        painter.end();

        this->label_image->setPixmap(pixmapIMG);
    }


    void ItemImageRoseRadio::enterEvent(QEvent *event){
        Q_UNUSED(event);

        this->btn_allBox->setVisible(true);
        this->btn_allBox->setCursor(QCursor(Qt::PointingHandCursor));
    }


    /**
     * @brief ImageLabel::leaveEvent : 마우스 Leave 이벤트 슬롯
     * @param event QEvent
     */
    void ItemImageRoseRadio::leaveEvent(QEvent *event){
        Q_UNUSED(event);

        this->btn_allBox->setVisible(false);
        this->btn_allBox->setCursor(QCursor(Qt::ArrowCursor));
    }
}
