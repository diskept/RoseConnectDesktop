#include "music/albumdetail.h"

#include "music/proc_addtackinplaylist.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"//c211213
#include "common/sqlitehelper.h"

#include "login/dialog_newplaylist.h"

#include "widget/toastmsg.h"
#include "widget/trackinfoedit.h"

#include <QJsonObject>
#include <QVBoxLayout>
#include <QPainter>
#include <QScrollArea>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>//c211213

#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>


const QString ALBTAB_STEP_FRIEND = "track";
const QString ALBTAB_STEP_RECEIVE = "cradit";
const QString ALBTAB_STEP_SEND = "recommend";

const int ALBUMIMG_SIZE = 300;
const int max_star_point = 5;

const int HTTP_GET_STAR = 11;
const int HTTP_SET_STAR = 12;

const int HTTP_PLAY_ALL = 99;
const int HTTP_PLAY_SHUFFLE = 98;

const int HTTP_FAVORITE_GET = 59;
const int HTTP_FAVORITE_SET = 58;



/**
     * @brief The AlbumDetail class 앨범상세 화면 @n
     *      앨범 관리(플레이리스트담기, 별점, 하트, 앨범 정보) 및 트랙 관리(재생, 플레이리스트담기, 하트, 별점, 메타정보) 지원
     * @param parent
     */
AlbumDetail::AlbumDetail(QWidget *parent) : AbstractPage(parent)
{
    this->setInit();
    this->setUIControl();
}


AlbumDetail::~AlbumDetail(){

    this->deleteLater();
}


/**
     * @brief AlbumDetail::setInit : 초기 세팅
     */
