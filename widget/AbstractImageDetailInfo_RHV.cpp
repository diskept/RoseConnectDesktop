#include "AbstractImageDetailInfo_RHV.h"
#include "widget/VerticalScrollArea.h"
#include <common/gscommon.h>

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScroller>
#include <QJsonArray>
#include <QJsonObject>

const int DEFAULT_IMAGE_SIZE = 400;
const int DEFAULT_IMAGE_SIZEW = 384;
const int DEFAULT_IMAGE_SIZEH = 186;
const int BTN_MENU_SIZE = 50;           ///< MyCollect, Fav, More 아이콘 버튼의 사이즈 (정사각형 고정 사이즈)

const QString IMAGE_PATH_DEFAULT_MUSIC = ":/images/def_mus_550.png";
const QString IMAGE_PATH_DEFAULT_TIDAL = ":/images/tidal/tidal_def_400.png";
const QString IMAGE_PATH_DEFAULT_BUGS = ":/images/bugs/bugs_def_430x2.png";
const QString IMAGE_PATH_DEFAULT_QOBUZ = ":/images/qobuz/qobuz_default_400.png";

const QString IMAGE_PATH_DEFAULT_MUSIC_PALYLIST = ":/images/def_mus_550.png";
const QString IMAGE_PATH_DEFAULT_TIDAL_PALYLIST = ":/images/tidal/tidal_def_video.png";
const QString IMAGE_PATH_DEFAULT_BUGS_PALYLIST = ":/images/bugs/bugs_def_284.png";
const QString IMAGE_PATH_DEFAULT_QOBUZ_PALYLIST = ":/images/qobuz/qobuz_default_384.png";

const QString IMAGE_PATH_DEFAULT_TIDAL_MQA = ":/images/tidal/mqa_icox2.png";
const QString IMAGE_PATH_DEFAULT_QOBUZ_HIRES = ":/images/qobuz/hires_a_ico.png";

const QString ICON_PATH___addCollect = ":/images/play_list_add_icon.png";
const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";       //":/images/list_fav_icon.png";
const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";       //":/images/list_fav_icon_on.png";
const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";
const QString ICON_PATH___thumbup_off = ":/images/rosehome/playlist_like_ico.png";
const QString ICON_PATH___thumbup_on = ":/images/rosehome/playlist like ico_on.png";
const QString ICON_PATH___info = ":/images/icon_info.png";
const QString ICON_PATH___more = ":/images/icon_menu.png";

const QString ICON_PATH___playAll = ":/images/icon_list_play.png";
const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";


/**
 * @brief 생성자
 * @param parent
 */
AbstractImageDetailInfo_RHV::AbstractImageDetailInfo_RHV(ContentsUIType p_ContentsUIType, QWidget *parent) : QWidget(parent){

    this->btnStyle = "QPushButton{background:#B18658; border:1px solid #B18658; border-radius:4px;}";
    this->btnStyle += "QPushButton:hover{background:#7D6144; border:1px solid #7D6144; border-radius:4px;}";

    this->curr_contentsType = p_ContentsUIType;

    switch(this->curr_contentsType){
        case Music_album:
        case Music_playlist:
        case Music_userplaylist:
            this->image_width = DEFAULT_IMAGE_SIZE;
            this->image_height = DEFAULT_IMAGE_SIZE;
            this->image_cornerRadius = 4;

            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;
        case Tidal_album:
            break;
        case Tidal_playlist:
            break;
        case Tidal_video:
            break;
        case Bugs_album:
            break;
        case Bugs_playlist:
            break;
        case Bugs_video:
            break;
        case Qobuz_album:
            this->image_width = DEFAULT_IMAGE_SIZE;
            this->image_height = DEFAULT_IMAGE_SIZE;
            this->image_cornerRadius = 4;

            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;
        case Qobuz_playlist:
            this->image_width = DEFAULT_IMAGE_SIZEW;
            this->image_height = DEFAULT_IMAGE_SIZEH;
            this->image_cornerRadius = 4;

            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;
    }
}


/**
 * @brief 소멸자. 메모리 해제 체크위해
 */
AbstractImageDetailInfo_RHV::~AbstractImageDetailInfo_RHV(){

    this->deleteLater();
}


/**
 * @brief 기본 UI 구성
 */
