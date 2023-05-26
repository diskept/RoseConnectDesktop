#include "common/global.h"
#include "metainfo.h"

#include <QJsonObject>
#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QJsonArray>
#include <QPainter>
#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <data/datapopup.h>
#include <widget/clickablelabel.h>
#include <widget/toastmsg.h>

const int ALBUMIMG_SIZE = 300;
const int LINEEDIT_W = 1137;
const int LINEEDIT_H = 50;

const int HTTP_METAINFO = 99;
MetaInfo::MetaInfo(QWidget *parent) : QWidget(parent)
{
    setUIControl();

}


MetaInfo::~MetaInfo(){

    this->deleteLater();
}

/**
 * @brief MetaInfo::setUIControl : UI 세팅
 */
void MetaInfo::setUIControl(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(":/images/def_mus_300.png");

    // 앨범이미지
    this->lb_albumImg = new QLabel();
    this->lb_albumImg->setFixedSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE);
    ClickableLabel *lb_albumImgEdit = new ClickableLabel();
    lb_albumImgEdit->setText(tr("Image change"));
    //lb_albumImgEdit->setText(tr("이미지 변경"));
    lb_albumImgEdit->setStyleSheet("font-size:20px;color:#FFFFFF;background-color:transparent;");
    lb_albumImgEdit->setCursor(Qt::PointingHandCursor);
    QLabel *lb_icon = GSCommon::getUILabelImg(":/images/thum_edit.png");
    lb_icon->setStyleSheet("background-color:transparent;");
    QHBoxLayout *hl_albumImgEdit = new QHBoxLayout();
    hl_albumImgEdit->setContentsMargins(0,0,0,0);
    hl_albumImgEdit->setSpacing(0);
    hl_albumImgEdit->setAlignment(Qt::AlignCenter);
    hl_albumImgEdit->addWidget(lb_icon);
    hl_albumImgEdit->addWidget(lb_albumImgEdit);
    widget_albumImgEdit = new QWidget(this->lb_albumImg);
    widget_albumImgEdit->setFixedSize(ALBUMIMG_SIZE,ALBUMIMG_SIZE);
    widget_albumImgEdit->setStyleSheet("background-color:#88000000;");
    widget_albumImgEdit->setLayout(hl_albumImgEdit);

    // 편집UI
    le_albumName = new QLineEdit();
    le_title = new QLineEdit();
    le_artist = new QLineEdit();
    le_gerne = new QLineEdit();
    le_composer = new QLineEdit();
    le_albumName->setFixedHeight(LINEEDIT_H);
    le_title->setFixedHeight(LINEEDIT_H);
    le_artist->setFixedHeight(LINEEDIT_H);
    le_gerne->setFixedHeight(LINEEDIT_H);
    le_composer->setFixedHeight(LINEEDIT_H);
    le_albumName->setMaximumWidth(LINEEDIT_W);
    le_title->setMaximumWidth(LINEEDIT_W);
    le_artist->setMaximumWidth(LINEEDIT_W);
    le_gerne->setMaximumWidth(LINEEDIT_W);
    le_composer->setMaximumWidth(LINEEDIT_W);
    le_albumName->setTextMargins(22,14,22,14);
    le_title->setTextMargins(22,14,22,14);
    le_artist->setTextMargins(22,14,22,14);
    le_gerne->setTextMargins(22,14,22,14);
    le_composer->setTextMargins(22,14,22,14);

    // 라벨
    QLabel *lb_albumName = new QLabel(tr("Album name"));
    //QLabel *lb_albumName = new QLabel(tr("앨범명"));
    //lb_albumName->setBuddy(nameLineEdit);
    lb_albumName->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QLabel *lb_title = new QLabel(tr("Title"));
    //QLabel *lb_title = new QLabel(tr("타이틀"));
    lb_title->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QLabel *lb_artist = new QLabel(tr("Artist"));
    //QLabel *lb_artist = new QLabel(tr("아티스트"));
    lb_artist->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QLabel *lb_gerne = new QLabel(tr("Genre"));
    //QLabel *lb_gerne = new QLabel(tr("장르"));
    lb_gerne->setStyleSheet("font-size:18px;color:#FFFFFF;");
    QLabel *lb_composer = new QLabel(tr("Composer"));
    //QLabel *lb_composer = new QLabel(tr("작곡가"));
    lb_composer->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_albumName->setFixedHeight(LINEEDIT_H);
    lb_title->setFixedHeight(LINEEDIT_H);
    lb_artist->setFixedHeight(LINEEDIT_H);
    lb_gerne->setFixedHeight(LINEEDIT_H);
    lb_composer->setFixedHeight(LINEEDIT_H);

    btn_edit = new QPushButton();
    btn_edit->setContentsMargins(53,10,53,10);
    btn_edit->setText(tr("Meta information editing"));
    //btn_edit->setText(tr("메타정보 편집"));
    btn_edit->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_edit->setCursor(Qt::PointingHandCursor);
    btn_edit->setFixedSize(210,40);

    btn_save = new QPushButton();
    btn_save->setContentsMargins(53,10,53,10);
    btn_save->setText(tr("Save"));
    //btn_save->setText(tr("저장"));
    btn_save->setStyleSheet("font-size:18px;color:#FFFFFF;background-color:#B18658;border-radius:20px;");
    btn_save->setCursor(Qt::PointingHandCursor);
    btn_save->setFixedSize(140,40);
    btn_cancel = new QPushButton();
    btn_save->setContentsMargins(53,10,53,10);
    btn_cancel->setText(tr("Cancel"));
    //btn_cancel->setText(tr("취소"));
    btn_cancel->setStyleSheet("font-size:18px;color:#777777;background-color:transparent;border-radius:20px;border:1px solid #777777;");
    btn_cancel->setCursor(Qt::PointingHandCursor);
    btn_cancel->setFixedSize(140,40);

    this->hl_btnEdit = new QHBoxLayout();
    this->hl_btnEdit->setContentsMargins(0,0,0,0);
    this->hl_btnEdit->setSpacing(0);
    this->hl_btnEdit->setAlignment(Qt::AlignCenter);
    this->hl_btnEdit->addWidget(this->btn_save);
    this->hl_btnEdit->addSpacing(30);
    this->hl_btnEdit->addWidget(this->btn_cancel);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setContentsMargins(33,40,33,40);
    formLayout->setSpacing(0);
    formLayout->setVerticalSpacing(15);
    formLayout->setHorizontalSpacing(30);
    formLayout->setFormAlignment(Qt::AlignCenter);
    formLayout->addRow(lb_albumName, le_albumName);
    formLayout->addRow(lb_title, le_title);
    formLayout->addRow(lb_artist, le_artist);
    formLayout->addRow(lb_gerne, le_gerne);
    formLayout->addRow(lb_composer, le_composer);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,10,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    vl_total->addWidget(this->lb_albumImg, 0, Qt::AlignCenter);
    vl_total->addLayout(formLayout);
    vl_total->addWidget(btn_edit, 0, Qt::AlignCenter);
    vl_total->addLayout(hl_btnEdit);

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setLayout(vl_total);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    setLayout(vl_scroll);

    // 커넥션
    connect(btn_edit, SIGNAL(clicked()), this, SLOT(slot_changedModeEdit()));
    connect(btn_save, SIGNAL(clicked()), this, SLOT(slot_saveInfo()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(slot_changedModeView()));
    connect(lb_albumImgEdit, SIGNAL(signal_clicked()), this, SLOT(slot_showPopupChangeImg()));
}

