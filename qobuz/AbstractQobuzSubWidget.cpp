#include "qobuz/AbstractQobuzSubWidget.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ItemAlbum_forQobuz.h"
#include "qobuz/ItemArtist_forQobuz.h"
#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/ItemVideo_forQobuz.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "ProcCommon_forQobuz.h"
#include "ProcRosePlay_withQobuz.h"

#include "widget/toastmsg.h"
#include "widget/NoData_Widget.h"

#include <QPixmapCache>
#include <QSettings>
#include "roseHome/ProcCommon_forRosehome.h"//c220824_4

namespace qobuz {

    /**
     * @brief Qobuz 서브페이지 위젯을 위한 부모 클래스
     * @param parent
     */
    AbstractQobuzSubWidget::AbstractQobuzSubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent) {

        // to check memory address number
        linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_Artist_movePage(QJsonObject)), SLOT(slot_dlg_signal_artist(QJsonObject)));
    }

    /**
     * @brief 소멸자 체크.
     */
    AbstractQobuzSubWidget::~AbstractQobuzSubWidget(){

        // to check memory address number
        this->deleteLater();
    }


    /**
     * @brief Qobuz 로그인이 필요하다는 화면 Widget을 반환한다. box_layout에 넣지는 않음
     * @return
     */
    QWidget* AbstractQobuzSubWidget::get_createUIControl_notLoginView(){

        QLabel *label_topComment = new QLabel(tr("You can use it by logging in to your Qobuz account."));
        //QLabel *label_topComment = new QLabel(tr("Qobuz 계정을 로그인하시면 이용하실 수 있습니다."));
        label_topComment->setStyleSheet("font-size: 18px; color: #ffffff;");
        label_topComment->setContentsMargins(0,0,0,0);
        label_topComment->setAlignment(Qt::AlignCenter);

        QPushButton *btn_login = new QPushButton();
        btn_login->setText(tr("Log-in"));
        //btn_login->setText(tr("로그인"));
        btn_login->setFixedHeight(40);
        btn_login->setStyleSheet("font-size:18px; color:#FFFFFF; background-color:#B18658; border-radius:20px; padding-left:44px; padding-right:44px;");
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setContentsMargins(0,0,0,0);        
        connect(btn_login, &QPushButton::clicked, this, &AbstractQobuzSubWidget::slot_clickBtnLogin_toShowDialog);

        // 라벨과 버튼을 세로로 배치
        QVBoxLayout *vBox = new QVBoxLayout();
        vBox->setSpacing(0);
        vBox->setContentsMargins(0,0,0,0);
        vBox->addWidget(label_topComment);
        vBox->addSpacing(30);
        vBox->addWidget(btn_login, 0, Qt::AlignCenter);

        vBox->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);

        // Widget
        QWidget *widget_output = new QWidget();
        widget_output->setContentsMargins(0,0,0,0);
        widget_output->setLayout(vBox);


        // 화면에 보이도록 설정
