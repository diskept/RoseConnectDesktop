#include "musiccollection.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "widget/framemyplaylist.h"
#include "widget/framefriendplaylist.h"
#include "widget/VerticalScrollArea.h"
#include "widget/toastmsg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScroller>
#include "common/global.h"
#include <common/sqlitehelper.h>
#include "widget/optionpopup.h"
#include <login/dialoglogin.h>
#include "common/rosesettings.h"
#include <QSettings>

const int HTTP_FAVORITE_TRACKS = 99;
const int HTTP_MY_PLAYLIST = 98;
const int HTTP_FRIEND_PLAYLIST = 97;
const int HTTP_NETWORK_PLAY = 96;

/**
 * @brief MusicCollection::MusicCollection : 음악-마이컬렉션
 * @note API 미지원, 디자인 시안과 다름</br>
 * App의 Favor 참고하여, Favor 트랙리스트, 마이,친구 플레이리스로 구성
 * @param parent
 */
MusicCollection::MusicCollection(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

MusicCollection::~MusicCollection(){
    this->listWidget->clear();
    GSCommon::clearLayout(this->boxMyPlayList);
    GSCommon::clearLayout(this->boxFriendPlayList);
    GSCommon::clearLayout(this->boxContents);
}

/**
 * @brief MusicCollection::setInit : 초기 세팅 함수
 */
void MusicCollection::setInit(){
    linker = Linker::getInstance();
    connect(linker, SIGNAL(signal_logined()), SLOT(slot_requestData()));
    connect(linker, SIGNAL(signal_myPlaylistChanged()), SLOT(slot_myPlaylistChanged()));
    connect(linker, SIGNAL(signal_favoriteTrackChanged()), SLOT(slot_favoriteTrackChanged()));

    delegateTracks = new MusicCollectionTrackDelegate(this);
    connect(delegateTracks, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));

    proc_addPlaylist = new Proc_AddTackInPlayList(this);
    connect(proc_addPlaylist, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_addPlaylist, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    listPopup = new QList<DataPopup*>();

}


/**
 * @brief MusicCategory::setUIControl : UI 세팅
 */
void MusicCollection::setUIControl(){

    boxContents = new QVBoxLayout();
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(0,0,0,0);
    boxContents->setAlignment(Qt::AlignTop);

    QWidget *widgetContents = new QWidget();
    widgetContents->setLayout(boxContents);
    widgetContents->setContentsMargins(22,0,0,22);

    setLoginInfoUI();
    setFavorTrackListUI();
    setMyPlayListUI();
    setFriendPlayListUI();

    scrollArea_content = new VerticalScrollArea();
    scrollArea_content->setWidget(widgetContents);
    scrollArea_content->setWidgetResizable(false);
    scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout();
    boxMain->setContentsMargins(0,0,0,0);
    boxMain->addWidget(scrollArea_content);
    boxMain->addWidget(widgetLogin);
    boxMain->setSizeConstraint(QLayout::SetNoConstraint);

    setLayout(boxMain);
}

/**
 * @brief MusicCollection::setLoginInfoUI 로그인 버튼 UI 세팅
 */
void MusicCollection::setLoginInfoUI(){


    QLabel *label_topComment = new QLabel(tr("You can use various services by logging in."));
    //QLabel *label_topComment = new QLabel(tr("로그인하시면 다양한 서비스를 이용하실 수 있습니다."));
    label_topComment->setStyleSheet("font-size: 18px; color: #ffffff;");
    label_topComment->setContentsMargins(0,0,0,0);
    label_topComment->setAlignment(Qt::AlignCenter);
    QPushButton *btn_login = new QPushButton();
    btn_login->setText(tr("Log-in"));//btn_login->setText(tr("로그인"));
    btn_login->setFixedSize(140,40);
    btn_login->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px; padding-left:44px; padding-right:44px;");
    btn_login->setCursor(Qt::PointingHandCursor);
    btn_login->setContentsMargins(0,0,0,0);
    connect(btn_login, SIGNAL(clicked()), this, SLOT(slot_clickedLogin()));
    QVBoxLayout *vl_loginBtn = new QVBoxLayout();
    vl_loginBtn->setContentsMargins(0,0,0,0);
    vl_loginBtn->setSpacing(0);
    vl_loginBtn->addWidget(label_topComment);
    vl_loginBtn->addSpacing(30);
    vl_loginBtn->addWidget(btn_login, 0, Qt::AlignCenter);
    vl_loginBtn->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    widgetLogin = new QWidget();
    widgetLogin->setLayout(vl_loginBtn);
}
/**
 * @brief MusicCollection::setFavorTrackListUI : 즐겨찾기 트랙 UI 세팅
 */
void MusicCollection::setFavorTrackListUI(){
    QLabel *labelSubTitle_1 = new QLabel;
    labelSubTitle_1->setText(tr("Track"));//labelSubTitle_1->setText(tr("트랙"));
    labelSubTitle_1->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_COLLECTION_TRACK);
    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,0,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_1);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);


    // -----------------------------------------------------
    //  트랙 목록 ListWidget
    // -----------------------------------------------------
    listWidget = new QListWidget(this);
    listWidget->setItemDelegate(delegateTracks);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("border:none;");
    listWidget->setFixedHeight(350);


    QVBoxLayout *box_total = new QVBoxLayout();
    box_total->setSpacing(0);
    box_total->setContentsMargins(5,0,28,0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(widget_top);
    box_total->addSpacing(5);
    box_total->addWidget(listWidget);

    widgetTracks = new QWidget(this);
    widgetTracks->setContentsMargins(0,10,0,30);
    widgetTracks->setLayout(box_total);


    boxContents->addWidget(widgetTracks);

}