/**
 * @brief MetaInfo::loadImage [SLOT] 앨범 이미지 세팅
 */
void MetaInfo::slot_loadImage()
{
    this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());
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

    this->fileDownLoader->deleteLater();
}

/**
 * @brief MetaInfo::requestDataFromAPI 메타정보 API 호출
 */
void MetaInfo::requestDataFromAPI(){

    if(!global.device.getDeviceIP().isEmpty()){

        QJsonObject tmp_songInfo;
        //tmp_songInfo.insert("album", "2 The Sky");
        //tmp_songInfo.insert("album_art", "/storage/emulated/0/Android/data/com.android.providers.media/albumthumbs/1591346331246");
        //tmp_songInfo.insert("album_id", 62);
        //tmp_songInfo.insert("album_key", "\b\u0016\b\u0004\bI\b·\b±\b\u0004\bI\b½\bU\b564663646");
        //tmp_songInfo.insert("artist", "화요비");
        //tmp_songInfo.insert("artist_id", "53");
        //tmp_songInfo.insert("artist_key", "\b&*y\u0003\b&\u001c\u0003\b&\u0014?\u0003\b");
        //tmp_songInfo.insert("composercnt", 0);
        //tmp_songInfo.insert("data", "/mnt/media_rw/cifs/192.168.10.22/share/music/052.화요비-2 The Sky.mp3");
        //tmp_songInfo.insert("duration", "0");
        //tmp_songInfo.insert("gener_song_cnt", 0);
        tmp_songInfo.insert("id", this->audio_id);
        //tmp_songInfo.insert("isSelected", false);
        //tmp_songInfo.insert("num_of_song", 0);
        //tmp_songInfo.insert("num_of_tracks", 0);
        //tmp_songInfo.insert("star", 0);
        //tmp_songInfo.insert("title", "2 The Sky");

        QJsonObject tmp_musicData;
        tmp_musicData.insert("isDirect", false);
        tmp_musicData.insert("mMusicSongInfo", tmp_songInfo);

        QJsonObject tmp_json;
        tmp_json.insert("musicData",tmp_musicData);
        tmp_json.insert("type","MUSIC");
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_METAINFO, QString("http://%1:%2/meta_info_get")
                         .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
    }
}

