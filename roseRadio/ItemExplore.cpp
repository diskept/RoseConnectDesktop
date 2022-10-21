#include "roseRadio/ItemExplore.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>


namespace roseRadio {

    const int ITEM_BETWEEN_MARGIN_RIGHT = 20;

    /**
     * @brief
     * @param parent
     */
    ItemExplore::ItemExplore(int index, int section, QWidget *parent)
    : QPushButton(parent)
    {

        this->windex = index;
        this->wsection = section;

        this->setUIControl();
    }


    ItemExplore::~ItemExplore(){

        this->deleteLater();
    }


    /**
     * @brief [override] UI 세팅
     */
    void ItemExplore::setUIControl(){

        // 파일다운로드 객체 생성 및 커넥트
        this->pixmap_albumImg = new QPixmap();
        this->fileDownLoader = new FileDownloader(this);
        connect(this->fileDownLoader, &FileDownloader::downloaded, this, &ItemExplore::slot_fileDownload_loadImage);

        this->label_background = new QLabel();
        this->label_background->setFixedSize(284, 284);
        this->label_background->setScaledContents(true);
        this->label_background->setStyleSheet("background-color:tranparent");
        this->label_background->setAlignment(Qt::AlignCenter);

        this->label_icon = new QLabel(this->label_background);
        this->label_icon->setScaledContents(true);
        this->label_icon->setGeometry(158, 30, 96, 96);
        this->label_icon->setStyleSheet("background-color:transparent;");

        this->label_name = new QLabel(this->label_background);
        this->label_name->setFixedSize(234, 28);
        this->label_name->setStyleSheet("background-color:transparent; color:#FFFFFF; font-size:24px; font-weight:bold; line-height: 0.71;");
        this->label_name->setGeometry(25, 237, 0, 0);

        this->btn_allBox = new QPushButton(this->label_background);
        this->btn_allBox->setContentsMargins(0, 0, 0, 0);
        this->btn_allBox->setObjectName("btn_allBox");
        this->btn_allBox->setFixedSize(284, 284);
        this->btn_allBox->setStyleSheet("#btn_allBox { background-color:#B3000000; border-radius:6px; }");
        this->btn_allBox->setAutoFillBackground(true);
        this->btn_allBox->setVisible(false);
        this->btn_allBox->setAttribute(Qt::WA_Hover);
        this->btn_allBox->setMouseTracking(true);

        connect(this->btn_allBox, &QPushButton::clicked, this, &ItemExplore::slot_clickBtn_signal);

        QVBoxLayout *boxLayout = new QVBoxLayout();
        boxLayout->setContentsMargins(0, 0, 0, 0);
        boxLayout->addWidget(this->label_background);

        this->setFixedSize(284, 284);
        this->setContentsMargins(0, 0, ITEM_BETWEEN_MARGIN_RIGHT, 0);
        this->setLayout(boxLayout);
    }


