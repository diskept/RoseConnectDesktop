#include "optionpopup.h"

#include "widget/OptMorePopup.h"
#include "widget/toastmsg.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QPainterPath>
#include <QTextCodec>
#include <QEvent>
#include <QMenu>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <common/sqlitehelper.h>

const int ALBUMIMG_SIZE = 60;
const int MENU_HEIGHT = 40;
const int POPUP_WIDTH = 300;
const int POPUP_WIDTH_MIN = 180;

const int HTTP_PLAY_SUB_ADDLAST = 20;
const int HTTP_PLAY_SUB_ADDEMPTY = 21;
const int HTTP_PLAY_SUB_ADDCURRNEXT = 22;

const int HTTP_PLAY_DIRECT = 10;
const int HTTP_PLAY_DIRECT_NETWORK = 11;
const int HTTP_GET_STAR_MUSIC = 12;
const int HTTP_SET_STAR_MUSIC = 13;

const int HTTP_GET_STAR_MUSIC_PLAYLIST = 15;
const int HTTP_SET_STAR_MUSIC_PLAYLIST = 16;

const int HTTP_GET_STAR_ROSETUBE_TRACK = 71;
const int HTTP_GET_STAR_ROSETUBE_ALBUM = 72;
const int HTTP_SET_STAR_ROSETUBE_TRACK = 76;
const int HTTP_SET_STAR_ROSETUBE_ALBUM = 77;

const int HTTP_DELETE_FAVOR_TRACK = 59;
const int HTTP_DELETE_MY_PLAYLIST = 58;
const int HTTP_DELETE_MY_PLAYLIST_TRACK = 57;


/** ******************************************************************************************************
 * ******************************************************************************************************
 *
 *  OptionPopup 클래스
 *
 * ******************************************************************************************************
 * ****************************************************************************************************** */
/**
 * @brief OptionPopup::OptionPopup 옵션팝업 클래스 생성자
 * @param parent
 */
OptionPopup::OptionPopup(QWidget *parent) : QDialog(parent)
{

    linker = Linker::getInstance();

    //this->setFixedSize(180,200);
    this->setWindowFlags(Qt::Popup| Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->hide();

    createActions();
    this->setUI();
}


OptionPopup::~OptionPopup(){

    this->deleteLater();
}


void OptionPopup::setUI(){

    // 파일다운로드 객체 생성 및 커넥트
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_60.png");

    // 앨범 이미지
    this->lb_albumImg = new QLabel();
    this->lb_albumImg->setFixedSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE);
    this->lb_albumImg->setScaledContents(true);
    this->lb_albumImg->setStyleSheet("background-color:#000000;");

    // 타이틀 및 가수명
    this->lb_title = new QLabel();
    this->lb_title->setStyleSheet("font-size:18px;color:white;");
    this->lb_singer = new QLabel();
    this->lb_singer->setStyleSheet("font-size:16px;color:#777777;");
    this->lb_title->setFixedWidth(190); // 최초 옵션팝업 띄울때 하필 title 긴 글자를 띄운경우에... fix 설정값이 없으면 알아서 안줄어든다.. why? 최초 show 되기 전엔 sizeHint가 적용안되어있기때문..
    this->lb_singer->setFixedWidth(190);
    this->lb_singer->setContentsMargins(0,3,0,0);


    QVBoxLayout *vl_title = new QVBoxLayout();
    vl_title->setContentsMargins(0,0,0,0);
    vl_title->setSpacing(0);
    //vl_title->addSpacing(23);
    vl_title->setAlignment(Qt::AlignVCenter);
    vl_title->addWidget(this->lb_title, 0, Qt::AlignVCenter);
    //vl_title->addSpacing(3);
    vl_title->addWidget(this->lb_singer, 0, Qt::AlignVCenter);

    QHBoxLayout *hl_albumInfo = new QHBoxLayout();
    hl_albumInfo->setSpacing(0);
    hl_albumInfo->addSpacing(21);
    hl_albumInfo->setContentsMargins(0,0,0,0);
    hl_albumInfo->addWidget(this->lb_albumImg);
    hl_albumInfo->addSpacing(8);
    hl_albumInfo->addLayout(vl_title);
    hl_albumInfo->addSpacing(8);

    widget_topAlbumInfo = new QWidget();
    widget_topAlbumInfo->setObjectName("widget_topAlbumInfo");
    widget_topAlbumInfo->setLayout(hl_albumInfo);
    widget_topAlbumInfo->setFixedHeight(90);
    widget_topAlbumInfo->setStyleSheet("#widget_topAlbumInfo { border-bottom:1px solid #4D4D4D; border-radius:0px; }");

    // Star
    QHBoxLayout *hl_star = new QHBoxLayout();
    hl_star->setSpacing(20);
    hl_star->setContentsMargins(0,1,0,1);
    hl_star->setAlignment(Qt::AlignCenter);
    this->list_lb_star = new QList<ClickableLabel*>();
    for(int i=0; i<5; i++){
        this->list_lb_star->append(new ClickableLabel());
        this->list_lb_star->at(i)->setCursor(Qt::PointingHandCursor);
        hl_star->addWidget(this->list_lb_star->at(i));
        connect(this->list_lb_star->at(i), SIGNAL(signal_clicked()), this, SLOT(slot_clickedStar()));
    }

    widget_star = new QWidget();
    widget_star->setObjectName("widget_star");
    widget_star->setFixedHeight(MENU_HEIGHT);
    widget_star->setStyleSheet("#widget_star { border-bottom:1px solid #4D4D4D; border-radius:0px; }");
    widget_star->setLayout(hl_star);

    // 메뉴 제외하고 상단 컨텐츠
    vl_topContent = new QVBoxLayout();
    vl_topContent->setContentsMargins(0,0,0,0);
    vl_topContent->setSpacing(0);
    //vl_topContent->setAlignment(Qt::AlignTop);
    vl_topContent->addWidget(widget_topAlbumInfo);
    vl_topContent->addWidget(widget_star);
    vl_topContent->setSizeConstraint(QLayout::SetFixedSize); // 사이즈


    this->menu = new QMenu();
    this->menu->setFixedWidth(POPUP_WIDTH);
    this->menu->setContentsMargins(0,0,0,0);
    this->menu->setCursor(Qt::PointingHandCursor);
    this->menuSub = menu->addMenu(tr("Submenu"));
    //this->menuSub = menu->addMenu(tr("서브메뉴"));
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(hide()));


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
 * @brief OptionPopup::createActions 액션 생성
 */
void OptionPopup::createActions(){

    //act_play = new QAction(tr("Play Now"));
    act_play = new QAction(tr("Play Now"));
    act_addPlayList = new QAction(tr("Add to a Playlist"));
    act_metaInfo = new QAction(tr("Meta Info"));
    act_goArtist = new QAction(tr("Go to Artist"));
    act_goAlbum = new QAction(tr("Go to Album"));
    act_goPlayList = new QAction(tr("Go to PlayList"));
    act_fileInfo = new QAction(tr("File Info"));
    act_multi = new QAction(tr("Select Multiple items"));
    act_edit = new QAction(tr("Edit"));
    act_delete = new QAction(tr("Delete"));
    act_sub = new QAction(tr("Subscribe Info"));
    /*
     *     act_play = new QAction(tr("바로듣기"));
    act_addPlayList = new QAction(tr("플레이리스트에 담기"));
    act_metaInfo = new QAction(tr("메타정보"));
    act_goArtist = new QAction(tr("Go to Artist"));
    act_goAlbum = new QAction(tr("Go to Album"));
    act_goPlayList = new QAction(tr("Go to PlayList"));
    act_fileInfo = new QAction(tr("파일 정보"));
    act_multi = new QAction(tr("여러항목 선택"));
    act_edit = new QAction(tr("편집"));
    act_delete = new QAction(tr("삭제"));
    act_sub = new QAction(tr("구독정보"));
    */

    act_metaInfo->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_OP_METAINFO);
    act_goArtist->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_OP_ARTISTINFO);
    act_goAlbum->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_OP_ALBUMINFO);
    act_goPlayList->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_OP_PLAYLISTINFO);
    act_sub->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_SUB_CHANNEL);
    act_fileInfo->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_OP_FILEINFO);
    act_multi->setProperty(KEY_PAGE_CODE.toStdString().c_str(), PAGECODE_OP_MULTI);

    // 서브메뉴 관련
    act_sub_addLast = new QAction(tr("Add to end of Queue"));
    act_sub_addEmpty = new QAction(tr("Empty Play Queue and Add a song"));
    act_sub_addCurrNext = new QAction(tr("Add Next to be queued up after the current song"));

    act_sub_playFromCurr = new QAction(tr("Play from here"));
    act_sub_playFromCurrEmpty = new QAction(tr("Empty Queue and Play from here"));
    act_sub_playFromCurrLast = new QAction(tr("Add to end of Queue from here"));