/**
 * @brief MetaInfo::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MetaInfo::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_METAINFO){
        if(p_jsonObject.contains("album")){
            this->albumName = p_jsonObject["album"].toString();
        }
        if(p_jsonObject.contains("artist")){
            this->artist = p_jsonObject["artist"].toString();
        }
        if(p_jsonObject.contains("genre")){
            this->genre = p_jsonObject["genre"].toString();
        }
        if(p_jsonObject.contains("composer")){
            this->composer = p_jsonObject["composer"].toString();
        }

        // UI에 데이터 세팅
        this->le_albumName->setText(this->albumName);
        this->le_title->setText(this->title);
        this->le_artist->setText(this->artist);
        this->le_gerne->setText(this->genre);
        this->le_composer->setText(this->composer);
    }
    sender()->deleteLater();
}
/**
 * @brief MetaInfo::setData 기본정보 데이터 세팅
 * @note [참고] OptionPopup 클래스의 setData 받는 구조와 동일하다
 * @param p_data
 */
void MetaInfo::setData(QJsonObject p_data){

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, SIGNAL (downloaded()), this, SLOT (slot_loadImage()));

    // 초기화
    this->type_optionMenu = OptionPopup::TypeMenu::Music_Etc_AlbumDetailTrack;
    this->albumName = "";
    this->albumImg = "";
    this->title = "";
    this->artist = "";
    this->genre = "";
    this->composer = "";
    this->audio_id = "";

    // 데이터 세팅
    if(p_data.contains(KEY_OP_TYPE)){
        this->type_optionMenu = static_cast<OptionPopup::TypeMenu>(p_data[KEY_OP_TYPE].toInt());
    }
    if(p_data.contains(KEY_OP_albumImg)){
        this->albumImg = p_data[KEY_OP_albumImg].toString();
    }
    if(p_data.contains(KEY_OP_DATA)){
        QJsonArray tmp_arr = p_data[KEY_OP_DATA].toArray();
        if(tmp_arr.count()>0){
            DataPopup *tmp_dataPopup = new DataPopup();
            tmp_dataPopup->setJsonData(tmp_arr.first().toObject());
            this->title = tmp_dataPopup->getTitle();
            this->audio_id = tmp_dataPopup->getId();
        }
    }
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

    // API 호출
    this->requestDataFromAPI();

    // View 모드로 UI 변경
    this->slot_changedModeView();
}

/**
 * @brief MetaInfo::slot_changedModeEdit [SLOT] 편집모드로 UI 변경
 */
void MetaInfo::slot_changedModeEdit(){

    // UI 모드 변경
    this->widget_albumImgEdit->setVisible(true);
    this->btn_edit->setVisible(false);
    this->btn_save->setVisible(true);
    this->btn_cancel->setVisible(true);

    le_albumName->setReadOnly(false);
    le_title->setReadOnly(false);
    le_artist->setReadOnly(false);
    le_gerne->setReadOnly(false);
    le_composer->setReadOnly(false);
    le_albumName->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:#333333;border-radius:2px;");
    le_title->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:#333333;border-radius:2px;");
    le_artist->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:#333333;border-radius:2px;");
    le_gerne->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:#333333;border-radius:2px;");
    le_composer->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:#333333;border-radius:2px;");
}

/**
 * @brief MetaInfo::slot_changedModeView [SLOT] View모드로 UI 변경
 */
void MetaInfo::slot_changedModeView(){

    // 데이터 원상복구
    this->le_albumName->setText(this->albumName);
    this->le_title->setText(this->title);
    this->le_artist->setText(this->artist);
    this->le_gerne->setText(this->genre);
    this->le_composer->setText(this->composer);

    // UI 모드 변경
    this->widget_albumImgEdit->setVisible(false);
    this->btn_edit->setVisible(true);
    this->btn_save->setVisible(false);
    this->btn_cancel->setVisible(false);

    le_albumName->setReadOnly(true);
    le_title->setReadOnly(true);
    le_artist->setReadOnly(true);
    le_gerne->setReadOnly(true);
    le_composer->setReadOnly(true);
    le_albumName->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:transparent;border-radius:2px;");
    le_title->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:transparent;border-radius:2px;");
    le_artist->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:transparent;border-radius:2px;");
    le_gerne->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:transparent;border-radius:2px;");
    le_composer->setStyleSheet("font-size:20px;color:#FFFFFF;border:1px solid #4D4D4D;background-color:transparent;border-radius:2px;");


    this->btn_edit->setVisible(false);      // 편집 기능 HIDE 함...

}

/**
 * @brief MetaInfo::slot_showPopupChangeImg [SLOT] 이미지 변경 버튼 클릭시,, 이미지 변경 팝업 show
 */
void MetaInfo::slot_showPopupChangeImg(){

    ToastMsg::show(this, "", "이미지 변경 팝업 띄워야함 !!!!!!");
}
/**
 * @brief MetaInfo::slot_saveInfo [SLOT] 저장 버튼 클릭시
 */
void MetaInfo::slot_saveInfo(){

    ToastMsg::show(this, "", "메타정보 편집 저장 완료 처리 준비중............");
}
