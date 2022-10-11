#include "OptMorePopup.h"
#include "common/gscommon.h"
#include "common/global.h"
#include <QPainter>
#include <QPainterPath>
#include "common/ProcStarData.h"


#include "tidal/ProcCommon.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "common/ProcJsonEasy.h"
#include "tidal/ConvertData.h"
#include "qobuz/ConvertData_forQobuz.h"



const int ALBUMIMG_SIZE = 60;
const int POPUP_WIDTH = 300;


/**
 * @brief OptMorePopup::OptMorePopup
 * @param parent
 */
OptMorePopup::OptMorePopup(QWidget *parent) : QDialog(parent){

    this->setWindowFlags(Qt::Popup| Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->hide();

    this->setUIControl_basic();

}


/**
 * @brief 소멸 체크를 위해
 */
OptMorePopup::~OptMorePopup(){

    this->deleteLater();
}


/**
 * @brief 공통으로 사용하는 기본 UI 컨트롤을 구성함.  OptionPopup 클래스의 UI 구성부분 카피함
 */
void OptMorePopup::setUIControl_basic(){

    // 파일다운로드 객체 생성 및 커넥트
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL(downloaded()), this, SLOT(slot_loadImage()));

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_60.png");

    // 앨범 이미지
    this->lb_albumImg = new QLabel();
    this->lb_albumImg->setFixedSize(ALBUMIMG_SIZE, ALBUMIMG_SIZE);
    this->lb_albumImg->setScaledContents(true);
    this->lb_albumImg->setStyleSheet("background-color:#000000;");

    // 타이틀 및 가수명
    this->lb_title = new QLabel();
    this->lb_title->setStyleSheet("font-size:18px;color:white;");
    this->lb_singer = new QLabel();
    this->lb_singer->setStyleSheet("font-size:16px;color:#777777;");
    this->lb_title->setFixedWidth(190); // 최초 옵션팝업 띄울때 하필 title 긴 글자를 띄운경우에... fix 설정값이 없으면 알아서 안줄어든다.. why? 최초 show 되기 전엔 sizeHint가 적용안되어있기때문..
    this->lb_singer->setFixedWidth(190);

    QVBoxLayout *vl_title = new QVBoxLayout();
    vl_title->setSpacing(0);
    vl_title->addSpacing(23);
    vl_title->setContentsMargins(0,0,0,0);
    vl_title->addWidget(this->lb_title, 0, Qt::AlignTop);
    vl_title->addSpacing(3);
    vl_title->addWidget(this->lb_singer, 0, Qt::AlignTop);
    vl_title->setAlignment(Qt::AlignTop);

    QHBoxLayout *hl_albumInfo = new QHBoxLayout();
    hl_albumInfo->setSpacing(0);
    hl_albumInfo->addSpacing(21);
    hl_albumInfo->setContentsMargins(0,0,0,0);
    hl_albumInfo->addWidget(this->lb_albumImg);
    hl_albumInfo->addSpacing(8);
    hl_albumInfo->addLayout(vl_title);
    hl_albumInfo->addSpacing(8);

    QWidget *widget_topAlbumInfo = new QWidget();
    widget_topAlbumInfo->setObjectName("widget_topAlbumInfo");
    widget_topAlbumInfo->setLayout(hl_albumInfo);
    widget_topAlbumInfo->setFixedHeight(90);
    widget_topAlbumInfo->setStyleSheet("#widget_topAlbumInfo { border-bottom:1px solid #4D4D4D; border-radius:0px; }");

    // 별점
    this->starPointWidget = new StarPointWidget();
    connect(this->starPointWidget, &StarPointWidget::signal_clicked, this, &OptMorePopup::slot_clicked_starPoint);

    // 메뉴 제외하고 상단 컨텐츠
    vl_topContent = new QVBoxLayout();
    vl_topContent->setContentsMargins(0,0,0,0);
    vl_topContent->setSpacing(0);
    vl_topContent->addWidget(widget_topAlbumInfo);
    vl_topContent->addWidget(this->starPointWidget);
    vl_topContent->setSizeConstraint(QLayout::SetFixedSize); // 사이즈


    this->menu = new QMenu();
    this->menu->setFixedWidth(POPUP_WIDTH);
    this->menu->setContentsMargins(0,0,0,0);
    this->menu->setCursor(Qt::PointingHandCursor);
    this->menuSub = menu->addMenu(tr("Submenu"));
    //this->menuSub = menu->addMenu(tr("서브메뉴"));
    connect(menu, &QMenu::aboutToHide, this, [=](){
        this->done(0);
    });


    vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addLayout(vl_topContent);
    vl_total->addWidget(menu);
    vl_total->setSizeConstraint(QLayout::SetFixedSize); // 사이즈

    widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:5px;border:1px solid #4D4D4D; } QMenu { font-size:14px;color:#CCCCCC;background-color:transparent; } QMenu::item { padding-left:25px;padding-top:12px;padding-bottom:12px;background-color:transparent; } QMenu::item:selected { background-color:#4D4D4D; } QMenu::indicator { color:red; background-color:yellow } ");
    widget_total->setLayout(vl_total);
    vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);
    vboxlayout->setSizeConstraint(QLayout::SetFixedSize); // 사이즈
    this->setLayout(vboxlayout);

}



