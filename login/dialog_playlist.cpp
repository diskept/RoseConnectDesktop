#include "dialog_playlist.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include <QLabel>
#include <QScrollArea>
#include <widget/imagelabel.h>
#include <common/my_lang.h>
#include <QDebug>

//#define MY_LANG
using namespace Dialog;



/**
 * @brief Dialog_Playlist::Dialog_Playlist : 플레이리스트-선택 다이얼로그
 * @param parent
 */
Dialog_Playlist::Dialog_Playlist(PlayListType p_playlistType, QWidget *parent)
    : QDialog(parent)
    , playListType(p_playlistType)
{
    setInit();
    setUIControl();
}

/**
 * @brief Dialog_Playlist::setInit : 초기 세팅
 */
void Dialog_Playlist::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
    this->setStyleSheet("background-color:transparent;");
}

/**
 * @brief Dialog_Playlist::setUIControl : UI 세팅
 */
void Dialog_Playlist::setUIControl(){

    // ------------------------------------------
    //  타이틀
    // ------------------------------------------
    QLabel *lb_title = new QLabel();
    // lb_title->setText(tr("플레이리스트에 담기"));
    lb_title->setText(tr("Add to playlist"));
    lb_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
    lb_title->setAlignment(Qt::AlignCenter);
    lb_title->setFixedHeight(70);

    // ------------------------------------------
    //  X 버튼
    // ------------------------------------------
    QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
    btn_close->setCursor(Qt::PointingHandCursor);
    btn_close->setFixedSize(QSize(70,70));
    btn_close->setParent(lb_title);
    btn_close->setGeometry(530, 0, 70, 70);
    btn_close->show();

    // ------------------------------------------
    //  타이틀 박스 구성
    // ------------------------------------------
    QHBoxLayout *box_top = new QHBoxLayout();
    box_top->setContentsMargins(0,0,0,0);
    box_top->setSpacing(0);
    box_top->addWidget(lb_title);

    QWidget *widget_title = new QWidget();
    widget_title->setObjectName("widget_title");
    widget_title->setLayout(box_top);
    widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");


    // ------------------------------------------
    // 플레이리스트 목록 박스 구성
    // ------------------------------------------
    box_playList = new QVBoxLayout();
    box_playList->setSpacing(0);
    box_playList->setContentsMargins(0,0,0,0);
    box_playList->setAlignment(Qt::AlignTop);

    QWidget *wg_scroll = new QWidget();
    wg_scroll->setContentsMargins(0,0,0,0);
    wg_scroll->setLayout(box_playList);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(wg_scroll);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border:none;");


    // ------------------------------------------
    //  토탈 박스 구성 및 레이아웃 설정
    // ------------------------------------------
    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setContentsMargins(0,0,0,0);
    box_total->setSpacing(0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addWidget(widget_title);
    box_total->addWidget(scrollArea);

    QWidget *widget_total = new QWidget();
    widget_total->setContentsMargins(0,0,0,10);
    widget_total->setObjectName("widget_total");
    widget_total->setLayout(box_total);
    widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");
    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->setContentsMargins(0,0,0,0);
    vboxlayout->setSpacing(0);
    vboxlayout->addWidget(widget_total);

    setLayout(vboxlayout);

    // 커넥션
    connect(btn_close, SIGNAL(clicked()), this, SLOT(close()));

    setInitPlayList();//처음 실행시 3회 실행됨
}

/**
 * @brief Dialog_Playlist::setInitPlayList : 플레이리스트 목록 clear
 */
void Dialog_Playlist::setInitPlayList(){
    GSCommon::clearLayout(box_playList);
    // appendPlayListUI(-1, tr("새 플레이리스트 추가"), "",-1);
    appendPlayListUI(-1, tr("Add a new playlist"), "",-1);
}

/**
 * @brief Dialog_Playlist::appendPlayListUI
 * @param p_playlistNo
 * @param p_name
 * @param p_thumbPath
 * @param p_numContents
 */
void Dialog_Playlist::appendPlayListUI(int p_playlistNo, QString p_name, QString p_thumbPath, int p_numContents){


    ImageLabel *lb_img = new ImageLabel();
    lb_img->setType(ImageLabel::Type::IconSmallRect);
    if(!p_thumbPath.isEmpty()){
        lb_img->setImageUrl(p_thumbPath);
    }else if(p_playlistNo < 0){
        lb_img->setImageResource(":/images/playlist_add_ico.png");
    }else{
        lb_img->setImageResource(":/images/def_mus_60.png");
    }


    QLabel *lb_name = new QLabel();
    lb_name->setText(GSCommon::getTextCutFromLabelWidth(p_name,DLG_WIDTH-230,lb_name->font()));
    lb_name->setStyleSheet("color:#FFFFFF;font-size:16px;");

    QLabel *lb_count = new QLabel();
    // lb_count->setText(QString::number(p_numContents)+tr("곡"));
    lb_count->setText(QString::number(p_numContents)+tr("Song"));
    lb_count->setStyleSheet("color:#777777;font-size:16px;");
    if(p_numContents < 0){
        lb_count->hide();
    }

    QVBoxLayout *box_info = new QVBoxLayout;
    box_info->setContentsMargins(0,0,0,0);
    box_info->setSpacing(10);
    box_info->setAlignment(Qt::AlignVCenter);
    box_info->addWidget(lb_name);
    box_info->addWidget(lb_count);

    QHBoxLayout *box_btn = new QHBoxLayout;
    box_btn->setContentsMargins(0,0,0,0);
    box_btn->setSpacing(20);
    box_btn->addWidget(lb_img);
    box_btn->addLayout(box_info);




    QPushButton *btn = new QPushButton();
    btn->setObjectName("wg_item");
    btn->setContentsMargins(10,10,10,10);
    btn->setLayout(box_btn);
    btn->setFixedHeight(80);
    btn->setProperty("playlistno", p_playlistNo);
    btn->setStyleSheet("#wg_item {border:none;border-bottom:1px solid #404040;margin-left:10px;margin-right:10px;} #wg_item:hover {border:none;border-bottom:1px solid #2A2A2A; background-color:#4D4D4D;margin-left:0px;margin-right:0px;}");

    box_playList->addWidget(btn);


    // 커넥트
    connect(btn, SIGNAL(clicked()), this, SLOT(slot_clickedPlaylist()));
}

/**
 * @brief Dialog_Playlist::requestMyPlayList : 마이 플레이리스트 요청
 * @note API 포스트맨에는 {MODE_LEGACY_BETA}/playlist/fetch type에 따라 youtube, music 플레이리스트 반환
 * 한다되어있으나, youtube만 동작함</br>
 * 음악 플레이리스트는 App가 사용하는 {V3_LEGACY}/playlist 사용
 *
 */
void Dialog_Playlist::requestMyPlayList(){

    // clear
    setInitPlayList();


    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


    if(playListType==PlayListType::YOUTUBE){
        QUrlQuery params;
        params.addQueryItem("type",playListType==PlayListType::YOUTUBE ? "youtube" : "music");
        params.addQueryItem("username",global.user.getUsername());
        //params.addQueryItem("range_start",QString::number(0));
        //params.addQueryItem("range_end",QString::number(50));

        network->request(HTTP_NETWORK_MY_PLAYLIST_YOUTUBE, QString("%1/playlist/fetch?range_start=0&range_end=50")
                         .arg(global.legacy_mod_api), params, true,true);
    }
    else if(playListType==PlayListType::MUSIC){
        QJsonObject jsonParam;
        jsonParam.insert("local_ip", global.device.getDeviceIP());
        jsonParam.insert("mac_address", global.device.getDeviceID());

        network->request(HTTP_NETWORK_MY_PLYLIST_MUSIC, QString("%1/playlist?page=0&size=50")
                .arg(global.legacy_v3_api), jsonParam, true, true);

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    }
}

/**
 * @brief Dialog_Playlist::setResultOfMyPlayList_Music : 마이 플레이리스트 목록 요청결과 처리(음악)
 * @param p_jsonObject
 */
void Dialog_Playlist::setResultOfMyPlayList_Music(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playlists = "playlists";
    const QString jsonKey_id = "id";
    const QString jsonKey_name = "name";
    const QString jsonKey_thumbnails = "thumbnails";
    const QString jsonKey_url = "url";
    const QString jsonKey_totalcount = "totalcount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_playlists)){
            QJsonArray jsonArr = p_jsonObject[jsonKey_playlists].toArray();

            for(int i = 0; i < jsonArr.size(); i++){
                QJsonObject jsonPlayList = jsonArr.at(i).toObject();
                if(jsonPlayList.contains(jsonKey_id)
                        && jsonPlayList.contains(jsonKey_name)
                        && jsonPlayList.contains(jsonKey_totalcount)
                        && jsonPlayList.contains(jsonKey_thumbnails)){

                    int tmp_no = jsonPlayList[jsonKey_id].toInt();
                    QString tmp_title = jsonPlayList[jsonKey_name].toString();
                    int tmp_totalcount = jsonPlayList[jsonKey_totalcount].toInt();
                    QString tmp_img="";

                    if(jsonPlayList.contains(jsonKey_thumbnails)){
                        QJsonArray jsonThumbArr = jsonPlayList[jsonKey_thumbnails].toArray();
                        if(jsonThumbArr.size() > 0){
                            QJsonObject jsonThumb = jsonThumbArr.at(0).toObject();
                            if(jsonThumb.contains(jsonKey_url)){
                                tmp_img = jsonThumb[jsonKey_url].toString();
                            }
                        }
                    }

                    appendPlayListUI(tmp_no, tmp_title, tmp_img, tmp_totalcount);
                }
            }
        }
    }
}

