#include "dialogArtistList.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

//#include "bugs/AbstractBugsSubWidget.h"
#include "bugs/ConvertData_forBugs.h"
#include "bugs/ConvertGenreCode_BugsAPI.h"
#include "bugs/DataCommon_Bugs.h"
#include "bugs/ItemArtist_bugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "tidal/AbstractItem.h"
//#include "tidal/AbstractTidalSubWidget.h"
#include "tidal/ConvertData.h"
#include "tidal/ItemArtist.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"


//#include "qobuz/AbstractQobuzSubWidget.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ItemArtist_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ProcRosePlay_withQobuz.h"

#include <QDebug>
#include <QJsonObject>

const int DLG_WIDTH = 650;
const int DLG_HIGHT = 550;

const int SECTION_FOR_MORE_POPUP___artist = 1;

DialogArtistList::DialogArtistList(QWidget *parent) : QDialog(parent){

    linker = Linker::getInstance();

    this->list_artist_forBugs = new QList<bugs::ArtistItemData>;
    this->list_artist_forTidal = new QList<tidal::ArtistItemData>;
    this->list_artist_forQobuz = new QList<qobuz::ArtistItemData>;

    setInit();
    setUIControl();
}


DialogArtistList::~DialogArtistList(){

}

void DialogArtistList::setInit(){
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(DLG_WIDTH, DLG_HIGHT);
    this->setStyleSheet("background-color:transparent;");
}

void DialogArtistList::setUIControl(){

    this->dlg_widget = new QWidget(this);
    this->dlg_widget->setStyleSheet("background-color:#333333;");
    this->dlg_widget->setFixedSize(DLG_WIDTH, DLG_HIGHT);

    QWidget *title_widget = new QWidget(dlg_widget);
    title_widget->setObjectName("wg_top");
    title_widget->setStyleSheet("#wg_top {background-color:transparent;border-bottom:1px solid #777777;}");
    title_widget->setGeometry(0, 0, DLG_WIDTH, 50);

    this->lb_title = new QLabel(title_widget);
    this->lb_title->setStyleSheet("color:#FFFFFF;font-size:20px;background-color:transparent;");
    this->lb_title->setGeometry(20, 0, DLG_WIDTH - 100, 50);

    QPushButton *btn_quit = GSCommon::getUIBtnImg("btn_quit", ":images/icon_close_gray.png", title_widget);
    btn_quit->setFixedSize(QSize(20, 20));
    btn_quit->setCursor(Qt::PointingHandCursor);
    btn_quit->setGeometry(DLG_WIDTH - 40, 15, 20, 20);

    this->dlg_layout = new FlowLayout(0, 0, 20);
    GSCommon::clearLayout(this->dlg_layout);
    //this->dlg_layout->setSizeConstraint(QLayout::SetMinimumSize);    

    this->dlg_body_widget = new QWidget();
    this->dlg_body_widget->setLayout(dlg_layout);
    this->dlg_body_widget->setContentsMargins(0, 20, 5, 0);
    this->dlg_body_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //this->dlg_body_widget->setGeometry(0, 0, DLG_WIDTH - 20, DLG_HIGHT - 50);

    this->dlg_scrollArea = new QScrollArea(dlg_widget);
    this->dlg_scrollArea->setWidgetResizable(true);
    this->dlg_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->dlg_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->dlg_scrollArea->setStyleSheet("background-color:transparent;border:0px;");
    this->dlg_scrollArea->setGeometry(20, 50, DLG_WIDTH - 20, DLG_HIGHT - 50);
    this->dlg_scrollArea->setWidget(dlg_body_widget);

    connect(btn_quit, SIGNAL(clicked()), this, SLOT(reject()));
}


void DialogArtistList::setData(const QJsonObject& jsonObj){

    GSCommon::clearLayout(this->dlg_layout);
    if(this->dlg_layout->count() > 0){
        this->dlg_layout->removeWidget(this->dlg_layout->widget());
    }

    if(ProcJsonEasy::getString(jsonObj, "manufacture") == "bugs"){
        this->list_artist_forBugs->clear();

        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id");

        for(int j = 0; j < tmp_jsonArr_artist.count(); j++){
            this->list_artist_id.append(tmp_jsonArr_artist.at(j).toInt());
        }

        for(int i = 0; i < this->list_artist_id.count(); i++){
            bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
            connect(proc, &bugs::ProcBugsAPI::completeReq_artist, this, &DialogArtistList::slot_applyResult_artistInfo_forBugs);
            proc->request_bugs_get_artist(this->list_artist_id.at(i));
        }
    }
    else if(ProcJsonEasy::getString(jsonObj, "manufacture") == "tidal"){
        this->list_artist_forTidal->clear();

        QJsonArray tmp_jsonArr_artist = ProcJsonEasy::getJsonArray(jsonObj, "list_artist_id");

        for(int j = 0; j < tmp_jsonArr_artist.count(); j++){
            this->list_artist_id.append(tmp_jsonArr_artist.at(j).toInt());
        }

        for(int i = 0; i < this->list_artist_id.count(); i++){
            tidal::ProcCommon *proc = new tidal::ProcCommon(this);
            connect(proc, &tidal::ProcCommon::completeReq_artist, this, &DialogArtistList::slot_applyResult_artistInfo_forTidal);
            proc->request_tidal_get_artist(this->list_artist_id.at(i));
        }
    }
    else if(ProcJsonEasy::getString(jsonObj, "manufacture") == "qobuz"){

    }
}