/**
 * @brief 별점 UI를 화면에 보일지의 여부 세팅 (별점 위젯을 사용할지의 여부를 세팅한다)
 * @param flagShow
 */
void OptMorePopup::resetUI_showStarRate(bool flagShow){
    if(flagShow){
        this->starPointWidget->setVisible(true);
        this->requestProcStarData_get();            // 별점정보 요청
    }
    else{
        this->starPointWidget->setVisible(false);
    }
}




/**
 * @brief MenuMode에 따라서 OptMorePopup의 세부적인 항목을 구성한다. 메뉴별로, 파트별로 구성하도록 한다. (MenuMode 을 공용으로 사용하지 않도록 함) @n
 *          setMenuMode 호출한 후에 @see showPopup 함수를 호출해야 화면에 보인다. 호출 순서 중요함
 * @param menuMode
 * @param index - OptMorePopup을 띄우는 자식이 Section 내에서 몇번째 index의 자식인지를 전달해줘야함. @see signal_clicked 시그널 발생 시 전달 용도
 * @param section
 */
void OptMorePopup::setMenuMode(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index, const int section, const bool p_flagForceHide_favorite){

    // 데이터 세팅
    this->data_header = data_header;
    this->curr_menuMode = menuMode;
    this->index = index;
    this->section = section;

    this->flagForceHide_favorite = p_flagForceHide_favorite;

    if(this->curr_menuMode)

    // 타이달인 경우, 따로 처리를 위해.
    if(this->flagForceHide_favorite == false && menuMode > 70){
        this->check_tidalFavorites();
        //this->check_qobuzFavorites();
    }

    // UI 관련 세팅
    this->setUIControl_header();
    this->createActionMenu();


}






/**
 * @brief showPopup 호하기 전에 @see setMenuMode 호출이 필요하다. 호출 순서 중요함
 */
void OptMorePopup::showPopup(){
    QPoint hintPoint = this->getHintPos();
    this->move(hintPoint);
    connect(this, &OptMorePopup::signal_menuClicked, this, [=](){
        this->done(0);       // 메뉴 클릭하면 항상 hide 해야하므로
    });
    this->show();
}




//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : Only TIDAL
//
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief 일부, 즐겨찾기 추가/삭제를 위해서 http 요청 필요함
 */
void OptMorePopup::check_tidalFavorites(){
    if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Video

    ){

        tidal::ProcCommon *proc = new tidal::ProcCommon(this);
        connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &OptMorePopup::slot_tidal_completeReq_listAll_myFavoritesIds);
        proc->request_tidal_getAll_favorites();
    }
}

/**
 * @brief TIDAL > 사용자의 Favorites의 ids 목록 결과 확인
 * @param jsonObj
 */
void OptMorePopup::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& jsonObj){

    QString check_key = "";
    if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Track){
        check_key = "TRACK";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Album){
        check_key = "ALBUM";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist){
        check_key = "PLAYLIST";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Video){
        check_key = "VIDEO";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Artist){
        check_key = "ARTIST";
    }

    /*if(check_key.isEmpty()==false){
        QVariantList arr_favId = ProcJsonEasy::getJsonArray(jsonObj, check_key).toVariantList();
        if(arr_favId.contains(data_header.data_pk)){
            // 포함되어 있음 - "삭제" 보임
            this->act_addFavorite->setVisible(false);
            this->act_deleteFavorite->setVisible(true);
        }
        else{
            // 포함되지 않음 - "추가" 보임
            this->act_addFavorite->setVisible(true);
            this->act_deleteFavorite->setVisible(false);
        }
    }*/
}


