#include "imagelabel.h"

#include <QGraphicsDropShadowEffect>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QVariant>
#include <QPixmapCache>



/**
 * @brief ImageLabel::ImageLabel
 * @param parent
 */
ImageLabel::ImageLabel(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();

    //this->fileDownLoader = new FileDownloader(this);
    //connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (loadImage()));
}

ImageLabel::~ImageLabel(){
    listHoverCode.clear();
    listHoverPath.clear();

    this->deleteLater();
}

/**
 * @brief ImageLabel::setInit : 초기 세팅
 */
void ImageLabel::setInit(){
    this->btn_play = new QPushButton();
    this->btn_favorite = new QPushButton();
    this->btn_more = new QPushButton();

    setFixedSize(imgWidth, imgHeight);

    listHoverPath.append(HOVER_FAV_OFF_IMG);
    listHoverPath.append(HOVER_PLAY_IMG);
    listHoverPath.append(HOVER_MORE_IMG);

    listHoverCode.append(HOVER_CODE_FAV);
    listHoverCode.append(HOVER_CODE_PLAY);
    listHoverCode.append(HOVER_CODE_MORE);

    list_hoverPushBtn.append(this->btn_favorite);
    list_hoverPushBtn.append(this->btn_play);
    list_hoverPushBtn.append(this->btn_more);
}

/**
 * @brief ImageLabel::setUIControl : UI 세팅
 */
void ImageLabel::setUIControl(){
    boxLayout = new QVBoxLayout;
    boxLayout->setSpacing(0);
    boxLayout->setMargin(0);
    boxLayout->setContentsMargins(0,0,0,0);

    labelIMG = new QLabel();
    labelIMG->setFixedSize(imgWidth, imgHeight);
    labelIMG->setScaledContents(true);
    labelIMG->setAlignment(Qt::AlignCenter);

    mime_type = new QLabel(labelIMG);
    mime_type->hide();

    boxLayout->addWidget(labelIMG);
    //boxLayout->addWidget(mime_type);

    /// play Time
    playTime = new QLabel(labelIMG);
    playTime->setStyleSheet("background-color:#99000000;color:#FFFFFF;font-size:18px;border-bottom-left-radius:3px;");
    //playTime->setText("3:10");
    playTime->setAlignment(Qt::AlignCenter);
    playTime->setFixedSize(80,30);
    playTime->hide();
    setLayout(boxLayout);

    // hover UI 세팅
    setHoverUIControl();
}

/**
 * @brief ImageLabel::setHoverUIControl : hover UI 세팅
 * @details 좋아요/재생/더보기 버튼
 */
void ImageLabel::setHoverUIControl(){

    QHBoxLayout *boxLayout = new QHBoxLayout;
    boxLayout->setSpacing(0);
    boxLayout->setContentsMargins(0,0,0,0);


    for(int i = 0; i < listHoverPath.size(); i++){

        if(listHoverPath.at(i).isEmpty()){
            boxLayout->addStretch(1);
        }
        else{
            QString tmp_imgPath = listHoverPath.at(i);
            QStringList tmp_img = tmp_imgPath.split(".");

            QString tmp_btnStyle = QString("#btn_"+QString::number(i)+" {background-image:url('%1');background-repeat:no-repeat;background-position:center;background-color:transparent;}").arg(listHoverPath.at(i));
            tmp_btnStyle += QString("#btn_"+QString::number(i)+":pressed{background-image:url('%1');background-repeat:no-repeat;background-position:center;background-color:transparent;}").arg(tmp_img.at(0)+"_h."+tmp_img.at(1));

            //QPixmap pixmap(pathList->at(i));
            QPushButton *btn = list_hoverPushBtn.at(i);
            btn->setObjectName(QString("btn_%1").arg(i));
            btn->setFixedSize(QSize(60,60));
            btn->setProperty("code", listHoverCode.at(i));
            btn->setStyleSheet(tmp_btnStyle);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, SIGNAL(clicked()), this, SLOT(clickedHoverItem()));
            boxLayout->addWidget(btn);
        }
    }

    /// hover widget
    hoverWidget = new QPushButton(labelIMG);
    hoverWidget->setContentsMargins(0,0,0,0);
    hoverWidget->setObjectName("hoverWidget");
    hoverWidget->setFixedSize(imgWidth, imgHeight);
    hoverWidget->setStyleSheet("#hoverWidget{ background-color:#B3000000; border-radius:3px; }");
    hoverWidget->setAutoFillBackground(true);
    hoverWidget->setVisible(false);
    hoverWidget->setAttribute(Qt::WA_Hover);
    hoverWidget->setMouseTracking(true);
    hoverWidget->setLayout(boxLayout);

    connect(hoverWidget, SIGNAL(clicked()), SLOT(slot_clickedItem()));
}

