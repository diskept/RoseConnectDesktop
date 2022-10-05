#include "BugsMyCollection.h"
#include "ConvertData_forBugs.h"
#include "ProcBugsAPI.h"

#include <QDebug>
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

namespace bugs {


    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX___track_like = 1;
    const int BTN_IDX___track_recently = 2;
    const int BTN_IDX___track_most = 3;

    const int SECTION_FOR_MORE_POPUP___track_like = 1;
    const int SECTION_FOR_MORE_POPUP___track_recently = 2;
    const int SECTION_FOR_MORE_POPUP___track_most = 3;
    const int SECTION_FOR_MORE_POPUP___my_album = 4;


    const QString BUGS_API_SUBPAHT___track_like = "mymusic/like/track";
    const QString BUGS_API_SUBPAHT___track_recently = "mymusic/track/listened/recent";
    const QString BUGS_API_SUBPAHT___track_most = "mymusic/track/listened/most";



    /**
     * @brief "TIDAL > My Collection" 화면을 위한 생성자.
     * @param parent
     */
    BugsMyCollection::BugsMyCollection(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll, parent) {

        // Data
        this->list_track_like = new QList<bugs::TrackItemData>();
        this->list_track_recently = new QList<bugs::TrackItemData>();
        this->list_track_most = new QList<bugs::TrackItemData>();
        this->list_album_my = new QList<bugs::MyAlbumItemData>();

        this->flagReq_like = false;
        this->flagReq_recently = false;
        this->flagReq_most = false;
        this->flagReq_my_album = false;
    }

