#include "musicfolder_editfavor.h"
#include <QScrollArea>
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "widget/dialogconfirm.h"
#include <QJsonObject>
#include <QJsonArray>

const int HTTP_FAVORITE_LIST = 99;
const int HTTP_FAVORITE_DEL = 98;

/**
 * @brief MusicFolder_EditFavor::MusicFolder_EditFavor : 음악-폴더-즐겨찾기 편집
 * @param parent
 */
MusicFolder_EditFavor::MusicFolder_EditFavor(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief MusicFolder_EditFavor::setInit : 초기 세팅
 */
void MusicFolder_EditFavor::setInit(){
}

/**
 * @brief MusicFolder_EditFavor::setUIControl : UI 세팅
 */
void MusicFolder_EditFavor::setUIControl(){

    boxFavorGroup = new QVBoxLayout;
    boxFavorGroup->setAlignment(Qt::AlignTop);
    boxFavorGroup->setContentsMargins(0,0,0,0);
    boxFavorGroup->setSpacing(0);

    QWidget *wg_scrollContents = new QWidget;
    wg_scrollContents->setContentsMargins(33,0,28,0);
    wg_scrollContents->setLayout(boxFavorGroup);

    // 스크롤
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(wg_scrollContents);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);
}



/**
 * @brief MusicFolder_EditFavor::setData : 폴더 즐겨찾기 데이터 세팅
 * @param p_jsonArrFavor QJsonArray
 */
void MusicFolder_EditFavor::setFavorData(const QJsonArray &p_jsonArrFavor){

    // clear
    GSCommon::clearLayout(boxFavorGroup);

    for(int i=0 ; i<p_jsonArrFavor.count(); i++){
        appendFavoriteUI(p_jsonArrFavor.at(i).toObject());
    }
}

/**
 * @brief MusicFolder_EditFavor::appendFavoriteUI : 즐겨찾기 편집 UI 세팅
 * @param p_name 즐겨찾기 이름
 * @param p_path 즐겨찾기 경로
 */
void MusicFolder_EditFavor::appendFavoriteUI(const QJsonObject &p_jsonData){

    QString tmp_name = "";
    QString tmp_path = "";
    QString tmp_pathKey = "";
    if(p_jsonData.contains("albumFavName")){
        tmp_name = p_jsonData["albumFavName"].toString();
    }
    if(p_jsonData.contains("albumFavPath")){
        tmp_path = p_jsonData["albumFavPath"].toString();
    }
    if(p_jsonData.contains("albumFavPathKey")){
        tmp_pathKey = p_jsonData["albumFavPathKey"].toString();
    }

    QLabel *lb_img = GSCommon::getUILabelImg(":images/icon_folder_bic.png");
    lb_img->setFixedSize(70,70);

    QLabel *lb_name = new QLabel;
    lb_name->setText(tmp_name);
    lb_name->setStyleSheet("color:#FFFFFF;font-size:16px;");

    QLabel *lb_path = new QLabel;
    lb_path->setText(tmp_path);
    lb_path->setStyleSheet("color:#999999;font-size:16px;");

    QVBoxLayout *box_info = new QVBoxLayout;
    box_info->setSpacing(5);
    box_info->setContentsMargins(0,0,0,0);
    box_info->setAlignment(Qt::AlignVCenter);
    box_info->addWidget(lb_name);
    box_info->addWidget(lb_path);

    QPushButton *btn_del = GSCommon::getUIBtnImg("btn_remove", ":images/fol_del_ico.png");
    btn_del->setFixedSize(QSize(40,40));
    btn_del->setCursor(Qt::PointingHandCursor);
    btn_del->setProperty("albumFavName", tmp_name);
    btn_del->setProperty("albumFavPath", tmp_path);
    btn_del->setProperty("albumFavPathKey", tmp_pathKey);

    QHBoxLayout *box_fav = new QHBoxLayout;
    box_fav->setContentsMargins(0,0,0,0);
    box_fav->setSpacing(0);
    box_fav->addWidget(lb_img);
    box_fav->addSpacing(15);
    box_fav->addLayout(box_info,1);
    box_fav->addWidget(btn_del,0, Qt::AlignVCenter);

    QWidget *wg_row = new QWidget;
    wg_row->setObjectName("wg_row");
    wg_row->setContentsMargins(0,0,0,0);
    wg_row->setLayout(box_fav);
    wg_row->setStyleSheet("#wg_row { border-bottom:2px solid #282828; } #wg_row:hover { background-color:#333333; } ");
    wg_row->setCursor(Qt::PointingHandCursor);

    boxFavorGroup->addWidget(wg_row);

    // 커넥트
    connect(btn_del, SIGNAL(clicked()), this, SLOT(slot_clickedRemoveFavorFolder()));
}