/**
 * @brief MusicCollection::setMyPlayListUI : My 플레이리스트 UI 세팅
 */
void MusicCollection::setMyPlayListUI(){
    QLabel *labelSubTitle_1 = new QLabel;
    labelSubTitle_1->setText(tr("My playlist"));//labelSubTitle_1->setText(tr("My 플레이리스트"));
    labelSubTitle_1->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_MYPLAYLIST);
    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,28,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_1);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);

    boxMyPlayList = new QHBoxLayout;
    boxMyPlayList->setSpacing(20);
    boxMyPlayList->setContentsMargins(10,10,0,0);
    boxMyPlayList->setSizeConstraint(QLayout::SetFixedSize);

    QWidget *widgetAlbum = new QWidget;
    widgetAlbum->setLayout(boxMyPlayList);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widgetAlbum);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);

    lb_emptyMyPlayList = new QLabel;
    //lb_emptyMyPlayList->setText(tr("My 플레이 리스트가 없습니다."));
    lb_emptyMyPlayList->setText(tr("There is no My Playlist."));
    lb_emptyMyPlayList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lb_emptyMyPlayList->setAlignment(Qt::AlignCenter);
    lb_emptyMyPlayList->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_emptyMyPlayList->setFixedHeight(286);

    QLabel *lb_login = new QLabel;
    lb_login->setText(tr("It is available after login."));
    //lb_login->setText(tr("로그인 후 이용하실 수 있습니다."));
    lb_login->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:18px;");

    QPushButton *btn_login = new QPushButton;
    btn_login->setText(tr("Log-in"));//btn_login->setText(tr("로그인"));
    btn_login->setFixedSize(QSize(140,40));
    btn_login->setStyleSheet("background-color:transparent;color:#B18658;border-radius:20px; border:1px solid #B18658;font-size:18px;");
    btn_login->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *box_login = new QVBoxLayout;
    box_login->addWidget(lb_login, 0, Qt::AlignHCenter);
    box_login->addWidget(btn_login, 0, Qt::AlignHCenter);
    box_login->setAlignment(Qt::AlignCenter);
    box_login->setSpacing(30);

    QWidget *wg_login = new QWidget();
    wg_login->setLayout(box_login);
    wg_login->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wg_login->setFixedHeight(286);

    stackMyPlayList = new QStackedWidget;
    stackMyPlayList->addWidget(wg_login);
    stackMyPlayList->addWidget(lb_emptyMyPlayList);
    stackMyPlayList->addWidget(scrollArea);

    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(0,0,0,5);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(stackMyPlayList);

    QWidget *wg_playList = new QWidget;
    wg_playList->setLayout(boxTotal);
    wg_playList->setStyleSheet("border:0px;");
    boxContents->addWidget(wg_playList);

    connect(btn_login, SIGNAL(clicked()), this, SLOT(slot_clickedLogin()));
}