void AbstractImageDetailInfo_RHV::setUIControl_basic(){
    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractImageDetailInfo_RHV::slot_fileDownload_loadImage);

    QString img_path = "";
    QString icon_path = "";
    switch(this->curr_contentsType){
        case Music_album:
        case Music_userplaylist:
            img_path = IMAGE_PATH_DEFAULT_MUSIC;
            break;
        case Music_playlist:
            img_path = IMAGE_PATH_DEFAULT_MUSIC_PALYLIST;
            break;
        case Tidal_album:
            img_path = IMAGE_PATH_DEFAULT_TIDAL;
            icon_path = IMAGE_PATH_DEFAULT_TIDAL_MQA;
            break;
        case Tidal_playlist:
            img_path = IMAGE_PATH_DEFAULT_TIDAL_PALYLIST;
            break;
        case Tidal_video:
            img_path = IMAGE_PATH_DEFAULT_TIDAL_PALYLIST;
            break;
        case Bugs_album:
            img_path = IMAGE_PATH_DEFAULT_BUGS;
            break;
        case Bugs_playlist:
            img_path = IMAGE_PATH_DEFAULT_BUGS_PALYLIST;
            break;
        case Bugs_video:
            img_path = IMAGE_PATH_DEFAULT_BUGS_PALYLIST;
            break;
        case Qobuz_album:
            img_path = IMAGE_PATH_DEFAULT_QOBUZ;
            icon_path = IMAGE_PATH_DEFAULT_QOBUZ_HIRES;
            break;
        case Qobuz_playlist:
            img_path = IMAGE_PATH_DEFAULT_QOBUZ_PALYLIST;
            break;
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(img_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(this->image_width, this->image_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->widget_info_main = new QWidget();
    this->widget_info_main->setFixedSize(1500, 405);
    this->widget_info_main->setContentsMargins(0, 0, 0, 0);

    // Big Image (Album or Playlist Main Image)
    this->label_imageBig = new QLabel(this->widget_info_main);
    this->label_imageBig->setFixedSize(this->image_width, this->image_height);
    if(this->curr_contentsType == Qobuz_playlist){
        this->label_imageBig->setGeometry(8, 107, 0, 0);
    }
    else{
        this->label_imageBig->setGeometry(0, 0, 0, 0);
    }

    // Title, Description, CreatorName
    this->label_titleUp = new QLabel(this->widget_info_main);
    this->label_titleUp->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_titleUp->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:50px;font-weight:500;");
    this->label_titleUp->setWordWrap(true);
    this->label_titleUp->setGeometry(450, 25, 990, 67);

    this->label_titleDown = new QLabel(this->widget_info_main);
    this->label_titleDown->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_titleDown->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:50px;font-weight:500;");
    this->label_titleDown->setWordWrap(true);
    this->label_titleDown->setGeometry(450, 92, 990, 67);

    this->label_imageHiRes = new QLabel(this->widget_info_main);

    if(!icon_path.isEmpty()){
        if(this->curr_contentsType == Tidal_album){
            this->label_imageHiRes->setFixedSize(142, 50);
            this->label_imageHiRes->setGeometry(450, 100, 0, 0);

            QPixmap pixmap(IMAGE_PATH_DEFAULT_QOBUZ_HIRES);
            pixmap = pixmap.scaled(142, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            QPixmap pixmap_painter = QPixmap(QSize(142, 50));
            pixmap_painter.fill(Qt::transparent);

            QPainter painter (&pixmap_painter);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QBrush brush = QBrush(pixmap);
            painter.setBrush(brush);
            painter.drawRoundedRect(0, 0, 142, 50, 2, Qt::RelativeSize);

            this->label_imageHiRes->setPixmap(pixmap_painter);
        }
        else if(this->curr_contentsType == Qobuz_album){
            this->label_imageHiRes->setFixedSize(50, 50);
            this->label_imageHiRes->setGeometry(1170, 35, 0, 0);

            QPixmap pixmap(IMAGE_PATH_DEFAULT_QOBUZ_HIRES);
            pixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            QPixmap pixmap_painter = QPixmap(QSize(50, 50));
            pixmap_painter.fill(Qt::transparent);

            QPainter painter (&pixmap_painter);
            painter.setRenderHint(QPainter::Antialiasing, true);
            QBrush brush = QBrush(pixmap);
            painter.setBrush(brush);
            painter.drawRoundedRect(0, 0, 50, 50, 2, Qt::RelativeSize);

            this->label_imageHiRes->setPixmap(pixmap_painter);
        }
    }
    this->label_imageHiRes->hide();

    this->label_creatorName = new QLabel(this->widget_info_main);
    this->label_creatorName->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_creatorName->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;");
    this->label_creatorName->setGeometry(450, 175, 700, 37);

    this->label_artist = new QLabel(this->widget_info_main);
    this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_artist->setStyleSheet("background-color:transparent;color:#E6E6E6;font-size:25px;font-weight:300;");
    this->label_artist->setGeometry(450, 175, 700, 37);

    this->label_resolution = new QLabel(this->widget_info_main);
    this->label_resolution->setStyleSheet("background-color:transparent;color:#777777;font-size:16px;font-weight:300;");
    this->label_resolution->setGeometry(450, 215, 700, 24);

    this->widget_Addbtn_Icon = new QWidget(this->widget_info_main);
    this->widget_Addbtn_Icon->setFixedSize(700, 50);
    this->widget_Addbtn_Icon->setGeometry(440, 245, 0, 0);

    this->widget_Addbtn_Play = new QWidget(this->widget_info_main);
    this->widget_Addbtn_Play->setFixedSize(700, 50);
    this->widget_Addbtn_Play->setGeometry(440, 350, 0, 0);

    this->widget_Addbtn_PlayCannot = new QWidget(this->widget_info_main);
    this->widget_Addbtn_PlayCannot->setFixedSize(700, 50);
    this->widget_Addbtn_PlayCannot->setGeometry(440, 350, 0, 0);


    //=====================================================================================================//
    this->widget_Addbtn_Open = new QWidget();
    this->widget_Addbtn_Open->setFixedSize(1500, 80);
    this->widget_Addbtn_Open->hide();

    this->label_description = new QLabel(this->widget_Addbtn_Open);
    this->label_description->setTextFormat(Qt::RichText);
    this->label_description->setWordWrap(true);
    this->label_description->setGeometry(0, 0, 1450, 40);

    this->label_open = new QLabel(this->widget_Addbtn_Open);
    this->label_open->setStyleSheet("background-color:transparent; border:0px;");
    this->label_open->setGeometry(0, 37, 1450, 30);

    this->btn_open = GSCommon::getUIBtnImg("btn_open", ":/images/text_open_ico.png", this->label_open);
    this->btn_open->setCursor(Qt::PointingHandCursor);
    this->btn_open->setGeometry(735, 0, 30, 30);
    //this->btn_open->setStyleSheet("background-color:transparent;");

    connect(this->btn_open, &QPushButton::clicked, this, &AbstractImageDetailInfo_RHV::slot_btnopen_clicked);


    //=====================================================================================================//
    this->vl_track_main = new QVBoxLayout();
    this->vl_track_main->setContentsMargins(0, 0, 0, 0);
    this->vl_track_main->setAlignment(Qt::AlignTop);


    //=====================================================================================================//
    this->vl_same_artist = new QVBoxLayout();
    this->vl_same_artist->setContentsMargins(0, 0, 0, 0);
    this->vl_same_artist->setAlignment(Qt::AlignTop);


    this->info_hbox = new QVBoxLayout();
    this->info_hbox->setContentsMargins(0, 20, 0, 0);
    this->info_hbox->addWidget(this->widget_info_main);
    this->info_hbox->addSpacing(70);
    this->info_hbox->addWidget(this->widget_Addbtn_Open);
    this->info_hbox->addSpacing(10);

    if(this->curr_contentsType != Qobuz_playlist && this->curr_contentsType != Music_playlist && this->curr_contentsType != Music_userplaylist && this->curr_contentsType != Music_album){
        this->info_hbox->addLayout(this->vl_track_main);
        this->info_hbox->addSpacing(70);
        this->info_hbox->addLayout(this->vl_same_artist);
    }

    this->setLayout(info_hbox);
}

/**
 * @brief 버튼 UI 설정 - My Collection 추가버튼, 좋아요 하트 버튼, 더보기 메뉴 버튼
 */
void AbstractImageDetailInfo_RHV::setUIControl_btnAddFavMore(){

    // 버튼 객체 생성
    this->btn_addCollect = GSCommon::getUIBtnImg("btn_addCollect", ICON_PATH___addCollect);    
    this->btn_fav_toAdd = GSCommon::getUIBtnImg("btn_fav_toAdd", ICON_PATH___favorite_off);
    this->btn_fav_toAddx2 = GSCommon::getUIBtnImg("btn_fav_toAddx2", ICON_PATH___favorite_on);
    this->btn_fav_toAddx3 = GSCommon::getUIBtnImg("btn_fav_toAddx3", ICON_PATH___favorite_on2);
    this->btn_fav_toDelete = GSCommon::getUIBtnImg("btn_fav_toDelete", ICON_PATH___favorite_on3);
    this->btn_thumb_toAdd = GSCommon::getUIBtnImg("btn_thumb_toAdd", ICON_PATH___thumbup_off);
    this->btn_thumb_toDelete = GSCommon::getUIBtnImg("btn_thumb_toDelete", ICON_PATH___thumbup_on);
    this->btn_info = GSCommon::getUIBtnImg("btn_info", ICON_PATH___info);
    this->btn_more = GSCommon::getUIBtnImg("btn_more", ICON_PATH___more);

    QHBoxLayout *hBox_btns = new QHBoxLayout();
    hBox_btns->setContentsMargins(0,0,0,0);            // 우측 여백만 설정
    hBox_btns->setSpacing(0);
    hBox_btns->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QList<QPushButton*> list_btn;
    QList<BtnClickMode> list_mode;
    if(this->curr_contentsType == Qobuz_album){
        list_btn = {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_info, this->btn_more};
        list_mode = {AddCollection, Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Info, More};
    }
    else if(this->curr_contentsType == Music_playlist || this->curr_contentsType == Music_album){
        list_btn = {this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_more};
        list_mode = {Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, More};
    }
    else if(this->curr_contentsType == Music_userplaylist){
        list_btn = {this->btn_thumb_toAdd, this->btn_thumb_toDelete, this->btn_more};
        list_mode = {Thumbup_toAdd, Thumbup_toDelete, More};
    }
    else{
        list_btn = {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_more};
        list_mode = {AddCollection, Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, More};
    }

    for(int i = 0; i < list_btn.length(); i++){
        list_btn.at(i)->setFixedSize(BTN_MENU_SIZE, BTN_MENU_SIZE);
        list_btn.at(i)->setCursor(Qt::PointingHandCursor);
        list_btn.at(i)->setProperty("mode", list_mode.at(i));
        hBox_btns->addWidget(list_btn.at(i));

        // connect
        connect(list_btn.at(i), &QPushButton::clicked, this, &AbstractImageDetailInfo_RHV::slot_btnClicked_mode);
    }

    this->widget_Addbtn_Icon->setLayout(hBox_btns);
}


/**
 * @brief 버튼 UI 설정 - 전체재생, 셔플재생
 */
void AbstractImageDetailInfo_RHV::setUIControl_btnPlays(){

    // Play All
    QPushButton *btn_play = new QPushButton(this->widget_Addbtn_Play);
    btn_play->setFixedSize(150, 50);
    btn_play->setGeometry(0, 0, 0, 0);
    btn_play->setStyleSheet(this->btnStyle);
    btn_play->setCursor(Qt::PointingHandCursor);
    btn_play->setProperty("mode", BtnClickMode::PlayAll);
    connect(btn_play, &QPushButton::clicked, this, &AbstractImageDetailInfo_RHV::slot_btnClicked_mode);

    QLabel *label_play_img = GSCommon::getUILabelImg(ICON_PATH___playAll, btn_play);
    label_play_img->setFixedSize(30, 30);
    label_play_img->setGeometry(32, 10, 0, 0);
    label_play_img->setStyleSheet("background-color:transparent;");

    QLabel *label_play = new QLabel(btn_play);
    label_play->setFixedSize(41, 29);
    label_play->setGeometry(67, 11, 0, 0);
    label_play->setStyleSheet("background-color:transparent;font-size:20px;font-weight:500;color:#FFFFFF;");
    label_play->setText("Play");


    // Shuffle Play
    QPushButton *btn_shuffle = new QPushButton(this->widget_Addbtn_Play);
    btn_shuffle->setFixedSize(150, 50);
    btn_shuffle->setGeometry(170, 0, 0, 0);
    btn_shuffle->setStyleSheet(this->btnStyle);
    btn_shuffle->setCursor(Qt::PointingHandCursor);
    btn_shuffle->setProperty("mode", BtnClickMode::PlayShuffle);
    connect(btn_shuffle, &QPushButton::clicked, this, &AbstractImageDetailInfo_RHV::slot_btnClicked_mode);

    QLabel *label_shuffle_img = GSCommon::getUILabelImg(ICON_PATH___playShuffle, btn_shuffle);
    label_shuffle_img->setFixedSize(30, 30);
    label_shuffle_img->setGeometry(23, 10, 0, 0);
    label_shuffle_img->setStyleSheet("background-color:transparent;");

    QLabel *label_shuffle = new QLabel(btn_shuffle);
    label_shuffle->setFixedSize(68, 29);
    label_shuffle->setGeometry(58, 11, 0, 0);
    label_shuffle->setStyleSheet("background-color:transparent;font-size:20px;font-weight:500;color:#FFFFFF;");
    label_shuffle->setText("Shuffle");
}


/**
 * @brief 재생불가 View
 */
void AbstractImageDetailInfo_RHV::setUIControl_btnPlays_cannot(){

    QLabel *tmp_label_lock = GSCommon::getUILabelImg(":/images/bugs/bugs-lock.png");
    tmp_label_lock->setFixedSize(30,30);

    QLabel *tmp_label_text = new QLabel();
    tmp_label_text->setText(tr("This album cannot be played at the request of the rights holder."));
    //tmp_label_text->setText(tr("이 앨범은 권리사의 요청으로 재생할 수 없습니다."));
    tmp_label_text->setStyleSheet("color:#e6e6e6; font-size:16px;");

    QHBoxLayout *hBox_btnPlays_cannot = new QHBoxLayout(this->widget_Addbtn_PlayCannot);
    hBox_btnPlays_cannot->setAlignment(Qt::AlignLeft);
    hBox_btnPlays_cannot->setSpacing(0);
    hBox_btnPlays_cannot->setContentsMargins(0,0,0,0);

    hBox_btnPlays_cannot->addWidget(tmp_label_lock);
    hBox_btnPlays_cannot->addWidget(tmp_label_text, 1, Qt::AlignLeft);
}


/**
 * @brief 우측에 Big 이미지에 imagePath 보이도록 한다.
 * @param imagePath
 */
void AbstractImageDetailInfo_RHV::setImage(QString imagePath){
    // 이미지 경로 설정
    if(imagePath.isEmpty() == false){
         this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }
}


/**
 * @brief 이미지 사이즈와 corner radius 정보를 세팅한다. child class 의 생성자에서 호출해주도록 한다.
 * @param width
 * @param height
 * @param cornerRadius
 */
void AbstractImageDetailInfo_RHV::setImageSizeAndCorner(int width, int height, int cornerRadius){

    this->image_width = width;
    this->image_height = height;
    this->image_cornerRadius = cornerRadius;
}


/**
 * @brief 즐겨찾기(좋아요 하트) 관련 설정
 * @param flagFavorite
 */
void AbstractImageDetailInfo_RHV::setFavorite(bool flagFavorite){
    // Data
    this->flagFavorite = flagFavorite;

    // UI
    if(this->curr_contentsType != Music_userplaylist){
        this->btn_fav_toAdd->setVisible(!flagFavorite);
        this->btn_fav_toAddx2->setVisible(flagFavorite);
        this->btn_fav_toAddx3->setVisible(flagFavorite);
        this->btn_fav_toDelete->setVisible(flagFavorite);
    }
    else if(this->curr_contentsType == Music_userplaylist){
        this->btn_thumb_toAdd->setVisible(!flagFavorite);
        this->btn_thumb_toDelete->setVisible(flagFavorite);
    }

}


void AbstractImageDetailInfo_RHV::setFavorite(bool flagFavorite, int star){

    // Data
    this->flagFavorite = flagFavorite;

    if(this->curr_contentsType != Music_userplaylist){
        // UI
        if(flagFavorite == true && star > 0){
            switch (star) {
                case 1:
                    this->btn_fav_toAdd->setVisible(!flagFavorite);
                    this->btn_fav_toAddx2->setVisible(flagFavorite);
                    this->btn_fav_toAddx3->setVisible(!flagFavorite);
                    this->btn_fav_toDelete->setVisible(!flagFavorite);
                    break;
                case 2:
                    this->btn_fav_toAdd->setVisible(!flagFavorite);
                    this->btn_fav_toAddx2->setVisible(!flagFavorite);
                    this->btn_fav_toAddx3->setVisible(flagFavorite);
                    this->btn_fav_toDelete->setVisible(!flagFavorite);
                    break;
                case 3:
                    this->btn_fav_toAdd->setVisible(!flagFavorite);
                    this->btn_fav_toAddx2->setVisible(!flagFavorite);
                    this->btn_fav_toAddx3->setVisible(!flagFavorite);
                    this->btn_fav_toDelete->setVisible(flagFavorite);
                    break;
            }
        }
        else{
            this->btn_fav_toAdd->setVisible(!flagFavorite);             // heart x0
            this->btn_fav_toAddx2->setVisible(flagFavorite);            // heart x1
            this->btn_fav_toAddx3->setVisible(flagFavorite);            // heart x2
            this->btn_fav_toDelete->setVisible(flagFavorite);           // heart x3
        }
    }
    else if(this->curr_contentsType == Music_userplaylist){
        // UI
        if(flagFavorite == true){
            this->btn_thumb_toAdd->setVisible(!flagFavorite);           // thumbup off
            this->btn_thumb_toDelete->setVisible(flagFavorite);         // thumbup on
        }
        else{
            this->btn_thumb_toAdd->setVisible(!flagFavorite);           // thumbup off
            this->btn_thumb_toDelete->setVisible(flagFavorite);         // thumbup on
        }
    }

    this->myFavoriteIds = flagFavorite;
    this->myStarIds = star;
}


/**
 * @brief AbstractImageDetailInfo::setPossiblePlay
 * @param flagCanPlay
 */
void AbstractImageDetailInfo_RHV::setPossiblePlay(bool flagCanPlay){
    if(flagCanPlay){
        this->widget_Addbtn_Play->setVisible(true);
        this->widget_Addbtn_PlayCannot->setVisible(false);
    }
    else{
        this->widget_Addbtn_Play->setVisible(false);
        this->widget_Addbtn_PlayCannot->setVisible(true);
    }
}


void AbstractImageDetailInfo_RHV::initView(){
    this->setImage("");
    this->label_titleUp->setText("");
    this->label_titleDown->setText("");
    //this->elidedLabel_description->setText("");
    this->label_description->setText("");
    this->label_creatorName->setText("");
    this->label_artist->setText("");

    this->btn_addCollect->setVisible(true);
    this->btn_more->setVisible(true);
    this->btn_info->setVisible(true);
    this->setFavorite(false);
    this->label_imageHiRes->hide();
}

/**
 * @brief AbstractImageDetailInfo::applyFlagMyData
 */
void AbstractImageDetailInfo_RHV::setMyData(bool flagMine){
    this->flagMyData = flagMine;
}


//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : [Slot]
//
//-----------------------------------------------------------------------------------------------------------------------


/**
 * @brief PlayListDetail::loadImage [SLOT] 앨범 이미지 세팅
 */
void AbstractImageDetailInfo_RHV::slot_fileDownload_loadImage()
{
    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(this->image_width, this->image_height));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(this->image_width, this->image_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, this->image_width, this->image_height, 8, 8);

        int leftValue = (this->image_width - tmp_pixmap.width()) / 2;
        int topValue = (this->image_height - tmp_pixmap.height()) / 2;

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->label_imageBig->setPixmap(pixmapIMG);
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }

    this->fileDownLoader->deleteLater();

    /*if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(this->image_width, this->image_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        this->paint_imageBig(tmp_pixmap);
        this->resize(this->size());
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }*/
}


/**
 * @brief pixmap 을 label_imageBig 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
 * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
 * @param pixmap
 */
void AbstractImageDetailInfo_RHV::paint_imageBig(QPixmap &pixmap){
    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(this->image_width, this->image_height));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, this->image_width, this->image_height, this->image_cornerRadius, Qt::RelativeSize);
    this->label_imageBig->setPixmap(pixmap_painter);
}