/**
 * @brief ImageLabel::setFavorStatus : Favor hover 아이콘 상태값 세팅
 * @param flagOn
 */
void ImageLabel::setFavorStatus(bool flagOn){

    if(type!=Type::Artist && type!=Type::PlayListFriend){
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

        if(hoverWidget->layout()->count() > 0){
            QPushButton *btn = qobject_cast<QPushButton*>(hoverWidget->layout()->itemAt(0)->widget());
            btn->setStyleSheet(tmp_btnStyle);
        }
    }
}

void ImageLabel::setHidden_play(bool flagHidden){
    btn_play->setHidden(flagHidden);
    btn_play->setVisible(!flagHidden);
}

void ImageLabel::setHidden_favorite(bool flagHidden){
    btn_favorite->setHidden(flagHidden);
    btn_favorite->setVisible(!flagHidden);
}

void ImageLabel::setHidden_more(bool flagHidden){
    btn_more->setHidden(flagHidden);
    btn_more->setVisible(!flagHidden);
}

/**
 * @brief ImageLabel::setImageUrl : 이미지 세팅
 * @param p_url
 * @note QPixmapCache 이용
 */
void ImageLabel::setImageUrl(QString p_url){
    /// 앨범이미지 download
    //QUrl imageUrl(p_url);

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (loadImage()));

    this->fileDownLoader->setImageURL(p_url);

    /* Pixmap 캐시 사용 주석
    QPixmap pixmap;
    if(!QPixmapCache::find(p_url, &pixmap)){
        fileDownLoader->setImageURL(p_url);
    }else{
        labelIMG->setPixmap(pixmap);
        // ==> playList 바 show 필요함.
    }

    imgPath = p_url;
    */

}

/**
 * @brief ImageLabel::setImageResource : rss 이미지 세팅
 * @param p_path
 */