void AlbumDetail::setInit(){

    linker = Linker::getInstance();

    proc_add = new Proc_AddTackInPlayList(this);
    connect(proc_add, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_add, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    pixmap_heart_on.load(":/images/list_fav_icon_on.png");
    pixmap_heart_off.load(":/images/list_fav_icon.png");


    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
}


/**
     * @brief AlbumDetail::setUIContro UI 세팅
     */
void AlbumDetail::setUIControl(){

    // 앨범 이미지
    this->lb_albumImg = new QLabel();
    this->lb_albumImg->setFixedSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE);

    // 타이틀 및 가수명
    this->lb_title = new QLabel();
    this->lb_title->setStyleSheet("color:#FFFFFF;font-size:30px;");
    this->lb_title->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_title->setContentsMargins(18,0,0,0);

    this->lb_artist = new QLabel();
    this->lb_artist->setStyleSheet("color:#E6E6E6;font-size:20px;");
    this->lb_artist->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
    this->lb_artist->setContentsMargins(18,0,0,0);

    // 별점
    QHBoxLayout *hl_star = new QHBoxLayout();
    hl_star->setContentsMargins(0,0,0,0);
    hl_star->setSpacing(0);
    hl_star->setAlignment(Qt::AlignLeft);
    hl_star->addSpacing(13);

    this->list_lb_star = new QList<ClickableLabel*>();
    for(int i = 0; i < max_star_point; i++){
        this->list_lb_star->append(new ClickableLabel());
        this->list_lb_star->at(i)->setPixmap(*GSCommon::getUIPixmapImg(":/images/star_icon_off.png"));
        this->list_lb_star->at(i)->setCursor(Qt::PointingHandCursor);
        hl_star->addWidget(this->list_lb_star->at(i));
        connect(this->list_lb_star->at(i), SIGNAL(signal_clicked()), this, SLOT(slot_clickedStar()));
    }

    // 플레이리스트 담기
    QPushButton *btn_play_list_add_icon = GSCommon::getUIBtnImg("btn_play_list_add_icon", ":/images/play_list_add_icon.png");
    btn_play_list_add_icon->setFixedSize(60,60);
    btn_play_list_add_icon->setCursor(Qt::PointingHandCursor);
    btn_play_list_add_icon->setProperty("code", "addPlayList");

    // 하트
    btn_heart = new QPushButton();
    btn_heart->setIcon(pixmap_heart_off);
    btn_heart->setIconSize(pixmap_heart_off.rect().size());
    btn_heart->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_heart->setCursor(Qt::PointingHandCursor);
    btn_heart->setFixedSize(60,70);

    QPushButton *btn_icon_info = GSCommon::getUIBtnImg("btn_icon_info", ":/images/icon_info.png");
    btn_icon_info->setFixedSize(60,60);
    btn_icon_info->setCursor(Qt::PointingHandCursor);
    btn_icon_info->setProperty("code", "info");
    QPushButton *btn_icon_menu = GSCommon::getUIBtnImg("btn_icon_menu", ":/images/icon_menu.png");
    btn_icon_menu->setFixedSize(60,60);
    btn_icon_menu->setCursor(Qt::PointingHandCursor);
    btn_icon_menu->setProperty("code", "more");
    QHBoxLayout *hl_hart = new QHBoxLayout();
    hl_hart->setContentsMargins(0,0,0,0);
    hl_hart->setSpacing(0);
    hl_hart->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hl_hart->addSpacing(3);
    hl_hart->addWidget(btn_play_list_add_icon);
    hl_hart->addWidget(btn_heart);
    hl_hart->addWidget(btn_icon_info);
    //hl_hart->addWidget(btn_icon_menu);

    // ------------------------------------------------------
    //  전체 재생 & 셔플 재생
    // ------------------------------------------------------
    QPixmap pixmap_allPlay(":/images/icon_list_allplay.png");
    QPushButton *btn_icon_list_allplay = new QPushButton();
    btn_icon_list_allplay->setFixedSize(96,30);
    btn_icon_list_allplay->setText(tr("Play All"));
    // btn_icon_list_allplay->setText(tr("전체재생"));
    btn_icon_list_allplay->setIcon(pixmap_allPlay);
    btn_icon_list_allplay->setIconSize(pixmap_allPlay.rect().size());
    btn_icon_list_allplay->setCursor(Qt::PointingHandCursor);
    btn_icon_list_allplay->setStyleSheet("font-size:16px;color:#FFFFFF;");


    QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
    QPushButton *btn_icon_list_shuffle = new QPushButton();
    //btn_icon_list_shuffle->setFixedSize(96,30);
    btn_icon_list_shuffle->setText(tr("Shuffle Play"));
    //btn_icon_list_shuffle->setText(tr("셔플재생"));
    btn_icon_list_shuffle->setIcon(pixmap_shuffle);
    btn_icon_list_shuffle->setIconSize(pixmap_shuffle.rect().size());
    btn_icon_list_shuffle->setCursor(Qt::PointingHandCursor);
    btn_icon_list_shuffle->setStyleSheet("font-size:16px;color:#FFFFFF;");


    QHBoxLayout *hl_playType = new QHBoxLayout();
    hl_playType->setContentsMargins(0,0,0,0);
    hl_playType->setSpacing(0);
    hl_playType->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hl_playType->addSpacing(18);
    hl_playType->addWidget(btn_icon_list_allplay);
    hl_playType->addSpacing(30);
    hl_playType->addWidget(btn_icon_list_shuffle);


    // ------------------------------------------------------
    //  앨범 기본 정보 박스 구성 및 레이아웃 세팅
    // ------------------------------------------------------
    QVBoxLayout *vl_albumInfo = new QVBoxLayout();
    vl_albumInfo->setContentsMargins(0,0,0,0);
    vl_albumInfo->setSpacing(0);
    //vl_albumInfo->setAlignment(Qt::AlignLeft | Qt::AlignBottom);    // VCenter 가 아님
    vl_albumInfo->setAlignment(Qt::AlignVCenter);
    vl_albumInfo->addWidget(this->lb_title);
    vl_albumInfo->addSpacing(10);
    vl_albumInfo->addWidget(this->lb_artist);
    vl_albumInfo->addLayout(hl_star);
    vl_albumInfo->addSpacing(12);
    vl_albumInfo->addLayout(hl_hart);
    vl_albumInfo->addSpacing(10);
    vl_albumInfo->addLayout(hl_playType);
    //vl_albumInfo->addSpacing(20);

    QHBoxLayout *hl_albumInfo = new QHBoxLayout();
    hl_albumInfo->setContentsMargins(0,0,0,0);
    hl_albumInfo->setSpacing(0);
    hl_albumInfo->addWidget(this->lb_albumImg);
    hl_albumInfo->addSpacing(10);
    hl_albumInfo->addLayout(vl_albumInfo);

    // tab
    QJsonArray *p_jsonArray_titlSub = new QJsonArray();
    QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_FRIEND} };
    //QJsonObject sub1 { {"name", tr("트랙")}, {"code", ALBTAB_STEP_FRIEND} };
    QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_RECEIVE} };
    //QJsonObject sub2 { {"name", tr("크레딧")}, {"code", ALBTAB_STEP_RECEIVE} };
    QJsonObject sub3 { {"name", tr("Recommended Album")}, {"code", ALBTAB_STEP_SEND} };
    //QJsonObject sub3 { {"name", tr("추천앨범")}, {"code", ALBTAB_STEP_SEND} };
    p_jsonArray_titlSub->push_back(sub1);
    //p_jsonArray_titlSub->push_back(sub2);
    //p_jsonArray_titlSub->push_back(sub3);
    this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
    this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

    // 하단 StackWidget 안에 들어갈 위젯 3개 생성
    this->vl_tracks = new QVBoxLayout();
    this->vl_tracks->setContentsMargins(0,0,0,0);
    this->vl_tracks->setSpacing(0);
    this->vl_tracks->setAlignment(Qt::AlignTop);
    this->vl_cradit = new QVBoxLayout();
    this->vl_cradit->setContentsMargins(0,0,0,0);
    this->vl_cradit->setSpacing(0);
    this->vl_cradit->setAlignment(Qt::AlignTop);

    this->fl_albumRecommend = new FlowLayout();
    this->fl_albumRecommend->setContentsMargins(0,0,0,0);
    this->fl_albumRecommend->setSpacing(0);
    QWidget *widget_tab_tracks = new QWidget();
    QWidget *widget_tab_cradit = new QWidget();
    QWidget *widget_tab_albumRecommend = new QWidget();
    widget_tab_tracks->setLayout(this->vl_tracks);
    widget_tab_cradit->setLayout(this->vl_cradit);
    widget_tab_albumRecommend->setLayout(this->fl_albumRecommend);

    // 하단 StackWidget
    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(widget_tab_tracks);
    this->stackedwidget->addWidget(widget_tab_cradit);
    this->stackedwidget->addWidget(widget_tab_albumRecommend);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(33,10,28,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addLayout(hl_albumInfo);
    vl_total->addSpacing(30);
    vl_total->addWidget(this->menubar);
    vl_total->addWidget(this->stackedwidget);


    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(8,0,5,100);
    widget_scrollArea->setLayout(vl_total);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);

    // 커넥션
    connect(btn_heart, SIGNAL(clicked()), this, SLOT(slot_requestAlbumFavorite()));
    connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));
    connect(btn_icon_info, SIGNAL(clicked()), this, SLOT(slot_openDlgInfo()));
    connect(btn_play_list_add_icon, &QPushButton::clicked, this, &AlbumDetail::slot_addPlayListTrack);
    connect(btn_icon_list_allplay, &QPushButton::clicked, this, &AlbumDetail::slot_playAll);
    connect(btn_icon_list_shuffle, &QPushButton::clicked, this, &AlbumDetail::slot_playShuffle);
}