/**
 * @brief MusicCollection::setFriendPlayListUI : 친구 플레이리스트 UI 세팅
 */
void MusicCollection::setFriendPlayListUI(){
    QLabel *labelSubTitle_1 = new QLabel;
    labelSubTitle_1->setText(tr("Friend playlist"));
    //labelSubTitle_1->setText(tr("친구 플레이리스트"));
    labelSubTitle_1->setStyleSheet("font-size:20px;color:#FFFFFF;margin-left:10px;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_FRIENDPLAYLIST);
    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,0,28,0);
    boxTop->setAlignment(Qt::AlignTop);
    boxTop->addWidget(labelSubTitle_1);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1);

    QWidget *widget_top = new QWidget;
    widget_top->setLayout(boxTop);



    boxFriendPlayList = new QHBoxLayout;
    boxFriendPlayList->setSpacing(20);
    boxFriendPlayList->setContentsMargins(10,10,0,0);
    boxFriendPlayList->setSizeConstraint(QLayout::SetFixedSize);

    QWidget *widgetAlbum = new QWidget;
    widgetAlbum->setLayout(boxFriendPlayList);


    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widgetAlbum);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);

    lb_emptyFriendPlayList = new QLabel;
    lb_emptyFriendPlayList->setText(tr("There is no friend playlist."));
    //lb_emptyFriendPlayList->setText(tr("친구 플레이 리스트가 없습니다."));
    lb_emptyFriendPlayList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lb_emptyFriendPlayList->setAlignment(Qt::AlignCenter);
    lb_emptyFriendPlayList->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_emptyFriendPlayList->setFixedHeight(308);

    QLabel *lb_login = new QLabel;
    lb_login->setText(tr("It is available after login."));
    //lb_login->setText(tr("로그인 후 이용하실 수 있습니다."));
    lb_login->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:18px;");

    QPushButton *btn_login = new QPushButton;
    btn_login->setText(tr("Log-in"));//btn_login->setText(tr("로그인"));
    btn_login->setFixedSize(QSize(140,40));
    btn_login->setStyleSheet("background-color:transparent;color:#B18658;border-radius:20px; border:1px solid #B18658;");
    btn_login->setCursor(Qt::PointingHandCursor);

    QVBoxLayout *box_login = new QVBoxLayout;
    box_login->addWidget(lb_login, 0, Qt::AlignHCenter);
    box_login->addWidget(btn_login, 0, Qt::AlignHCenter);
    box_login->setAlignment(Qt::AlignCenter);
    box_login->setSpacing(30);

    QWidget *wg_login = new QWidget();
    wg_login->setLayout(box_login);
    wg_login->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wg_login->setFixedHeight(308);

    stackFriendPlayList = new QStackedWidget;
    stackFriendPlayList->addWidget(wg_login);
    stackFriendPlayList->addWidget(lb_emptyFriendPlayList);
    stackFriendPlayList->addWidget(scrollArea);

    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(0,0,0,5);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(stackFriendPlayList);

    QWidget *wg_playList = new QWidget;
    wg_playList->setLayout(boxTotal);
    wg_playList->setStyleSheet("border:0px;");

    boxContents->addWidget(wg_playList);

    connect(btn_login, SIGNAL(clicked()), this, SLOT(slot_clickedLogin()));
}

