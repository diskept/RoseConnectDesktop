#include "dialogVideoList.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "bugs/AbstractBugsSubWidget.h"
#include "bugs/ConvertData_forBugs.h"
#include "bugs/ConvertGenreCode_BugsAPI.h"
#include "bugs/DataCommon_Bugs.h"
#include "bugs/ItemVideo_bugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "tidal/AbstractItem.h"
#include "roseHome/ProcCommon_forRosehome.h"//c220902_1

#include <QDebug>
#include <QJsonObject>

const int DLG_WIDTH = 590;
const int DLG_HIGHT = 540;

const int SECTION_FOR_MORE_POPUP___video = 1;


DialogVideoList::DialogVideoList(QWidget *parent) : QDialog(parent){

    linker = Linker::getInstance();

    this->list_video = new QList<bugs::VideoItemData>();

    setInit();
    setUIControl();
}

DialogVideoList::~DialogVideoList(){

}

void DialogVideoList::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    //this->setFixedWidth(DLG_WIDTH);
    //this->setFixedHeight(DLG_HIGHT);
    this->setStyleSheet("background-color:transparent;");
}

void DialogVideoList::setUIControl(){

    this->dlg_widget = new QWidget(this);
    this->dlg_widget->setStyleSheet("background-color:#333333;");
    this->dlg_widget->setFixedSize(DLG_WIDTH, DLG_HIGHT);

    QWidget *title_widget = new QWidget(dlg_widget);
    title_widget->setObjectName("wg_top");
    title_widget->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #777777;}");
    title_widget->setGeometry(0, 0, DLG_WIDTH, 50);

    this->lb_title = new QLabel(title_widget);
    this->lb_title->setStyleSheet("color:#FFFFFF;font-size:20px;background-color:transparent;");
    //this->lb_title->setText(tr("Music video of this song"));
    this->lb_title->setGeometry(20, 0, DLG_WIDTH - 270, 50);

    QPushButton *btn_quit = GSCommon::getUIBtnImg("btn_quit", ":images/icon_close_gray.png", title_widget);
    btn_quit->setFixedSize(QSize(20, 20));
    btn_quit->setCursor(Qt::PointingHandCursor);
    btn_quit->setGeometry(DLG_WIDTH - 40, 15, 20, 20);

    this->dlg_layout = new FlowLayout(0, 0, 20);
    //this->dlg_layout->setSizeConstraint(QLayout::SetMinimumSize);

    /*this->dlg_body_widget = new QWidget(dlg_widget);
    this->dlg_body_widget->setLayout(dlg_layout);
    //this->dlg_body_widget->setContentsMargins(0, 20, 5, 0);
    //this->dlg_body_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->dlg_body_widget->setGeometry(20, 70, 830, DLG_HIGHT - 50);*/

    this->dlg_body_widget = new QWidget();
    this->dlg_body_widget->setLayout(dlg_layout);
    this->dlg_body_widget->setContentsMargins(0, 0, 5, 20);
    this->dlg_body_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->dlg_body_widget->setGeometry(0, 0, DLG_WIDTH - 20, DLG_HIGHT - 50);

    this->dlg_scrollArea = new QScrollArea(dlg_widget);
    this->dlg_scrollArea->setWidgetResizable(true);
    this->dlg_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->dlg_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->dlg_scrollArea->setStyleSheet("background-color:transparent;border:0px;");
    this->dlg_scrollArea->setGeometry(20, 100, DLG_WIDTH - 20, DLG_HIGHT - 100);
    this->dlg_scrollArea->setWidget(dlg_body_widget);

    // 재생버튼 관련 - Video 용으로
    //  전체재생
    QPixmap pixmap_play(":/images/icon_list_allplay.png");
    QPushButton *btn_playAll = new QPushButton(dlg_widget);
    btn_playAll->setIcon(pixmap_play);
    btn_playAll->setIconSize(pixmap_play.rect().size());
    btn_playAll->setText(tr("Play All"));//btn_playAll->setText(tr("전체재생"));
    btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    btn_playAll->setCursor(Qt::PointingHandCursor);
    btn_playAll->setGeometry(00, 60, 120, 30);

    connect(btn_quit, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_playAll, &QPushButton::clicked, this, &DialogVideoList::slot_btnClicked_playAll);
}

void DialogVideoList::setData(const QJsonObject& jsonObj){

    bugs::TrackItemData tmp_data_track = bugs::ConvertData_forBugs::convertData_trackData(jsonObj);

    this->data_track = tmp_data_track;

    this->list_video->clear();
    GSCommon::clearLayout(this->dlg_layout);

    for(int i = 0 ; i < this->data_track.list_mv_id.count() ; i++){
        bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
        connect(proc, &bugs::ProcBugsAPI::completeReq_video, this, &DialogVideoList::slot_applyResult_videoInfo);
        proc->request_bugs_get_video(this->data_track.list_mv_id.at(i));
    }
}