/**
 * @brief [Slot] 버튼 클릭에 대한 처리. 시그널 발생
 */
void AbstractImageDetailInfo_RHV::slot_btnClicked_mode(){
    BtnClickMode clickMode = sender()->property("mode").value<BtnClickMode>();
    emit this->signal_clicked(clickMode);

    /*if(clickMode == BtnClickMode::Favorite_toAdd){
        this->btn_fav_toAdd->setVisible(true);
        this->btn_fav_toAddx2->setVisible(false);
        this->btn_fav_toAddx3->setVisible(false);
        this->btn_fav_toDelete->setVisible(false);
    }
    else if(clickMode == BtnClickMode::Favorite_toAddx2){
        this->btn_fav_toAdd->setVisible(false);
        this->btn_fav_toAddx2->setVisible(true);
        this->btn_fav_toAddx3->setVisible(false);
        this->btn_fav_toDelete->setVisible(false);
    }
    else if(clickMode == BtnClickMode::Favorite_toAddx3){
        this->btn_fav_toAdd->setVisible(false);
        this->btn_fav_toAddx2->setVisible(false);
        this->btn_fav_toAddx3->setVisible(true);
        this->btn_fav_toDelete->setVisible(false);
    }
    else if(clickMode == BtnClickMode::Favorite_toDelete){
        this->btn_fav_toAdd->setVisible(true);
        this->btn_fav_toAddx2->setVisible(false);
        this->btn_fav_toAddx3->setVisible(false);
        this->btn_fav_toDelete->setVisible(false);
    }*/
}


