#include "BugsMusicChart.h"
#include "ConvertData_forBugs.h"
#include "DataCommon_Bugs.h"
#include "ProcBugsAPI.h"
#include <QDebug>
#include "common/global.h"
#include "common/gscommon.h"
#include <delegate/BugsTrackDelegate.h>
#include "common/ProcJsonEasy.h"
#include "tidal/AbstractItem.h"
#include "ConvertGenreCode_BugsAPI.h"
#include <QScroller>
#include <QScrollBar>
#include <QResizeEvent>
#include <QMouseEvent>


namespace bugs {

    const int SECTION_FOR_MORE_POPUP___track = 1;
    const int SECTION_FOR_MORE_POPUP___album = 2;
    const int SECTION_FOR_MORE_POPUP___video = 3;

    const QString ICON_PATH___playAll = ":/images/icon_list_allplay.png";
    const QString ICON_PATH___playShuffle = ":/images/icon_list_shuffle.png";

    const int GET_ITEM_SIZE___ONCE = 30;



    /**
     * @brief "TIDAL > 홈 > 최신음악 메인" 화면을 위한 생성자.
     * @param parent
     */
    BugsMusicChart::BugsMusicChart(QWidget *parent) : AbstractBugsSubWidget(NoneScroll, parent) {

        // Data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_album = new QList<bugs::AlbumItemData>();
        this->list_video = new QList<bugs::VideoItemData>();


        // margin 변경
        this->box_contents->setContentsMargins(33,0,0,0);
    }


    /**
     * @brief BugsMusicRecently::setActivePage
     */
    void BugsMusicChart::setActivePage(){

        // 부모 함수 호출하지 않음. (scrolling 을 그대로 유지하기 위함)

        // Login 상태변경에 대한 처리와 동일.
        this->applyLoginSuccessState();

    }


    /**
     * @brief BugsMusicRecently::applyLoginSuccessState
     */
    void BugsMusicChart::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();

