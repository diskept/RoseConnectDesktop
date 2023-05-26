#include "widget/AbstractImageDetailContents_RHV.h"

#include "apple/Apple_struct.h"
#include "bugs/bugs_struct.h"
#include "qobuz/qobuz_struct.h"
#include "roseHome/rosehome_struct.h"
#include "tidal/tidal_struct.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/commonGetRedirectUrl.h"    //j220903 twitter

#include "roseHome/ProcCommon_forRosehome.h"

#include "widget/dialogconfirm.h"//c230215

#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScroller>


const int DEFAULT_IMAGE_SIZE = 400;
const int DEFAULT_IMAGE_SIZEW = 384;
const int DEFAULT_IMAGE_SIZEH = 186;
const int DEFAULT_IMAGE_SIZEW_RT = 360;
const int DEFAULT_IMAGE_SIZEH_RT = 200;
const int DEFUALT_IMAGE_SIZEW_V = 280;
const int DEFUALT_IMAGE_SIZEH_V = 394;
const int BTN_MENU_SIZE = 50;           ///< MyCollect, Fav, More 아이콘 버튼의 사이즈 (정사각형 고정 사이즈)

const QString IMAGE_PATH_DEFAULT_ARTIST = ":/images/rosehome/artist_def.png";

const QString IMAGE_PATH_DEFAULT_MUSIC = ":/images/def_mus_550.png";
const QString IMAGE_PATH_DEFAULT_VIDEO = ":/images/def_video_200x2.png";
const QString IMAGE_PATH_DEFAULT_CD = ":/images/cd/cd_thum_60x3.png";
const QString IMAGE_PATH_DEFAULT_TIDAL = ":/images/tidal/tidal_def_400.png";
const QString IMAGE_PATH_DEFAULT_BUGS = ":/images/bugs/bugs_def_430x2.png";
const QString IMAGE_PATH_DEFAULT_QOBUZ = ":/images/qobuz/qobuz_default_400.png";
const QString IMAGE_PATH_DEFAULT_APPLE = ":/images/apple/apple_def.png";

const QString IMAGE_PATH_DEFAULT_MUSIC_PLAYLIST = ":/images/def_mus_550.png";
const QString IMAGE_PATH_DEFAULT_ROSETUBE_PLAYLIST = ":/images/def_tube_340.png";
const QString IMAGE_PATH_DEFAULT_TIDAL_PLAYLIST = ":/images/tidal/tidal_def_video.png";
const QString IMAGE_PATH_DEFAULT_Bugs_pdAlbum = ":/images/bugs/bugs_def_284.png";
const QString IMAGE_PATH_DEFAULT_QOBUZ_PLAYLIST = ":/images/qobuz/qobuz_default_384.png";
const QString IMAGE_PATH_DEFAULT_APPLE_PLAYLIST = ":/images/apple/apple_def.png";

const QString IMAGE_PATH_DEFAULT_TIDAL_MQA = ":/images/tidal/mqa_icox2.png";
const QString IMAGE_PATH_DEFAULT_QOBUZ_HIRES = ":/images/qobuz/hires_a_ico.png";

const QString ICON_PATH___addCollect = ":/images/play_list_add_icon.png";
const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";
const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";
const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";
const QString ICON_PATH___thumbup_off = ":/images/rosehome/playlist_like_ico.png";
const QString ICON_PATH___thumbup_on = ":/images/rosehome/playlist like ico_on.png";
const QString ICON_PATH___share = ":/images/rosehome/icon_share.png";//c220816
const QString ICON_PATH___info = ":/images/icon_info.png";
const QString ICON_PATH___more = ":/images/icon_menu.png";

const QString ICON_PATH___playAll = ":/images/icon_list_play.png";
const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";


/**
 * @brief 생성자
 * @param parent
 */
