#include "roseHome/AbstractRoseHomeSubWidget.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ItemRecommends_rosehome.h"
#include "roseHome/ItemAlbum_rosehome.h"
#include "roseHome/ItemPlaylist_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "bugs/ProcRoseAPI_withBugs.h"
#include "bugs/bugs_struct.h"
#include "tidal/ProcRosePlay_withTidal.h"
#include "tidal/tidal_struct.h"
#include "qobuz/ProcRosePlay_withQobuz.h"
#include "qobuz/qobuz_struct.h"
#include "apple/ProcRosePlay_withApple.h"
#include "apple/Apple_struct.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include <QPixmapCache>
#include <QJsonDocument>
#include <QSettings>


namespace roseHome {

    /**
     * @brief Qobuz 서브페이지 위젯을 위한 부모 클래스
     * @param parent
     */
    AbstractRoseHomeSubWidget::AbstractRoseHomeSubWidget(MainUIType p_mainUIType, QWidget *parent) : common::AbstractCommonSubWidget(p_mainUIType, parent) {
        // to check memory address number
    }

    /**
     * @brief 소멸자 체크.
     */
    AbstractRoseHomeSubWidget::~AbstractRoseHomeSubWidget(){

        // to check memory address number
        this->deleteLater();
    }


    void AbstractRoseHomeSubWidget::setDataABSPage(QJsonObject p_data){//cheon211018
        this->pageCode = "";
        this->contentStep = "";

        // 데이터 세팅
        if(p_data.contains(KEY_PAGE_CODE)){
            this->pageCode = p_data[KEY_PAGE_CODE].toString();
        }
        if(p_data.contains(KEY_CONTENT_STEP)){
            this->contentStep = p_data[KEY_CONTENT_STEP].toString();
        }
    }


    void AbstractRoseHomeSubWidget::slot_changedSubTabUI(const QJsonObject &p_data){//cheon211018
        Q_UNUSED(p_data);
        // Nothing...
    }


    QJsonObject AbstractRoseHomeSubWidget::getDataJson(){//cheon211018

        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = this->pageCode;
        tmp_data[KEY_CONTENT_STEP] = this->contentStep;

        return tmp_data;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : list_data 들을 토대로 Item (AbstractItem 상속받은) 객체들을 생성하여 Layout에 배치한다.
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief RecommendsItemData 데이터로 ItemRecommends 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param seciton
     */
    void AbstractRoseHomeSubWidget::createList_itemRecommend_applyingWithData(const QList<roseHome::RecommendsItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i = 0; i < list_data.size(); i++){
            roseHome::ItemRecommends_rosehome *tmp_widget = new roseHome::ItemRecommends_rosehome(start_index + i, section, imageSizeMode);
            connect(tmp_widget, &roseHome::ItemRecommends_rosehome::signal_clicked, this, &AbstractRoseHomeSubWidget::slot_clickedItemPlaylist);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 250);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief AlbumItemData 데이터로 ItemAlbum 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param seciton
     */
    void AbstractRoseHomeSubWidget::createList_itemAlbum_applyingWithData(const QList<roseHome::AlbumItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i = 0; i < list_data.size(); i++){
            roseHome::ItemAlbum_rosehome *tmp_widget = new roseHome::ItemAlbum_rosehome(start_index + i, section, imageSizeMode, false);
            connect(tmp_widget, &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AbstractRoseHomeSubWidget::slot_clickedItemAlbum);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if( (start_index + list_data.size()) == 0 ){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 250);
            p_layout->addWidget(noData_widget);
        }
    }