/**
 * @brief MusicFolder_EditFavor::slot_clickedRemoveFavorFolder : [슬롯] 삭제 클릭
 */
void MusicFolder_EditFavor::slot_clickedRemoveFavorFolder(){
    DialogConfirm *dlg = new DialogConfirm(this);
    dlg->setTitle(tr("Delete Favorite Folder"));
    //dlg->setTitle(tr("즐겨찾기 폴더 삭제"));
    dlg->setText(tr("Are you sure you want to delete?"));
    //dlg->setText(tr("정말 삭제하시겠습니까?"));

    int result = dlg->exec();

    if(result==QDialog::Accepted){
        requestRemoveFavorFolder(sender()->property("albumFavName").toString(), sender()->property("albumFavPath").toString(), sender()->property("albumFavPathKey").toString());
    }

    delete dlg;
}

/**
 * @brief MusicFolder_EditFavor::requestFavorList : 즐겨찾기 목록 요청
 */
void MusicFolder_EditFavor::requestFavorList(){

    QJsonObject tmp_json;
    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_FAVORITE_LIST, QString("http://%1:%2/media.album.folder.favs.get")
                     .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
}

/**
 * @brief MusicFolder_EditFavor::requestRemoveFavorFolder : 즐겨찾기 폴더 삭제 요청
 * @param p_name QString 즐겨찾기 이름
 * @param p_path QString 즐겨찾기 경로
 */
void MusicFolder_EditFavor::requestRemoveFavorFolder(const QString &p_name, const QString &p_path, const QString &p_pathKey){
    QJsonObject tmp_jsonFav;
    tmp_jsonFav.insert("albumFavName", p_name);
    tmp_jsonFav.insert("albumFavPath", p_path);
    tmp_jsonFav.insert("albumFavPathKey", p_pathKey);

    QJsonArray jsonArrData;
    jsonArrData.append(tmp_jsonFav);

    QJsonObject jsonData;
    jsonData.insert("data", jsonArrData);

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_FAVORITE_DEL, QString("http://%1:%2/media.album.folder.fav.delete")
                     .arg(global.device.getDeviceIP()).arg(global.port), jsonData, true);
}

/**
 * @brief MusicFolder_EditFavor::setResultOfFavorList : 즐겨찾기 폴더 목록 요청결과 처리
 * @param p_jsonObject QJsonObject
 */
void MusicFolder_EditFavor::setResultOfFavorList(const QJsonObject &p_jsonObject){

    if(p_jsonObject.contains("data")){
        setFavorData(p_jsonObject["data"].toArray());
    }
}

/**
 * @brief MusicFolder_EditFavor::setResultOfRemoveFavorFolder
 * @param p_jsonData
 */
void MusicFolder_EditFavor::setResultOfRemoveFavorFolder(const QJsonObject &p_jsonData){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_status = "status";
    const QString jsonKey_outs = "outs";

    if(p_jsonData.contains(jsonKey_flagOk)&&p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_status)){
            QJsonObject jsonStatus = p_jsonData[jsonKey_status].toObject();
            // TODO:: Search Devices 16/02/2021 by disket
            //if(jsonStatus.contains(jsonKey_outs)&&jsonStatus[jsonKey_outs].toString().toLower()=="ok"){
                requestFavorList();
            //}
        }
    }
}


/**
 * @brief MusicFolder_EditFavor::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicFolder_EditFavor::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_FAVORITE_DEL){
        setResultOfRemoveFavorFolder(p_jsonObject);
    }
    else if(p_id == HTTP_FAVORITE_LIST){
        setResultOfFavorList(p_jsonObject);
    }

    sender()->deleteLater();
}
