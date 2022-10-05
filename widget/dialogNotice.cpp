#include "widget/dialogNotice.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>


const int DLG_WIDTH = 820;
const int DLG_HIDTH = 650;

QString IMAGE_PATH_DEFAULT_QOBUZ = ":/images/qobuz/qobuz_default_400.png";


DialogNotice::DialogNotice(QWidget *parent, DlgNotice p_confirmType)
    : QDialog(parent)
    , confirmType(p_confirmType)
{

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &DialogNotice::slot_fileDownload_loadImage);

    setInit();
    setUIControl();
}


DialogNotice::~DialogNotice(){

    this->deleteLater();
}


void DialogNotice::setInit(){

    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HIDTH);
    this->setStyleSheet("background-color:transparent;");
}


void DialogNotice::setUIControl(){

    this->widget_total = new QWidget();
    this->widget_total->setObjectName("widget_total");
    this->widget_total->setStyleSheet("#widget_total {background-color:#333333;border-radius:10px;border:1px solid #4D4D4D;}");
    this->widget_total->setFixedSize(800, 630);

    this->box_total = new QVBoxLayout();

    if(this->confirmType == Qobuz_artist_more){

        QString tmp_text = QString("<html><head/><body><span style='font-size:20px; font-weight:500; line-height:1.88; color:#0F78F0;'>Biography</span></body></html>");

        this->lb_titleType = new QLabel(this->widget_total);
        this->lb_titleType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->lb_titleType->setFixedSize(200, 25);
        this->lb_titleType->setGeometry(30, 20, 0, 0);
        this->lb_titleType->setText(tmp_text);

        this->lb_title = new LabelLongAnimation(680, 530, 40, this->widget_total);
        this->lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:30px;font-weight:500;");
        this->lb_title->setGeometry(30, 45, 0, 0);

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png", this->widget_total);
        btn_close->setFixedSize(QSize(70, 70));
        btn_close->setCursor(Qt::PointingHandCursor);
        btn_close->setGeometry(730, -10, 0, 0);

        connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

        QWidget *wg_line = new QWidget(this->widget_total);
        wg_line->setObjectName("wg_top");
        wg_line->setStyleSheet("#wg_top {background-color:#404040;}");
        wg_line->setFixedSize(DLG_WIDTH, 1);
        wg_line->setGeometry(0, 95, 0, 0);


        this->lb_content = new QLabel();
        this->lb_content->setAlignment(Qt::AlignLeft);
        this->lb_content->setFixedWidth(680);
        this->lb_content->setWordWrap(true);

        this->scrollArea_content = new QScrollArea(this->widget_total);
        this->scrollArea_content->setWidgetResizable(true);
        this->scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->scrollArea_content->setStyleSheet("background-color:transparent;border:none;");
        this->scrollArea_content->setFixedSize(730, 440);
        this->scrollArea_content->setGeometry(30, 110, 0, 0);
        this->scrollArea_content->setWidget(this->lb_content);


        // 확인 버튼
        QPushButton *btn_confirm = new QPushButton(this->widget_total);
        btn_confirm->setText(tr("Confirm"));
        btn_confirm->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
        btn_confirm->setCursor(Qt::PointingHandCursor);
        btn_confirm->setFixedSize(140, 40);
        btn_confirm->setGeometry(330, 570, 0, 0);

        connect(btn_confirm, SIGNAL(clicked()), this, SLOT(accept()));

        this->box_total->addWidget(this->widget_total);
    }
    else if(this->confirmType == Qobuz_album_info){

        // 파일다운로드 객체 생성 및 커넥트
        this->pixmap_albumImg = new QPixmap();

        this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT_QOBUZ);
        *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        this->lb_title_img = new QLabel(this->widget_total);
        this->lb_title_img->setStyleSheet("background-color:transparent;");
        this->lb_title_img->setFixedSize(140, 140);
        this->lb_title_img->setGeometry(20, 20, 0, 0);

        this->paint_imageBig(*this->pixmap_albumImg_default);

        this->lb_title = new LabelLongAnimation(500, 300, 40, this->widget_total);
        this->lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:35px;font-weight:500;");
        this->lb_title->setFixedSize(500, 40);
        this->lb_title->setGeometry(190, 20, 0, 0);

        this->lb_artist = new QLabel(this->widget_total);
        this->lb_artist->setStyleSheet("background-color:transparent;");
        this->lb_artist->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        //this->lb_artist->setFixedSize(500, 25);
        this->lb_artist->setGeometry(190, 80, 500, 25);

        this->lb_composer = new QLabel(this->widget_total);
        this->lb_composer->setStyleSheet("background-color:transparent;");
        this->lb_composer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->lb_composer->setFixedSize(500, 25);
        this->lb_composer->setGeometry(190, 105, 0, 0);

        this->lb_released = new QLabel(this->widget_total);
        this->lb_released->setStyleSheet("background-color:transparent;");
        this->lb_released->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        this->lb_released->setFixedSize(600, 50);
        this->lb_released->setWordWrap(true);
        this->lb_released->setGeometry(190, 130, 0, 0);

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png", this->widget_total);
        btn_close->setFixedSize(QSize(70, 70));
        btn_close->setCursor(Qt::PointingHandCursor);
        btn_close->setGeometry(730, -10, 0, 0);

        connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

        QWidget *wg_line = new QWidget(this->widget_total);
        wg_line->setObjectName("wg_top");
        wg_line->setStyleSheet("#wg_top {background-color:#404040;}");
        wg_line->setFixedSize(DLG_WIDTH, 1);
        wg_line->setGeometry(0, 200, 0, 0);


        this->lb_content = new QLabel();
        this->lb_content->setAlignment(Qt::AlignLeft);
        this->lb_content->setFixedWidth(680);
        this->lb_content->setWordWrap(true);

        this->scrollArea_content = new QScrollArea(this->widget_total);
        this->scrollArea_content->setWidgetResizable(true);
        this->scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->scrollArea_content->setStyleSheet("background-color:transparent;border:none;");
        this->scrollArea_content->setFixedSize(730, 330);
        this->scrollArea_content->setGeometry(30, 225, 0, 0);
        //this->scrollArea_content->setWidget(this->lb_resolution);
        this->scrollArea_content->setWidget(this->lb_content);


        // 확인 버튼
        QPushButton *btn_confirm = new QPushButton(this->widget_total);
        btn_confirm->setText(tr("Confirm"));
        btn_confirm->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
        btn_confirm->setCursor(Qt::PointingHandCursor);
        btn_confirm->setFixedSize(140, 40);
        btn_confirm->setGeometry(330, 570, 0, 0);

        connect(btn_confirm, SIGNAL(clicked()), this, SLOT(accept()));

        this->box_total->addWidget(this->widget_total);
    }
    else if(this->confirmType == Music_album_info){

        // 파일다운로드 객체 생성 및 커넥트
        this->pixmap_albumImg = new QPixmap();

        this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT_QOBUZ);
        *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        this->lb_title_img = new QLabel(this->widget_total);
        this->lb_title_img->setStyleSheet("background-color:transparent;");
        this->lb_title_img->setFixedSize(140, 140);
        this->lb_title_img->setGeometry(20, 20, 0, 0);

        this->paint_imageBig(*this->pixmap_albumImg_default);

        this->lb_title = new LabelLongAnimation(500, 300, 40, this->widget_total);
        this->lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:35px;font-weight:500;");
        this->lb_title->setFixedSize(500, 40);
        this->lb_title->setGeometry(190, 20, 0, 0);

        this->lb_artist = new QLabel(this->widget_total);
        this->lb_artist->setStyleSheet("background-color:transparent;");
        this->lb_artist->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        //this->lb_artist->setFixedSize(500, 25);
        this->lb_artist->setGeometry(190, 80, 500, 25);

        this->lb_composer = new QLabel(this->widget_total);
        this->lb_composer->setStyleSheet("background-color:transparent;");
        this->lb_composer->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->lb_composer->setFixedSize(500, 25);
        this->lb_composer->setGeometry(190, 105, 0, 0);

        this->lb_released = new QLabel(this->widget_total);
        this->lb_released->setStyleSheet("background-color:transparent;");
        this->lb_released->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        this->lb_released->setFixedSize(600, 50);
        this->lb_released->setWordWrap(true);
        this->lb_released->setGeometry(190, 130, 0, 0);

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":/images/icon_close_gray.png", this->widget_total);
        btn_close->setFixedSize(QSize(70, 70));
        btn_close->setCursor(Qt::PointingHandCursor);
        btn_close->setGeometry(730, -10, 0, 0);

        connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

        QWidget *wg_line = new QWidget(this->widget_total);
        wg_line->setObjectName("wg_top");
        wg_line->setStyleSheet("#wg_top {background-color:#404040;}");
        wg_line->setFixedSize(DLG_WIDTH, 1);
        wg_line->setGeometry(0, 200, 0, 0);


        this->lb_content = new QLabel();
        this->lb_content->setAlignment(Qt::AlignLeft);
        this->lb_content->setFixedWidth(680);
        this->lb_content->setWordWrap(true);

        this->scrollArea_content = new QScrollArea(this->widget_total);
        this->scrollArea_content->setWidgetResizable(true);
        this->scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        this->scrollArea_content->setStyleSheet("background-color:transparent;border:none;");
        this->scrollArea_content->setFixedSize(730, 330);
        this->scrollArea_content->setGeometry(30, 225, 0, 0);
        //this->scrollArea_content->setWidget(this->lb_resolution);
        this->scrollArea_content->setWidget(this->lb_content);


        // 확인 버튼
        QPushButton *btn_confirm = new QPushButton(this->widget_total);
        btn_confirm->setText(tr("Confirm"));
        btn_confirm->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
        btn_confirm->setCursor(Qt::PointingHandCursor);
        btn_confirm->setFixedSize(140, 40);
        btn_confirm->setGeometry(330, 570, 0, 0);

        connect(btn_confirm, SIGNAL(clicked()), this, SLOT(accept()));

        this->box_total->addWidget(this->widget_total);
    }

    this->setLayout(box_total);
}


