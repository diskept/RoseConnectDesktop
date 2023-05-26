#include "BugsTrackListAll_ofArtist.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"

#include "widget/NoData_Widget.h"

#include <QSettings>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPixmapCache>
#include <QPainterPath>


namespace bugs {


    const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";
    const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";
    const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
    const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___track = 2;


    const int GET_ITEM_SIZE___ONCE = 100;


    /**
     * @brief
     * @param parent
     */
    BugsTrackListAll_ofArtist::BugsTrackListAll_ofArtist(QWidget *parent) : AbstractBugsSubWidget(MainUIType::VerticalScroll_rosefilter, parent) {
        // data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_filterOpt_sort = AbstractBugsSubWidget::get_list_filterOptData_forMyCollection();

        if(global.user_forBugs.isLogined() == true){
            // UI
            this->setUIControl_tracks();
        }
    }
    /**
     * @brief 소멸자.
     */
    BugsTrackListAll_ofArtist::~BugsTrackListAll_ofArtist(){

        this->deleteLater();
    }



    /**
     * @brief
     * @param jsonObj
     */
    void BugsTrackListAll_ofArtist::setJsonObject_forData(const QJsonObject &jsonObj){

        bugs::PageInfo_AlbumAllView_ofArtist data_param = ConvertData_forBugs::convertData_pageInfo_albumAllView_ofArtist(jsonObj);
        QString tmp_filterOpt = this->filterOpt;
        int tmp_current_id = this->artist_id;

        this->flagNeedReload = false;

        if((tmp_filterOpt != data_param.filterOpt) || (tmp_current_id != data_param.artist_id)){
            this->flagNeedReload = true;

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);
                GSCommon::clearLayout(this->box_contents);

                this->box_contents->setAlignment(Qt::AlignTop);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                this->flag_widget_visible = false;
            }

            this->label_mainTitle->setText(data_param.mainTitle);

            this->filterOpt = data_param.filterOpt;
            this->artist_id = data_param.artist_id;

            this->list_track->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();


            this->track_id_fav = 0;
            this->track_idx_fav = 0;


            this->track_totalCount = 0;
            this->track_drawCount = 0;
            this->track_favoriteOffset = 0;

            this->flag_draw = false;
            this->flag_check_track = false;
            this->flag_track_fav = false;
            this->flag_send_track = false;

            // request HTTP API
            this->flagReqMore_track = false;
            this->flag_lastPage_track = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
    */
    void BugsTrackListAll_ofArtist::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            /*// init
            this->chooseFilterOpt->hideFilterBox();

            // init Data
            this->selected_filterCode = this->list_filterOpt_sort.first().opt_code;         // default

            QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
            QString def_name = this->list_filterOpt_sort.first().opt_name;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(def_code, def_name);*/

            this->box_filter->removeWidget(this->widget_filter_contents);

            this->widget_filter_contents = new QWidget();
            this->widget_filter_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_filter_contents->setFixedSize(1500, 50);

            int width = 0;
            int left = 0;
            // All
            QPixmap pixmap_play(":/images/icon_list_allplay.png");
            QPushButton *btn_playAll = new QPushButton(this->widget_filter_contents);
            btn_playAll->setIcon(pixmap_play);
            btn_playAll->setIconSize(pixmap_play.rect().size());
            btn_playAll->setText("Play All");
            btn_playAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_playAll->setCursor(Qt::PointingHandCursor);

            width = btn_playAll->sizeHint().width();
            btn_playAll->setGeometry(left, 0, width, 50);
            connect(btn_playAll, &QPushButton::clicked, this, &BugsTrackListAll_ofArtist::slot_btnClicked_playAll);

            // shuffle
            QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
            QPushButton *btn_shuffleAll = new QPushButton(this->widget_filter_contents);
            btn_shuffleAll->setIcon(pixmap_shuffle);
            btn_shuffleAll->setIconSize(pixmap_shuffle.rect().size());
            btn_shuffleAll->setText("Shuffle Play");
            btn_shuffleAll->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            btn_shuffleAll->setCursor(Qt::PointingHandCursor);

            left = width + 30;
            width = btn_shuffleAll->sizeHint().width();
            btn_shuffleAll->setGeometry(left, 0, width, 50);
            connect(btn_shuffleAll, &QPushButton::clicked, this, &BugsTrackListAll_ofArtist::slot_btnClicked_playShuffle);

