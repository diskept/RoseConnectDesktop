#include "rosetube_viewalldetail.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/rosesettings.h"
#include "widget/framerosetube.h"
#include "widget/VerticalScrollArea.h"
#include "QStackedWidget"
#include "QApplication"

#include <QSettings>
#include <QJsonArray>
#include <QScroller>
#include <QWheelEvent>

const int CNT_DRAW_ROW = 2;
const int HTTP_CATEGORY_PLAY_LIST = 99;

//const QString PAGECODE_VA_ROSE_PLAY_LIST = "viewAll_rosetube_playList";


/**
 * @brief RoseTube_ViewAllDetail::RoseTube_ViewAllDetail : 로즈튜브-탐색-장르/상황 기본
 * @details 로즈튜브 장르별, 상황별 로즈튜브 리스트 화면
 * @param parent
 */
RoseTube_ViewAllDetail::RoseTube_ViewAllDetail(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief RoseTube_ViewAllDetail::setInit : 초기 세팅
 */
void RoseTube_ViewAllDetail::setInit(){
    linker = Linker::getInstance();
    listJson = new QList<QJsonObject>();
}

/**
 * @brief RoseTube_ViewAllDetail::setUIControl : UI 세팅
 */
void RoseTube_ViewAllDetail::setUIControl(){
    box_contents = new QVBoxLayout;
    box_contents->setSpacing(0);
    box_contents->setContentsMargins(0,0,0,0);
    box_contents->setAlignment(Qt::AlignTop);

    wg_contents = new QWidget;
    wg_contents->setLayout(box_contents);
    wg_contents->setContentsMargins(33,0,0,0);

    VerticalScrollArea *scrollArea = new VerticalScrollArea;
    scrollArea->setWidget(wg_contents);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setContentsMargins(0,0,0,0);
    boxMain->addWidget(scrollArea);
    boxMain->setSizeConstraint(QLayout::SetNoConstraint);

    setLayout(boxMain);
}

void RoseTube_ViewAllDetail::setData(const QJsonObject &p_jsonObject){
    const QString jsonKey_id = "menu_id";
    const QString jsonKey_nm = "nm";
    const QString jsonKey_type = "type";

    int tmp_id = -1;
    QString tmp_typeName = "";
    QString tmp_typeSubName = "";

    if(p_jsonObject.contains(jsonKey_id)){
        tmp_id = p_jsonObject[jsonKey_id].toInt();
    }
    if(p_jsonObject.contains(jsonKey_nm)){
        tmp_typeSubName = p_jsonObject[jsonKey_nm].toString();
    }
    if(p_jsonObject.contains(jsonKey_type)){
        tmp_typeName = p_jsonObject[jsonKey_type].toString();
    }

    if(typeName!=tmp_typeName || typeSubName!=tmp_typeSubName || typeId!=tmp_id){
        typeId = tmp_id;
        typeName = tmp_typeName;
        typeSubName = tmp_typeSubName;


        requestData();
    }
}

/**
 * @brief RoseTube_ViewAllDetail::appendPlayList : 플레이리스트 UI 추가
 * @param p_jsonObject QJsonObject 플레이리스트 정보
 */
void RoseTube_ViewAllDetail::appendPlayList(const QJsonObject( &p_jsonObject)){



    const QString jsonKey_nm = "nm";            // 타이틀
    const QString jsonKey_URLS = "_URLs";       // 플레이리스트 정보

    QString tmp_title;

    if(p_jsonObject.contains(jsonKey_nm)){
        tmp_title = p_jsonObject[jsonKey_nm].toString();
    }
    if(p_jsonObject.contains(jsonKey_URLS)){
        appendPlayListItem(tmp_title, p_jsonObject[jsonKey_URLS].toArray());

        // UI 강제 업데이트
        QApplication::processEvents();
    }
}

/**
 * @brief RoseTube_ViewAllDetail::appendPlayListItem : 플레이리스트 곡 정보 UI 추가
 * @param p_title QString 플레이리스트명
 * @param p_jsonArray QJsonArray 곡 정보
 */
void RoseTube_ViewAllDetail::appendPlayListItem(const QString &p_title, const QJsonArray &p_jsonArray){
    const QString jsonKey_no = "no";
    const QString jsonKey_tracks = "tracks";

    // 타이틀 + View ALL
    QLabel *labelSubTitle_1 = new QLabel;
    labelSubTitle_1->setText(p_title);
    labelSubTitle_1->setStyleSheet("font-size:22px;color:#FFFFFF;");

    QPushButton *btnViewAll_1 = new QPushButton;
    btnViewAll_1->setText(tr("View All"));
    btnViewAll_1->setStyleSheet("QPushButton{background:transparent;color:#CCCCCC;font-size:20px;}QPushButton:hover{color:#B18658;}");
    btnViewAll_1->setProperty(KEY_VIEWALL_MODE.toStdString().c_str(), PAGECODE_VA_ROSE_PLAY_LIST);
    btnViewAll_1->setProperty("title", p_title);
    btnViewAll_1->setProperty("type","common");
    btnViewAll_1->setCursor(Qt::PointingHandCursor);

    connect(btnViewAll_1, SIGNAL(clicked()), this, SLOT(slot_clickedViewAll()));

    QHBoxLayout *boxTop = new QHBoxLayout;
    boxTop->setSpacing(0);
    boxTop->setContentsMargins(0,10,28,0);
    boxTop->addWidget(labelSubTitle_1,0, Qt::AlignVCenter);
    boxTop->addStretch();
    boxTop->addWidget(btnViewAll_1,0, Qt::AlignVCenter);

    QWidget *widget_top = new QWidget;
    widget_top->setContentsMargins(0,0,0,0);
    widget_top->setLayout(boxTop);


    // 플레이리스트 곡 목록
    QHBoxLayout *boxPlayList = new QHBoxLayout;
    boxPlayList->setSpacing(0);
    boxPlayList->setContentsMargins(0,0,0,0);

    QWidget *wg_playList = new QWidget;
    wg_playList->setLayout(boxPlayList);

    // JsonArray Size 항상 1
    for(int i = 0 ; i < p_jsonArray.size(); i++){
        QJsonObject jsonItems = p_jsonArray.at(i).toObject();

        if(jsonItems.contains(jsonKey_no)){
            btnViewAll_1->setProperty("index", jsonItems[jsonKey_no].toInt());
        }

        if(jsonItems.contains(jsonKey_tracks)){
            QJsonArray jsonArrTrack = jsonItems[jsonKey_tracks].toArray();
            for(int j=0; j < jsonArrTrack.size(); j++){

                FrameRoseTube *tmp_widget = new FrameRoseTube(OptionPopup::TypeMenu::RoseTube_Common);
                tmp_widget->setRequestTrackInfoAfterSetData();
                tmp_widget->setData(jsonArrTrack.at(j).toObject());
                tmp_widget->setHover();

                boxPlayList->addWidget(tmp_widget);
            }
        }
    }



    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(wg_playList);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("margin-bottom:0px;");

    QScroller::grabGesture(scrollArea, QScroller::LeftMouseButtonGesture);


    // empty 라벨
    QLabel *lb_empty = new QLabel;
    lb_empty->setText(tr("No songs appearing in Playlists"));
    //lb_empty->setText(tr("플레이리스트 곡이 없습니다."));
    lb_empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lb_empty->setAlignment(Qt::AlignCenter);
    lb_empty->setStyleSheet("font-size:18px;color:#FFFFFF;");
    lb_empty->setFixedHeight(288);

    // empty 라벨 + 플레이리스트 곡 목록 스택
    QStackedWidget *stackPlaylist = new QStackedWidget;
    stackPlaylist->addWidget(lb_empty);
    stackPlaylist->addWidget(scrollArea);


    if(boxPlayList->count() > 0){
        stackPlaylist->setCurrentWidget(scrollArea);
    }


    QVBoxLayout *boxTotal = new QVBoxLayout;
    boxTotal->setContentsMargins(0,0,0,10);
    boxTotal->setSpacing(0);
    boxTotal->addWidget(widget_top);
    boxTotal->addWidget(stackPlaylist);

    QWidget *wg_total = new QWidget;
    wg_total->setLayout(boxTotal);
    wg_total->setStyleSheet("border:0px;");

    box_contents->addWidget(wg_total);
    wg_contents->adjustSize();
}

/**
 * @brief RoseTube_ViewAllDetail::requestData : 카테고리 데이터 요청
 */
void RoseTube_ViewAllDetail::requestData(){

    GSCommon::clearLayout(box_contents);
    listJson->clear();

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);



    QUrlQuery params;
    params.addQueryItem("name","rosetube_home");
    params.addQueryItem("lang",settings->value(rosesettings.SETTINGS_LANG,"kr").toString());
    params.addQueryItem("submenu",QString("%1/%2").arg(typeName).arg(typeSubName));

    network->request(HTTP_CATEGORY_PLAY_LIST, QString("%1/menu/get")
                     .arg(global.legacy_mod), params, true,true);
}