//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : Only QOBUZ
//
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief 일부, 즐겨찾기 추가/삭제를 위해서 http 요청 필요함
 */

void OptMorePopup::check_qobuzFavorites(){
    if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Video

    ){

        //qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
        //connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &OptMorePopup::slot_qobuz_completeReq_listAll_myFavoritesIds);
        //proc->request_qobuz_getAll_favorites();
    }
}

/**
 * @brief TIDAL > 사용자의 Favorites의 ids 목록 결과 확인
 * @param jsonObj
 */
void OptMorePopup::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& jsonObj){

    QString check_key = "";
    if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Track){
        check_key = "TRACK";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Album){
        check_key = "ALBUM";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist){
        check_key = "PLAYLIST";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Video){
        check_key = "VIDEO";
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Artist){
        check_key = "ARTIST";
    }

    if(check_key.isEmpty()==false){
        QVariantList arr_favId = ProcJsonEasy::getJsonArray(jsonObj, check_key).toVariantList();
        if(arr_favId.contains(data_header.data_pk)){
            // 포함되어 있음 - "삭제" 보임
            this->act_addFavorite->setVisible(false);
            this->act_deleteFavorite->setVisible(true);
        }
        else{
            // 포함되지 않음 - "추가" 보임
            this->act_addFavorite->setVisible(true);
            this->act_deleteFavorite->setVisible(false);
        }
    }
}



//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : private slot
//
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief OptMorePopup::loadImage [SLOT] 앨범 이미지 세팅
 */
void OptMorePopup::slot_loadImage(){

    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){

        QPixmap pixmapIMG = QPixmap(QSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(ALBUMIMG_SIZE, ALBUMIMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);


        // --------------------------------------------------------------------
        // 로즈튜브의 경우 이미지가 가로형태이다.
        // Pixmap 센터 조정
        // --------------------------------------------------------------------
        QPainterPath path;
        path.addRoundedRect(0, 0, ALBUMIMG_SIZE, ALBUMIMG_SIZE, 2, 2);
        painter.setClipPath(path);
        painter.drawPixmap(0, (ALBUMIMG_SIZE-tmp_pixmap.height())/2, tmp_pixmap);
        painter.end();

        lb_albumImg->setPixmap(pixmapIMG);
    }
    else{
        // 기본 이미지로 세팅
        QString default_img = "";
        if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Track

                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Video
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Track
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Caching

                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Track
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Artist)
        {
            default_img = ":/images/def_mus_300.png";
        }
        else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Video_Alubm
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Video_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Video_Track)
        {
            default_img = ":/images/def_mus_300.png";
        }
        else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Track
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist_Created
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Artist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Video)
        {
            default_img = ":/images/tidal/tidal_def_200.png";
        }
        else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Track
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Track_withVideo
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_PD_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_MyAlbum
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Video
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Artist)
        {
             default_img = ":/images/bugs/bugs_def_430.png";
        }
        else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Track
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist_Created
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Artist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Video)
        {
            default_img = ":/images/qobuz/qobuz_default_400.png";
        }
        else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Album
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Track
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Playlist_Created            ///< 사용자가 생성한 playlist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Artist
                || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Video)
        {
            default_img = ":/images/apple/apple_def.png";
        }

        QPixmap pixmap(default_img);
        pixmap = pixmap.scaled(ALBUMIMG_SIZE, ALBUMIMG_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        this->lb_albumImg->setPixmap(pixmap);
    }

    this->fileDownLoader->deleteLater();
}


/**
 * @brief [Slot] 별점 클릭에 대한 이벤트 처리
 */
void OptMorePopup::slot_clicked_starPoint(const int starRate){
    // param 이 같지만, 차후 복사&확장 용이하도록 구분함. slot과 실제 처리함수
    this->requestProcStarData_set(starRate);
}






//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : 별점 starRate 관련 처리
//
//-----------------------------------------------------------------------------------------------------------------------