/*
 *     act_sub_addLast = new QAction(tr("큐 끝에 추가"));
    act_sub_addEmpty = new QAction(tr("큐 비우고 추가"));
    act_sub_addCurrNext = new QAction(tr("현재 재생곡 다음 추가"));

    act_sub_playFromCurr = new QAction(tr("여기부터 재생"));
    act_sub_playFromCurrEmpty = new QAction(tr("큐 비우고 여기부터 재생"));
    act_sub_playFromCurrLast = new QAction(tr("여기부터 큐 끝에 추가"));
    */

    // 커넥션
    connect(act_play, &QAction::triggered, this, &OptionPopup::slot_clickedPlay);
    connect(act_addPlayList, &QAction::triggered, this, &OptionPopup::slot_clickedAddPlayList);
    connect(act_metaInfo, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_goArtist, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_goAlbum, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_goPlayList, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_sub, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_fileInfo, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_multi, &QAction::triggered, this, &OptionPopup::slot_gotoPage);
    connect(act_delete, &QAction::triggered, this, &OptionPopup::slot_clickedDelete);

    connect(act_sub_addLast, &QAction::triggered, this, &OptionPopup::slot_clicked_subAddLast);
    connect(act_sub_addEmpty, &QAction::triggered, this, &OptionPopup::slot_clicked_subAddEmpty);
    connect(act_sub_addCurrNext, &QAction::triggered, this, &OptionPopup::slot_clicked_subAddCurrNext);
    connect(act_sub_playFromCurr, &QAction::triggered, this, &OptionPopup::slot_clicked_subPlayFromCurr);
    connect(act_sub_playFromCurrEmpty, &QAction::triggered, this, &OptionPopup::slot_clicked_subPlayFromCurrEmpty);
    connect(act_sub_playFromCurrLast, &QAction::triggered, this, &OptionPopup::slot_clicked_subPlayFromCurrLast);
}


/**
 * @brief OptionPopup::loadImage [SLOT] 앨범 이미지 세팅
 */
void OptionPopup::slot_loadImage()
{
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

    this->fileDownLoader->deleteLater();
}

/**
 * @brief OptionPopup::setData 기본정보 데이터 세팅
 * @note [참고] AlbumDetail 클래스의 setData 받는 구조와 동일하다
 * @param p_data title,singer,cntStar,앨범이미지정보
 */
void OptionPopup::setDataPopup(QJsonObject p_data){


    // 1>  json 풀어서 데이터 세팅
    this->setJsonData(p_data);

    // 2>  UI에 데이터 세팅
    this->setUIDataFromMemberData();

    // 3>  옵션팝업에 보여질 메뉴구성 세팅
    this->setPopupMenuFromMode();

    // 4>  [API 호출] 별 정보 GET
    this->requestGetStarInfo();
}


OptionPopup::TypeMenu OptionPopup::getOptionMenuType(int op_Type){
    TypeMenu rtn = TypeMenu::Music_Home_RecentAlbum;

    switch (op_Type) {
        case Music_Home_RecentAlbum:
            rtn = TypeMenu::Music_Home_RecentAlbum;
            break;
        case Music_Home_AddedAlbum:
            rtn = TypeMenu::Music_Home_AddedAlbum;
            break;
        case Music_Home_MyPlayList:
            rtn = TypeMenu::Music_Home_MyPlayList;
            break;
        case Music_Home_FriendPlayList:
            rtn = TypeMenu::Music_Home_FriendPlayList;
            break;
        case Music_Home_Artist:
            rtn = TypeMenu::Music_Home_Artist;
            break;
        case Music_Category_Track:
            rtn = TypeMenu::Music_Category_Track;
            break;
        case Music_Category_Genre:
            rtn = TypeMenu::Music_Category_Genre;
            break;
        case Music_Category_Artist:
            rtn = TypeMenu::Music_Category_Artist;
            break;
        case Music_Category_Composer:
            rtn = TypeMenu::Music_Category_Composer;
            break;
        case Music_Collection_Track:
            rtn = TypeMenu::Music_Collection_Track;
            break;
        case Music_Collection_Track_Unidentified:
            rtn = TypeMenu::Music_Collection_Track_Unidentified;
            break;
        case Music_Album:
            rtn = TypeMenu::Music_Album;
            break;
        case Music_Folder:
            rtn = TypeMenu::Music_Folder;
            break;
        case Music_MyCollection:
            rtn = TypeMenu::Music_MyCollection;
            break;
        case Music_Etc_AlbumDetailTrack:
            rtn = TypeMenu::Music_Etc_AlbumDetailTrack;
            break;
        case Music_Etc_ArtistDetailTrack:
            rtn = TypeMenu::Music_Etc_ArtistDetailTrack;
            break;
        case Music_Etc_PlayListDetailInfo:
            rtn = TypeMenu::Music_Etc_PlayListDetailInfo;
            break;
        case Music_Etc_PlayListDetailTrack_My:
            rtn = TypeMenu::Music_Etc_PlayListDetailTrack_My;
            break;
        case Music_Etc_PlayListDetailTrack_Friend:
            rtn = TypeMenu::Music_Etc_PlayListDetailTrack_Friend;
            break;
        case Music_Etc_RelationMusic:
            rtn = TypeMenu::Music_Etc_RelationMusic;
            break;
        case Video:
            rtn = TypeMenu::Video;
            break;
        case Radio:
            rtn = TypeMenu::Radio;
            break;
        case RoseTube_Common:
            rtn = TypeMenu::RoseTube_Common;
            break;
        case RoseTube_Home_PlayList_My:
            rtn = TypeMenu::RoseTube_Home_PlayList_My;
            break;
        case RoseTube_Home_PlayList_Friend:
            rtn = TypeMenu::RoseTube_Home_PlayList_Friend;
            break;
        case RoseTube_Etc_PlayListDetailInfo:
            rtn = TypeMenu::RoseTube_Etc_PlayListDetailInfo;
            break;
        case RoseTube_Etc_PlayListDetailTrack_My:
            rtn = TypeMenu::RoseTube_Etc_PlayListDetailTrack_My;
            break;
        case RoseTube_Etc_PlayListDetailTrack_Friend:
            rtn = TypeMenu::RoseTube_Etc_PlayListDetailTrack_Friend;
            break;
        case RoseTube_Etc_RelationVideo:
            rtn = TypeMenu::RoseTube_Etc_RelationVideo;
            break;
        case QueuePlay:
            rtn = TypeMenu::QueuePlay;
            break;
        case TidalPlayList:
            rtn = TypeMenu::TidalPlayList;
            break;
        case TidalAlbum:
            rtn = TypeMenu::TidalAlbum;
            break;
        case Common_Etc_PlayFullScreenCurrTrack:
            rtn = TypeMenu::Common_Etc_PlayFullScreenCurrTrack;
            break;
    }

    return rtn;
}

/**
 * @brief OptionPopup::setJsonData json 데이터를 풀어서 멤버변수에 세팅한다.
 * @param p_jsonObject
 */