/**
     * @brief AlbumDetail::setFavoriteUI : (하트)즐겨찾기 UI 아이콘 세팅
     */
void AlbumDetail::setFavoriteUI(){

    if(isFavorite){
        btn_heart->setIcon(pixmap_heart_on);
    }else{
        btn_heart->setIcon(pixmap_heart_off);
    }
}

/**
     * @brief AlbumDetail::setStarUI : 별점 세팅
     */
void AlbumDetail::setStarPointUI(){

    for(int i= 0 ; i < max_star_point; i++){
        if(i < cntStar){
            list_lb_star->at(i)->setPixmap(*GSCommon::getUIPixmapImg(":/images/star_icon_on.png"));
        }else{
            list_lb_star->at(i)->setPixmap(*GSCommon::getUIPixmapImg(":/images/star_icon_off.png"));
        }
    }
}


/**
     * @brief AlbumDetail::loadImage [SLOT] 앨범 이미지 세팅
     */
void AlbumDetail::slot_loadImage(){

    bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap tmp_pixmap = this->pixmap_albumImg->scaled(ALBUMIMG_SIZE, ALBUMIMG_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 빈 Pixmap
        QPixmap pixmap_painter = QPixmap(QSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);
        QBrush brush = QBrush(tmp_pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, ALBUMIMG_SIZE, ALBUMIMG_SIZE, 4, 4);    // border-radius:4px
        this->lb_albumImg->setPixmap(pixmap_painter);
    }else{
        pixmap_albumImg = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");
    }    

    this->fileDownLoader->deleteLater();
}


/**
     * @brief AlbumDetail::slot_openDlgInfo [SLOT] 파일 인포 다이얼로그
     */
void AlbumDetail::slot_openDlgInfo(){

    // 시그널 발생 : 페이지가 변경되었다고 부모에게 알려준다.
    QJsonObject tmp_data = this->getDataJson();
    tmp_data[KEY_PAGE_CODE] = PAGECODE_OP_METAINFO;
    emit signal_clickedMovePage(tmp_data);
}


/**
     * @brief OptMorePopup::ClickMode 에 따라 Rose로 요청할 playType 정보를 int 로 반환한다.
     * @details Tidal의 음원, 비디오 모두 똑같다. playType 옵션값이.   음악, 비디오 등의 메뉴에서도 똑같은지는 확인해봐야.
     * @param clickMode
     * @return
     */
int AlbumDetail::get_rose_playType(OptMorePopup::ClickMode clickMode){//c211213


            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
            || clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
            ){
        // 바로 재생을 요청하는 타입
        return 15;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
            ){
        // 현재 Queue 끝에 추가
        return 13;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
            ){
        // 큐를 비우고 추가(재생)
        return 12;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
            ){
        return 16;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
        // 여기서부터 재생
        return 17;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
        // 큐 비우고 여기서부터 재생
        return 18;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
        // 여기서부터 큐 끝에 추가
        return 19;
    }
    else{
        return 0;           // unknown or not supported
    }
}


/**
     * @brief AlbumDetail::slot_playAll [SLOT] 전체재생
     */