void OptMorePopup::requestProcStarData_get(){
    common::ProcStarData *proc = new common::ProcStarData(this);
    connect(proc, &common::ProcStarData::signal_completed_getStarRate, this, &OptMorePopup::slot_procStarData_complete_getStarRate);
    proc->request_getStarPoint(this->curr_menuMode, data_header.data_pk);
}


void OptMorePopup::requestProcStarData_set(const int starRate){
    common::ProcStarData *proc = new common::ProcStarData(this);
    connect(proc, &common::ProcStarData::signal_completed_setStarRate, this, &OptMorePopup::slot_procStarData_complete_setStarRate);
    proc->request_setStarPoint(this->curr_menuMode, data_header.data_pk, starRate);
}


/**
 * @brief [slot] @see ProcStarData 클래스의 시그널 signal_complete_getStarRate 에 대한 연결 처리
 * @param starRate
 */
void OptMorePopup::slot_procStarData_complete_getStarRate(const int starRate){
    this->starPointWidget->setStarRate(starRate);
}

/**
 * @brief [slot] @see ProcStarData 클래스의 시그널 signal_complete_setStarRate 에 대한 연결 처리
 * @param starRate
 */
void OptMorePopup::slot_procStarData_complete_setStarRate(const int starRate){
    this->starPointWidget->setStarRate(starRate);
}






//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : private functions 모음
//
//-----------------------------------------------------------------------------------------------------------------------



/**
 * @brief 화면에 보일 시작위치를 계산하여 반환
 * @return OptMorePopup 이 위치할 Left, Top 포인트를 반환한다.
 */
QPoint OptMorePopup::getHintPos(){

    QPoint point_mouseClicked = QCursor::pos();     // 화면에서의 마우스 클릭 포인트. (화면의 좌측상단이 0,0 임)

    int check_width = global.left_mainwindow + global.width_mainwindow - point_mouseClicked.x() - 10;             // 여유를 10 정도 더둠
    int check_height = global.top_mainwindow + global.height_mainwindow - point_mouseClicked.y() - 10;            // 여유를 10 정도 더둠

    // X 좌표 체크
    int tmp_posX = point_mouseClicked.x();
    if(check_width < this->sizeHint().width()){
        tmp_posX -= this->sizeHint().width();       // 마우스 클릭의 왼쪽으로 뷰가 생기도록
    }

    // Y 좌표 체크
    int tmp_posY = point_mouseClicked.y();
    if(check_height < this->sizeHint().height()){
        tmp_posY -= (this->sizeHint().height() - check_height);     // over 된 만큼만 위쪽으로 이동 되도록
    }

    return QPoint(tmp_posX, tmp_posY);

}



/**
 * @brief 썸네일 이미지 세팅한다. empty 이면 기본 이미지 세팅
 * @param imageUrl
 */
void OptMorePopup::setThumbnailImage(const QString &imageUrl){

    if(imageUrl.isEmpty()){
        this->lb_albumImg->setPixmap(*this->pixmap_albumImg_default);
    }
    else{
        // 이미지 다운받아서 세팅 : loadImage() 참고
        this->fileDownLoader->setImageURL(QUrl(imageUrl));
    }
}


/**
 * @brief 메인 메뉴를 추가한다. 자동으로 connect 연결함
 * @param p_clickMode
 */
void OptMorePopup::add_mainAction(const OptMorePopup::ClickMode p_clickMode){
    QAction *tmp_act = new QAction(this->getMenuString_byMenuMode(p_clickMode));
    connect(tmp_act, &QAction::triggered, this, [=](){
        print_debug();
        emit this->signal_menuClicked(p_clickMode, this->index, this->section);
    });
    this->menu->addAction(tmp_act);

    if(p_clickMode == ClickMode::Add_Favorite){
        this->act_addFavorite = tmp_act;
    }
    else if(p_clickMode == ClickMode::Delete_Favorite){
        this->act_deleteFavorite = tmp_act;
    }
}



/**
 * @brief 서브 메뉴를 추가한다. 자동으로 connect 연결함
 * @param p_clickMode
 */