void OptionPopup::setJsonData(QJsonObject p_jsonObject){

    // 초기화
    //this->type_optionMenu = TypeMenu::Music_Home_RecentAlbum;
    this->type_optionMenu = this->getOptionMenuType(ProcJsonEasy::getInt(p_jsonObject, "op_type"));
    this->albumImg = "";
    this->cntStar = 0;
    this->list_audioInfo = QList<DataPopup*>();
    this->title = "";
    this->artist = "";
    this->mime = "";
    this->mainMenuCode = 0;
    this->isAlbum = false;
    this->playurl = "";
    this->jsonData_other = QJsonObject();
    this->jsonData_other_rosetubeSub = QJsonObject();
    this->jsonData_other_music_add_playlist = QJsonArray();

    // 데이터 세팅
    if(p_jsonObject.contains(KEY_MAIN_CODE)){
        this->mainMenuCode = p_jsonObject[KEY_MAIN_CODE].toInt();
    }
    if(p_jsonObject.contains(KEY_OP_TYPE)){
        this->type_optionMenu = static_cast<TypeMenu>(p_jsonObject[KEY_OP_TYPE].toInt());
    }
    if(p_jsonObject.contains(KEY_OP_albumImg)){
        this->albumImg = p_jsonObject[KEY_OP_albumImg].toString();
    }
    if(p_jsonObject.contains(KEY_OP_cntStar)){
        this->cntStar = p_jsonObject[KEY_OP_cntStar].toInt();
    }
    if(p_jsonObject.contains(KEY_OP_DATA)){
        QJsonArray tmp_arr = p_jsonObject[KEY_OP_DATA].toArray();
        //this->list_audioInfo = tmp_arr;
        for(int i=0; i<tmp_arr.count(); i++){
            DataPopup *tmp_dataPopup = new DataPopup();
            tmp_dataPopup->setJsonData(tmp_arr[i].toObject());
            this->list_audioInfo.append(tmp_dataPopup);
        }
    }if(p_jsonObject.contains(KEY_OP_DATA_TOTAL)){
        QJsonArray tmp_arr = p_jsonObject[KEY_OP_DATA_TOTAL].toArray();

        for(int i=0; i<tmp_arr.count(); i++){
            DataPopup *tmp_dataPopup = new DataPopup();
            tmp_dataPopup->setJsonData(tmp_arr[i].toObject());
            this->list_audioInfo_total.append(tmp_dataPopup);
        }
    }
    if(p_jsonObject.contains(KEY_OP_TRACK_INDEX)){
        this->track_idx = p_jsonObject[KEY_OP_TRACK_INDEX].toInt();
    }
    if(p_jsonObject.contains(KEY_OP_TITLE_MAIN)){
        this->title = p_jsonObject[KEY_OP_TITLE_MAIN].toString();
    }
    if(p_jsonObject.contains(KEY_OP_TITLE_SUB)){
        this->artist = p_jsonObject[KEY_OP_TITLE_SUB].toString();
    }
    if(p_jsonObject.contains(KEY_OP_MIME)){
        this->mime = p_jsonObject[KEY_OP_MIME].toString();
    }
    if(p_jsonObject.contains(KEY_OP_ROSETUBE_PLAYURL)){
        this->playurl = p_jsonObject[KEY_OP_ROSETUBE_PLAYURL].toString();
    }
    if(p_jsonObject.contains("isAlbum")){
        this->isAlbum = p_jsonObject["isAlbum"].toBool();
    }
    if(p_jsonObject.contains(KEY_OP_PLAYLISTNO)){
        this->playListNo = p_jsonObject[KEY_OP_PLAYLISTNO].toInt();
        if(p_jsonObject.contains("flagPlayListMy")){
            this->flagPlayListMy = p_jsonObject["flagPlayListMy"].toBool();
        }
    }
    if(p_jsonObject.contains(KEY_DATA)){      // for 로즈튜브 구독정보 페이지 이동
        this->jsonData_other_rosetubeSub = p_jsonObject[KEY_DATA].toObject();
    }
    if(p_jsonObject.contains("playList_info")){   // for 로즈튜브 플레이리스트 (마이/친구) 페이지 이동
        this->jsonData_other = p_jsonObject["playList_info"].toObject();
    }
    if(p_jsonObject.contains(KEY_OP_MUSIC_PLAYLIST_ADD_DATA)){
        this->jsonData_other_music_add_playlist = p_jsonObject[KEY_OP_MUSIC_PLAYLIST_ADD_DATA].toArray();
    }
}


/**
 * @brief OptionPopup::getJsonDataOptionPopup 옵션팝업에서 사용하는 데이터 정보 반환
 * @return
 */
QJsonObject OptionPopup::getJsonDataOptionPopup(){
    QJsonObject tmp_data;
    tmp_data[KEY_OP_TITLE_MAIN] = this->title;
    tmp_data[KEY_OP_TITLE_SUB] = this->artist;
    tmp_data[KEY_OP_TYPE] = this->type_optionMenu;
    tmp_data[KEY_OP_albumImg] = this->albumImg;
    tmp_data[KEY_OP_MIME] = this->mime;
    tmp_data[KEY_OP_ARTIST] = this->artist;
    tmp_data[KEY_OP_cntStar] = this->cntStar;

    QJsonArray tmp_array;
    for(int i = 0; i < this->list_audioInfo.count(); i++){
        tmp_array.append(this->list_audioInfo.at(i)->getJsonData());
    }

    tmp_data[KEY_OP_DATA] = tmp_array;
    tmp_data["isAlbum"] = this->isAlbum;
    tmp_data["playListNo"] = this->playListNo;

    return tmp_data;
}


/**
 * @brief OptionPopup::setUIDataFromMemberData UI에 데이터 세팅
 */
void OptionPopup::setUIDataFromMemberData(){

    // Rose 기기 이미지 경로 설정
    QString tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->albumImg);

    // 외부 이미지 경로
    if(this->type_optionMenu==TypeMenu::Music_Home_MyPlayList
            || this->type_optionMenu==TypeMenu::Music_Home_FriendPlayList
            || this->type_optionMenu==TypeMenu::Music_Etc_PlayListDetailTrack_My
            || this->type_optionMenu==TypeMenu::RoseTube_Common
            || this->type_optionMenu==TypeMenu::RoseTube_Home_PlayList_My
            || this->type_optionMenu==TypeMenu::RoseTube_Home_PlayList_Friend
            || this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_My
            || this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_Friend
            || this->type_optionMenu==TypeMenu::TidalPlayList
    ){
        tmp_imgURL = this->albumImg;
    }
    this->lb_albumImg->setPixmap(*this->pixmap_albumImg_default);
    if(this->albumImg!=""){
        // 앨범이미지 다운받아서 세팅 : loadImage() 참고
        this->fileDownLoader->setImageURL(QUrl(tmp_imgURL));
    }

    QString tmp_title = GSCommon::getTextCutFromLabelWidth(this->title, lb_title->width(), lb_title->font());
    QString tmp_singer = GSCommon::getTextCutFromLabelWidth(this->artist, lb_singer->width(), lb_singer->font());
    this->lb_title->setText(tmp_title);
    this->lb_singer->setText(tmp_singer);
    this->lb_singer->setVisible(false);
    if(this->artist!=""){
        this->lb_singer->setVisible(true);
    }
    for(int i=0; i<cntStar; i++){
        if(i < list_lb_star->size()){
            QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/pop_star_icon_on.png");
            this->list_lb_star->at(i)->setPixmap(*pixmap_icon);
        }
    }
    for(int i=cntStar; i<5; i++){
        if(i>=0 && i < list_lb_star->size()){
            QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/pop_star_icon_off.png");
            this->list_lb_star->at(i)->setPixmap(*pixmap_icon);
        }
    }
}

/**
 * @brief OptionPopup::clearInitMenuAction 옵션팝업에 보여질 메뉴(Action)을 구성하기전에 clear 또는 init 한다.
 */
void OptionPopup::clearInitMenuAction(){

    // 메뉴관련 UI 클리어
    this->menu->setFixedWidth(POPUP_WIDTH);
    //this->menuActionBox->menu_subAddPlayList->clear();
    this->menuSub->clear();
    this->menu->clear();

    // 공통 메뉴
    this->widget_topAlbumInfo->setVisible(true);
    //this->widget_star->setVisible(true);
    this->widget_star->setVisible(false);

    this->menu->addAction(act_play);
    //this->menuSub = this->menu->addMenu(tr("Add a Playlist"));
    this->menuSub = this->menu->addMenu(tr("Add a Queue list"));
    //this->menuSub = this->menu->addMenu(tr("재생목록 추가"));
    this->menuSub->setFixedWidth(388);//cheon 188
    this->menuSub->setContentsMargins(0,0,0,0);
    this->menuSub->setWindowFlags(Qt::Popup| Qt::FramelessWindowHint);
    this->menuSub->setAttribute(Qt::WA_TranslucentBackground);
    this->menuSub->setObjectName("menu_subAddPlayList");
    this->menuSub->setStyleSheet("#menu_subAddPlayList { background-color:#333333;border-radius:5px;border:1px solid #4D4D4D; } QMenu { font-size:14px;color:#CCCCCC;background-color:transparent; } QMenu::item { padding-left:25px;padding-top:12px;padding-bottom:12px;background-color:transparent; } QMenu::item:selected { background-color:#4D4D4D; }");
}

/**
 * @brief OptionPopup::setPopupMenuFromMode 옵션팝업에 보여질 메뉴(Action)들을 구성한다.
 */