                // init Data
                DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
                this->selected_filterCode = dataCommon_bugs.list_all.count()>0 ? dataCommon_bugs.list_all.at(0).opt_code : ConvertGenreCode_BugsAPI_const::DEFAULT_GENRE_CODE;

            }

            // clear & init
            this->initAll();
            this->chooseFilterOpt->hideFilterBox();

            this->selected_tabIndex = -1;
            this->segmentTabWidget->set_selectedIndex(0);
            this->slot_segmentTag_clicked(0);

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
     * @brief 데이터 , UI 초기화
     */
    void BugsMusicChart::initAll(){

        this->list_track->clear();
        this->list_album->clear();
        this->list_video->clear();

        this->jsonArr_tracks_toPlay = QJsonArray();
        this->jsonArr_videos_toPlay = QJsonArray();

        this->flag_lastPage_track = false;
        this->flag_lastPage_album = false;
        this->flag_lastPage_video = false;

        this->flagReqMore_track = false;
        this->flagReqMore_album = false;
        this->flagReqMore_video = false;

        this->listWidget_track->clear();
        GSCommon::clearLayout(this->flowLayout_album);
        GSCommon::clearLayout(this->flowLayout_video);
    }

    /**
     * @brief BugsMusicChart::setUIControl_logined
     */
    void BugsMusicChart::setUIControl_logined(){

        // Title 영역 ------------------------------------------ START
        this->addUIControl_mainTitle("벅스 차트");
        this->setUIControl_filterAndTab();



        // Body 영역 ------------------------------------------ START

        // 탭에 의해서 Widget 조절하기 위한 StackedWidget
        this->stackedWidget_content = new QStackedWidget();
        this->stackedWidget_content->setContentsMargins(0,0,0,0);          // bottom에 spacing 넣어둠
        this->stackedWidget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->box_contents->addWidget(this->stackedWidget_content, 1);


        this->setUIControl_body_Track();
        this->setUIControl_body_Album();
        this->setUIControl_body_Video();

        // 필터링 옵션선택 Widget
        this->chooseFilterOpt = new BugsChooseFilterOpt(BugsChooseFilterOpt::FilterOptSize::Small, this);
        connect(this->chooseFilterOpt, &BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsMusicChart::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();

    }


    /**
     * @brief BugsMusicRecently::setUIControl_filterAndTab
     */
    void BugsMusicChart::setUIControl_filterAndTab(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        this->filterWidget->add_filter("all", "전체 장르");
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsMusicChart::slot_filterClicked);

        // 좌측의 탭 메뉴
        QList<QString> list_tab = { "곡", "앨범", "영상" };
        this->segmentTabWidget = new SegmentTabWidget();
        this->segmentTabWidget->setList_tabName(list_tab);
        this->segmentTabWidget->set_selectedIndex(0);
        this->segmentTabWidget->set_fixedTabBtnSize(QSize(125,40));
        connect(this->segmentTabWidget, &SegmentTabWidget::signal_clicked, this, &BugsMusicChart::slot_segmentTag_clicked);

        QHBoxLayout *hBox_filter = new QHBoxLayout();
        hBox_filter->setSpacing(0);
        hBox_filter->setContentsMargins(0,0,0,0);
        hBox_filter->addWidget(this->filterWidget, 0, Qt::AlignVCenter);
        hBox_filter->addStretch(1);
        hBox_filter->addWidget(this->segmentTabWidget, 0, Qt::AlignVCenter);
        hBox_filter->addSpacing(77);

        QWidget *tmp_widget_forFilter = new QWidget();
        tmp_widget_forFilter->setContentsMargins(0,0,0,0);
        tmp_widget_forFilter->setObjectName("widget_forFilter");
        tmp_widget_forFilter->setStyleSheet("#widget_forFilter { background-color:#171717; }");
        tmp_widget_forFilter->setLayout(hBox_filter);
        tmp_widget_forFilter->setFixedHeight(70);

        this->box_mainTitle->addWidget(tmp_widget_forFilter);
        this->addUIControl_dividedLine_onMain();

    }



    /**
     * @brief BugsMusicChart::setUIControl_body_Track
     */
    void BugsMusicChart::setUIControl_body_Track(){

        // 재생버튼 관련 - track용으로
        QWidget *widget_playBtn = this->get_UIControl_btnPlays(true, true);

        // track
        this->delegate = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsTrackDelegate::Bugs_Track_withRank, this);
        connect(delegate, &BugsTrackDelegate::clicked, this, &BugsMusicChart::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_UIControl_ListWidget(this->delegate);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,0,0,0);
        //tmp_vBox->addWidget(widget_playBtn, 0, Qt::AlignLeft);
        tmp_vBox->addWidget(this->listWidget_track);

        this->widget_inScroll_track = new QWidget();
        this->widget_inScroll_track->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_track = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_track->setWidget(this->widget_inScroll_track);

        QVBoxLayout *tmp_playbtn = new QVBoxLayout();
        tmp_playbtn->setSpacing(0);
        tmp_playbtn->setContentsMargins(0,10,0,30);
        tmp_playbtn->addWidget(widget_playBtn, 0, Qt::AlignLeft);
        tmp_playbtn->setSpacing(10);
        tmp_playbtn->addWidget(this->scrollArea_track);

        QWidget *widget_track = new QWidget();
        widget_track->setLayout(tmp_playbtn);

        this->stackedWidget_content->addWidget(widget_track);
    }


    /**
     * @brief BugsMusicChart::setUIControl_body_Album
     */
    void BugsMusicChart::setUIControl_body_Album(){

        // 재생버튼 관련 - album 용으로 :: 없음

        // Album
        this->flowLayout_album = this->get_UIControl_flowLayout(0, 20);

        QWidget *widget_forFlow = new QWidget();
        widget_forFlow->setLayout(this->flowLayout_album);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,10,0,30);
        tmp_vBox->addWidget(widget_forFlow);


        this->widget_inScroll_album = new QWidget();
        this->widget_inScroll_album->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_album = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_album->setWidget(this->widget_inScroll_album);

        this->stackedWidget_content->addWidget(this->scrollArea_album);
    }


    /**
     * @brief BugsMusicChart::setUIControl_body_Video
     */
    void BugsMusicChart::setUIControl_body_Video(){

        // 재생버튼 관련 - Video 용으로
        QWidget *widget_playBtn = this->get_UIControl_btnPlays(true, false);

        // Video
        this->flowLayout_video = this->get_UIControl_flowLayout(0, 20);

        QWidget *widget_forFlow = new QWidget();
        widget_forFlow->setLayout(this->flowLayout_video);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,10,0,30);
        tmp_vBox->addWidget(widget_playBtn, 0, Qt::AlignLeft);
        tmp_vBox->addWidget(widget_forFlow);

        this->widget_inScroll_video = new QWidget();
        this->widget_inScroll_video->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_video = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_video->setWidget(this->widget_inScroll_video);

        this->stackedWidget_content->addWidget(this->scrollArea_video);
    }







    /**
     * @brief BugsMusicRecently::resizeEvent
     * @param event
     */
    void BugsMusicChart::resizeEvent(QResizeEvent *event){
        AbstractBugsSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }





    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Request HTTP API
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief BugsGenre::request_trackData
     * @param page
     */
    void BugsMusicChart::request_more_trackData(){
        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // genre code 에 따라서 Bugs API path 정보 필요함
            QString api_subPath = ConvertGenreCode_BugsAPI::convert_api_subPath_bugsChart(ConvertGenreCode_BugsAPI::ContentType::track, this->selected_filterCode);

            // getting track list - request HTTP API
            ProcBugsAPI *proc_track = new ProcBugsAPI(this);
            proc_track->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc_track, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMusicChart::slot_applyResult_tracks);
            proc_track->request_bugs_getList_tracks(api_subPath, GET_ITEM_SIZE___ONCE, page_reqNext);
        }
    }


    /**
     * @brief BugsMusicRecently::request_more_albumData
     */
    void BugsMusicChart::request_more_albumData(){

        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

            // page number
            int page_reqNext = this->list_album->size() / GET_ITEM_SIZE___ONCE + 1;

            // genre code 에 따라서 Bugs API path 정보 필요함
            QString api_subPath = ConvertGenreCode_BugsAPI::convert_api_subPath_bugsChart(ConvertGenreCode_BugsAPI::ContentType::album, this->selected_filterCode);

            // getting album list - request HTTP API
            ProcBugsAPI *proc_album = new ProcBugsAPI(this);
            proc_album->setProperty("section", SECTION_FOR_MORE_POPUP___album);
            connect(proc_album, &ProcBugsAPI::completeReq_list_albums, this, &BugsMusicChart::slot_applyResult_albums);
            proc_album->request_bugs_getList_albums(api_subPath, GET_ITEM_SIZE___ONCE, page_reqNext);
        }
    }


    /**
     * @brief BugsMusicRecently::request_more_videoData
     */
    void BugsMusicChart::request_more_videoData(){
        if(!this->flagReqMore_video && !this->flag_lastPage_video){
            this->flagReqMore_video = true;

            // page number
            int page_reqNext = this->list_video->size() / GET_ITEM_SIZE___ONCE + 1;

            // genre code 에 따라서 Bugs API path 정보 필요함
            QString api_subPath = ConvertGenreCode_BugsAPI::convert_api_subPath_bugsChart(ConvertGenreCode_BugsAPI::ContentType::mv, this->selected_filterCode);

            // getting video list - request HTTP API
            ProcBugsAPI *proc_video = new ProcBugsAPI(this);
            proc_video->setProperty("section", SECTION_FOR_MORE_POPUP___video);
            connect(proc_video, &ProcBugsAPI::completeReq_list_videos, this, &BugsMusicChart::slot_applyResult_videos);
            proc_video->request_bugs_getList_videos(api_subPath, GET_ITEM_SIZE___ONCE, page_reqNext);
        }
    }




    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsMusicChart::proc_wheelEvent_to_getMoreData(){
        if(this->selected_tabIndex == 0){
            this->request_more_trackData();
        }
        else if(this->selected_tabIndex == 1){
            this->request_more_albumData();
        }
        else if(this->selected_tabIndex == 2){
            this->request_more_videoData();
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsMusicChart::slot_filterClicked(){
        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // 장르 데이터 가져오기 위해 singleton data 객체 access
            DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();

            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup("All", dataCommon_bugs.list_all,  this->selected_filterCode);
            this->chooseFilterOpt->addFilterGroup("K-POP", dataCommon_bugs.list_kpop, this->selected_filterCode);
            this->chooseFilterOpt->addFilterGroup("POP", dataCommon_bugs.list_pop,  this->selected_filterCode);
            this->chooseFilterOpt->addFilterGroup("Others", dataCommon_bugs.list_etc,  this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }


    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void BugsMusicChart::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            // 데이터를 새로 다시 가져온다. (selected_index 에 따라 처리함)
            if(this->selected_tabIndex == 0){
                this->scrollArea_track->verticalScrollBar()->setValue(0);
                this->request_more_trackData();
            }
            else if(this->selected_tabIndex == 1){
                this->scrollArea_album->verticalScrollBar()->setValue(0);
                this->request_more_albumData();
            }
            else if(this->selected_tabIndex == 2){
                this->scrollArea_video->verticalScrollBar()->setValue(0);
                this->request_more_videoData();
            }

        }

    }


    /**
     * @brief tab index 변경마다 처리함
     * @param clickedIndex
     */
    void BugsMusicChart::slot_segmentTag_clicked(int clickedIndex){
        // 원래의 index와 다른 경우에만 동작
        if(clickedIndex != this->selected_tabIndex){

            this->selected_tabIndex = clickedIndex;
            this->stackedWidget_content->setCurrentIndex(clickedIndex);

            if(clickedIndex == 0){
                // track
                if(this->list_track->length() == 0){
                    this->request_more_trackData();
                }
            }
            else if(clickedIndex == 1){
                // album
                if(this->list_album->length() == 0){
                    this->request_more_albumData();
                }
            }
            else if(clickedIndex == 2){
                // video
                if(this->list_video->length() == 0){
                    this->request_more_videoData();//cheon210704-list

                }
            }

        }
    }





    /**
     * @brief "전체재생"  버튼 클릭
     */
    void BugsMusicChart::slot_btnClicked_playAll(){
        if(this->selected_tabIndex == 0){
            this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
        }
        else if(this->selected_tabIndex == 2){
            this->proc_play_allVideo(this->jsonArr_videos_toPlay, false);
        }
    }


    /**
     * @brief "셔플재생" 버튼 클릭
     */
    void BugsMusicChart::slot_btnClicked_playShuffle(){
        if(this->selected_tabIndex == 0){
            this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
        }
    }


    /**
     * @brief BugsMusicRecently::slot_applyResult_tracks
     * @param list_data
     * @param jsonObj_track
     */
    void BugsMusicChart::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){
        this->flag_lastPage_track = flag_lastPage;
        this->flagReqMore_track = false;            // init
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track){
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            int start_index = this->list_track->size();
            this->list_track->append(list_data);
            this->createList_itemTrackDelegate_applyingWithData_showImage(list_data, this->listWidget_track, start_index, section, true);
        }
    }


    /**
     * @brief BugsMusicRecently::slot_applyResult_albums
     * @param list_data
     */
    void BugsMusicChart::slot_applyResult_albums(const QList<bugs::AlbumItemData>& list_data, const bool flag_lastPage){
        this->flag_lastPage_album = flag_lastPage;
        this->flagReqMore_album = false;            // init
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___album){
            int start_index = this->list_album->length();
            this->list_album->append(list_data);
            this->createList_itemAlbum_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_album, start_index, section);
        }
    }

    /**
     * @brief BugsMusicRecently::slot_applyResult_videos
     * @param list_data
     * @param jsonArr_dataToPlay
     */
    void BugsMusicChart::slot_applyResult_videos(const QList<bugs::VideoItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){
        this->flag_lastPage_video = flag_lastPage;
        this->flagReqMore_video = false;            // init
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___video){
            this->jsonArr_videos_toPlay = jsonArr_dataToPlay;
            int start_index = this->list_video->length();
            this->list_video->append(list_data);
            this->createList_itemVideo_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Ractangle_251x139, this->flowLayout_video, start_index, section);
        }
    }





    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void BugsMusicChart::slot_thumbnailDownloaded_fotListWidget_delegate(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        int section = fileDownloader_sender->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsMusicChart::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsMusicRecently::slot_clickedItemAlbum
     */
    void BugsMusicChart::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief BugsMusicRecently::slot_clickedItemVideo
     * @param clickMode
     */
    void BugsMusicChart::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
    }



    /**
     * @brief BugsHome::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsMusicChart::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
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
     * @brief BugsMusicRecently::proc_preUpdate_favorite_album
     * @param data_itemPos
     */
    void BugsMusicChart::proc_preUpdate_favorite_album(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___album){
            // Update Data
            bugs::AlbumItemData data_album = this->list_album->at(data_itemPos.index);
            data_album.likes_yn = data_itemPos.likes_yn;
            this->list_album->replace(data_itemPos.index, data_album);
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
    void BugsMusicChart::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___album){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
        }
    }



}