/**
 * @brief RoseTube_ViewAllDetail::setResultOfCategoryData : 카테고리 데이터 요청 결과 처리
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAllDetail::setResultOfCategoryData(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_submenu = "submenu";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_submenu)){
            QJsonArray jsonArrSub = p_jsonObject[jsonKey_submenu].toArray();

            for(int i = 0 ; i < jsonArrSub.size(); i++){
                //appendPlayList(jsonArrSub.at(i).toObject());
                listJson->append(jsonArrSub.at(i).toObject());
            }

        }
    }

    int tmp_cntDrawRow = CNT_DRAW_ROW+1;
    if(listJson->size() < tmp_cntDrawRow){
        tmp_cntDrawRow = listJson->size();
    }

    for(int i = 0 ; i < tmp_cntDrawRow; i++){
        if(i < listJson->count()){
            appendPlayList(listJson->at(i));
        }
    }
}

/**
 * @brief RoseTube_ViewAllDetail::slot_responseHttp : [슬롯] Http 요청결과 처리
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void RoseTube_ViewAllDetail::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_CATEGORY_PLAY_LIST){
        setResultOfCategoryData(p_jsonObject);
    }

    sender()->deleteLater();
}

/**
 * @brief RoseTube_ViewAllDetail::slot_clickedViewAll [SLOT] View All 버튼 클릭시
 */