void ImageLabel::setImageResource(QString p_path){

    QPixmap pixmap(p_path);
    pixmap.scaled(imgWidth, imgHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->labelIMG->setPixmap(pixmap);

    /*if(type==Video && playList_bar_220!=NULL){
        playList_bar_220->setVisible(true);
    }*/
}


/**
 * @brief ImageLabel::setImageDefault : Type에 따른 기본 이미지 세팅
 */
void ImageLabel::setImageDefault(){
    labelIMG->setPixmap(getDefualImgPath());
}

void ImageLabel::setMimeType(QString mime){

    int width_set = 0;

    QLabel *tmp_mime = new QLabel();
    tmp_mime->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:10px;");
    tmp_mime->setText(mime);

    width_set = tmp_mime->sizeHint().width() + 10;

    this->mime_type->hide();
    this->mime_type->setFixedSize(width_set, 20);
    this->mime_type->setGeometry(10, imgHeight - 30, 0, 0);

    this->mime_type->setStyleSheet("background-color:#75000000;font-size:11px;font-weight:500;color:#FFFFFF;Text-align:Center;border:2px solid #FFFFFF;border-radius:10px;");
    this->mime_type->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    this->mime_type->setText(mime);
    this->mime_type->show();
}


/**
 * @brief ImageLabel::setType   : 타입 세팅
 * @param p_type {@link ImageLabel::Type} 앨범, 아티스트, 플레이리스트
 */
void ImageLabel::setType(Type p_type){
    type = p_type;

    if(p_type==PlayListMy || p_type==PlayListFriend){
        playList_bar_180 = new QLabel();
        playList_bar_180->setAlignment(Qt::AlignCenter);

        QPixmap pixmap(":/images/playlist_bg.png");
        playList_bar_180->setPixmap(pixmap);

        boxLayout->insertWidget(0, playList_bar_180);

        setFixedSize(imgWidth, imgHeight + 20);
    }
    else if(p_type==playListMy_C || p_type==playListFriend_C){
        imgWidth = IMG_WIDTH_MUSIC_CATEGORY;
        imgHeight = IMG_WIDTH_MUSIC_CATEGORY;

        playList_bar_165 = new QLabel();
        playList_bar_165->setAlignment(Qt::AlignCenter);
        //playList_bar_165->setVisible(false);

        QPixmap pixmap(":/images/playlist_bg_s.png");
        playList_bar_165->setPixmap(pixmap);

        boxLayout->insertWidget(0, playList_bar_165);

        setFixedSize(imgWidth, imgHeight + 20);
    }
    else if(p_type==Artist){
        imgWidth = IMG_WIDTH_ARTIST;
        imgHeight = IMG_HEIGTH_ARTIST;

        hoverWidget->setStyleSheet(QString("#hoverWidget { background-color:#B3000000; border-radius:%1; }").arg(imgWidth/2));
        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==Video){

        /*playList_bar_220 = new QLabel();
        playList_bar_220->setAlignment(Qt::AlignCenter);

        QPixmap pixmap(":/images/playlist_video_s_bg.png");
        playList_bar_220->setPixmap(pixmap);

        imgWidth = IMG_WIDTH_VIDEO;
        imgHeight = IMG_HEIGHT_VIDEO;

        boxLayout->insertWidget(0, playList_bar_220);

        setFixedSize(imgWidth, imgHeight+20);*/

        imgWidth = IMG_WIDTH_VIDEO;
        imgHeight = IMG_HEIGHT_VIDEO;

        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==MusicCategory){
        imgWidth = IMG_WIDTH_MUSIC_CATEGORY;
        imgHeight = IMG_WIDTH_MUSIC_CATEGORY;

        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==Podcast){
        imgWidth = IMG_WIDTH_PODCAST;
        imgHeight = IMG_HEIGHT_PODCAST;
        setFixedSize(imgWidth,imgHeight);
    }
    else if(p_type==RoseTube){
        imgWidth = IMG_WIDTH_ROSETUBE;
        imgHeight = IMG_HEIGHT_ROSETUBE;

        setFixedSize(imgWidth, imgHeight);
        playTime->move(0,imgHeight-30);
    }
    else if(p_type==RoseTube_s){
        imgWidth = IMG_WIDTH_ROSETUBE_VIDEO;        //IMG_WIDTH_VIDEO;
        imgHeight = IMG_HEIGHT_ROSETUBE_VIDEO;      //IMG_HEIGHT_VIDEO;

        setFixedSize(imgWidth, imgHeight);
        playTime->move(0,imgHeight-30);
    }
    else if(p_type==RoseTubeList_s){
        playList_bar_220 = new QLabel();
        playList_bar_220->setAlignment(Qt::AlignCenter);

        QPixmap pixmap(":/images/playlist_video_s_bg.png");
        playList_bar_220->setPixmap(pixmap);

        imgWidth = IMG_WIDTH_ROSETUBE_VIDEO;        //IMG_WIDTH_VIDEO;
        imgHeight = IMG_HEIGHT_ROSETUBE_VIDEO;      //IMG_HEIGHT_VIDEO;

        boxLayout->insertWidget(0, playList_bar_220);

        setFixedSize(imgWidth, imgHeight+20);
    }
    else if(p_type==RoseTubeList){

        playList_bar_300 = new QLabel();
        playList_bar_300->setAlignment(Qt::AlignCenter);

        QPixmap pixmap(":/images/playlist_video_bg.png");
        playList_bar_300->setPixmap(pixmap);

        boxLayout->insertWidget(0, playList_bar_300);

        imgWidth = IMG_WIDTH_ROSETUBE;
        imgHeight = IMG_HEIGHT_ROSETUBE;

        setFixedSize(imgWidth, imgHeight+20);
    }
    else if(p_type==IconSmallRound){
        imgWidth = IMG_WIDTH_ICON_SMALL;
        imgHeight = IMG_HEIGTH_ICON_SMALL;

        hoverWidget->setStyleSheet(QString("#hoverWidget{background-color:#B3000000;border-radius:%1;}").arg(imgWidth/2));
        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==IconSmallRect){
        imgWidth = IMG_WIDTH_ICON_SMALL;
        imgHeight = IMG_HEIGTH_ICON_SMALL;
        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==Avatar){
        imgWidth = IMG_WIDTH_ARTIST;
        imgHeight = IMG_HEIGTH_ARTIST;

        hoverWidget->setStyleSheet(QString("#hoverWidget{background-color:#B3000000;border-radius:%1;}").arg(imgWidth/2));
        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==None300){
        imgWidth = 300;
        imgHeight = 300;

        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==MusicPlayListDetail){
        imgWidth = IMG_WIDTH_MUSIC_PLAYLISTDETAIL;
        imgHeight = IMG_HEIGTH_MUSIC_PLAYLISTDETAIL;

        setFixedSize(imgWidth, imgHeight);
    }
    else if(p_type==RoseTubeDetail){
        imgWidth = IMG_WIDTH_ROSETUBE_DETAIL;
        imgHeight = IMG_HEIGHT_ROSETUBE_DETAIL;

        setFixedSize(imgWidth, imgHeight);
    }

    labelIMG->setFixedSize(imgWidth, imgHeight);
    hoverWidget->setFixedSize(imgWidth, imgHeight);

    setImageResource(getDefualImgPath());
}

/**
 * @brief ImageLabel::setHover : Hover Widget 생성
 */
void ImageLabel::setHover(){
    flagUseHover = true;
}

void ImageLabel::setHover2(){
    flagUseHover2 = true;
}

void ImageLabel::setDuration(const QString &p_duration){
    playTime->setText(p_duration);
    playTime->show();
}

/**
 * @brief ImageLabel::getDefualImgPath : 타입에 따른 기본 썸네일 경로 반환
 * @return
 */
QString ImageLabel::getDefualImgPath() const
{
    QString defaultImgPath = ":/images/def_mus_200.png";
    if(type==Artist || type==Avatar){
        //defaultImgPath = ":images/def_mus_200_c.png";
        defaultImgPath = ":/images/qobuz/artist_def_200.png";
    }else if(type==MusicPlayListDetail){
        defaultImgPath = ":/images/def_mus_300.png";
    }else if(type==Video){
        defaultImgPath = ":/images/def_video_200.png";
    }else if(type==RoseTube_s || type==RoseTubeList_s || type==RoseTubeDetail){
        defaultImgPath = ":/images/no_image_video.png";
    }else if(type==RoseTube || type==RoseTubeList){
        defaultImgPath = ":/images/def_tube_340.png";
    }else if(type==IconSmallRound){
        defaultImgPath = ":/images/def_mus_60_c.png";
    }else if(type==IconSmallRect){
        defaultImgPath = ":/images/def_mus_60.png";
    }

    return defaultImgPath;
}


/**
 * @brief ImageLabel::loadImage : 다운로드 받은 이미지 세팅 함수
 * @details {@link ImageLabel::Type::Artist} 인 경우에만 원형 이미지로보인다.
 * 플레이리스트 상단바 이미지는 생성시 visible=false이며,  여기서 visible=true
 */
void ImageLabel::loadImage(){

    int cornerPx = 3;
    if(type==Artist || type==IconSmallRound || type==Avatar){
        cornerPx = imgWidth/2;              // 원형 스타일
    }

    /// freezing 이유로 QImage 사용
    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG(QSize(imgWidth,imgHeight));

        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(imgWidth, imgHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, imgWidth, imgHeight, cornerPx, cornerPx);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, tmp_pixmap);

        painter.end();
        labelIMG->setPixmap(pixmapIMG);

        // cache 사용 주석
        //QPixmapCache::insert(imgPath, pixmapIMG);

        // path 적용전 pixmap
        emit setPixmapCompleted(tmp_pixmap);
    }else{

        QPixmap pixmap(getDefualImgPath());
        pixmap.scaled(imgWidth, imgHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->labelIMG->setPixmap(pixmap);
    }

    this->fileDownLoader->deleteLater();
}

