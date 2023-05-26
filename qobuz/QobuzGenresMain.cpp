#include "QobuzGenresMain.h"
#include <QDebug>
#include <QScroller>
#include "widget/VerticalScrollArea.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "qobuz/ProcCommon_forQobuz.h"
#include "qobuz/ItemPlaylist_forQobuz.h"
#include "qobuz/ItemAlbum_forQobuz.h"

#include "qobuz/ConvertData_forQobuz.h"

namespace qobuz {

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;

    const int BTN_IDX_SUBTITLE_universalClassic = 2;
    const int BTN_IDX_SUBTITLE_idealDiscography = 2;
    const int BTN_IDX_SUBTITLE_stillTranding = 3;
    const int BTN_IDX_SUBTITLE_pressAwards = 4;
    const int BTN_IDX_SUBTITLE_qobuzIssime = 5;
    const int BTN_IDX_SUBTITLE_topRelease = 6;
    //const int BTN_IDX_SUBTITLE_suggestNewTracks = 3;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___classic = 1;
    const int SECTION_FOR_MORE_POPUP___idealdiscography = 2;
    const int SECTION_FOR_MORE_POPUP___stilltrending = 3;
    const int SECTION_FOR_MORE_POPUP___pressawards = 4;
    const int SECTION_FOR_MORE_POPUP___qobuzissime = 5;
    const int SECTION_FOR_MORE_POPUP___toprelease = 6;

    // API 관련
    const QString QOBUZ_API_SUGGESTED_ALBUM = "/album/getFeatured";

    const QString QOBUZ_API_UNIVERSAL_CLASSIC = "universal-classic";
    const QString QOBUZ_API_IDEAL_DISCOGRAPHY = "ideal-discography";
    const QString QOBUZ_API_STILL_TRANDING = "recent-releases";
    const QString QOBUZ_API_PRESS_AWARDS = "press-awards";
    const QString QOBUZ_API_QOBUZ_ISSIME = "qobuzissims";

    const QString QOBUZ_API_TOP_TOP_RELEASE = "most-streamed";


    /**
     * @brief 생성자. 기본UI만 세팅함
     * @param parent
     */
    QobuzGenresMain::QobuzGenresMain(QWidget *parent) : AbstractQobuzSubWidget(VerticalScroll, parent){

        // 기본 UI 세팅
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ");

        // 데이터
        this->list_classic = new QList<qobuz::AlbumItemData>();
        this->list_idealDiscography = new QList<qobuz::AlbumItemData>();
        this->list_still_trending = new QList<qobuz::AlbumItemData>();
        this->list_press_awards = new QList<qobuz::AlbumItemData>();
        this->list_qobuz_issime = new QList<qobuz::AlbumItemData>();

        this->list_top_release = new QList<qobuz::AlbumItemData>();
    }


