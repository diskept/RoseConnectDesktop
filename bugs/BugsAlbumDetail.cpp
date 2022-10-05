#include "BugsAlbumDetail.h"
#include <QPixmapCache>

#include "common/gscommon.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "bugs/ConvertData_forBugs.h"
//#include "delegate/AbstractTidalTrackDelegate.h"
#include "roseHome/ProcCommon_forRosehome.h"//c220818


namespace bugs {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___track = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;


    /**
     * @brief
     * @param parent
     */
    BugsAlbumDetail::BugsAlbumDetail(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();
    }

    /**
     * @brief 소멸자.
     */
    BugsAlbumDetail::~BugsAlbumDetail(){

        this->deleteLater();
    }


    /**
     * @brief BugsAlbumDetail::setJsonObject_forData
     * @param jsonObj
     */
    void BugsAlbumDetail::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::AlbumItemData tmp_data_album = ConvertData_forBugs::convertData_albumData(jsonObj);

        this->flagNeedReload = false;
        if(tmp_data_album.album_id != this->data_album.album_id){
            this->flagNeedReload = true;
            this->data_album = tmp_data_album;
        }

    }


    /**
     * @brief BugsAlbumDetail::applyLoginSuccessState
     */
    void BugsAlbumDetail::applyLoginSuccessState(){

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
                this->label_mainTitle->setText(this->data_album.title);
                this->label_mainTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy
                this->albumImageDetailInfo->initView_forBugs();

                this->list_track->clear();
                this->listWidget_track->clear();
                this->listWidget_track->setFixedHeight(70*5);


                // request HTTP API
                ProcBugsAPI *proc = new ProcBugsAPI(this);
                if(this->data_album.title.isEmpty()){
                    // Album 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
                    connect(proc, &ProcBugsAPI::completeReq_album, this, &BugsAlbumDetail::slot_applyResult_albumInfo);
                    proc->request_bugs_get_album(this->data_album.album_id);
                }
                else{
                    this->albumImageDetailInfo->setData_fromBugsData(this->data_album);
                }

                // get Tracks of the album
                connect(proc, &ProcBugsAPI::completeReq_list_items_of_album, this, &BugsAlbumDetail::slot_applyResult_tracks);
                proc->request_bugs_getList_items_of_album(this->data_album.album_id);

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
    void BugsAlbumDetail::setUIControl_logined(){

        // 메인 타이틀
        this->label_mainTitle = this->get_addUIControl_mainTitle("Album Title");
        this->label_mainTitle->setTextInteractionFlags(Qt::TextSelectableByMouse);//cheon210714-mousecopy

        // 앨범 상세정보
        this->albumImageDetailInfo = new AlbumImageDetailInfo();
        connect(this->albumImageDetailInfo, &AlbumImageDetailInfo::signal_clicked, this, &BugsAlbumDetail::slot_detailInfo_btnClicked);

        this->box_contents->addWidget(this->albumImageDetailInfo, 0, Qt::AlignTop);
        this->box_contents->addSpacing(30);


        // 앨범의 트랙정보
        this->delegate_track = new BugsAlbumTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsAlbumTrackDelegate::Bugs_Track_General, this);
        connect(delegate_track, &BugsAlbumTrackDelegate::clicked, this, &BugsAlbumDetail::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_addUIControl_ListWidget(this->delegate_track);

    }




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsAlbumDetail::slot_applyResult_albumInfo
     * @param data_album
     */
    void BugsAlbumDetail::slot_applyResult_albumInfo(bugs::AlbumItemData data_album){
        // Update Data
        this->data_album = data_album;

        // Update UI
        this->label_mainTitle->setText(this->data_album.title);
        this->albumImageDetailInfo->setData_fromBugsData(this->data_album);
        this->albumImageDetailInfo->setPossiblePlay(this->data_album.valid_yn);

        QString param = "BUGS/ALBUM/" + QString("%1").arg(this->data_album.album_id); //j220905 share link
        QString artist = this->data_album.list_artist_nm.join(",");

        roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
        connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &BugsAlbumDetail::slot_applyResult_getShareLink);
        proc_link->request_rose_get_shareLink(this->data_album.album_image, artist, this->data_album.title, param);
    }


    void BugsAlbumDetail::slot_applyResult_getShareLink(const QString &link){//c220818

        this->shareLink = link;
    }

    /**
     * @brief BugsAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds
     * @param p_jsonObj
     */
    void BugsAlbumDetail::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        this->albumImageDetailInfo->setFavoritesIds_forTidal(ProcJsonEasy::getJsonArray(p_jsonObj, "ALBUM").toVariantList());
    }

    /**
     * @brief BugsAlbumDetail::slot_applyResult_tracks
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsAlbumDetail::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
        this->list_track->append(list_data);
        this->createList_itemTrackDelegate_applyingWithData(list_data, this->listWidget_track, true);
    }



    /**
     * @brief [Slot] AlbumImageDetailInfo의 signal에 대하여 처리
     * @param clickMode
     */
    void BugsAlbumDetail::slot_detailInfo_btnClicked(const AbstractImageDetailInfo::BtnClickMode clickMode){
        this->proc_clicked_imageDetailInfo_fromAlbum(this->data_album, this->jsonArr_tracks_toPlay, SECTION_FOR_MORE_POPUP___albums, clickMode);
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsAlbumDetail::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsAlbumDetail::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsAlbumDetail::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
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
     * @brief BugsAlbumDetail::proc_preUpdate_favorite_album
     * @param data_itemPos
     */
    void BugsAlbumDetail::proc_preUpdate_favorite_album(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___albums){
            // Update Data
            this->data_album.likes_yn = data_itemPos.likes_yn;
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
    void BugsAlbumDetail::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(data_album, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