/**
 * @brief MusicCollection::requestFavorTracks : 사용자 즐겨찾기(하트) 트랙 목록 요청
 */
void MusicCollection::requestFavorTracks(){

    listPopup->clear();
    listWidget->clear();

    NetworkHttp *network = new NetworkHttp(this);
    network->setProperty("uuid", uuid);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QJsonObject json;
    json.insert("local_ip", global.device.getDeviceIP());
    json.insert("mac_address", global.device.getDeviceID());
    json.insert("permission","1");
    network->request(HTTP_FAVORITE_TRACKS, QString("%1/playlist/0?page=0&size=5")
                     .arg(global.legacy_v3)
                     , json, true,true);
}


/**
 * @brief MusicCollection::requestMyPlayList : My 플레이리스트 요청
 */
void MusicCollection::requestMyPlayList(){
    if(!global.user.getAccess_token().isEmpty()){

        // 라벨 show
        GSCommon::clearLayout(boxMyPlayList);
        stackMyPlayList->setCurrentIndex(1);
        lb_emptyMyPlayList->setText(tr("Retrieving information."));
        //lb_emptyMyPlayList->setText(tr("정보를 가져오는 중입니다."));

        NetworkHttp *network = new NetworkHttp(this);
        network->setProperty("uuid", uuid);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        json.insert("local_ip", global.device.getDeviceIP());
        json.insert("mac_address", global.device.getDeviceID());
        network->request(HTTP_MY_PLAYLIST, QString("%1/playlist?page=0&size=10")
                         .arg(global.legacy_v3_api)
                         , json, true,true);
    }
}

/**
 * @brief MusicCollection::setResultOfFavoriteTrack : 사용자 즐겨찾기(하트) 트랙 요청결과 처리
 * @param p_jsonObject QJsonObject response
 */
void MusicCollection::setResultOfFavoriteTrack(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playlist = "playlist";
    const QString jsonKey_tracks = "tracks";
    const QString jsonKey_client_id = "client_id";
    const QString jsonKey_id = "id";
    const QString jsonKey_sort = "sort";
    const QString jsonKey_star = "star";
    const QString jsonKey_thumbnails = "thumbnails";
    const QString jsonKey_title = "title";
    const QString jsonKey_type = "type";
    const QString jsonKey_url = "url";
    const QString jsonKey_macaddress = "mac_address";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_playlist)){

            QJsonObject jsonPlaylist = p_jsonObject[jsonKey_playlist].toObject();

            if(jsonPlaylist.contains(jsonKey_tracks)){
                QJsonArray jsonArrTrack = jsonPlaylist[jsonKey_tracks].toArray();

                for(int i=0; i < jsonArrTrack.size(); i++){
                    QJsonObject jsonTrack = jsonArrTrack.at(i).toObject();

                    QString tmp_title;
                    QString tmp_thumbnail;
                    QString tmp_url;
                    QString tmp_mac_address;
                    QString tmp_client_id="";
                    int tmp_star=0;
                    int tmp_id=0;

                    if(jsonTrack.contains(jsonKey_title)){
                        tmp_title = jsonTrack[jsonKey_title].toString();
                    }
                    if(jsonTrack.contains(jsonKey_thumbnails)){
                        QJsonArray jsonArrThumb = jsonTrack[jsonKey_thumbnails].toArray();
                        if(jsonArrThumb.size() > 0){
                            tmp_thumbnail = jsonArrThumb.at(0).toObject()[jsonKey_url].toString();
                        }
                    }
                    if(jsonTrack.contains(jsonKey_url)){
                        tmp_url = jsonTrack[jsonKey_url].toString();
                    }
                    if(jsonTrack.contains(jsonKey_macaddress)){
                        tmp_mac_address = jsonTrack[jsonKey_macaddress].toString();
                    }
                    if(jsonTrack.contains(jsonKey_client_id)){
                        tmp_client_id = jsonTrack[jsonKey_client_id].toString();
                    }
                    if(jsonTrack.contains(jsonKey_star)){
                        tmp_star = jsonTrack[jsonKey_star].toInt();
                    }
                    if(jsonTrack.contains(jsonKey_id)){
                        tmp_id = jsonTrack[jsonKey_id].toInt();
                    }

                  
                    DataPopup *dataPopup;
                    if(!tmp_url.isEmpty()){
                        dataPopup = getTrackInfoFromDB(tmp_url);
                    }else{
                        dataPopup = new DataPopup(this);
                    }
                    dataPopup->setFavorite_id(tmp_id);
                    dataPopup->setProperty("thumbnail", tmp_thumbnail);
                    dataPopup->setProperty("url", tmp_url);
                    dataPopup->setProperty("mac_address", tmp_mac_address);

                    listPopup->append(dataPopup);


                    // --------------------------------------------------
                    //  UserRole 데이터 세팅
                    // --------------------------------------------------
                    QJsonObject json;
                    json.insert("index", i);
                    json.insert("title", tmp_title);
                    json.insert("artist", dataPopup->getArtist());
                    json.insert("album", dataPopup->getAlbum());
                    json.insert("isFavorite", "0");
                    json.insert("url", tmp_url);
                    json.insert("thumbnail", tmp_thumbnail);
                    json.insert("mac_address", tmp_mac_address);
                    json.insert("client_id", tmp_client_id);
                    json.insert("star", tmp_star);
                    json.insert("duration", GSCommon::convertTimeFormat(dataPopup->getDuration()));

                    QListWidgetItem *item = new QListWidgetItem;
                    item->setData(Qt::UserRole, json);
                    item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));

                    listWidget->addItem(item);

                    // --------------------------------------------------
                    //  썸네일 다운로드
                    // --------------------------------------------------
                    FileDownloader *fileDownloader = new FileDownloader(this);
                    fileDownloader->setProperty("uuid",uuid);
                    fileDownloader->setProperty("index", i);
                    fileDownloader->setImageURL(tmp_thumbnail);
                    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                }
            }
        }
    }
}