    /**
     * @brief 소멸자
     */
    QobuzGenresMain::~QobuzGenresMain(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzGenresMain::setJsonObject_forData(const QJsonObject &jsonObj){

        // set the main title and genre's path
        this->flagNeedReload = false;

        QString tmp_genre_id = ProcJsonEasy::getString(jsonObj, "genre_id");
        int tmp_gerne_cnt = ProcJsonEasy::getInt(jsonObj, "genre_cnt");

        if(this->genre_ids != tmp_genre_id)
        {
            this->flagNeedReload = true;
            this->label_mainTitle->setText(ProcJsonEasy::getString(jsonObj, "name"));            
            this->genre_cnt = tmp_gerne_cnt;
            this->genre_ids = tmp_genre_id;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzGenresMain::setActivePage(){

        // 항상 부모클래스의 함수 먼저 호출
        AbstractQobuzSubWidget::setActivePage();

        if(this->flagNeedReload){

            // init view
            /*this->setUIControl_univertalClassic();
            GSCommon::clearLayout(this->hBox_classic);

            this->setUIControl_idealDiscography();
            GSCommon::clearLayout(this->hBox_idealDiscography);
            this->setUIControl_stillTranding();
            GSCommon::clearLayout(this->hBox_stillTranding);
            this->setUIControl_pressAwards();
            GSCommon::clearLayout(this->hBox_pressAwards);
            this->setUIcontrol_qobuzIssime();
            GSCommon::clearLayout(this->hBox_qobuzIssime);
            this->setUIcontrol_topRelease();
            this->listWidget_toprelease->clear();*/

            // init data
            jsonArr_tracks_toPlay = QJsonArray();

            this->list_classic->clear();
            this->list_idealDiscography->clear();
            this->list_still_trending->clear();
            this->list_press_awards->clear();
            this->list_qobuz_issime->clear();

            this->list_top_release->clear();

            for(int i = 0; i < this->genre_cnt; i++){
                if(global.user_forQobuz.enable_explore_filter[i] == 1){
                    if(genre_ids.isEmpty()){
                        genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(i).id);
                    }
                    else{
                        genre_ids += QString(", %1").arg(ProcCommon::getGenreInfo(i).id);
                    }
                }
            }

            // request HTTP API
            /*qobuz::ProcCommon *proc_c = new qobuz::ProcCommon(this);
            connect(proc_c, &ProcCommon::completeReq_list_albums, this, &QobuzGenresMain::slot_applyResult_classic);
            proc_c->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_UNIVERSAL_CLASSIC, this->genre_id, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);*/

            qobuz::ProcCommon *proc_i = new qobuz::ProcCommon(this);
            connect(proc_i, &ProcCommon::completeReq_list_albums, this, &QobuzGenresMain::slot_applyResult_idealDiscography);
            proc_i->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_IDEAL_DISCOGRAPHY, this->genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            qobuz::ProcCommon *proc_s = new qobuz::ProcCommon(this);
            connect(proc_s, &ProcCommon::completeReq_list_albums, this, &QobuzGenresMain::slot_applyResult_stillTrending);
            proc_s->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_STILL_TRANDING, this->genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            qobuz::ProcCommon *proc_q = new qobuz::ProcCommon(this);
            connect(proc_q, &ProcCommon::completeReq_list_albums, this, &QobuzGenresMain::slot_applyResult_pressAwards);
            proc_q->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_PRESS_AWARDS, this->genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            qobuz::ProcCommon *proc_p = new qobuz::ProcCommon(this);
            connect(proc_p, &ProcCommon::completeReq_list_albums, this, &QobuzGenresMain::slot_applyResult_qobuzIssime);
            proc_p->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_QOBUZ_ISSIME, this->genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzGenresMain::slot_applyResult_topRelease);
            proc->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_TOP_TOP_RELEASE, this->genre_ids, 5, 0);

            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief Suggested New Album 파트 기본 UI 구성
     */
    void QobuzGenresMain::setUIControl_univertalClassic(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Classic"), tr("View All"), BTN_IDX_SUBTITLE_universalClassic);
        this->hBox_classic = this->get_addUIControl_hBoxLayout_forAlbum();
    }

    /**
     * @brief Suggested New Album 파트 기본 UI 구성
     */
    void QobuzGenresMain::setUIControl_idealDiscography(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Ideal discography"), tr("View All"), BTN_IDX_SUBTITLE_idealDiscography);
        this->hBox_idealDiscography = this->get_addUIControl_hBoxLayout_forAlbum();
    }

    /**
     * @brief Suggested New Album 파트 기본 UI 구성
     */
    void QobuzGenresMain::setUIControl_stillTranding(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Still trending"), tr("View All"), BTN_IDX_SUBTITLE_stillTranding);
        this->hBox_stillTranding = this->get_addUIControl_hBoxLayout_forAlbum();
    }

    /**
     * @brief Suggested New Album 파트 기본 UI 구성
     */
    void QobuzGenresMain::setUIControl_pressAwards(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Press awards"), tr("View All"), BTN_IDX_SUBTITLE_pressAwards);
        this->hBox_pressAwards = this->get_addUIControl_hBoxLayout_forAlbum();
    }

    /**
     * @brief Suggested New Album 파트 기본 UI 구성
     */
    void QobuzGenresMain::setUIcontrol_qobuzIssime(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Qobuzissime"), tr("View All"), BTN_IDX_SUBTITLE_qobuzIssime);
        this->hBox_qobuzIssime = this->get_addUIControl_hBoxLayout_forAlbum();
    }


    /**
     * @brief Suggested New Tracks 파트 기본 UI 구성
     */
    void QobuzGenresMain::setUIcontrol_topRelease(){
        this->addUIControl_subTitle_withSideBtn_common(tr("Top release"), tr("View All"), BTN_IDX_SUBTITLE_topRelease);
        this->delegate_album = new QobuzAlbumDelegate(SECTION_FOR_MORE_POPUP___toprelease, QobuzAlbumDelegate::Qobuz_Track_General, this);
        connect(delegate_album, &QobuzTrackDelegate::clicked, this, &QobuzGenresMain::slot_clickedItemAlbum_inList);

        this->listWidget_toprelease = this->get_addUIControl_ListWidget(this->delegate_album);
        this->listWidget_toprelease->setFixedHeight(70*5);
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzGenresMain::slot_applyResult_classic(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        if(list_data.length() > 0)
        {
            Q_UNUSED(jsonArr_dataToPlay);
            Q_UNUSED(flag_lastPage);
            this->list_classic->append(list_data);
            this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_classic, 0, SECTION_FOR_MORE_POPUP___classic);
        }
    }

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzGenresMain::slot_applyResult_idealDiscography(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        if(list_data.length() > 0)
        {
            Q_UNUSED(jsonArr_dataToPlay);
            Q_UNUSED(flag_lastPage);
            this->list_idealDiscography->append(list_data);
            //this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_idealDiscography, 0, SECTION_FOR_MORE_POPUP___idealdiscography);
            this->setUIcontrol_All_contents("ideal", true);
        }
        else{
            this->setUIcontrol_All_contents("ideal", false);
        }
    }

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzGenresMain::slot_applyResult_stillTrending(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        if(list_data.length() > 0)
        {
            Q_UNUSED(jsonArr_dataToPlay);
            Q_UNUSED(flag_lastPage);
            this->list_still_trending->append(list_data);
            //this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_stillTranding, 0, SECTION_FOR_MORE_POPUP___stilltrending);
            this->setUIcontrol_All_contents("still", true);
        }
        else{
            this->setUIcontrol_All_contents("still", false);
        }
    }

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzGenresMain::slot_applyResult_pressAwards(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        if(list_data.length() > 0)
        {
            Q_UNUSED(jsonArr_dataToPlay);
            Q_UNUSED(flag_lastPage);
            this->list_press_awards->append(list_data);
            //this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_pressAwards, 0, SECTION_FOR_MORE_POPUP___pressawards);
            this->setUIcontrol_All_contents("press", true);
        }
        else{
            this->setUIcontrol_All_contents("press", false);
        }
    }

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzGenresMain::slot_applyResult_qobuzIssime(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        if(list_data.length() > 0)
        {
            Q_UNUSED(jsonArr_dataToPlay);
            Q_UNUSED(flag_lastPage);
            this->list_qobuz_issime->append(list_data);
            //this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_qobuzIssime, 0, SECTION_FOR_MORE_POPUP___qobuzissime);
            this->setUIcontrol_All_contents("qobuz", true);
        }
        else{
            this->setUIcontrol_All_contents("qobuz", false);
        }
    }

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzGenresMain::slot_applyResult_topRelease(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){
        if(list_data.length() > 0)
        {
            Q_UNUSED(jsonArr_dataToPlay);
            Q_UNUSED(flag_lastPage);
            this->list_top_release->append(list_data);
            //this->createList_itemAlbumDelegate_applyingWithData_showImage(list_data, this->listWidget_toprelease, 0, SECTION_FOR_MORE_POPUP___toprelease, true);
            this->setUIcontrol_All_contents("top", true);
        }
        else{
            this->setUIcontrol_All_contents("top", false);
        }
    }

    void QobuzGenresMain::setUIcontrol_All_contents(const QString type, const bool flag){
        if(type == "ideal"){
            this->flag_ideal[0] = true;
            this->flag_ideal[1] = flag;
        }
        else if(type == "still"){
            this->flag_still[0] = true;
            this->flag_still[1] = flag;
        }
        else if(type == "press"){
            this->flag_press[0] = true;
            this->flag_press[1] = flag;
        }
        else if(type == "qobuz"){
            this->flag_qobuz[0] = true;
            this->flag_qobuz[1] = flag;
        }
        else if(type == "top"){
            this->flag_top[0] = true;
            this->flag_top[1] = flag;
        }

        if((this->flag_ideal[0] == true) && (this->flag_still[0] == true) && (this->flag_press[0] == true) && (this->flag_qobuz[0] == true) && (this->flag_top[0] == true)){
            if(this->flag_ideal[1] == true){
                this->setUIControl_idealDiscography();
                GSCommon::clearLayout(this->hBox_idealDiscography);

                this->createList_itemAlbum_applyingWithData(*this->list_idealDiscography, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_idealDiscography, 0, SECTION_FOR_MORE_POPUP___idealdiscography);
            }

            if(this->flag_still[1] == true){
                this->setUIControl_stillTranding();
                GSCommon::clearLayout(this->hBox_stillTranding);

                this->createList_itemAlbum_applyingWithData(*this->list_still_trending, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_stillTranding, 0, SECTION_FOR_MORE_POPUP___stilltrending);
            }

            if(this->flag_press[1] == true){
                this->setUIControl_pressAwards();
                GSCommon::clearLayout(this->hBox_pressAwards);

                this->createList_itemAlbum_applyingWithData(*this->list_press_awards, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_pressAwards, 0, SECTION_FOR_MORE_POPUP___pressawards);
            }

            if(this->flag_qobuz[1] == true){
                this->setUIcontrol_qobuzIssime();
                GSCommon::clearLayout(this->hBox_qobuzIssime);

                this->createList_itemAlbum_applyingWithData(*this->list_qobuz_issime, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_qobuzIssime, 0, SECTION_FOR_MORE_POPUP___qobuzissime);
            }

            if(this->flag_top[1] == true){
                this->setUIcontrol_topRelease();
                this->listWidget_toprelease->clear();

                this->createList_itemAlbumDelegate_applyingWithData_showImage(*this->list_top_release, this->listWidget_toprelease, 0, SECTION_FOR_MORE_POPUP___toprelease, true);
            }
        }
    }


    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void QobuzGenresMain::slot_thumbnailDownloaded_fotListWidget_track(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        //this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
        this->applyImageData_itemAlbumDelegate_withFileDownload(fileDownloader_sender, this->listWidget_toprelease, this->list_top_release);
    }


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void QobuzGenresMain::slot_clickBtn_subTitle_side(int btnId){

        /*if(btnId == BTN_IDX_SUBTITLE_universalClassic){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = "Classic";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_UNIVERSAL_CLASSIC;
            data_page.genre_id = this->genre_id;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_CLASSIC_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }*/
        if(btnId == BTN_IDX_SUBTITLE_idealDiscography){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = "Ideal discography";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_IDEAL_DISCOGRAPHY;
            //data_page.genre_id = this->genre_id;
            data_page.genre_ids = this->genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_IDEAL_DISCOGRAPHY_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_stillTranding){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = "Still trending";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_STILL_TRANDING;
            //data_page.genre_id = this->genre_id;
            data_page.genre_ids = this->genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_STILL_TRENDING_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_pressAwards){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = "Press awards";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_PRESS_AWARDS;
            //data_page.genre_id = this->genre_id;
            data_page.genre_ids = this->genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_PRESS_AWARDS_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_qobuzIssime){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = "Qobuzissime";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_QOBUZ_ISSIME;
            //data_page.genre_id = this->genre_id;
            data_page.genre_ids = this->genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_QOBUZ_ISSIME_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_topRelease){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = "Top release";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_TOP_TOP_RELEASE;
            //data_page.genre_id = this->genre_id;
            data_page.genre_ids = this->genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_TOPRELEASE_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }

    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzGenresMain::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        /*if(section == SECTION_FOR_MORE_POPUP___classic){
            this->proc_clicked_itemAlbum(this->list_classic, clickMode, index, section);
        }*/
        if(section == SECTION_FOR_MORE_POPUP___idealdiscography){
            this->proc_clicked_itemAlbum(this->list_idealDiscography, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___stilltrending){
            this->proc_clicked_itemAlbum(this->list_still_trending, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___pressawards){
            this->proc_clicked_itemAlbum(this->list_press_awards, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___qobuzissime){
            this->proc_clicked_itemAlbum(this->list_qobuz_issime, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___toprelease){
            this->proc_clicked_itemAlbum(this->list_top_release, clickMode, index, section);
        }
    }

    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    /*void QobuzGenresMain::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
    }*/


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    /*void QobuzGenresMain::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
    }*/


    void QobuzGenresMain::slot_clickedItemAlbum_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
            int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

            // ClickMode 별로 처리
            this->proc_clicked_itemAlbum_inList(this->list_top_release, clickMode, idx, section);
        }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void QobuzGenresMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        /*if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }*/

        /*if(section == SECTION_FOR_MORE_POPUP___classic){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_classic, index, clickMode);
        }*/
        if(section == SECTION_FOR_MORE_POPUP___idealdiscography){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_idealDiscography, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___stilltrending){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_still_trending, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___pressawards){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_press_awards, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___qobuzissime){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_qobuz_issime, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___toprelease){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_top_release, index, clickMode);
        }
    }

}

