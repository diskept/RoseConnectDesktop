#include "qobuz/Dialog_ChoosePlaylist_forQobuz.h"
#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "common/gscommon.h"

#include <QLabel>
#include <QPixmapCache>

namespace qobuz {

    const int IMAGE_SIZE_OF_PLAYLIST = 60;

    /**
     * @brief Dialog_ChoosePlaylist_forQobuz::Dialog_ChoosePlaylist_forQobuz
     * @param parent
     */
    Dialog_ChoosePlaylist_forQobuz::Dialog_ChoosePlaylist_forQobuz(QWidget *parent) : QDialog(parent) {
        // data
        this->list_playlist = new QList<qobuz::PlaylistItemData>();

        setInit();

        setUIControl();
        setUIControl_topTitle();
        setUIControl_body();

        this->m_content_id = "";
        this->m_content_type = ItemType_forAddCollection::None;


        // 새 플레이리스트 추가 관련해서 먼저 처리
//        qobuz::PlaylistItemData tmp_new;
//        tmp_new.title = tr("Add a new Playlist");
//        tmp_new.title = tr("새 플레이리스트 추가");
//        this->list_playlist->append(tmp_new);

//        this->add_listWidgetItem(0);
    }


    /**
     * @brief 다이얼로그 관련 초기설정
     */
    void Dialog_ChoosePlaylist_forQobuz::setInit(){

        this->setModal(true);
        this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
//        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setFixedSize(DLG_WIDTH, DLG_HEIGHT);
        this->setStyleSheet("background-color:transparent;");

        this->setMouseTracking(true);
        this->setAutoFillBackground(false);
        this->setWindowModality(Qt::ApplicationModal);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief UI 생성 - 화면 구성을 위한 기본 UI 설정
     */
    void Dialog_ChoosePlaylist_forQobuz::setUIControl(){

        this->vBox_total = new QVBoxLayout();
        vBox_total->setMargin(0);
        vBox_total->setContentsMargins(1,1,1,1);
        vBox_total->setAlignment(Qt::AlignTop);


        QWidget *widget_total = new QWidget();
        widget_total->setLayout(vBox_total);
        widget_total->setObjectName("widget_total");
        widget_total->setStyleSheet("#widget_total {background-color:#333333;border-radius:4px;border:1px solid #4D4D4D;}");

        QVBoxLayout *vboxlayout = new QVBoxLayout();
        vboxlayout->setMargin(0);
        vboxlayout->setSpacing(0);
        vboxlayout->addWidget(widget_total);

        this->setLayout(vboxlayout);
    }



    /**
     * @brief UI 생성 - 다이얼로그의 Top Title 파트
     */
    void Dialog_ChoosePlaylist_forQobuz::setUIControl_topTitle(){

        // 타이틀
        QLabel *label_title = new QLabel;
        label_title->setText(tr("Select Playlist"));
        //label_title->setText(tr("플레이리스트에 담기"));
        label_title->setAlignment(Qt::AlignCenter);
        label_title->setStyleSheet("color:#FFFFFF; font-size:22px; background-color:transparent;");

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
        btn_close->setFixedSize(QSize(70,70));
        btn_close->setCursor(Qt::PointingHandCursor);
        connect(btn_close, &QPushButton::clicked, this, &Dialog_ChoosePlaylist_forQobuz::reject);

        QHBoxLayout *hBox = new QHBoxLayout;
        hBox->setContentsMargins(70,0,0,0);
        hBox->addWidget(label_title,1);
        hBox->addWidget(btn_close);

        QWidget *widget_topTitle = new QWidget;
        widget_topTitle->setObjectName("widget_top");
        widget_topTitle->setLayout(hBox);
        widget_topTitle->setStyleSheet("#widget_top { background-color:transparent; border-bottom:1px solid #404040; }");

        // 메인 layout에 추가
        this->vBox_total->addWidget(widget_topTitle);
    }


    /**
     * @brief UI 생성 - 다이얼로그의 바디 파트
     */
    void Dialog_ChoosePlaylist_forQobuz::setUIControl_body(){

        this->delegate = new DlgChoosePlaylistDelegate(this);
        connect(delegate, &DlgChoosePlaylistDelegate::clicked, this, &Dialog_ChoosePlaylist_forQobuz::slot_clickedItem_inList);

        this->listWidget_playlist = new QListWidget();
        this->listWidget_playlist->setContentsMargins(0,0,0,0);
        this->listWidget_playlist->setItemDelegate(delegate);
        this->listWidget_playlist->setCursor(Qt::PointingHandCursor);
        this->listWidget_playlist->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->listWidget_playlist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->listWidget_playlist->setStyleSheet("border:none;");
        this->listWidget_playlist->setFixedHeight(DLG_HEIGHT - 70);

        this->vBox_total->addWidget(this->listWidget_playlist, 1);
        this->vBox_total->addSpacing(10);

        qobuz::PlaylistItemData tmp_list_playlist;
        tmp_list_playlist.title = tr("Add new playlist");
        this->list_playlist->append(tmp_list_playlist);

        QListWidgetItem *tmp_item = new QListWidgetItem();
        tmp_item->setData(Qt::UserRole, ConvertData::getObjectJson_playlistData(this->list_playlist->at(0)));

        QPixmap pixmap_new(":/images/playlist_add_ico.png");
        tmp_item->setData(Qt::DecorationRole, pixmap_new);
        this->listWidget_playlist->addItem(tmp_item);
    }


    /**
     * @brief Dialog_ChoosePlaylist_forQobuz::showEvent
     */
    void Dialog_ChoosePlaylist_forQobuz::showEvent(QShowEvent *){

        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_list_myCreatedPlaylists, this, &Dialog_ChoosePlaylist_forQobuz::slot_applyResult_myCreatedPlaylists_forAddCollection);
        proc->request_qobuz_getList_myCreatedPlaylists(ProcCommon::DataOrderOption::DATE, ProcCommon::DataOrderDirection::DESC, 99, 0);
    }