            this->box_filter->addWidget(this->widget_filter_contents);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);
            this->box_main_contents->setAlignment(Qt::AlignTop);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents);

            this->listWidget_contents = new QListWidget(this);
            this->listWidget_contents->setCursor(Qt::PointingHandCursor);
            this->listWidget_contents->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            this->listWidget_contents->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            this->listWidget_contents->setStyleSheet("background-color:transparent;");

            connect(this->listWidget_contents, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));

            this->box_main_contents->addWidget(this->listWidget_contents);
        }
    }


    // MARK : Create UI and Set --------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void BugsTrackListAll_ofArtist::setUIControl_tracks(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Tracks");

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // filtering
        //this->setUiControl_filter();

        // filter option box
        this->chooseFilterOpt = new bugs::BugsChooseFilterOpt(this);
        connect(this->chooseFilterOpt, &bugs::BugsChooseFilterOpt::signal_clicked_filterOpt, this, &BugsTrackListAll_ofArtist::slot_chooseFilterOption);
        this->chooseFilterOpt->hideFilterBox();
    }


    /**
     * @brief QobuzMyAlbumAll::setUiControl_filter
     */
    void BugsTrackListAll_ofArtist::setUiControl_filter(){

        // 필터링 부분
        this->filterWidget = new FilterWidget();
        QString def_code = this->list_filterOpt_sort.first().opt_code.toString();
        QString def_name = this->list_filterOpt_sort.first().opt_name;
        this->filterWidget->add_filter(def_code, def_name);
        connect(this->filterWidget, &FilterWidget::signal_clickedFilter, this, &BugsTrackListAll_ofArtist::slot_filterClicked);


        QHBoxLayout *hBox_filter = new QHBoxLayout();
        hBox_filter->setSpacing(0);
        hBox_filter->setContentsMargins(0,0,0,0);
        hBox_filter->addWidget(this->filterWidget, 0, Qt::AlignVCenter | Qt::AlignLeft);
        hBox_filter->addStretch(1);
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
     * @brief 데이터 , UI 초기화
     */
    void BugsTrackListAll_ofArtist::initAll(){

        // init
        this->list_track->clear();
        this->jsonArr_tracks_toPlay = QJsonArray();

        this->track_id_fav = 0;
        this->track_idx_fav = 0;

        this->track_totalCount = 0;
        this->track_drawCount = 0;
        this->track_favoriteOffset = 0;

        this->flag_draw = false;
        this->flag_check_track = false;
        this->flag_track_fav = false;
        this->flag_send_track = false;

        // request HTTP API
        this->flagReqMore_track = false;
        this->flag_lastPage_track = false;

        this->widget_main_contents->hide();
        this->box_contents->removeWidget(this->widget_main_contents);
        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->box_main_contents = new QVBoxLayout();
        this->box_main_contents->setSpacing(0);
        this->box_main_contents->setContentsMargins(0, 0, 0, 0);
        this->box_main_contents->setAlignment(Qt::AlignTop);

        this->widget_main_contents = new QWidget();
        this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_main_contents->setLayout(this->box_main_contents);

        this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void BugsTrackListAll_ofArtist::proc_wheelEvent_to_getMoreData(){

        if((this->track_totalCount > this->track_drawCount) && (this->flag_draw == false) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())
                && (this->before_scrollMax != this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;
            this->before_scrollMax = this->scrollArea_main->verticalScrollBar()->maximum();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_trackDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void BugsTrackListAll_ofArtist::resizeEvent(QResizeEvent *event){

        AbstractBugsSubWidget::resizeEvent(event);

        // BugsChooseFilterOpt 위젯의 사이즈를 업데이트 해줘야함
        if(this->chooseFilterOpt != nullptr){
            this->chooseFilterOpt->setFixedSize(event->size());
        }
    }


    // MARK : about Filter Option Box ---------------------------------------------------------------------------------

    /**
     * @brief 필터 아이콘 버튼 클릭에 대한 처리 - 필터링 옵션박스 Widget을 띄운다.
     */
    void BugsTrackListAll_ofArtist::slot_filterClicked(){

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
    void BugsTrackListAll_ofArtist::slot_chooseFilterOption(QVariant selected_filterCode, QString selected_filterName){

        // 코드가 다른 경우에 대해서만 처리한다.
        if(this->selected_filterCode != selected_filterCode){

            // update
            this->selected_filterCode = selected_filterCode;
            this->filterWidget->clear_filter();
            this->filterWidget->add_filter(selected_filterCode.toString(), selected_filterName);

            // clear - 옵션코드가 바뀌었으므로 모두 초기화한다.
            this->initAll();

            this->scrollTop();
            this->request_more_trackData();

        }

    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void BugsTrackListAll_ofArtist::request_more_trackData(){
        if(!this->flagReqMore_track && !this->flag_lastPage_track){
            this->flagReqMore_track = true;

            int page_reqNext = (this->list_track->size() / GET_ITEM_SIZE___ONCE) + 1;


            ProcBugsAPI::BugsArtist_TrackFilter tmp_filter = ProcBugsAPI::BugsArtist_TrackFilter::Release;          // 기본값
            if(this->filterOpt == QVariant::fromValue(ProcBugsAPI::BugsArtist_TrackFilter::All).toString()){
                tmp_filter = ProcBugsAPI::BugsArtist_TrackFilter::All;
            }
            else if(this->filterOpt == QVariant::fromValue(ProcBugsAPI::BugsArtist_TrackFilter::Join).toString()){
                tmp_filter = ProcBugsAPI::BugsArtist_TrackFilter::Join;
            }

            ProcBugsAPI::BugsAritst_TrackSort tmp_sort = ProcBugsAPI::BugsAritst_TrackSort::Popular;          // 기본값
            if(this->filterOpt == QVariant::fromValue(ProcBugsAPI::BugsAritst_TrackSort::Recent).toString()){
                tmp_sort = ProcBugsAPI::BugsAritst_TrackSort::Recent;
            }
            else if(this->filterOpt == QVariant::fromValue(ProcBugsAPI::BugsAritst_TrackSort::Word).toString()){
                tmp_sort = ProcBugsAPI::BugsAritst_TrackSort::Word;
            }

            // getting album list - request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_tracks, this, &BugsTrackListAll_ofArtist::slot_applyResult_track_ofArtist);
            proc->request_bugs_getList_tracks_of_artist(this->artist_id, GET_ITEM_SIZE___ONCE, page_reqNext, tmp_filter, tmp_sort);
            proc->setProperty("artistID", this->artist_id);
       }
    }



    void BugsTrackListAll_ofArtist::request_more_trackDraw(){

        int start_index = this->track_drawCount;
        int max_cnt = ((this->track_totalCount - this->track_drawCount) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->track_totalCount - this->track_drawCount);
        this->track_drawCount += max_cnt;

        QList<QString> tmp_clientkey;
        this->track_favoriteOffset = start_index;
        for(int i = start_index; i < this->track_drawCount; i++){
            tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

            int height = this->track_drawCount * 70;
            this->listWidget_contents->setFixedHeight(height);

            this->iniDataInfo_RoseList_Item(this->list_track->at(i), i);
            this->setDataInfo_RoseList_Item(this->list_track->at(i), i);
        }

        // request HTTP API - get favorite for Rose Server
        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTrackListAll_ofArtist::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

        if(this->track_totalCount == this->track_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void BugsTrackListAll_ofArtist::slot_applyResult_track_ofArtist(const QList<bugs::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            int artistID = sender()->property("artistID").toInt();

            if(artistID != this->artist_id){
                return;
            }

            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();      //cheon Tidal

                return;
            }

            if(this->list_track->count() == 0){
                this->jsonArr_tracks_toPlay = QJsonArray();
                this->track_totalCount = list_data.at(0).total_count;
            }

            int start_index = this->list_track->count();

            this->list_track->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(start_index == 0){
                int max_cnt = this->list_track->size();
                this->track_drawCount = max_cnt;

                QList<QString> tmp_clientkey;
                this->track_favoriteOffset = start_index;
                for(int i = start_index; i < this->track_drawCount; i++){
                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));

                    int height = this->track_drawCount * 70;
                    this->listWidget_contents->setFixedHeight(height);

                    this->iniDataInfo_RoseList_Item(this->list_track->at(i), i);
                    this->setDataInfo_RoseList_Item(this->list_track->at(i), i);

                    QCoreApplication::processEvents();
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTrackListAll_ofArtist::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);

                ContentLoadingwaitingMsgHide();      //cheon Tidal
                this->flag_widget_visible = true;

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
                else{
                    this->box_main_contents->addSpacing(30);
                }
            }
            else{

                if(this->track_totalCount > this->list_track->count()){
                    this->request_more_trackData();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();      //cheon Tidal

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->box_main_contents->addWidget(noData_widget);
        }
    }

    inline void BugsTrackListAll_ofArtist::iniDataInfo_RoseList_Item(const bugs::TrackItemData &data_track, const int &p_index){

        QString thumbnail = data_track.album_image;

        int id = data_track.track_id;

        QWidget *widget_track_info = new QWidget();
        widget_track_info->setFixedSize(1550, 70);
        widget_track_info->setObjectName("widget_track_info");
        widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                            "#widget_track_info:hover{background-color:#333333;} ");

        QString default_path = ":/images/bugs/bugs_def_430x2.png";

        QImage img_album;
        QPixmap pixmap_image;
        if(img_album.load(default_path)){
            QPixmap pixmapIMG = QPixmap(QSize(60, 60));
            pixmapIMG.fill(Qt::black);

            pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
            pixmap_image = pixmap_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            path.addRoundedRect(0, 0, 60, 60, 2, 2);

            int leftValue = (60 - pixmap_image.width()) / 2;
            int topValue = (60 - pixmap_image.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, pixmap_image);
            painter.end();
        }

        this->lb_thumb[p_index] = new QLabel(widget_track_info);
        this->lb_thumb[p_index]->setFixedSize(60, 60);
        this->lb_thumb[p_index]->setGeometry(5, 5, 0, 0);
        this->lb_thumb[p_index]->setStyleSheet("background-color:transparent;");
        this->lb_thumb[p_index]->setPixmap(pixmap_image);

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", p_index);
        fileDownloader->setImageURL(thumbnail);
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_list()));

        this->lb_explicit[p_index] = new QLabel(widget_track_info);
        this->lb_explicit[p_index]->setFixedSize(30, 30);
        this->lb_explicit[p_index]->setGeometry(180, 20, 0, 0);
        this->lb_explicit[p_index]->hide();

        this->lb_title[p_index] = new QLabel(widget_track_info);
        this->lb_title[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        this->lb_title[p_index]->setGeometry(180, 10, 720, 49);

        this->lb_album[p_index] = new QLabel(widget_track_info);
        this->lb_album[p_index]->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        this->lb_album[p_index]->setWordWrap(true);
        this->lb_album[p_index]->setGeometry(937, 14, 300, 40);

        this->lb_resolution[p_index] = new QLabel(widget_track_info);
        this->lb_resolution[p_index]->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        this->lb_resolution[p_index]->setGeometry(900, 10, 20, 49);
        this->lb_resolution[p_index]->hide();

        this->lb_artist[p_index] = new QLabel(widget_track_info);
        this->lb_artist[p_index]->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        this->lb_artist[p_index]->setGeometry(180, 34, 680, 49);

        this->lb_duration[p_index] = new QLabel(widget_track_info);
        this->lb_duration[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        this->lb_duration[p_index]->setGeometry(1231, 24, 65, 20);

        QPushButton *btn_video_list = GSCommon::getUIBtnImg("btn_video_list", ":/images/list_video_ico.png", widget_track_info);
        btn_video_list->setGeometry(1350, 10, 50, 50);
        btn_video_list->setCursor(Qt::PointingHandCursor);
        btn_video_list->setProperty("idx", p_index);
        btn_video_list->hide();

        connect(btn_video_list, &QPushButton::clicked, this, &BugsTrackListAll_ofArtist::slot_clicked_btnVideolist);

        if(data_track.list_mv_id.count() > 0){
            btn_video_list->show();
        }

        this->btn_list_fav[p_index] = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, widget_track_info);
        this->btn_list_fav[p_index]->setGeometry(1400, 6, 50, 58);
        this->btn_list_fav[p_index]->setCursor(Qt::PointingHandCursor);
        this->btn_list_fav[p_index]->setProperty("idx", p_index);

        connect(this->btn_list_fav[p_index], &QPushButton::clicked, this, &BugsTrackListAll_ofArtist::slot_clicked_btnFavorite);

        this->track_star_fav[p_index] = 0;

        QPushButton *btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", widget_track_info);
        btn_menu->setGeometry(1450, 6, 50, 58);
        btn_menu->setCursor(Qt::PointingHandCursor);
        btn_menu->setProperty("idx", p_index);

        connect(btn_menu, &QPushButton::clicked, this, &BugsTrackListAll_ofArtist::slot_clicked_btnMenu);

        QString tmpProperty = QString("IDX:%1,").arg(p_index) + QString("ID:%1").arg(id);

        QListWidgetItem *item = new QListWidgetItem();
        item->setWhatsThis(tmpProperty);
        item->setSizeHint(QSize(90, 70));

        this->listWidget_contents->addItem(item);

        this->listWidget_contents->setItemWidget(item, widget_track_info);
    }


    void BugsTrackListAll_ofArtist::setDataInfo_RoseList_Item(const bugs::TrackItemData &data_track, const int &p_index){

        QString title = data_track.track_title;
        QString artist = data_track.list_artist_nm.join(",");;
        QString album  = data_track.album_title;
        QString bitrates = data_track.bitrates;
        QString duration = data_track.len;

        bool explicit_flag = data_track.adult_yn;

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        tmp_title->setText(title);

        int title_width = 0;
        int title_width_resize = 0;
        title_width = tmp_title->sizeHint().width();

        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        int width = 0;
        int left = 0;
        if(explicit_flag == true){
            QString adult_certification_path = "";

            if(global.lang == 0){
                adult_certification_path = ":/images/tidal/tidal-e-ico.png";
            }
            else{
                adult_certification_path = ":/images/bugs/ico-19.png";
            }

            QImage img;
            QPixmap *img_adult_certification = new QPixmap();
            if(img.load(adult_certification_path)){
                *img_adult_certification = QPixmap::fromImage(img);
                *img_adult_certification = img_adult_certification->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            this->lb_explicit[p_index]->setPixmap(*img_adult_certification);
            this->lb_explicit[p_index]->setFixedSize(30, 30);
            this->lb_explicit[p_index]->setGeometry(85, 20, 0, 0);
            this->lb_explicit[p_index]->show();

            if(!bitrates.isEmpty()){
                this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                title_width += (15 + width);

                if(title_width > 680){
                    title_width_resize = 680 - (15 + width);
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(135, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width);
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(135, 10, title_width_resize, 25);
                }

                left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                this->lb_resolution[p_index]->show();
            }
            else{
                if(title_width > 680){
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 680, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(135, 10, 680, 25);
                }
                else{
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(135, 10, title_width, 25);
                }
            }

            if(artist_width > 640){
                this->lb_artist[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 640, this->lb_artist[p_index]->font()));
                if(this->lb_artist[p_index]->text().contains("…")){
                    this->lb_artist[p_index]->setToolTip(artist);
                    this->lb_artist[p_index]->setToolTipDuration(2000);
                }
                this->lb_artist[p_index]->setGeometry(135, 34, 640, 25);
            }
            else{
                this->lb_artist[p_index]->setText(artist);
                this->lb_artist[p_index]->setGeometry(135, 34, artist_width, 25);
            }
        }
        else{
            if(!bitrates.isEmpty()){
                this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                title_width += (15 + width);

                if(title_width > 720){
                    title_width_resize = 720 - (15 + width);
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(85, 10, title_width_resize, 25);
                }
                else{
                    title_width_resize = title_width - (15 + width);
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(85, 10, title_width_resize, 25);
                }

                left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                this->lb_resolution[p_index]->show();
            }
            else{
                if(title_width > 720){
                    this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->lb_title[p_index]->font()));
                    if(this->lb_title[p_index]->text().contains("…")){
                        this->lb_title[p_index]->setToolTip(title);
                        this->lb_title[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_title[p_index]->setGeometry(85, 10, 720, 25);
                }
                else{
                    this->lb_title[p_index]->setText(title);
                    this->lb_title[p_index]->setGeometry(85, 10, title_width, 25);
                }
            }

            if(artist_width > 680){
                this->lb_artist[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 680, this->lb_artist[p_index]->font()));
                if(this->lb_artist[p_index]->text().contains("…")){
                    this->lb_artist[p_index]->setToolTip(artist);
                    this->lb_artist[p_index]->setToolTipDuration(2000);
                }
                this->lb_artist[p_index]->setGeometry(85, 34, 680, 25);
            }
            else{
                this->lb_artist[p_index]->setText(artist);
                this->lb_artist[p_index]->setGeometry(85, 34, artist_width, 25);
            }
        }

        QLabel *tmp_album = new QLabel();
        tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        tmp_album->setText(album);

        int album_title_width = 0;
        album_title_width = tmp_album->sizeHint().width();

        if(album_title_width > 220){
            QString tmp_split = "";
            QStringList splitToken;
            QString tmp_album_set = "";

            tmp_split = album;
            splitToken = tmp_split.split(" ");

            tmp_album->setText("");
            int i = 0;
            if(splitToken.size() > 1){

                for(i = 0; i < splitToken.count(); i++){
                    if(i == 0){
                        tmp_album_set = splitToken.at(i);
                    }
                    else{
                        tmp_album_set += " " + splitToken.at(i);
                    }
                    tmp_album->setText(tmp_album_set);

                    if(tmp_album->sizeHint().width() >= 220){
                        tmp_album->setText("");
                        tmp_album_set.replace(splitToken.at(i), "");
                        break;
                    }
                }
            }

            tmp_album->setText("");
            tmp_album->setText(tmp_album_set);

            album_title_width = tmp_album->sizeHint().width() + 220;

            QString strAlbum = album;
            QLabel *tmpAlbum = new QLabel();
            tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
            tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

            int tmpAlbum_width = tmpAlbum->sizeHint().width();

            if(tmpAlbum_width >= 220){
                this->lb_album[p_index]->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, this->lb_album[p_index]->font()));
                if(this->lb_album[p_index]->text().contains("…")){
                    this->lb_album[p_index]->setToolTip(album);
                    this->lb_album[p_index]->setToolTipDuration(2000);
                }
            }
            else{
                this->lb_album[p_index]->setText(album);
            }
            this->lb_album[p_index]->setGeometry(937, 14, 220, 40);
        }
        else{
            this->lb_album[p_index]->setText(album);
            this->lb_album[p_index]->setGeometry(937, 24, 220, 20);
        }

        this->lb_duration[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        this->lb_duration[p_index]->setGeometry(1231, 24, 65, 20);
        this->lb_duration[p_index]->setText(duration);

    }

    void BugsTrackListAll_ofArtist::slot_listwidgetItemPressed(QListWidgetItem *item){

        QString tmpStr = item->whatsThis();

        QStringList strSplit = tmpStr.split(",");
        QStringList split_idx = strSplit.at(0).split(":");

        int idx = split_idx.at(1).toInt();

        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::AllBox, idx, SECTION_FOR_MORE_POPUP___track);
    }


    void BugsTrackListAll_ofArtist::slot_clicked_btnVideolist(){

        int idx = sender()->property("idx").toInt();

        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::MusicVideoBtn, idx, SECTION_FOR_MORE_POPUP___track);
    }


    void BugsTrackListAll_ofArtist::slot_clicked_btnFavorite(){

        int idx = sender()->property("idx").toInt();

        if(this->flag_check_track == false){
            if(this->track_star_fav[idx] == 3){
                this->track_star_fav[idx] = 0;

                this->track_idx_fav = idx;
                this->flag_track_fav = false;
            }
            else if(this->track_star_fav[idx] >= 0 && this->track_star_fav[idx] < 3){
                this->track_star_fav[idx]++;

                this->track_idx_fav = idx;
                this->flag_track_fav = true;
            }

            if(this->track_star_fav[idx] == 0 || this->track_star_fav[idx] == 1){
                // Bugs Favorite
                ItemPositionData itemPosData;
                itemPosData.section = SECTION_FOR_MORE_POPUP___track;
                itemPosData.index = idx;
                itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                ProcBugsAPI *proc = new ProcBugsAPI(this);
                connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsTrackListAll_ofArtist::slot_bugs_completeReq_listAll_myFavoritesIds);

                if(this->track_star_fav[idx] == 0){
                    itemPosData.likes_yn = false;

                    proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }
                else if(this->track_star_fav[idx] == 1){
                    itemPosData.likes_yn = true;

                    proc->request_bugs_addFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                }

                this->flag_send_track = true;
            }

            this->track_idx_fav = idx;

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
            connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTrackListAll_ofArtist::slot_applyResult_checkRating_track);
            proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

            this->flag_check_track = true;
        }
    }


    void BugsTrackListAll_ofArtist::slot_clicked_btnMenu(){

        int idx = sender()->property("idx").toInt();

        QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

        this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn, idx, SECTION_FOR_MORE_POPUP___track);
    }

    void BugsTrackListAll_ofArtist::slot_thumbnailDownload_list(){

        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();

        if(tmp_index >= 0 && tmp_index < this->list_track->size()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
            if(flagLoaded){
                QPixmap pixmapIMG = QPixmap(QSize(60, 60));
                pixmapIMG.fill(Qt::black);

                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                QPainter painter (&pixmapIMG);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
                painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

                QPainterPath path = QPainterPath();
                path.addRoundedRect(0, 0, 60, 60, 2, 2);

                int leftValue = (60 - tmp_pixmap.width()) / 2;
                int topValue = (60 - tmp_pixmap.height()) / 2;

                painter.setClipPath(path);
                painter.drawPixmap(leftValue, topValue, tmp_pixmap);
                painter.end();

                this->lb_thumb[tmp_index]->setPixmap(pixmapIMG);
            }
        }

        fileDownloader->deleteLater();
    }

    void BugsTrackListAll_ofArtist::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌.
        int idx = ProcJsonEasy::getInt(p_jsonObj, "index");
        int track_id = ProcJsonEasy::getString(p_jsonObj, "data_id").toInt();

        if(track_id == this->list_track->at(idx).track_id){
            qDebug() << "likes_yn : " << ProcJsonEasy::getBool(p_jsonObj, "likes_yn");
        }

        this->flag_send_track = false;
    }

    void BugsTrackListAll_ofArtist::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav[this->track_idx_fav]);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", "BUGS");

                QJsonObject track;
                track.insert("duration", 0);
                track.insert("favorite", false);
                track.insert("id", id);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 0);
                track.insert("type", "BUGS");

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsTrackListAll_ofArtist::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav[this->track_idx_fav]);
            }
            else{
                QJsonObject data = this->jsonArr_tracks_toPlay[this->track_idx_fav].toObject();

                QJsonObject track;
                track.insert("clientKey", ProcJsonEasy::getInt(data, "track_id"));
                track.insert("data", data);
                track.insert("duration", 0);
                track.insert("favorite", true);
                track.insert("ownerId", 0);
                track.insert("sort", 0);
                track.insert("star", 1);
                track.insert("thumbnailUrl", this->list_track->at(this->track_idx_fav).album_image);
                track.insert("title", ProcJsonEasy::getString(data, "track_title"));
                track.insert("type", "BUGS");

                QJsonArray tracks;
                tracks.append(track);

                QJsonObject json;
                json.insert("tracks", tracks);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsTrackListAll_ofArtist::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsTrackListAll_ofArtist::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");

                    this->track_star_fav[this->track_favoriteOffset + i] = star;

                    if(this->track_star_fav[this->track_favoriteOffset + i] > 0){
                        switch (this->track_star_fav[this->track_favoriteOffset + i]) {
                            case 1:
                                this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                            case 2:
                                this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                            case 3:
                                this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        }
                    }
                    else{
                        this->btn_list_fav[this->track_favoriteOffset + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                      ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                    }
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                if(this->track_star_fav[this->track_idx_fav] > 0){
                    switch (this->track_star_fav[this->track_idx_fav]) {
                        case 1:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        case 2:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        case 3:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    }
                }
                else{
                    this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }

    void BugsTrackListAll_ofArtist::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            if(jsonObj.contains("message") && (jsonObj["message"].toString() == "정상")){
                if(this->track_star_fav[this->track_idx_fav] > 0){
                    switch (this->track_star_fav[this->track_idx_fav]) {
                    case 1:
                        this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                               ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    case 2:
                        this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                               ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    case 3:
                        this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                               ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    }
                }
                else{
                    this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                                           ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsTrackListAll_ofArtist::slot_btnClicked_playAll(){

        // 전체재생
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, false);
    }


    void BugsTrackListAll_ofArtist::slot_btnClicked_playShuffle(){

        // 셔플재생
        this->proc_play_allTrack(this->jsonArr_tracks_toPlay, true);
    }



    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsTrackListAll_ofArtist::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }




}