void RoseTube_ViewAllDetail::slot_clickedViewAll(){

    QJsonObject tmp_data;
    tmp_data.insert(KEY_VIEWALL_MODE, sender()->property(KEY_VIEWALL_MODE.toStdString().c_str()).toString());
    tmp_data.insert("index", sender()->property("index").toInt());
    tmp_data.insert("type", sender()->property("type").toString());
    tmp_data.insert("title", sender()->property("title").toString());

    // 부모에게 페이지 변경하라고 시그널 보냄
    emit this->signal_clickedViewAll(tmp_data);
}

/**
 * @brief RoseTube_ViewAllDetail::wheelEvent : 휠 이벤트
 * @param event
 */
void RoseTube_ViewAllDetail::wheelEvent(QWheelEvent *event){

    // -------------------------------
    //  pixelDelta().y() always 0
    // -------------------------------
    // 더 가져오기 실행
    if(event->delta() < 0){
        if(!flagReqMore && box_contents->count() < listJson->count()){
            flagReqMore = true;

            int tmp_beginIdx = box_contents->count();
            int tmp_endIdx = tmp_beginIdx+CNT_DRAW_ROW;
            if(tmp_endIdx >= listJson->count()){
                tmp_endIdx = listJson->count()-1;
            }

            for(int i=tmp_beginIdx; i <= tmp_endIdx; i++){
                appendPlayList(listJson->at(i));
            }
            flagReqMore = false;
        }
    }
}