void OptionPopup::setPopupMenuFromMode(){

    // 메뉴관련 UI 클리어 하고 공통 메뉴 세팅
    this->clearInitMenuAction();

    if(this->type_optionMenu==TypeMenu::Music_Home_RecentAlbum){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);

    }else if(this->type_optionMenu==TypeMenu::Music_Home_AddedAlbum){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);

    }else if(this->type_optionMenu==TypeMenu::Music_Home_MyPlayList){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goPlayList);
        //this->menu->addAction(act_edit);
        this->menu->addAction(act_delete);

    }else if(this->type_optionMenu==TypeMenu::Music_Home_FriendPlayList){

        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goPlayList);
        //this->menu->addAction(act_delete);

    }else if(this->type_optionMenu==TypeMenu::Music_Home_Artist
             || this->type_optionMenu==TypeMenu::Music_Category_Artist){
        this->widget_star->setVisible(false);
        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);

    }else if(this->type_optionMenu==TypeMenu::Music_Category_Track){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);
        //this->menu->addAction(act_fileInfo);
        this->menu->addAction(act_metaInfo);

    }else if(this->type_optionMenu==TypeMenu::Music_Category_Genre
                || this->type_optionMenu==TypeMenu::Music_Category_Composer){

        this->widget_star->setVisible(false);
        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        //this->menu->addAction(act_addPlayList);
    }else if(this->type_optionMenu==TypeMenu::Music_Collection_Track){

        this->menu->addAction(act_delete);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
    }else if(this->type_optionMenu==TypeMenu::Music_Collection_Track_Unidentified){

        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menu->addAction(act_delete);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);


    }else if(this->type_optionMenu==TypeMenu::Music_Album){

        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);

    }else if(this->type_optionMenu==TypeMenu::Music_Folder){

        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);

        //this->menu->addAction(act_goAlbum);
        //this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);
        //this->menu->addAction(act_fileInfo);

    }else if(this->type_optionMenu==TypeMenu::Music_Etc_AlbumDetailTrack){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);

        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);
        //this->menu->addAction(act_fileInfo);
        this->menu->addAction(act_metaInfo);

    }else if(this->type_optionMenu==TypeMenu::Music_Etc_ArtistDetailTrack){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);

        this->menu->addAction(act_goAlbum);
        //this->menu->addAction(act_addPlayList);
        //this->menu->addAction(act_fileInfo);
        this->menu->addAction(act_metaInfo);

    }else if(this->type_optionMenu==TypeMenu::Music_Etc_PlayListDetailInfo){

        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

    }else if(this->type_optionMenu==TypeMenu::Music_Etc_PlayListDetailTrack_My){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);

        this->menu->addAction(act_delete);

    }else if(this->type_optionMenu==TypeMenu::Music_Etc_PlayListDetailTrack_Friend){

        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);
    }else if(this->type_optionMenu==TypeMenu::Music_Etc_RelationMusic){
        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_addPlayList);
        //this->menu->addAction(act_fileInfo);
        this->menu->addAction(act_metaInfo);
        //this->menu->addAction(act_addPlayList);

    }else if(this->type_optionMenu==TypeMenu::Radio){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        this->menu->addAction(act_addPlayList);
        this->menu->addAction(act_metaInfo);

    }else if(this->type_optionMenu==TypeMenu::Video){

        this->widget_star->setVisible(false);
        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

    //}else if(this->type_frameWidget==TypeMenu::RoseTube){
    }else if(this->type_optionMenu==TypeMenu::RoseTube_Common){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        //this->menu->addAction(act_addPlayList);
        this->menu->addAction(act_sub);
        //this->menu->addAction(act_multi);

    }else if(this->type_optionMenu==TypeMenu::RoseTube_Home_PlayList_My){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goPlayList);
        //this->menu->addAction(act_edit);
        this->menu->addAction(act_delete);

    }else if(this->type_optionMenu==TypeMenu::RoseTube_Home_PlayList_Friend){

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);

        this->menu->addAction(act_goPlayList);
        //this->menu->addAction(act_edit);
        //this->menu->addAction(act_delete);

    }else if(this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailInfo){

        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
    }else if(this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_My){

        //this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        //this->widget_topAlbumInfo->setVisible(false);
        //this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        //this->menu->addAction(act_addPlayList);
        this->menu->addAction(act_sub);
        this->menu->addAction(act_delete);
        //this->menu->addAction(act_multi);

    }else if(this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_Friend){

        //this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        //this->widget_topAlbumInfo->setVisible(false);
        //this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        //this->menu->addAction(act_addPlayList);
        this->menu->addAction(act_sub);
        //this->menu->addAction(act_multi);

    }else if(this->type_optionMenu==TypeMenu::RoseTube_Etc_RelationVideo){
        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        //this->menu->addAction(act_addPlayList);

    }else if(this->type_optionMenu==TypeMenu::QueuePlay){

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        this->menu->addAction(act_metaInfo);

    }else if(this->type_optionMenu==TypeMenu::TidalPlayList){

        this->widget_star->setVisible(false);
        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);

        this->menu->addAction(act_goArtist);
        this->menu->addAction(act_addPlayList);

        disconnect(this->act_play, &QAction::triggered, this, &OptionPopup::slot_clickedPlay);
        connect(this->act_play, &QAction::triggered,  [=](){
            emit this->signal_clicked_play();
        });

    }else if(this->type_optionMenu==TypeMenu::TidalAlbum){

        this->widget_star->setVisible(false);
        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
        this->menuSub->addAction(act_sub_playFromCurr);
        this->menuSub->addAction(act_sub_playFromCurrEmpty);
        this->menuSub->addAction(act_sub_playFromCurrLast);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        this->menu->addAction(act_addPlayList);

        disconnect(this->act_play);
        connect(this->act_play, &QAction::triggered,  [=](){
            emit this->signal_clicked_play();
        });

    }else if(this->type_optionMenu==TypeMenu::Common_Etc_PlayFullScreenCurrTrack){

        this->menuSub->clear();
        this->menu->clear();
        this->menu->setFixedWidth(POPUP_WIDTH_MIN);

        this->widget_topAlbumInfo->setVisible(false);
        this->widget_star->setVisible(false);

        this->menu->addAction(act_goAlbum);
        this->menu->addAction(act_goArtist);
        //this->menu->addAction(act_fileInfo);
        this->menu->addAction(act_metaInfo);

    }else{

        // type 없는경우 최소 메뉴 (ex>Queue List)
        this->menuSub->addAction(act_sub_addLast);
        this->menuSub->addAction(act_sub_addEmpty);
        this->menuSub->addAction(act_sub_addCurrNext);
    }
}


/**
 * @brief OptionPopup::getFirstDataPopup 옵션팝업에 세팅된 DataPopup클래스 Array 중에서 1st째의 DataPopup를 반환한다.
 * @return
 */
DataPopup* OptionPopup::getFirstDataPopup(){
    return this->list_audioInfo.first();
}

/**
 * @brief OptionPopup::getAlbumImg 옵션팝업의 썸네일이미지 반환
 * @return
 */
QString OptionPopup::getAlbumImg(){
    return this->albumImg;
}

/**
 * @brief OptionPopup::showMenu 해당 위치에 메뉴박스를 show 해라
 * @param ax
 * @param ay
 */
void OptionPopup::showMenu(int ax, int ay){

    this->move(ax, ay); // 메뉴 hover 하고 난 뒤로 사이즈 empty
    this->show();

    // exec : 음악 홈 의 가로스크롤이 안먹어버림.. 그래서 비동기식 popup 사용함
    //this->menuActionBox->popup(QPoint(0, this->vl_topContent->sizeHint().height()));  // QMenu가 한번이라도 Action에 Hover또는Trigger된 다음부터는  hide 되어 안보인다. exec를 해줘야한다.
}
/**
 * @brief OptionPopup::responseHttp : 로즈 장치 정보요청 결과 처리
 * @param p_jsonObject QJsonObject&
 */
void OptionPopup::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    switch(p_id){
    case HTTP_PLAY_DIRECT:
        emit linker->signal_rosetubeRecentChanged();
        emit linker->signal_musicRecentChanged();
        break;
    case HTTP_PLAY_SUB_ADDLAST:
        break;
    case HTTP_PLAY_SUB_ADDEMPTY:
        break;
    case HTTP_PLAY_SUB_ADDCURRNEXT:
        break;
    case HTTP_GET_STAR_MUSIC:
        if(p_jsonObject.contains("starRate")){
            this->cntStar = p_jsonObject["starRate"].toInt();
            this->repaintStarUI();
        }
        break;
    case HTTP_GET_STAR_MUSIC_PLAYLIST :
        if(p_jsonObject.contains("playlist")){
            QJsonObject tmp_data = p_jsonObject["playlist"].toObject();
            if(tmp_data.contains("star")){
                this->cntStar = tmp_data["star"].toInt();
                this->repaintStarUI();
            }
        }
        break;
    case HTTP_GET_STAR_ROSETUBE_TRACK:
        if(p_jsonObject.contains("star")){
            this->cntStar = p_jsonObject["star"].toInt();
            this->repaintStarUI();
        }
        break;
    case HTTP_GET_STAR_ROSETUBE_ALBUM:
        if(p_jsonObject.contains("star")){
            this->cntStar = p_jsonObject["star"].toInt();
            this->repaintStarUI();
        }
        break;
    case HTTP_SET_STAR_MUSIC:
        // Nothing..
        break;
    case HTTP_SET_STAR_MUSIC_PLAYLIST:
        // Nothing..
        break;
    case HTTP_SET_STAR_ROSETUBE_TRACK:
        // Nothing..
        break;
    case HTTP_SET_STAR_ROSETUBE_ALBUM:
        // Nothing..
        break;
    case HTTP_DELETE_FAVOR_TRACK :
        linker->signal_favoriteTrackChanged();
        break;
    case HTTP_DELETE_MY_PLAYLIST :
        linker->signal_myPlaylistChanged();
        break;
    case HTTP_DELETE_MY_PLAYLIST_TRACK :
        linker->signal_myPlaylistChanged();
        linker->signal_myPlaylistTrackChanged();

        break;
    }
    sender()->deleteLater();
}