void AbstractImageDetailInfo_RHV::slot_btnopen_clicked(){
    if(this->flagBtnOpen == false){
        this->widget_Addbtn_Open->hide();
        if(this->flagOpen == true){
            this->widget_Addbtn_Open->setFixedSize(1500, 80 + this->heightOpen);
            this->label_description->setGeometry(0, -20, 1450, 40 + this->heightOpen);
            this->label_open->setGeometry(0, 37 + this->heightOpen - 20, 1450, 30);
        }
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_open_ico.png", "text_close_ico.png"));
        this->widget_Addbtn_Open->show();

        this->flagBtnOpen = true;
    }
    else if(this->flagBtnOpen == true){
        this->widget_Addbtn_Open->hide();
        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_open->setGeometry(0, 37, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();

        this->flagBtnOpen = false;
    }
}



/**
 * @brief AbstractImageDetailInfo::resizeEvent
 * @param event
 */
void AbstractImageDetailInfo_RHV::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    /*if(this->elidedLabel_description->isVisible()){
        this->elidedLabel_description->setFixedWidth(event->size().width() - 350);
    }*/
}



/**
 * @brief AbstractImageDetailInfo::setTextDescription_withAdaptedWidgh
 * @param text
 */
void AbstractImageDetailInfo_RHV::setTextDescription_withAdaptedWidgh(QString text){
    Q_UNUSED(text);
    /*this->elidedLabel_description->setText(text);
    this->elidedLabel_description->setFixedWidth(this->size().width() - 350);
    this->elidedLabel_description->setFixedHeight(66);*/
}