    /**
     * @brief PlaylistItemData 데이터로 ItemPlaylist 객체를 생성하여 Layout 에 추가한다.
     * @param list_data
     * @param p_layout
     * @param section
     */
    void AbstractRoseHomeSubWidget::createList_itemPlaylsit_applyingWithData(const QList<roseHome::PlaylistItemData> &list_data, tidal::AbstractItem::ImageSizeMode imageSizeMode, QLayout *p_layout, const int start_index, const int section){
        for(int i = 0; i < list_data.size(); i++){
            roseHome::ItemPlaylist_rosehome *tmp_widget = new roseHome::ItemPlaylist_rosehome(start_index + i, section, imageSizeMode, section, false);
            connect(tmp_widget, &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &AbstractRoseHomeSubWidget::slot_clickedItemPlaylist);
            tmp_widget->setData(list_data.at(i));
            p_layout->addWidget(tmp_widget);
        }

        if((start_index + list_data.size()) == 0){
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 250);
            p_layout->addWidget(noData_widget);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Item (AbstractItem 상속받은) 객체의 custom click 이벤트에 대하여 처리하는 함수 모음
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // Item 클릭 관련 처리 of Album ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemAlbum에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractRoseHomeSubWidget::proc_clicked_itemAlbum(roseHome::AlbumItemData &data_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){
        if(index >= 0){
            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

                // Album 바로 재생
                if(data_album.type == "TIDAL"){
                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(data_album.type == "BUGS"){
                    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(data_album.type == "QOBUZ"){
                    qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(data_album.type == "APPLE_MUSIC"){
                    apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Album Detail 페이지 진입
                QJsonObject jsonObj_move = ConvertData::getObjectJson_albumData(data_album);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ALBUM_DETAIL);
                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, ConvertData::getConvertOptHeaderData(data_album), index, section);
            }
        }
    }


    void AbstractRoseHomeSubWidget::slot_applyResult_getShareLink(const QString &link){//c220825_2

        this->shareLink = link;
        print_debug();
        qDebug() << "this->shareLink=" << this->shareLink;
    }


    /**
     * @brief ItemAlbum에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함  [overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractRoseHomeSubWidget::proc_clicked_itemAlbum(QList<roseHome::AlbumItemData> *list_album, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        int real_index = this->checkValid_index(list_album->length(), index);

        if(real_index >= 0){
            roseHome::AlbumItemData data = list_album->at(real_index);
            this->proc_clicked_itemAlbum(data, clickMode, real_index, section);
        }
    }


    // Item 클릭 관련 처리 of Playlist ---------------------------------------------------------------------------------------------

    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractRoseHomeSubWidget::proc_clicked_itemPlaylist(roseHome::PlaylistItemData &data_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        if(index >= 0){

            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

                // Playlist 바로 재생
                if(data_playlist.type == "TIDAL" && data_playlist.isRose == false){
                    tidal::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.uuid = data_playlist.clientKey;

                    if(data_playlist.title.contains("Mix")){
                        tmpPlaylist.description = "mixes";
                    }

                    tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(data_playlist.type == "BUGS" && data_playlist.isRose == false){
                    bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                    procRosePlay->requestPlayRose_byPD_AlbumID(data_playlist.clientKey.toInt(), OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(data_playlist.type == "QOBUZ" && data_playlist.isRose == false){
                    qobuz::PlaylistItemData tmpPlaylist;
                    tmpPlaylist.id = data_playlist.clientKey;
                    tmpPlaylist.tracks_count = 100;
                    qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                    procRosePlay->requestPlayRose_byPlaylistID(tmpPlaylist, OptMorePopup::ClickMode::Play_RightNow);
                }
                else if(data_playlist.type == "APPLE_MUSIC" && data_playlist.isRose == false){
                    apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                    procRosePlay->requestPlayRose_byPlaylistID(data_playlist.clientKey, OptMorePopup::ClickMode::Play_RightNow);
                }
                else{
                    ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                    procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
                }
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입 - General Playlist
                QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);

                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_PLAYLIST_DETAIL);
                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                //c220825_2  이하 10줄

                // OptionPopup 띄우기 필요
                print_debug();
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), index, section);
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
    void AbstractRoseHomeSubWidget::proc_clicked_itemPlaylist(QList<roseHome::PlaylistItemData> *list_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        int real_index = this->checkValid_index(list_playlist->length(), index);

        if(real_index >= 0){
            //print_debug();
            //qDebug()<< "real_index = " << real_index;
            roseHome::PlaylistItemData data = list_playlist->at(real_index);
            this->proc_clicked_itemPlaylist(data, clickMode, real_index, section);
        }
    }


    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractRoseHomeSubWidget::proc_clicked_itemPlaylist_rose(roseHome::PlaylistItemData &data_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        if(index >= 0){
            if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){
                // Playlist Detail 페이지 진입 - General Playlist
                QJsonObject jsonObj_move = ConvertData::getObjectJson_playlistData(data_playlist);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_PLAYLIST_ROSE_DETAIL);
                emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptionPopup 띄우기 필요
                //c220825_2  이하 10줄
                print_debug();

                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), index, section);
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
    void AbstractRoseHomeSubWidget::proc_clicked_itemPlaylist_rose(QList<roseHome::PlaylistItemData> *list_playlist, const tidal::AbstractItem::ClickMode clickMode, const int index, const int section){

        int real_index = this->checkValid_index(list_playlist->length(), index);

        if(real_index >= 0){
            roseHome::PlaylistItemData data = list_playlist->at(real_index);
            this->proc_clicked_itemPlaylist_rose(data, clickMode, real_index, section);
        }
    }


    /**
     * @brief ItemPlsylist에서 발생한 custom click 이벤트에 대한 실제 처리를 진행함
     * @param data_playlist
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractRoseHomeSubWidget::proc_clicked_itemTrack_inList(roseHome::TrackItemData& data_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        if(index >= 0){
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
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

                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, jsonArray_toPlayAll, index, playType);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                print_debug();

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Track, ConvertData::getConvertOptHeaderData(data_track), index, section);
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
    void AbstractRoseHomeSubWidget::proc_clicked_itemTrack_inList(QList<roseHome::TrackItemData>* list_track, const QJsonArray& jsonArray_toPlayAll, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode, const int index, const int section){

        int real_index = this->checkValid_index(list_track->length(), index);
        if(real_index >= 0){
            roseHome::TrackItemData data = list_track->at(real_index);
            this->proc_clicked_itemTrack_inList(data, jsonArray_toPlayAll, clickMode, real_index, section);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 DetailInfo 클릭에 대한 상세처리하기 위한 함수들
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief AbstractRoseHomeSubWidget::proc_clicked_imageDetailInfo_fromAlbum
     * @param data_album
     * @param section
     * @param btnClickMode
     */
    void AbstractRoseHomeSubWidget::proc_clicked_imageDetailInfo_fromAlbum(roseHome::AlbumItemData& data_album, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode){

        if(data_album.id > 0){
            if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::More){
                //c220825_2  이하
                print_debug();

                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, ConvertData::getConvertOptHeaderData(data_album), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayAll){
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbum(data_album, p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbum(data_album, p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withRosehome::PlayShuffleMode::ShufflePlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
            }
        }
    }


    /**
     * @brief AbstractRoseHomeSubWidget::proc_clicked_imageDetailInfo_fromPlaylist
     * @param data_playlist
     * @param section
     * @param btnClickMode
     */
    void AbstractRoseHomeSubWidget::proc_clicked_imageDetailInfo_fromPlaylist(roseHome::PlaylistItemData& data_playlist, const QJsonArray p_jsonArr_toPlay, const int section, AbstractImageDetailInfo_RHV::BtnClickMode btnClickMode){

        if(data_playlist.id > 0){
            if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toAdd){
                // 즐겨찾기 추가 - Playlist
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::Favorite_toDelete){
                // 즐겨찾기 삭제 - Playlist
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::More){
                //c220825_2  이하 10줄
                print_debug();

                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Playlist, ConvertData::getConvertOptHeaderData(data_playlist), 0, section, true);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayAll){
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylist(data_playlist, p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withRosehome::PlayShuffleMode::JustPlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::PlayShuffle){
                // 셔플재생
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylist(data_playlist, p_jsonArr_toPlay, 0, OptMorePopup::ClickMode::Play_RightNow, ProcRosePlay_withRosehome::PlayShuffleMode::ShufflePlay);
            }
            else if(btnClickMode == AbstractImageDetailInfo_RHV::BtnClickMode::AddCollection){
                // 플레이리스트 담기 - playlist용
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
     * @brief PlaylistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractRoseHomeSubWidget::proc_clicked_optMorePopup_fromAlbum(roseHome::AlbumItemData& data_album, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                if(data_album.type != "MUSIC"){//c220903_2
                    setUIShare();
                }else{
                    ToastMsg::show(this, "", tr("Music content does not provide a sharing service."), 2000);
                }
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            if(data_album.type == "TIDAL"){
                tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey.toInt(), clickMode);
            }
            else if(data_album.type == "BUGS"){
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey.toInt(), clickMode);
            }
            else if(data_album.type == "QOBUZ"){
                qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey, clickMode);
            }
            else if(data_album.type == "APPLE_MUSIC"){
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.clientKey.toInt(), clickMode);
            }
            else{
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.id, clickMode);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Playlist
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Playlist
        }
    }


    /**
     * @brief AlbumItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.  [Overloading]
     * @details 편의 제공을 위해 overloading.
     * @param list_album
     * @param index
     * @param clickMode
     */
    void AbstractRoseHomeSubWidget::proc_clicked_optMorePopup_fromAlbum(QList<roseHome::AlbumItemData>* list_album, const int index, const OptMorePopup::ClickMode clickMode){

        int real_index = this->checkValid_index(list_album->length(), index);
        if(real_index >= 0){
            roseHome::AlbumItemData data = list_album->at(real_index);
            this->proc_clicked_optMorePopup_fromAlbum(data, clickMode);
        }
    }


    // OptMorePopup 관련 처리 of Playlist ---------------------------------------------------------------------------------------------

    /**
     * @brief PlaylistItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractRoseHomeSubWidget::proc_clicked_optMorePopup_fromPlaylist(roseHome::PlaylistItemData& data_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
        )
        {
            // Rose Play 요청
            if(data_playlist.ownerName == "tidal"){
                tidal::PlaylistItemData tmpPlaylist;
                tmpPlaylist.uuid = data_playlist.clientKey;

                tidal::ProcRosePlay_withTidal *procRosePlay = new tidal::ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_byPlaylistUUID(tmpPlaylist, clickMode);
            }
            else if(data_playlist.type == "BUGS"){
                bugs::ProcRoseAPI_withBugs *procRosePlay = new bugs::ProcRoseAPI_withBugs(this);
                procRosePlay->requestPlayRose_byPD_AlbumID(data_playlist.clientKey.toInt(), clickMode);
            }
            else if(data_playlist.type == "QOBUZ"){
                qobuz::PlaylistItemData tmpPlaylist;
                tmpPlaylist.id = data_playlist.clientKey;
                qobuz::ProcRosePlay_withQobuz *procRosePlay = new qobuz::ProcRosePlay_withQobuz(this);
                procRosePlay->requestPlayRose_byPlaylistID(tmpPlaylist, clickMode);
            }
            else if(data_playlist.type == "APPLE_MUSIC"){
                apple::ProcRosePlay_withApple *procRosePlay = new apple::ProcRosePlay_withApple(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.clientKey, clickMode);
            }
            else{
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, clickMode);
            }
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
        }
        else if(clickMode == OptMorePopup::ClickMode::Edit){
            // 사용자가 만든 playlist에 한하여 동작하는 것임.  (OptMorePopup 생성을 구분해서 하고 있음)
            //this->movePage_playlist_editOfMine(data_playlist);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete){
            // 사용자가 만든 playlist에 한하여 동작하는 것임.  (OptMorePopup 생성을 구분해서 하고 있음)

            // update UI
            /*ItemPositionData itemPosData;
            itemPosData.section = section;
            itemPosData.index = index;
            itemPosData.data_id = data_playlist.id;
            this->proc_preUpdate_delete_myCreatePlaylist(itemPosData);

            // request Qobuz PI
            ProcCommon *proc = new ProcCommon(this);
            proc->request_qobuz_delete_myCreatedPlaylist(data_playlist.id, QJsonObject());*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220818
            print_debug();//c220818-3dot
            if(data_playlist.type != "MUSIC"){//c220903_2
                setUIShare();
            }
            else if(data_playlist.type == "MUSIC" && data_playlist.ownerName != global.user.getUsername()){//j220906 share link
                setUIShare();
            }
            else{
                ToastMsg::show(this, "", tr("Music content does not provide a sharing service."), 2000);
            }
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
    void AbstractRoseHomeSubWidget::proc_clicked_optMorePopup_fromPlaylist(QList<roseHome::PlaylistItemData>* list_playlist, const int index, const int section, const OptMorePopup::ClickMode clickMode){

        int real_index = this->checkValid_index(list_playlist->length(), index);
        if(real_index >= 0){
            roseHome::PlaylistItemData data = list_playlist->at(real_index);
            this->proc_clicked_optMorePopup_fromPlaylist(data, real_index, section, clickMode);
        }
    }


    // OptMorePopup 관련 처리 of Track ---------------------------------------------------------------------------------------------

    /**
     * @brief TrackItemData 를 활용하여 OptMorePopup 의 signal 을 처리한다.
     * @param data_album
     * @param clickMode
     */
    void AbstractRoseHomeSubWidget::proc_clicked_optMorePopup_fromTrack(roseHome::TrackItemData& data_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

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
            ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
            procRosePlay->requestPlayRose_byTracks(data_track, jsonArr_toPlayAll, index, clickMode);

            /*if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
            {
                QJsonArray tmpJsonArr = QJsonArray();
                for(int i = index; i < jsonArr_toPlayAll.size(); i++){
                    QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(i).toObject();
                    tmpJsonArr.append(tmpJsonObj);
                }

                // Rose Play 요청
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, tmpJsonArr, 0, clickMode);
            }
            else{
                QJsonObject tmpJsonObj = jsonArr_toPlayAll.at(index).toObject();
                QJsonArray tmpJsonArr = QJsonArray();
                tmpJsonArr.append(tmpJsonObj);

                // Rose Play 요청
                ProcRosePlay_withRosehome *procRosePlay = new ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_track, tmpJsonArr, 0, clickMode);
            }*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_Favorite){
            // 즐겨찾기 추가 - Track
            //ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_addFavorite_track(data_track.id);
            //proc->request_qobuz_set_favorite("track", QString("%1").arg(data_track.id), true);
        }
        else if(clickMode == OptMorePopup::ClickMode::Delete_Favorite){
            // 즐겨찾기 삭제 - Track
            //ProcCommon *proc = new ProcCommon(this);
            //proc->request_qobuz_deleteFavorite_track(data_track.id);
            //proc->request_qobuz_set_favorite("track", QString("%1").arg(data_track.id), false);
        }
        else if(clickMode == OptMorePopup::ClickMode::Add_MyCollection){
            // 플레이리스트 담기 - Track
            //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
            // Artist DetailView
            //this->movePage_artist_detailView(data_track.list_artist_id, data_track.list_artist_name);

            // Artist DetailView
            /*if(data_track.list_artist_id.count() > 1){
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
            }*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Go_Album){
            // Album DetailView
            /*if(global.user_forQobuz.flag_rosehome == true){
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
            }*/
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220823
            if(data_track.type != "MUSIC"){//c220903_2
                setUIShare();
            }else{
                ToastMsg::show(this, "", tr("Music content does not provide a sharing service."), 2000);
            }
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
    void AbstractRoseHomeSubWidget::proc_clicked_optMorePopup_fromTrack(QList<roseHome::TrackItemData>* list_track, const QJsonArray& jsonArr_toPlayAll, const int index, const OptMorePopup::ClickMode clickMode){

        int real_index = this->checkValid_index(list_track->length(), index);
        if(real_index >= 0){
            roseHome::TrackItemData data = list_track->at(real_index);
            this->proc_clicked_optMorePopup_fromTrack(data, jsonArr_toPlayAll, real_index, clickMode);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 자식 클래스에서 페이지 이동의 간편화. SubPage에 대해, PageInfo struct로 페이지 이동을 처리하기 위함.
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Album All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractRoseHomeSubWidget::movePage_album_allView(roseHome::PageInfo_AlbumAllView &data_pageInfo){

        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ALBUM_LIST_ALL_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
    }


    /**
     * @brief Playlist All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractRoseHomeSubWidget::movePage_playlist_allView(roseHome::PageInfo_PlaylistAllView &data_pageInfo){

        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_playlistAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_PLAYLIST_LIST_ALL_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    void AbstractRoseHomeSubWidget::movePage_userPage(roseHome::PageInfo_UserPage &data_pageInfo){

        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_UserPage(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_USERPAGE);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    void AbstractRoseHomeSubWidget::movePage_radio_allView(roseHome::PageInfo_RadioAllView &data_pageInfo){

        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_radioAllView(data_pageInfo);
        if(data_pageInfo.type == "Genre"){
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RR_GENREVIEWALL);
        }
        else if(data_pageInfo.type == "Country"){
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RR_COUNTRYVIEWALL);
        }
        else if(data_pageInfo.type == "CountryDetail"){
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RR_COUNTRYDETAIL);
        }
        else if(data_pageInfo.type == "Region"){
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RR_REGIONVIEWALL);
        }
        else if(data_pageInfo.type == "Language"){
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RR_LANGUAGEVIEWALL);
        }
        else{
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RR_LISTVIEWALL);
        }
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    /**
     * @brief Rosetube All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractRoseHomeSubWidget::movePage_rosetube_allView(roseHome::PageInfo_PlaylistAllView &data_pageInfo){

        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_playlistAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_ROSETUBE_LIST_ALL_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }

    /**
     * @brief Track All List 화면으로 이동
     * @param data_pageInfo
     */
    void AbstractRoseHomeSubWidget::movePage_track_allView(roseHome::PageInfo_TrackAllView &data_pageInfo){

        QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_trackAllView(data_pageInfo);
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_RH_TRACK_LIST_ALL_VIEW);
        emit this->signal_clickedMovePage(jsonObj_move);
    }


    QList<bugs::FilterOptionData> AbstractRoseHomeSubWidget::get_list_filterOptData_forMyCollection(){

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
}
