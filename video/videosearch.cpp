#include "videosearch.h"
#include <common/gscommon.h>
#include <common/global.h>
#include <common/sqlitehelper.h>

#include <qlabel.h>
#include <QWheelEvent>
#include <QScrollBar>

#include <widget/framevideo.h>


/**
 * @brief VideoSearch::VideoSearch : 비디오 검색 결과 화면
 * @param parent
 */
VideoSearch::VideoSearch(QWidget *parent)
     : QWidget(parent)
{
    setInit();
    setUIControl();
}


VideoSearch::~VideoSearch(){

    this->deleteLater();
}


/**
 * @brief VideoSearch::setInit : 초기 세팅
 */
void VideoSearch::setInit(){

    linker = Linker::getInstance();
}


/**
 * @brief VideoSearch::setUIControl : UI 세팅
 */
void VideoSearch::setUIControl(){

    // -------------------------------------------------------
    //  empty 라벨 박스
    // -------------------------------------------------------
    QLabel *lb_empty = new QLabel(tr("No results found"));
    //QLabel *lb_empty = new QLabel(tr("검색결과가 없습니다."));
    lb_empty->setStyleSheet("font-size:18px; color:#FFFFFF;");
    lb_empty->setContentsMargins(0,0,0,0);
    lb_empty->setAlignment(Qt::AlignCenter);

    QVBoxLayout *box_empty = new QVBoxLayout();
    box_empty->setContentsMargins(0,0,0,0);
    box_empty->setSpacing(0);
    box_empty->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    box_empty->addWidget(lb_empty,0, Qt::AlignCenter);

    widgetEmpty = new QWidget();
    widgetEmpty->setLayout(box_empty);
    widgetEmpty->hide();


    flowLayout = new FlowLayout(0,0,20);
    flowLayout->setContentsMargins(0,0,0,10);


    QWidget *wg_contents = new QWidget;
    wg_contents->setLayout(flowLayout);
    wg_contents->setContentsMargins(0,0,0,0);

    scrollArea_content = new QScrollArea();
    scrollArea_content->setContentsMargins(0,0,0,0);
    scrollArea_content->setWidget(wg_contents);
    scrollArea_content->setWidgetResizable(true);
    scrollArea_content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_content->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea_content->setStyleSheet("border:0px;");

    QVBoxLayout *boxMain = new QVBoxLayout;
    boxMain->setContentsMargins(20,10,18,0);
    boxMain->addWidget(scrollArea_content);
    boxMain->addWidget(widgetEmpty);

    setLayout(boxMain);
}

/**
 * @brief VideoSearch::showEmptyLabel : empty 라벨 Show
 * @param bool if true show
 */
void VideoSearch::showEmptyLabel(bool p_flagShow){

    if(p_flagShow){
        widgetEmpty->setVisible(true);
        scrollArea_content->setVisible(false);
    }else{
        widgetEmpty->setVisible(false);
        scrollArea_content->setVisible(true);
    }
}

/**
 * @brief VideoSearch::search : 비디오 검색
 * @param p_text
 */
void VideoSearch::search(const QString &p_text){

    strSearchWord = p_text;

    // 부모의 메뉴명 변경 -> 앨범명 (서브메뉴는 hide)
    emit signal_changedMenuName("'" + p_text + "' " + tr("Search Result"));
    //emit signal_changedMenuName("'"+p_text+"' "+tr("검색결과"));

    setVideo();

    showEmptyLabel(flowLayout->count() > 0 ? false : true);
}

/**
 * @brief VideoSearch::setVideo : 비디오 데이터 세팅
 */
void VideoSearch::setVideo(){
    GSCommon::clearLayout(flowLayout);
    showEmptyLabel(true);

    OffsetPoint = 0;
    flag_page_first = false;

    QJsonArray jsonArrVideo = searchVideo();

    for(int i = 0 ; i < jsonArrVideo.count(); i++){
        FrameVideo *wg_video = new FrameVideo;
        wg_video->setData(jsonArrVideo.at(i).toObject());
        wg_video->setHover();
        this->flowLayout->addWidget(wg_video);
    }
}

/**
 * @brief VideoSearch::searchVideo : 비디오 조회
 */
QJsonArray VideoSearch::searchVideo(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;

    if(global.LmtCnt <= 1200){
        hwindow_width = global.LmtCnt - 80;
    }
    else{
        hwindow_width = global.LmtCnt - 240;
    }

    cnt = hwindow_width / 215;
    LMT_CNT = cnt * 10;

    if(flag_page_first == false){
        OffsetPoint = 0;
        flag_page_first = true;
    }
    else{
        OffsetPoint += LMT_CNT;
    }

    QJsonArray jsonArr;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery += " SELECT DISTINCT album, _data as 'videoFilePath' ";
        strQuery += " FROM video ";
        strQuery += " WHERE album LIKE '%"+strSearchWord+"%' OR title LIKE '%"+strSearchWord+"%' ";
        strQuery += " ORDER BY album ";
        strQuery += QString(" LIMIT %1 OFFSET %2").arg(LMT_CNT).arg(OffsetPoint);

        /*strQuery += " SELECT DISTINCT title, _data as 'videoFilePath' ";
        strQuery += " FROM video ";
        strQuery += " WHERE title LIKE '%" + strSearchWord + "%' ";
        strQuery += " ORDER BY title ";
        strQuery += QString(" LIMIT %1 OFFSET %2").arg(LMT_CNT).arg(OffsetPoint);*/

        QVariantList data;
        sqlite->exec(strQuery, data);

        for(int i = 0 ; i < data.size(); i++){
            jsonArr.append(data.at(i).toJsonObject());
        }

    }

    sqlite->close();
    delete sqlite;

    return jsonArr;
}

void VideoSearch::serachMoreVideo(){
    QJsonArray jsonArrVideo = searchVideo();

    for(int i = 0 ; i < jsonArrVideo.count(); i++){
        FrameVideo *wg_video = new FrameVideo;
        wg_video->setData(jsonArrVideo.at(i).toObject());
        wg_video->setHover();
        this->flowLayout->addWidget(wg_video);
    }
}

void VideoSearch::wheelEvent(QWheelEvent *event){

    // -------------------------------
    //  pixelDelta().y() always 0
    // -------------------------------
    // 더 가져오기 실행
    scroll_pos_currunt = this->scrollArea_content->verticalScrollBar()->sliderPosition();

    if(event->delta() < -60){
        if(scroll_pos_currunt > scroll_pos_before){
            serachMoreVideo();
        }
    }

    scroll_pos_before = scroll_pos_currunt;
}