/**
 * @brief ArtistImageDetailInfo::getNormalText
 * @details @ref https://stackoverflow.com/questions/11229831/regular-expression-to-remove-html-tags-from-a-string
 *      Testing : @ref https://www.freeformatter.com/regex-tester.html - \[[^\]]*\] 로 테스트 해보면 됨.
 * @param text
 * @return
 */
QString AbstractImageDetailInfo_RHV::getNormalText(QString text){

    text.replace("[", "<");
    text.replace("]", ">");
    text = text.remove( QRegExp( "<[^>]*>" ) );

    return text;

}



/**
 * @brief AbstractImageDetailInfo::setVisible_optionBtn
 * @param flagVisiable
 */
void AbstractImageDetailInfo_RHV::setVisible_optionBtn(bool flagVisiable){
    this->widget_optBtn->setVisible(flagVisiable);
}

/**
 * @brief AbstractImageDetailInfo::setVisible_playBtn
 * @param flagVisiable
 */
void AbstractImageDetailInfo_RHV::setVisible_playBtn(bool flagVisiable){
    this->widget_Addbtn_Play->setVisible(flagVisiable);
}

void AbstractImageDetailInfo_RHV::setHiRes_Left(int left, int height){
    if(height == 0){
        this->label_imageHiRes->setGeometry(450 + left + 20, 35, 0, 0);
    }
    else if(height == 1){
        this->label_imageHiRes->setGeometry(450 + left + 20, 100, 0, 0);
    }

    this->label_imageHiRes->show();
}

void AbstractImageDetailInfo_RHV::setOpen_Height(int height, bool flag){
    this->heightOpen = height;
    this->flagOpen = flag;
}