void AlbumDetail::slot_playAll(){//c211213

    QJsonArray array_music;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        array_music.append(this->list_audioInfo.at(i)->getJsonData());
    }
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    //int playType = this->get_rose_playType(this->curr_clickMode);//c211213
    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");
        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 0);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    network->request(HTTP_PLAY_ALL, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
}


/**
     * @brief AlbumDetail::slot_playShuffle [SLOT] 셔플재생
     */
void AlbumDetail::slot_playShuffle(){//c211213

    QJsonArray array_music;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        array_music.append(this->list_audioInfo.at(i)->getJsonData());
    }
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    if(global.Queue_track_count != 0) {
        print_debug();emit linker->signal_checkQueue(27, "");

        return;
    }
    print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
    global.Queue_track_count += array_music.count();     // 220419 queue count

    QJsonObject tmp_json;
    tmp_json.insert("music", array_music);
    tmp_json.insert("musicPlayType", 15);
    tmp_json.insert("currentPosition", 0);
    tmp_json.insert("shuffle", 1);
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    network->request(HTTP_PLAY_SHUFFLE, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
}


/**
     * @brief AlbumDetail::slot_addPlayListTrack [SLOT]플레이리스트 담기
     */
void AlbumDetail::slot_addPlayListTrack(){
    proc_add->setProperty("index",-1);
    proc_add->showDlgOfPlayList();
}


/**
     * @brief AlbumDetail::slot_clickedStar [SLOT] 별 점수 클릭시 처리
     * @note 별 정보 GET/SET 은....  음악(트랙/앨점)과 로즈튜브에서만 별 정보 사용한다.
     */
void AlbumDetail::slot_clickedStar(){

    int tmp_index = this->list_lb_star->indexOf(dynamic_cast<ClickableLabel*>(this->sender()));
    this->cntStarTmp = cntStar;
    this->cntStar = tmp_index+1;
    setStarPointUI();

    QJsonObject tmp_json;
    tmp_json.insert("starRate", this->cntStar);
    tmp_json.insert("id", this->list_audioInfo.first()->getAlbum_id());   // 앨범 PK
    tmp_json.insert("isAlbum", true);                               // 오디오냐 앨범이냐
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_SET_STAR, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_set), tmp_json, true, true);


}


/**
     * @brief AlbumDetail::setTitleAlbum 앨범명 없는경우... 재세팅
     */
void AlbumDetail::setTitleAlbum(){

    if(this->albumName==""){
        if(this->list_audioInfo.count()>0){
            this->albumName = this->list_audioInfo.first()->getAlbum();

            // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
            emit signal_changedMenuName(this->albumName);
            this->lb_title->setText(this->albumName);
        }
    }
}


/**
     * @brief AlbumDetail::changedOnlyTabUI_notSendSignal 페이지 변경 및 데이터 세팅 (부모에게 페이지 변경되엇다는 시그널 없음)
     * @param p_step
     */
void AlbumDetail::changedOnlyTabUI_notSendSignal(QString p_step){

    this->contentStep = p_step;

    // Tab 타이틀 변경
    this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

    // DB 가져와서 데이터 세팅
    if(this->contentStep==ALBTAB_STEP_FRIEND){
        this->setDataTrackFromDB();
        this->stackedwidget->setCurrentIndex(0);
        this->setTitleAlbum();  // 앨범명 타이틀 없는경우 재세팅한다.
    }else if(this->contentStep==ALBTAB_STEP_RECEIVE){
        this->setDataCraditFromDB();
        this->stackedwidget->setCurrentIndex(1);
    }else if(this->contentStep==ALBTAB_STEP_SEND){
        this->setDataRecommendFromDB();
        this->stackedwidget->setCurrentIndex(2);
    }
}


/**
     * @brief AlbumDetail::changedSubTab [SLOT][override]
     * @param p_data
     */
void AlbumDetail::slot_changedSubTabUI(const QJsonObject &p_data){

    QString tmp_step = p_data[KEY_PAGE_CODE].toString();

    // 페이지 변경 및 데이터 세팅
    this->changedOnlyTabUI_notSendSignal(tmp_step);

    // 시그널 발생 : 페이지가 변경되었다고 부모에게 알려준다.
    QJsonObject tmp_data = this->getDataJson();
    tmp_data[KEY_CONTENT_STEP] = tmp_step;
    emit clickedSubTab(tmp_data);
}


/**
     * @brief AlbumDetail::setDataTrackFromDB 탭 [트랙] 데이터 가져와서 UI 세팅
     */