/**
 * @brief MusicCollection::getTrackInfoFromDB : GET 트랙 정보 FROM DB
 * @param p_clientId QString audio_id
 * @return
 */
DataPopup* MusicCollection::getTrackInfoFromDB(const QString &p_url){

    DataPopup *dataPopup = new DataPopup(this);

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
        strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        strQuery += " WHERE A._data='"+p_url+"'";
        QVariantList dataDB;
        sqlite->exec(strQuery, dataDB);
        if(dataDB.size() >0){
            dataPopup->setJsonData(dataDB.at(0).toJsonObject());
        }
    }

    sqlite->close();
    delete sqlite;

    return dataPopup;
}


/**
 * @brief MusicCollection::setResultOfMyPlayList : MY 플레이리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject
 */
void MusicCollection::setResultOfMyPlayList(const QJsonObject &p_jsonObject){

    bool flagOk = false;
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            flagOk = true;
            int tmp_cntPlayList = p_jsonObject[jsonKey_totalcount].toInt(0);

            if(tmp_cntPlayList){
                if(p_jsonObject.contains(jsonKey_playLists)){
                    QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();

                    for(int i = 0; i < jsonArr.size(); i++){
                        QJsonObject jsonPlayList = jsonArr.at(i).toObject();
                        appendMyPlayList(jsonPlayList);
                    }
                    stackMyPlayList->setCurrentIndex(2);
                }
            }else{
                lb_emptyMyPlayList->setText(tr("There is no My Playlist."));
                //lb_emptyMyPlayList->setText(tr("My 플레이 리스트가 없습니다."));
            }
        }
    }

    if(!flagOk){
        lb_emptyMyPlayList->setText(tr("My playlist information cannot be checked."));
        //lb_emptyMyPlayList->setText(tr("My 플레이 리스트 정보를 확인할 수 없습니다."));
    }
}

/**
 * @brief MusicCollection::setResultOfFrienPlayList : 친구 플레이리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject
 */
