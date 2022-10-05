#include "imagelabelradio.h"

#include <QGraphicsDropShadowEffect>
#include <QVariant>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QVariant>


const QString HOVER_FAV_ON_IMG = ":/images/album_fav_icon_on.png";
const QString HOVER_FAV_OFF_IMG = ":/images/album_fav_icon_off.png";
const QString HOVER_PLAY_IMG = ":/images/album_play_icon.png";
const QString HOVER_DEL_IMG = ":/images/fol_del_ico.png";


/**
 * @brief ImageLabelRadio::ImageLabelRadio : 라디오 이미지 라벨
 * @details 라디오 배경 이미지 + (라디오 썸네일 이미지 or 라디오명)
 * Hover 기능 지원(즐겨찾기 설정,해제 / 바로 듣기)
 * @param parent
 */
ImageLabelRadio::ImageLabelRadio(QWidget *parent, Mode mode)
    : QWidget(parent)
    , mode(mode)
{
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (loadImage()));

    setInit();
    setUIControl();
}


ImageLabelRadio::~ImageLabelRadio(){

    listHoverCode.clear();
    listHoverPath.clear();

    this->deleteLater();
}


/**
 * @brief ImageLabelRadio::setInit : 초기 세팅
 */
void ImageLabelRadio::setInit(){

    setFixedSize(IMG_WIDTH, IMG_HEIGHT);

    if(mode == Mode::Eidt){
        listHoverPath.append("");
        listHoverPath.append(HOVER_DEL_IMG);
        listHoverPath.append("");

        listHoverCode.append("");
        listHoverCode.append(HOVER_CODE_DEL);
        listHoverCode.append("");
    }else if(mode == Mode::Normal){
        listHoverPath.append(HOVER_FAV_ON_IMG);
        listHoverPath.append(HOVER_PLAY_IMG);
        listHoverPath.append("");

        listHoverCode.append(HOVER_CODE_FAV);
        listHoverCode.append(HOVER_CODE_PLAY);
        listHoverCode.append("");
    }else if(mode == Mode::User){
        listHoverPath.append("");
        listHoverPath.append(HOVER_PLAY_IMG);
        listHoverPath.append("");

        listHoverCode.append("");
        listHoverCode.append(HOVER_CODE_PLAY);
        listHoverCode.append("");
    }
}


/**
 * @brief ImageLabelRadio::setUIControl : UI 세팅
 */
void ImageLabelRadio::setUIControl(){

    boxLayout = new QVBoxLayout;
    boxLayout->setSpacing(0);
    boxLayout->setContentsMargins(0, 0, 0, 0);

    lb_img = new QLabel;
    lb_img->setFixedSize(IMG_WIDTH, IMG_HEIGHT);
    boxLayout->addWidget(lb_img);

    lb_imgTitle = new QLabel(lb_img);
    lb_imgTitle->setFixedSize(IMG_WIDTH, IMG_HEIGHT);
    lb_imgTitle->setStyleSheet("background-color:transparent;");
    lb_imgTitle->hide();

    lb_title = new QLabel(lb_img);
    lb_title->setFixedSize(IMG_WIDTH, IMG_HEIGHT);
    lb_title->setStyleSheet("font-size:26px;background-color:transparent;color:#FFFFFF;");
    lb_title->setWordWrap(true);
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setContentsMargins(20, 20, 20, 20);
    lb_title->hide();

    setLayout(boxLayout);

    // Hover UI 세팅
    setHoverUIControl();
}

void ImageLabelRadio::setHoverUIControl(){

    QHBoxLayout *boxLayout = new QHBoxLayout;
    boxLayout->setSpacing(0);

    for(int i = 0; i < listHoverPath.size(); i++){
        QString tmp_imgPath = listHoverPath.at(i);
        QStringList tmp_img = tmp_imgPath.split(".");

        if(tmp_imgPath.isEmpty()){
            boxLayout->addSpacing(60);
        }else{
            QString tmp_btnStyle;

            if(mode==Mode::Eidt){
                tmp_btnStyle = "#btn_"+QString::number(i)+QString(" { background-image:url('%1');background-repeat:no-repeat;background-position:center;background-color:transparent;} ").arg(tmp_imgPath);
                tmp_btnStyle += "#btn_"+QString::number(i)+QString(":hover{background-image:url('%1');background-repeat:no-repeat;background-position:center;background-color:#B18658;border-radius:30px;}").arg(tmp_imgPath);
            }else{
                tmp_btnStyle = QString("#btn_"+QString::number(i)+" {background-image:url('%1');background-color:transparent;}").arg(tmp_imgPath);
                tmp_btnStyle += QString("#btn_"+QString::number(i)+":pressed{background-image:url('%1');background-color:transparent;}").arg(tmp_img.at(0)+"_h."+tmp_img.at(1));
            }

            //QPixmap pixmap(pathList->at(i));
            QPushButton *btn = new QPushButton;
            btn->setObjectName(QString("btn_%1").arg(i));
            btn->setFixedSize(60,60);
            btn->setStyleSheet(tmp_btnStyle);
            btn->setProperty("code", listHoverCode.at(i));
            connect(btn, SIGNAL(clicked()), this, SLOT(clickedHoverItem()));
            boxLayout->addWidget(btn);
        }
    }

    // hover widget
    wg_hover = new QWidget(lb_img);
    wg_hover->setObjectName("hoverWidget");
    wg_hover->setFixedSize(IMG_WIDTH, IMG_WIDTH);
    wg_hover->setStyleSheet("#hoverWidget{background-color:#B3000000;border-radius:3px;}");
    wg_hover->setAutoFillBackground(true);
    wg_hover->setVisible(false);
    wg_hover->setAttribute(Qt::WA_Hover);
    wg_hover->setMouseTracking(true);
    wg_hover->setLayout(boxLayout);
}