void AlbumDetail::setDataTrackFromDB(){

    // 초기화
    GSCommon::clearLayout(this->vl_tracks);
    this->list_audioInfo = QList<DataPopup*>();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id WHERE A.album_id=%1 ORDER BY A.track ";

        QVariantList data;
        sqlite->exec(strQuery.arg(this->album_id), data);

        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(data.at(i).toJsonObject());
                this->list_audioInfo.append(tmp_dataPopup);
            }

            for(int j = 0; j < this->list_audioInfo.size(); j++){
                TrackInfoEdit *tmp_trackInfo = new TrackInfoEdit();
                tmp_trackInfo->setProperty("index", j);
                tmp_trackInfo->setDataTrackInfoEdit(this->list_audioInfo.at(j), this->albumImg);
                tmp_trackInfo->setDataTracksInfoEdit(this->list_audioInfo);

                connect(tmp_trackInfo, SIGNAL(signal_clickedAddPlaylist()), this, SLOT(slot_playlistClickedTrack()));

                this->vl_tracks->addWidget(tmp_trackInfo);
            }
        }
    }

    sqlite->close();
    delete sqlite;
}


/**
     * @brief AlbumDetail::setDataCraditFromDB 탭 [크레딧] 데이터 가져와서 UI 세팅
     */
void AlbumDetail::setDataCraditFromDB(){

    GSCommon::clearLayout(this->vl_cradit);

    QLabel *lb_text = new QLabel();
    lb_text->setContentsMargins(10,30,0,0);
    lb_text->setText("☞ 크레딧 API 없음. 안드로이드 어플에 화면 자체가 없음");
    lb_text->setStyleSheet("font-size:20px;color:#FFFFFF;");
    this->vl_cradit->addWidget(lb_text);
}


/**
     * @brief AlbumDetail::setDataRecommendFromDB 탭 [추천앨범] 데이터 가져와서 UI 세팅
     */
void AlbumDetail::setDataRecommendFromDB(){

    GSCommon::clearLayout(this->fl_albumRecommend);

    QLabel *lb_text = new QLabel();
    lb_text->setContentsMargins(10,30,0,0);
    lb_text->setText("☞ 추천앨범 API 없음. 안드로이드 어플에 화면 자체가 없음");
    lb_text->setStyleSheet("font-size:20px;color:#FFFFFF;");
    this->fl_albumRecommend->addWidget(lb_text);
}


/**
     * @brief AlbumDetail::setData [override] 기본정보 데이터 세팅
     * @note [참고] OptionPopup 클래스의 setData 받는 구조와 동일하다
     * @param p_data
     */
void AlbumDetail::setDataABSPage(QJsonObject p_data){

    // 초기화
    this->contentStep = "";
    this->type_optionMenu = OptionPopup::TypeMenu::Music_Etc_AlbumDetailTrack;
    this->albumImg = "";
    this->cntStar = 0;
    this->cntStarTmp = 0;
    this->list_audioInfo = QList<DataPopup*>();
    this->album_id = 0;
    this->albumName = "";
    this->artist = "";
    this->isFavorite = false;

    setStarPointUI();
    setFavoriteUI();

    // 공통 데이터 세팅
    AbstractPage::setDataABSPage(p_data);
    if(this->contentStep==""){
        this->contentStep = ALBTAB_STEP_FRIEND;
    }
    // 데이터 세팅
    if(p_data.contains(KEY_OP_TYPE)){
        //this->type_optionMenu = p_data[KEY_OP_TYPE].toInt();
        this->type_optionMenu = static_cast<OptionPopup::TypeMenu>(p_data[KEY_OP_TYPE].toInt());
    }
    if(p_data.contains(KEY_OP_albumImg)){
        this->albumImg = p_data[KEY_OP_albumImg].toString();

    }
    if(p_data.contains(KEY_OP_cntStar)){
        this->cntStar = p_data[KEY_OP_cntStar].toInt();
    }
    if(p_data.contains(KEY_OP_DATA)){
        QJsonArray tmp_arr = p_data[KEY_OP_DATA].toArray();
        //this->list_audioInfo = tmp_arr;
        for(int i=0; i<tmp_arr.count(); i++){
            DataPopup *tmp_dataPopup = new DataPopup();
            tmp_dataPopup->setJsonData(tmp_arr[i].toObject());
            this->list_audioInfo.append(tmp_dataPopup);
        }
    }
    if(this->list_audioInfo.count()>0){
        // 첫번째 audioInfo 를 옵션 팝업 화면에 보여질 데이터로 사용한다.
        this->album_id = this->list_audioInfo.first()->getAlbum_id();
        this->albumName = this->list_audioInfo.first()->getAlbum();
        this->artist = this->list_audioInfo.first()->getArtist();
        setAlbumkeyFromDB();
    }
    if(this->albumImg.isEmpty()){
        this->setAlbumArtFromDB();
    }


    // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
    emit signal_changedMenuName(this->albumName);


    // 이미지 경로 설정
    QString tmp_imgURL;
    if(albumImg.startsWith("http")){
        tmp_imgURL = albumImg;
    }else{
        tmp_imgURL = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->albumImg);
    }
    this->lb_albumImg->setPixmap(*this->pixmap_albumImg_default);
    if(this->albumImg!=""){
        // 앨범이미지 다운받아서 세팅 : loadImage() 참고
        this->fileDownLoader->setImageURL(QUrl(tmp_imgURL));
    }

    // UI에 데이터 세팅
    this->lb_title->setText(this->albumName);
    this->lb_artist->setText(this->artist);

    // 앨범 별 GET
    this->requestIsStar();

    // 트랙 하트 GET
    this->requestIsfavorite();

    // step 있는 경우
    this->changedOnlyTabUI_notSendSignal(this->contentStep);
}


