#include "AbstractImageDetailInfo.h"

#include <common/gscommon.h>

#include <QPainter>
#include <QPixmap>
#include <QRegularExpression>
#include <QResizeEvent>

const int DEFAULT_IMAGE_SIZE = 300;
const int BTN_MENU_SIZE = 60;           ///< MyCollect, Fav, More 아이콘 버튼의 사이즈 (정사각형 고정 사이즈)

const QString IMAGE_PATH_DEFAULT = ":/images/qobuz/artist_def_400.png";      //":/images/def_mus_300.png";

const QString ICON_PATH___addCollect = ":/images/play_list_add_icon.png";
const QString ICON_PATH___favorite_off = ":/images/list_fav_icon.png";
const QString ICON_PATH___favorite_on = ":/images/list_fav_icon_on.png";
const QString ICON_PATH___share = ":/images/rosehome/icon_share.png";//c220825_2
const QString ICON_PATH___more = ":/images/icon_menu.png";

const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";


const int DEFAULT_IMAGE_SIZEW = 384;
const int DEFAULT_IMAGE_SIZEH = 186;
const QString IMAGE_PATH_DEFAULT2 = ":/images/def_mus_384.png";

/**
 * @brief 생성자
 * @param parent
 */
AbstractImageDetailInfo::AbstractImageDetailInfo(QWidget *parent) : QWidget(parent){

    this->image_width = DEFAULT_IMAGE_SIZE;
    this->image_height = DEFAULT_IMAGE_SIZE;
    this->image_cornerRadius = 3;

    this->setUIControl_basic();
    this->setUIControl_btnAddFavMore();
    this->setUIControl_btnPlays();
    this->setUIControl_btnPlays_cannot();

    // 기본값
    this->setPossiblePlay(true);

}


/**
 * @brief 소멸자. 메모리 해제 체크위해
 */
AbstractImageDetailInfo::~AbstractImageDetailInfo(){

    this->fileDownLoader->deleteLater();
    this->deleteLater();
}

/**
 * @brief 기본 UI 구성
 */