/**
 * @brief ImageLabelRadio::setFavorStatus : Favor hover 아이콘 상태값 세팅
 * @details Mode 값이 normal 인 경우에만 실행
 * flagModMode인 경우에는 Favor 미사용, 수정/삭제/이동만 있음.
 * @param flagOn
 */
void ImageLabelRadio::setFavorStatus(bool flagOn){

    if(mode==Mode::Normal){
        flagFavorOn = flagOn;

        QString tmp_imgPath;

        if(flagOn){
            tmp_imgPath = HOVER_FAV_ON_IMG;
        }else{
            tmp_imgPath = HOVER_FAV_OFF_IMG;
        }

        QStringList tmp_img = tmp_imgPath.split(".");

        QString tmp_btnStyle = QString("#btn_0 {background-image:url('%1');background-repeat:no-repeat;background-position:center;background-color:transparent;}").arg(tmp_imgPath);
        tmp_btnStyle += QString("#btn_0:pressed{background-image:url('%1');background-repeat:no-repeat;background-position:center;background-color:transparent;}").arg(tmp_img.at(0)+"_h."+tmp_img.at(1));

        if(wg_hover->layout()->count() > 0){
            QPushButton *btn = qobject_cast<QPushButton*>(wg_hover->layout()->itemAt(0)->widget());
            btn->setStyleSheet(tmp_btnStyle);
        }
    }
}


/**
 * @brief ImageLabelRadio::setBackgroundImg : 백그라운드 이미지 num 세팅
 * @details num 값에 따라 백그라운드 이미지 세팅
 * @param index
 */
void ImageLabelRadio::setBackgroundImg(const int &p_imgNumber){

    QPixmap pixmap(QString(":/images/radio_thumd%1.png").arg(p_imgNumber));
    lb_img->setPixmap(pixmap.scaled(IMG_WIDTH, IMG_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void ImageLabelRadio::setImageTitle(const QString &p_url){

    lb_imgTitle->show();

    /// 앨범이미지 download
    QUrl imageUrl(p_url);
    this->fileDownLoader->setImageURL(p_url);
}


void ImageLabelRadio::setTitle(const QString &p_title){

    if(p_title.contains("CPBC  카톨릭평화방송")){
        lb_title->setText("CPBC 카톨릭 평화방송");
        lb_title->show();
    }
    else{
        lb_title->setText(p_title);
        lb_title->show();
    }
}


/**
 * @brief ImageLabel::setHover : Hover Widget 생성
 */
void ImageLabelRadio::setHover(){

    flagUseHover = true;
}


/**
 * @brief ImageLabel::enterEvent : 마우스 Enter 이벤트 슬롯
 * @param event QEvent
 */
void ImageLabelRadio::enterEvent(QEvent *event){

    Q_UNUSED(event);        // by sunnyfish
    if(flagUseHover){
        wg_hover->setVisible(true);
        wg_hover->setCursor(QCursor(Qt::PointingHandCursor));
    }
}


/**
 * @brief ImageLabel::leaveEvent : 마우스 Leave 이벤트 슬롯
 * @param event QEvent
 */
void ImageLabelRadio::leaveEvent(QEvent *event){

    Q_UNUSED(event);        // by sunnyfish
    if(flagUseHover){
        wg_hover->setVisible(false);
        wg_hover->setCursor(QCursor(Qt::ArrowCursor));
    }

}


/**
 * @brief ImageLabelRadio::clickedHoverItem : Hover 아이템 클릭 슬롯
 */
void ImageLabelRadio::clickedHoverItem(){

    QString tmp_code = sender()->property("code").toString();

    if(tmp_code == HOVER_CODE_FAV){
        setFavorStatus(!flagFavorOn);
    }

    emit clickedHoverItem(tmp_code);
}


/**
 * @brief ImageLabelRadio::loadImage : 다운로드 받은 이미지 세팅 함수
 * @details {@link ImageLabel::Type::Artist} 인 경우에만 원형 이미지로보인다.
 * 플레이리스트 상단바 이미지는 생성시 visible=false이며,  여기서 visible=true
 */
void ImageLabelRadio::loadImage(){

    int cornerPx = 3;

    QPixmap pixmapIMG = QPixmap(QSize(IMG_WIDTH,IMG_WIDTH));
    pixmapIMG.fill(Qt::transparent);

    /// freezing 이유로 QImage 사용
    QImage image;
    image.loadFromData(this->fileDownLoader->downloadedData());

    QPixmap tmp_pixmap;
    tmp_pixmap = tmp_pixmap.fromImage(image);
    tmp_pixmap = tmp_pixmap.scaled(IMG_WIDTH, IMG_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter (&pixmapIMG);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path = QPainterPath();
    path.addRoundedRect(0, 0, IMG_WIDTH, IMG_HEIGHT, cornerPx, cornerPx);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, tmp_pixmap);
    painter.end();

    lb_imgTitle->setPixmap(pixmapIMG);

    this->fileDownLoader->deleteLater();
}