void AlbumDetail::saveAlbumImgToFile(const QString &p_playUrl){

    qint64 tmp_time = QDateTime::currentMSecsSinceEpoch();

    //QString tmp_fileName = QString("%1/%2").arg(dir.path()).arg(QString::number(tmp_time)+".jpg");
    QString tmp_savedfileName = "tmp_thumb.jpg";

    //QFile file(tmp_savedfileName);
    QFile file(QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).arg(tmp_savedfileName));
    file.open(QIODevice::WriteOnly);
    pixmap_albumImg->save(&file, "JPG");
    file.close();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    multiPart->setBoundary("356bb3c7-a864-48f1-8dc3-5d7965188eff");

    QHttpPart playUrlPart;
    playUrlPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"playUrl\""));
    playUrlPart.setBody(p_playUrl.toUtf8());

    QHttpPart macPart;
    macPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"macAddress\""));
    macPart.setBody(global.device.getDeviceID().toUtf8());

    fileUpload = new QFile(tmp_savedfileName);
    fileUpload->open(QIODevice::ReadOnly);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+QString::number(tmp_time)+".jpg\""));
    imagePart.setBodyDevice(fileUpload);
    fileUpload->setParent(multiPart);

    multiPart->append(playUrlPart);
    multiPart->append(macPart);
    multiPart->append(imagePart);

    QUrl url("https://api.roseaudio.kr/legacy/v3/file/trackthumbnail");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; charset=utf-8; boundary=\"356bb3c7-a864-48f1-8dc3-5d7965188eff\"");
    QString header = global.user.getToken_type()+" "+global.user.getAccess_token();
    request.setRawHeader("Authorization", header.toUtf8());


    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, multiPart);
    reply->setProperty("filename", tmp_savedfileName);
    multiPart->setParent(reply);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_networkManagerFinished()));
    connect(reply, SIGNAL(finished()), this, SLOT(slot_replyFinished()));
}


/**
     * @brief AlbumDetail::getDataJson [override]
     * @return
     */
QJsonObject AlbumDetail::getDataJson(){

    // 공통 데이터 반환
    QJsonObject tmp_data = AbstractPage::getDataJson();

    // 데이터 반환
    tmp_data[KEY_OP_TITLE_MAIN] = "오디서사용하는거니?(0821)";
    tmp_data[KEY_OP_TITLE_SUB] = "오디서사용하는거니?(0821)";
    tmp_data[KEY_OP_albumImg] = this->albumImg;
    tmp_data[KEY_OP_cntStar] = this->cntStar;
    tmp_data[KEY_OP_TYPE] = this->type_optionMenu;
    QJsonArray tmp_array;
    for(int i=0; i<this->list_audioInfo.count(); i++){
        tmp_array.append(this->list_audioInfo.at(i)->getJsonData());
    }
    tmp_data[KEY_OP_DATA] = tmp_array;

    return tmp_data;
}


/**
     * @brief AlbumDetail::getPlayUrlList : 트랙 재생 url 반환
     * @param p_index if -1, 앨범 전체
     * @return  QStringList
     */
QStringList AlbumDetail::getPlayUrlList(const int &p_index){

    QStringList listUrl;

    if(p_index < 0){
        for(int i = 0 ; i < list_audioInfo.size(); i++){
            listUrl.append(list_audioInfo.at(i)->getData());
        }
    }else{
        listUrl.append(list_audioInfo.at(p_index)->getData());
    }

    return listUrl;
}


/**
     * @brief AlbumDetail::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
     * @param p_index if -1, 앨범 전체
     * @return QJsonArray
     */
QJsonArray AlbumDetail::getJsonArr_TrackData(const int &p_index){

    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < list_audioInfo.size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }
    return jsonArray;
}


/**
     * @brief AlbumDetail::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
     * @param p_index
     * @return QJsonObject
     */
QJsonObject AlbumDetail::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < list_audioInfo.size()){
        DataPopup *dataPopup = list_audioInfo.at(p_index);

        // 앨범
        jsonTrack.insert("album", dataPopup->getAlbum());

        // 아티스트
        QJsonObject jsonArtist;
        jsonArtist.insert("name", dataPopup->getArtist());
        QJsonArray jsonArrArtists;
        jsonArrArtists.append(jsonArtist);
        jsonTrack.insert("artists", jsonArrArtists);

        // 클라이언트 아이디 = 오디오 아이디
        jsonTrack.insert("client_id", dataPopup->getId());

        // 썸네일
        QJsonObject jsonThumb;
        jsonThumb.insert("url", albumImg);

        QJsonArray jsonArrThumbs;
        jsonArrThumbs.append(jsonThumb);
        jsonTrack.insert("thumbnails", jsonArrThumbs);

        // 타이틀
        jsonTrack.insert("title", dataPopup->getTitle());

        // 재생 URL
        jsonTrack.insert("url", dataPopup->getData());

        jsonTrack.insert("isFavorite", false);
        jsonTrack.insert("isSelected", false);
        jsonTrack.insert("mac_address", global.device.getDeviceID());
        jsonTrack.insert("type","music");
    }

    return jsonTrack;
}