/**
 * @brief Dialog_Playlist::setResultOfMyPlayList_Youtube : 마이 플레이리스트 목록 요청결과 처리(로즈튜브)
 * @param p_jsonObject QJsonObject response
 */
void Dialog_Playlist::setResultOfMyPlayList_Youtube(const QJsonObject &p_jsonObject){


    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playlists = "playlists";
    const QString jsonKey_type = "type";
    const QString jsonKey_no = "no";
    const QString jsonKey_title = "title";
    const QString jsonKey_totalcount = "totalcount";
    const QString jsonKey_coverimg = "coverimg";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_playlists)){
            QJsonArray jsonArr = p_jsonObject[jsonKey_playlists].toArray();

            for(int i = 0; i < jsonArr.size(); i++){
                QJsonObject jsonPlayList = jsonArr.at(i).toObject();

                if(playListType == PlayListType::YOUTUBE){

                    if(jsonPlayList.contains(jsonKey_type) && jsonPlayList[jsonKey_type].toString()=="youtube"){
                        if(jsonPlayList.contains(jsonKey_no)
                                && jsonPlayList.contains(jsonKey_title)
                                && jsonPlayList.contains(jsonKey_totalcount)
                                && jsonPlayList.contains(jsonKey_coverimg)){

                            int tmp_no = jsonPlayList[jsonKey_no].toInt();
                            QString tmp_title = jsonPlayList[jsonKey_title].toString();
                            QString tmp_img = jsonPlayList[jsonKey_coverimg].toString();
                            int tmp_totalcount = jsonPlayList[jsonKey_totalcount].toInt();

                            appendPlayListUI(tmp_no, tmp_title, tmp_img, tmp_totalcount);
                        }

                    }
                }
            }
        }
    }
}

/**
 * @brief Dialog_Playlist::slot_responseHttp : [슬롯] Http 요청 결과 처리
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void Dialog_Playlist::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id==HTTP_NETWORK_MY_PLAYLIST_YOUTUBE){
        setResultOfMyPlayList_Youtube(p_jsonObject);
    }
    else if(p_id==HTTP_NETWORK_MY_PLYLIST_MUSIC){
        setResultOfMyPlayList_Music(p_jsonObject);
    }


   sender()->deleteLater();
}

/**
 * @brief Dialog_Playlist::slot_clickedPlaylistj : [슬롯] 플레이리스트 클릭
 */
void Dialog_Playlist::slot_clickedPlaylist(){
    int tmp_playlistno = sender()->property("playlistno").toInt();

    setResult(QDialog::Accepted);
    hide();

    emit signal_clickedPlaylist(tmp_playlistno);
}