//        GSCommon::clearLayout(this->box_contents);
//        this->box_contents->addWidget(widget_output, 1);
//        this->box_contents->setAlignment(Qt::AlignCenter);


        // return
        return widget_output;

    }


    /**
     * @brief "로그인" 버튼 클릭 처리 - 로그인 다이얼로그 띄우기
     */
    void AbstractQobuzSubWidget::slot_clickBtnLogin_toShowDialog(){

        // Qobuz Login system pendin... change source code Added 03.02.2021 by Jeon
        this->dlbLogin = new DialogLogin_forQobuz(this);
        connect(this->dlbLogin, &DialogLogin_forQobuz::successLogin, this, &AbstractQobuzSubWidget::slot_acceptedDialogLogin);
        this->dlbLogin->exec();
    }


    /**
     * @brief Qobuz 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    void AbstractQobuzSubWidget::slot_acceptedDialogLogin(){

        ToastMsg::show(this, "", tr("QOBUZ service login was successful."));
        //ToastMsg::show(this, "", tr("QOBUZ 서비스 로그인에 성공했습니다."));
    }


    void AbstractQobuzSubWidget::slot_acceptedDialogLogout(){

        ToastMsg::show(this, "", tr("QOBUZ service logout was successful."));
    }


    /**
     * @brief Qobuz 로그인 여부 반환
     * @return
     */
    bool AbstractQobuzSubWidget::is_qobuz_logined(){
        return global.user_forQobuz.isLogined();
    }


    /**
     * @brief 화면에 로그인이 필요하다고 알려줌
     */
    void AbstractQobuzSubWidget::showNeededLoginQobuz(){
        this->showToastMsg(tr("     QOBUZ account login required.     "));
        //this->showToastMsg(tr("      QOBUZ 계정 로그인이 필요합니다.      "));
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : list_data 들을 토대로 Item (AbstractItem 상속받은) 객체들을 생성하여 Layout에 배치한다.
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief AlbumItemData 데이터로 ItemAlbum 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param seciton
     */
    void AbstractQobuzSubWidget::createList_itemAlbum_applyingWithData(const QList<qobuz::AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i = 0; i < list_data.size(); i++){
            qobuz::ItemAlbum_qobuz *tmp_widget = new qobuz::ItemAlbum_qobuz(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &qobuz::ItemAlbum_qobuz::signal_clicked, this, &AbstractQobuzSubWidget::slot_clickedItemAlbum);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief PlaylistItemData 데이터로 ItemPlaylist 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractQobuzSubWidget::createList_itemPlaylsit_applyingWithData(const QList<qobuz::PlaylistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            qobuz::ItemPlaylist_qobuz *tmp_widget = new qobuz::ItemPlaylist_qobuz(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &AbstractQobuzSubWidget::slot_clickedItemPlaylist);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief ArtistItemData 데이터로 ItemArtist 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractQobuzSubWidget::createList_itemArtist_applyingWithData(const QList<qobuz::ArtistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            qobuz::ItemArtist_qobuz *tmp_widget = new qobuz::ItemArtist_qobuz(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &qobuz::ItemArtist_qobuz::signal_clicked, this, &AbstractQobuzSubWidget::slot_clickedItemArtist);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }

    }


    /**
     * @brief VideoItemData 데이터로 ItemVideo 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractQobuzSubWidget::createList_itemVideo_applyingWithData(const QList<qobuz::VideoItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i=0 ; i<list_data.size() ; i++){
            qobuz::ItemVideo_qobuz *tmp_widget = new qobuz::ItemVideo_qobuz(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &qobuz::ItemVideo_qobuz::signal_clicked, this, &AbstractQobuzSubWidget::slot_clickedItemVideo);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Video_NoData);
            noData_widget->setFixedHeight(60);
            p_layout->addWidget(noData_widget);
        }
    }




    /**
     * @brief TrackItemData 데이터로 ListWidgetItem 생성하여 ListWidget에 추가한다. 이미지 없음. (slot 연결도 없음)
     * @param list_data
     * @param p_listWidget
     * @param flagAdjustHeight
     */
    void AbstractQobuzSubWidget::createList_itemTrackDelegate_applyingWithData(const QList<qobuz::TrackItemData> &list_data, QListWidget *p_listWidget, bool flagAdjustHeight){
        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *item = new QListWidgetItem();
            item->setData(Qt::UserRole, ConvertData::getObjectJson_trackData(list_data.at(i)));
            p_listWidget->addItem(item);
        }

        if(list_data.length() > 0){
            p_listWidget->setVisible(true);
            // listWidget height 조절
            if(flagAdjustHeight){
                p_listWidget->setFixedHeight(70 * list_data.length());         // 고정값 설정
            }
        }
        else{
            p_listWidget->setVisible(false);
        }
    }


    /**
     * @brief TrackItemData 데이터로 ListWidgetItem 생성하여 ListWidget에 추가한다. 이미지 처리가 포함됨. (slot 연결이 있음)
     * @param list_data
     * @param p_listWidget
     * @param flagAdjustHeight 기본값은 false. list_data 개수에 맞춰서 ListWidget의 사이즈를 조정할지의 여부임
     */
    void AbstractQobuzSubWidget::createList_itemTrackDelegate_applyingWithData_showImage(const QList<qobuz::TrackItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight){
        // Track 기본 이미지
        QPixmap pixmap_default = this->getDefaultImage_forTrack();

        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, ConvertData::getObjectJson_trackData(list_data.at(i)));

            QPixmap pixmap;
            //QString tmp_imgPath = list_data.at(i).image;
            QString tmp_imgPath = list_data.at(i).album_image_thumbnail;
            if(tmp_imgPath.isEmpty()){
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                connect(fileDownloader, &FileDownloader::downloaded, this, &AbstractQobuzSubWidget::slot_thumbnailDownloaded_fotListWidget_track);
                fileDownloader->setProperty("section", section);
                fileDownloader->setProperty("index", i + start_index);
                fileDownloader->setImageURL(tmp_imgPath);
            }

            p_listWidget->addItem(tmp_item);
        }


        if(list_data.length() > 0){
            p_listWidget->setVisible(true);
            // listWidget height 조절
            if(flagAdjustHeight){
                p_listWidget->setFixedHeight(70 * (start_index + list_data.length()));         // 고정값 설정
            }
        }
        else{
            p_listWidget->setVisible(false);
        }
    }


    void AbstractQobuzSubWidget::createList_itemAlbumDelegate_applyingWithData_showImage(const QList<qobuz::AlbumItemData>& list_data, QListWidget *p_listWidget, const int start_index, const int section, bool flagAdjustHeight){
        // Track 기본 이미지
        QPixmap pixmap_default = this->getDefaultImage_forTrack();

        for(int i=0 ; i<list_data.size() ; i++){
            //QListWidgetItem *tmp_item = new QListWidgetItem();
            //tmp_item->setData(Qt::UserRole, ConvertData::getObjectJson_trackData(list_data.at(i)));
            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, ConvertData::getObjectJson_albumData(list_data.at(i)));

            QPixmap pixmap;
            //QString tmp_imgPath = list_data.at(i).image;
            QString tmp_imgPath = list_data.at(i).image_thumbnail;
            if(tmp_imgPath.isEmpty()){
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                connect(fileDownloader, &FileDownloader::downloaded, this, &AbstractQobuzSubWidget::slot_thumbnailDownloaded_fotListWidget_track);
                fileDownloader->setProperty("section", section);
                fileDownloader->setProperty("index", i + start_index);
                fileDownloader->setImageURL(tmp_imgPath);
            }

            p_listWidget->addItem(tmp_item);
        }


        if(list_data.length() > 0){
            p_listWidget->setVisible(true);
            // listWidget height 조절
            if(flagAdjustHeight){
                p_listWidget->setFixedHeight(70 * (start_index + list_data.length()));         // 고정값 설정
            }
        }
        else{
            p_listWidget->setVisible(false);
        }
    }



    /**
     * @brief ListWidget과 관련 - Image 다운로드가 완료되면 ListWidget에 보이도록 하기 위한 처리 (DecorationRole 로 설정)
     * @param p_listWidget
     * @param p_list_track
     */
    void AbstractQobuzSubWidget::applyImageData_itemTrackDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<qobuz::TrackItemData>* p_list_track){
        int tmp_index = fileDownload_sender->property("index").toInt();

        if(tmp_index < p_list_track->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownload_sender->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                //QPixmapCache::insert(p_list_track->at(tmp_index).image, tmp_pixmap);
                QPixmapCache::insert(p_list_track->at(tmp_index).album_image_thumbnail, tmp_pixmap);
            }
        }

        fileDownload_sender->deleteLater();
    }


    void AbstractQobuzSubWidget::applyImageData_itemAlbumDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<qobuz::AlbumItemData>* p_list_album){
        int tmp_index = fileDownload_sender->property("index").toInt();

        if(tmp_index < p_list_album->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownload_sender->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                //QPixmapCache::insert(p_list_track->at(tmp_index).image, tmp_pixmap);
                QPixmapCache::insert(p_list_album->at(tmp_index).image_thumbnail, tmp_pixmap);
            }
        }

        fileDownload_sender->deleteLater();
    }






    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Item (AbstractItem 상속받은) 객체의 custom click 이벤트에 대하여 처리하는 함수 모음
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // Item 클릭 관련 처리 of Playlist ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemPlaylist(qobuz::PlaylistItemData &data_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Playlist 바로 재생
                    ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byPlaylistID(data_playlist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    if(global.user_forQobuz.flag_rosehome == true){
                        global.user_forQobuz.rosehome_obj = QJsonObject();
                        global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_DETAIL);
                        QJsonObject tmp_data = ConvertData::getObjectJson_playlistData(data_playlist);
                        global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
                    }
                    else{
                        /*if(data_playlist.flagCreatedByUser){
                            // Playlist Detail 페이지 진입 - User Created Playlist
                            QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_COLLECTION_PLAYLIST_DETAIL);
                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }
                        else{
                            // Playlist Detail 페이지 진입 - General Playlist
                            QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
                            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_DETAIL);
                            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                        }*/

                        QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_DETAIL);
                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){

                    //---------------------------------//c220824_4


                    // OptionPopup 띄우기 필요
                    if(data_playlist.flagCreatedByUser){
                        // User Created Playlist
                        this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), index, section);
                    }
                    else{
                        // Qobuz Playlist
                        this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), index, section);
                    }
                }
            }
        }
    }


    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemPlaylist(QList<qobuz::PlaylistItemData>* list_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_playlist->length(), index);
            if(real_index >= 0){
                qobuz::PlaylistItemData data = list_playlist->at(real_index);
                this->proc_clicked_itemPlaylist(data, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of Album ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemAlbum에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemAlbum(qobuz::AlbumItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            //print_qobuz_func();
            //qDebug() << "2. [QOBUZ][SEQ][FUNC] : AbstractQobuzSubWidget.cpp -> AbstractQobuzSubWidget::proc_clicked_itemAlbum(qobuz::AlbumItemData& data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section)";
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                    // Album 바로 재생
                    //qDebug() << "[QOBUZ][SEQ][MSG] album play clicked -> goto Next step\n";
                    ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    // Album Detail 페이지 진입
                    //qDebug() << "[QOBUZ][SEQ][MSG] album detail info clicked -> goto Next step\n";                    
                    if(global.user_forQobuz.flag_rosehome == true){
                        global.user_forQobuz.rosehome_obj = QJsonObject();
                        global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
                        QJsonObject tmp_data = ConvertData::getObjectJson_albumData(data_album);
                        global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                        emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
                    }
                    else{
                        QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(data_album);
                        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
                        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                    }
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    print_debug();



                    // OptionPopup 띄우기 필요
                    //qDebug() << "[QOBUZ][SEQ][MSG] popup menu display -> goto Next step\n";
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Album, ConvertData::getConvertOptHeaderData(data_album), index, section);
                }
            }
        }
    }


    /**
     * @brief ItemAlbum에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemAlbum(QList<qobuz::AlbumItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // Qobuz 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_album->length(), index);

            //print_qobuz_func();
            //qDebug() << "1. [QOBUZ][SEQ][FUNC] : AbstractQobuzSubWidget.cpp -> AbstractQobuzSubWidget::proc_clicked_itemAlbum(QList<qobuz::AlbumItemData>* list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section)";
            //qDebug() << "[QOBUZ][SEQ][MSG] clicked index : " << real_index;

            if(real_index >= 0){
                //qDebug() << "[QOBUZ][SEQ][MSG] index is true -> goto Next step\n";
                qobuz::AlbumItemData data = list_album->at(real_index);                
                this->proc_clicked_itemAlbum(data, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of Artist ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemArtist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemArtist(qobuz::ArtistItemData &data_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                    // Artist Detail 페이지 진입
                    QJsonObject jsonObj_move = ConvertData::getObjectJson_artistData(data_artist);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
                    emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    // OptionPopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Artist, ConvertData::getConvertOptHeaderData(data_artist), index, section);
                }
            }
        }
    }


    /**
     * @brief ItemArtist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemArtist(QList<qobuz::ArtistItemData>* list_artist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                qobuz::ArtistItemData data = list_artist->at(real_index);
                this->proc_clicked_itemArtist(data, clickMode, real_index, section);
            }
        }
    }


    // Item 클릭 관련 처리 of Video ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemVideo에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_video
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemVideo(qobuz::VideoItemData &data_video, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn
                        || clickMode == tidal::AbstractItem::ClickMode::AllBox
                ){
                    // Video 바로 재생 - OptMorePopup의 Play_RightNow와 같음
                    ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_video(data_video.jsonObj_toPlay, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                    print_debug();
                // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Video, ConvertData::getConvertOptHeaderData(data_video), index, section);
                }
            }
        }
    }


    /**
     * @brief ItemVideo에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_artist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemVideo(QList<qobuz::VideoItemData>* list_video, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_video->length(), index);
            if(real_index >= 0){
                qobuz::VideoItemData data = list_video->at(real_index);
                this->proc_clicked_itemVideo(data, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of Track (ListWidget with Delegate) ---------------------------------------------------------------------------------------------
    //      AbstractItem 상속 아님.  - AbstractQobuzTrackDelegate 상속 구조 참고

    /**
     * @brief AbstractQobuzTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemTrack_inList(qobuz::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll,const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
                    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                    int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 0:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 1:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 2:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 3:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 4:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 5:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 6:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }*/

                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, playType);
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);

                    /*QJsonObject json = QJsonObject();
                    json.insert("track", jsonArr_toPlayAll.at(index).toObject());

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);                    
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                    dialog_playlist->exec();*/
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::MoreBtn){
                    print_debug();
                // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
                }
            }
        }
    }


    /**
     * @brief AbstractQobuzTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함 [overloading]
     * @param list_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemTrack_inList(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                qobuz::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArr_toPlayAll, clickMode, real_index, section);
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_itemTrack_inList(qobuz::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll,const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
                    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                    int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 0:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 1:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 2:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 3:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 4:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 5:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 6:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }*/

                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, playType);
                }
                else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);

                    QJsonArray tracks = QJsonArray();
                    tracks.append(jsonArr_toPlayAll.at(index).toObject());

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);                    
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::MoreBtn){



                    print_debug();
                // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
                }
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_itemTrack_inList(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const AlbumTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                qobuz::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArr_toPlayAll, clickMode, real_index, section);
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_itemTrack_inList(qobuz::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Track 바로 재생 - OptMorePopup의 Play_RightNow와 같음. index에 따라서 reorder 처리 필요함.
                    /*QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
                    int curr_clickMode = settings->value(rosesettings.SETTINGS_TRACK_OPTION).toInt();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 0:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 1:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 2:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 3:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 4:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 5:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 6:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }*/

                    int curr_clickMode = global.device.getMusicPlayType();

                    OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    switch(curr_clickMode)
                    {
                        case 13:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                            break;
                        case 12:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                            break;
                        case 15:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                            break;
                        case 16:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                            break;
                        case 17:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                            break;
                        case 18:
                            playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                            break;
                        case 19:
                            playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                            break;
                    }

                    ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byTracks(jsonArray_toPlayAll, index, playType);
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);

                    QJsonArray tracks = QJsonArray();
                    tracks.append(jsonArray_toPlayAll.at(index).toObject());

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){



                    print_debug();
                // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
                }
            }
        }
    }

    //c220818 share link
    void AbstractQobuzSubWidget::slot_applyResult_getShareLink(const QString &link){

        this->shareLink = link;
        print_debug();
        qDebug() << "this->shareLink=" << this->shareLink;
    }

    void AbstractQobuzSubWidget::proc_clicked_itemTrack_inList(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                qobuz::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_itemTrack_inList(data, jsonArray_toPlayAll, clickMode, real_index, section);
            }
        }
    }



    // Item 클릭 관련 처리 of Track (ListWidget with Delegate) ---------------------------------------------------------------------------------------------
    //      AbstractItem 상속 아님.  - AbstractQobuzTrackDelegate 상속 구조 참고

    /**
     * @brief AbstractQobuzTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemAlbum_inList(qobuz::AlbumItemData& data_album, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::AllBox){
                    // Album Detail 페이지 진입
                    QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(data_album);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
                    emit this->signal_clickedMovePage(jsonObj_move);
                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Album);

                }
                else if(clickMode == tidal::AbstractTidalTrackDelegate::ClickMode::MoreBtn){
                    print_debug();
                    print_debug();
                // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Album, ConvertData::getConvertOptHeaderData(data_album), index, section);
                }
            }
        }
    }


    /**
     * @brief AbstractQobuzTrackDelegate 에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함 [overloading]
     * @param list_track
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractQobuzSubWidget::proc_clicked_itemAlbum_inList(QList<qobuz::AlbumItemData>* data_album, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(data_album->length(), index);
            if(real_index >= 0){
                qobuz::AlbumItemData data = data_album->at(real_index);
                this->proc_clicked_itemAlbum_inList(data, clickMode, real_index, section);
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_itemAlbum_inList(qobuz::AlbumItemData &data_album, const QJsonArray &jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            if(index >= 0){
                if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                    // Album Detail 페이지 진입
                    QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(data_album);
                    jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
                    emit this->signal_clickedMovePage(jsonObj_move);
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                    // 플레이리스트 담기 - Track
                    //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Album);

                    QJsonArray tracks = QJsonArray();
                    tracks = jsonArray_toPlayAll.at(index).toArray();

                    QJsonObject json = QJsonObject();
                    json.insert("tracks", tracks);

                    Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                    dialog_playlist->request_playlist_fetch();
                    connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                    int result = dialog_playlist->exec();

                    if(result == QDialog::Accepted){
                        delete dialog_playlist;
                    }
                }
                else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                    print_debug();
                    print_debug();
                // OptMorePopup 띄우기 필요
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Album, ConvertData::getConvertOptHeaderData(data_album), index, section);
                }
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_itemAlbum_inList(QList<qobuz::AlbumItemData>* data_album, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(data_album->length(), index);
            if(real_index >= 0){
                qobuz::AlbumItemData data = data_album->at(real_index);
                this->proc_clicked_itemAlbum_inList(data, jsonArray_toPlayAll, clickMode, real_index, section);
            }
        }
    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : OptMorePopup 객체의 signal 대하여, Data Type 별로 처리하는 함수 모음
    //
    //-----------------------------------------------------------------------------------------------------------------------


    // OptMorePopup 관련 처리 of Album ---------------------------------------------------------------------------------------------

    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromAlbum(qobuz::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
                )
        {
            // Rose Play 요청
            ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
            procRosePlay->requestPlayRose_byAlbumID(data_album.id, clickMode);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Album
            //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Album);

            /*QJsonObject json;
            Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
            dialog_playlist->request_playlist_fetch();
            dialog_playlist->exec();*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - 앨범
            ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_addFavorite_album(data_album.id);
            proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), true);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 추가 - 삭제
            ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_deleteFavorite_album(data_album.id);
            proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), false);
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_album.list_artist_id, data_album.list_artist_name);

            if(global.user_forQobuz.flag_rosehome == true){
                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
                qobuz::ArtistItemData tmp_data_artist;
                tmp_data_artist.id = data_album.list_artist_id.first();
                QJsonObject tmp_data = ConvertData::getObjectJson_artistData(tmp_data_artist);
                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                QList<int> artist_id;
                QList<QString> artist_name;

                artist_id.append(data_album.artist_id);
                artist_name.append(data_album.artist_name);

                this->movePage_artist_detailView(artist_id, artist_name);
            }

            // Artist DetailView
            /*if(data_album.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData::getObjectJson_albumData(data_album);
                tmp_artist.insert("manufacture", "qobuz");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forQobuz.dlg_isShow() == true){
                    global.user_forQobuz.dlg_set_state(false);
                }

                if(global.user_forQobuz.flag_rosehome == true){
                    global.user_forQobuz.rosehome_obj = QJsonObject();
                    global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
                    qobuz::ArtistItemData tmp_data_artist;
                    tmp_data_artist.id = data_album.list_artist_id.first();
                    QJsonObject tmp_data = ConvertData::getObjectJson_artistData(tmp_data_artist);
                    global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
                }
                else{
                    QList<int> artist_id;
                    QList<QString> artist_name;

                    artist_id.append(data_album.artist_id);
                    artist_name.append(data_album.artist_name);

                    this->movePage_artist_detailView(artist_id, artist_name);
                }
            }*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album DetailView
            if(global.user_forQobuz.flag_rosehome == true){
                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
                qobuz::AlbumItemData tmp_data_album;
                tmp_data_album.id = data_album.id;
                QJsonObject tmp_data = ConvertData::getObjectJson_albumData(tmp_data_album);
                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                this->movePage_album_detailView(data_album.id);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;

        }
    }



    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromAlbum(QList<qobuz::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode){
        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_album->length(), index);
            if(real_index >= 0){
                qobuz::AlbumItemData data = list_album->at(real_index);
                this->proc_clicked_optMorePopup_fromAlbum(data, clickMode);
            }
        }

    }



    // OptMorePopup 관련 처리 of Video ---------------------------------------------------------------------------------------------

    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromVideo(qobuz::VideoItemData& data_video, const OptMorePopup::ClickMode clickMode){


        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
            procRosePlay->requestPlayRose_video(data_video.jsonObj_toPlay, clickMode);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Video
            ProcCommon *proc = new ProcCommon(this);
            proc->request_qobuz_addFavorite_video(data_video.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Video
            ProcCommon *proc = new ProcCommon(this);
            proc->request_qobuz_deleteFavorite_video(data_video.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;

        }
    }



    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromVideo(QList<qobuz::VideoItemData>* list_video, const int index, const OptMorePopup::ClickMode clickMode){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_video->length(), index);
            if(real_index >= 0){
                qobuz::VideoItemData data = list_video->at(real_index);
                this->proc_clicked_optMorePopup_fromVideo(data, clickMode);
            }
        }

    }



    // OptMorePopup 관련 처리 of Playlist ---------------------------------------------------------------------------------------------

    /**
     * @brief PlaylistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
            procRosePlay->requestPlayRose_byPlaylistID(data_playlist, clickMode);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Playlist
            //ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_addFavorite_playlist(data_playlist.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Playlist
            //ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_deleteFavorite_playlist(data_playlist.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Playlist
            //this->showDialog_toAddMyCollection(data_playlist.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Playlist);

            /*QJsonObject json;
            Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
            dialog_playlist->request_playlist_fetch();
            dialog_playlist->exec();*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Edit){
            // 사용자가 만든 playlist에 한하여 동작하는 것임.  (OptMorePopup 생성을 구분해서 하고 있음)
            this->movePage_playlist_editOfMine(data_playlist);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete){
            // 사용자가 만든 playlist에 한하여 동작하는 것임.  (OptMorePopup 생성을 구분해서 하고 있음)

            // update UI
            ItemPositionData itemPosData;
            itemPosData.section = section;
            itemPosData.index = index;
            itemPosData.data_id = data_playlist.id;
            this->proc_preUpdate_delete_myCreatePlaylist(itemPosData);

            // request Qobuz PI
            ProcCommon *proc = new ProcCommon(this);
            proc->request_qobuz_delete_myCreatedPlaylist(data_playlist.id, QJsonObject());
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;

        }
    }



    /**
     * @brief PlaylistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromPlaylist(QList<qobuz::PlaylistItemData>* list_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_playlist->length(), index);
            if(real_index >= 0){
                qobuz::PlaylistItemData data = list_playlist->at(real_index);
                this->proc_clicked_optMorePopup_fromPlaylist(data, real_index, section, clickMode);
            }
        }
    }


    // OptMorePopup 관련 처리 of Track ---------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromTrack(qobuz::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

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
            //ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
            //procRosePlay->requestPlayRose_byTracks(jsonArr_toPlayAll, index, clickMode);

            if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
            {
                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < jsonArr_toPlayAll.size(); i++){
                    QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(i).toObject();
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, clickMode);
            }
            else{
                QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(index).toObject();
                QJsonArray tmpJsonArr = QJsonArray();
                tmpJsonArr.append(tmpJsonObj);

                // Rose Play 요청
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(tmpJsonArr, 0, clickMode);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Track
            ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_addFavorite_track(data_track.id);
            proc->request_qobuz_set_favorite("track", QString("%1").arg(data_track.id), true);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Track
            ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_deleteFavorite_track(data_track.id);
            proc->request_qobuz_set_favorite("track", QString("%1").arg(data_track.id), false);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Track
            //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);

            /*QJsonObject json;
            Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
            dialog_playlist->request_playlist_fetch();
            dialog_playlist->exec();*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_name);

            // Artist DetailView
            if(data_track.list_artist_id.count() > 1){
                QJsonObject tmp_artist = ConvertData::getObjectJson_trackData(data_track);
                tmp_artist.insert("manufacture", "qobuz");

                this->dlgArtist = new DialogArtistList(this);
                this->dlgArtist->setData(tmp_artist);
                this->dlgArtist->exec();
            }
            else{
                if(global.user_forQobuz.dlg_isShow() == true){
                    global.user_forQobuz.dlg_set_state(false);
                }

                if(global.user_forQobuz.flag_rosehome == true){
                    global.user_forQobuz.rosehome_obj = QJsonObject();
                    global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
                    qobuz::ArtistItemData tmp_data_artist;
                    tmp_data_artist.id = data_track.list_artist_id.first();
                    QJsonObject tmp_data = ConvertData::getObjectJson_artistData(tmp_data_artist);
                    global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                    emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
                }
                else{
                    this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_name);
                }
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album DetailView
            if(global.user_forQobuz.flag_rosehome == true){
                global.user_forQobuz.rosehome_obj = QJsonObject();
                global.user_forQobuz.rosehome_obj.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
                qobuz::AlbumItemData tmp_data_album;
                tmp_data_album.id = data_track.album_id;
                QJsonObject tmp_data = ConvertData::getObjectJson_albumData(tmp_data_album);
                global.user_forQobuz.rosehome_obj.insert(KEY_DATA, tmp_data);

                emit linker->signal_RoseHome_movePage(QString(GSCommon::MainMenuCode::Qobuz));
            }
            else{
                this->movePage_album_detailView(data_track.album_id);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;

        }
    }



    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromTrack(QList<qobuz::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_track->length(), index);
            if(real_index >= 0){
                qobuz::TrackItemData data = list_track->at(real_index);
                this->proc_clicked_optMorePopup_fromTrack(data, jsonArr_toPlayAll, real_index, clickMode);
            }
        }
    }




    // OptMorePopup 관련 처리 of Artist ---------------------------------------------------------------------------------------------

    /**
     * @brief ArtistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromArtist(qobuz::ArtistItemData& data_artist, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Artist
            ProcCommon *proc = new ProcCommon(this);
            proc->request_qobuz_addFavorite_artist(data_artist.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Artist
            ProcCommon *proc = new ProcCommon(this);
            proc->request_qobuz_deleteFavorite_artist(data_artist.id);
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;

        }
    }



    /**
     * @brief ArtistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_optMorePopup_fromArtist(QList<qobuz::ArtistItemData>* list_artist, const int index, const OptMorePopup::ClickMode clickMode){

        if(this->is_qobuz_logined() == false){
            this->showNeededLoginQobuz();               // QOBUZ 로그인 안된 경우
        }
        else{
            int real_index = this->checkValid_index(list_artist->length(), index);
            if(real_index >= 0){
                qobuz::ArtistItemData data = list_artist->at(real_index);
                this->proc_clicked_optMorePopup_fromArtist(data, clickMode);
            }
        }
    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromAlbum
     * @param data_album - 현재는 id만 사용하지만, 차후 확장 고려위해 AlbumItemData 을 받도록함
     * @param btnClickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromAlbum(qobuz::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_album.id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - Album                
                //print_qobuz_func();
                //qDebug() << "[QOBUZ][MSG][FUNC] : btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection";
                //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Album);

                /*QJsonObject json;
                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                dialog_playlist->exec();*/
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Album
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_album(data_album.id);
                proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), true);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Album
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_album(data_album.id);
                proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), false);
            }            
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
                print_debug();
                print_debug();
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Qobuz_Album, ConvertData::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생
                //print_qobuz_func();
                //qDebug() << "[QOBUZ][MSG][FUNC] : btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll";
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
        }
    }

    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromAlbum(qobuz::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_album.id > 0){
            if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - Album
                //print_qobuz_func();
                //qDebug() << "[QOBUZ][MSG][FUNC] : btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection";
                //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Album);

                /*QJsonObject json;
                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                dialog_playlist->exec();*/

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Album
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_album(data_album.id);
                proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), true);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Album
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_album(data_album.id);
                proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), false);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::More){
                print_debug();
                print_debug();
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Qobuz_Album, ConvertData::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayAll){
                // 전체재생
                //print_qobuz_func();
                //qDebug() << "[QOBUZ][MSG][FUNC] : btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll";
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromAlbum(qobuz::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_album.id > 0){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - Album
                //print_qobuz_func();
                //qDebug() << "[QOBUZ][MSG][FUNC] : btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection";
                //this->showDialog_toAddMyCollection(data_album.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Album);

                /*QJsonObject json;
                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                dialog_playlist->exec();*/

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Album
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_album(data_album.id);
                proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), true);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Album
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_album(data_album.id);
                proc->request_qobuz_set_favorite("album", QString("%1").arg(data_album.id), false);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                print_debug();
                // OptMorePopup 띄우기 - 즐겨찾기 관련 메뉴를 가림. DetailView에 하트가 있음
                this->makeObj_optMorePopup(OptMorePopup::Qobuz_Album, ConvertData::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                // 전체재생
                //print_qobuz_func();
                //qDebug() << "[QOBUZ][MSG][FUNC] : btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll";
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromArtist
     * @param data_artist
     * @param section
     * @param btnClickMode
     */
    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromArtist(qobuz::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_artist.id > 0){
            if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_artist(data_artist.id);
                proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), true);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_artist(data_artist.id);
                proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), false);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Qobuz_Artist, ConvertData::getConvertOptHeaderData(data_artist), 0, section);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
        }
    }

    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromArtist(qobuz::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_artist.id > 0){
            if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_artist(data_artist.id);
                proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), true);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_artist(data_artist.id);
                proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), false);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::More){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Qobuz_Artist, ConvertData::getConvertOptHeaderData(data_artist), 0, section);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
        }
    }

    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromArtist(qobuz::ArtistItemData& data_artist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){
        // 데이터 유효할 때에만
        if(data_artist.id > 0){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_artist(data_artist.id);
                proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), true);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Artiest
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_artist(data_artist.id);
                proc->request_qobuz_set_favorite("artist", QString("%1").arg(data_artist.id), false);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){
                QJsonArray tracks = QJsonArray();
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Qobuz_Artist, ConvertData::getConvertOptHeaderData(data_artist), 0, section);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                // 전체재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
        }
    }


    /**
     * @brief AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromPlaylist
     * @param data_playlist
     * @param section
     * @param btnClickMode
     */
    //void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo::BtnClickMode btnClickMode){
    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_forQobuz::BtnClickMode btnClickMode){
        print_debug();
        if(data_playlist.id.isEmpty() == false){
            //if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
            if(btnClickMode == AbstractImageDetailInfo_forQobuz::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_playlist(data_playlist.id);
                proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), true);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
            else if(btnClickMode == AbstractImageDetailInfo_forQobuz::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_playlist(data_playlist.id);
                proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), false);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
            else if(btnClickMode == AbstractImageDetailInfo_forQobuz::BtnClickMode::More){
                print_debug();
                print_debug();
                // OptMorePopup 띄우기
                if(data_playlist.flagCreatedByUser){
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
                else{
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
            else if(btnClickMode == AbstractImageDetailInfo_forQobuz::BtnClickMode::PlayAll){
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
            else if(btnClickMode == AbstractImageDetailInfo_forQobuz::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection){
            else if(btnClickMode == AbstractImageDetailInfo_forQobuz::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
                //this->showDialog_toAddMyCollection(data_playlist.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Playlist);

                /*QJsonObject json;
                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                dialog_playlist->exec();*/
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode){
        if(data_playlist.id.isEmpty() == false){
            //if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
            if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_playlist(data_playlist.id);
                proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), true);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_playlist(data_playlist.id);
                proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), false);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::More){
                print_debug();
                print_debug();
                // OptMorePopup 띄우기
                if(data_playlist.flagCreatedByUser){
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
                else{
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayAll){
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection){
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
                //this->showDialog_toAddMyCollection(data_playlist.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Playlist);

                /*QJsonObject json;
                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                dialog_playlist->exec();*/

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
        }
    }


    void AbstractQobuzSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(qobuz::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailContents_RHV::BtnClickMode btnClickMode){
        if(data_playlist.id.isEmpty() == false){
            //if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toAdd){
            if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_addFavorite_playlist(data_playlist.id);
                proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), true);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::Favorite_toDelete){
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
                ProcCommon *proc = new ProcCommon(this);
                //proc->request_qobuz_deleteFavorite_playlist(data_playlist.id);
                proc->request_qobuz_set_favorite("playlist", QString("%1").arg(data_playlist.id), false);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::More){
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){
                print_debug();
                print_debug();
                // OptMorePopup 띄우기
                if(data_playlist.flagCreatedByUser){
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist_Created, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
                else{
                    this->makeObj_optMorePopup(OptMorePopup::Qobuz_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
                }
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayAll){
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::JustPlay);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::PlayShuffle){
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withQobuz *procRosePlay = new ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byTracks(p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withQobuz::PlayShuffleMode::ShufflePlay);
            }
            //else if(btnClickMode == AbstractImageDetailInfo::BtnClickMode::AddCollection){
            else if(btnClickMode == AbstractImageDetailContents_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
                //this->showDialog_toAddMyCollection(data_playlist.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Playlist);

                /*QJsonObject json;
                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                dialog_playlist->exec();*/

                QJsonArray tracks = QJsonArray();
                //tracks.append(jsonArr_toPlayAll.at(index).toObject());
                tracks = p_jsonArr_toPlay;

                QJsonObject json = QJsonObject();
                json.insert("tracks", tracks);

                Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::QOBUZ, json, this);
                dialog_playlist->request_playlist_fetch();
                connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz);
                int result = dialog_playlist->exec();

                if(result == QDialog::Accepted){
                    delete dialog_playlist;
                }
            }
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 페이지 이동의 간편화. SubPage에 대해, PageInfo struct로 페이지 이동을 처리하기 위함.
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Video All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractQobuzSubWidget::movePage_video_allView(qobuz::PageInfo_VideoAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_videoAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_VIDEO_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }

    /**
     * @brief Track All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractQobuzSubWidget::movePage_track_allView(qobuz::PageInfo_TrackAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_TRACK_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }

    /**
     * @brief Album All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractQobuzSubWidget::movePage_album_allView(qobuz::PageInfo_AlbumAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }


    void AbstractQobuzSubWidget::movePage_sameArtist_allView(qobuz::PageInfo_AlbumAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SAME_ARTIST_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }


    /**
     * @brief Playlist All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractQobuzSubWidget::movePage_playlist_allView(qobuz::PageInfo_PlaylistAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_playlistAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_PLAYLIST_ALL_LIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    void AbstractQobuzSubWidget::movePage_similarPlaylist_allView(qobuz::PageInfo_PlaylistAllView& data_pageInfo){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_playlistAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SIMILAR_PLAYLIST_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    /**
     * @brief My Playlist 편집 페이지 이동
     * @param data_playlist
     */
    void AbstractQobuzSubWidget::movePage_playlist_editOfMine(qobuz::PlaylistItemData& data_playlist){
        QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_MY_COLLECTION_PLAYLIST_EDIT);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    /**
     * @brief AbstractQobuzSubWidget::movePage_artist_detailView
     * @param artist_id
     */
    void AbstractQobuzSubWidget::movePage_artist_detailView(const QList<int>& list_artist_id, const QList<QString>& list_artist_name){
        Q_UNUSED(list_artist_name);

        if(list_artist_id.length() > 0){
            // 바로 Artist Detail 페이지 진입 (id 만 넣어서 진입)
            qobuz::ArtistItemData tmp_data_artist;
            tmp_data_artist.id = list_artist_id.first();

            QJsonObject jsonObj_move = ConvertData::getObjectJson_artistData(tmp_data_artist);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);                    // 페이지 이동 signal
        }
    }


    /**
     * @brief Album ID로, Album Detail View 페이지 이동을 위한 처리
     * @param album_id
     */
    void AbstractQobuzSubWidget::movePage_album_detailView(const QString album_id){
        if(album_id > 0){
            // Album Detail 페이지 진입 (id 만 넣어서 진입)
            qobuz::AlbumItemData tmp_data_album;
            tmp_data_album.id = album_id;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(tmp_data_album);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
            emit this->signal_clickedMovePage(jsonObj_move);                    // 페이지 이동 signal
        }
    }


    void AbstractQobuzSubWidget::slot_dlg_signal_artist(const QJsonObject &p_jsonObject){

        QList<int> list_artist_id;
        QList<QString> list_artist_name;

        list_artist_id.append(ProcJsonEasy::getInt(p_jsonObject, "artist_id"));

        this->movePage_artist_detailView(list_artist_id, list_artist_name);
        global.user_forQobuz.dlg_set_state(true);
    }








    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 사용자의 플레이리스트 담기 관련
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 플레이리스트 담기 다이얼로그 띄우기
     * @param content_id
     * @param content_type
     */
    void AbstractQobuzSubWidget::showDialog_toAddMyCollection(QVariant content_id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection content_type){
        // 플레이리스트 담기 다이얼로그 띄우기 - 선택한 옵션 함수는 하나이지만, slot은 의도적으로 구분함
        Dialog_ChoosePlaylist_forQobuz *dialog_chooslPlaylist = new Dialog_ChoosePlaylist_forQobuz(this);
        dialog_chooslPlaylist->set_contentData(content_id, content_type);
        connect(dialog_chooslPlaylist, &Dialog_ChoosePlaylist_forQobuz::signal_choosePlaylist_forTrack, this, &AbstractQobuzSubWidget::slot_add_choosePlaylist_forTrack);
        connect(dialog_chooslPlaylist, &Dialog_ChoosePlaylist_forQobuz::signal_choosePlaylist_forAlbum, this, &AbstractQobuzSubWidget::slot_add_choosePlaylist_forAlbum);
        connect(dialog_chooslPlaylist, &Dialog_ChoosePlaylist_forQobuz::signal_choosePlaylist_forPlaylist, this, &AbstractQobuzSubWidget::slot_add_choosePlaylist_forPlaylist);
        dialog_chooslPlaylist->exec();
    }

    /**
     * @brief 선택한 Track 에 대해서, 플레이리스트 담기 처리 진행
     * @param playlist_uuid_target - 선택한 플레이리스트
     * @param track_id
     */
    void AbstractQobuzSubWidget::slot_add_choosePlaylist_forTrack(QString playlist_uuid_target, int track_id){
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &AbstractQobuzSubWidget::slot_done_addPlaylist);
        proc->request_qobuz_add_track_in_myPlaylist(playlist_uuid_target, track_id);
    }

    /**
     * @brief 선택한 Album 에 대해서, 플레이리스트 담기 처리 진행
     * @param playlist_uuid_target - 선택한 플레이리스트
     * @param album_id
     */
    void AbstractQobuzSubWidget::slot_add_choosePlaylist_forAlbum(QString playlist_uuid_target, int album_id){
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &AbstractQobuzSubWidget::slot_done_addPlaylist);
        proc->request_qobuz_add_album_in_myPlaylist(playlist_uuid_target, album_id);
    }

    /**
     * @brief 선택한 Playlist 에 대해서, 플레이리스트 담기 처리 진행
     * @param playlist_uuid_target - 선택한 플레이리스트
     * @param playlist_uuid
     */
    void AbstractQobuzSubWidget::slot_add_choosePlaylist_forPlaylist(QString playlist_uuid_target, QString playlist_uuid){
        ProcCommon *proc = new ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &AbstractQobuzSubWidget::slot_done_addPlaylist);
        proc->request_qobuz_add_playlist_in_myPlaylist(playlist_uuid_target, playlist_uuid);
    }


    void AbstractQobuzSubWidget::slot_add_rosePlaylist_withQobuz(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        if(global.user_forQobuz.flag_rosehome == false){

            QJsonObject jsonObj_move = dataObj;
            jsonObj_move.insert("view_type", view_type);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ADD_PLAYLIST);

            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else{

            QJsonObject data;
            data.insert("tracks", ProcJsonEasy::getJsonArray(dataObj, "tracks"));
            data.insert("view_type", view_type);
            data.insert("type", "QOBUZ");

            QJsonObject jsonObj_move;
            jsonObj_move.insert("data", data);

            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ADD_PLAYLIST);

            emit linker->signal_clicked_movePage(jsonObj_move);
        }
    }


    /**
     * @brief 플레이리스트 담기 처리 완료
     */
    void AbstractQobuzSubWidget::slot_done_addPlaylist(){
       this->showToastMsg(tr("Added to playlist."));
       // this->showToastMsg(tr("플레이리스트에 추가되었습니다."));
    }






    // MARK : about Filter Option Box -----------------------------------------------------------------------------------------------------

    /**
     * @brief My Collection 에서 사용하는 Filter Option Data 목록 반환함
     * @return
     */
    QList<bugs::FilterOptionData> AbstractQobuzSubWidget::get_list_filterOptData_forMyCollection(){

        QList<bugs::FilterOptionData> list_output;

        bugs::FilterOptionData filterOpt_date;
        filterOpt_date.opt_code = QVariant::fromValue(ProcCommon::DataOrderOption::DATE);
        filterOpt_date.opt_name = tr("Date added");
        list_output.append(filterOpt_date);

        bugs::FilterOptionData filterOpt_name;
        filterOpt_name.opt_code = QVariant::fromValue(ProcCommon::DataOrderOption::NAME);
        filterOpt_name.opt_name = tr("A-Z");
        list_output.append(filterOpt_name);

        return list_output;

    }




    // MARK : Rose 요청 실패에 대한 공용 처리 -----------------------------------------------------------------------------------

    /**
     * @brief errorMsg를 Toast로 띄움
     * @param errorMsg
     */
    void AbstractQobuzSubWidget::slot_cannotRosePlay(const QString errorMsg){
        this->showToastMsg(errorMsg);
    }


}