/**
     * @brief AlbumDetail::getAlbumArt : 앨범 이미지 세팅
     */
void AlbumDetail::setAlbumArtFromDB(){

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT _data FROM album_art WHERE album_id=%1";
        QVariantList data;
        sqlite->exec(strQuery.arg(this->album_id), data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();
                if(tmp_data.contains("_data")){
                    this->albumImg = tmp_data["_data"].toString();
                }
            }
        }
    }

    sqlite->close();
    delete sqlite;
}


/**
     * @brief AlbumDetail::setAlbumkeyFromDB : 앨범 키 세팅
     */
void AlbumDetail::setAlbumkeyFromDB(){

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT album_key FROM audio WHERE album_id=%1 LIMIT 1";
        QVariantList data;
        sqlite->exec(strQuery.arg(this->album_id), data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();
                if(tmp_data.contains("album_key")){
                    this->albumKey = tmp_data["album_key"].toString();
                }
            }
        }
    }

    sqlite->close();
    delete sqlite;
}


/**
     * @brief AlbumDetail::requestIsStar 앨범 별 정보 가져오는 API 호출
     */
void AlbumDetail::requestIsStar(){

    // 별 정보 GET : 음악(트랙/앨범)과 로즈튜브에서만 별 정보 사용한다.
    //if(this->mainMenuCode==GSCommon::MainMenuCode::Music){
    if(this->list_audioInfo.count()>0){

        QJsonObject tmp_json;
        tmp_json.insert("id", this->list_audioInfo.first()->getAlbum_id());    // 오디오 PK
        tmp_json.insert("isAlbum", true);                                // 오디오냐 앨범이냐

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_GET_STAR, QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port).arg(global.api_music_rate_get), tmp_json, true, true);
    }
    //}
}


/**
     * @brief AlbumDetail::requestIsfavorite 트랙 하트 가져오는 API 호출
     */
void AlbumDetail::requestIsfavorite(){

    if(!global.device.getDeviceIP().isEmpty()){
        QJsonObject tmp_json;
        tmp_json.insert("album_key", this->albumKey);
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3).arg(global.api_album_favorite_get), tmp_json, true, true);
    }
}


/**
     * @brief AlbumDetail::slot_requestAlbumFavorite 앨범 하트 세팅 API 호출 (설정/해제)
     */
void AlbumDetail::slot_requestAlbumFavorite(){

    isFavorite = isFavorite ? false : true;
    setFavoriteUI();

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject jsonParam;
    jsonParam.insert("local_ip", global.device.getDeviceIP());
    jsonParam.insert("mac_address", global.device.getDeviceID());

    long local_key = QDateTime::currentMSecsSinceEpoch();

    QJsonObject jsonPlaylist;
    jsonPlaylist.insert("album_key", albumKey);
    jsonPlaylist.insert("isFavorite", isFavorite);
    jsonPlaylist.insert("isFavoriteTrack", false);
    jsonPlaylist.insert("local_key", QString::number(local_key));
    jsonPlaylist.insert("name", albumName);
    jsonPlaylist.insert("share","0");
    jsonPlaylist.insert("star", cntStar);
    jsonPlaylist.insert("totalcount", list_audioInfo.size());
    jsonPlaylist.insert("tracks", getJsonArr_TrackData(-1));
    jsonPlaylist.insert("type", "album");

    jsonParam.insert("playlist", jsonPlaylist);

    network->request(HTTP_FAVORITE_SET, QString("%1%2").arg(global.legacy_v3).arg(global.api_album_favorite_set), jsonParam, true, true);
}


/**
     * @brief AlbumDetail::slot_responseHttp : Http 요청 결과 처리 함수
     * @param p_id int 요청 아이디
     * @param p_jsonObject QJsonObject response
     */
void AlbumDetail::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    switch(p_id){
    case HTTP_GET_STAR :
        setResultOfGetStar(p_jsonObject);
        break;
    case HTTP_SET_STAR :
        setResultOfSetStar(p_jsonObject);
        break;
    case HTTP_FAVORITE_GET:
        if(p_jsonObject.contains("favorite")){
            this->isFavorite = p_jsonObject["favorite"].toBool();
            setFavoriteUI();
        }
        break;
    case HTTP_FAVORITE_SET:
        setResultOfSetFavorites(p_jsonObject);
        break;
    }


    sender()->deleteLater();
}