/**
 * @brief OptionPopup::setStarUI 별 UI 갱신
 */
void OptionPopup::repaintStarUI(){

    for(int i=0; i<this->cntStar; i++){
        if(i < list_lb_star->size()){
            QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/pop_star_icon_on.png");
            this->list_lb_star->at(i)->setPixmap(*pixmap_icon);
        }
    }
    for(int i=this->cntStar; i<5; i++){
        if(i>=0 && i < list_lb_star->size()){
            QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/pop_star_icon_off.png");
            this->list_lb_star->at(i)->setPixmap(*pixmap_icon);
        }
    }
}

/**
 * @brief OptionPopup::requestGetStarInfo 별 정보 GET 하는 API 호출한다.
 */
void OptionPopup::requestGetStarInfo(){

    if(this->mainMenuCode==GSCommon::MainMenuCode::Music || this->mainMenuCode==GSCommon::MainMenuCode::MusicNetwork){
        if(this->type_optionMenu==TypeMenu::Music_Home_MyPlayList){
            // ===============================================================
            // [음악] 마이플레이리스트 파트
            // ===============================================================
            if(global.device.getDeviceIP()!="" && global.device.getDeviceID()!=""){
                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                QJsonObject tmp_json;
                tmp_json.insert("local_ip", global.device.getDeviceIP());
                tmp_json.insert("mac_address", global.device.getDeviceID());
                tmp_json.insert("permission","1");
                network->request(HTTP_GET_STAR_MUSIC_PLAYLIST, QString("%1/playlist/%2?page=0&size=20")
                             .arg(global.legacy_v3_api).arg(this->playListNo), tmp_json, true,true);
            }

        }else{
            // ===============================================================
            // [음악] 마이플레이리스트 제외한 모든 파트
            // ===============================================================
            if(this->list_audioInfo.count()>0){
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                int tmp_idInt = this->list_audioInfo.first()->getAlbum_id();
                if(!isAlbum){
                    tmp_idInt = this->list_audioInfo.first()->getId().toInt();
                }
                QJsonObject tmp_json;
                tmp_json.insert("id", tmp_idInt);                               // 오디오 PK 또는 앨범 PK
                tmp_json.insert("isAlbum", this->isAlbum);                      // 오디오냐 앨범이냐
                network->request(HTTP_GET_STAR_MUSIC, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_get), tmp_json, true, true);
            }
        }
    }else if(this->mainMenuCode==GSCommon::MainMenuCode::RoseTubeList){


        if(this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_My
                || this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_Friend){

            // ===============================================================
            // [로즈튜브] 플레이리스트 파트 제외한 모든 파트 (트랙형태)
            // ===============================================================
            if(global.user.getUsername()!=""){
                if(this->playurl!=""){
                    NetworkHttp *network = new NetworkHttp;
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                    QUrlQuery params;
                    params.addQueryItem(KEY_OP_ROSETUBE_PLAYURL, this->playurl);
                    params.addQueryItem("username", global.user.getUsername());
                    network->request(HTTP_GET_STAR_ROSETUBE_TRACK, QString("%1/playlist/track/get")
                                     .arg(global.legacy_mod_api), params, true,true);
                }
            }

        }else{
            // ===============================================================
            // [로즈튜브] 플레이리스트 파트 (앨범형태)
            // ===============================================================
            if(global.user.getUsername()!=""){
                if(this->playListNo>0){
                    NetworkHttp *network = new NetworkHttp;
                    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                    QUrlQuery params;
                    params.addQueryItem("no",QString::number(this->playListNo));    // playListNo
                    params.addQueryItem("star_fav",QString::number(1));
                    params.addQueryItem("username", global.user.getUsername());
                    network->request(HTTP_GET_STAR_ROSETUBE_ALBUM, QString("%1/playlist/fetch")
                                     .arg(global.legacy_mod_api), params, true,true);
                }
            }
        }
    }else if(this->mainMenuCode==GSCommon::MainMenuCode::RoseTube){


        // ===============================================================
        // [로즈튜브] 플레이리스트 파트 제외한 모든 파트 (트랙형태)
        // ===============================================================
        if(global.user.getUsername()!=""){
            if(this->playurl!=""){
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QUrlQuery params;
                params.addQueryItem(KEY_OP_ROSETUBE_PLAYURL, this->playurl);
                params.addQueryItem("username", global.user.getUsername());
                network->request(HTTP_GET_STAR_ROSETUBE_TRACK, QString("%1/playlist/track/get")
                                 .arg(global.legacy_mod_api), params, true,true);
            }
        }
    }
}


/**
 * @brief OptionPopup::slot_clickedStar [SLOT] 별 점수 클릭시 처리
 * @note 별 정보 GET/SET 은....  음악(트랙/앨점)과 로즈튜브에서만 별 정보 사용한다.
 */
void OptionPopup::slot_clickedStar(){

    int tmp_index = this->list_lb_star->indexOf(dynamic_cast<ClickableLabel*>(this->sender()));
    if(this->mainMenuCode==GSCommon::MainMenuCode::Music || this->mainMenuCode==GSCommon::MainMenuCode::MusicNetwork){
        if(this->type_optionMenu==TypeMenu::Music_Home_MyPlayList){
            // ===============================================================
            // [음악] 마이플레이리스트 파트 (앨범형태)
            // ===============================================================
            this->cntStar = tmp_index+1;    // 유효할때만 세팅
            this->repaintStarUI();

            if(global.device.getDeviceIP()!="" && global.device.getDeviceID()!=""){

                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                QJsonObject tmp_json;
                tmp_json.insert("id", QString::number(this->playListNo));
                tmp_json.insert("local_ip", global.device.getDeviceIP());
                tmp_json.insert("mac_address", global.device.getDeviceID());
                tmp_json.insert("star",this->cntStar);
                network->request(HTTP_SET_STAR_MUSIC_PLAYLIST, QString("%1/playlist/star")
                             .arg(global.legacy_v3_api), tmp_json, true,true);
            }
        }else{
            // ===============================================================
            // [음악] 마이플레이리스트 제외한 모든 파트
            // ===============================================================
            if(this->list_audioInfo.count()>0){
                this->cntStar = tmp_index+1;    // 유효할때만 세팅
                this->repaintStarUI();
                int tmp_idInt = this->list_audioInfo.first()->getAlbum_id();
                if(!isAlbum){
                    tmp_idInt = this->list_audioInfo.first()->getId().toInt();
                }
                QJsonObject tmp_json;
                tmp_json.insert("starRate", this->cntStar);
                tmp_json.insert("id", tmp_idInt);                               // 오디오 PK 또는 앨범 PK
                tmp_json.insert("isAlbum", this->isAlbum);                      // 오디오냐 앨범이냐
                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(HTTP_SET_STAR_MUSIC, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_set), tmp_json, true, true);
             }
        }
    }else if(this->mainMenuCode==GSCommon::MainMenuCode::RoseTubeList){

        if(this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_My
                || this->type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_Friend){

            // ===============================================================
            // [로즈튜브] 플레이리스트 파트 제외한 모든 파트 (트랙형태)
            // ===============================================================
            if(this->playurl!=""){

                this->cntStar = tmp_index+1;    // 유효할때만 세팅
                this->repaintStarUI();

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QUrlQuery params;
                params.addQueryItem("star", QString::number(this->cntStar));
                params.addQueryItem(KEY_OP_ROSETUBE_PLAYURL, this->playurl);
                network->request(HTTP_SET_STAR_ROSETUBE_TRACK, QString("%1/playlist/track/star")
                                 .arg(global.legacy_mod_api), params, true,true);
            }
        }else{
            // ===============================================================
            // [로즈튜브] 플레이리스트 파트 (앨범형태)
            // ===============================================================
            if(global.user.getUsername()!=""){
                    if(this->playListNo>0){

                        this->cntStar = tmp_index+1;    // 유효할때만 세팅
                        this->repaintStarUI();

                        NetworkHttp *network = new NetworkHttp;
                        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                        QUrlQuery params;
                        params.addQueryItem("no",QString::number(this->playListNo));    // playListNo
                        params.addQueryItem("star",QString::number(this->cntStar));
                        params.addQueryItem("username", global.user.getUsername());
                        network->request(HTTP_SET_STAR_ROSETUBE_ALBUM, QString("%1/playlist/star/modify")
                                         .arg(global.legacy_mod_api), params, true,true);

                    }
            }
        }



    }else if(this->mainMenuCode==GSCommon::MainMenuCode::RoseTube){
        // ===============================================================
        // [로즈튜브] 플레이리스트 파트 제외한 모든 파트 (트랙형태)
        // ===============================================================
        if(this->playurl!=""){

            this->cntStar = tmp_index+1;    // 유효할때만 세팅
            this->repaintStarUI();

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QUrlQuery params;
            params.addQueryItem("star", QString::number(this->cntStar));
            params.addQueryItem(KEY_OP_ROSETUBE_PLAYURL, this->playurl);
            network->request(HTTP_SET_STAR_ROSETUBE_TRACK, QString("%1/playlist/track/star")
                             .arg(global.legacy_mod_api), params, true,true);
        }
    }

}