/**
 * @brief ImageLabel::clickedHoverItem : Hover 아이템 클릭 슬롯
 * @details FAV 에 한하여 이미지 아이콘 교체
 */
void ImageLabel::clickedHoverItem(){

    QString tmp_code = sender()->property("code").toString();

    if(tmp_code == HOVER_CODE_FAV){

        setFavorStatus(!flagFavorOn);

    }

    emit clickedHoverItem(tmp_code);
}

/**
 * @brief ImageLabel::slot_clickedItem:[슬롯] 아이템 클릭 슬롯
 * @note hover item(하트/재생/더보기 외 영역) 클릭
 */
void ImageLabel::slot_clickedItem(){
    emit clickedItem();
}

/**
 * @brief ImageLabel::enterEvent : 마우스 Enter 이벤트 슬롯
 * @param event QEvent
 */
void ImageLabel::enterEvent(QEvent *event){
    Q_UNUSED(event);

    if(flagUseHover){
        setHidden_favorite(true);

        hoverWidget->setVisible(true);
        hoverWidget->setCursor(QCursor(Qt::PointingHandCursor));
    }
    else if(flagUseHover2){
        setHidden_play(false);
        setHidden_favorite(true);
        setHidden_more(true);

        hoverWidget->setVisible(true);
        hoverWidget->setCursor(QCursor(Qt::PointingHandCursor));
    }
}


/**
 * @brief ImageLabel::leaveEvent : 마우스 Leave 이벤트 슬롯
 * @param event QEvent
 */
void ImageLabel::leaveEvent(QEvent *event){
    Q_UNUSED(event);

    if(flagUseHover){
        hoverWidget->setVisible(false);
        hoverWidget->setCursor(QCursor(Qt::ArrowCursor));
    }
    else if(flagUseHover2){
        hoverWidget->setVisible(false);
        hoverWidget->setCursor(QCursor(Qt::ArrowCursor));
    }
}

