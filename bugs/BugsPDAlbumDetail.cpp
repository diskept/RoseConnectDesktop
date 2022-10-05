#include "BugsPDAlbumDetail.h"
#include <QPixmapCache>

#include "common/gscommon.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "bugs/ConvertData_forBugs.h"

#include "roseHome/ProcCommon_forRosehome.h"


namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___pd_album = 1;
    const int SECTION_FOR_MORE_POPUP___track = 2;



    /**
     * @brief
     * @param parent
     */
    BugsPDAlbumDetail::BugsPDAlbumDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();
    }

    /**
     * @brief 소멸자.
     */
    BugsPDAlbumDetail::~BugsPDAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief BugsPDAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsPDAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::PD_AlbumItemData tmp_data_pd_album = ConvertData_forBugs::convertData_pd_albumData(jsonObj);

        this->flagNeedReload = false;
        if(tmp_data_pd_album.es_album_id != this->data_pd_album.es_album_id){
            this->flagNeedReload = true;
            this->data_pd_album = tmp_data_pd_album;
        }

    }


    /**
     * @brief BugsPDAlbumDetail::applyLoginSuccessState
     */
    void BugsPDAlbumDetail::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();
            }


            // 필요에 따라 리로드 한다.
            if(this->flagNeedReload){

                // Init UI
                this->label_mainTitle->setText(this->data_pd_album.title);
                this->label_mainTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
                this->pd_albumImageDetailInfo->initView();

                this->list_track->clear();
                this->listWidget_track->clear();
                this->listWidget_track->setFixedHeight(70*5);


                // request HTTP API
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                if(this->data_pd_album.title.isEmpty()){
                    // PD_Album 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                    connect(proc, &ProcBugsAPI::completeReq_pd_album, this, &BugsPDAlbumDetail::slot_applyResult_pd_albumInfo);
                    proc->request_bugs_get_pd_album(this->data_pd_album.es_album_id);
                }
                else{
                    this->pd_albumImageDetailInfo->setData_fromBugsData(this->data_pd_album);
                }

                // get Tracks of the album
                connect(proc, &ProcBugsAPI::completeReq_list_items_of_pd_album, this, &BugsPDAlbumDetail::slot_applyResult_tracks);
                proc->request_bugs_getList_items_of_pd_album(this->data_pd_album.es_album_id);

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
     * @brief 화면 UI 세팅
     */
    void BugsPDAlbumDetail::setUIControl_logined(){

        // 메인 타이틀
        this->label_mainTitle = this->get_addUIControl_mainTitle("Album Title");
        this->label_mainTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy

        // 앨범 상세정보
        this->pd_albumImageDetailInfo = new PDAlbumImageDetailInfo();
        connect(this->pd_albumImageDetailInfo, &PDAlbumImageDetailInfo::signal_clicked, this, &BugsPDAlbumDetail::slot_detailInfo_btnClicked);
        connect(this->pd_albumImageDetailInfo, &PDAlbumImageDetailInfo::signal_tagBtnClicked, this, &BugsPDAlbumDetail::slot_pdAlbumInfo_tagBtnClicked);

        this->box_contents->addWidget(this->pd_albumImageDetailInfo, 0, Qt::AlignTop);
        this->box_contents->addSpacing(30);


        // 앨범의 트랙정보
        this->delegate_track = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsTrackDelegate::Bugs_Track_General, this);
        connect(delegate_track, &BugsTrackDelegate::clicked, this, &BugsPDAlbumDetail::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate_track);

    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief PDAlbumImageDetailInfo 에서 태그 버튼 클릭에 대한 처리
     * @param index_tag
     */
    void BugsPDAlbumDetail::slot_pdAlbumInfo_tagBtnClicked(int index_tag){
        // check if the index is valid
        if(index_tag>=0 && index_tag<this->data_pd_album.list_tag_id.count()){

            // PA's Album All View 페이지 이동 (해당 tag에 대하여)
            PageInfo_PDAlbumAllView data_pageInfo;
            data_pageInfo.sortOpt = QVariant::fromValue(ProcBugsAPI::BugsPDAlbum_Sort::Recent).toString();
            data_pageInfo.tag_ids = QString("%1").arg(this->data_pd_album.list_tag_id.at(index_tag));
            data_pageInfo.mainTitle = this->data_pd_album.list_tag_nm.at(index_tag);

            this->movePage_pd_album_allView(data_pageInfo);
        }
    }


    /**
     * @brief BugsPDAlbumDetail::slot_applyResult_albumInfo
     * @param data_album
     */
    void BugsPDAlbumDetail::slot_applyResult_pd_albumInfo(bugs::PD_AlbumItemData data_pd_album){
        // Update Data
        this->data_pd_album = data_pd_album;

        // Update UI
        this->label_mainTitle->setText(this->data_pd_album.title);
        this->pd_albumImageDetailInfo->setData_fromBugsData(this->data_pd_album);

        QString param = "BUGS/musicpd/" + QString("%1").arg(this->data_pd_album.es_album_id); //j220905 share link

        roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
        connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &BugsPDAlbumDetail::slot_applyResult_getShareLink);
        proc_link->request_rose_get_shareLink(this->data_pd_album.image, this->data_pd_album.descr, this->data_pd_album.title, param);
    }


    void BugsPDAlbumDetail::slot_applyResult_getShareLink(const QString &link){   //j220818 share link

        this->shareLink = link;
    }


    /**
     * @brief BugsPDAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsPDAlbumDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);

        // PD's Album UI - 트랙 정보 업데이트를 위해
        this->pd_albumImageDetailInfo->setData_timeInfo(list_data);

        // Track UI Update
        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, 0, SECTION_FOR_MORE_POPUP___track, true);
    }

    /**
     * @brief [슬롯] 썸네일 다운로드 완료
     */
    void BugsPDAlbumDetail::slot_thumbnailDownloaded_fotListWidget_delegate(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
    }



    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void BugsPDAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode clickMode){
        this->proc_clicked_imageDetailInfo_fromPDAlbum(this->data_pd_album, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___pd_album, clickMode);

    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsPDAlbumDetail::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsPDAlbumDetail::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsPDAlbumDetail::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___track){
            // Update Data
            bugs::TrackItemData data_track = this->list_track->at(data_itemPos.index);
            data_track.likes_yn = data_itemPos.likes_yn;
            this->list_track->replace(data_itemPos.index, data_track);

            // Update ListWidget
            this->listWidget_track->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_trackData(data_track));
        }
    }


    /**
     * @brief BugsPDAlbumDetail::proc_preUpdate_favorite_pd_album
     * @param data_itemPos
     */
    void BugsPDAlbumDetail::proc_preUpdate_favorite_pd_album(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___pd_album){
            // Update Data
            this->data_pd_album.likes_yn = data_itemPos.likes_yn;
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
    void BugsPDAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___pd_album){
            this->proc_clicked_optMorePopup_fromPD_Album(this->data_pd_album, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