/**
 * @brief OptionPopup::clickedPlay [SLOT] 바로 듣기 clicked
 * @details
 */
void OptionPopup::slot_clickedPlay(){
    this->hide();

    QJsonObject tmp_1;
    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;

    switch(this->mainMenuCode){
    case GSCommon::MainMenuCode::Music :
        httpCode = "music_song";
        tmp_json.insert("musicPlayType", 15);
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::MusicNetwork:
        httpCode = "music_network";
        tmp_json.insert("musicPlayType",15);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        for(int i = 0 ; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::Video :
        httpCode = "external_usb";
        tmp_json.insert("musicType",11);    // 안넣어도되는듯?
        tmp_json.insert("playType", 21);
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp.insert("isFile", true);
            tmp.insert("path", this->list_audioInfo.at(i)->getFilePath());
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);
        break;
    case GSCommon::MainMenuCode::Radio :
        httpCode = "radioPlay";
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }
        tmp_json.insert("radio", tmp_array);
        break;
    case GSCommon::MainMenuCode::RoseTube :
        httpCode = "youtubePlay";
        for(int i = 0; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }
        tmp_json.insert("youtube", tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::RoseTubeList :
        httpCode = "youtubePlay.playlist.add";
        tmp_json.insert("youtubePlayType", 15);
        for(int i = 0; i < this->list_audioInfo.count(); i++){
            QJsonObject jsonTracks;
            jsonTracks.insert("id", this->list_audioInfo.at(i)->getId());
            jsonTracks.insert("channelId", this->list_audioInfo.at(i)->getChannelId());
            jsonTracks.insert("channelName", this->list_audioInfo.at(i)->getChannelName());
            jsonTracks.insert("thumbnailUrl", this->list_audioInfo.at(i)->getThumbnailUrl());
            jsonTracks.insert("title", this->list_audioInfo.at(i)->getTitle());
            jsonTracks.insert("duration", this->list_audioInfo.at(i)->getstrDuration());
            tmp_array.append(jsonTracks);

            //QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            //tmp_array.append(tmp);
        }
        tmp_json.insert("youtube", tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::PodCast :
        break;
    case GSCommon::MainMenuCode::Tidal :
        break;
    case GSCommon::MainMenuCode::Bugs :
        break;
    case GSCommon::MainMenuCode::Qobuz :
        break;
    default:
        break;
    }


    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [바로 듣기] 미구현... ");
    }
    else if(httpCode == "music_network"){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }
    else if(httpCode == "youtubePlay.playlist.add"){

        tmp_json.insert("shuffle", 0);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_PLAY_DIRECT, QString("http://%1:%2/%3")
                         .arg(global.device.getDeviceIP())
                         .arg(global.port).arg(httpCode), tmp_json, true);
    }
    else{

        // 공통
        tmp_json.insert("currentPosition", 0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }
}


void OptionPopup::slot_clickedPlayRoseTube(QJsonObject &jsonObj){
    this->hide();

    QJsonArray jsonArr_tracks;
    jsonArr_tracks.append(jsonObj);

    if(global.queueTimerFlag && !global.queuelist_mouse_trigger_menu_flag && global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

    int clickMode = global.device.getMusicPlayType();
    int playType = 15;

    if(clickMode == OptMorePopup::ClickMode::Play_RightNow
            || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
    ){
        // 바로 재생을 요청하는 타입
        playType = 15;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
    ){
        // 현재 큐 끝에 추가
        playType = 13;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
    ){
        // 큐를 비우고 추가(재생)
        playType = 12;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
    ){
        playType = 16;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
        // 여기부터 재생
        playType = 17;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
        // 큐 비우고 여기부터 재생
        playType = 18;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
        // 여기부터 큐 끝에 추가
        playType = 19;
    }

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject jsonObj_send;
    jsonObj_send.insert("playType", playType);
    jsonObj_send.insert("shuffle", -1);
    // shuffle option
    /*if(jsonArr_tracks.size() < 2){
        jsonObj_send.insert("shuffle", -1);
    }
    else if(shuffleMode == JustPlay){
        jsonObj_send.insert("shuffle", 0);
    }
    else if(shuffleMode == ShufflePlay){
        jsonObj_send.insert("shuffle", 1);
    }*/
    jsonObj_send.insert("tracks", jsonArr_tracks);
    jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());

    network->request(HTTP_PLAY_DIRECT,
                     QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_play_list_add"),
                     jsonObj_send,
                     true,
                     true);
}


/**
 * @brief OptionPopup::slot_clicked_subAddLast [SLOT] 큐 끝에 추가
 */
void OptionPopup::slot_clicked_subAddLast(){
    //this->hide();// 서브메뉴는 알아서 hide 되서 호출안해도됨

    QJsonObject tmp_1;
    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;

    switch(this->mainMenuCode){
        case GSCommon::MainMenuCode::Music :
            httpCode = "music_song";
            //tmp_json.insert("musicPlayType",15);      // 바로듣기
            //tmp_json.insert("musicPlayType",13);      // 큐 끝에 추가
            //tmp_json.insert("musicPlayType",12);      // 큐 비우고 추가
            //tmp_json.insert("musicPlayType",16);      // 현재 재생곡 다음추가
            tmp_json.insert("musicPlayType",13);        // 큐 끝에 추가
            for(int i=0; i<this->list_audioInfo.count(); i++){
                QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
                tmp_array.append(tmp);
            }
            tmp_json.insert("music",tmp_array);

            if(global.Queue_track_count != 0) {
                print_debug();emit linker->signal_checkQueue(27, "");

                return;
            }
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += tmp_array.count();     // 220419 queue count
            break;
        case GSCommon::MainMenuCode::MusicNetwork :
            httpCode = "music_network";
            tmp_json.insert("musicPlayType",13);
            tmp_json.insert("playType",20);
            tmp_json.insert("shuffle",0);
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
            for(int i = 0 ; i < this->list_audioInfo.count(); i++){
                QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
                tmp_array.append(tmp);
            }
            tmp_json.insert("music",tmp_array);

            if(global.Queue_track_count != 0) {
                print_debug();emit linker->signal_checkQueue(27, "");

                return;
            }
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += tmp_array.count();     // 220419 queue count
            break;
        case GSCommon::MainMenuCode::Video :
            // 팝업메뉴 존재하지 않음
            break;
        case GSCommon::MainMenuCode::Radio :
            // 팝업메뉴 존재하지 않음
            break;
        case GSCommon::MainMenuCode::RoseTube :
            httpCode = "youtubePlay.playlist.add";
            for(int i = 0; i < this->list_audioInfo.count(); i++){
                QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
                tmp_array.append(tmp);
            }
            tmp_json.insert("youtubePlayType",13);
            tmp_json.insert("youtube", tmp_array);

            if(global.Queue_track_count != 0) {
                print_debug();emit linker->signal_checkQueue(27, "");

                return;
            }
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += tmp_array.count();     // 220419 queue count
            break;
        case GSCommon::MainMenuCode::RoseTubeList :
            httpCode = "youtubePlay.playlist.add";
            for(int i = 0; i < this->list_audioInfo.count(); i++){
                QJsonObject jsonTracks;
                jsonTracks.insert("id", this->list_audioInfo.at(i)->getId());
                jsonTracks.insert("channelId", this->list_audioInfo.at(i)->getChannelId());
                jsonTracks.insert("channelName", this->list_audioInfo.at(i)->getChannelName());
                jsonTracks.insert("thumbnailUrl", this->list_audioInfo.at(i)->getThumbnailUrl());
                jsonTracks.insert("title", this->list_audioInfo.at(i)->getTitle());
                jsonTracks.insert("duration", this->list_audioInfo.at(i)->getstrDuration());
                tmp_array.append(jsonTracks);

                //QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
                //tmp_array.append(tmp);
            }
            tmp_json.insert("youtubePlayType",13);
            tmp_json.insert("youtube", tmp_array);

            global.Queue_track_count += tmp_array.count();     // 220419 queue count
            break;
        case GSCommon::MainMenuCode::PodCast :
            break;
        case GSCommon::MainMenuCode::Tidal :
            break;
        case GSCommon::MainMenuCode::Bugs :
            break;
        case GSCommon::MainMenuCode::Qobuz :
            break;
        default:
            break;
    }

    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [큐 끝에 추가] 미구현...");
    }else if(httpCode=="music_network"){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }else{

        // 공통
        tmp_json.insert("currentPosition",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_SUB_ADDLAST, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }
}