void OptMorePopup::add_subAction(const OptMorePopup::ClickMode p_clickMode){

    if(this->cnt_subMenu == 0){
        // 서브 메뉴가 하나도 없는 경우의 처리
        this->menuSub = this->menu->addMenu(tr("Add a Queue list"));
        //this->menuSub = this->menu->addMenu(tr("Add Playlist"));tr("Add a Queue list")
        //this->menuSub = this->menu->addMenu(tr("재생목록 추가"));
        this->menuSub->setFixedWidth(388);             // cheon_debug120110   ::original:: this->menuSub->setFixedWidth(188);
        this->menuSub->setContentsMargins(0,0,0,0);
        this->menuSub->setWindowFlags(Qt::Popup| Qt::FramelessWindowHint);
        this->menuSub->setAttribute(Qt::WA_TranslucentBackground);
        this->menuSub->setObjectName("menu_subAddPlayList");
        this->menuSub->setStyleSheet(" #menu_subAddPlayList { background-color:#333333;border-radius:5px;border:1px solid #4D4D4D; } "
                                     " QMenu { font-size:14px;color:#CCCCCC;background-color:transparent; } "
                                     " QMenu::item { padding-left:25px;padding-top:12px;padding-bottom:12px;background-color:transparent; } "
                                     " QMenu::item:selected { background-color:#4D4D4D; }" );
    }

    QAction *tmp_act = new QAction(this->getMenuString_byMenuMode(p_clickMode));
    connect(tmp_act, &QAction::triggered, this, [=](){
        print_debug();
        emit this->signal_menuClicked(p_clickMode, this->index, this->section);
    });
    this->menuSub->addAction(tmp_act);

    this->cnt_subMenu += 1;
}


/**
 * @brief ClickMode에 대한 메뉴명 텍스트 반환
 * @param p_clickMode
 * @return
 */
QString OptMorePopup::getMenuString_byMenuMode(const OptMorePopup::ClickMode p_clickMode){
    switch (p_clickMode) {
    case ClickMode::Play_RightNow :                         return tr("Play Now");
    case ClickMode::Play_RightNow_withReorder :             return tr("Play Now");
    case ClickMode::Add_MyCollection :                      return tr("Add to a Playlist");
    case ClickMode::MetaInfo :                              return tr("Meta Info");
    case ClickMode::Go_Artist :                             return tr("Go to Artist");
    case ClickMode::Go_Album :                              return tr("Go to Album");
    case ClickMode::Go_Playlist :                           return tr("Go to Playlist");
    case ClickMode::Go_Ref_Videos :                         return tr("Go to Video");
    case ClickMode::FileInfo :                              return tr("File Info");
    case ClickMode::Multi_Select :                          return tr("Select Multiple items");
    case ClickMode::Edit :                                  return tr("Edit");
    case ClickMode::Delete :                                return tr("Delete");
    case ClickMode::SubscribInfo :                          return tr("Subscribe Info");
    case ClickMode::Caching_ahead :                         return tr("Caching ahead");
    case ClickMode::Cache_Lock :                            return tr("Lock");
    case ClickMode::Cache_Unlock :                          return tr("Unlock");
    case ClickMode::Add_Favorite :                          return tr("Add to Favorites");
    case ClickMode::Delete_Favorite :                       return tr("Delete Favorites");

    case ClickMode::SubMenu_QueueAdd_Last :                 return tr("Add to end of Queue");//c220511
    case ClickMode::SubMenu_QueueAdd_Empty :                return tr("Empty the queue and add");//c220511
    case ClickMode::SubMenu_QueueAdd_RightNow:              return tr("Add to a direct play");//c220511
    case ClickMode::SubMenu_QueueAdd_CurrNext :             return tr("Add as next play");//c220511
    case ClickMode::SubMenu_Play_FromHere :                 return tr("Play from here");//c220511
    case ClickMode::SubMenu_Play_FromHere_procEmpty :       return tr("Empty the queue and play from here");//c220511
    case ClickMode::SubMenu_QueueAdd_FromHere_Last :        return tr("Add from here to the end of the queue");//c220511

    case ClickMode::SubMenu_QueueAdd_Last_OnlyOne :         return tr("One song, Add to end of queue ");//c220511
    case ClickMode::SubMenu_QueueAdd_Empty_OnlyOne :        return tr("One song, Empty the queue and add");//c220511
    case ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne :     return tr("One song, Add as next play");//c220511
    case ClickMode::SubMenu_Play_RightNow_OnlyOne :         return tr("One song, Add to a direct play");//c220511

    case ClickMode::Share :                                 return tr("Share");//c220818

    /*
    case ClickMode::Play_RightNow :             return tr("바로 듣기");
    case ClickMode::Play_RightNow_withReorder : return tr("바로 재생");
    case ClickMode::Add_MyCollection :          return tr("플레이리스트에 담기");
    case ClickMode::MetaInfo :                  return tr("메타정보");
    case ClickMode::Go_Artist :                 return tr("Go to Artist");
    case ClickMode::Go_Album :                  return tr("Go to Album");
    case ClickMode::Go_Playlist :               return tr("Go to Playlist");
    case ClickMode::Go_Ref_Videos :             return tr("영상보기");
    case ClickMode::FileInfo :                  return tr("파일 정보");
    case ClickMode::Multi_Select :              return tr("여러항목 선택");
    case ClickMode::Edit :                      return tr("편집");
    case ClickMode::Delete :                    return tr("삭제");
    case ClickMode::SubscribInfo :              return tr("구독정보");
    case ClickMode::Add_Favorite :              return tr("즐겨찾기 추가");
    case ClickMode::Delete_Favorite :           return tr("즐겨찾기 삭제");

    case ClickMode::SubMenu_QueueAdd_Last :                 return tr("큐 끝에 추가");
    case ClickMode::SubMenu_QueueAdd_Empty :                return tr("큐 비우고 추가");
    case ClickMode::SubMenu_QueueAdd_CurrNext :             return tr("현재 재생곡 다음 추가");
    case ClickMode::SubMenu_Play_FromHere :             return tr("여기부터 재생");
    case ClickMode::SubMenu_Play_FromHere_procEmpty :   return tr("큐 비우고 여기부터 재생");
    case ClickMode::SubMenu_QueueAdd_FromHere_Last :        return tr("여기부터 큐 끝에 추가");

    case ClickMode::SubMenu_QueueAdd_Last_OnlyOne :         return tr("한 곡 큐 끝에 추가");
    case ClickMode::SubMenu_QueueAdd_Empty_OnlyOne :        return tr("한 곡 큐 비우고 추가");
    case ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne :     return tr("한 곡 현재 재생곡 다음 추가");
    case ClickMode::SubMenu_Play_RightNow_OnlyOne :         return tr("한 곡 바로 듣기 추가");
    */

    }

    return "";
}