void MusicCollection::setResultOfFrienPlayList(const QJsonObject &p_jsonObject){

    bool flagOk = false;
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            flagOk = true;
            int tmp_cntPlayList = p_jsonObject[jsonKey_totalcount].toInt(0);

            if(tmp_cntPlayList){
                if(p_jsonObject.contains(jsonKey_playLists)){
                    QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();

                    for(int i = 0; i < jsonArr.size(); i++){
                        QJsonObject jsonPlayList = jsonArr.at(i).toObject();
                        appendFriendPlayList(jsonPlayList);
                    }
                    stackFriendPlayList->setCurrentIndex(2);
                }
            }else{
                lb_emptyFriendPlayList->setText(tr("There is no friend playlist."));
                //lb_emptyFriendPlayList->setText(tr("친구 플레이 리스트가 없습니다."));
            }
        }
    }

    if(!flagOk){
        lb_emptyFriendPlayList->setText(tr("I can't check my friend's playlist information."));
        //lb_emptyFriendPlayList->setText(tr("친구 플레이 리스트 정보를 확인할 수 없습니다."));
    }
}

/**
 * @brief MusicCollection::appendMyPlayList : My 플레이리스트 UI 추가
 * @param p_json QJsonObject 플레이리스트 정보
 */
void MusicCollection::appendMyPlayList(const QJsonObject &p_json){

    FrameMyPlayList *tmp_widget = new FrameMyPlayList;
    tmp_widget->setData(p_json);
    tmp_widget->setHover();
    boxMyPlayList->addWidget(tmp_widget);
}

/**
 * @brief MusicHome::appendFriendPlayList : 친구 플레이리스트 UI 추가
 * @param p_json QJsonObject 플레이리스트 정보
 */
void MusicCollection::appendFriendPlayList(const QJsonObject &p_json){

    FrameFriendPlayList *tmp_widget = new FrameFriendPlayList;
    tmp_widget->setData(p_json);
    tmp_widget->setHover();

    boxFriendPlayList->addWidget(tmp_widget);
}

/**
 * @brief MusicCollection::getJsonObject_OptionPopup : 옵션팝업에 사용할 데이터 반환
 * @param p_index
 * @return
 */
QJsonObject MusicCollection::getJsonObject_OptionPopup(const int &p_index){

    QJsonObject tmp_jsonObject;
    if(p_index >=0 && p_index < listPopup->count()){
        DataPopup *data = listPopup->at(p_index);
        QString tmp_thumbnail = data->property("thumbnail").toString();
        QString tmp_url = data->property("url").toString();
        QString tmp_macAddress = data->property("mac_address").toString();

        QJsonObject jsonTracks = data->getJsonData();
        jsonTracks.insert("ip", "?"+tmp_macAddress);
        jsonTracks.insert("isFile", true);
        jsonTracks.insert("isServer", true);
        jsonTracks.insert("name", tmp_url.split("/").last());      // 파일명
        jsonTracks.insert("path", "http://127.0.0.1:7080"+(tmp_url.replace(tmp_url.split("/").last(),"")));
        jsonTracks.insert("thumbnail", tmp_thumbnail);
        QJsonArray tmp_array;
        tmp_array.append(jsonTracks);


        tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, data->getTitle());
        tmp_jsonObject.insert(KEY_OP_TITLE_SUB, data->getArtist());
        tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);       // 강제로 넣어주는 경우 : AbstractMainContain 페이지가 아닌 다른 페이지에서 옵션팝업을 띄우는 경우
        tmp_jsonObject.insert(KEY_OP_TYPE, data->getId().isEmpty() ? OptionPopup::TypeMenu::Music_Collection_Track_Unidentified : OptionPopup::TypeMenu::Music_Collection_Track);
        tmp_jsonObject.insert(KEY_OP_albumImg, data->getAlbum_art());
        tmp_jsonObject.insert(KEY_OP_cntStar, 0);
        tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
        tmp_jsonObject.insert("isAlbum", false);
    }

    return tmp_jsonObject;
}