    /**
     * @brief Dialog_ChoosePlaylist_forQobuz::slot_clickedItem_inList
     * @param index
     */
    void Dialog_ChoosePlaylist_forQobuz::slot_clickedItem_inList(const int index){

        /*if(this->list_playlist->at(index).id.isEmpty()){
            // 신규 플레이리스트 추가 필요함
        }
        else{
            // 선택한 플레이리스트로 담기 처리 진행함 - 여기에서 Proc 진행하면, 다이얼로그가 오래 화면에 남게됨. (처리 동안)
            if(this->m_content_type == ItemType_forAddCollection::Track){
                // Track 에 대해서 담기 진행
                emit this->signal_choosePlaylist_forTrack(this->list_playlist->at(index).id, this->m_content_id.toInt());
            }
            else if(this->m_content_type == ItemType_forAddCollection::Album){
                // Album 에 대해서 담기 진행
                emit this->signal_choosePlaylist_forAlbum(this->list_playlist->at(index).id, this->m_content_id.toInt());
            }
            else if(this->m_content_type == ItemType_forAddCollection::Playlist){
                // Playlist 에 대해서 담기 진행
                emit this->signal_choosePlaylist_forPlaylist(this->list_playlist->at(index).id, this->m_content_id.toString());
            }

            this->close();
        }*/

        if(index == 0){
            /*Dialog::Dialog_NewPlaylist *dlg_newPlaylist = new Dialog::Dialog_NewPlaylist(this);

            int tmp_result = dlg_newPlaylist->exec();

            if(tmp_result == QDialog::Accepted){
                //emit signal_newPlaylistNameEdited(dlg_newPlaylist->getName());
                emit this->signal_choosePlaylist_forQOBUZ(0, dlg_newPlaylist->getName());

                delete dlg_newPlaylist;

                this->accept();
            }
            else{
                dlg_newPlaylist->hide();

                delete dlg_newPlaylist;
            }*/

            Dialog::Dialog_NewPlaylist_OnRose *dlg_newPlaylist = new Dialog::Dialog_NewPlaylist_OnRose(this);
            int tmp_result = dlg_newPlaylist->exec();

            if(tmp_result == QDialog::Accepted){
                emit this->signal_choosePlaylist_forQOBUZ(0, dlg_newPlaylist->getTitle(), dlg_newPlaylist->getDestription());

                delete dlg_newPlaylist;

                this->accept();
            }
            else{
                dlg_newPlaylist->hide();

                delete dlg_newPlaylist;
            }
        }
        else{
            emit this->signal_choosePlaylist_forQOBUZ(this->list_playlist->at(index).id, this->list_playlist->at(index).title, this->list_playlist->at(index).description);

            this->accept();
        }
    }