// BUGS ----------------------------------------- START

void DialogArtistList::slot_applyResult_artistInfo_forBugs(const bugs::ArtistItemData &data_artist){

    int start_index = this->list_artist_forBugs->length();
    this->list_artist_forBugs->append(data_artist);

    QString title_tmp = tr("Artist") + QString(" (%1)").arg(this->list_artist_id.count());
    this->lb_title->setText(title_tmp);

    for(int i = 0 ; i < (this->list_artist_forBugs->size() - start_index); i++){
        bugs::ItemArtist_bugs *tmp_widget = new bugs::ItemArtist_bugs(start_index + i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_180x180);
        connect(tmp_widget, &bugs::ItemArtist_bugs::signal_clicked, this, &DialogArtistList::slot_clickedItemArtist_forBugs);
        tmp_widget->setData(this->list_artist_forBugs->at(start_index));
        this->dlg_layout->addWidget(tmp_widget);
    }
}



void DialogArtistList::slot_clickedItemArtist_forBugs(const tidal::AbstractItem::ClickMode clickMode){
    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    // ClickMode 별로 처리
    this->proc_clicked_itemArtist_forBugs(this->list_artist_forBugs, clickMode, index, section);
}


void DialogArtistList::proc_clicked_itemArtist_forBugs(bugs::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    if(index >= 0){
        if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
            // Artist Detail 페이지 진입
            this->hide();

            QJsonObject tmp_Obj;
            tmp_Obj.insert("artist_id", data_artist.artist_id);
            tmp_Obj.insert("artist_nm", data_artist.artist_nm);

            if(global.user_forBugs.dlg_isShow() == true){
                global.user_forBugs.dlg_set_state(false);
            }

            if(global.user_forBugs.dlg_isSignal() == true){
                global.user_forBugs.dlg_set_signal(false);
            }

            if(global.user_forBugs.flag_rosehome == true){
                global.user_forBugs.rosehome_obj = QJsonObject();
                global.user_forBugs.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);
                bugs::ArtistItemData tmp_data_artist;
                tmp_data_artist.artist_id = data_artist.artist_id;
                QJsonObject tmp_data = bugs::ConvertData_forBugs::getObjectJson_artistData(tmp_data_artist);
                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Bugs));
            }
            else{
                emit linker->signal_Artist_movePage(tmp_Obj);
            }
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptionPopup 띄우기 필요
            //this->makeObj_optMorePopup(OptMorePopup::Bugs_Album, ConvertData_forBugs::getConvertOptHeaderData(data_album), index, section);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Add){
            // 좋아요 하트 On 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
            bugs::ItemPositionData itemPosData;
            itemPosData.section = section;
            itemPosData.index = index;
            itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
            itemPosData.likes_yn = true;
            this->proc_preUpdate_favorite_artist_forBugs(itemPosData);

            bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
            proc->request_bugs_addFavorite_artist(data_artist.artist_id, bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::FavBtn_Delete){
            // 좋아요 하트 Off (해제) 처리 - Case (2) :: 아래 proc_clicked_itemTrack_inList 함수 주석 참고
            bugs::ItemPositionData itemPosData;
            itemPosData.section = section;
            itemPosData.index = index;
            itemPosData.data_id = QString("%1").arg(data_artist.artist_id);
            itemPosData.likes_yn = false;
            this->proc_preUpdate_favorite_artist_forBugs(itemPosData);

            bugs::ProcBugsAPI *proc = new bugs::ProcBugsAPI(this);
            proc->request_bugs_deleteFavorite_artist(data_artist.artist_id, bugs::ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
        }
    }
}

void DialogArtistList::proc_clicked_itemArtist_forBugs(QList<bugs::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    int real_index = this->checkValid_index(list_artist->length(), index);
    if(real_index >= 0){
        bugs::ArtistItemData data = list_artist->at(real_index);
        this->proc_clicked_itemArtist_forBugs(data, clickMode, real_index, section);
    }
}