/**
 * @brief MusicCollection::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray MusicCollection::getJsonArr_TrackData(const int &p_index){
    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < listPopup->size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }

    return jsonArray;
}

/**
 * @brief MusicCollection::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject MusicCollection::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < listPopup->size()){
        DataPopup *dataPopup = listPopup->at(p_index);

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
        jsonThumb.insert("url", dataPopup->getAlbum_art());

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
 * @brief MusicCollection::requestFriendPlayList : 친구 플레이리스트 요청
 */
void MusicCollection::requestFriendPlayList(){

    if(!global.user.getAccess_token().isEmpty()){
        GSCommon::clearLayout(boxFriendPlayList);
        stackFriendPlayList->setCurrentIndex(1);
        lb_emptyFriendPlayList->setText(tr("Retrieving information."));
        //lb_emptyFriendPlayList->setText(tr("정보를 가져오는 중입니다."));

        NetworkHttp *network = new NetworkHttp(this);
        network->setProperty("uuid", uuid);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        json.insert("local_ip", global.device.getDeviceIP());
        json.insert("mac_address", global.device.getDeviceID());
        json.insert("permission","1");
        json.insert("type","music");
        network->request(HTTP_FRIEND_PLAYLIST, QString("%1/playlist/friend?page=0&size=10")
                         .arg(global.legacy_v3)
                         , json, true,true);
    }
}

/**
 * @brief MusicCollection::requestFlagLoginInfo 로그인 유무 체크
 */
void MusicCollection::requestFlagLoginInfo(){

    if(!global.user.isValid()){

        this->widgetLogin->setVisible(true);
        this->scrollArea_content->setVisible(false);

    }else{
        this->slot_requestData();
    }
}

/**
 * @brief MusicCollection::slot_logined : [슬롯] 로그인 성공
 * @details 로그인 이후에 처리할 수 있는 My,친구 플레이르스트 목록을 그린다.
 */
void MusicCollection::slot_requestData(){

    // uuid 설정
    uuid = QDateTime::currentMSecsSinceEpoch();

    // -----------------------------------------
    //  로그인 화면 hide, Contents Show
    // -----------------------------------------
    this->widgetLogin->setVisible(false);
    this->scrollArea_content->setVisible(true);

    // -----------------------------------------
    //  데이터 요청
    // -----------------------------------------
    requestFavorTracks();
    requestMyPlayList();
    requestFriendPlayList();
}

/**
 * @brief MusicCollection::responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicCollection::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(uuid == sender()->property("uuid").toLongLong()){
        if(p_id == HTTP_MY_PLAYLIST){
            setResultOfMyPlayList(p_jsonObject);
        }
        else if(p_id == HTTP_FRIEND_PLAYLIST){
            setResultOfFrienPlayList(p_jsonObject);
        }
        else if(p_id == HTTP_FAVORITE_TRACKS){
            setResultOfFavoriteTrack(p_jsonObject);

        }
    }

   sender()->deleteLater();
}

/**
 * @brief MusicCollection::slot_clickedViewAll [SLOT] View All 버튼 클릭시
 */
void MusicCollection::slot_clickedViewAll(){

    // 부모에게 페이지 변경하라고 시그널 보냄
    emit this->signal_clickedViewAll(sender()->property(KEY_VIEWALL_MODE.toStdString().c_str()).toString());
}