    /**
     * @brief 데이터 세팅
     * @param p_jsonObject 데이터
     */
    void ItemExplore::setData(const QJsonObject &p_data){

        this->data_genre = p_data;

        QString name = ProcJsonEasy::getString(this->data_genre, "name");
        QString imageUrl = ProcJsonEasy::getString(this->data_genre, "imageUrl");

        if(imageUrl.contains(":/image")){
            this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(imageUrl);
            *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        if(name == "Country"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(187, 187, 187, 255), stop:1 rgba(142, 125, 115, 255)); border-radius:6px;");
        }
        else if(name == "Language"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(207, 152, 113, 255), stop:1 rgba(90, 49, 87, 255)); border-radius:6px;");
        }
        else if(name == "Classic"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(227, 191, 91, 255), stop:1 rgba(124, 180, 113, 255)); border-radius:6px;");
        }
        else if(name == "Country Music"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(98, 227, 52, 255), stop:1 rgba(32, 227, 150, 255)); border-radius:6px;");
        }
        else if(name == "ETC"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(176, 79, 163, 255), stop:1 rgba(206, 117, 117, 255)); border-radius:6px;");
        }
        else if(name == "Folk"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(214, 91, 18, 255), stop:1 rgba(235, 203, 30, 255)); border-radius:6px;");
        }
        else if(name == "Hip-Hop"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(21, 235, 160, 255), stop:1 rgba(10, 165, 235, 255)); border-radius:6px;");
        }
        else if(name == "Indie"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(235, 139, 226, 255), stop:1 rgba(180, 78, 235, 255)); border-radius:6px;");
        }
        else if(name == "Jazz"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(105, 70, 176, 255), stop:1 rgba(84, 104, 168, 255)); border-radius:6px;");
        }
        else if(name == "Music"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(151, 252, 224, 255), stop:1 rgba(77, 84, 193, 255)); border-radius:6px;");
        }
        else if(name == "News"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(137, 173, 227, 255), stop:1 rgba(62, 47, 97, 255)); border-radius:6px;");
        }
        else if(name == "POP"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(235, 54, 83, 255), stop:1 rgba(247, 142, 128, 255)); border-radius:6px;");
        }
        else if(name == "R&B"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(142, 64, 58, 255), stop:1 rgba(191, 85, 157, 255)); border-radius:6px;");
        }
        else if(name == "Rock"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(55, 99, 78, 255), stop:1 rgba(63, 77, 99, 255)); border-radius:6px;");
        }
        else if(name == "Sports"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(223, 144, 114, 255), stop:1 rgba(98, 135, 183, 255)); border-radius:6px;");
        }
        else if(name == "Music-Chart"){
            this->label_background->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(53, 128, 187, 255), stop:1 rgba(130, 125, 217, 255)); border-radius:6px;");
        }

        this->setImage(imageUrl);
        this->label_name->setText(name);
    }


    void ItemExplore::setImage(QString imagePath){

        // 이미지 경로 설정
        if(imagePath.isEmpty() == false){
             this->fileDownLoader->setImageURL(QUrl(imagePath));
        }
        else{
            this->paint_image(*this->pixmap_albumImg_default);
        }
    }


    void ItemExplore::slot_fileDownload_loadImage()
    {
        QImage image;
        bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

        if(flagLoad){
            QPixmap pixmapIMG = QPixmap(QSize(96, 96));
            pixmapIMG.fill(Qt::transparent);

            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(96, 96, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 96, 96, 2, Qt::RelativeSize);
            painter.setClipPath(path);
            painter.drawPixmap(0, 0, tmp_pixmap);
            painter.end();

            this->label_icon->setPixmap(pixmapIMG);
        }
        else{
            this->paint_image(*this->pixmap_albumImg_default);
        }

        this->fileDownLoader->deleteLater();
    }


    void ItemExplore::slot_clickBtn_signal(){

        emit explore_clicked(this->wsection, this->windex, roseRadio::ItemExplore::ClickMode_radio::AllBox);
    }

    void ItemExplore::paint_image(QPixmap &pixmap){

        // 빈 Pixmap
        QPixmap pixmapIMG = QPixmap(QSize(96, 96));
        pixmapIMG.fill(Qt::transparent);

        pixmap = pixmap.scaled(96, 96, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, 96, 96, 2, Qt::RelativeSize);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, pixmap);
        painter.end();

        this->label_icon->setPixmap(pixmapIMG);
    }


    void ItemExplore::enterEvent(QEvent *event){
        Q_UNUSED(event);

        this->btn_allBox->setVisible(true);
        this->btn_allBox->setCursor(QCursor(Qt::PointingHandCursor));
    }


    /**
     * @brief ImageLabel::leaveEvent : 마우스 Leave 이벤트 슬롯
     * @param event QEvent
     */
    void ItemExplore::leaveEvent(QEvent *event){
        Q_UNUSED(event);

        this->btn_allBox->setVisible(false);
        this->btn_allBox->setCursor(QCursor(Qt::ArrowCursor));
    }
}