    /**
     * @brief [Slot] 사용자 playlist 목록 응답 처리
     */
    void Dialog_ChoosePlaylist_forQobuz::slot_applyResult_myCreatedPlaylists_forAddCollection(const QList<qobuz::PlaylistItemData>& p_list_playlist, const QJsonArray&, const bool, const QJsonObject&){

        int start_index = this->list_playlist->count();

        this->list_playlist->append(p_list_playlist);
        this->add_listWidgetItem(start_index);
    }


    /**
     * @brief Dialog_ChoosePlaylist_forQobuz::slot_completeItems
     */
    void Dialog_ChoosePlaylist_forQobuz::slot_completeItems(){

        this->accept();
    }


    /**
     * @brief Dialog_ChoosePlaylist_forQobuz::add_listWidgetItem
     * @param start_index
     */
    void Dialog_ChoosePlaylist_forQobuz::add_listWidgetItem(int start_index){

        QPixmap pixmap_empty(":/images/playlist-def.png");
        QPixmap pixmap_new(":/images/playlist_add_ico.png");

        // listWidget -> item 세팅
        for(int i = start_index; i < this->list_playlist->count(); i++){

            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, ConvertData::getObjectJson_playlistData(this->list_playlist->at(i)));

            QPixmap pixmap;
            QString tmp_imgPath = this->list_playlist->at(i).image;
            if(tmp_imgPath.isEmpty()){
                if(this->list_playlist->at(i).id.isEmpty()){
                    tmp_item->setData(Qt::DecorationRole, pixmap_new);
                }
                else{
                    tmp_item->setData(Qt::DecorationRole, pixmap_empty);
                }
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_empty);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                fileDownloader->setProperty("index", i);
                connect(fileDownloader, &FileDownloader::downloaded, this, &Dialog_ChoosePlaylist_forQobuz::slot_thumbnailDownloaded_fotListWidget_playlist);
                fileDownloader->setImageURL(tmp_imgPath);
            }

            this->listWidget_playlist->addItem(tmp_item);

        }
    }


    /**
    * @brief Dialog_ChoosePlaylist_forQobuz::set_contentData
    * @param content_id
    * @param content_type
    */
    void Dialog_ChoosePlaylist_forQobuz::set_contentData(QVariant content_id, ItemType_forAddCollection content_type){

        this->m_content_id = content_id;
        this->m_content_type = content_type;
    }


    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void Dialog_ChoosePlaylist_forQobuz::slot_thumbnailDownloaded_fotListWidget_playlist(){

        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());

        if(fileDownloader_sender){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader_sender->downloadedData());
            int tmp_index = fileDownloader_sender->property("index").toInt();

            if(tmp_index>=0 && tmp_index<this->list_playlist->count()){
                if(flagLoaded){
                    QPixmap tmp_pixmap;
                    tmp_pixmap = tmp_pixmap.fromImage(image);
                    tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_PLAYLIST, IMAGE_SIZE_OF_PLAYLIST, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    this->listWidget_playlist->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                    QPixmapCache::insert(this->list_playlist->at(tmp_index).image, tmp_pixmap);
                }
            }
        }

        fileDownloader_sender->deleteLater();
    }

}
