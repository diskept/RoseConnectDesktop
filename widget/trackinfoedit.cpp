#include "trackinfoedit.h"
#include "common/global.h"

#include <common/gscommon.h>
#include "widget/framewidget.h"

#include <QJsonObject>
#include <QTime>
#include <QJsonArray>
#include "widget/optionpopup.h"
#include <common/networkhttp.h>
#include <music/proc_uploadthumbnail.h>

const int HTTP_FAVORITE_GET = 59;
const int HTTP_FAVORITE_ADD = 58;
const int HTTP_FAVORITE_DELETE = 57;

TrackInfoEdit::TrackInfoEdit(QWidget *parent) : QPushButton(parent)
{
    linker = Linker::getInstance();

    QLabel *lb_icon = GSCommon::getUILabelImg(":/images/icon_list_play.png");
    lb_icon->setObjectName("lb_icon");
    lb_icon->setStyleSheet("#lb_icon { background-color:transparent; }");
    lb_title = new QLabel();
    lb_title->setObjectName("lb_title");
    lb_title->setStyleSheet("#lb_title { font-size:16px;color:#FFFFFF;background-color:transparent; }");
    lb_artist = new QLabel();
    lb_artist->setObjectName("lb_artist");
    lb_artist->setStyleSheet("#lb_artist { font-size:16px;color:#FFFFFF;background-color:transparent; }");
    lb_duration = new QLabel();
    lb_duration->setObjectName("lb_duration");
    lb_duration->setStyleSheet("#lb_duration { font-size:16px;color:#FFFFFF;background-color:transparent; }");

    // 버튼 3개
    QPushButton *btn_play_list_add_icon = GSCommon::getUIBtnImg("btn_play_list_add_icon", ":/images/play_list_add_icon.png");
    btn_play_list_add_icon->setFixedSize(60,60);
    btn_play_list_add_icon->setCursor(Qt::PointingHandCursor);
    btn_list_fav_icon = GSCommon::getUIBtnImg("btn_list_fav_icon", ":/images/list_fav_icon.png");
    btn_list_fav_icon->setFixedSize(60,60);
    btn_list_fav_icon->setCursor(Qt::PointingHandCursor);
    QPushButton *btn_icon_menu = GSCommon::getUIBtnImg("btn_icon_menu", ":/images/icon_menu.png");
    btn_icon_menu->setFixedSize(60,60);
    btn_icon_menu->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(20,0,0,0);
    hl_total->setSpacing(0);
    hl_total->setAlignment(Qt::AlignVCenter);
    hl_total->addWidget(lb_icon);
    hl_total->addSpacing(30);
    hl_total->addWidget(lb_title, 5);
    hl_total->addWidget(lb_artist, 2);
    hl_total->addWidget(lb_duration);
    hl_total->addSpacing(118);
    hl_total->addWidget(btn_play_list_add_icon);
    hl_total->addWidget(btn_list_fav_icon);
    hl_total->addWidget(btn_icon_menu);

    QWidget *widget = new QWidget();
    widget->setObjectName("trackInfoEdit");
    widget->setLayout(hl_total);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget);

    this->setStyleSheet("#trackInfoEdit { border-bottom:1px solid #2A2A2A; } #trackInfoEdit:hover { background-color:#333333; } ");
    this->setFixedHeight(70);
    this->setLayout(vl_total);
    this->setCursor(Qt::PointingHandCursor);

    // 커넥트
    //connect(btn_list_fav_icon, &ClickableLabel::signal_clicked, this, &TrackInfoEdit::slot_requestTrackFavorite);
    connect(btn_play_list_add_icon, SIGNAL(clicked()), this, SIGNAL(signal_clickedAddPlaylist()));
    connect(btn_list_fav_icon, &QPushButton::clicked, this, &TrackInfoEdit::slot_requestTrackFavorite);
    connect(btn_icon_menu, SIGNAL(clicked()), this, SLOT(slot_showOptionPopup()));
    connect(this, &QPushButton::clicked, this, &TrackInfoEdit::slot_clickedPlay);

}

/**
 * @brief TrackInfoEdit::slot_clickedPlay [SLOT] 해당 트랙을 바로 재생
 */
void TrackInfoEdit::slot_clickedPlay(){

    QJsonObject tmp_data;
    tmp_data = this->dataPopup->getJsonData();
    QJsonArray tmp_array;
    tmp_array.append(tmp_data);

    QJsonObject tmp_jsonObject;
    tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, this->dataPopup->getTitle());
    tmp_jsonObject.insert(KEY_OP_TITLE_SUB, this->dataPopup->getArtist());
    tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
    tmp_jsonObject.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_AlbumDetailTrack);
    tmp_jsonObject.insert(KEY_OP_albumImg, this->albumImg);
    tmp_jsonObject.insert(KEY_OP_cntStar, 0);
    tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
    tmp_jsonObject.insert("isAlbum", false); // 앨범이 아니라 곡임..

    emit linker->signal_clickedHoverItem("play", tmp_jsonObject);
}

/**
 * @brief TrackInfoEdit::slot_showOptionPopup [SLOT] 옵션팝업 띄우기
 */