/**
 * @brief OptionPopup::slot_clicked_subAddEmpty [SLOT] 큐 비우고 추가
 */
void OptionPopup::slot_clicked_subAddEmpty(){
    //this->hide();// 서브메뉴는 알아서 hide 되서 호출안해도됨

    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;
    switch(this->mainMenuCode){
    case GSCommon::MainMenuCode::Music :
        httpCode = "music_song";
        //tmp_json.insert("musicPlayType",15);      // 바로듣기
        //tmp_json.insert("musicPlayType",13);      // 큐 끝에 추가
        //tmp_json.insert("musicPlayType",12);      // 큐 비우고 추가
        //tmp_json.insert("musicPlayType",16);      // 현재 재생곡 다음추가
        tmp_json.insert("musicPlayType",12);        // 큐 비우고 추가
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::MusicNetwork:
        httpCode = "music_network";
        tmp_json.insert("musicPlayType",12);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        for(int i = 0 ; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");
            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::Video :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::Radio :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::RoseTube :
        httpCode = "youtubePlay.playlist.add";
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }
        tmp_json.insert("youtubePlayType",12);
        tmp_json.insert("youtube", tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::RoseTubeList :
        httpCode = "youtubePlay.playlist.add";
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject jsonTracks;
            jsonTracks.insert("id", this->list_audioInfo.at(i)->getId());
            jsonTracks.insert("channelId", this->list_audioInfo.at(i)->getChannelId());
            jsonTracks.insert("channelName", this->list_audioInfo.at(i)->getChannelName());
            jsonTracks.insert("thumbnailUrl", this->list_audioInfo.at(i)->getThumbnailUrl());
            jsonTracks.insert("title", this->list_audioInfo.at(i)->getTitle());
            jsonTracks.insert("duration", this->list_audioInfo.at(i)->getstrDuration());
            tmp_array.append(jsonTracks);

            //QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            //tmp_array.append(tmp);
        }
        tmp_json.insert("youtubePlayType",12);
        tmp_json.insert("youtube", tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::PodCast :
        break;
    case GSCommon::MainMenuCode::Tidal :
        break;
    case GSCommon::MainMenuCode::Bugs :
        break;
    case GSCommon::MainMenuCode::Qobuz :
        break;
    default:
        break;
    }

    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [큐 비우고 추가] 미구현...");
    }else if(httpCode=="music_network"){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }else{

        // 공통
        tmp_json.insert("currentPosition",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert("music",tmp_array);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_SUB_ADDEMPTY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }
}

/**
 * @brief OptionPopup::slot_clicked_subAddCurrNext [SLOT] 현재 재생곡 다음추가
 */
void OptionPopup::slot_clicked_subAddCurrNext(){
    //this->hide();// 서브메뉴는 알아서 hide 되서 호출안해도됨

    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;
    switch(this->mainMenuCode){
    case GSCommon::MainMenuCode::Music :
        httpCode = "music_song";
        //tmp_json.insert("musicPlayType",15);      // 바로듣기
        //tmp_json.insert("musicPlayType",13);      // 큐 끝에 추가
        //tmp_json.insert("musicPlayType",12);      // 큐 비우고 추가
        //tmp_json.insert("musicPlayType",16);      // 현재 재생곡 다음추가
        tmp_json.insert("musicPlayType",16);        // 현재 재생곡 다음추가
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::MusicNetwork :
        httpCode = "music_network";
        tmp_json.insert("musicPlayType",16);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        for(int i = 0 ; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::Video :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::Radio :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::RoseTube :
        httpCode = "youtubePlay.playlist.add";
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }
        tmp_json.insert("youtubePlayType",16);
        tmp_json.insert("youtube", tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::RoseTubeList :
        httpCode = "youtubePlay.playlist.add";
        for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject jsonTracks;
            jsonTracks.insert("id", this->list_audioInfo.at(i)->getId());
            jsonTracks.insert("channelId", this->list_audioInfo.at(i)->getChannelId());
            jsonTracks.insert("channelName", this->list_audioInfo.at(i)->getChannelName());
            jsonTracks.insert("thumbnailUrl", this->list_audioInfo.at(i)->getThumbnailUrl());
            jsonTracks.insert("title", this->list_audioInfo.at(i)->getTitle());
            jsonTracks.insert("duration", this->list_audioInfo.at(i)->getstrDuration());
            tmp_array.append(jsonTracks);

            //QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            //tmp_array.append(tmp);
        }
        tmp_json.insert("youtubePlayType",16);
        tmp_json.insert("youtube", tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::PodCast :
        break;
    case GSCommon::MainMenuCode::Tidal :
        break;
    case GSCommon::MainMenuCode::Bugs :
        break;
    case GSCommon::MainMenuCode::Qobuz :
        break;
    default:
        break;
    }

    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [현재 재생곡 다음추가] 미구현...");
    }else if(httpCode=="music_network"){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }else{

        // 공통
        tmp_json.insert("currentPosition",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert("music",tmp_array);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_SUB_ADDCURRNEXT, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }
}

/**
 * @brief OptionPopup::slot_clicked_subPlayFromCurr [SLOT] 여기부터 재생
 */
void OptionPopup::slot_clicked_subPlayFromCurr(){
    //this->hide();// 서브메뉴는 알아서 hide 되서 호출안해도됨

    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;
    switch(this->mainMenuCode){
    case GSCommon::MainMenuCode::Music :
        httpCode = "music_song";
        //tmp_json.insert("musicPlayType",15);      // 바로듣기
        //tmp_json.insert("musicPlayType",13);      // 큐 끝에 추가
        //tmp_json.insert("musicPlayType",12);      // 큐 비우고 추가
        //tmp_json.insert("musicPlayType",16);      // 현재 재생곡 다음추가
        tmp_json.insert("musicPlayType",15);        // 여기부터 재생
        /*for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }*/
        for(int i = this->track_idx; i < this->list_audioInfo_total.size(); i++){
            QJsonObject tmp = this->list_audioInfo_total.at(i)->getJsonData();
            tmp_array.append(tmp);
        }

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::MusicNetwork :
        httpCode = "music_network";
        tmp_json.insert("musicPlayType",15);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        for(int i = 0 ; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::Video :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::Radio :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::RoseTube :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::PodCast :
        break;
    case GSCommon::MainMenuCode::Tidal :
        break;
    case GSCommon::MainMenuCode::Bugs :
        break;
    case GSCommon::MainMenuCode::Qobuz :
        break;
    default:
        break;
    }

    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [여기부터 재생] 미구현...");
    }else if(httpCode=="music_network"){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }else{

        // 공통
        tmp_json.insert("currentPosition",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert("music",tmp_array);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_SUB_ADDCURRNEXT, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }

}
/**
 * @brief OptionPopup::slot_clicked_subPlayFromCurrEmpty [SLOT] 큐 비우고 여기부터 재생
 */
void OptionPopup::slot_clicked_subPlayFromCurrEmpty(){
    //this->hide();// 서브메뉴는 알아서 hide 되서 호출안해도됨

    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;
    switch(this->mainMenuCode){
    case GSCommon::MainMenuCode::Music :
        httpCode = "music_song";
        //tmp_json.insert("musicPlayType",15);      // 바로듣기
        //tmp_json.insert("musicPlayType",13);      // 큐 끝에 추가
        //tmp_json.insert("musicPlayType",12);      // 큐 비우고 추가
        //tmp_json.insert("musicPlayType",16);      // 현재 재생곡 다음추가
        tmp_json.insert("musicPlayType",12);        // 큐 비우고 여기부터 재생
        /*for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }*/
        for(int i = this->track_idx; i < this->list_audioInfo_total.size(); i++){
            QJsonObject tmp = this->list_audioInfo_total.at(i)->getJsonData();
            tmp_array.append(tmp);
        }

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::MusicNetwork :
        httpCode = "music_network";
        tmp_json.insert("musicPlayType",12);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        for(int i = 0 ; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            return;
        }
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::Video :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::Radio :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::RoseTube :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::PodCast :
        break;
    case GSCommon::MainMenuCode::Tidal :
        break;
    case GSCommon::MainMenuCode::Bugs :
        break;
    case GSCommon::MainMenuCode::Qobuz :
        break;
    default:
        break;
    }

    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [큐 비우고 여기부터 재생] 미구현...");
    }else if(httpCode=="music_network"){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }else{

        // 공통
        tmp_json.insert("currentPosition",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert("music",tmp_array);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_SUB_ADDCURRNEXT, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }

}

/**
 * @brief OptionPopup::slot_clicked_subPlayFromCurrLast [SLOT] 여기부터 큐 끝에 추가
 */
void OptionPopup::slot_clicked_subPlayFromCurrLast(){
    //this->hide();// 서브메뉴는 알아서 hide 되서 호출안해도됨

    QJsonArray tmp_array;
    QString httpCode = "";
    QJsonObject tmp_json;
    switch(this->mainMenuCode){
    case GSCommon::MainMenuCode::Music :
        httpCode = "music_song";
        //tmp_json.insert("musicPlayType",15);      // 바로듣기
        //tmp_json.insert("musicPlayType",13);      // 큐 끝에 추가
        //tmp_json.insert("musicPlayType",12);      // 큐 비우고 추가
        //tmp_json.insert("musicPlayType",16);      // 현재 재생곡 다음추가
        tmp_json.insert("musicPlayType",13);    // 여기부터 큐 끝에 추가
        /*for(int i=0; i<this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonData();
            tmp_array.append(tmp);
        }*/
        for(int i = this->track_idx; i < this->list_audioInfo_total.size(); i++){
            QJsonObject tmp = this->list_audioInfo_total.at(i)->getJsonData();
            tmp_array.append(tmp);
        }

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::MusicNetwork :
        httpCode = "music_network";
        tmp_json.insert("musicPlayType",13);
        tmp_json.insert("playType",20);
        tmp_json.insert("shuffle",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        for(int i = 0 ; i < this->list_audioInfo.count(); i++){
            QJsonObject tmp = this->list_audioInfo.at(i)->getJsonDataForNetworkPlay();
            tmp_array.append(tmp);
        }
        tmp_json.insert("music",tmp_array);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += tmp_array.count();     // 220419 queue count
        break;
    case GSCommon::MainMenuCode::Video :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::Radio :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::RoseTube :
        // 팝업메뉴 존재하지 않음
        break;
    case GSCommon::MainMenuCode::PodCast :
        break;
    case GSCommon::MainMenuCode::Tidal :
        break;
    case GSCommon::MainMenuCode::Bugs :
        break;
    case GSCommon::MainMenuCode::Qobuz :
        break;
    default:
        break;
    }

    if(httpCode==""){
        ToastMsg::show(this, "", "OptionPopup 의 [여기부터 큐 끝에 추가] 미구현...");
    }else if(httpCode=="music_network"){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_DIRECT_NETWORK, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
    }else{

        // 공통
        tmp_json.insert("currentPosition",0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert("music",tmp_array);

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_PLAY_SUB_ADDCURRNEXT, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg(httpCode), tmp_json, true, true);
    }
}

/**
 * @brief OptionPopup::gotoPage [SLOT] 페이지를 이동하라
 * @note Go to Album, Go to Artist, 메타정보 등등등....페이지 이동하는 Action 클릭시 처리
 */
void OptionPopup::slot_gotoPage(){
    this->hide();

    QJsonObject tmp_data = this->getJsonDataOptionPopup();
    tmp_data[KEY_PAGE_CODE] = sender()->property(KEY_PAGE_CODE.toStdString().c_str()).toString();
    // 음악 관련...
    tmp_data["flagPlayListMy"] = this->flagPlayListMy;  // [음악] 마이/친구 플레이리스트 유형 => 음악 플레이리스트 상세화면 이동 하는 경우 추가 데이터 세팅


    // 로즈튜브 관련...
    //if(this->mainMenuCode==GSCommon::MainMenuCode::RoseTube){
        // 구독정보 페이지 이동하는 경우 추가 데이터 세팅
        tmp_data[KEY_DATA] = this->jsonData_other_rosetubeSub;

    //}else if(this->mainMenuCode==GSCommon::MainMenuCode::RoseTubeList){
        // 친구/마이 플레이리스트 페이지 이동하는 경우 추가 데이터 세팅
        tmp_data.insert("playList_info", this->jsonData_other);
    //}

    if(this->type_optionMenu==TypeMenu::Music_Etc_RelationMusic){
        emit signal_clickedMovePage_mustMainMenu(GSCommon::MainMenuCode::Music, tmp_data);    // 무조건 음악 메인메뉴로 이동한 다음 서브페이지 이동
    }else if(this->type_optionMenu==TypeMenu::Common_Etc_PlayFullScreenCurrTrack){
            emit signal_clickedMovePage_mustMainMenu(this->mainMenuCode, tmp_data);         // 무조건 해당 메인메뉴로 이동한 다음 서브페이지 이동
    }else{
        emit signal_clickedMovePage(tmp_data);
    }
}

/**
 * @brief OptionPopup::slot_clickedAddPlayList [SLOT] 플레이리스트 담기 버튼 클릭시
 * @note 플레이리스트에 담을 트랙 정보를 추가</br>
 * 플레이리스트 담기 처리는 {@linke AbstractMainContent}에서 처리
 *
 */
void OptionPopup::slot_clickedAddPlayList(){

    QJsonObject tmp_data = this->getJsonDataOptionPopup();
    tmp_data.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, jsonData_other_music_add_playlist);
    tmp_data.insert(KEY_OP_ROSETUBE_PLAYURL, playurl);


    emit linker->signal_clickedHoverItem("addPlayList", tmp_data);
}

/**
 * @brief OptionPopup::slot_clickedDelete [SLOT] 삭제 클릭시
 */
void OptionPopup::slot_clickedDelete(){

    QJsonObject tmp_data = this->getJsonDataOptionPopup();

    if(this->type_optionMenu == TypeMenu::Music_Collection_Track
            || type_optionMenu == TypeMenu::Music_Collection_Track_Unidentified
            || type_optionMenu == TypeMenu::Music_Etc_PlayListDetailTrack_My
            || type_optionMenu == TypeMenu::RoseTube_Etc_PlayListDetailTrack_My){
        // ---------------------------------------------------------
        //  [음악] 트랙 삭제
        // ---------------------------------------------------------
        QJsonArray jsonArrIDS;
        if(tmp_data.contains(KEY_OP_DATA)){
            QJsonArray jsonArrData = tmp_data[KEY_OP_DATA].toArray();
            for(int i = 0 ; i < jsonArrData.size(); i++){
                QJsonObject jsonData = jsonArrData.at(i).toObject();

                if(jsonData.contains("favorite_id")){
                    jsonArrIDS.append(jsonData["favorite_id"].toInt());
                }
            }
        }
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonParam;
        jsonParam.insert("ids", jsonArrIDS);
        jsonParam.insert("local_ip", global.device.getDeviceIP());
        jsonParam.insert("mac_address", global.device.getDeviceID());

        if(type_optionMenu==TypeMenu::Music_Etc_PlayListDetailTrack_My
                || type_optionMenu==TypeMenu::RoseTube_Etc_PlayListDetailTrack_My){
            int tmp_playListNo=0;
            if(tmp_data.contains("playListNo")){
                tmp_playListNo = tmp_data["playListNo"].toInt();
            }
            network->request(HTTP_DELETE_MY_PLAYLIST_TRACK, QString("%1/playlist/%2/track/delete").arg(global.legacy_v3_api).arg(tmp_playListNo), jsonParam, true, true);
        }else{
            network->request(HTTP_DELETE_FAVOR_TRACK, QString("%1/playlist/0/track/delete").arg(global.legacy_v3_api), jsonParam, true, true);
        }
    }
    else if(this->type_optionMenu == TypeMenu::Music_Home_MyPlayList
            || this->type_optionMenu == TypeMenu::RoseTube_Home_PlayList_My){
        // ---------------------------------------------------------
        //  [음악 & 로즈튜브] 마이플레이리스트 삭제
        // ---------------------------------------------------------
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        int tmp_playListNo=0;
        if(tmp_data.contains("playListNo")){
            tmp_playListNo = tmp_data["playListNo"].toInt();
        }

        QJsonObject jsonParam;
        jsonParam.insert("local_ip", global.device.getDeviceIP());
        jsonParam.insert("mac_address", global.device.getDeviceID());

        network->request(HTTP_DELETE_MY_PLAYLIST, QString("%1/playlist/%2/delete").arg(global.legacy_v3_api).arg(tmp_playListNo), jsonParam, true, true);

    }else if(this->type_optionMenu == TypeMenu::RoseTube_Etc_PlayListDetailTrack_My){


        ToastMsg::show(this, "", "[로즈튜브] 트렉 삭제.....");
    }

    this->hide();
}