//-----------------------------------------------------------------------------------------------------------------------
//
// MARK : 메뉴별로, 타입별로 함수 바디를 추가 수정해야함
//
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief MenuMode에 따라서, data_header 참고하여 헤더 구성한다.
 * @param menuMode
 * @param data_header
 */
void OptMorePopup::setUIControl_header(){

    if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Track

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Track_inFullScreen
            ){
        // MUSIC > Playlist, Album, Track, Artist
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);

    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Video_Alubm
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Video_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Video_Track
            ){
        // VIDEO > Playlist, Album, Track
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Video
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosetube_Caching

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Music_Track_inFullScreen
            ){
        // ROSETUBE > Video, Playlist, Track
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);

    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist_Created

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Track_inFullScreen
    ){
        // TIDAL > Playlist, Album, Track, Artist
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);

    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Video){
        // TIDAL > Video
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);
    }    
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_PD_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Video
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Track_withVideo
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_MyAlbum

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Track_inFullScreen
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Track_inFullScreen_withVideo
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Bugs_Video_inFullScreen
    ){

        // Bugs
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist_Created

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Track_inFullScreen
    ){
        // QOBUZ > Playlist, Album, Track, Artist
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);

    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Video){
        // QOBUZ > Video
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Playlist_Created

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Track_inFullScreen
    ){
        // ROSEHOME > Playlist, Album, Track, Artist
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);

    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Video){
        // ROSEHOME > Video
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Album
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Playlist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Track
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Artist
            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Playlist_Created

            || this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Track_inFullScreen
    ){
        // APPLE > Playlist, Album, Track, Artist
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);

    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Video){
        // APPLE > Video
        this->lb_title->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.main_title, lb_title->width(), lb_title->font()));
        this->lb_singer->setText(GSCommon::getTextCutFromLabelWidth(this->data_header.sub_title, lb_singer->width(), lb_singer->font()));
        this->setThumbnailImage(this->data_header.imageUrl);
        this->resetUI_showStarRate(this->data_header.flagProcStar);
    }
}