void DialogVideoList::slot_applyResult_videoInfo(bugs::VideoItemData data_video, const QJsonObject& jsonObj_dataToPlay){

    QList<bugs::VideoItemData> tmp_list;
    tmp_list.append(data_video);

    int start_index = this->list_video->length();
    this->list_video->append(tmp_list);
    this->jsonArr_videos_toPlay.append(jsonObj_dataToPlay);

    QString title_tmp = tr("Music video of this song.") + QString(" (%1)").arg(this->data_track.list_mv_id.count());
    this->lb_title->setText(title_tmp);

    for(int i = 0 ; i < (list_video->size() - start_index) ; i++){
        bugs::ItemVideo_bugs *tmp_widget = new bugs::ItemVideo_bugs(start_index + i, SECTION_FOR_MORE_POPUP___video, tidal::AbstractItem::ImageSizeMode::Ractangle_251x139);
        connect(tmp_widget, &bugs::ItemVideo_bugs::signal_clicked, this, &DialogVideoList::slot_clickedItemVideo);
        tmp_widget->setData(list_video->at(start_index));
        this->dlg_layout->addWidget(tmp_widget);
    }
}

void DialogVideoList::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){
    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    // ClickMode 별로 처리
    this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
}

void DialogVideoList::proc_clicked_itemVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    if(index >= 0){
        if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn
                || clickMode == tidal::AbstractItem::ClickMode::AllBox
        ){
            // Video 바로 재생
            bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
            procRosePlay->requestPlayRose_byVideos(jsonArray_toPlayAll, index, OptMorePopup::ClickMode::Play_RightNow_withReorder);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            print_debug();

            // OptionPopup 띄우기 필요
            this->makeObj_optMorePopup(OptMorePopup::Bugs_Video, bugs::ConvertData_forBugs::getConvertOptHeaderData(data_video), index, section);
        }
    }
}

void DialogVideoList::slot_applyResult_getShareLink(const QString &link){//c220902_1

    this->link_str = link;
}

void DialogVideoList::proc_clicked_itemVideo(QList<bugs::VideoItemData>* list_video, const QJsonArray& jsonArray_toPlayAll, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    int real_index = this->checkValid_index(list_video->length(), index);
    if(real_index >= 0){
        bugs::VideoItemData data = list_video->at(real_index);
        this->proc_clicked_itemVideo(data, jsonArray_toPlayAll, clickMode, real_index, section);
    }
}


void DialogVideoList::makeObj_optMorePopup(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index, const int section, const bool flagForceHide_favorite){
    // OptMorePopup 을 띄운다.
    OptMorePopup *optPopup = new OptMorePopup(this);
    connect(optPopup, &OptMorePopup::signal_menuClicked, this, &DialogVideoList::slot_optMorePopup_menuClicked);
    optPopup->setMenuMode(menuMode, data_header, index, section, flagForceHide_favorite);
    optPopup->showPopup();
}

int DialogVideoList::checkValid_index(int length_ofData, int index){
    if(index<0 || index>=length_ofData){
        index = -1;
    }

    return index;
}

void DialogVideoList::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
    if(section == SECTION_FOR_MORE_POPUP___video){
        this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
    }
}

void DialogVideoList::proc_clicked_optMorePopup_fromVideo(bugs::VideoItemData& data_video, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){



            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

    )
    {
        // Rose Play 요청
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byVideos(jsonArr_toPlayAll, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                //qDebug() << data_video.list_artist_id << data_video.list_artist_id.count();

                this->hide();

                QJsonObject tmp_Obj = bugs::ConvertData_forBugs::getObjectJson_videoData(data_video);

                global.user_forBugs.dlg_set_signal(false);
                emit linker->signal_Video_movePage(tmp_Obj);
            }else  if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                emit linker->signal_dialog_share_link(link_str);
                //setUIShare();
                //qDebug() << "this->shareLink="<<this->shareLink;

            }

}

void DialogVideoList::proc_clicked_optMorePopup_fromVideo(QList<bugs::VideoItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){
    int real_index = this->checkValid_index(list_track->length(), index);
    if(real_index >= 0){
        bugs::VideoItemData data = list_track->at(real_index);
        this->proc_clicked_optMorePopup_fromVideo(data, jsonArr_toPlayAll, real_index, clickMode);
    }
}

void DialogVideoList::slot_btnClicked_playAll(){
    // Rose Play 요청 - Bugs Video (전체재생 / 셔플재생)
    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
    procRosePlay->requestPlayRose_byVideos(this->jsonArr_videos_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, bugs::ProcRoseAPI_withBugs::PlayShuffleMode::JustPlay);
}