void TrackInfoEdit::slot_showOptionPopup(){

    QJsonObject tmp_data;
    tmp_data = this->dataPopup->getJsonData();
    QJsonArray tmp_array;
    tmp_array.append(tmp_data);

    int track_idx = 0;
    QJsonArray tmp_tarray;
    for(int i = 0; i < list_audioInfo.size(); i++){
        QJsonObject tmp_tdata;
        tmp_tdata = this->list_audioInfo.at(i)->getJsonData();
        tmp_tarray.append(tmp_tdata);

        if(this->dataPopup->getId() == this->list_audioInfo.at(i)->getId()){
            track_idx = i;
        }
    }

    QJsonObject tmp_jsonObject;
    tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, this->dataPopup->getTitle());
    tmp_jsonObject.insert(KEY_OP_TITLE_SUB, this->dataPopup->getArtist());
    tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
    tmp_jsonObject.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_AlbumDetailTrack);
    tmp_jsonObject.insert(KEY_OP_albumImg, this->albumImg);
    tmp_jsonObject.insert(KEY_OP_cntStar, 0);
    tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
    tmp_jsonObject.insert(KEY_OP_DATA_TOTAL, tmp_tarray);
    tmp_jsonObject.insert(KEY_OP_TRACK_INDEX, track_idx);
    tmp_jsonObject.insert("isAlbum", false); // 앨범이 아니라 곡임..

    emit linker->signal_clickedHoverItem("more", tmp_jsonObject);
}

/**
 * @brief TrackInfoEdit::setDataTrackInfo 데이터 세팅 (DataPopup클래스를 매개변수로 받음)
 * @param p_data
 */
void TrackInfoEdit::setDataTrackInfoEdit(DataPopup *p_data, QString p_albumImg){

    this->dataPopup = p_data;
    this->albumImg = p_albumImg;

    lb_title->setText(p_data->getTitle());
    lb_artist->setText(p_data->getArtist());
    QTime tmp_duration = QTime::fromMSecsSinceStartOfDay(p_data->getDuration());
    lb_duration->setText(tmp_duration.toString("mm:ss"));

    // 트랙 하트 GET
    this->requestIsfavorite();
}

void TrackInfoEdit::setDataTracksInfoEdit(QList<DataPopup*> list_Info){

    memcpy(&this->list_audioInfo, &list_Info, sizeof (list_Info));
}

/**
 * @brief TrackInfoEdit::requestIsfavorite 트랙 하트 가져오는 API 호출
 */
void TrackInfoEdit::requestIsfavorite(){

    if(!global.device.getDeviceIP().isEmpty()){
        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",dataPopup->getData());


        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_get), tmp_json, true, true);
    }
}

/**
 * @brief TrackInfoEdit::slot_requestTrackFavorite 트랙 하트 세팅 API 호출 (설정/해제)
 */
void TrackInfoEdit::slot_requestTrackFavorite(){

    if(!global.device.getDeviceIP().isEmpty()){
        QJsonArray tmp_jsonArray;
        QJsonObject tmp_data = getJsonObject_TrackData();
        tmp_data.insert("type", "music");
        tmp_data.insert("url",dataPopup->getData());
        tmp_data.insert("mac_address", global.device.getDeviceID());
        tmp_data.insert("isFavorite", !this->isFavorite);
        tmp_jsonArray.append(tmp_data);

        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",dataPopup->getData());
        tmp_json.insert("favorite", !this->isFavorite);
        tmp_json.insert("tracks",tmp_jsonArray);

        if(this->isFavorite==true){
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);
        }else{
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_ADD, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);


            if(!albumImg.isEmpty()){
                QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(this->albumImg);
                Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
                proc_uploadThumb->uploadThumnail(imgPath, dataPopup->getData());
                connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
            }
        }
    }
}

/**
 * @brief TrackInfoEdit::getJsonObject_TrackData : Track 정보 반환
 * @return QJsonObject
 */
QJsonObject TrackInfoEdit::getJsonObject_TrackData(){

    QJsonObject jsonTrack;
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
    jsonThumb.insert("url", this->albumImg);

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

    return jsonTrack;
}


/**
 * @brief TrackInfoEdit::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void TrackInfoEdit::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_FAVORITE_GET){
        if(p_jsonObject.contains("favorite")){
            this->isFavorite = p_jsonObject["favorite"].toBool();
            if(this->isFavorite==true){
                btn_list_fav_icon->setStyleSheet(btn_list_fav_icon->styleSheet().replace("list_fav_icon.png", "list_fav_icon_on.png"));
            }else{
                btn_list_fav_icon->setStyleSheet(btn_list_fav_icon->styleSheet().replace("list_fav_icon_on.png", "list_fav_icon.png"));
            }
        }
    }else if(p_id == HTTP_FAVORITE_DELETE){
        if(p_jsonObject.contains("status")){
            if(p_jsonObject["status"]=="OK"){
                this->isFavorite = false;
                btn_list_fav_icon->setStyleSheet(btn_list_fav_icon->styleSheet().replace("list_fav_icon_on.png", "list_fav_icon.png"));
            }
        }
    }else if(p_id == HTTP_FAVORITE_ADD){
        if(p_jsonObject.contains("status")){
            if(p_jsonObject["status"]=="OK"){
                this->isFavorite = true;
                btn_list_fav_icon->setStyleSheet(btn_list_fav_icon->styleSheet().replace("list_fav_icon.png", "list_fav_icon_on.png"));
            }
        }
    }

    sender()->deleteLater();
}