void AbstractImageDetailInfo::setUIControl_basic(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractImageDetailInfo::slot_fileDownload_loadImage);

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(this->image_width, this->image_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Big Image (Album or Playlist Main Image)
    this->label_imageBig = new QLabel();
    this->label_imageBig->setFixedSize(this->image_width, this->image_height);

    //------------------------------------------------------------------------------------------
    this->label_artistInfo = new QPushButton();//cheonartistmoreinfo
    this->label_artistInfo->setFixedSize(296,30);//cheonartistmoreinfo
    this->label_artistInfo->setText(tr(""));//cheonartistmoreinfo

   // this->label_artistInfo->setCursor(Qt::PointingHandCursor);//cheonartistmoreinfo
    this->label_artistInfo->setStyleSheet("font-size:16px;color:#FFFFFF;");//cheonartistmoreinfo

    // Title, Description, CreatorName
    this->label_title = new QLabel();
    this->label_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->label_title->setStyleSheet("color:#FFFFFF; font-size:30px;");
    this->label_title->setContentsMargins(30,0,0,0);

//    QFont font_20;
//    font_20.setPixelSize(20);
    this->elidedLabel_description = new ElidedLabel("");
    this->elidedLabel_description->setStyleSheet("color:#E6E6E6; font-size:20px;");
    this->elidedLabel_description->setFrameStyle(QFrame::Box);
    this->elidedLabel_description->setContentsMargins(30,10,0,0);
    this->elidedLabel_description->setMaximumHeight(66);
//    this->elidedLabel_description->setMinimumHeight(46);
//    this->elidedLabel_description->setFont(font_20);


    this->label_creatorName = new QLabel();
    this->label_creatorName->setStyleSheet("color:#b18658; font-size:16px;");
    this->label_creatorName->setContentsMargins(30,10,0,0);             // show/hide 로 spacing을 여기에 넣음 (not in Layout)

    this->label_artist = new QLabel();
    this->label_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->label_artist->setStyleSheet("color:#E6E6E6; font-size:20px;");
    this->label_artist->setContentsMargins(30,10,0,0);                  // show/hide 로 spacing을 여기에 넣음 (not in Layout)

    this->widget_spaceWithTextAndBtn = new QWidget();
    this->widget_spaceWithTextAndBtn->setContentsMargins(0,0,0,0);
    this->widget_spaceWithTextAndBtn->setFixedHeight(10);

    this->label_hires = new QLabel();//cheontidalhires
    this->label_hires->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_hires->setContentsMargins(30,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_hires->setText(tr(""));//cheontidalhires


    this->label_hires_img = new QLabel();//cheon0812-hires
    this->label_hires_img->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon0812-hires
    //this->label_hires_img->setContentsMargins(30,0,0,0);  //cheon0812-hires
    //QPixmap pic(":images/qobuz/hires_ico.png"); //cheon0812-hires
    //this->label_hires_img->setPixmap(pic); // 해당 이미지를 라벨에 넣어줌//cheon0812-hires

    this->label_genre = new QLabel();//cheon210606
    this->label_genre->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheon210606
    this->label_genre->setContentsMargins(30,10,0,0);   //cheon210606               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_genre->setText(tr(""));//cheon210606

    this->label_popularity = new QLabel();//cheontidalhires
    this->label_popularity->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_popularity->setContentsMargins(30,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_popularity->setText(tr(""));//cheontidalhires

    this->label_streamStartDate = new QLabel();//cheontidalhires
    this->label_streamStartDate->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_streamStartDate->setContentsMargins(0,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)
    this->label_streamStartDate->setText(tr(""));//cheontidalhires

    this->label_numberOfTracks = new QLabel();//cheon210604
    this->label_numberOfTracks->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_numberOfTracks->setContentsMargins(30,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_numberOfTracks->setText(tr(""));//cheon210604

    this->label_descr = new QLabel();//cheon210604
    this->label_descr->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_descr->setContentsMargins(30,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_descr->setText(tr(""));//cheon210604

    this->label_upd_dt = new QLabel();//cheon210604
    this->label_upd_dt->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_upd_dt->setContentsMargins(30,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_upd_dt->setText(tr(""));//cheon210604

    this->label_track_count = new QLabel();//cheon210604
    this->label_track_count->setStyleSheet("color:#E6E6E6; font-size:16px;");//cheon210604
    this->label_track_count->setContentsMargins(30,0,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_track_count->setText(tr(""));//cheon210604

    //this->label_profile_url = new QLabel();//cheon210606
    //this->label_profile_url->setText("<a href=\"whatever\">text</a>");//cheon210606
    //this->label_profile_url->setTextInteractionFlags(Qt::TextBrowserInteraction);//cheon210606

    QHBoxLayout *hl_lb_MQA = new QHBoxLayout();//cheontidalhires
    hl_lb_MQA->setContentsMargins(33,0,0,0);//cheontidalhires

    hl_lb_MQA->addWidget(this->label_streamStartDate);//cheontidalhires
    hl_lb_MQA->addWidget(this->label_genre);//cheon210606
    hl_lb_MQA->addWidget(this->label_popularity);//cheontidalhires
    hl_lb_MQA->addWidget(this->label_hires);//cheontidalhires
    hl_lb_MQA->addWidget(this->label_hires_img);//cheon0812-hires

    hl_lb_MQA->addWidget(this->label_numberOfTracks);//cheon210604

    QHBoxLayout *hl_lb_deescr = new QHBoxLayout();//cheon210604
    hl_lb_deescr->setContentsMargins(33,0,0,0);//cheon210604
    hl_lb_deescr->addWidget(this->label_descr);//cheon210604
    hl_lb_deescr->addWidget(this->label_upd_dt);//cheon210604
    hl_lb_deescr->addWidget(this->label_track_count);//cheon210604
    //hl_lb_deescr->addWidget(this->label_profile_url);//cheon210606

    QWidget *widget_total_MQA = new QWidget();//cheon210604
    widget_total_MQA->setObjectName("widget_total");//cheon210604
    widget_total_MQA->setStyleSheet("#widget_total {font-size:7px;color:#aaaaaa; } ");//cheon210604
    widget_total_MQA->setLayout(hl_lb_MQA);//cheon210604

    QWidget *widget_total_descr = new QWidget();//cheon210604
    widget_total_descr->setObjectName("widget_total");//cheon210604
    widget_total_descr->setStyleSheet("#widget_total {font-size:7px;color:#aaaaaa; } ");//cheon210604
    widget_total_descr->setLayout(hl_lb_deescr);//cheon210604

    // layout
    this->vBox_albumInfo = new QVBoxLayout();
    this->vBox_albumInfo->setContentsMargins(0,0,0,0);
    this->vBox_albumInfo->setSpacing(0);
    this->vBox_albumInfo->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    this->vBox_albumInfo->addWidget(this->label_title, 1, Qt::AlignLeft | Qt::AlignBottom);

    this->vBox_albumInfo->addWidget(this->elidedLabel_description, 0, Qt::AlignLeft);
    this->vBox_albumInfo->addWidget(this->label_artistInfo, 0, Qt::AlignLeft | Qt::AlignBottom);//cheonartist
    this->vBox_albumInfo->addWidget(this->label_creatorName, 0, Qt::AlignLeft | Qt::AlignBottom);
    this->vBox_albumInfo->addWidget(this->label_artist, 0, Qt::AlignLeft | Qt::AlignBottom);

    this->vBox_albumInfo->addWidget(widget_total_MQA, 0, Qt::AlignLeft | Qt::AlignBottom);//cheontidalhires
    this->vBox_albumInfo->addWidget(widget_total_descr, 0, Qt::AlignLeft | Qt::AlignBottom);//cheon210604
    this->vBox_albumInfo->addWidget(this->widget_spaceWithTextAndBtn, 0, Qt::AlignLeft | Qt::AlignBottom);

    QWidget *tmp_widget_albumInfo = new QWidget();
    tmp_widget_albumInfo->setLayout(this->vBox_albumInfo);
    tmp_widget_albumInfo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    //------------------------------------------------------------------------------------------

    // 전체
    QHBoxLayout *hBox_total = new QHBoxLayout();
    hBox_total->setContentsMargins(0,0,33,0);           // 왼쪽에 여백 넣음
    hBox_total->setSpacing(0);
    hBox_total->addWidget(this->label_imageBig, 0, Qt::AlignBottom);
    hBox_total->addSpacing(10);
    hBox_total->addWidget(tmp_widget_albumInfo, 1, Qt::AlignBottom);

    this->setLayout(hBox_total);

    setUIArtist_detailInfoPop();//cheonartist

}


void AbstractImageDetailInfo::setUIArtist_detailInfoPop()//cheonartist
{
    this->label_artText = new QLabel();//cheonartist
    this->label_artText->setStyleSheet("color:#FFFFFF; font-size:24px;border-bottom:1px solid #777777;");
    this->label_artText->setContentsMargins(30,0,0,0);
    //  this->label_artText->setFixedWidth(200);
    this->label_artText->setFixedHeight(50);
    this->label_artText->setText(tr("Detail Info"));//cheon210603

    this->textEdit = new QTextEdit;
    this->textEdit->setReadOnly(true);
    this->textEdit->setContentsMargins(15,10,15,0);
    this->textEdit->setMinimumHeight(400);
    this->textEdit->setStyleSheet("color:white;font-size:18px;background-color:#333333;border-bottom:1px solid #777777;");
    this->textEdit->setText(tr("Sorry.\nThere is no artist information."));//cheonartist
    //this->textEdit->setTextColor( QColor( "red" ) );
    //textEdit->setGeometry(QRect(10, 10, 200, 200));


    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));//btn_cancel->setText(tr("취소"));
    btn_cancel->setCursor(Qt::PointingHandCursor);
    btn_cancel->setStyleSheet("font-size:18px;color:#CCCCCC;border:1px solid #777777;border-radius:10px;");
    btn_cancel->setFixedSize(120,30);
    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->setContentsMargins(0,0,0,15);
    hl_btn->setSpacing(0);
    hl_btn->setAlignment(Qt::AlignCenter);
    hl_btn->addWidget(btn_cancel);


    QVBoxLayout *vl_total;
    vl_total= new QVBoxLayout();
    vl_total->setContentsMargins(5,10,5,0);
    //this->vl_total->setSpacing(0);
    //this->vl_total->setAlignment(Qt::AlignTop);
    vl_total->addSpacing(0);
    vl_total->addWidget(label_artText);

    vl_total->addSpacing(10);
    vl_total->addWidget(textEdit);
    vl_total->addSpacing(20);
    vl_total->addLayout(hl_btn);
    //this->vl_total->addWidget(this->scrollArea);
    //this->vl_total->addWidget(textEdit);

    //dialog->move(dialog->pos().x(), (global.height_mainwindow - dialog->height() - 100));
    this->sample = new QWidget(this);
    this->sample->setMinimumWidth(800);
    this->sample->setGeometry(600, 300, 200, 200);

    this->sample->setStyleSheet("background-color:#333333;");
    if(this->sample->isMinimized()) {
        this->sample->showNormal();
    }
    this->sample->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
   // this->sample->setWindowModality(Qt::NonModaless);
    //connect(label_artistInfo, SIGNAL(clicked()), this, SLOT(slot_clickedArtist_detailInfo()));//cheonartist
    connect(this->label_artistInfo, SIGNAL(clicked()), this, SLOT(slot_show_pushButton_clicked()));//cheonartist
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(slot_hide_pushButton_clicked()));//cheonartist
    this->sample->setLayout(vl_total);
}


void AbstractImageDetailInfo::slot_hide_pushButton_clicked()//cheonartist
{

    this->sample->hide();
}

void AbstractImageDetailInfo::slot_show_pushButton_clicked()//cheonartist
{

    this->sample->hide();
    this->sample->show();
}

/**
 * @brief 버튼 UI 설정 - My Collection 추가버튼, 좋아요 하트 버튼, 더보기 메뉴 버튼
 */
void AbstractImageDetailInfo::setUIControl_btnAddFavMore(){

    // 버튼 객체 생성
    this->btn_addCollect = GSCommon::getUIBtnImg("btn_addCollect", ICON_PATH___addCollect);
    this->btn_fav_toAdd = GSCommon::getUIBtnImg("btn_fav_toAdd", ICON_PATH___favorite_off);
    this->btn_fav_toDelete = GSCommon::getUIBtnImg("btn_fav_toDelete", ICON_PATH___favorite_on);
    this->btn_share = GSCommon::getUIBtnImg("btn_share", ICON_PATH___share);    //c220818
    this->btn_more = GSCommon::getUIBtnImg("btn_more", ICON_PATH___more);


    // 버튼을 담을 레이아웃
    QHBoxLayout *hBox_btns = new QHBoxLayout();
    hBox_btns->setContentsMargins(12,0,0,0);            // 우측 여백만 설정
    hBox_btns->setSpacing(0);
    hBox_btns->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 위의 버튼들에 대해 설정 후 레이아웃에 넣기
    //c220818
    QList<QPushButton*> list_btn {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toDelete, this->btn_share, this->btn_more};
    QList<BtnClickMode> list_mode {AddCollection, Favorite_toAdd, Favorite_toDelete, Share, More};
    for(int i=0 ; i<list_btn.length() ; i++){
        list_btn.at(i)->setFixedSize(BTN_MENU_SIZE, BTN_MENU_SIZE);
        list_btn.at(i)->setCursor(Qt::PointingHandCursor);
        list_btn.at(i)->setProperty("mode", list_mode.at(i));
        hBox_btns->addWidget(list_btn.at(i));

        // connect
        connect(list_btn.at(i), &QPushButton::clicked, this, &AbstractImageDetailInfo::slot_btnClicked_mode);
    }

    this->widget_optBtn = new QWidget();
    this->widget_optBtn->setLayout(hBox_btns);
    this->widget_optBtn->setFixedHeight(70);

    // apply layout
    this->vBox_albumInfo->addWidget(this->widget_optBtn, 0, Qt::AlignBottom);

}


/**
 * @brief 버튼 UI 설정 - 전체재생, 셔플재생
 */
void AbstractImageDetailInfo::setUIControl_btnPlays(){

    //  전체재생
    QPixmap pixmap_play(ICON_PATH___playAll);
    QPushButton *btn_play = new QPushButton;
    btn_play->setIcon(pixmap_play);
    btn_play->setIconSize(pixmap_play.rect().size());
    btn_play->setText(tr("Play All"));
    //btn_play->setText(tr("전체재생"));
    btn_play->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_play->setCursor(Qt::PointingHandCursor);
    btn_play->setProperty("mode", BtnClickMode::PlayAll);
    connect(btn_play, &QPushButton::clicked, this, &AbstractImageDetailInfo::slot_btnClicked_mode);

    // 셔플재생
    QPixmap pixmap_shuffle(ICON_PATH___playShuffle);
    QPushButton *btn_shuffle = new QPushButton;
    btn_shuffle->setIcon(pixmap_shuffle);
    btn_shuffle->setIconSize(pixmap_play.rect().size());
    btn_shuffle->setText(tr("Shuffle Play"));
    //btn_shuffle->setText(tr("셔플재생"));
    btn_shuffle->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_shuffle->setCursor(Qt::PointingHandCursor);
    btn_shuffle->setProperty("mode", BtnClickMode::PlayShuffle);
    connect(btn_shuffle, &QPushButton::clicked, this, &AbstractImageDetailInfo::slot_btnClicked_mode);

    QHBoxLayout *hBox_btnPlays = new QHBoxLayout;
    hBox_btnPlays->setAlignment(Qt::AlignLeft);
    hBox_btnPlays->setSpacing(0);
    hBox_btnPlays->setContentsMargins(0,0,0,0);

    hBox_btnPlays->addSpacing(30);
    hBox_btnPlays->addWidget(btn_play);
    hBox_btnPlays->addSpacing(30);
    hBox_btnPlays->addWidget(btn_shuffle);


    this->widget_playBtns = new QWidget();
    this->widget_playBtns->setContentsMargins(0,16,0,0);
    this->widget_playBtns->setLayout(hBox_btnPlays);
    this->widget_playBtns->setFixedHeight(50);

    // apply layout
    this->vBox_albumInfo->addWidget(this->widget_playBtns, 0, Qt::AlignLeft | Qt::AlignBottom);

}


/**
 * @brief 재생불가 View
 */
void AbstractImageDetailInfo::setUIControl_btnPlays_cannot(){

    QLabel *tmp_label_lock = GSCommon::getUILabelImg(":/images/bugs/bugs-lock.png");
    tmp_label_lock->setFixedSize(30,30);

    QLabel *tmp_label_text = new QLabel();
    tmp_label_text->setText(tr("This album cannot be played at the request of the rights holder."));
    //tmp_label_text->setText(tr("이 앨범은 권리사의 요청으로 재생할 수 없습니다."));
    tmp_label_text->setStyleSheet("color:#e6e6e6; font-size:16px;");

    QHBoxLayout *hBox_btnPlays_cannot = new QHBoxLayout;
    hBox_btnPlays_cannot->setAlignment(Qt::AlignLeft);
    hBox_btnPlays_cannot->setSpacing(0);
    hBox_btnPlays_cannot->setContentsMargins(0,0,0,0);

    hBox_btnPlays_cannot->addWidget(tmp_label_lock);
    hBox_btnPlays_cannot->addWidget(tmp_label_text, 1, Qt::AlignLeft);

    this->widget_playBtns_cannot = new QWidget();
    this->widget_playBtns_cannot->setContentsMargins(30,16,0,0);
    this->widget_playBtns_cannot->setLayout(hBox_btnPlays_cannot);

    // apply layout
    this->vBox_albumInfo->addWidget(this->widget_playBtns_cannot, 0, Qt::AlignLeft);


}



/**
 * @brief 우측에 Big 이미지에 imagePath 보이도록 한다.
 * @param imagePath
 */
void AbstractImageDetailInfo::setImage(QString imagePath){
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
void AbstractImageDetailInfo::setImageSizeAndCorner(int width, int height, int cornerRadius){

    this->image_width = width;
    this->image_height = height;
    this->image_cornerRadius = cornerRadius;
}


/**
 * @brief 즐겨찾기(좋아요 하트) 관련 설정
 * @param flagFavorite
 */
void AbstractImageDetailInfo::setFavorite(bool flagFavorite){
    // Data
    this->flagFavorite = flagFavorite;

    // UI
    this->btn_fav_toDelete->setVisible(flagFavorite);
    this->btn_fav_toAdd->setVisible(!flagFavorite);
}

/**
 * @brief AbstractImageDetailInfo::setPossiblePlay
 * @param flagCanPlay
 */
void AbstractImageDetailInfo::setPossiblePlay(bool flagCanPlay){
    if(flagCanPlay){
        this->widget_playBtns->setVisible(true);
        this->widget_playBtns_cannot->setVisible(false);
    }
    else{
        this->widget_playBtns->setVisible(false);
        this->widget_playBtns_cannot->setVisible(true);
    }
}


void AbstractImageDetailInfo::initView(){
    this->setImage("");
    this->label_title->setText("");
    this->elidedLabel_description->setText("");
    this->label_creatorName->setText("");
    this->label_artist->setText("");

    this->label_hires->setText("");//cheontidalhires
    this->label_popularity->setText("");//cheontidalhires
    this->label_streamStartDate->setText("");//cheontidalhires
    this->label_numberOfTracks->setText("");//cheon210604
    this->btn_addCollect->setVisible(true);
    this->btn_more->setVisible(true);
    this->setFavorite(false);
}

/**
 * @brief AbstractImageDetailInfo::applyFlagMyData
 */
void AbstractImageDetailInfo::setMyData(bool flagMine){
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
void AbstractImageDetailInfo::slot_fileDownload_loadImage()
{
    bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(this->image_width, this->image_height, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        this->paint_imageBig(tmp_pixmap);
        this->resize(this->size());
    }
}


/**
 * @brief pixmap 을 label_imageBig 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
 * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
 * @param pixmap
 */
void AbstractImageDetailInfo::paint_imageBig(QPixmap &pixmap){
    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(this->image_width, this->image_height));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, this->image_width, this->image_height, this->image_cornerRadius, this->image_cornerRadius);
    this->label_imageBig->setPixmap(pixmap_painter);
}


/**
 * @brief [Slot] 버튼 클릭에 대한 처리. 시그널 발생
 */
void AbstractImageDetailInfo::slot_btnClicked_mode(){
    BtnClickMode clickMode = sender()->property("mode").value<BtnClickMode>();
    emit this->signal_clicked(clickMode);

    if(clickMode == BtnClickMode::Favorite_toAdd){
        this->btn_fav_toAdd->setVisible(false);
        this->btn_fav_toDelete->setVisible(true);
    }
    else if(clickMode == BtnClickMode::Favorite_toDelete){
        this->btn_fav_toAdd->setVisible(true);
        this->btn_fav_toDelete->setVisible(false);
    }
    else if(clickMode == BtnClickMode::Share){
        //c220818
        print_debug();
    }
}



/**
 * @brief AbstractImageDetailInfo::resizeEvent
 * @param event
 */
void AbstractImageDetailInfo::resizeEvent(QResizeEvent *event){
    if(this->elidedLabel_description->isVisible()){
        this->elidedLabel_description->setFixedWidth(event->size().width() - 350);
    }
}



/**
 * @brief AbstractImageDetailInfo::setTextDescription_withAdaptedWidgh
 * @param text
 */
void AbstractImageDetailInfo::setTextDescription_withAdaptedWidgh(QString text){
    this->elidedLabel_description->setText(text);
    this->elidedLabel_description->setFixedWidth(this->size().width() - 350);
    this->elidedLabel_description->setFixedHeight(66);
}



/**
 * @brief ArtistImageDetailInfo::getNormalText
 * @details @ref https://stackoverflow.com/questions/11229831/regular-expression-to-remove-html-tags-from-a-string
 *      Testing : @ref https://www.freeformatter.com/regex-tester.html - \[[^\]]*\] 로 테스트 해보면 됨.
 * @param text
 * @return
 */
QString AbstractImageDetailInfo::getNormalText(QString text){

    text.replace("[", "<");
    text.replace("]", ">");
    text = text.remove( QRegExp( "<[^>]*>" ) );

    return text;

}



/**
 * @brief AbstractImageDetailInfo::setVisible_optionBtn
 * @param flagVisiable
 */
void AbstractImageDetailInfo::setVisible_optionBtn(bool flagVisiable){
    this->widget_optBtn->setVisible(flagVisiable);
}

/**
 * @brief AbstractImageDetailInfo::setVisible_playBtn
 * @param flagVisiable
 */
void AbstractImageDetailInfo::setVisible_playBtn(bool flagVisiable){
    this->widget_playBtns->setVisible(flagVisiable);
}



/**
 * @brief 생성자
 * @param parent
 */
AbstractImageDetailInfo_forQobuz::AbstractImageDetailInfo_forQobuz(QWidget *parent) : QWidget(parent){

    this->image_width = DEFAULT_IMAGE_SIZEW;
    this->image_height = DEFAULT_IMAGE_SIZEH;
    this->image_cornerRadius = 3;

    this->setUIControl_basic();
    this->setUIControl_btnAddFavMore();
    this->setUIControl_btnPlays();
    this->setUIControl_btnPlays_cannot();

    // 기본값
    this->setPossiblePlay(true);

}


/**
 * @brief 소멸자. 메모리 해제 체크위해
 */
AbstractImageDetailInfo_forQobuz::~AbstractImageDetailInfo_forQobuz(){

    this->deleteLater();
}

/**
 * @brief 기본 UI 구성
 */
void AbstractImageDetailInfo_forQobuz::setUIControl_basic(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractImageDetailInfo_forQobuz::slot_fileDownload_loadImage);

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT2);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(this->image_width, this->image_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // Big Image (Album or Playlist Main Image)
    this->label_imageBig = new QLabel();
    this->label_imageBig->setFixedSize(this->image_width, this->image_height);

    //------------------------------------------------------------------------------------------
    this->label_artistInfo = new QPushButton();//cheonartistmoreinfo
    this->label_artistInfo->setFixedSize(296,30);//cheonartistmoreinfo
    this->label_artistInfo->setText(tr(""));//cheonartistmoreinfo

   // this->label_artistInfo->setCursor(Qt::PointingHandCursor);//cheonartistmoreinfo
    this->label_artistInfo->setStyleSheet("font-size:16px;color:#FFFFFF;");//cheonartistmoreinfo

    // Title, Description, CreatorName
    this->label_title = new QLabel();
    this->label_title->setStyleSheet("color:#FFFFFF; font-size:30px;");
    this->label_title->setContentsMargins(30,0,0,0);

//    QFont font_20;
//    font_20.setPixelSize(20);
    this->elidedLabel_description = new ElidedLabel("");
    this->elidedLabel_description->setStyleSheet("color:#E6E6E6; font-size:20px;");
    this->elidedLabel_description->setFrameStyle(QFrame::Box);
    this->elidedLabel_description->setContentsMargins(30,10,0,0);
    this->elidedLabel_description->setMaximumHeight(66);
//    this->elidedLabel_description->setMinimumHeight(46);
//    this->elidedLabel_description->setFont(font_20);


    this->label_creatorName = new QLabel();
    this->label_creatorName->setStyleSheet("color:#b18658; font-size:16px;");
    this->label_creatorName->setContentsMargins(30,10,0,0);             // show/hide 로 spacing을 여기에 넣음 (not in Layout)

    this->label_artist = new QLabel();
    this->label_artist->setStyleSheet("color:#E6E6E6; font-size:20px;");
    this->label_artist->setContentsMargins(30,10,0,0);                  // show/hide 로 spacing을 여기에 넣음 (not in Layout)


    this->label_hires = new QLabel();//cheontidalhires
    this->label_hires->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheontidalhires
    this->label_hires->setContentsMargins(30,10,0,0);   //cheontidalhires               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_hires->setText(tr(""));//cheontidalhires

    this->label_genre = new QLabel();//cheon210606
    this->label_genre->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheon210606
    this->label_genre->setContentsMargins(30,10,0,0);   //cheon210606               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_genre->setText(tr(""));//cheon210606

    this->label_popularity = new QLabel();//cheontidalhires
    this->label_popularity->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheontidalhires
    this->label_popularity->setContentsMargins(30,10,0,0);   //cheontidalhires               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_popularity->setText(tr(""));//cheontidalhires

    this->label_streamStartDate = new QLabel();//cheontidalhires
    this->label_streamStartDate->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheontidalhires
    this->label_streamStartDate->setContentsMargins(0,10,0,0);   //cheontidalhires               // show/hide 로 spacing을 여기에 넣음 (not in Layout)
    this->label_streamStartDate->setText(tr(""));//cheontidalhires

    this->label_numberOfTracks = new QLabel();//cheon210604
    this->label_numberOfTracks->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheon210604
    this->label_numberOfTracks->setContentsMargins(30,10,0,0);   //cheon210604               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    this->label_numberOfTracks->setText(tr(""));//cheon210604

    //this->label_profile_url = new QLabel();//cheon210606
    //this->label_profile_url->setText("<a href=\"whatever\">text</a>");//cheon210606
    //this->label_profile_url->setTextInteractionFlags(Qt::TextBrowserInteraction);//cheon210606
    //this->label_profile_url->setStyleSheet("color:#E6E6E6; font-size:20px;");//cheontidalhires
    //this->label_profile_url->setContentsMargins(30,10,0,0);   //cheontidalhires               // show/hide 로 spacing을 여기에 넣음 (not in Layout)//cheontidalhires
    //this->label_profile_url->setText(tr(""));//cheontidalhires

    QHBoxLayout *hl_lb_MQA = new QHBoxLayout();//cheontidalhires
    hl_lb_MQA->setContentsMargins(33,10,0,0);//cheontidalhires

    hl_lb_MQA->addWidget(this->label_streamStartDate);//cheontidalhires
    hl_lb_MQA->addWidget(this->label_genre);//cheon210606
    hl_lb_MQA->addWidget(this->label_popularity);//cheontidalhires
    hl_lb_MQA->addWidget(this->label_hires);//cheontidalhires

    hl_lb_MQA->addWidget(this->label_numberOfTracks);//cheontidalhires

    //hl_lb_MQA->addWidget(this->label_profile_url);//cheon210606

    QWidget *widget_total_MQA = new QWidget();//cheontidalhires
    widget_total_MQA->setObjectName("widget_total");//cheontidalhires
    widget_total_MQA->setStyleSheet("#widget_total {font-size:7px;color:#aaaaaa; } ");//cheontidalhires
    widget_total_MQA->setLayout(hl_lb_MQA);//cheontidalhires

    this->widget_spaceWithTextAndBtn = new QWidget();
    this->widget_spaceWithTextAndBtn->setContentsMargins(0,0,0,0);
    this->widget_spaceWithTextAndBtn->setFixedHeight(10);


    // layout
    this->vBox_albumInfo = new QVBoxLayout();
    this->vBox_albumInfo->setContentsMargins(0,0,0,0);
    this->vBox_albumInfo->setSpacing(0);
    this->vBox_albumInfo->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    this->vBox_albumInfo->addWidget(this->label_title, 1, Qt::AlignLeft | Qt::AlignBottom);
    this->vBox_albumInfo->addWidget(this->elidedLabel_description, 0, Qt::AlignLeft);
    this->vBox_albumInfo->addWidget(this->label_artistInfo, 1, Qt::AlignLeft | Qt::AlignBottom);//cheonartist
    this->vBox_albumInfo->addWidget(this->label_creatorName, 0, Qt::AlignLeft | Qt::AlignBottom);
    this->vBox_albumInfo->addWidget(this->label_artist, 0, Qt::AlignLeft | Qt::AlignBottom);
    this->vBox_albumInfo->addWidget(widget_total_MQA, 0, Qt::AlignLeft | Qt::AlignBottom);//cheontidalhires
    this->vBox_albumInfo->addWidget(this->widget_spaceWithTextAndBtn, 0, Qt::AlignLeft | Qt::AlignBottom);

    QWidget *tmp_widget_albumInfo = new QWidget();
    tmp_widget_albumInfo->setLayout(this->vBox_albumInfo);
    tmp_widget_albumInfo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    //------------------------------------------------------------------------------------------

    // 전체
    QHBoxLayout *hBox_total = new QHBoxLayout();
    hBox_total->setContentsMargins(0,0,33,0);           // 왼쪽에 여백 넣음
    hBox_total->setSpacing(0);
    hBox_total->addWidget(this->label_imageBig, 0, Qt::AlignVCenter);
    hBox_total->addSpacing(10);
    hBox_total->addWidget(tmp_widget_albumInfo, 1, Qt::AlignBottom);

    this->setLayout(hBox_total);

    setUIArtist_detailInfoPop();//cheonartist

}


void AbstractImageDetailInfo_forQobuz::setUIArtist_detailInfoPop()//cheonartist
{


    this->label_artText = new QLabel();//cheonartist
    this->label_artText->setStyleSheet("color:#FFFFFF; font-size:24px;border-bottom:1px solid #777777;");
    this->label_artText->setContentsMargins(30,0,0,0);
    //  this->label_artText->setFixedWidth(200);
    this->label_artText->setFixedHeight(50);
    this->label_artText->setText(tr("Detail Info"));//cheon2106033

    this->textEdit = new QTextEdit;
    this->textEdit->setReadOnly(true);
    this->textEdit->setContentsMargins(15,10,15,0);
    this->textEdit->setMinimumHeight(400);
    this->textEdit->setStyleSheet("color:white;font-size:18px;background-color:#333333;border-bottom:1px solid #777777;");
    this->textEdit->setText(tr("Sorry.\nThere is no artist information."));//cheonartist
    //this->textEdit->setTextColor( QColor( "red" ) );
    //textEdit->setGeometry(QRect(10, 10, 200, 200));


    QPushButton *btn_cancel = new QPushButton;
    btn_cancel->setText(tr("Cancel"));//btn_cancel->setText(tr("취소"));
    btn_cancel->setCursor(Qt::PointingHandCursor);
    btn_cancel->setStyleSheet("font-size:18px;color:#CCCCCC;border:1px solid #777777;border-radius:10px;");
    btn_cancel->setFixedSize(120,30);
    QHBoxLayout *hl_btn = new QHBoxLayout();
    hl_btn->setContentsMargins(0,0,0,15);
    hl_btn->setSpacing(0);
    hl_btn->setAlignment(Qt::AlignCenter);
    hl_btn->addWidget(btn_cancel);


    QVBoxLayout *vl_total;
    vl_total= new QVBoxLayout();
    vl_total->setContentsMargins(5,10,5,0);
    //this->vl_total->setSpacing(0);
    //this->vl_total->setAlignment(Qt::AlignTop);
    vl_total->addSpacing(0);
    vl_total->addWidget(label_artText);

    vl_total->addSpacing(10);
    vl_total->addWidget(textEdit);
    vl_total->addSpacing(20);
    vl_total->addLayout(hl_btn);
    //this->vl_total->addWidget(this->scrollArea);
    //this->vl_total->addWidget(textEdit);

    //dialog->move(dialog->pos().x(), (global.height_mainwindow - dialog->height() - 100));
    this->sample = new QWidget(this);
    this->sample->setMinimumWidth(800);
    this->sample->setGeometry(600, 300, 200, 200);

    this->sample->setStyleSheet("background-color:#333333;");
    if(this->sample->isMinimized()) {
        this->sample->showNormal();
    }
    this->sample->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
   // this->sample->setWindowModality(Qt::NonModal);
    //connect(label_artistInfo, SIGNAL(clicked()), this, SLOT(slot_clickedArtist_detailInfo()));//cheonartist
    connect(this->label_artistInfo, SIGNAL(clicked()), this, SLOT(slot_show_pushButton_clicked()));//cheonartist
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(slot_hide_pushButton_clicked()));//cheonartist
    this->sample->setLayout(vl_total);
}


void AbstractImageDetailInfo_forQobuz::slot_hide_pushButton_clicked()//cheonartist
{

    this->sample->hide();
}

void AbstractImageDetailInfo_forQobuz::slot_show_pushButton_clicked()//cheonartist
{

    this->sample->hide();
    this->sample->show();
}

/**
 * @brief 버튼 UI 설정 - My Collection 추가버튼, 좋아요 하트 버튼, 더보기 메뉴 버튼
 */
void AbstractImageDetailInfo_forQobuz::setUIControl_btnAddFavMore(){

    // 버튼 객체 생성
    this->btn_addCollect = GSCommon::getUIBtnImg("btn_addCollect", ICON_PATH___addCollect);
    this->btn_fav_toAdd = GSCommon::getUIBtnImg("btn_fav_toAdd", ICON_PATH___favorite_off);
    this->btn_fav_toDelete = GSCommon::getUIBtnImg("btn_fav_toDelete", ICON_PATH___favorite_on);
    this->btn_more = GSCommon::getUIBtnImg("btn_more", ICON_PATH___more);


    // 버튼을 담을 레이아웃
    QHBoxLayout *hBox_btns = new QHBoxLayout();
    hBox_btns->setContentsMargins(12,0,0,0);            // 우측 여백만 설정
    hBox_btns->setSpacing(0);
    hBox_btns->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // 위의 버튼들에 대해 설정 후 레이아웃에 넣기
    QList<QPushButton*> list_btn {this->btn_addCollect, this->btn_fav_toAdd, this->btn_fav_toDelete, this->btn_more};
    QList<BtnClickMode> list_mode {AddCollection, Favorite_toAdd, Favorite_toDelete, More};
    for(int i=0 ; i<list_btn.length() ; i++){
        list_btn.at(i)->setFixedSize(BTN_MENU_SIZE, BTN_MENU_SIZE);
        list_btn.at(i)->setCursor(Qt::PointingHandCursor);
        list_btn.at(i)->setProperty("mode", list_mode.at(i));
        hBox_btns->addWidget(list_btn.at(i));

        // connect
        connect(list_btn.at(i), &QPushButton::clicked, this, &AbstractImageDetailInfo_forQobuz::slot_btnClicked_mode);
    }

    this->widget_optBtn = new QWidget();
    this->widget_optBtn->setLayout(hBox_btns);
    this->widget_optBtn->setFixedHeight(70);

    // apply layout
    this->vBox_albumInfo->addWidget(this->widget_optBtn, 0, Qt::AlignBottom);

}


/**
 * @brief 버튼 UI 설정 - 전체재생, 셔플재생
 */
void AbstractImageDetailInfo_forQobuz::setUIControl_btnPlays(){

    //  전체재생
    QPixmap pixmap_play(ICON_PATH___playAll);
    QPushButton *btn_play = new QPushButton;
    btn_play->setIcon(pixmap_play);
    btn_play->setIconSize(pixmap_play.rect().size());
    btn_play->setText(tr("Play All"));
    //btn_play->setText(tr("전체재생"));
    btn_play->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_play->setCursor(Qt::PointingHandCursor);
    btn_play->setProperty("mode", BtnClickMode::PlayAll);
    connect(btn_play, &QPushButton::clicked, this, &AbstractImageDetailInfo_forQobuz::slot_btnClicked_mode);

    // 셔플재생
    QPixmap pixmap_shuffle(ICON_PATH___playShuffle);
    QPushButton *btn_shuffle = new QPushButton;
    btn_shuffle->setIcon(pixmap_shuffle);
    btn_shuffle->setIconSize(pixmap_play.rect().size());
    btn_shuffle->setText(tr("Shuffle Play"));
    //btn_shuffle->setText(tr("셔플재생"));
    btn_shuffle->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_shuffle->setCursor(Qt::PointingHandCursor);
    btn_shuffle->setProperty("mode", BtnClickMode::PlayShuffle);
    connect(btn_shuffle, &QPushButton::clicked, this, &AbstractImageDetailInfo_forQobuz::slot_btnClicked_mode);

    QHBoxLayout *hBox_btnPlays = new QHBoxLayout;
    hBox_btnPlays->setAlignment(Qt::AlignLeft);
    hBox_btnPlays->setSpacing(0);
    hBox_btnPlays->setContentsMargins(0,0,0,0);

    hBox_btnPlays->addSpacing(30);
    hBox_btnPlays->addWidget(btn_play);
    hBox_btnPlays->addSpacing(30);
    hBox_btnPlays->addWidget(btn_shuffle);


    this->widget_playBtns = new QWidget();
    this->widget_playBtns->setContentsMargins(0,16,0,0);
    this->widget_playBtns->setLayout(hBox_btnPlays);
    this->widget_playBtns->setFixedHeight(50);

    // apply layout
    this->vBox_albumInfo->addWidget(this->widget_playBtns, 0, Qt::AlignLeft | Qt::AlignBottom);

}


/**
 * @brief 재생불가 View
 */
void AbstractImageDetailInfo_forQobuz::setUIControl_btnPlays_cannot(){

    QLabel *tmp_label_lock = GSCommon::getUILabelImg(":/images/bugs/bugs-lock.png");
    tmp_label_lock->setFixedSize(30,30);

    QLabel *tmp_label_text = new QLabel();
    tmp_label_text->setText(tr("This album cannot be played at the request of the rights holder."));
    //tmp_label_text->setText(tr("이 앨범은 권리사의 요청으로 재생할 수 없습니다."));
    tmp_label_text->setStyleSheet("color:#e6e6e6; font-size:16px;");

    QHBoxLayout *hBox_btnPlays_cannot = new QHBoxLayout;
    hBox_btnPlays_cannot->setAlignment(Qt::AlignLeft);
    hBox_btnPlays_cannot->setSpacing(0);
    hBox_btnPlays_cannot->setContentsMargins(0,0,0,0);

    hBox_btnPlays_cannot->addWidget(tmp_label_lock);
    hBox_btnPlays_cannot->addWidget(tmp_label_text, 1, Qt::AlignLeft);

    this->widget_playBtns_cannot = new QWidget();
    this->widget_playBtns_cannot->setContentsMargins(30,16,0,0);
    this->widget_playBtns_cannot->setLayout(hBox_btnPlays_cannot);

    // apply layout
    this->vBox_albumInfo->addWidget(this->widget_playBtns_cannot, 0, Qt::AlignLeft);


}



/**
 * @brief 우측에 Big 이미지에 imagePath 보이도록 한다.
 * @param imagePath
 */
void AbstractImageDetailInfo_forQobuz::setImage(QString imagePath){
    // 이미지 경로 설정
    if(imagePath.isEmpty() == false){
        this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }
}


void AbstractImageDetailInfo_forQobuz::setPlaylistImage(QString imagePath){
    // 이미지 경로 설정
    if(imagePath.isEmpty() == false){
        this->flagPlaylist = true;
        if(imagePath == "qobuz_playlist")
        {
            QPixmap pixmap(":images/playlist_def_q.png");
            this->paint_imageBig(pixmap);
        }
        else
        {
            this->fileDownLoader->setImageURL(QUrl(imagePath));
        }

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
void AbstractImageDetailInfo_forQobuz::setImageSizeAndCorner(int width, int height, int cornerRadius){

    this->image_width = width;
    this->image_height = height;
    this->image_cornerRadius = cornerRadius;
}


/**
 * @brief 즐겨찾기(좋아요 하트) 관련 설정
 * @param flagFavorite
 */
void AbstractImageDetailInfo_forQobuz::setFavorite(bool flagFavorite){
    // Data
    this->flagFavorite = flagFavorite;

    // UI
    this->btn_fav_toDelete->setVisible(flagFavorite);
    this->btn_fav_toAdd->setVisible(!flagFavorite);
}

/**
 * @brief AbstractImageDetailInfo::setPossiblePlay
 * @param flagCanPlay
 */
void AbstractImageDetailInfo_forQobuz::setPossiblePlay(bool flagCanPlay){
    if(flagCanPlay){
        this->widget_playBtns->setVisible(true);
        this->widget_playBtns_cannot->setVisible(false);
    }
    else{
        this->widget_playBtns->setVisible(false);
        this->widget_playBtns_cannot->setVisible(true);
    }
}


void AbstractImageDetailInfo_forQobuz::initView(){
    this->setImage("");
    this->label_title->setText("");
    this->elidedLabel_description->setText("");
    this->label_creatorName->setText("");
    this->label_artist->setText("");

    this->label_popularity->setText("");//cheontidalhires
    this->label_hires->setText("");//cheontidalhires
    this->label_streamStartDate->setText("");//cheontidalhires
    this->label_numberOfTracks->setText("");//cheontidalhires

    this->btn_addCollect->setVisible(true);
    this->btn_more->setVisible(true);
    this->setFavorite(false);
}

/**
 * @brief AbstractImageDetailInfo::applyFlagMyData
 */
void AbstractImageDetailInfo_forQobuz::setMyData(bool flagMine){
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
void AbstractImageDetailInfo_forQobuz::slot_fileDownload_loadImage()
{
    bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap;
        if(this->flagPlaylist == true && (this->pixmap_albumImg->width() < 384))
        {
            tmp_pixmap = this->pixmap_albumImg->scaled(384, 186, Qt::IgnoreAspectRatio);
        }
        else
        {
            tmp_pixmap = this->pixmap_albumImg->scaled(this->image_width, this->image_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        this->paint_imageBig(tmp_pixmap);
        this->resize(this->size());
    }

    this->fileDownLoader->deleteLater();
}


/**
 * @brief pixmap 을 label_imageBig 에 그린다. 가로/세로 사이즈와 corner 까지 고려함.
 * @default 기본이미지, 다운로드 이미지 모두 적용하기 위함임
 * @param pixmap
 */
void AbstractImageDetailInfo_forQobuz::paint_imageBig(QPixmap &pixmap){
    // 빈 Pixmap
    QPixmap pixmap_painter = QPixmap(QSize(this->image_width, this->image_height));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, this->image_width, this->image_height, this->image_cornerRadius, this->image_cornerRadius);
    this->label_imageBig->setPixmap(pixmap_painter);
}


/**
 * @brief [Slot] 버튼 클릭에 대한 처리. 시그널 발생
 */
void AbstractImageDetailInfo_forQobuz::slot_btnClicked_mode(){
    BtnClickMode clickMode = sender()->property("mode").value<BtnClickMode>();
    emit this->signal_clicked(clickMode);

    if(clickMode == BtnClickMode::Favorite_toAdd){
        this->btn_fav_toAdd->setVisible(false);
        this->btn_fav_toDelete->setVisible(true);
    }
    else if(clickMode == BtnClickMode::Favorite_toDelete){
        this->btn_fav_toAdd->setVisible(true);
        this->btn_fav_toDelete->setVisible(false);
    }
}



/**
 * @brief AbstractImageDetailInfo::resizeEvent
 * @param event
 */
void AbstractImageDetailInfo_forQobuz::resizeEvent(QResizeEvent *event){
    if(this->elidedLabel_description->isVisible()){
        this->elidedLabel_description->setFixedWidth(event->size().width() - 350 - 84);
    }
}



/**
 * @brief AbstractImageDetailInfo::setTextDescription_withAdaptedWidgh
 * @param text
 */
void AbstractImageDetailInfo_forQobuz::setTextDescription_withAdaptedWidgh(QString text){
    this->elidedLabel_description->setText(text);
    this->elidedLabel_description->setFixedWidth(this->size().width() - 350 - 84);
    this->elidedLabel_description->setFixedHeight(66);
}



/**
 * @brief ArtistImageDetailInfo::getNormalText
 * @details @ref https://stackoverflow.com/questions/11229831/regular-expression-to-remove-html-tags-from-a-string
 *      Testing : @ref https://www.freeformatter.com/regex-tester.html - \[[^\]]*\] 로 테스트 해보면 됨.
 * @param text
 * @return
 */
QString AbstractImageDetailInfo_forQobuz::getNormalText(QString text){

    text.replace("[", "<");
    text.replace("]", ">");
    text = text.remove( QRegExp( "<[^>]*>" ) );

    return text;

}



/**
 * @brief AbstractImageDetailInfo::setVisible_optionBtn
 * @param flagVisiable
 */
void AbstractImageDetailInfo_forQobuz::setVisible_optionBtn(bool flagVisiable){
    this->widget_optBtn->setVisible(flagVisiable);
}

/**
 * @brief AbstractImageDetailInfo::setVisible_playBtn
 * @param flagVisiable
 */
void AbstractImageDetailInfo_forQobuz::setVisible_playBtn(bool flagVisiable){
    this->widget_playBtns->setVisible(flagVisiable);
}
