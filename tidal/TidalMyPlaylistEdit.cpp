#include "TidalMyPlaylistEdit.h"

#include <QDebug>
#include <QPainter>
#include <QScrollArea>
#include <QPixmapCache>

#include "ProcCommon.h"
#include <common/gscommon.h>
#include "common/ProcJsonEasy.h"
#include <widget/VerticalScrollArea.h>
#include "ConvertData.h"

namespace tidal {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;



    /**
     * @brief
     * @param parent
     */
    TidalMyPlaylistEdit::TidalMyPlaylistEdit(QWidget *parent)
        : AbstractTidalSubWidget(MainUIType::VerticalScroll, parent)
    {

        // data
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_item_track = new QList<editDelegate::ItemEditData>();

        this->label_mainTitle = this->get_addUIControl_mainTitle(tr("Edit Playlist"));
        //this->label_mainTitle = this->get_addUIControl_mainTitle(tr("플레이리스트 수정"));
        this->setUIControl_basic();

        // reset
        this->box_contents->setContentsMargins(0,10,33,0);
    }


    /**
     * @brief 소멸자.
     */
    TidalMyPlaylistEdit::~TidalMyPlaylistEdit(){
    }


    /**
     * @brief TidalMyPlaylistEdit::setJsonObject_forData
     * @param jsonObj
     */
    void TidalMyPlaylistEdit::setJsonObject_forData(const QJsonObject &jsonObj){

        tidal::PlaylistItemData tmp_data_playlist = ConvertData::convertData_playlistData(jsonObj);

        // 항상 reload
        this->flagNeedReload = true;
        this->data_playlist = tmp_data_playlist;
        this->list_track->clear();

        this->listWidget_track->clear();
        this->listWidget_track->setFixedHeight(70*5);

    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalMyPlaylistEdit::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractTidalSubWidget::setActivePage();

        if(this->flagNeedReload){
            this->playlistImageDetailInfo->setData_fromTidalData(this->data_playlist);
            this->playlistImageDetailInfo->set_display_forEditMode();

            this->isAllSelected = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;
            this->request_more_trackData();
        }

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief TidalMyPlaylistEdit::setUIControl_playlist_info
     */
    void TidalMyPlaylistEdit::setUIControl_basic(){

        // Playlist 상세정보
        this->playlistImageDetailInfo = new PlaylistImageDetailInfo();
        this->playlistImageDetailInfo->setMyData(true);
        QObject::connect(this->playlistImageDetailInfo, &PlaylistImageDetailInfo::signal_clicked, this, &TidalMyPlaylistEdit::slot_detailInfo_btnClicked);
        this->box_contents->addWidget(this->playlistImageDetailInfo, 0, Qt::AlignTop);
        this->box_contents->addSpacing(30);

        // 전체선택, 삭제 버튼
        this->setUIControl_topBtns();

        // Playlist의 트랙정보
        this->delegate_edit = new TidalEditTrackDelegate(AbstractTidalSubWidget::parent());
        QObject::connect(this->delegate_edit, &TidalEditTrackDelegate::clicked, this, &TidalMyPlaylistEdit::slot_editDelegate_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget_forEdit(this->delegate_edit);

    }


    /**
     * @brief TidalMyPlaylistEdit::setUIControl_topBtns
     */
    void TidalMyPlaylistEdit::setUIControl_topBtns(){

        // 전체선택 체크박스
        this->checkbox_allSelected = new QCheckBox();
        this->checkbox_allSelected->setObjectName("checkbox_allSelected");
        this->checkbox_allSelected->setText(tr("Select All"));
        //this->checkbox_allSelected->setText(tr("전체선택"));
        this->checkbox_allSelected->setStyleSheet(
                    "#checkbox_allSelected { background-color:transparent;font-size:16px;color:#b18658;spacing:0px;font:bold;}"
                    "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                    "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                    );
        this->checkbox_allSelected->setCursor(Qt::PointingHandCursor);
        this->checkbox_allSelected->setFixedHeight(50);
        QObject::connect(checkbox_allSelected, &QCheckBox::clicked, this, &TidalMyPlaylistEdit::slot_clickedAllSelected);

        // 삭제 버튼
        QPushButton *btn_delete = new QPushButton(tr("Delete"));
        //QPushButton *btn_delete = new QPushButton(tr("삭제"));
        btn_delete->setCursor(Qt::PointingHandCursor);
        btn_delete->setFixedSize(45,50);
        btn_delete->setStyleSheet("color:#b18658;font-size:16px;font:bold;background-color:transparent;");
        QObject::connect(btn_delete, &QPushButton::clicked, this, &TidalMyPlaylistEdit::slot_clickedDelete);


        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->addWidget(this->checkbox_allSelected);
        tmp_hBox->addStretch(1);
        tmp_hBox->addWidget(btn_delete);
        tmp_hBox->addSpacing(10);


        QWidget *widget_topBtn = new QWidget();
        widget_topBtn->setObjectName("widget_topBtn");
        widget_topBtn->setStyleSheet("#widget_topBtn {border-bottom:1px solid #cccccc; border-top:1px solid #cccccc;}");
        widget_topBtn->setLayout(tmp_hBox);

        this->box_contents->addWidget(widget_topBtn);

    }



    /**
     * @brief TidalMyPlaylistEdit::slot_clickedAllSelected [SLOT] 전체선택 체크박스 클릭시
     */
    void TidalMyPlaylistEdit::slot_clickedAllSelected(){

        this->isAllSelected = !this->isAllSelected;
        if(this->isAllSelected==true){
            for(int i=0; i<this->list_item_track->count(); i++){
                QListWidgetItem *item = this->listWidget_track->item(i);
                editDelegate::ItemEditData data_item = AbstractEditListDelegate::convertData_itemEditData(item->data(Qt::UserRole).toJsonObject());
                data_item.flagChecked = true;
                item->setData(Qt::UserRole, AbstractEditListDelegate::getObjectJson_itemEditData(data_item));
            }
        }else{
            for(int i=0; i<this->list_item_track->count(); i++){
                QListWidgetItem *item = listWidget_track->item(i);
                editDelegate::ItemEditData data_item = AbstractEditListDelegate::convertData_itemEditData(item->data(Qt::UserRole).toJsonObject());
                data_item.flagChecked = false;
                item->setData(Qt::UserRole, AbstractEditListDelegate::getObjectJson_itemEditData(data_item));
            }
        }
    }


    /**
     * @brief TidalMyPlaylistEdit::slot_clickedDelete
     */
    void TidalMyPlaylistEdit::slot_clickedDelete(){

        // 1> 삭제 체크한 item 들의 index를 찾는다.
        QList<QString> list_index;                  // QString 으로 받음
        for(int i=0; i<this->list_item_track->count(); i++){
            QListWidgetItem *item = this->listWidget_track->item(i);
            editDelegate::ItemEditData data_item = AbstractEditListDelegate::convertData_itemEditData(item->data(Qt::UserRole).toJsonObject());
            if(data_item.flagChecked){
                list_index.append(QString("%1").arg(i));
            }
        }

        if(list_index.count() > 0){
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_delete_items_in_myPlaylist, this, &TidalMyPlaylistEdit::slot_completeReq_delete_items_in_myPlaylist);
            proc->request_tidal_delete_items_in_myPlaylist(this->data_playlist.uuid, list_index);
        }

    }



    /**
     * @brief TidalMyPlaylistEdit::slot_editDelegate_clickedItemTrack_inList
     * @param index
     * @param clickMode
     */
    void TidalMyPlaylistEdit::slot_editDelegate_clickedItemTrack_inList(const int index, const AbstractEditListDelegate::ClickMode clickMode){

        QListWidgetItem *item = this->listWidget_track->item(index);
        editDelegate::ItemEditData data_item = AbstractEditListDelegate::convertData_itemEditData(item->data(Qt::UserRole).toJsonObject());

        if(clickMode == AbstractEditListDelegate::ClickMode::CheckBox){
            data_item.flagChecked = !data_item.flagChecked;
            item->setData(Qt::UserRole, AbstractEditListDelegate::getObjectJson_itemEditData(data_item));

            int cnt_checked = 0;
            for(int i=0; i<this->list_item_track->count(); i++){
                QListWidgetItem *item = this->listWidget_track->item(i);
                editDelegate::ItemEditData tmp_data = AbstractEditListDelegate::convertData_itemEditData(item->data(Qt::UserRole).toJsonObject());
                if(tmp_data.flagChecked){
                    cnt_checked += 1;
                }
            }

            // 모드 체크되었으면 전체선택 체크박스UI와 동기화
            if(cnt_checked>0 && cnt_checked==this->list_item_track->count()){
                this->isAllSelected = true;
                this->checkbox_allSelected->setChecked(true);
            }else{
                this->isAllSelected = false;
                this->checkbox_allSelected->setChecked(false);
            }

        }
        else if(clickMode == AbstractEditListDelegate::ClickMode::TrashBtn){
            // 개별 삭제 처리
            QList<QString> list_index;
            list_index.append(QString("%1").arg(index));

            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_delete_items_in_myPlaylist, this, &TidalMyPlaylistEdit::slot_completeReq_delete_items_in_myPlaylist);
            proc->request_tidal_delete_items_in_myPlaylist(this->data_playlist.uuid, list_index);
        }

    }