    /**
     * @brief BugsHome::applyLoginSuccessState
     */
    void BugsMyCollection::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }


            //-----  항상 새롭게 요청함 -------------

            // 광클 대비 : 좋아한 음악
            if(this->flagReq_like == false){
                this->flagReq_like = true;

                this->list_track_like->clear();

                GSCommon::clearLayout(this->hBox_like);

                // request HTTP API
                ProcBugsAPI *proc_like = new ProcBugsAPI(this);
                proc_like->setProperty("section", SECTION_FOR_MORE_POPUP___track_like);
                connect(proc_like, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyCollection::slot_applyResult_tracks);
                proc_like->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_like, 10, 1);
            }

            // 광클 대비 : 최근 들은 곡
            if(this->flagReq_recently == false){
                this->flagReq_recently = true;

                this->list_track_recently->clear();

                this->listWidget_track_recently->clear();
                this->listWidget_track_recently->setFixedHeight(70*5);

                ProcBugsAPI *proc_recently = new ProcBugsAPI(this);
                proc_recently->setProperty("section", SECTION_FOR_MORE_POPUP___track_recently);
                connect(proc_recently, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyCollection::slot_applyResult_tracks);
                proc_recently->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_recently, 5, 1);
            }

            // 광클 대비 : 많이 들은 곡
            if(this->flagReq_most == false){
                this->flagReq_most = true;

                this->list_track_most->clear();

                this->listWidget_track_most->clear();
                this->listWidget_track_most->setFixedHeight(70*5);

                ProcBugsAPI *proc_most = new ProcBugsAPI(this);
                proc_most->setProperty("section", SECTION_FOR_MORE_POPUP___track_most);
                connect(proc_most, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyCollection::slot_applyResult_tracks);
                proc_most->request_bugs_getList_tracks(BUGS_API_SUBPAHT___track_most, 5, 1);
            }


            // 광클 대비 : 내 일범
            if(this->flagReq_my_album == false){
                this->flagReq_my_album = true;

                this->list_album_my->clear();

                GSCommon::clearLayout(this->flowLayout_album_my);

                ProcBugsAPI *proc_my = new ProcBugsAPI(this);
                proc_my->setProperty("section", SECTION_FOR_MORE_POPUP___my_album);
                connect(proc_my, &ProcBugsAPI::completeReq_list_my_albums, this, &BugsMyCollection::slot_applyResult_my_albums);
                proc_my->request_bugs_getList_my_albums(500, 1);          // 기본값으로 요청함 (doc 참고함)
            }

        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsMyCollection::setUIControl_logined
     */
    void BugsMyCollection::setUIControl_logined(){

        // 좋아한 음악
        this->addUIControl_subTitle_withSideBtn_common("좋아한 음악", "View All", BTN_IDX___track_like);
        this->hBox_like = this->get_addUIControl_hBoxLayout_forMusic();

        // 최근 들은 곡
        this->addUIControl_subTitle_withSideBtn_common("최근 들은 곡", "View All", BTN_IDX___track_recently);
        this->delegate_recently = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track_recently, BugsTrackDelegate::Bugs_Track_General, this);
        connect(this->delegate_recently, &BugsTrackDelegate::clicked, this, &BugsMyCollection::slot_clickedItemTrack_inList);
        this->listWidget_track_recently = this->get_addUIControl_ListWidget(this->delegate_recently);

        // 많이 들은 곡
        this->addUIControl_subTitle_withSideBtn_common("많이 들은 곡", "View All", BTN_IDX___track_most);
        this->delegate_most = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track_most, BugsTrackDelegate::Bugs_Track_General, this);
        connect(this->delegate_most, &BugsTrackDelegate::clicked, this, &BugsMyCollection::slot_clickedItemTrack_inList);
        this->listWidget_track_most = this->get_addUIControl_ListWidget(this->delegate_most);


        // 내 앨범
        this->addUIControl_subTitle_withSideBtn_common("내 앨범", "", 0);
        this->flowLayout_album_my = this->get_addUIControl_flowLayout(0, 47);

    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void BugsMyCollection::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX___track_like){
            // 좋아한 음악 - 이동
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___MY_COLLECTION___LIKE_MUSIC);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___track_recently){
            bugs::PageInfo_TrackAllView data_pageInfo;
            data_pageInfo.pathTitle = "최근 들은 곡";
            data_pageInfo.api_subPath = BUGS_API_SUBPAHT___track_recently;
            this->movePage_track_allView(data_pageInfo);
        }
        else if(btnId == BTN_IDX___track_most){
            bugs::PageInfo_TrackAllView data_pageInfo;
            data_pageInfo.pathTitle = "많이 들은 곡";
            data_pageInfo.api_subPath = BUGS_API_SUBPAHT___track_most;
            this->movePage_track_allView(data_pageInfo);
        }

    }

    /**
     * @brief BugsHome::slot_applyResult_tracks
     * @param list_data
     * @param jsonObj_track
     */
    void BugsMyCollection::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track_like){
            this->flagReq_like = false;
            this->jsonArr_tracks_toPlay_like = jsonArr_dataToPlay;
            this->list_track_like->append(list_data);
            this->createList_itemTrackSqure_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_like, SECTION_FOR_MORE_POPUP___track_like);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_recently){
            this->flagReq_recently = false;
            this->jsonArr_tracks_toPlay_recently = jsonArr_dataToPlay;
            this->list_track_recently->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track_recently, 0, section, true);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_most){
            this->flagReq_recently = false;
            this->jsonArr_tracks_toPlay_most = jsonArr_dataToPlay;
            this->list_track_most->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track_most, 0, section, true);
        }
    }


    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void BugsMyCollection::slot_thumbnailDownloaded_fotListWidget_delegate(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        int section = fileDownloader_sender->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track_recently){
            this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track_recently, this->list_track_recently);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_most){
            this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track_most, this->list_track_most);
        }
    }



    /**
     * @brief [Slot] My Album에 대한 처리
     * @param list_data
     * @param flag_lastPage
     */
    void BugsMyCollection::slot_applyResult_my_albums(const QList<bugs::MyAlbumItemData>& list_data, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        this->flagReq_my_album = false;
        int start_index = this->list_album_my->count();
        this->list_album_my->append(list_data);
        this->createList_itemMyAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->flowLayout_album_my, start_index, SECTION_FOR_MORE_POPUP___my_album);
    }


    /**
     * @brief BugsHome::slot_clickedItemTrackSqure
     */
    void BugsMyCollection::slot_clickedItemTrackSqure(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemTrackSquare(this->list_track_like, this->jsonArr_tracks_toPlay_like, clickMode, index, section);
    }

    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsMyCollection::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track_recently){
            this->proc_clicked_itemTrack_inList(this->list_track_recently, this->jsonArr_tracks_toPlay_recently, clickMode, idx, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_most){
            this->proc_clicked_itemTrack_inList(this->list_track_most, this->jsonArr_tracks_toPlay_most, clickMode, idx, section);
        }

    }


    /**
     * @brief BugsMyCollection::slot_clickedItemMyAlbum
     * @param clickMode
     */
    void BugsMyCollection::slot_clickedItemMyAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemMyAlbum(this->list_album_my, clickMode, index, section);
    }



    /**
     * @brief BugsHome::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsMyCollection::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track_like){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_like->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track_like->replace(data_itemPos.index, data_track);
        }
        else if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track_recently){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_recently->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track_recently->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_track_recently->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }
        else if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track_most){
            // Update Data
            bugs::TrackItemData data_track = this->list_track_most->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track_most->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_track_most->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }

    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsMyCollection::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___track_like){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track_like, this->jsonArr_tracks_toPlay_like, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_recently){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track_recently, this->jsonArr_tracks_toPlay_recently, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track_most){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track_most, this->jsonArr_tracks_toPlay_most, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___my_album){
            this->proc_clicked_optMorePopup_fromMyAlbum(this->list_album_my, index, clickMode);
        }
    }

}