void DialogArtistList::proc_preUpdate_favorite_artist_forBugs(const bugs::ItemPositionData & data_itemPos){
    bugs::ArtistItemData data_artist = this->list_artist_forBugs->at(data_itemPos.index);
    data_artist.likes_yn = data_itemPos.likes_yn;
    this->list_artist_forBugs->replace(data_itemPos.index, data_artist);
}

// BUGS ----------------------------------------- END



// TIDAL ----------------------------------------- START

void DialogArtistList::slot_applyResult_artistInfo_forTidal(const tidal::ArtistItemData &data_artist){

    int start_index = this->list_artist_forTidal->length();
    this->list_artist_forTidal->append(data_artist);

    QString title_tmp = tr("Artist") + QString(" (%1)").arg(this->list_artist_id.count());
    this->lb_title->setText(title_tmp);

    for(int i = 0 ; i < (this->list_artist_forTidal->size() - start_index); i++){
        tidal::ItemArtist *tmp_widget = new tidal::ItemArtist(start_index + i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_180x180);
        connect(tmp_widget, &tidal::ItemArtist::signal_clicked, this, &DialogArtistList::slot_clickedItemArtist_forTidal);
        tmp_widget->setData(this->list_artist_forTidal->at(start_index));
        this->dlg_layout->addWidget(tmp_widget);
    }
}



void DialogArtistList::slot_clickedItemArtist_forTidal(const tidal::AbstractItem::ClickMode clickMode){
    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    // ClickMode 별로 처리
    this->proc_clicked_itemArtist_forTidal(this->list_artist_forTidal, clickMode, index, section);
}


void DialogArtistList::proc_clicked_itemArtist_forTidal(tidal::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    if(index >= 0){
        if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
            // Artist Detail 페이지 진입
            this->hide();

            QJsonObject tmp_Obj;
            tmp_Obj.insert("artist_id", data_artist.id);
            tmp_Obj.insert("artist_name", data_artist.name);

            if(global.user_forTidal.dlg_isShow() == true){
                global.user_forTidal.dlg_set_state(false);
            }

            if(global.user_forTidal.flag_rosehome == true){
                global.user_forTidal.rosehome_obj = QJsonObject();
                global.user_forTidal.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_T_ARTIST_DETAIL);
                tidal::ArtistItemData tmp_data_artist;
                tmp_data_artist.id = data_artist.id;
                QJsonObject tmp_data = tidal::ConvertData::getObjectJson_artistData(tmp_data_artist);
                global.user_forTidal.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Tidal));
            }
            else{
                emit linker->signal_Artist_movePage(tmp_Obj);
            }
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptionPopup 띄우기 필요
            this->makeObj_optMorePopup_forTidal(OptMorePopup::Tidal_Artist, tidal::ConvertData::getConvertOptHeaderData(data_artist), index, section);
        }
    }
}

void DialogArtistList::proc_clicked_itemArtist_forTidal(QList<tidal::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    int real_index = this->checkValid_index(list_artist->length(), index);
    if(real_index >= 0){
        tidal::ArtistItemData data = list_artist->at(real_index);
        this->proc_clicked_itemArtist_forTidal(data, clickMode, real_index, section);
    }
}

void DialogArtistList::makeObj_optMorePopup_forTidal(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index, const int section, const bool flagForceHide_favorite){
    // OptMorePopup 을 띄운다.
    OptMorePopup *optPopup = new OptMorePopup(this);
    connect(optPopup, &OptMorePopup::signal_menuClicked, this, &DialogArtistList::slot_optMorePopup_menuClicked_forTidal);
    optPopup->setMenuMode(menuMode, data_header, index, section, flagForceHide_favorite);
    optPopup->showPopup();
}

void DialogArtistList::slot_optMorePopup_menuClicked_forTidal(const OptMorePopup::ClickMode clickMode, const int index, const int section){
    Q_UNUSED(section);
    this->proc_clicked_optMorePopup_fromArtist_forTidal(this->list_artist_forTidal, index, clickMode);
}

void DialogArtistList::proc_clicked_optMorePopup_fromArtist_forTidal(tidal::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode){
    if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
        // 즐겨찾기 추가 - Artist
        tidal::ProcCommon *proc = new tidal::ProcCommon(this);
        proc->request_tidal_addFavorite_artist(data_artist.id);
    }
    else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
        // 즐겨찾기 삭제 - Artist
        tidal::ProcCommon *proc = new tidal::ProcCommon(this);
        proc->request_tidal_deleteFavorite_artist(data_artist.id);
    }
}

void DialogArtistList::proc_clicked_optMorePopup_fromArtist_forTidal(QList<tidal::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode){
    int real_index = this->checkValid_index(list_artist->length(), index);
    if(real_index >= 0){
        tidal::ArtistItemData data = list_artist->at(real_index);
        this->proc_clicked_optMorePopup_fromArtist_forTidal(data, clickMode);
    }
}