/**
 * @brief OptMorePopup::createActionMenu
 * @param menuMode
 */
void OptMorePopup::createActionMenu(){

    // init
    this->menuSub->clear();
    this->menu->clear();
    this->cnt_subMenu = 0;

    if(this->curr_menuMode == PopupMenuMode::Music_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
        this->add_mainAction(ClickMode::Go_Artist);
        //this->add_mainAction(ClickMode::Share); //c220818
    }
    else if(this->curr_menuMode == PopupMenuMode::Music_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);
        //this->add_mainAction(ClickMode::Share); //c220818

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        //this->add_mainAction(ClickMode::Go_Album);
        //this->add_mainAction(ClickMode::Go_Artist);

    }

    else if(this->curr_menuMode == PopupMenuMode::Video_Alubm){

    }
    else if(this->curr_menuMode == PopupMenuMode::Video_Playlist){

    }
    else if(this->curr_menuMode == PopupMenuMode::Video_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);
    }

    else if(this->curr_menuMode == PopupMenuMode::Rosetube_Video){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);


        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        if(!this->data_header.sub_title.isEmpty()){
            this->add_mainAction(ClickMode::SubscribInfo);          // Subscrition
        }
        if(global.debugCHK > 3){
            this->add_mainAction(ClickMode::Caching_ahead);             // Caching
        }
        this->add_mainAction(ClickMode::Share); //c220818
    }
    else if(this->curr_menuMode == PopupMenuMode::Rosetube_Playlist){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        //this->add_mainAction(ClickMode::Go_Artist);
    }
    else if(this->curr_menuMode == PopupMenuMode::Rosetube_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        //this->add_mainAction(ClickMode::Go_Album);
        //this->add_mainAction(ClickMode::Go_Artist);

        if(global.debugCHK > 3){
            this->add_mainAction(ClickMode::Caching_ahead);             // Caching
        }
        this->add_mainAction(ClickMode::Share); //c220818
    }
    else if(this->curr_menuMode == PopupMenuMode::Rosetube_Caching){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        //this->add_mainAction(ClickMode::Add_MyCollection);

        if(!this->data_header.sub_title.isEmpty()){
            this->add_mainAction(ClickMode::SubscribInfo);          // Subscrition
        }

        if(this->data_header.cache_lock == true){
            this->add_mainAction(ClickMode::Cache_Unlock);
        }
        else if(this->data_header.cache_lock == false){
            this->add_mainAction(ClickMode::Cache_Lock);
        }

        this->add_mainAction(ClickMode::Delete);
        this->add_mainAction(ClickMode::Share); //c220818
    }

    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        if(this->data_header.isMix == false){
            this->add_mainAction(ClickMode::Share); //j220906 share link
        }

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Tidal_Playlist_Created){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Edit);
        //this->add_mainAction(ClickMode::Delete);

        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Tidal_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
        this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Tidal_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Tidal_Track_inFullScreen){
        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Tidal_Artist){
        if(!this->flagForceHide_favorite){
            //this->add_mainAction(ClickMode::Add_Favorite);
            //this->add_mainAction(ClickMode::Delete_Favorite);
            //this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)

            this->add_mainAction(ClickMode::Share); //c220818
        }

    }
    else if(this->curr_menuMode == PopupMenuMode::Tidal_Video){

        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        this->add_mainAction(ClickMode::Share); //c220818
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

    }

    else if(this->curr_menuMode == PopupMenuMode::Bugs_Track
            || this->curr_menuMode == PopupMenuMode::Bugs_Track_withVideo
    ){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);

        if(this->curr_menuMode == PopupMenuMode::Bugs_Track_withVideo){
            // 영상보기
            this->add_mainAction(ClickMode::Go_Ref_Videos);
        }

        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_Track_inFullScreen
            || this->curr_menuMode == PopupMenuMode::Bugs_Track_inFullScreen_withVideo
    ){
        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);

        if(this->curr_menuMode == PopupMenuMode::Bugs_Track_inFullScreen_withVideo){
            // 영상보기
            this->add_mainAction(ClickMode::Go_Ref_Videos);
        }

    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_Video){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818
    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_Video_inFullScreen){
        this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_PD_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);
        this->add_mainAction(ClickMode::Share); //c220818
    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_MyAlbum){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);
        //this->add_mainAction(ClickMode::Share); //c220818
    }
    else if(this->curr_menuMode == PopupMenuMode::Bugs_Artist){
        //this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818
    }

    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        this->add_mainAction(ClickMode::Share); //c220818

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Qobuz_Playlist_Created){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Edit);
        //this->add_mainAction(ClickMode::Delete);

    }
    else if(this->curr_menuMode == PopupMenuMode::Qobuz_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
        this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Qobuz_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);
        this->add_mainAction(ClickMode::Share); //c220818

    }
    else if(this->curr_menuMode == PopupMenuMode::Qobuz_Track_inFullScreen){
        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Qobuz_Artist){
        /*this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        this->add_mainAction(ClickMode::Share); //j220907 share link

    }
    else if(this->curr_menuMode == PopupMenuMode::Qobuz_Video){

        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }
        this->add_mainAction(ClickMode::Share); //c220818

    }

    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Playlist){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Share); //c220818
        if(data_header.type != "MUSIC" && data_header.isShare == true){            //j220905
            this->add_mainAction(ClickMode::Share);
        }
        else if(data_header.type == "MUSIC" && data_header.isShare == true && data_header.sub_title != global.user.getUsername()){
            this->add_mainAction(ClickMode::Share);
        }

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Rosehome_Playlist_Created){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Edit);
        //this->add_mainAction(ClickMode::Delete);

    }
    else if(this->curr_menuMode == PopupMenuMode::Rosehome_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Share); //c220818
        if(data_header.type != "MUSIC" && data_header.isShare == true){            //j220905
            this->add_mainAction(ClickMode::Share);
        }
        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }
        this->add_mainAction(ClickMode::Go_Artist);*/

    }
    else if(this->curr_menuMode == PopupMenuMode::Rosehome_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        //this->add_mainAction(ClickMode::Share); //c220818
        if(data_header.type != "MUSIC" && data_header.isShare == true){            //j220905
            this->add_mainAction(ClickMode::Share);
        }

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        //this->add_mainAction(ClickMode::Go_Album);
        //this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Rosehome_Track_inFullScreen){
        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Rosehome_Artist){

        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

    }
    else if(this->curr_menuMode == PopupMenuMode::Rosehome_Video){

        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Share); //c220818
        if(data_header.type != "MUSIC" && data_header.isShare == true){            //j220905
            this->add_mainAction(ClickMode::Share);
        }

        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

    }

    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Playlist){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        this->add_mainAction(ClickMode::Share); //c220818
        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
    }
    else if(this->curr_menuMode == OptMorePopup::PopupMenuMode::Apple_Playlist_Created){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        //this->add_mainAction(ClickMode::Edit);
        //this->add_mainAction(ClickMode::Delete);

    }
    else if(this->curr_menuMode == PopupMenuMode::Apple_Album){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        this->add_mainAction(ClickMode::Share); //c220818
        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/
        //this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Apple_Track){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        this->add_mainAction(ClickMode::Share); //c220818

        //this->add_mainAction(ClickMode::Add_MyCollection);
        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

        //this->add_mainAction(ClickMode::Go_Album);
        //this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Apple_Track_inFullScreen){
        this->add_mainAction(ClickMode::Go_Album);
        this->add_mainAction(ClickMode::Go_Artist);

    }
    else if(this->curr_menuMode == PopupMenuMode::Apple_Artist){
        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_RightNow_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere);
        this->add_subAction(ClickMode::SubMenu_Play_FromHere_procEmpty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_FromHere_Last);

        /*if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }*/

    }
    else if(this->curr_menuMode == PopupMenuMode::Apple_Video){

        this->add_mainAction(ClickMode::Play_RightNow);

        this->add_subAction(ClickMode::SubMenu_QueueAdd_Last);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_Empty);
        this->add_subAction(ClickMode::SubMenu_QueueAdd_CurrNext);

        if(!this->flagForceHide_favorite){
            this->add_mainAction(ClickMode::Add_Favorite);
            this->add_mainAction(ClickMode::Delete_Favorite);
            this->act_deleteFavorite->setVisible(false);            // 우선 삭제를 가림 (기본값)
        }
        this->add_mainAction(ClickMode::Share); //c220818

    }
}