/**
 * @brief MusicCollection::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void MusicCollection::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();
    qint64 tmp_uuid = fileDownloader->property("uuid").toLongLong();

    if(uuid==tmp_uuid){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            if(tmp_index < listWidget->count()){
                listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
            }
        }
    }

    fileDownloader->deleteLater();
}

/**
 * @brief MusicCollection::slot_delegateClicked : [슬롯] 트렉정보 delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void MusicCollection::slot_delegateClicked(const int &p_index, const int &p_btnType){

    QListWidgetItem *item = listWidget->item(p_index);

    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    //int tmp_index = map["index"].toInt();
    QString tmp_title = map["title"].toString();
    QString tmp_url = map["url"].toString();
    QString tmp_thumbnail = map["thumbnail"].toString();
    QString tmp_macAddress = map["mac_address"].toString();

    if(p_btnType == MusicCollectionTrackDelegate::BtnType::etc){
        QJsonArray jsonArr;

        QJsonObject jsonTracks;
        jsonTracks.insert("ip", "?"+tmp_macAddress);
        jsonTracks.insert("isFile", true);
        jsonTracks.insert("isServer", true);
        jsonTracks.insert("name", tmp_url.split("/").last());      // 파일명
        jsonTracks.insert("path", "http://127.0.0.1:7080"+(tmp_url.replace(tmp_url.split("/").last(),"")));
        jsonTracks.insert("thumbnail", tmp_thumbnail);
        jsonArr.append(jsonTracks);

        QJsonObject jsonData;
        //jsonData.insert("musicPlayType",15);
        /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

        int playType = 0;
        switch(curr_clickMode)
        {
            case 0:
                playType = 13;
                break;
            case 1:
                playType = 12;
                break;
            case 2:
                playType = 15;
                break;
            case 3:
                playType = 16;
                break;
            case 4:
                playType = 17;
                break;
            case 5:
                playType = 18;
                break;
            case 6:
                playType = 19;
                break;
        }

        jsonData.insert("musicPlayType", playType);*/
        jsonData.insert("musicPlayType", global.device.getMusicPlayType());
        jsonData.insert("music", jsonArr);
        jsonData.insert("playType", 20);
        jsonData.insert("shuffle", 0);
        jsonData.insert("roseToken", global.device.getDeviceRoseToken());

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        network->setProperty("uuid", uuid);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3")
                         .arg(global.device.getDeviceIP())
                         .arg(global.port).arg("music_network_play"), jsonData, true);
    }
    else if(p_btnType == MusicCollectionTrackDelegate::BtnType::playListAdd){
        // 플레이리스트 담기
        if(p_index >=0 && p_index < listPopup->size() && !listPopup->at(p_index)->getId().isEmpty()){
            proc_addPlaylist->setProperty("index", p_index);
            proc_addPlaylist->showDlgOfPlayList();
        }else{
            ToastMsg::show(this,"",tr("You cannot add playlists."));
            //ToastMsg::show(this,"",tr("플레이리스트 담기를 할 수 없습니다."));
        }
    }
    else if(p_btnType == MusicCollectionTrackDelegate::BtnType::more){
        // 더보기
        if(p_index >=0 && p_index < listPopup->size()){
            emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, getJsonObject_OptionPopup(p_index));
        }else{
            ToastMsg::show(this,"",tr("The More option is not available."));
           // ToastMsg::show(this,"",tr("더보기 옵션을 사용할 수 없습니다."));
        }
    }
}

/**
 * @brief MusicCollection::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void MusicCollection::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
        proc_addPlaylist->requestAddPlayListTrack(p_playlistno);
    }else{
        proc_addPlaylist->showDlgOfNewPlayList();
    }
}

/**
 * @brief MusicCollection::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void MusicCollection::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
    proc_addPlaylist->requestNewPlaylistTrack(p_name);

}

/**
 * @brief MusicCollection::slot_myPlaylistChanged : [슬롯] My 플레이리스트 변경
 */
void MusicCollection::slot_myPlaylistChanged(){
    requestMyPlayList();
}

/**
 * @brief MusicCollection::slot_favoriteTrackChanged : [슬롯] Favorite 트랙 변경
 */
void MusicCollection::slot_favoriteTrackChanged(){
    requestFavorTracks();
}

/**
 * @brief MusicCollection::slot_clickedLogin : [슬롯] 로그인 버튼 클릭
 * @details : 로그인 Dialog Show
 */
void MusicCollection::slot_clickedLogin(){

    Dialog::DialogLogin *dlg = new Dialog::DialogLogin(this);
    dlg->exec();
}
