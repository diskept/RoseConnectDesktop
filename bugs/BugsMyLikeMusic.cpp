#include "BugsMyLikeMusic.h"
#include "ConvertData_forBugs.h"
#include "DataCommon_Bugs.h"
#include "ProcBugsAPI.h"
#include "ProcRoseAPI_withBugs.h"
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
    const int SECTION_FOR_MORE_POPUP___artist = 3;
    const int SECTION_FOR_MORE_POPUP___pd_album = 4;

    const int GET_ITEM_SIZE___ONCE = 30;

    const QString BUGS_API_SUBPAHT___like_track = "mymusic/like/track";
    const QString BUGS_API_SUBPAHT___like_album = "mymusic/like/album";
    const QString BUGS_API_SUBPAHT___like_artist = "mymusic/like/artist";
    const QString BUGS_API_SUBPAHT___like_pd_album = "mymusic/like/musicpd/album";



    /**
     * @brief "TIDAL > 홈 > 최신음악 메인" 화면을 위한 생성자.
     * @param parent
     */
    BugsMyLikeMusic::BugsMyLikeMusic(QWidget *parent) : AbstractBugsSubWidget(NoneScroll, parent) {

        // Data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_album = new QList<bugs::AlbumItemData>();
        this->list_artist = new QList<bugs::ArtistItemData>();
        this->list_pd_album = new QList<bugs::PD_AlbumItemData>();


        // Data - 필터링 옵션 만들기 ---------------------------------------- START
        bugs::FilterOptionData filterOpt_recent;
        filterOpt_recent.opt_code = QVariant::fromValue(ProcBugsAPI::BugsMyMusic_Sort::Recent);
        filterOpt_recent.opt_name = "최신 좋아순";

        bugs::FilterOptionData filterOpt_name;
        filterOpt_name.opt_code = QVariant::fromValue(ProcBugsAPI::BugsMyMusic_Sort::Name);
        filterOpt_name.opt_name = "가나다순";

        this->list_filterOpt_sort.append(filterOpt_recent);
        this->list_filterOpt_sort.append(filterOpt_name);
        // Data - 필터링 옵션 만들기 ---------------------------------------- END


        // margin 변경. (전체적으로 바꾸면, 모든 메뉴에 영향이 있어서)
        this->box_contents->setContentsMargins(33,0,0,0);
    }


    /**
     * @brief BugsMusicRecently::setActivePage
     */
    void BugsMyLikeMusic::setActivePage(){

        // 부모 함수 호출하지 않음. (scrolling 을 그대로 유지하기 위함)

        // Login 상태변경에 대한 처리와 동일.
        this->applyLoginSuccessState();

    }


    /**
     * @brief BugsMusicRecently::applyLoginSuccessState
     */
    void BugsMyLikeMusic::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();

                // init Data
                this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            }

            // Track에 좋아요 data sync 때문에 항상 새로 가져오기를 해야함 (Go to Album, Go to Artist 페이지에서 "좋아요" 변경 가능성 있음)
            this->initAll();
            this->chooseFilterOpt->hideFilterBox();

            this->selected_tabIndex = -1;
            this->segmentTabWidget->set_selectedIndex(0);
            this->slot_segmentTag_clicked(0);

                // 로그인 계속 유지.  selected_index 및 필터 초기화 필요? 아니면 그대로 유지?
                //  --> 어떤 경우인지 알기가 조금 어렵네. 현 구조상... Back 한건지, 새롭게 진입한건지의 구분이 필요하겠다.