void DialogNotice::setData_formJsonObj(const QJsonObject &jsonObj){

    if(this->confirmType == Qobuz_artist_more){

        QString artist = ProcJsonEasy::getString(jsonObj, "artist");
        this->lb_title->setText(artist);

        QString tmpData = ProcJsonEasy::getString(jsonObj, "biography");
        if(tmpData.isEmpty()){
            tmpData = tr("No artist biography content.");
        }

        QString biography = QString("<html><head/><body><br /><br /><span style='font-size:20px; font-weight:300; line-height: 1.88; color:#999999;'>%1</span><br /><br /></body></html>").arg(tmpData);
        this->lb_content->setText(biography);
    }
    else if(this->confirmType == Qobuz_album_info){

        QString title = ProcJsonEasy::getString(jsonObj, "title");

        if(title == "Error"){
            this->lb_title_img->hide();
            this->lb_title->hide();
            this->lb_composer->hide();
            this->lb_released->hide();

            QString artist = QString("<html><head/><body><span style='font-size:50px; font-weight:300; line-height: 1.88; color:#FFFFFF;'>%1</span></body></html>").arg(title);
            this->lb_artist->setGeometry(319, 100, 112, 59);
            this->lb_artist->setText(artist);

            QString message = QString("<html><head/><body><span style='font-size:50px; font-weight:300; line-height: 1.88; color:#FFFFFF;'>%1</span></body></html>").arg(tr("This page is unvailable.\nPlease try again."));
            this->lb_content->setText(message);

        }
        else{

            this->lb_title->setText(title);

            QString image = ProcJsonEasy::getString(jsonObj, "image");
            if(image.isEmpty() == false){
                 this->fileDownLoader->setImageURL(QUrl(image));
            }
            else{
                this->paint_imageBig(*this->pixmap_albumImg_default);
            }

            QString artist = QString("<html><head/><body><span style='font-size:22px; font-weight:300; line-height: 1.88; color:#999999;'>%1</span></body></html>").arg(ProcJsonEasy::getString(jsonObj, "artist"));
            this->lb_artist->setText(artist);

            QString composer = QString("<html><head/><body>"
                                        "<span style='font-size:22px; font-weight:300; line-height: 1.88; color:#999999;'>Composer : </span>"
                                        "<span style='font-size:22px; font-weight:300; line-height: 1.88; color:#e6e6e6;'>%1</span></body></html>").arg(ProcJsonEasy::getString(jsonObj, "composer"));
            this->lb_composer->setText(composer);

            QString tmpDate = ProcJsonEasy::getString(jsonObj, "released");

            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = tmpDate;
            splitToken = tmp_split.split("-");

            if(splitToken.size() > 2){

                switch (splitToken.at(1).toInt()) {
                    case 1:
                        tmpDate = "January";
                        break;
                    case 2:
                        tmpDate = "February";
                        break;
                    case 3:
                        tmpDate = "March";
                        break;
                    case 4:
                        tmpDate = "April";
                        break;
                    case 5:
                        tmpDate = "May";
                        break;
                    case 6:
                        tmpDate = "June";
                        break;
                    case 7:
                        tmpDate = "July";
                        break;
                    case 8:
                        tmpDate = "August";
                        break;
                    case 9:
                        tmpDate = "September";
                        break;
                    case 10:
                        tmpDate = "October";
                        break;
                    case 11:
                        tmpDate = "November";
                        break;
                    case 12:
                        tmpDate = "December";
                        break;
                }

                tmpDate += " " + splitToken.at(2) + ", " + splitToken.at(0);
            }
            else{
                tmpDate = "";
            }

            QString released = "<html><head/><body><span style='font-size:23px; font-weight:300; line-height: 1.88; color:#999999;'>Released by </span>" +
                                QString("<span style='font-size:23px; font-weight:300; line-height: 1.88; color:#e6e6e6;'>%1</span>").arg(ProcJsonEasy::getString(jsonObj, "copyright")) +
                                QString("<span style='font-size:23px; font-weight:300; line-height: 1.88; color:#999999;'> on %2</span></body></html>").arg(tmpDate);
            this->lb_released->setText(released);

            int tmpduration = ProcJsonEasy::getInt(jsonObj, "duration");
            QString duration = "";
            if(tmpduration == 0){

            }
            else if(tmpduration >= 3600){
                QString tmp_split = "";
                QStringList splitToken;

                tmp_split = QDateTime::fromTime_t(tmpduration).toUTC().toString("hh:mm:ss");
                splitToken = tmp_split.split(":");

                duration = splitToken.at(0) + "h " + splitToken.at(1) + "m " + splitToken.at(2) + "s";
            }
            else{
                QString tmp_split = "";
                QStringList splitToken;

                tmp_split = QDateTime::fromTime_t(tmpduration).toUTC().toString("mm:ss");
                splitToken = tmp_split.split(":");

                duration = splitToken.at(0) + "m " + splitToken.at(1) + "s";
            }

            QString description = QString("<html><head/><body><br /><br /><span style='font-size:20px; font-weight:500; line-height: 1.88; color:#e6e6e6;'>%1 </span>").arg(ProcJsonEasy::getString(jsonObj, "genre")) +
                                QString("<span style='font-size:20px; font-weight:300; line-height: 1.88; color:#999999;'>· %1 Tracks </span>").arg(ProcJsonEasy::getInt(jsonObj, "track")) +
                                QString("<span style='font-size:20px; font-weight:300; line-height: 1.88; color:#999999;'>· %1</span><br /><br />").arg(duration) +
                                QString("<html><head/><body><span style='font-size:20px; font-weight:300; line-height: 1.88; color:#e6e6e6;'>%1</span><br /><br /></body></html>").arg(ProcJsonEasy::getString(jsonObj, "description"));
            this->lb_content->setText(description);
        }
    }
    else if(this->confirmType == Music_album_info){

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        this->lb_title->setText(title);

        QString image = ProcJsonEasy::getString(jsonObj, "image");
        if(image.isEmpty() == false){
             this->fileDownLoader->setImageURL(QUrl(image));
        }
        else{
            this->paint_imageBig(*this->pixmap_albumImg_default);
        }

        QString artist = ProcJsonEasy::getString(jsonObj, "artist");

        QLabel *tmpArtist = new QLabel();
        tmpArtist->setStyleSheet("font-size:22px; font-weight:300; line-height: 1.88; color:#999999;");
        tmpArtist->setFixedWidth(500);
        tmpArtist->setWordWrap(true);
        tmpArtist->setText(artist);

        int tmpHeight = tmpArtist->sizeHint().height();

        this->lb_artist->setStyleSheet("font-size:22px; font-weight:300; line-height: 1.88; color:#999999;");
        this->lb_artist->setWordWrap(true);
        this->lb_artist->setGeometry(190, 80, 500, tmpHeight);
        this->lb_artist->setText(artist);

        if(jsonObj.contains("composer")){
            QString composer = QString("<html><head/><body>"
                                        "<span style='font-size:22px; font-weight:300; line-height: 1.88; color:#999999;'>Composer : </span>"
                                        "<span style='font-size:22px; font-weight:300; line-height: 1.88; color:#e6e6e6;'>%1</span></body></html>").arg(ProcJsonEasy::getString(jsonObj, "composer"));
            this->lb_composer->setText(composer);
        }

        if(jsonObj.contains("released")){
            QString tmpDate = ProcJsonEasy::getString(jsonObj, "released");

            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = tmpDate;
            splitToken = tmp_split.split("-");

            if(splitToken.size() > 2){

                switch (splitToken.at(1).toInt()) {
                    case 1:
                        tmpDate = "January";
                        break;
                    case 2:
                        tmpDate = "February";
                        break;
                    case 3:
                        tmpDate = "March";
                        break;
                    case 4:
                        tmpDate = "April";
                        break;
                    case 5:
                        tmpDate = "May";
                        break;
                    case 6:
                        tmpDate = "June";
                        break;
                    case 7:
                        tmpDate = "July";
                        break;
                    case 8:
                        tmpDate = "August";
                        break;
                    case 9:
                        tmpDate = "September";
                        break;
                    case 10:
                        tmpDate = "October";
                        break;
                    case 11:
                        tmpDate = "November";
                        break;
                    case 12:
                        tmpDate = "December";
                        break;
                }

                tmpDate += " " + splitToken.at(2) + ", " + splitToken.at(0);
            }
            else{
                tmpDate = "";
            }

            QString released = "<html><head/><body><span style='font-size:23px; font-weight:300; line-height: 1.88; color:#999999;'>Released by </span>" +
                                QString("<span style='font-size:23px; font-weight:300; line-height: 1.88; color:#e6e6e6;'>%1</span>").arg(ProcJsonEasy::getString(jsonObj, "copyright")) +
                                QString("<span style='font-size:23px; font-weight:300; line-height: 1.88; color:#999999;'> on %2</span></body></html>").arg(tmpDate);
            this->lb_released->setText(released);
        }

        int tmpduration = ProcJsonEasy::getInt(jsonObj, "duration");
        QString duration = "";
        if(tmpduration == 0){

        }
        else if(tmpduration >= 3600){
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = QDateTime::fromTime_t(tmpduration).toUTC().toString("hh:mm:ss");
            splitToken = tmp_split.split(":");

            duration = splitToken.at(0) + "h " + splitToken.at(1) + "m " + splitToken.at(2) + "s";
        }
        else{
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = QDateTime::fromTime_t(tmpduration).toUTC().toString("mm:ss");
            splitToken = tmp_split.split(":");

            duration = splitToken.at(0) + "m " + splitToken.at(1) + "s";
        }

        QString description = QString("<span style='font-size:20px; font-weight:300; line-height: 1.88; color:#999999;'>%1 Tracks </span>").arg(ProcJsonEasy::getInt(jsonObj, "track")) +
                            QString("<span style='font-size:20px; font-weight:300; line-height: 1.88; color:#999999;'>· %1</span><br /><br />").arg(duration) +
                            QString("<html><head/><body><span style='font-size:20px; font-weight:300; line-height: 1.88; color:#e6e6e6;'>%1</span><br /><br /></body></html>").arg(ProcJsonEasy::getString(jsonObj, "description"));
        this->lb_content->setText(description);
    }
}


void DialogNotice::slot_fileDownload_loadImage(){

    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(140, 140));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(140, 140, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, 140, 140, 2, 2);

        painter.setClipPath(path);

        int leftValue = 0;
        int topValue = 0;

        if(tmp_pixmap.width() > 140){
            leftValue = (140 - tmp_pixmap.width()) / 2;
        }

        if(tmp_pixmap.height() > 140){
            topValue = (140 - tmp_pixmap.height()) / 2;
        }

        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_title_img->setPixmap(pixmapIMG);
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }

    this->fileDownLoader->deleteLater();
}


/**
 * @brief pixmap 을 label_imageBig 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
 * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
 * @param pixmap
 */
void DialogNotice::paint_imageBig(QPixmap &pixmap){

    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(140, 140));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, 140, 140, 2, 2);
    this->lb_title_img->setPixmap(pixmap_painter);
}