// TIDAL ----------------------------------------- END



// QOBUZ ----------------------------------------- START

void DialogArtistList::slot_applyResult_artistInfo_forQobuz(const qobuz::ArtistItemData &data_artist){

    int start_index = this->list_artist_forTidal->length();
    this->list_artist_forQobuz->append(data_artist);

    QString title_tmp = tr("Artist") + QString(" (%1)").arg(this->list_artist_id.count());
    this->lb_title->setText(title_tmp);

    for(int i = 0 ; i < (this->list_artist_forQobuz->size() - start_index); i++){
        qobuz::ItemArtist_qobuz *tmp_widget = new qobuz::ItemArtist_qobuz(start_index + i, SECTION_FOR_MORE_POPUP___artist, tidal::AbstractItem::ImageSizeMode::Square_180x180);
        connect(tmp_widget, &qobuz::ItemArtist_qobuz::signal_clicked, this, &DialogArtistList::slot_clickedItemArtist_forQobuz);
        tmp_widget->setData(this->list_artist_forQobuz->at(start_index));
        this->dlg_layout->addWidget(tmp_widget);
    }
}



void DialogArtistList::slot_clickedItemArtist_forQobuz(const tidal::AbstractItem::ClickMode clickMode){
    int index = ((tidal::AbstractItem*)sender())->index();
    int section = ((tidal::AbstractItem*)sender())->section();

    // ClickMode 별로 처리
    this->proc_clicked_itemArtist_forQobuz(this->list_artist_forQobuz, clickMode, index, section);
}


void DialogArtistList::proc_clicked_itemArtist_forQobuz(qobuz::ArtistItemData& data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    if(index >= 0){
        if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
            // Artist Detail 페이지 진입
            this->hide();

            QJsonObject tmp_Obj;
            tmp_Obj.insert("artist_id", data_artist.id);
            tmp_Obj.insert("artist_name", data_artist.name);

            if(global.user_forQobuz.dlg_isShow() == true){
                global.user_forQobuz.dlg_set_state(false);
            }

            emit linker->signal_Artist_movePage(tmp_Obj);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptionPopup 띄우기 필요
            this->makeObj_optMorePopup_forQobuz(OptMorePopup::Qobuz_Artist, qobuz::ConvertData::getConvertOptHeaderData(data_artist), index, section);
        }
    }
}

void DialogArtistList::proc_clicked_itemArtist_forQobuz(QList<qobuz::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
    int real_index = this->checkValid_index(list_artist->length(), index);
    if(real_index >= 0){
        qobuz::ArtistItemData data = list_artist->at(real_index);
        this->proc_clicked_itemArtist_forQobuz(data, clickMode, real_index, section);
    }
}

void DialogArtistList::makeObj_optMorePopup_forQobuz(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index, const int section, const bool flagForceHide_favorite){
    // OptMorePopup 을 띄운다.
    OptMorePopup *optPopup = new OptMorePopup(this);
    connect(optPopup, &OptMorePopup::signal_menuClicked, this, &DialogArtistList::slot_optMorePopup_menuClicked_forQobuz);
    optPopup->setMenuMode(menuMode, data_header, index, section, flagForceHide_favorite);
    optPopup->showPopup();
}

void DialogArtistList::slot_optMorePopup_menuClicked_forQobuz(const OptMorePopup::ClickMode clickMode, const int index, const int section){
    Q_UNUSED(section);
    this->proc_clicked_optMorePopup_fromArtist_forQobuz(this->list_artist_forQobuz, index, clickMode);
}

void DialogArtistList::proc_clicked_optMorePopup_fromArtist_forQobuz(qobuz::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode){
    if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
        // 즐겨찾기 추가 - Artist
        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
        //proc->request_qobuz_addFavorite_artist(data_artist.id);
        proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), true);
    }
    else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
        // 즐겨찾기 삭제 - Artist
        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
        //proc->request_qobuz_deleteFavorite_artist(data_artist.id);
        proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), false);
    }
}

void DialogArtistList::proc_clicked_optMorePopup_fromArtist_forQobuz(QList<qobuz::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode){
    int real_index = this->checkValid_index(list_artist->length(), index);
    if(real_index >= 0){
        qobuz::ArtistItemData data = list_artist->at(real_index);
        this->proc_clicked_optMorePopup_fromArtist_forQobuz(data, clickMode);
    }
}

// QOBUZ ----------------------------------------- END


int DialogArtistList::checkValid_index(int length_ofData, int index){
    if(index<0 || index>=length_ofData){
        index = -1;
    }
    return index;
}