/**
     * @brief AlbumDetail::setResultOfGetStar : 별정소 Get 요청결과 처리
     * @param p_jsonData QJsonObjecgt response
     */
void AlbumDetail::setResultOfGetStar(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();

            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs) && jsonStatus[jsonKey_outs].toString().toLower()=="ok"){

            if(p_jsonObject.contains("starRate")){
                this->cntStar = p_jsonObject["starRate"].toInt();
                setStarPointUI();
            }
            //}
        }
    }
}


/**
     * @brief AlbumDetail::setResultOfSetStar : 별정보 Set 요청결과처리
     * @param p_jsonObject QJsonObject response
     */
void AlbumDetail::setResultOfSetStar(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonObject[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
            // noting
            flagOk = true;
            //}
        }
    }

    if(!flagOk){
        cntStar = cntStarTmp;
        setStarPointUI();

        ToastMsg::show(this,"",tr("You cannot change the star rating"));
        //ToastMsg::show(this,"",tr("별점을 변경할 수 없습니다."));
    }
}


/**
     * @brief AlbumDetail::setResultOfSetFavorites : 앨범 하트 SET 요청결과 처리
     * @param p_jsonObject QJsonObject response
     */
void AlbumDetail::setResultOfSetFavorites(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";

    bool flagOk = false;

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_status) && p_jsonObject[jsonKey_status].toString().toLower()=="ok"){
            // noting
            flagOk = true;
        }
    }

    if(!flagOk){
        // -------------------------------------
        //  이전 상태 되돌리기
        // -------------------------------------
        isFavorite = isFavorite ? false : true;
        setFavoriteUI();

        ToastMsg::show(this,"",tr("You cannot change your favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}


/**
     * @brief AlbumDetail::setResultOfAddPlayListTrack : 플레이리스트 곡 추가 요청결과처리
     * @param p_jsonData
     */
void AlbumDetail::setResultOfAddPlayListTrack(const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";

    bool flagOk = false;
    QString tmp_errorMsg = "";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
            emit linker->signal_myPlaylistChanged();
        }else{
            if(p_jsonData.contains(jsonKey_message)){
                tmp_errorMsg = p_jsonData[jsonKey_message].toString();
            }
        }
    }

    // show Error
    if(!flagOk){
        if(tmp_errorMsg.isEmpty()){
            tmp_errorMsg = tr("It cannot be added to the playlist.");
            //tmp_errorMsg = tr("플레이리스트에 추가할 수 없습니다.");
        }
        ToastMsg::show(this,"",tmp_errorMsg);
    }
}


/**
     * @brief AlbumDetail::setResultOfNewPlayListTrack : 플레이리스트 생성후 곡 추가 요청결과처리
     * @param p_jsonData
     */
void AlbumDetail::setResultOfNewPlayListTrack(const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";

    bool flagOk = false;
    QString tmp_errorMsg = "";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;


            saveAlbumImgToFile(list_audioInfo.at(0)->getData());
        }else{
            if(p_jsonData.contains(jsonKey_message)){
                tmp_errorMsg = p_jsonData[jsonKey_message].toString();
            }
        }
    }

    // show Error
    if(!flagOk){
        if(tmp_errorMsg.isEmpty()){
            tmp_errorMsg = tr("You cannot create a playlist.");
            //tmp_errorMsg = tr("플레이리스트를 생성할 수 없습니다.");
        }
        ToastMsg::show(this,"",tmp_errorMsg);
    }
}


/**
     * @brief AlbumDetail::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
     * @param p_playlistno
     */
void AlbumDetail::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_add->requestAddPlayListTrack(p_playlistno, getJsonArr_TrackData(tmp_index));

        // ------------------------------------------------
        //  썸네일 업로드
        // ------------------------------------------------
        QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(albumImg);
        Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
        proc_uploadThumb->uploadThumbnails(imgPath, getPlayUrlList(tmp_index));
        connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));

    }else{
        proc_add->showDlgOfNewPlayList();
    }
}


/**
     * @brief AlbumDetail::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
     * @param p_name
     * @note 신규 플레이리스트 생성
     */
void AlbumDetail::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    QString tmp_playUrl;
    if(tmp_index <=0){
        tmp_playUrl = list_audioInfo.at(0)->getData();
    }else{
        tmp_playUrl = list_audioInfo.at(tmp_index)->getData();
    }

    proc_add->requestNewPlaylistTrack(*pixmap_albumImg, tmp_playUrl, p_name, getJsonArr_TrackData(tmp_index));
}


/**
     * @brief AlbumDetail::slot_playlistClickedTrack : [슬롯]트랙 플레이리스트 담기 클릭
     */
void AlbumDetail::slot_playlistClickedTrack(){

    int tmp_index = sender()->property("index").toInt();
    proc_add->setProperty("index",tmp_index);
    proc_add->showDlgOfPlayList();
}