    /**
     * @brief TidalMyPlaylistEdit::slot_completeReq_delete_items_in_myPlaylist
     */
    void TidalMyPlaylistEdit::slot_completeReq_delete_items_in_myPlaylist(){

        this->listWidget_track->clear();
        this->list_track->clear();
        this->list_item_track->clear();

        this->isAllSelected = false;
        this->checkbox_allSelected->setChecked(false);

        this->flagReqMore_track = false;
        this->flag_lastPage_track = false;
        this->request_more_trackData();

    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void TidalMyPlaylistEdit::request_more_trackData(){
        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // next_offset
            int next_offset = 0;

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this);
            QObject::connect(proc, &ProcCommon::completeReq_list_items_of_playlist, this, &TidalMyPlaylistEdit::slot_applyResult_items_of_playlist);
            proc->request_tidal_getList_items_of_playlist(this->data_playlist.uuid, this->data_playlist.numberOfTracks, next_offset);

        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot]
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief playlist 내의 트랙 아이템들 반환 데이터를 처리함
     * @param list_track
     * @param jsonArr_dataToPlay
     */
    void TidalMyPlaylistEdit::slot_applyResult_items_of_playlist(const QList<tidal::TrackItemData> &list_track, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(jsonArr_dataToPlay);
        this->flag_lastPage_track = flag_lastPage;
        this->flagReqMore_track = false;

        int start_index = this->list_track->size();

        this->list_track->append(list_track);

        // convert trackItem --> editItem
        QList<editDelegate::ItemEditData> list_editItem;
        for(int i=0 ; i<list_track.count() ; i++){
            list_editItem.append(ConvertData::convertEditItem_fromTrackData(list_track.at(i)));
        }
        this->list_item_track->append(list_editItem);

        this->createList_itemEditDelegate_applyingWithData_showImage(list_editItem, this->listWidget_track, start_index, true);
    }


    /**
     * @brief [슬롯] QListWidget에서 썸네일 다운로드 완료
     */
    void TidalMyPlaylistEdit::slot_editDelegate_thumbnailDownloaded_fotListWidget(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemEditDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_item_track);
    }


    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void TidalMyPlaylistEdit::slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode clickMode){
        this->proc_clicked_imageDetailInfo_fromPlaylist(this->data_playlist, QJsonArray(), SECTION_FOR_MORE_POPUP___playlists, clickMode);
    }



}