//                this->slot_segmentTag_clicked(0);
//                this->scrollArea_main->verticalScrollBar()->setValue(0);

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
    void BugsMyLikeMusic::initAll(){

        this->list_track->clear();
        this->list_album->clear();
        this->list_artist->clear();
        this->list_pd_album->clear();

        this->jsonArr_tracks_toPlay = QJsonArray();

        this->flag_lastPage_track = false;
        this->flag_lastPage_album = false;
        this->flag_lastPage_artist = false;
        this->flag_lastPage_pd_album = false;

        this->flagReqMore_track = false;
        this->flagReqMore_album = false;
        this->flagReqMore_artist = false;
        this->flagReqMore_pd_album = false;

        this->listWidget_track->clear();
        GSCommon::clearLayout(this->flowLayout_album);
        this->listWidget_artist->clear();
        GSCommon::clearLayout(this->flowLayout_pd_album);

    }


    /**
     * @brief BugsMyLikeMusic::setUIControl_logined
     */
    void BugsMyLikeMusic::setUIControl_logined(){

        // Title 영역 ------------------------------------------ START
        this->addUIControl_mainTitle("좋아한 음악");
        this->setUIControl_filterAndTab();


        // 탭에 의해서 Widget 조절하기 위한 StackedWidget
        this->stackedWidget_content = new QStackedWidget();
        this->stackedWidget_content->setContentsMargins(0,0,0,0);          // bottom에 spacing 넣어둠
        this->stackedWidget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->box_contents->addWidget(this->stackedWidget_content, 1);

        this->setUIControl_body_Track();
        this->setUIControl_body_Album();
        this->setUIControl_body_Artist();
        this->setUIControl_body_PD_Album();

        // 필터링 옵션선택 Widget
        this->chooseFilterOpt = new BugsChooseFilterOpt(BugsChooseFilterOpt::FilterOptSize::Small, this);
        connect(this->chooseFilterOpt, &BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsMyLikeMusic::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();

    }


    /**
     * @brief BugsMyLikeMusic::setUIControl_filterAndTab
     */
    void BugsMyLikeMusic::setUIControl_filterAndTab(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        this->filterWidget->add_filter(this->list_filterOpt_sort.first().opt_code.toString(), this->list_filterOpt_sort.first().opt_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsMyLikeMusic::slot_filterClicked);

        // 좌측의 탭 메뉴
        QList<QString> list_tab = { "곡", "앨범", "아티스트", "뮤직PD앨범" };
        this->segmentTabWidget = new SegmentTabWidget();
        this->segmentTabWidget->setList_tabName(list_tab);
        this->segmentTabWidget->set_selectedIndex(0);
        this->segmentTabWidget->set_fixedTabBtnSize(QSize(125,40));
        connect(this->segmentTabWidget, &SegmentTabWidget::signal_clicked, this, &BugsMyLikeMusic::slot_segmentTag_clicked);

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
     * @brief BugsMyLikeMusic::setUIControl_body_Track
     */
    void BugsMyLikeMusic::setUIControl_body_Track(){

        // 재생버튼 관련 - track용으로
        QWidget *widget_playBtn = this->get_UIControl_btnPlays(true, false);

        // track
        this->delegate_track = new BugsTrackDelegate(SECTION_FOR_MORE_POPUP___track, BugsTrackDelegate::Bugs_Track_General, this);
        connect(delegate_track, &BugsTrackDelegate::clicked, this, &BugsMyLikeMusic::slot_clickedItemTrack_inList);
        this->listWidget_track = this->get_UIControl_ListWidget(this->delegate_track);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,0,0,30);
        tmp_vBox->addWidget(widget_playBtn, 0, Qt::AlignLeft);
        tmp_vBox->addWidget(this->listWidget_track, 1, Qt::AlignTop);

        this->widget_inScroll_track = new QWidget();
        this->widget_inScroll_track->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_track = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_track->setWidget(this->widget_inScroll_track);

        this->stackedWidget_content->addWidget(this->scrollArea_track);
    }


    /**
     * @brief BugsMyLikeMusic::setUIControl_body_Album
     */
    void BugsMyLikeMusic::setUIControl_body_Album(){

        // 재생버튼 관련 - album 용으로 :: 없음

        // Album
        this->flowLayout_album = this->get_UIControl_flowLayout(0, 20);

        QWidget *widget_forFlow = new QWidget();
        widget_forFlow->setLayout(this->flowLayout_album);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,10,0,30);
        tmp_vBox->addWidget(widget_forFlow, 1, Qt::AlignTop);


        this->widget_inScroll_album = new QWidget();
        this->widget_inScroll_album->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_album = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_album->setWidget(this->widget_inScroll_album);

        this->stackedWidget_content->addWidget(this->scrollArea_album);
    }


    /**
     * @brief BugsMyLikeMusic::setUIControl_body_Artist
     */
    void BugsMyLikeMusic::setUIControl_body_Artist(){

        // 재생버튼 관련 - Artist 용으로 : 없음

        // Artist
        this->delegate_artist = new BugsArtistDelegate(SECTION_FOR_MORE_POPUP___artist, BugsTrackDelegate::Bugs_Artist_General, this);
        connect(delegate_artist, &BugsTrackDelegate::clicked, this, &BugsMyLikeMusic::slot_clickedItemTrack_inList);
        this->listWidget_artist = this->get_UIControl_ListWidget(this->delegate_artist);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,10,0,30);
        tmp_vBox->addWidget(this->listWidget_artist, 1, Qt::AlignTop);

        this->widget_inScroll_artist = new QWidget();
        this->widget_inScroll_artist->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_artist = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_artist->setWidget(this->widget_inScroll_artist);

        this->stackedWidget_content->addWidget(this->scrollArea_artist);

    }


    /**
     * @brief BugsMyLikeMusic::setUIControl_body_PD_Album
     */
    void BugsMyLikeMusic::setUIControl_body_PD_Album(){

        // 재생버튼 관련 - PD's album 용으로 :: 없음

        // Album
        this->flowLayout_pd_album = this->get_UIControl_flowLayout(0, 20);

        QWidget *widget_forFlow = new QWidget();
        widget_forFlow->setLayout(this->flowLayout_pd_album);

        QVBoxLayout *tmp_vBox = new QVBoxLayout();
        tmp_vBox->setSpacing(0);
        tmp_vBox->setContentsMargins(0,10,0,30);
        tmp_vBox->addWidget(widget_forFlow, 1, Qt::AlignTop);


        this->widget_inScroll_pd_album = new QWidget();
        this->widget_inScroll_pd_album->setLayout(tmp_vBox);

        // scrollArea
        this->scrollArea_pd_album = this->get_UIControl_VerticalScrollArea();
        this->scrollArea_pd_album->setWidget(this->widget_inScroll_pd_album);

        this->stackedWidget_content->addWidget(this->scrollArea_pd_album);
    }



    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsMyLikeMusic::resizeEvent(QResizeEvent *event){
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
     * @brief Track 정보를 요청 (Bugs API)
     */
    void BugsMyLikeMusic::request_more_trackData(){
        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            // page number
            int page_reqNext = this->list_track->size() / GET_ITEM_SIZE___ONCE + 1;

            // getting track list - request HTTP API
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcBugsAPI::BugsMyMusic_Sort sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Recent;      // 기본값
            if(selected_sortOpt == QVariant::fromValue(ProcBugsAPI::BugsMyMusic_Sort::Name).toString()){
                sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Name;
            }

            ProcBugsAPI *proc_track = new ProcBugsAPI(this);
            proc_track->setProperty("section", SECTION_FOR_MORE_POPUP___track);
            connect(proc_track, &ProcBugsAPI::completeReq_list_tracks, this, &BugsMyLikeMusic::slot_applyResult_tracks);
            proc_track->request_bugs_getList_tracks_of_myMusic(GET_ITEM_SIZE___ONCE, page_reqNext, sortOpt);
        }
    }


    /**
     * @brief Album 정보를 요청 (Bugs API)
     */
    void BugsMyLikeMusic::request_more_albumData(){
        if(!this->flagReqMore_album && !this->flag_lastPage_album){
            this->flagReqMore_album = true;

            // page number
            int page_reqNext = this->list_album->size() / GET_ITEM_SIZE___ONCE + 1;

            // getting track list - request HTTP API
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcBugsAPI::BugsMyMusic_Sort sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Recent;      // 기본값
            if(selected_sortOpt == QVariant::fromValue(ProcBugsAPI::BugsMyMusic_Sort::Name).toString()){
                sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Name;
            }

            ProcBugsAPI *proc_album = new ProcBugsAPI(this);
            proc_album->setProperty("section", SECTION_FOR_MORE_POPUP___album);
            connect(proc_album, &ProcBugsAPI::completeReq_list_albums, this, &BugsMyLikeMusic::slot_applyResult_albums);
            proc_album->request_bugs_getList_albums_of_myMusic(GET_ITEM_SIZE___ONCE, page_reqNext, sortOpt);
        }
    }


    /**
     * @brief Artist 정보를 요청 (Bugs API)
     */
    void BugsMyLikeMusic::request_more_artistData(){
        if(!this->flagReqMore_artist && !this->flag_lastPage_artist){
            this->flagReqMore_artist = true;

            // page number
            int page_reqNext = this->list_artist->size() / GET_ITEM_SIZE___ONCE + 1;

            // getting track list - request HTTP API
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcBugsAPI::BugsMyMusic_Sort sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Recent;      // 기본값
            if(selected_sortOpt == QVariant::fromValue(ProcBugsAPI::BugsMyMusic_Sort::Name).toString()){
                sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Name;
            }

            ProcBugsAPI *proc_album = new ProcBugsAPI(this);
            proc_album->setProperty("section", SECTION_FOR_MORE_POPUP___artist);
            connect(proc_album, &ProcBugsAPI::completeReq_list_artists, this, &BugsMyLikeMusic::slot_applyResult_artists);
            proc_album->request_bugs_getList_artists_of_myMusic(GET_ITEM_SIZE___ONCE, page_reqNext, sortOpt);
        }
    }


    /**
     * @brief PD's Album 정보를 요청 (Bugs API)
     */
    void BugsMyLikeMusic::request_more_pd_albumData(){
        if(!this->flagReqMore_pd_album && !this->flag_lastPage_pd_album){
            this->flagReqMore_pd_album = true;

            // page number
            int page_reqNext = this->list_pd_album->size() / GET_ITEM_SIZE___ONCE + 1;

            // getting track list - request HTTP API
            QString selected_sortOpt = this->selected_filterCode.toString();
            ProcBugsAPI::BugsMyMusic_Sort sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Recent;      // 기본값
            if(selected_sortOpt == QVariant::fromValue(ProcBugsAPI::BugsMyMusic_Sort::Name).toString()){
                sortOpt = ProcBugsAPI::BugsMyMusic_Sort::Name;
            }

            ProcBugsAPI *proc = new ProcBugsAPI(this);
            proc->setProperty("section", SECTION_FOR_MORE_POPUP___pd_album);
            connect(proc, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsMyLikeMusic::slot_applyResult_pd_albums);
            proc->request_bugs_getList_pd_albums_of_myMusic(GET_ITEM_SIZE___ONCE, page_reqNext, sortOpt);
        }
    }




    /**
     * @brief BugsMusicRecently::proc_wheelEvent_to_getMoreData
     */
    void BugsMyLikeMusic::proc_wheelEvent_to_getMoreData(){
        if(this->selected_tabIndex == 0){
            this->request_more_trackData();
        }
        else if(this->selected_tabIndex == 1){
            this->request_more_albumData();
        }
        else if(this->selected_tabIndex == 2){
            this->request_more_artistData();
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
    void BugsMyLikeMusic::slot_filterClicked(){
        if(this->chooseFilterOpt->isVisible()){
            this->chooseFilterOpt->hideFilterBox();
        }
        else{
            // Filter Box Show
            this->chooseFilterOpt->clearFilter();
            this->chooseFilterOpt->addFilterGroup("정렬", this->list_filterOpt_sort, this->selected_filterCode);
            this->chooseFilterOpt->showFilterBox(QPoint(30, 130), this->size());
        }
    }


    /**
     * @brief 필터링 옵션박스 Widget에서 필터링을 하나 클릭한 경우이다.
     * @param selectedCode
     */
    void BugsMyLikeMusic::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

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
                this->scrollArea_artist->verticalScrollBar()->setValue(0);
                this->request_more_artistData();
            }
            else if(this->selected_tabIndex == 3){
                this->scrollArea_pd_album->verticalScrollBar()->setValue(0);
                this->request_more_pd_albumData();
            }

        }

    }


    /**
     * @brief tab index 변경마다 처리함
     * @param clickedIndex
     */
    void BugsMyLikeMusic::slot_segmentTag_clicked(int clickedIndex){
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
                // artist
                if(this->list_artist->length() == 0){
                    this->request_more_artistData();
                }
            }
            else if(clickedIndex == 3){
                // PD's album
                if(this->list_pd_album->length() == 0){
                    this->request_more_pd_albumData();
                }
            }

        }
    }





    /**
     * @brief "전체재생"  버튼 클릭
     */
    void BugsMyLikeMusic::slot_btnClicked_playAll(){
        if(this->selected_tabIndex == 0){
            this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
        }
    }



    /**
     * @brief BugsMusicRecently::slot_applyResult_tracks
     * @param list_data
     * @param jsonObj_track
     */
    void BugsMyLikeMusic::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){
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
    void BugsMyLikeMusic::slot_applyResult_albums(const QList<bugs::AlbumItemData>& list_data, const bool flag_lastPage){
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
     * @brief BugsMyLikeMusic::slot_applyResult_artists
     * @param list_data
     * @param flag_lastPage
     */
    void BugsMyLikeMusic::slot_applyResult_artists(const QList<bugs::ArtistItemData>& list_data, const bool flag_lastPage){
        this->flag_lastPage_artist = flag_lastPage;
        this->flagReqMore_artist = false;            // init
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___artist){
            int start_index = this->list_artist->size();
            this->list_artist->append(list_data);
            this->createList_itemArtistDelegate_applyingWithData_showImage(list_data, this->listWidget_artist, start_index, section, true);
        }
    }


    /**
     * @brief BugsMyLikeMusic::slot_applyResult_pd_albums
     * @param list_data
     * @param flag_lastPage
     */
    void BugsMyLikeMusic::slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>& list_data, const bool flag_lastPage){
        this->flagReqMore_pd_album = false;            // init (ready to get more data)
        this->flag_lastPage_pd_album = flag_lastPage;

        int start_index = this->list_pd_album->length();
        this->list_pd_album->append(list_data);
        this->createList_itemPD_Album_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_186x186, this->flowLayout_pd_album, start_index, SECTION_FOR_MORE_POPUP___pd_album);
    }



    /**
     * @brief [Slot] ListWidgetItem 관련 - 썸네일 다운로드 완료
     */
    void BugsMyLikeMusic::slot_thumbnailDownloaded_fotListWidget_delegate(){
        FileDownloader *fileDownloader_sender = qobject_cast<FileDownloader*>(sender());
        int section = fileDownloader_sender->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->applyImageData_itemTrackDelegate_withFileDownload(fileDownloader_sender, this->listWidget_track, this->list_track);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artist){
            this->applyImageData_itemArtistDelegate_withFileDownload(fileDownloader_sender, this->listWidget_artist, this->list_artist);
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void BugsMyLikeMusic::slot_clickedItemTrack_inList(const int idx, const tidal::AbstractTidalTrackDelegate::ClickMode clickMode){
        int section = ((tidal::AbstractTidalTrackDelegate*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_itemArtist_inList(this->list_artist, clickMode, idx, section);
        }
    }


    /**
     * @brief BugsMusicRecently::slot_clickedItemAlbum
     */
    void BugsMyLikeMusic::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void BugsMyLikeMusic::slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPD_Album(this->list_pd_album, clickMode, index, section);
    }



    /**
     * @brief BugsMyLikeMusic::proc_preUpdate_favorite_track
     * @param data_itemPos
     */
    void BugsMyLikeMusic::proc_preUpdate_favorite_track(const ItemPositionData& data_itemPos){
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
     * @brief BugsMyLikeMusic::proc_preUpdate_favorite_album
     * @param data_itemPos
     */
    void BugsMyLikeMusic::proc_preUpdate_favorite_album(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___album){
            // Update Data
            bugs::AlbumItemData data_album = this->list_album->at(data_itemPos.index);
            data_album.likes_yn = data_itemPos.likes_yn;
            this->list_album->replace(data_itemPos.index, data_album);
        }
    }


    /**
     * @brief BugsMyLikeMusic::proc_preUpdate_favorite_artist
     * @param data_itemPos
     */
    void BugsMyLikeMusic::proc_preUpdate_favorite_artist(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___artist){
            // Update Data
            bugs::ArtistItemData data_artist = this->list_artist->at(data_itemPos.index);
            data_artist.likes_yn = data_itemPos.likes_yn;
            this->list_artist->replace(data_itemPos.index, data_artist);

            // Update ListWidget
            this->listWidget_artist->item(data_itemPos.index)->setData(Qt::UserRole, ConvertData_forBugs::getObjectJson_artistData(data_artist));
        }
    }


    /**
     * @brief BugsMyLikeMusic::proc_preUpdate_favorite_pd_album
     * @param data_itemPos
     */
    void BugsMyLikeMusic::proc_preUpdate_favorite_pd_album(const ItemPositionData& data_itemPos){
        // Update Data
        bugs::PD_AlbumItemData data_pd_album = this->list_pd_album->at(data_itemPos.index);
        data_pd_album.likes_yn = data_itemPos.likes_yn;
        this->list_pd_album->replace(data_itemPos.index, data_pd_album);
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
    void BugsMyLikeMusic::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___album){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___pd_album){
            this->proc_clicked_optMorePopup_fromPD_Album(this->list_pd_album, index, clickMode);
        }
    }



}