AbstractImageDetailContents_RHV::AbstractImageDetailContents_RHV(ContentsUIType p_ContentsUIType, QWidget *parent)
    : QWidget(parent)
    , curr_contentsType(p_ContentsUIType)
{

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractImageDetailContents_RHV::slot_fileDownload_loadImage);

    this->btnStyle_play = "QPushButton{background:#B18658; border:1px solid #B18658; border-radius:4px;}";
    this->btnStyle_play += "QPushButton:hover{background:#7D6144; border:1px solid #7D6144; border-radius:4px;}";

    this->btnStyle_shuffle = "QPushButton{background:#777777; border:1px solid #777777; border-radius:4px;}";
    this->btnStyle_shuffle += "QPushButton:hover{background:#4D4D4D; border:1px solid #4D4D4D; border-radius:4px;}";

    this->image_width = DEFAULT_IMAGE_SIZE;
    this->image_height = DEFAULT_IMAGE_SIZE;
    this->image_cornerRadius = 4;

    switch(this->curr_contentsType){
        case rose_artist:
        case rose_album:
        case rose_playlist:
        case rose_userplaylist:
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
        case Music_artist:
        case Music_album:
        case Music_playlist:
        case Music_userplaylist:
            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;

        case Rosetube_userplaylist:
            this->image_width = DEFAULT_IMAGE_SIZEW_RT;
            this->image_height = DEFAULT_IMAGE_SIZEH_RT;
            this->image_cornerRadius = 4;

            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;

        case Tidal_artist:
        case Tidal_album:
        case Tidal_playlist:
        case Tidal_playlistMix:
            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;
        case Tidal_video:
            break;

        case Bugs_artist:
        case Bugs_album:
        case Bugs_pdAlbum:
        case Bugs_video:
            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;

        case Qobuz_artist:
        case Qobuz_album:
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

        case Apple_artist:
        case Apple_album:
        case Apple_playlist:
            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;

        case CDPlay_album://c220724
            this->setUIControl_basic();
            this->setUIControl_btnAddFavMore();
            this->setUIControl_btnPlays();
            this->setUIControl_btnPlays_cannot();

            // 기본값
            this->setPossiblePlay(true);
            break;

        case Video:
            this->image_width = DEFUALT_IMAGE_SIZEW_V;
            this->image_height = DEFUALT_IMAGE_SIZEH_V;
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
AbstractImageDetailContents_RHV::~AbstractImageDetailContents_RHV(){

    this->fileDownLoader->deleteLater();
    this->deleteLater();
}


/**
 * @brief 기본 UI 구성
 */
void AbstractImageDetailContents_RHV::setUIControl_basic(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();

    QString img_path = "";
    QString icon_path = "";
    switch(this->curr_contentsType){
        case rose_artist:
        case Music_artist:
        case Tidal_artist:
        case Bugs_artist:
        case Qobuz_artist:
        case Apple_artist:
            img_path = IMAGE_PATH_DEFAULT_ARTIST;
            this->image_cornerRadius = 200;
            break;

        case rose_album:
        case rose_playlist:
        case rose_userplaylist:
            img_path = IMAGE_PATH_DEFAULT_MUSIC;
            break;
        case Music_album:
        case Music_userplaylist:
            img_path = IMAGE_PATH_DEFAULT_MUSIC;
            break;
        case Music_playlist:
            img_path = IMAGE_PATH_DEFAULT_MUSIC_PLAYLIST;
            break;
        case Rosetube_userplaylist:
            img_path = IMAGE_PATH_DEFAULT_ROSETUBE_PLAYLIST;
            break;
        case Tidal_album:
            img_path = IMAGE_PATH_DEFAULT_TIDAL;
            icon_path = IMAGE_PATH_DEFAULT_TIDAL_MQA;
            break;
        case Tidal_playlist:
        case Tidal_playlistMix:
            img_path = IMAGE_PATH_DEFAULT_TIDAL;
            icon_path = IMAGE_PATH_DEFAULT_TIDAL_MQA;
            break;
        case Tidal_video:
            img_path = IMAGE_PATH_DEFAULT_TIDAL_PLAYLIST;
            break;
        case Bugs_album:
        case Bugs_pdAlbum:
            img_path = IMAGE_PATH_DEFAULT_BUGS;
            break;
        case Bugs_video:
            img_path = IMAGE_PATH_DEFAULT_Bugs_pdAlbum;
            break;
        case Qobuz_album:
            img_path = IMAGE_PATH_DEFAULT_QOBUZ;
            icon_path = IMAGE_PATH_DEFAULT_QOBUZ_HIRES;
            break;
        case Qobuz_playlist:
            img_path = IMAGE_PATH_DEFAULT_QOBUZ_PLAYLIST;
            break;
        case Apple_album:
            img_path = IMAGE_PATH_DEFAULT_APPLE;
            break;
        case Apple_playlist:
            img_path = IMAGE_PATH_DEFAULT_APPLE_PLAYLIST;
            break;
        case CDPlay_album://c220724
            img_path = IMAGE_PATH_DEFAULT_CD;
            break;
        case Video:
            img_path = IMAGE_PATH_DEFAULT_VIDEO;
            break;
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(img_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(this->image_width, this->image_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->widget_info_main = new QWidget();
    this->widget_info_main->setFixedSize(1500, 405);
    this->widget_info_main->setContentsMargins(0, 0, 0, 0);

    // Big Image (Album or Playlist Main Image)
    this->btn_Label_imageBig = new QPushButton(this->widget_info_main);//c230215
    this->btn_Label_imageBig->setProperty("imagePath","");//c230215
    this->btn_Label_imageBig->setFixedSize(this->image_width, this->image_height);
    this->btn_Label_imageBig->setCursor(Qt::PointingHandCursor);//c230215

    connect(this->btn_Label_imageBig, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_imageClick);//c230215

    //this->label_imageBig = new QLabel(this->widget_info_main);
    this->label_imageBig = new QLabel(this->btn_Label_imageBig);//c230215
    this->label_imageBig->setFixedSize(this->image_width, this->image_height);
    this->label_imageBig->setGeometry(0, 0, 0, 0);

    if(this->curr_contentsType == Qobuz_playlist){
        this->btn_Label_imageBig->setGeometry(8, 107, 0, 0);    //384, 186
        //this->label_imageBig->setGeometry(8, 107, 0, 0);    //384, 186
    }
    else if(this->curr_contentsType == Rosetube_userplaylist){
        this->btn_Label_imageBig->setGeometry(20, 100, 0, 0);    //360, 200
        //this->label_imageBig->setGeometry(20, 100, 0, 0);    //360, 200
    }
    else if(this->curr_contentsType == Video){
        this->btn_Label_imageBig->setGeometry(70, 3, 0, 0);    //280, 394
        //this->label_imageBig->setGeometry(70, 3, 0, 0);    //280, 394
    }
    else{
        this->btn_Label_imageBig->setGeometry(0, 0, 0, 0);
        //this->label_imageBig->setGeometry(0, 0, 0, 0);
    }

    this->label_imageType = new QLabel(this->label_imageBig);
    this->label_imageType->setStyleSheet("background-color:transparent;");

    // Title, Description, CreatorName
    this->label_title = new LabelLongAnimation(800, 500, 70, this->widget_info_main);
    //this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_title->setStyleSheet("background-color:transparent;color:#ffffff;font-size:50px;font-weight:500;line-height: 1.2;font-style: normal;text-align: top;");
    this->label_title->setGeometry(450, 20, 800, 70);

    this->label_imageHiRes = new QLabel(this->widget_info_main);

    if(!icon_path.isEmpty()){
        if(this->curr_contentsType == Tidal_album){//c220724
            this->label_imageHiRes->setFixedSize(142, 50);
            this->label_imageHiRes->setGeometry(1170, 35, 0, 0);

            QPixmap *img_mqa = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT_TIDAL_MQA);
            this->label_imageHiRes->setPixmap(*img_mqa);
        }
        else if(this->curr_contentsType == Qobuz_album){
            this->label_imageHiRes->setFixedSize(50, 50);
            this->label_imageHiRes->setGeometry(1170, 35, 0, 0);

            QPixmap *img_hiRes = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT_QOBUZ_HIRES);
            this->label_imageHiRes->setPixmap(*img_hiRes);
        }
    }
    this->label_imageHiRes->hide();

    this->label_creatorName = new QLabel(this->widget_info_main);
    this->label_creatorName->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_creatorName->setWordWrap(true);
    this->label_creatorName->setStyleSheet("background-color:transparent;font-size:25px;font-weight:300;font-style:normal;line-height:1.34;text-align:left;color:#e6e6e6;");
    this->label_creatorName->setGeometry(450, 175, 700, 40);

    this->label_artist = new QLabel(this->widget_info_main);
    this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);
    this->label_artist->setCursor(Qt::PointingHandCursor);
    this->label_artist->setWordWrap(true);
    this->label_artist->setStyleSheet("background-color:transparent;font-size:25px;font-weight:300;font-style:normal;line-height:1.34;text-align:left;color:#e6e6e6;");
    this->label_artist->setGeometry(450, 180, 700, 30);

    this->label_resolution = new QLabel(this->widget_info_main);
    this->label_resolution->setStyleSheet("background-color:transparent;color:#777777;font-size:16px;font-weight:300;font-style: normal;line-height: 2.1;text-align: left;");
    this->label_resolution->setGeometry(450, 215, 700, 24);

    this->widget_Addbtn_Icon = new QWidget(this->widget_info_main);
    this->widget_Addbtn_Icon->setFixedSize(700, 50);
    this->widget_Addbtn_Icon->setGeometry(450, 245, 0, 0);

    this->widget_Addbtn_Play = new QWidget(this->widget_info_main);
    this->widget_Addbtn_Play->setFixedSize(700, 50);
    this->widget_Addbtn_Play->setGeometry(450, 350, 0, 0);

    this->widget_Addbtn_PlayCannot = new QWidget(this->widget_info_main);
    this->widget_Addbtn_PlayCannot->setFixedSize(700, 50);
    this->widget_Addbtn_PlayCannot->setGeometry(450, 350, 0, 0);

    this->vBox_info = new QVBoxLayout();
    this->vBox_info->setContentsMargins(0, 20, 0, 0);
    this->vBox_info->addWidget(this->widget_info_main);
    this->vBox_info->addSpacing(70);

    this->widget_Addbtn_Open = new QWidget();
    this->widget_Addbtn_Open->setStyleSheet("background-color:transparent; border:1px;");
    this->widget_Addbtn_Open->setFixedSize(1500, 80);
    this->widget_Addbtn_Open->hide();


    //=====================================================================================================//

    /*if(this->curr_contentsType == CDPlay_album){
        this->widget_Addbtn_Play->setGeometry(450, 300, 0, 0);

        this->label_mInfo = new QLabel(this->widget_Addbtn_Play);//c220724
        this->label_mInfo->setTextInteractionFlags(Qt::TextSelectableByMouse);
        this->label_mInfo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        this->label_mInfo->setWordWrap(true);
        this->label_mInfo->setStyleSheet("background-color:transparent;color:#777777;font-size:20px;font-weight:300;");
        this->label_mInfo->setGeometry(0, 10, 700, 30);
        this->label_mInfo->hide();
    }*/
    if(this->curr_contentsType == Bugs_pdAlbum){

        this->widget_info_tag = new QWidget(this->widget_info_main);
        this->widget_info_tag->setStyleSheet("background-color:transparent;");
        this->widget_info_tag->setFixedSize(900, 45);
        this->widget_info_tag->setGeometry(450, 240, 0, 0);

        this->widget_Addbtn_Icon->setGeometry(450, 280, 0, 0);

        this->flowLayout_tag = new FlowLayout(this->widget_info_tag, 4, 6);
        this->flowLayout_tag->setSizeConstraint(QLayout::SetMinimumSize);
        this->flowLayout_tag->setContentsMargins(0,0,0,0);

        this->label_description = new QLabel(this->widget_Addbtn_Open);
        //this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setWordWrap(true);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_description->setStyleSheet("background-color:transparent; border:1px;");

        this->label_open = new QLabel(this->widget_Addbtn_Open);
        this->label_open->setStyleSheet("background-color:transparent; border:0px;");
        this->label_open->setGeometry(0, 37, 1450, 30);

        this->btn_open = GSCommon::getUIBtnImg("btn_open", ":/images/text_open_ico.png", this->label_open);
        this->btn_open->setCursor(Qt::PointingHandCursor);
        this->btn_open->setGeometry(735, 0, 30, 30);

        connect(this->btn_open, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_open);

        this->vBox_info->addWidget(this->widget_Addbtn_Open);
        this->vBox_info->addSpacing(10);
    }
    else if(this->curr_contentsType != Music_artist && this->curr_contentsType != Tidal_artist && this->curr_contentsType != Bugs_artist &&
            this->curr_contentsType != Qobuz_artist && this->curr_contentsType != Apple_artist){

        this->label_description = new QLabel(this->widget_Addbtn_Open);
        //this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setWordWrap(true);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_description->setStyleSheet("background-color:transparent; border:1px;");

        this->label_open = new QLabel(this->widget_Addbtn_Open);
        this->label_open->setStyleSheet("background-color:transparent; border:0px;");
        this->label_open->setGeometry(0, 37, 1450, 30);

        this->btn_open = GSCommon::getUIBtnImg("btn_open", ":/images/text_open_ico.png", this->label_open);
        this->btn_open->setCursor(Qt::PointingHandCursor);
        this->btn_open->setGeometry(735, 0, 30, 30);

        connect(this->btn_open, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_open);

        this->vBox_info->addWidget(this->widget_Addbtn_Open);
        this->vBox_info->addSpacing(10);
    }
    else if(this->curr_contentsType == Music_artist || this->curr_contentsType == Tidal_artist || this->curr_contentsType == Bugs_artist ||
                this->curr_contentsType == Qobuz_artist || this->curr_contentsType == Apple_artist){

        //this->label_title->setGeometry(450,10,0,0);

        this->label_biography = new QLabel(this->widget_info_main);
        this->label_biography->setGeometry(450, 90, 1000, 110);
        this->label_biography->setTextInteractionFlags(Qt::TextSelectableByMouse);
        this->label_biography->setTextFormat(Qt::RichText);
        this->label_biography->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;font-weight: normal;font-style: normal;line-height: 1.2;text-align: left;");
        this->label_biography->setWordWrap(true);

        this->btn_more_artist = new QPushButton(this->widget_info_main);
        this->btn_more_artist->setGeometry(450, 210, 200, 25);
        this->btn_more_artist->setCursor(Qt::PointingHandCursor);

        connect(this->btn_more_artist, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_artistMore);

        QLabel *label_more = new QLabel(this->btn_more_artist);        
        label_more->setStyleSheet("background-color:transparent;color:#b18658;font-size:20px;");
        label_more->setText(tr("More"));        
        label_more->setGeometry(0, 0, 200, 23);
        label_more->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    }

    this->setLayout(vBox_info);
}


/**
 * @brief 버튼 UI 설정 - My Collection 추가버튼, 좋아요 하트 버튼, 더보기 메뉴 버튼
 */
void AbstractImageDetailContents_RHV::setUIControl_btnAddFavMore(){

    // 버튼 객체 생성
    this->btn_addCollect = GSCommon::getUIBtnImg("btn_addCollect", ICON_PATH___addCollect);
    this->btn_fav_toAdd = GSCommon::getUIBtnImg("btn_fav_toAdd", ICON_PATH___favorite_off);
    this->btn_fav_toAddx2 = GSCommon::getUIBtnImg("btn_fav_toAddx2", ICON_PATH___favorite_on);
    this->btn_fav_toAddx3 = GSCommon::getUIBtnImg("btn_fav_toAddx3", ICON_PATH___favorite_on2);
    this->btn_fav_toDelete = GSCommon::getUIBtnImg("btn_fav_toDelete", ICON_PATH___favorite_on3);
    this->btn_thumb_toAdd = GSCommon::getUIBtnImg("btn_thumb_toAdd", ICON_PATH___thumbup_off);
    this->btn_thumb_toDelete = GSCommon::getUIBtnImg("btn_thumb_toDelete", ICON_PATH___thumbup_on);
    this->btn_share =  GSCommon::getUIBtnImg("btn_share", ICON_PATH___share);//c220818
    this->btn_info = GSCommon::getUIBtnImg("btn_info", ICON_PATH___info);
    this->btn_more = GSCommon::getUIBtnImg("btn_more", ICON_PATH___more);

    QHBoxLayout *hBox_btns = new QHBoxLayout();
    hBox_btns->setContentsMargins(0,0,0,0);            // 우측 여백만 설정
    hBox_btns->setSpacing(0);
    hBox_btns->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QList<QPushButton*> list_btn;
    QList<BtnClickMode> list_mode;

    if(this->curr_contentsType == Music_artist || this->curr_contentsType == CDPlay_album || this->curr_contentsType == Video){
        list_btn = {};
        list_mode = {};
    }
    else if(this->curr_contentsType == Tidal_artist || this->curr_contentsType == Bugs_artist || this->curr_contentsType == Qobuz_artist  || this->curr_contentsType == Apple_artist){
        list_btn = {this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_share, this->btn_more};
        list_mode = {Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Share, More};
    }
    else if(this->curr_contentsType == Music_album){
        list_btn = {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_info, this->btn_more};
        list_mode = {AddCollection, Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Info, More};
    }
    else if(this->curr_contentsType == Music_playlist){
        list_btn = {this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_more};
        list_mode = {Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, More};
    }
    else if(this->curr_contentsType == rose_album || this->curr_contentsType == rose_playlist){
        list_btn = {this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_share, this->btn_more};
        list_mode = {Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Share, More};
        this->btn_share->hide();
    }
    else if(this->curr_contentsType == Music_userplaylist || this->curr_contentsType == rose_userplaylist || this->curr_contentsType == Rosetube_userplaylist){
        list_btn = {this->btn_thumb_toAdd, this->btn_thumb_toDelete, this->btn_share, this->btn_more};
        list_mode = {Thumbup_toAdd, Thumbup_toDelete, Share, More};
    }
    else if(this->curr_contentsType == Tidal_playlistMix){
        list_btn = {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_more};
        list_mode = {AddCollection, Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, More};
    }
    else if(this->curr_contentsType == Qobuz_album){
        list_btn = {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_info, this->btn_share, this->btn_more};
        list_mode = {AddCollection, Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Info, Share, More};
    }
    else if(this->curr_contentsType == Apple_album || this->curr_contentsType == Apple_playlist){
        list_btn = {this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_share, this->btn_more};
        list_mode = {Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Share, More};
    }
    else{
        list_btn = {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toAddx2, this->btn_fav_toAddx3, this->btn_fav_toDelete, this->btn_share, this->btn_more};
        list_mode = {AddCollection, Favorite_toAdd, Favorite_toAddx2, Favorite_toAddx3, Favorite_toDelete, Share, More};
    }

    for(int i = 0; i < list_btn.length(); i++){
        list_btn.at(i)->setFixedSize(BTN_MENU_SIZE, BTN_MENU_SIZE);
        list_btn.at(i)->setCursor(Qt::PointingHandCursor);
        list_btn.at(i)->setProperty("mode", list_mode.at(i));
        hBox_btns->addWidget(list_btn.at(i));

        // connect
        connect(list_btn.at(i), &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_mode);
    }

    this->widget_Addbtn_Icon->setLayout(hBox_btns);
}


/**
 * @brief 버튼 UI 설정 - 전체재생, 셔플재생
 */
void AbstractImageDetailContents_RHV::setUIControl_btnPlays(){

    // Play All
    QPushButton *btn_play = new QPushButton(this->widget_Addbtn_Play);
    btn_play->setFixedSize(150, 50);
    btn_play->setGeometry(0, 0, 0, 0);
    btn_play->setStyleSheet(this->btnStyle_play);
    btn_play->setCursor(Qt::PointingHandCursor);
    btn_play->setProperty("mode", BtnClickMode::PlayAll);
    connect(btn_play, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_mode);

    QLabel *label_play_img = GSCommon::getUILabelImg(ICON_PATH___playAll, btn_play);
    label_play_img->setFixedSize(30, 30);
    label_play_img->setGeometry(32, 10, 0, 0);
    label_play_img->setStyleSheet("background-color:transparent;");

    QLabel *label_play = new QLabel(btn_play);
    //label_play->setFixedSize(41, 29);
    label_play->setStyleSheet("background-color:transparent;font-size:20px;font-weight:500;color:#FFFFFF;");
    label_play->setText("Play");
    label_play->setGeometry(67, 11, label_play->sizeHint().width(), 29);


    // Shuffle Play
    QPushButton *btn_shuffle = new QPushButton(this->widget_Addbtn_Play);
    btn_shuffle->setFixedSize(150, 50);
    btn_shuffle->setGeometry(170, 0, 0, 0);
    btn_shuffle->setStyleSheet(this->btnStyle_shuffle);
    btn_shuffle->setCursor(Qt::PointingHandCursor);
    btn_shuffle->setProperty("mode", BtnClickMode::PlayShuffle);
    connect(btn_shuffle, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_mode);

    if(this->curr_contentsType == CDPlay_album){
        QLabel *label_stop_img = new QLabel(btn_shuffle);
        label_stop_img->setFixedSize(12, 12);
        label_stop_img->setGeometry(36, 19, 0, 0);
        label_stop_img->setScaledContents(true);
        label_stop_img->setStyleSheet("background-color:#FFFFFF;");

        QLabel *label_stop = new QLabel(btn_shuffle);
//        label_stop->setFixedSize(68, 29);
        label_stop->setStyleSheet("background-color:transparent;font-size:20px;font-weight:500;color:#FFFFFF;");
        label_stop->setText("Stop");
        label_stop->setGeometry(58, 11, label_stop->sizeHint().width(), 29);
    }
    else{
        QLabel *label_shuffle_img = GSCommon::getUILabelImg(ICON_PATH___playShuffle, btn_shuffle);
        label_shuffle_img->setFixedSize(30, 30);
        label_shuffle_img->setGeometry(23, 10, 0, 0);
        label_shuffle_img->setStyleSheet("background-color:transparent;");

        QLabel *label_shuffle = new QLabel(btn_shuffle);
//        label_shuffle->setFixedSize(68, 29);
        label_shuffle->setStyleSheet("background-color:transparent;font-size:20px;font-weight:500;color:#FFFFFF;");
        label_shuffle->setText("Shuffle");
        label_shuffle->setGeometry(58, 11, this->sizeHint().width(), 29);
    }
}


/**
 * @brief 재생불가 View
 */
void AbstractImageDetailContents_RHV::setUIControl_btnPlays_cannot(){

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


void AbstractImageDetailContents_RHV::request_shareLlink(const QString thumnail, const QString desc, const QString title, const QString id){    //j220905 share link

    QString param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(id);

    switch(this->curr_contentsType){
        case rose_artist:
        case rose_album:

        case Music_artist:
        case Music_album:
        case Music_playlist:
        case Music_userplaylist:

        case CDPlay_album:

        case Tidal_playlistMix:
        case Tidal_video:

        case Bugs_video:
        case Video:
            break;

        case rose_playlist:
        case rose_userplaylist:
        case Rosetube_userplaylist:
            param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(id);
            break;

        case Tidal_album:
            param = "TIDAL/ALBUM/" + QString("%1").arg(id);
            break;

        case Tidal_playlist:
            param = "TIDAL/PLAYLIST/" + QString("%1").arg(id);
            break;

        case Tidal_artist:
            param = "TIDAL/ARTIST/" + QString("%1").arg(id);
            break;

        case Bugs_album:
            param = "BUGS/ALBUM/" + QString("%1").arg(id);
            break;

        case Bugs_pdAlbum:
            param = "BUGS/musicpd/" + QString("%1").arg(id);
            break;

        case Bugs_artist:
            param = "BUGS/ARTIST/" + QString("%1").arg(id);
            break;

        case Qobuz_album:
            param = "QOBUZ/ALBUM/" + QString("%1").arg(id);
            break;

        case Qobuz_playlist:
            param = "QOBUZ/PLAYLIST/" + QString("%1").arg(id);
            break;

        case Qobuz_artist:
            param = "QOBUZ/ARTIST/" + QString("%1").arg(id);
            break;

        case Apple_album:
            param = "APPLE_MUSIC/ALBUM/" + QString("%1").arg(id);
            break;

        case Apple_playlist:
            param = "APPLE_MUSIC/PLAYLIST/" + QString("%1").arg(id);
            break;

        case Apple_artist:
            param = "APPLE_MUSIC/ARTIST/" + QString("%1").arg(id);
            break;
    }

    roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
    connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &AbstractImageDetailContents_RHV::slot_applyResult_getShareLink);
    proc_link->request_rose_get_shareLink(thumnail, desc, title, param);
}


/**
 * @brief 우측에 Big 이미지에 imagePath 보이도록 한다.
 * @param imagePath
 */
void AbstractImageDetailContents_RHV::setImage(QString imagePath){

    // 이미지 경로 설정
    if(imagePath.isEmpty() == false){
        this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }

    this->btn_Label_imageBig->setProperty("imagePath",imagePath);//c230215
}


/**
 * @brief 이미지 사이즈와 corner radius 정보를 세팅한다. child class 의 생성자에서 호출해주도록 한다.
 * @param width
 * @param height
 * @param cornerRadius
 */
void AbstractImageDetailContents_RHV::setImageSizeAndCorner(int width, int height, int cornerRadius){

    this->image_width = width;
    this->image_height = height;
    this->image_cornerRadius = cornerRadius;
}


void AbstractImageDetailContents_RHV::initView(){

    this->detailInfo_shareLink = "";

    this->widget_Addbtn_Open->hide();

    this->setImage("");

    this->label_title->setText("");
    this->label_creatorName->setText("");
    this->label_artist->setText("");
    this->label_resolution->setText("");    
    this->label_imageHiRes->hide();

    this->type_image_path = "";
    this->flag_type_image = false;

    if(this->curr_contentsType == CDPlay_album || this->curr_contentsType ==  Video){
        this->label_description->setText("");
    }
    else if(this->curr_contentsType != Music_artist && this->curr_contentsType != Tidal_artist &&
            this->curr_contentsType != Bugs_artist && this->curr_contentsType != Qobuz_artist){

        this->setFavorite(false, 0);
        this->label_description->setText("");
    }
    else if(this->curr_contentsType == Music_artist || this->curr_contentsType == Tidal_artist ||
                this->curr_contentsType == Bugs_artist || this->curr_contentsType == Qobuz_artist){

        if(this->curr_contentsType != Music_artist){
            this->setFavorite(false, 0);
        }
        this->label_biography->setText("");
    }
    else if(this->curr_contentsType == Bugs_pdAlbum){

    }

    if(this->flagBtnOpen == true){
        this->flagBtnOpen = false;
    }
}


void AbstractImageDetailContents_RHV::setData_fromRoseData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == rose_album){
        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString type = ProcJsonEasy::getString(jsonObj, "type");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString owner = ProcJsonEasy::getString(jsonObj, "artist");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");

        QString registDateTime = ProcJsonEasy::getString(jsonObj, "registDateTime");

        QString comment = ProcJsonEasy::getString(jsonObj, "comment");

        QString image_type = "";
        if(type == "MUSIC"){
            image_type = ":/images/rosehome/home_musicx2.png";
        }
        else if(type == "VIDEO"){
            image_type = ":/images/rosehome/home_videox2.png";
        }
        else if(type == "YOUTUBE"){
            image_type = ":/images/rosehome/home_tubex2.png";
        }
        else if(type == "TIDAL"){
            image_type = ":/images/rosehome/home_tidalx2.png";
        }
        else if(type == "BUGS"){
            image_type = ":/images/rosehome/home_bugsx2.png";
        }
        else if(type == "QOBUZ"){
            image_type = ":/images/rosehome/home_qobuzx2.png";
        }
        else if(type == "APPLE_MUSIC"){
            image_type = ":/images/rosehome/home_applex2.png";
        }
        else{
            image_type = ":/images/rosehome/home_allx2.png";
        }

        // album 모드로 배치한다.
        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        // 데이터를 UI에 세팅한다.
        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(owner);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = owner;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(owner);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(owner);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(!registDateTime.isEmpty()){
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = registDateTime;
            splitToken = tmp_split.split("T");

            QDate lastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Registration date " + QLocale().toString(lastupdated, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += "| 등록일자 " + lastupdated.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!comment.isEmpty()){
            setHTML = comment;

            QLabel *tmp_html = new QLabel();
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = tr("There is no description for the playlist.");
        }

        this->setOpen_Height(height, flag);
        this->label_description->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_open->setGeometry(0, 43, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
    else if(this->curr_contentsType == rose_playlist){

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString type = ProcJsonEasy::getString(jsonObj, "type");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString owner = ProcJsonEasy::getString(jsonObj, "owner");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");

        QString registDateTime = ProcJsonEasy::getString(jsonObj, "registDateTime");

        QString comment = ProcJsonEasy::getString(jsonObj, "comment");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(img_path, comment, title, QString("%1").arg(id));    //j220905 share link

        QString image_type = "";
        if(type == "MUSIC"){
            image_type = ":/images/rosehome/home_musicx2.png";
        }
        else if(type == "VIDEO"){
            image_type = ":/images/rosehome/home_videox2.png";
        }
        else if(type == "YOUTUBE"){
            image_type = ":/images/rosehome/home_tubex2.png";
        }
        else if(type == "TIDAL"){
            image_type = ":/images/rosehome/home_tidalx2.png";
        }
        else if(type == "BUGS"){
            image_type = ":/images/rosehome/home_bugsx2.png";
        }
        else if(type == "QOBUZ"){
            image_type = ":/images/rosehome/home_qobuzx2.png";
        }
        else if(type == "APPLE_MUSIC"){
            image_type = ":/images/rosehome/home_applex2.png";
        }
        else{
            image_type = ":/images/rosehome/home_allx2.png";
        }

        if((owner == "Public") && (type != "MUSIC") ){
            this->btn_share->show();    //j220906 share link
        }

        // Playlist 모드로 배치한다.
        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        // 데이터를 UI에 세팅한다.
        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(owner);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = owner;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(owner);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(owner);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(!registDateTime.isEmpty()){
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = registDateTime;
            splitToken = tmp_split.split("T");

            QDate lastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Registration date " + QLocale().toString(lastupdated, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += "| 등록일자 " + lastupdated.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!comment.isEmpty()){
            setHTML = comment;

            QLabel *tmp_html = new QLabel();
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = tr("There is no description for the playlist.");
        }

        this->setOpen_Height(height, flag);
        this->label_description->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_open->setGeometry(0, 43, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
    else if(this->curr_contentsType == rose_userplaylist){

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString type = ProcJsonEasy::getString(jsonObj, "type");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString owner = ProcJsonEasy::getString(jsonObj, "owner");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");

        QString registDateTime = ProcJsonEasy::getString(jsonObj, "registDateTime");

        QString comment = ProcJsonEasy::getString(jsonObj, "comment");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(img_path, comment, title, QString("%1").arg(id));    //j220905 share link

        QString image_type = "";
        if(type == "MUSIC"){
            image_type = ":/images/rosehome/home_musicx2.png";
        }
        else if(type == "VIDEO"){
            image_type = ":/images/rosehome/home_videox2.png";
        }
        else if(type == "YOUTUBE"){
            image_type = ":/images/rosehome/home_tubex2.png";
        }
        else if(type == "TIDAL"){
            image_type = ":/images/rosehome/home_tidalx2.png";
        }
        else if(type == "BUGS"){
            image_type = ":/images/rosehome/home_bugsx2.png";
        }
        else if(type == "QOBUZ"){
            image_type = ":/images/rosehome/home_qobuzx2.png";
        }
        else if(type == "APPLE_MUSIC"){
            image_type = ":/images/rosehome/home_applex2.png";
        }
        else{
            image_type = ":/images/rosehome/home_allx2.png";
        }

        this->btn_share->show();

        // Playlist 모드로 배치한다.
        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        // 데이터를 UI에 세팅한다.
        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(owner);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = owner;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(owner);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(owner);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(!registDateTime.isEmpty()){
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = registDateTime;
            splitToken = tmp_split.split("T");

            QDate lastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Registration date " + QLocale().toString(lastupdated, "MMM d, yyyy");
            }
            else if(global.lang == 1){
                str_resol += "| 등록일자 " + lastupdated.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!comment.isEmpty()){
            setHTML = comment;

            QLabel *tmp_html = new QLabel();
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = tr("There is no description for the playlist.");
        }

        this->setOpen_Height(height, flag);
        this->label_description->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_open->setGeometry(0, 43, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void AbstractImageDetailContents_RHV::setData_fromMusicData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == Music_artist){

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString title = ProcJsonEasy::getString(jsonObj, "title");

        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(false);

        this->setImage(img_path);

        this->label_title->setText(title);
    }
    else if(this->curr_contentsType == Music_album){

        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString mime = ProcJsonEasy::getString(jsonObj, "mime");
        QString artist = ProcJsonEasy::getString(jsonObj, "artist");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_musicx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        int mime_left = this->label_title->sizeHint().width();
        int mime_top = this->label_title->geometry().top() + 10;

        if(!mime.isEmpty()){
            int width_set = 0;

            QLabel *tmp_mime = new QLabel();
            tmp_mime->setStyleSheet("background-color:#75000000;font-size:35px;font-weight:500;color:#FFFFFF;Text-align:Center;border:1px solid #FFFFFF;border-radius:20px;");
            tmp_mime->setText(mime);

            width_set = tmp_mime->sizeHint().width() + 10;

            this->label_imageHiRes->setFixedSize(width_set, 40);
            this->label_imageHiRes->setStyleSheet("background-color:#75000000;font-size:35px;font-weight:500;color:#FFFFFF;Text-align:Center;border:2px solid #FFFFFF;border-radius:20px;");
            this->label_imageHiRes->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

            this->label_imageHiRes->setText(mime);
            this->setHiRes_Left(mime_left, mime_top);
        }

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        this->label_resolution->setText(str_resol);
    }
}


void AbstractImageDetailContents_RHV::setData_fromRosetubeData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == Rosetube_userplaylist){

        // Playlist 모드로 배치한다.
        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString owner = ProcJsonEasy::getString(jsonObj, "owner");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");

        QString registDateTime = ProcJsonEasy::getString(jsonObj, "registDateTime");

        QString comment = ProcJsonEasy::getString(jsonObj, "comment");

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_tubex2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(owner);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = owner;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(owner);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(owner);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(!registDateTime.isEmpty()){
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = registDateTime;
            splitToken = tmp_split.split("T");

            QDate lastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Registration date " + QLocale().toString(lastupdated, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += "| 등록일자 " + lastupdated.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!comment.isEmpty()){
            setHTML = comment;

            QLabel *tmp_html = new QLabel();
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = tr("There is no description for the playlist.");
        }

        this->setOpen_Height(height, flag);
        this->label_description->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 48);
        this->label_open->setGeometry(0, 55, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void AbstractImageDetailContents_RHV::setData_fromTidalData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == Tidal_artist){
        QString name = ProcJsonEasy::getString(jsonObj, "name");
        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString biography = ProcJsonEasy::getString(jsonObj, "biography");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220907 share link

        this->request_shareLlink(img_path, biography, name, QString("%1").arg(id));    //j220907 share link

        if(biography.length() < 173){
            QString setHtml = QString("<html><head/><body><span style='font-size:14px; font-weight:normal; line-height: 1.14; color:#FFFFFF;'>%1</span></body></html>").arg(biography);
            this->label_biography->setText(setHtml);
        }
        else{
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_bio_line1 = "";
            QString tmp_bio_line2 = "";
            QString tmp_bio_line3 = "";
            QString tmp_bio_total = "";

            tmp_split = biography;
            splitToken = tmp_split.split(" ");

            int i, j, k = 0;
            for (i = 0; i < splitToken.count(); i++){
                tmp_bio_line1 += splitToken.at(i) + " ";

                QLabel *tmp_bio1 = new QLabel();
                tmp_bio1->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio1->setText(tmp_bio_line1);

                int bio1_width = tmp_bio1->sizeHint().width();

                if(bio1_width > 1000){
                    tmp_bio_total += tmp_bio_line1.replace(splitToken.at(i) + " ", "");
                    break;
                }
            }

            for (j = i; j < splitToken.count(); j++){
                tmp_bio_line2 += splitToken.at(j) + " ";

                QLabel *tmp_bio2 = new QLabel();
                tmp_bio2->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio2->setText(tmp_bio_line2);

                int bio2_width = tmp_bio2->sizeHint().width();

                if(bio2_width > 1000){
                    tmp_bio_total += tmp_bio_line2.replace(splitToken.at(j) + " ", "");
                    break;
                }
            }

            for (k = j; j < splitToken.count(); k++){
                tmp_bio_line3 += splitToken.at(k) + " ";

                QLabel *tmp_bio3 = new QLabel();
                tmp_bio3->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio3->setText(tmp_bio_line3);

                int bio3_width = tmp_bio3->sizeHint().width();

                if(bio3_width > 1000){
                    tmp_bio_total += tmp_bio_line3.replace(splitToken.at(k) + " ", "");
                    break;
                }
            }

            QLabel *set_bio_total = new QLabel();
            set_bio_total->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
            set_bio_total->setText(tmp_bio_total);

            int bio_total_width = set_bio_total->sizeHint().width();

            this->label_biography->setText(GSCommon::getTextCutFromLabelWidth(tmp_bio_total, bio_total_width, this->label_biography->font()));
            if(this->label_biography->text().contains("…")){
                this->label_biography->setToolTip(tmp_bio_total);
                this->label_biography->setToolTipDuration(2000);
            }
        }

        this->label_title->setText(name);
        this->setImage(img_path);
    }
    else if(this->curr_contentsType == Tidal_album){

        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");

        tidal::AlbumItemData data_output;
        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "artist").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_name.append(tmp_val.value<QString>());
        }

        QString artist = data_output.list_artist_name.join(",");

        QString audioQuality = ProcJsonEasy::getString(jsonObj, "audioQuality");
        QString release = ProcJsonEasy::getString(jsonObj, "releaseDate");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(img_path, artist, title, QString("%1").arg(id));    //j220905 share link

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_tidalx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        int mqa_left = this->label_title->sizeHint().width();
        int mqa_top = this->label_title->geometry().bottom() - 60;

        if(audioQuality == "HI_RES"){
            this->setHiRes_Left(mqa_left, mqa_top);
        }

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        if(!release.isEmpty()){

            QDate releaseDate = QDate::fromString(release, "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Release " + QLocale().toString(releaseDate, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += "| 발매 " + releaseDate.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);
    }
    else if(this->curr_contentsType == Tidal_playlist || this->curr_contentsType == Tidal_playlistMix){

        // Playlist 모드로 배치한다.
        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString owner = ProcJsonEasy::getString(jsonObj, "owner");


        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        QString lastUpdated = ProcJsonEasy::getString(jsonObj, "lastUpdated");

        QString description = ProcJsonEasy::getString(jsonObj, "description");

        QString id = ProcJsonEasy::getString(jsonObj, "uuid");    //j220905 share link

        if(this->curr_contentsType == Tidal_playlist){
            this->request_shareLlink(img_path, description, title, id);    //j220905 share link
        }

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_tidalx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(owner);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = owner;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(owner);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(owner);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        if(!lastUpdated.isEmpty()){
            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = lastUpdated;
            splitToken = tmp_split.split("T");

            QDate qDatelastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Last updated " + QLocale().toString(qDatelastupdated, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += QString("| Last updated %1").arg(qDatelastupdated.toString("M월 d일, yyyy"));
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(description);

            QLabel *tmp_html = new QLabel();
            tmp_html->setTextFormat(Qt::RichText);
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the playlist."));
        }

        this->setOpen_Height(height, flag);
        this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 48);
        this->label_open->setGeometry(0, 55, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void  AbstractImageDetailContents_RHV::setData_fromBugsData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == Bugs_artist){
        QString name = ProcJsonEasy::getString(jsonObj, "name");
        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString biography = ProcJsonEasy::getString(jsonObj, "biography");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220907 share link

        this->request_shareLlink(img_path, biography, name, QString("%1").arg(id));    //j220907 share link

        if(biography.length() < 173){
            this->label_biography->setText(biography);
        }
        else{
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_bio_line1 = "";
            QString tmp_bio_line2 = "";
            QString tmp_bio_line3 = "";
            QString tmp_bio_total = "";

            tmp_split = biography;
            splitToken = tmp_split.split(" ");

            int i, j, k = 0;
            for (i = 0; i < splitToken.count(); i++){
                tmp_bio_line1 += splitToken.at(i) + " ";

                QLabel *tmp_bio1 = new QLabel();
                tmp_bio1->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio1->setText(tmp_bio_line1);

                int bio1_width = tmp_bio1->sizeHint().width();

                if(bio1_width > 1000){
                    tmp_bio_total += tmp_bio_line1.replace(splitToken.at(i) + " ", "");
                    break;
                }
            }

            for (j = i; j < splitToken.count(); j++){
                tmp_bio_line2 += splitToken.at(j) + " ";

                QLabel *tmp_bio2 = new QLabel();
                tmp_bio2->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio2->setText(tmp_bio_line2);

                int bio2_width = tmp_bio2->sizeHint().width();

                if(bio2_width > 1000){
                    tmp_bio_total += tmp_bio_line2.replace(splitToken.at(j) + " ", "");
                    break;
                }
            }

            for (k = j; j < splitToken.count(); k++){
                tmp_bio_line3 += splitToken.at(k) + " ";

                QLabel *tmp_bio3 = new QLabel();
                tmp_bio3->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio3->setText(tmp_bio_line3);

                int bio3_width = tmp_bio3->sizeHint().width();

                if(bio3_width > 1000){
                    tmp_bio_total += tmp_bio_line3.replace(splitToken.at(k) + " ", "");
                    break;
                }
            }

            QLabel *set_bio_total = new QLabel();
            set_bio_total->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
            set_bio_total->setText(tmp_bio_total);

            int bio_total_width = set_bio_total->sizeHint().width();

            this->label_biography->setText(GSCommon::getTextCutFromLabelWidth(tmp_bio_total, bio_total_width, this->label_biography->font()));
            if(this->label_biography->text().contains("…")){
                this->label_biography->setToolTip(tmp_bio_total);
                this->label_biography->setToolTipDuration(2000);
            }
        }

        this->label_title->setText(name);
        this->setImage(img_path);
    }
    else if(this->curr_contentsType == Bugs_album){

        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");

        bugs::AlbumItemData data_output;
        QVariantList tmp_list_name = ProcJsonEasy::getJsonArray(jsonObj, "artist").toVariantList();
        foreach(QVariant tmp_val, tmp_list_name){
            data_output.list_artist_nm.append(tmp_val.value<QString>());
        }

        QString artist = data_output.list_artist_nm.join(",");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        QString release = ProcJsonEasy::getString(jsonObj, "releaseDate");

        QString description = ProcJsonEasy::getString(jsonObj, "description");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220905 share link    //c221209

        this->request_shareLlink(img_path, artist, title, QString("%1").arg(id));    //j220905 share link  //c221209

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_bugsx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(track_count > 0){
            if(global.lang == 0){
                if(track_count > 1){
                    str_resol = QString("%1 tracks ").arg(track_count);
                }
                else{
                    str_resol = QString("%1 track ").arg(track_count);
                }
            }
            else if(global.lang == 1){
                str_resol = QString("총 %1개 트랙 ").arg(track_count);
            }

            if(duration == 0){

            }
            else if(duration >= 3600){
                str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
            }
            else{
                str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
            }

            if(!release.isEmpty()){

                QDate releaseDate = QDate::fromString(release, "yyyy-MM-dd");

                if(global.lang == 0){
                    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                    str_resol += "| Release " + QLocale().toString(releaseDate, "MMM d, yyyy");

                    QLocale::setDefault(QLocale::system());
                }
                else if(global.lang == 1){
                    str_resol += "| 발매 " + releaseDate.toString("M월 d일, yyyy");
                }
            }
        }
        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = description;

            QLabel *tmp_html = new QLabel();
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = tr("There is no description for the album.");
        }

        this->setOpen_Height(height, flag);
        this->label_description->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 41);
        this->label_open->setGeometry(0, 44, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
    else if(this->curr_contentsType == Bugs_pdAlbum){

        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");

        QString artist = ProcJsonEasy::getString(jsonObj, "artist");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        QStringList tmpRelease = ProcJsonEasy::getString(jsonObj, "releaseDate").split("T");
        QString release = tmpRelease.at(0);

        QString description = ProcJsonEasy::getString(jsonObj, "description");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220905 share link    //c221209

        this->request_shareLlink(img_path, artist, title, QString("%1").arg(id));    //j220905 share link  //c221209

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_bugsx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(artist);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 145, 700, 60);
        }
        else{
            this->label_creatorName->setText(artist);
            this->label_creatorName->setGeometry(450, 175, 700, 30);
        }
        this->label_creatorName->show();

        QString str_resol = "";
        if(track_count > 0){
            if(global.lang == 0){
                if(track_count > 1){
                    str_resol = QString("%1 tracks ").arg(track_count);
                }
                else{
                    str_resol = QString("%1 track ").arg(track_count);
                }
            }
            else if(global.lang == 1){
                str_resol = QString("총 %1개 트랙 ").arg(track_count);
            }

            if(duration == 0){

            }
            else if(duration >= 3600){
                str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
            }
            else{
                str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
            }

            if(!release.isEmpty()){

                QDate releaseDate = QDate::fromString(release, "yyyy-MM-dd");

                if(global.lang == 0){
                    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                    str_resol += "| Release " + QLocale().toString(releaseDate, "MMM d, yyyy");

                    QLocale::setDefault(QLocale::system());
                }
                else if(global.lang == 1){
                    str_resol += "| 발매 " + releaseDate.toString("M월 d일, yyyy");
                }
            }
        }
        this->label_resolution->setText(str_resol);
        this->label_resolution->setGeometry(450, 205, 700, 24);

        QJsonArray tmpTag = ProcJsonEasy::getJsonArray(jsonObj, "tag");

        this->list_tag_name.clear();
        for(int i = 0; i < tmpTag.count(); i++){
            QJsonObject dataTag = tmpTag.at(i).toObject();

            // 태그 정보
            this->list_tag_name.append(ProcJsonEasy::getString(dataTag, "name"));

            QPushButton *btn_tag = new QPushButton();
            btn_tag->setProperty("idx", i);
            btn_tag->setText("#" + this->list_tag_name.at(i));
            btn_tag->setStyleSheet("font-size:14px; background-color:#333333; color:#919191; padding: 2px 7px;");
            btn_tag->setCursor(Qt::PointingHandCursor);
            btn_tag->setFixedHeight(20);
            connect(btn_tag, &QPushButton::clicked, this, &AbstractImageDetailContents_RHV::slot_btnClicked_tag);

            this->flowLayout_tag->addWidget(btn_tag);
        }

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = description;

            QLabel *tmp_html = new QLabel();
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = tr("There is no description for the album.");
        }

        this->setOpen_Height(height, flag);
        this->label_description->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:16px;");
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 48);
        this->label_open->setGeometry(0, 55, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void AbstractImageDetailContents_RHV::setData_fromQobuzData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == Qobuz_artist){
        QString name = ProcJsonEasy::getString(jsonObj, "name");
        QString img_path = ProcJsonEasy::getString(jsonObj, "image");
        QString biography = ProcJsonEasy::getString(jsonObj, "biography");

        int id = ProcJsonEasy::getInt(jsonObj, "id");    //j220907 share link

        this->request_shareLlink(img_path, biography, name, QString("%1").arg(id));    //j220907 share link

        if(biography.length() < 173){
            this->label_biography->setText(biography);
        }
        else{
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_bio_line1 = "";
            QString tmp_bio_line2 = "";
            QString tmp_bio_line3 = "";
            QString tmp_bio_total = "";

            tmp_split = biography;
            splitToken = tmp_split.split(" ");

            int i, j, k = 0;
            for (i = 0; i < splitToken.count(); i++){
                tmp_bio_line1 += splitToken.at(i) + " ";

                QLabel *tmp_bio1 = new QLabel();
                tmp_bio1->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio1->setText(tmp_bio_line1);

                int bio1_width = tmp_bio1->sizeHint().width();

                if(bio1_width > 1000){
                    tmp_bio_total += tmp_bio_line1.replace(splitToken.at(i) + " ", "");
                    break;
                }
            }

            for (j = i; j < splitToken.count(); j++){
                tmp_bio_line2 += splitToken.at(j) + " ";

                QLabel *tmp_bio2 = new QLabel();
                tmp_bio2->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio2->setText(tmp_bio_line2);

                int bio2_width = tmp_bio2->sizeHint().width();

                if(bio2_width > 1000){
                    tmp_bio_total += tmp_bio_line2.replace(splitToken.at(j) + " ", "");
                    break;
                }
            }

            for (k = j; j < splitToken.count(); k++){
                tmp_bio_line3 += splitToken.at(k) + " ";

                QLabel *tmp_bio3 = new QLabel();
                tmp_bio3->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
                tmp_bio3->setText(tmp_bio_line3);

                int bio3_width = tmp_bio3->sizeHint().width();

                if(bio3_width > 1000){
                    tmp_bio_total += tmp_bio_line3.replace(splitToken.at(k) + " ", "");
                    break;
                }
            }

            QLabel *set_bio_total = new QLabel();
            set_bio_total->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
            set_bio_total->setText(tmp_bio_total);

            int bio_total_width = set_bio_total->sizeHint().width();

            this->label_biography->setText(GSCommon::getTextCutFromLabelWidth(tmp_bio_total, bio_total_width, this->label_biography->font()));
            if(this->label_biography->text().contains("…")){
                this->label_biography->setToolTip(tmp_bio_total);
                this->label_biography->setToolTipDuration(2000);
            }
        }

        this->label_title->setText(name);
        this->setImage(img_path);
    }
    else if(this->curr_contentsType == Qobuz_album){
        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString artist = ProcJsonEasy::getString(jsonObj, "artist");

        bool hires = ProcJsonEasy::getBool(jsonObj, "hires");

        int track_count = ProcJsonEasy::getInt(jsonObj, "numberOfTracks");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");
        double sampleRate = ProcJsonEasy::getDouble(jsonObj, "sampleRate");
        int bitDepth = ProcJsonEasy::getDouble(jsonObj, "bitDepth");

        QString release = ProcJsonEasy::getString(jsonObj, "release_date_stream");

        QString description = ProcJsonEasy::getString(jsonObj, "description");

        QString id = ProcJsonEasy::getString(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(img_path, artist, title, id);    //j220905 share link

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_qobuzx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        int hires_left = this->label_title->sizeHint().width();
        int hires_top = this->label_title->geometry().bottom() - 60;

        if(hires){
            this->setHiRes_Left(hires_left, hires_top);
        }

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        if(hires){
            str_resol += QString("| FLAC %1kHz %2bit ").arg(sampleRate).arg(bitDepth);
        }
        else{
            str_resol += QString("| %1kHz %2bit ").arg(sampleRate).arg(bitDepth);
        }

        if(!release.isEmpty()){

            QDate releaseDate = QDate::fromString(release, "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Release " + QLocale().toString(releaseDate, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += "| 발매 " + releaseDate.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(description);

            QLabel *tmp_html = new QLabel();
            tmp_html->setTextFormat(Qt::RichText);
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the album."));
        }

        this->setOpen_Height(height, flag);
        this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 48);
        this->label_open->setGeometry(0, 55, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
    else if(this->curr_contentsType == Qobuz_playlist){

        // Playlist 모드로 배치한다.
        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        QString img_path = ProcJsonEasy::getString(jsonObj, "image");

        QString title = ProcJsonEasy::getString(jsonObj, "title");
        QString owner = ProcJsonEasy::getString(jsonObj, "owner");

        bool hires = ProcJsonEasy::getBool(jsonObj, "hires");

        int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        QString lastUpdated = ProcJsonEasy::getString(jsonObj, "lastUpdated");

        QString description = ProcJsonEasy::getString(jsonObj, "description");

        QString id = ProcJsonEasy::getString(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(img_path, description, title, id);    //j220905 share link

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_qobuzx2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(img_path);

        this->label_title->setText(title);

        int hires_left = this->label_title->sizeHint().width();
        int hires_top = this->label_title->geometry().bottom() - 60;

        if(hires){
            this->setHiRes_Left(hires_left, hires_top);
        }

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(owner);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = owner;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(owner);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(owner);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        if(!lastUpdated.isEmpty()){

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                QDate tmpDate = QDateTime::fromTime_t(lastUpdated.toInt()).toUTC().date();
                str_resol += "| Last updated " + QLocale().toString(tmpDate, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += QString("| Last updated %1").arg(QDateTime::fromTime_t(lastUpdated.toInt()).toUTC().toString("M월 d일, yyyy"));
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(description);

            QLabel *tmp_html = new QLabel();
            tmp_html->setTextFormat(Qt::RichText);
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the playlist."));
        }

        this->setOpen_Height(height, flag);
        this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 48);
        this->label_open->setGeometry(0, 55, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void AbstractImageDetailContents_RHV::setData_fromAppleData(const QJsonObject &jsonObj){

    if(this->curr_contentsType == Apple_artist){

    }
    else if(this->curr_contentsType == Apple_album){

        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        QJsonObject artwork = ProcJsonEasy::getJsonObject(jsonObj, "artwork");
        QString url = ProcJsonEasy::getString(artwork, "url");
        url.replace("{w}x{h}", "400x400");

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_applex2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(url);

        QString title = ProcJsonEasy::getString(jsonObj, "name");
        QString artist = ProcJsonEasy::getString(jsonObj, "artistName");

        int track_count = ProcJsonEasy::getInt(jsonObj, "trackCount");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        QString recordLabel = ProcJsonEasy::getString(jsonObj, "recordLabel");
        QString release = ProcJsonEasy::getString(jsonObj, "releaseDate");

        QJsonObject editorialNotes = ProcJsonEasy::getJsonObject(jsonObj, "editorialNotes");
        QString standard = ProcJsonEasy::getString(editorialNotes, "standard");

        QString id = ProcJsonEasy::getString(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(url, artist, title, id);    //j220905 share link

        this->label_title->setText(title);

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 700){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        QString DateString = "";
        if(!release.isEmpty()){

            QDate releaseDate = QDate::fromString(release, "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                DateString = QLocale().toString(releaseDate, "MMM d, yyyy");
                str_resol += "| Release " + DateString;

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                DateString = releaseDate.toString("yyyy년 M월 d일");
                str_resol += "| 발매 " + releaseDate.toString("M월 d일, yyyy");
            }
        }

        this->label_resolution->setText(str_resol);

        QString description = "";
        description += title + " by " + artist + "<br><br>";

        if(!recordLabel.isEmpty() && !release.isEmpty()){
            description += "Release by " + recordLabel + " on " + DateString + "<br><br>";
        }

        if(!standard.isEmpty()){
            description += standard;
        }

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(description);

            QLabel *tmp_html = new QLabel();
            tmp_html->setTextFormat(Qt::RichText);
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the album."));
        }

        this->setOpen_Height(height, flag);
        this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_open->setGeometry(0, 37, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
    else if(this->curr_contentsType == Apple_playlist){

        // Playlist 모드로 배치한다.
        this->label_artist->setVisible(false);
        this->label_creatorName->setVisible(true);

        QJsonObject artwork = ProcJsonEasy::getJsonObject(jsonObj, "artwork");
        QString url = ProcJsonEasy::getString(artwork, "url");
        url.replace("{w}x{h}", "400x400");

        // 데이터를 UI에 세팅한다.
        QString image_type = ":/images/rosehome/home_applex2.png";

        this->flag_type_image = true;
        this->type_image_path = image_type;
        this->setImage(url);

        QString title = ProcJsonEasy::getString(jsonObj, "name");
        QString curator = ProcJsonEasy::getString(jsonObj, "curatorName");

        int track_count = ProcJsonEasy::getInt(jsonObj, "trackCount");
        int duration = ProcJsonEasy::getInt(jsonObj, "duration");

        QString lastModifiedDate = ProcJsonEasy::getString(jsonObj, "lastModifiedDate");

        QJsonObject descriptionObj = ProcJsonEasy::getJsonObject(jsonObj, "description");
        QString description = ProcJsonEasy::getString(descriptionObj, "standard");

        QString id = ProcJsonEasy::getString(jsonObj, "id");    //j220905 share link

        this->request_shareLlink(url, description, title, id);    //j220905 share link

        this->label_title->setText(title);

        QLabel *tmp_creator = new QLabel();
        tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_creator->setText(curator);

        int creator_width = 0;
        creator_width = tmp_creator->sizeHint().width();

        if(creator_width > 700){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_creator_line1 = "";

            tmp_split = curator;
            splitToken = tmp_split.split(" ");

            tmp_creator->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_creator_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_creator_line1 += " " + splitToken.at(i);
                    }
                    tmp_creator->setText(tmp_creator_line1);

                    if(tmp_creator->sizeHint().width() > 700){
                        tmp_creator->setText("");
                        tmp_creator_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_creator->setText("");
            tmp_creator->setText(tmp_creator_line1);

            creator_width = tmp_creator->sizeHint().width() + 700;

            this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(curator, creator_width, this->label_creatorName->font()));
            if(this->label_creatorName->text().contains("…")){
                this->label_creatorName->setToolTip(curator);
                this->label_creatorName->setToolTipDuration(2000);
            }
            this->label_creatorName->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_creatorName->setText(curator);
            this->label_creatorName->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        if(!lastModifiedDate.isEmpty()){

            QString tmp_split = "";
            QStringList splitToken;

            tmp_split = lastModifiedDate;
            splitToken = tmp_split.split("T");

            QDate qDatelastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

            if(global.lang == 0){
                QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
                str_resol += "| Last updated " + QLocale().toString(qDatelastupdated, "MMM d, yyyy");

                QLocale::setDefault(QLocale::system());
            }
            else if(global.lang == 1){
                str_resol += QString("| Last updated %1").arg(qDatelastupdated.toString("M월 d일, yyyy"));
            }
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!description.isEmpty()){
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(description);

            QLabel *tmp_html = new QLabel();
            tmp_html->setTextFormat(Qt::RichText);
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the playlist."));
        }

        this->setOpen_Height(height, flag);
        this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 40);
        this->label_open->setGeometry(0, 37, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void AbstractImageDetailContents_RHV::setData_fromCDplayData(const QJsonObject &jsonObj){//c220724

    if(this->curr_contentsType == CDPlay_album){

        QString img_path = ProcJsonEasy::getString(jsonObj, "mImageUrl");
        QString artist = ProcJsonEasy::getString(jsonObj, "mArtist");
        QString title = ProcJsonEasy::getString(jsonObj, "mTitle");
        QString info = ProcJsonEasy::getString(jsonObj, "mInfo");

        CommonGetRedirectUrl *redirectUrl = new CommonGetRedirectUrl("CD", img_path);   //j220903 twitter
        connect(redirectUrl, &CommonGetRedirectUrl::signal_redirectUrl, this, &AbstractImageDetailContents_RHV::slot_redirectUrl);  //j220903 twitter

        QJsonArray tracks = ProcJsonEasy::getJsonArray(jsonObj, "mArr");
        int track_count = tracks.count();

        int duration = 0;
        int hour = 0;
        int min = 0;
        int sec = 0;

        for(int i = 0; i < track_count; i++){
            QString tmpSplit= ProcJsonEasy::getString(tracks[i].toObject(), "mRunningTime");

            QStringList tmpDur = tmpSplit.split(":");

            if(tmpDur.count() == 1){
                sec += tmpDur[0].toDouble();
            }
            else if(tmpDur.count() == 2){
                min += tmpDur[0].toInt() * 60;
                sec += tmpDur[1].toDouble();
            }
            else if(tmpDur.count() == 3){
                hour += tmpDur[1].toInt() * 3600;
                min += tmpDur[1].toInt() * 60;
                sec += tmpDur[2].toDouble();
            }
        }

        duration = hour + min + sec;

        QString mRecording = "";

        QString tmpRecording = ProcJsonEasy::getString(jsonObj, "mRecording");

        QStringList tmpSplit = tmpRecording.split(",");

        if(tmpSplit.count() > 1){
            QString channel = "";
            QString bits = "";
            QString freq = "";

            for(int i = 0; i < tmpSplit.count(); i++){
                QStringList tmpStr = tmpSplit[i].split(":");

                if(tmpStr[0].trimmed() == "channel"){
                    if(tmpStr[1] =="stereo"){
                        channel = "2ch ";
                    }
                }
                else if(tmpStr[0].trimmed() == "bits"){
                    bits = QString("%1bits ").arg(tmpStr[1]);
                }
                else if(tmpStr[0].trimmed() == "rate"){
                    double rate = tmpStr[1].toDouble() / 1000;
                    freq = QString(" %1khz ").arg(rate);
                }
            }

            mRecording = bits + freq + channel;
        }

        this->label_artist->setVisible(true);
        this->label_creatorName->setVisible(false);

        //this->label_mInfo->setVisible(true);

        this->setImage(img_path);

        this->label_title->setText(title);
        //this->label_mInfo->setText(info);

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        if(artist_width > 750){

            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_artist_line1 = "";

            tmp_split = artist;
            splitToken = tmp_split.split(" ");

            tmp_artist->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_artist_line1 = splitToken.at(i);
                    }
                    else{
                        tmp_artist_line1 += " " + splitToken.at(i);
                    }
                    tmp_artist->setText(tmp_artist_line1);

                    if(tmp_artist->sizeHint().width() > 700){
                        tmp_artist->setText("");
                        tmp_artist_line1.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_artist->setText("");
            tmp_artist->setText(tmp_artist_line1);

            artist_width = tmp_artist->sizeHint().width() + 700;

            this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, artist_width, this->label_artist->font()));
            if(this->label_artist->text().contains("…")){
                this->label_artist->setToolTip(artist);
                this->label_artist->setToolTipDuration(2000);
            }
            this->label_artist->setGeometry(450, 140, 700, 80);
        }
        else{
            this->label_artist->setText(artist);
            this->label_artist->setGeometry(450, 175, 700, 40);
        }

        QString str_resol = "";
        if(global.lang == 0){
            if(track_count > 1){
                str_resol = QString("%1 tracks ").arg(track_count);
            }
            else{
                str_resol = QString("%1 track ").arg(track_count);
            }
        }
        else if(global.lang == 1){
            str_resol = QString("총 %1개 트랙 ").arg(track_count);
        }

        if(duration == 0){

        }
        else if(duration >= 3600){
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
        }
        else{
            str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
        }

        if(!mRecording.isEmpty()){
            str_resol += "| " + mRecording;
        }

        this->label_resolution->setText(str_resol);

        QString setHTML = "";
        int height = 0;
        bool flag = false;
        if(!info.isEmpty()){
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(info);

            QLabel *tmp_html = new QLabel();
            tmp_html->setTextFormat(Qt::RichText);
            tmp_html->setWordWrap(true);
            tmp_html->setFixedWidth(1450);
            tmp_html->setText(setHTML);

            height = tmp_html->sizeHint().height();
            flag = true;
        }
        else{
            setHTML = QString("<html><head/><body><span style='font-size:16px; font-weight:normal; color:#CCCCCC;'>%1</span></body></html>").arg(tr("There is no description for the album."));
        }

        this->setOpen_Height(height, flag);
        this->label_description->setTextFormat(Qt::RichText);
        this->label_description->setText(setHTML);

        this->widget_Addbtn_Open->setFixedSize(1500, 80);
        this->label_description->setGeometry(0, 0, 1450, 48);
        this->label_open->setGeometry(0, 55, 1450, 30);
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();
    }
}


void AbstractImageDetailContents_RHV::setData_fromVideoData(const QJsonObject &jsonObj){

    QString img_path = ProcJsonEasy::getString(jsonObj, "image");
    QString title = ProcJsonEasy::getString(jsonObj, "title");
    int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
    int duration = ProcJsonEasy::getInt(jsonObj, "duration");

    // 데이터를 UI에 세팅한다.
    QString image_type = ":/images/rosehome/home_videox2.png";

    this->flag_type_image = true;
    this->type_image_path = image_type;
    this->setImage(img_path);

    this->label_title->setText(title);

    QString str_resol = "";
    if(global.lang == 0){
        if(track_count > 1){
            str_resol = QString("%1 tracks ").arg(track_count);
        }
        else{
            str_resol = QString("%1 track ").arg(track_count);
        }
    }
    else if(global.lang == 1){
        str_resol = QString("총 %1개 트랙 ").arg(track_count);
    }

    if(duration == 0){

    }
    else if(duration >= 3600){
        str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }

    this->label_resolution->setText(str_resol);
}


void AbstractImageDetailContents_RHV::setData_Resolution(const QJsonObject &jsonObj){

    int track_count = ProcJsonEasy::getInt(jsonObj, "track_count");
    int duration = ProcJsonEasy::getInt(jsonObj, "duration");
    QString registDateTime = ProcJsonEasy::getString(jsonObj, "registDateTime");
    QString owner = ProcJsonEasy::getString(jsonObj, "owner");

    if(this->label_creatorName->text().isEmpty()){

        if(this->curr_contentsType == rose_album){
            QLabel *tmp_artist = new QLabel();
            tmp_artist->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
            tmp_artist->setText(owner);

            int artist_width = 0;
            artist_width = tmp_artist->sizeHint().width();

            if(artist_width > 750){

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_artist_line1 = "";

                tmp_split = owner;
                splitToken = tmp_split.split(" ");

                tmp_artist->setText("");
                int i = 0;
                if(splitToken.size() > 1){

                    for(i = 0; i < splitToken.count(); i++){
                        if(i == 0){
                            tmp_artist_line1 = splitToken.at(i);
                        }
                        else{
                            tmp_artist_line1 += " " + splitToken.at(i);
                        }
                        tmp_artist->setText(tmp_artist_line1);

                        if(tmp_artist->sizeHint().width() > 700){
                            tmp_artist->setText("");
                            tmp_artist_line1.replace(splitToken.at(i), "");
                            break;
                        }
                    }
                }

                tmp_artist->setText("");
                tmp_artist->setText(tmp_artist_line1);

                artist_width = tmp_artist->sizeHint().width() + 700;

                this->label_artist->setText(GSCommon::getTextCutFromLabelWidth(owner, artist_width, this->label_artist->font()));
                if(this->label_artist->text().contains("…")){
                    this->label_artist->setToolTip(owner);
                    this->label_artist->setToolTipDuration(2000);
                }
                this->label_artist->setGeometry(450, 140, 700, 80);
            }
            else{
                this->label_artist->setText(owner);
                this->label_artist->setGeometry(450, 175, 700, 40);
            }
        }
        else{
            QLabel *tmp_creator = new QLabel();
            tmp_creator->setStyleSheet("background-color:transparent;color:#B18658;font-size:25px;font-weight:300;");
            tmp_creator->setText(owner);

            int creator_width = 0;
            creator_width = tmp_creator->sizeHint().width();

            if(creator_width > 750){

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_creator_line1 = "";

                tmp_split = owner;
                splitToken = tmp_split.split(" ");

                tmp_creator->setText("");
                int i = 0;
                if(splitToken.size() > 1){

                    for(i = 0; i < splitToken.count(); i++){
                        if(i == 0){
                            tmp_creator_line1 = splitToken.at(i);
                        }
                        else{
                            tmp_creator_line1 += " " + splitToken.at(i);
                        }
                        tmp_creator->setText(tmp_creator_line1);

                        if(tmp_creator->sizeHint().width() > 700){
                            tmp_creator->setText("");
                            tmp_creator_line1.replace(splitToken.at(i), "");
                            break;
                        }
                    }
                }

                tmp_creator->setText("");
                tmp_creator->setText(tmp_creator_line1);

                creator_width = tmp_creator->sizeHint().width() + 700;

                this->label_creatorName->setText(GSCommon::getTextCutFromLabelWidth(owner, creator_width, this->label_creatorName->font()));
                if(this->label_creatorName->text().contains("…")){
                    this->label_creatorName->setToolTip(owner);
                    this->label_creatorName->setToolTipDuration(2000);
                }
                this->label_creatorName->setGeometry(450, 140, 700, 80);
            }
            else{
                this->label_creatorName->setText(owner);
                this->label_creatorName->setGeometry(450, 175, 700, 40);
            }
        }
    }

    QString str_resol = "";
    if(global.lang == 0){
        if(track_count > 1){
            str_resol = QString("%1 tracks ").arg(track_count);
        }
        else{
            str_resol = QString("%1 track ").arg(track_count);
        }
    }
    else if(global.lang == 1){
        str_resol = QString("총 %1개 트랙 ").arg(track_count);
    }

    if(duration == 0){

    }
    else if(duration >= 3600){
        str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        str_resol += QString("· %1 ").arg(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }

    if(!registDateTime.isEmpty()){
        QString tmp_split = "";
        QStringList splitToken;

        tmp_split = registDateTime;
        splitToken = tmp_split.split("T");

        QDate lastupdated = QDate::fromString(splitToken.at(0), "yyyy-MM-dd");

        if(global.lang == 0){
            QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
            if(this->curr_contentsType == Apple_playlist){
                str_resol += "| Last updated " + QLocale().toString(lastupdated, "MMM d, yyyy");
            }
            else{
                str_resol += "| Registration date " + QLocale().toString(lastupdated, "MMM d, yyyy");
            }

            QLocale::setDefault(QLocale::system());
        }
        else if(global.lang == 1){
            if(this->curr_contentsType == Apple_playlist){
                str_resol += QString("| Last updated %1").arg(lastupdated.toString("M월 d일, yyyy"));
            }
            else{
                str_resol += "| 등록일자 " + lastupdated.toString("M월 d일, yyyy");
            }
        }
    }

    this->label_resolution->setText(str_resol);
}


void AbstractImageDetailContents_RHV::setData_Biography(const QJsonObject &jsonObj){

    QString biography = ProcJsonEasy::getString(jsonObj, "biography");

    if(biography.length() < 173){
        this->label_biography->setText(biography);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_bio_line1 = "";
        QString tmp_bio_line2 = "";
        QString tmp_bio_line3 = "";
        QString tmp_bio_total = "";

        tmp_split = biography;
        splitToken = tmp_split.split(" ");

        int i, j, k = 0;
        for (i = 0; i < splitToken.count(); i++){
            tmp_bio_line1 += splitToken.at(i) + " ";

            QLabel *tmp_bio1 = new QLabel();
            tmp_bio1->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
            tmp_bio1->setText(tmp_bio_line1);

            int bio1_width = tmp_bio1->sizeHint().width();

            if(bio1_width > 1000){
                tmp_bio_total += tmp_bio_line1.replace(splitToken.at(i) + " ", "");
                break;
            }
        }

        for (j = i; j < splitToken.count(); j++){
            tmp_bio_line2 += splitToken.at(j) + " ";

            QLabel *tmp_bio2 = new QLabel();
            tmp_bio2->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
            tmp_bio2->setText(tmp_bio_line2);

            int bio2_width = tmp_bio2->sizeHint().width();

            if(bio2_width > 1000){
                tmp_bio_total += tmp_bio_line2.replace(splitToken.at(j) + " ", "");
                break;
            }
        }

        for (k = j; j < splitToken.count(); k++){
            tmp_bio_line3 += splitToken.at(k) + " ";

            QLabel *tmp_bio3 = new QLabel();
            tmp_bio3->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
            tmp_bio3->setText(tmp_bio_line3);

            int bio3_width = tmp_bio3->sizeHint().width();

            if(bio3_width > 1000){
                tmp_bio_total += tmp_bio_line3.replace(splitToken.at(k) + " ", "");
                break;
            }
        }

        QLabel *set_bio_total = new QLabel();
        set_bio_total->setStyleSheet("background-color:transparent;color:#e6e6e6;font-size:25px;");
        set_bio_total->setText(tmp_bio_total);

        int bio_total_width = set_bio_total->sizeHint().width();

        this->label_biography->setText(GSCommon::getTextCutFromLabelWidth(tmp_bio_total, bio_total_width, this->label_biography->font()));
        if(this->label_biography->text().contains("…")){
            this->label_biography->setToolTip(tmp_bio_total);
            this->label_biography->setToolTipDuration(2000);
        }
    }
}


void AbstractImageDetailContents_RHV::setFavorite(bool flagFavorite, int star){

    // Data
    this->flagFavorite = flagFavorite;

    if((this->curr_contentsType != Music_userplaylist) && (this->curr_contentsType != rose_userplaylist) && (this->curr_contentsType != Rosetube_userplaylist)){
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
    else if((this->curr_contentsType == Music_userplaylist) || (this->curr_contentsType == rose_userplaylist) || (this->curr_contentsType == Rosetube_userplaylist)){
        // UI
        if(flagFavorite == true){
            this->btn_thumb_toAdd->setVisible(false);           // thumbup off
            this->btn_thumb_toDelete->setVisible(true);         // thumbup on
        }
        else{
            this->btn_thumb_toAdd->setVisible(true);            // thumbup off
            this->btn_thumb_toDelete->setVisible(false);        // thumbup on
        }
    }

    this->myFavoriteIds = flagFavorite;
    this->myStarIds = star;
}


/**
 * @brief AbstractImageDetailInfo::setPossiblePlay
 * @param flagCanPlay
 */
void AbstractImageDetailContents_RHV::setPossiblePlay(bool flagCanPlay){

    if(flagCanPlay){
        this->widget_Addbtn_Play->setVisible(true);
        this->widget_Addbtn_PlayCannot->setVisible(false);
    }
    else{
        this->widget_Addbtn_Play->setVisible(false);
        this->widget_Addbtn_PlayCannot->setVisible(true);
    }
}


//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : [Slot]
//
//-----------------------------------------------------------------------------------------------------------------------


/**
 * @brief PlayListDetail::loadImage [SLOT] 앨범 이미지 세팅
 */
void AbstractImageDetailContents_RHV::slot_fileDownload_loadImage()
{
    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(this->image_width, this->image_height));
        pixmapIMG.fill(Qt::transparent);
        /*if(this->curr_contentsType == Music_playlist || this->curr_contentsType == Music_userplaylist  || this->curr_contentsType == rose_playlist
                || this->curr_contentsType == rose_userplaylist || this->curr_contentsType == Rosetube_userplaylist || this->curr_contentsType == Tidal_video
                || this->curr_contentsType == Bugs_video || this->curr_contentsType == Qobuz_playlist || this->curr_contentsType == Video){
            pixmapIMG.fill(Qt::black);
        }
        else{
            pixmapIMG.fill(Qt::transparent);
        }*/

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        /*if(this->curr_contentsType == Music_playlist || this->curr_contentsType == Music_userplaylist  || this->curr_contentsType == rose_playlist
                || this->curr_contentsType == rose_userplaylist || this->curr_contentsType == Rosetube_userplaylist || this->curr_contentsType == Tidal_video
                || this->curr_contentsType == Bugs_video || this->curr_contentsType == Qobuz_playlist || this->curr_contentsType == Video){
            tmp_pixmap = tmp_pixmap.scaled(this->image_width, this->image_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else{
            if(this->image_width > tmp_pixmap.width() && this->image_height <= tmp_pixmap.height()){
                pixmapIMG.fill(Qt::black);
                tmp_pixmap = tmp_pixmap.scaled(tmp_pixmap.width(), this->image_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
            else if(this->image_width <= tmp_pixmap.width() && this->image_height > tmp_pixmap.height()){
                pixmapIMG.fill(Qt::black);
                tmp_pixmap = tmp_pixmap.scaled(this->image_width, tmp_pixmap.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
            else{
                tmp_pixmap = tmp_pixmap.scaled(this->image_width, this->image_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
        }*/

        if(this->curr_contentsType == rose_artist || this->curr_contentsType == Music_artist || this->curr_contentsType == Tidal_artist || this->curr_contentsType == Bugs_artist || this->curr_contentsType == Qobuz_artist || this->curr_contentsType == Apple_artist){
            tmp_pixmap = tmp_pixmap.scaled(this->image_width, this->image_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        }
        else{
            if(tmp_pixmap.width() > tmp_pixmap.height()){
                tmp_pixmap = tmp_pixmap.scaledToWidth(this->image_width, Qt::SmoothTransformation);
                pixmapIMG.fill(Qt::black);
            }
            else if(tmp_pixmap.height() > tmp_pixmap.width()){
                tmp_pixmap = tmp_pixmap.scaledToHeight(this->image_height, Qt::SmoothTransformation);
                pixmapIMG.fill(Qt::black);
            }
            else{
                tmp_pixmap = tmp_pixmap.scaled(this->image_width, this->image_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
        }

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, this->image_width, this->image_height, this->image_cornerRadius, this->image_cornerRadius);

        int leftValue = (this->image_width - tmp_pixmap.width()) / 2;
        int topValue = (this->image_height - tmp_pixmap.height()) / 2;

        painter.setClipPath(path);
        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->label_imageBig->setPixmap(pixmapIMG);

        if(this->flag_type_image){
            QImage img;
            QPixmap *img_type = new QPixmap();
            if(img.load(this->type_image_path)){
                *img_type = QPixmap::fromImage(img);
                *img_type = img_type->scaled(img.width(), img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }

            this->label_imageType->setPixmap(*img_type);
            this->label_imageType->setFixedSize(img.width(), img.height());
            this->label_imageType->setGeometry(0, 0, 0, 0);
        }
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }
}


/**
 * @brief pixmap 을 label_imageBig 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
 * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
 * @param pixmap
 */
void AbstractImageDetailContents_RHV::paint_imageBig(QPixmap &pixmap){

    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(this->image_width, this->image_height));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, this->image_width, this->image_height, this->image_cornerRadius, this->image_cornerRadius);
    painter.end();

    this->label_imageBig->setPixmap(pixmap_painter);

    if(this->flag_type_image){
        QImage img;
        QPixmap *img_type = new QPixmap();
        if(img.load(this->type_image_path)){
            *img_type = QPixmap::fromImage(img);
            *img_type = img_type->scaled(img.width(), img.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        this->label_imageType->setPixmap(*img_type);
        this->label_imageType->setFixedSize(img.width(), img.height());
        this->label_imageType->setGeometry(0, 0, 0, 0);
    }
}


void AbstractImageDetailContents_RHV::downloadThumbImageBig(QString pathImg){//c230215

    FileDownloader *fileDownloader = new FileDownloader(this);
    fileDownloader->setProperty("filedown_name", "bigImg");
    fileDownloader->setImageURL(pathImg);

    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_downloadThumbImageBig()));
}


void AbstractImageDetailContents_RHV::slot_downloadThumbImageBig(){//c230215

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    QString tmp_name = fileDownloader->property("filedown_name").toString();
    print_debug();
    if(tmp_name == "bigImg"){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

        if(flagLoaded){
            DialogConfirm *dlgConfirmOutput = new DialogConfirm(this);
            QPixmap tmp_pixmap = tmp_pixmap.fromImage(image);

            qDebug() << "tmp_pixmap.width()=" << tmp_pixmap.width();
            qDebug() << "tmp_pixmap.height()=" << tmp_pixmap.height();

            QWidget *topLevelWidget = this->window();       // 최상위 부모 윈도우를 가져옴
            QSize size = topLevelWidget->size();            // 윈도우의 크기를 가져옴
            int window_width = size.width();                // 윈도우의 가로 크기를 가져옴
            int window_height = size.height();              // 윈도우의 세로 크기를 가져옴

            int tmp_length;
            if(window_width > window_height){
                tmp_length = window_height;
            }else{
                tmp_length = window_width;
            }

            if(tmp_pixmap.width() != tmp_pixmap.height()){

                double tmp_pixmap_width = tmp_pixmap.width();
                double tmp_pixmap_height = tmp_pixmap.height();

                if(tmp_pixmap_width >= tmp_pixmap_height){
                    double k = tmp_pixmap_height/tmp_pixmap_width;
                    this->bigImgWidth = tmp_length*0.8;
                    this->bigImgHeight = (tmp_length*0.8)*k;
                }
                else{
                    double k = tmp_pixmap_width/tmp_pixmap_height;
                    this->bigImgWidth = (tmp_length*0.8)*k;
                    this->bigImgHeight = tmp_length*0.8;
                }
            }else{

                this->bigImgWidth = tmp_length*0.7;
                this->bigImgHeight = tmp_length*0.7;
            }
            tmp_pixmap = tmp_pixmap.scaled(this->bigImgWidth, this->bigImgHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);


            /*if(800 <= tmp_pixmap.width() || 800 <= tmp_pixmap.height()){
                this->bigImgWidth = tmp_pixmap.width();
                this->bigImgHeight = tmp_pixmap.height();
                tmp_pixmap = tmp_pixmap.scaled(tmp_pixmap.width(), tmp_pixmap.height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
            else{
                this->bigImgWidth = tmp_pixmap.width()*1.5;
                this->bigImgHeight = tmp_pixmap.height()*1.5;
                tmp_pixmap = tmp_pixmap.scaled(this->bigImgWidth, this->bigImgHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }*/

            int left = global.left_mainwindow + ((global.width_mainwindow - (this->bigImgWidth + 50)) / 2);
            int top = global.top_mainwindow + ((global.height_mainwindow - (this->bigImgHeight + 150)) / 2);

            dlgConfirmOutput->setAlignment(Qt::AlignLeft);
            dlgConfirmOutput->setTitle(tr("Image View"));
            dlgConfirmOutput->setText(tr(""));
            //dlgConfirmOutput->setTextHeight(250);
            dlgConfirmOutput->setFixedWidth(this->bigImgWidth+50);
            dlgConfirmOutput->setAlignment(Qt::AlignLeft);
            dlgConfirmOutput->setGeometry(left, top, this->bigImgWidth + 50, this->bigImgHeight + 150);

            dlgConfirmOutput->setAlertMode();
            dlgConfirmOutput->setProperty("flagShown",false);


            QLabel *lb_BigImg = new QLabel;
            lb_BigImg->setFixedSize(this->bigImgWidth,this->bigImgHeight);
            lb_BigImg->setPixmap(tmp_pixmap);

            qDebug() << "this->bigImgWidth=" << this->bigImgWidth;
            qDebug() << "this->bigImgHeight=" << this->bigImgHeight;

            QHBoxLayout *lh_PI_Info = new QHBoxLayout;
            lh_PI_Info->setSpacing(30);
            lh_PI_Info->setContentsMargins(0,0,0,20);
            lh_PI_Info->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
            lh_PI_Info->addWidget(lb_BigImg);
            dlgConfirmOutput->setLayoutMy(lh_PI_Info);

            if(dlgConfirmOutput->property("flagShown").toBool()==false){
                dlgConfirmOutput->setProperty("flagShown",true);

                int result = dlgConfirmOutput->exec();

                if(result != QDialog::Rejected){
                    print_debug();
                    //dlgConfirmOutput->close();
                    //delete lb_BigImg;
                }
            }
        }
        else{
            //print_debug();
        }

    }

    fileDownloader->deleteLater();
}


void AbstractImageDetailContents_RHV::slot_imageClick(){//c230215

    QPushButton *tmp_clickableImg = dynamic_cast<QPushButton *>(sender());
    qDebug() << "imagePath = " << tmp_clickableImg->property("imagePath").toString();
    QString imagePath = tmp_clickableImg->property("imagePath").toString();

    downloadThumbImageBig(imagePath);
    //sender()->deleteLater();
}


/**
 * @brief [Slot] 버튼 클릭에 대한 처리. 시그널 발생
 */
void AbstractImageDetailContents_RHV::slot_btnClicked_mode(){

    BtnClickMode clickMode = sender()->property("mode").value<BtnClickMode>();
    emit this->signal_clicked(clickMode);
}


void AbstractImageDetailContents_RHV::slot_btnClicked_open(){

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
        if(this->curr_contentsType == Bugs_album){
            this->label_description->setGeometry(0, 0, 1450, 41);
            this->label_open->setGeometry(0, 44, 1450, 30);
        }
        else{
            this->label_description->setGeometry(0, 0, 1450, 48);
            this->label_open->setGeometry(0, 55, 1450, 30);
        }
        this->btn_open->setStyleSheet(this->btn_open->styleSheet().replace("text_close_ico.png", "text_open_ico.png"));
        this->widget_Addbtn_Open->show();

        this->flagBtnOpen = false;
    }
}


void AbstractImageDetailContents_RHV::slot_btnClicked_artistMore(){

    emit this->signal_clicked_artistMore();
}


void AbstractImageDetailContents_RHV::slot_btnClicked_tag(){

    int idx = sender()->property("idx").toInt();

    emit this->signal_clicked_tag(idx);
}


void AbstractImageDetailContents_RHV::slot_redirectUrl(const QString url){  //j220903 twitter

    this->setImage(url);
}


void AbstractImageDetailContents_RHV::slot_applyResult_getShareLink(const QString &link){   //j220818 share link

    this->detailInfo_shareLink = link;
}


/**
 * @brief AbstractImageDetailInfo::setVisible_optionBtn
 * @param flagVisiable
 */
void AbstractImageDetailContents_RHV::setVisible_optionBtn(bool flagVisiable){

    this->widget_optBtn->setVisible(flagVisiable);
}


/**
 * @brief AbstractImageDetailInfo::setVisible_playBtn
 * @param flagVisiable
 */
void AbstractImageDetailContents_RHV::setVisible_playBtn(bool flagVisiable){

    this->widget_Addbtn_Play->setVisible(flagVisiable);
}


void AbstractImageDetailContents_RHV::setHiRes_Left(int left, int height){

    this->label_imageHiRes->setGeometry(450 + left + 20, height, 0, 0);

    this->label_imageHiRes->show();
}


void AbstractImageDetailContents_RHV::setOpen_Height(int height, bool flag){

    this->heightOpen = height;
    this->flagOpen = flag;
}


int AbstractImageDetailContents_RHV::getFavoritesStars(){

    return this->myStarIds;
}


/**
 * @brief AbstractImageDetailInfo::resizeEvent
 * @param event
 */
void AbstractImageDetailContents_RHV::resizeEvent(QResizeEvent *event){

    Q_UNUSED(event);

    /*if(this->elidedLabel_description->isVisible()){
        this->elidedLabel_description->setFixedWidth(event->size().width() - 350);
    }*/
}
