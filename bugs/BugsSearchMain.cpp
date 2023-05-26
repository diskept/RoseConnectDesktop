#include "bugs/BugsSearchMain.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QDebug>
#include <QScroller>

namespace bugs {

    const QString tmp_btnStyle      = "padding:8px;border:1px solid #707070;color:#CCCCCC;font-size:16px;";//cheon211008
    //const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01
    const QString tmp_btnStyleHover = "background-color:#CCCCCC;color:#FFFFFF;";//cheon211115-01
    const QString tmp_btnStyleHover_selected = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01


    const QString SORT_STEP_MUSIC = "music";//cheon211008
    const QString SORT_STEP_RoseTube = "rosetube";//cheon211008
    const QString SORT_STEP_RoseRadio = "roseradio";//cheon211008
    const QString SORT_STEP_TIDAL = "tidal";//cheon211008
    const QString SORT_STEP_BUGS = "bugs";//cheon211008
    const QString SORT_STEP_QOBUZ = "qobuz";//cheon211008

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX___search_track = 1;
    const int BTN_IDX___search_artist = 2;
    const int BTN_IDX___search_album = 3;
    const int BTN_IDX___search_pd_album = 4;
    const int BTN_IDX___search_video = 5;

    const int SECTION_FOR_MORE_POPUP___search_track = 1;
    const int SECTION_FOR_MORE_POPUP___search_artist = 2;
    const int SECTION_FOR_MORE_POPUP___search_album = 3;
    const int SECTION_FOR_MORE_POPUP___search_pd_album = 4;
    const int SECTION_FOR_MORE_POPUP___search_video = 5;


    /**
     * @brief 생성자
     * @param parent
     */
    BugsSearchMain::BugsSearchMain(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll_filter, parent) {

        // Data
        this->list_track = new QList<bugs::TrackItemData>();
        this->list_artist = new QList<bugs::ArtistItemData>();
        this->list_album = new QList<bugs::AlbumItemData>();
        this->list_pdAlbum = new QList<bugs::PD_AlbumItemData>();
        this->list_video = new QList<bugs::VideoItemData>();

        // 기본 UI 세팅
        this->setUIControl_basic();
    }


    /**
     * @brief 소멸자.
     */
    BugsSearchMain::~BugsSearchMain(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsSearchMain::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + "검색 결과");

            // init data
            this->list_track->clear();
            this->list_artist->clear();
            this->list_album->clear();
            this->list_pdAlbum->clear();
            this->list_video->clear();

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flag_artist[0] = false;
            this->flag_album[0] = false;
            this->flag_track[0] = false;
            this->flag_pdAlbum[0] = false;
            this->flag_video[0] = false;

            this->flag_artist[1] = false;
            this->flag_album[1] = false;
            this->flag_track[1] = false;
            this->flag_pdAlbum[1] = false;
            this->flag_video[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            ProcBugsAPI *proc = new ProcBugsAPI(this);
            connect(proc, &ProcBugsAPI::completeReq_list_tracks, this, &BugsSearchMain::slot_applyResult_tracks);
            proc->request_bugs_search_track(this->data_search.search_word, 1, 5, ProcBugsAPI::BugsSearch_Sort::exact);

            connect(proc, &ProcBugsAPI::completeReq_list_artists, this, &BugsSearchMain::slot_applyResult_artists);
            proc->request_bugs_search_artist(this->data_search.search_word, 1, 15, ProcBugsAPI::BugsSearch_Sort::exact);

            connect(proc, &ProcBugsAPI::completeReq_list_albums, this, &BugsSearchMain::slot_applyResult_albums);
            proc->request_bugs_search_album(this->data_search.search_word, 1, 15, ProcBugsAPI::BugsSearch_Sort::exact);

            connect(proc, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsSearchMain::slot_applyResult_pd_albums);
            proc->request_bugs_search_pd_album(this->data_search.search_word, 1, 15, ProcBugsAPI::BugsSearch_Sort::exact);

            connect(proc, &ProcBugsAPI::completeReq_list_videos, this, &BugsSearchMain::slot_applyResult_videos);
            proc->request_bugs_search_video(this->data_search.search_word, 1, 15, ProcBugsAPI::BugsSearch_Sort::exact);
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief bugsSearchMain::setActivePage
     */
    void BugsSearchMain::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            // j230407 Removing ghosting effect start
            if(this->widget_main_contents != nullptr){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);

                delete this->widget_main_contents;
            }
            // j230407 Removing ghosting effect finish

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // init UI
            this->vBox_artist = new QVBoxLayout();
            this->vBox_album = new QVBoxLayout();
            this->vBox_track = new QVBoxLayout();
            this->vBox_pdAlbum = new QVBoxLayout();
            this->vBox_video = new QVBoxLayout();

            this->hBox_artist = new QHBoxLayout();
            this->hBox_album = new QHBoxLayout();
            this->hBox_pdAlbum = new QHBoxLayout();
            this->hBox_video = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_artist);
            GSCommon::clearLayout(this->vBox_album);
            GSCommon::clearLayout(this->vBox_track);
            GSCommon::clearLayout(this->vBox_pdAlbum);
            GSCommon::clearLayout(this->vBox_video);

            GSCommon::clearLayout(this->hBox_artist);
            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_pdAlbum);
            GSCommon::clearLayout(this->hBox_video);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->search_artist[i] = new bugs::ItemArtist_bugs(i, SECTION_FOR_MORE_POPUP___search_artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_artist[i], &bugs::ItemArtist_bugs::signal_clicked, this, &BugsSearchMain::slot_clickedItemArtist);
            }

            for(int i = 0; i < 15; i++){
                this->search_album[i] = new bugs::ItemAlbum_bugs(i, SECTION_FOR_MORE_POPUP___search_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_album[i], &bugs::ItemAlbum_bugs::signal_clicked, this, &BugsSearchMain::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->search_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->search_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &BugsSearchMain::slot_clickedItemTrack_inList);
                this->search_track[i]->setProperty("index", i);
                this->search_track[i]->setObjectName("top_tracks");
            }

            for(int i = 0; i < 15; i++){
                this->search_pdAlbum[i] = new bugs::ItemPD_Album_bugs(i, SECTION_FOR_MORE_POPUP___search_pd_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_pdAlbum[i], &bugs::ItemPD_Album_bugs::signal_clicked, this, &BugsSearchMain::slot_clickedItemPD_Album);
            }

            for(int i = 0; i < 15; i++){
                this->search_video[i] = new bugs::ItemVideo_bugs(i, SECTION_FOR_MORE_POPUP___search_video, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->search_video[i], &bugs::ItemVideo_bugs::signal_clicked, this, &BugsSearchMain::slot_clickedItemVideo);
            }
        }
    }


    /**
     * @brief BugsHome::applyLoginSuccessState
     */
    void BugsSearchMain::applyLoginSuccessState(){

        if(this->is_bugs_logined()){

            if(!this->m_flagCurrLogined){

                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                /*GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();*/
            }
        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop);

                this->setUIControl_notLogin();
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void BugsSearchMain::setUIControl_basic(){//cheon211008-search

        //start 상단 step 버튼//cheon211008-search-----------------------------
        global.btn_Music= new QPushButton(tr("Music"));
        global.btn_Music->setObjectName("btn_Music");
        global.btn_Music->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_MUSIC);

        global.btn_RoseTube = new QPushButton(tr("RoseTube"));
        global.btn_RoseTube->setObjectName("btn_RoseTube");
        global.btn_RoseTube->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseTube);

        global.btn_RoseRadio= new QPushButton(tr("RoseRadio"));
        global.btn_RoseRadio->setObjectName("btn_RoseRadio");
        global.btn_RoseRadio->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseRadio);

        global.btn_Tidal = new QPushButton(tr("Tidal"));
        global.btn_Tidal->setObjectName("btn_Tidal");
        global.btn_Tidal->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_TIDAL);

        global.btn_Bugs= new QPushButton(tr("Bugs"));
        global.btn_Bugs->setObjectName("btn_Bugs");
        global.btn_Bugs->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_BUGS);

        global.btn_Qobuz = new QPushButton(tr("Qobuz"));
        global.btn_Qobuz->setObjectName("btn_Qobuz");
        global.btn_Qobuz->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_QOBUZ);


        global.btn_Music->setFixedSize(125,40);
        global.btn_RoseTube->setFixedSize(125,40);
        global.btn_RoseRadio->setFixedSize(125,40);
        global.btn_Tidal->setFixedSize(125,40);
        global.btn_Bugs->setFixedSize(125,40);
        global.btn_Qobuz->setFixedSize(125,40);


        global.btn_Music->setCursor(Qt::PointingHandCursor);
        global.btn_RoseTube->setCursor(Qt::PointingHandCursor);
        global.btn_RoseRadio->setCursor(Qt::PointingHandCursor);
        global.btn_Tidal->setCursor(Qt::PointingHandCursor);
        global.btn_Bugs->setCursor(Qt::PointingHandCursor);
        global.btn_Qobuz->setCursor(Qt::PointingHandCursor);

        //end  상단 step 버튼 //--------------------------------------------------------------
        global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover_selected));
        global.btn_RoseRadio->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseRadio").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle).arg(tmp_btnStyleHover));//cheon211115-01
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg(tmp_btnStyleHover));
        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

        // Main Title
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ", global.btn_Music, global.btn_RoseRadio, global.btn_RoseTube, global.btn_Tidal,  global.btn_Bugs, global.btn_Qobuz );//cheon211008-search
    }


    void BugsSearchMain::setUIControl_appendWidget(){

        if(flag_artist[0] == true && flag_album[0] == true && flag_track[0] == true && flag_pdAlbum[0] == true && flag_video[0] == true){

            if(flag_track[1] == true){
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn("곡" + QString(" (%1)").arg(this->list_track->at(0).total_count), "View All", BTN_IDX___search_track, this->vBox_track);

                this->vBox_track->addSpacing(10);

                QList<QString> tmp_clientkey;
                int maxCount = this->list_track->size();
                if(this->list_track->size() > 5){
                    maxCount = 5;
                }

                for(int i = 0; i < maxCount; i++){
                    this->search_track[i]->setDataTrackInfo_Bugs(this->list_track->at(i), "search");
                    this->search_track[i]->resize(1550, 70);

                    this->vBox_track->addWidget(this->search_track[i]);

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).track_id));
                }

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsSearchMain::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("BUGS", tmp_clientkey);
            }

            if(flag_artist[1] == true){
                this->widget_artist = new QWidget();
                this->widget_artist = this->setUIControl_subTitle_withSideBtn("아티스트" + QString(" (%1)").arg(this->list_artist->at(0).total_count), "View All", BTN_IDX___search_artist, this->vBox_artist);

                this->vBox_artist->addSpacing(10);

                this->hBox_artist = this->setUIControl_hBoxLayout_forArtist(this->vBox_artist);

                for(int i = 0; i < this->list_artist->size(); i++){
                    this->search_artist[i]->setData(this->list_artist->at(i));
                    this->hBox_artist->addWidget(this->search_artist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_artist);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_album[1] == true){
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn("앨범" + QString(" (%1)").arg(this->list_album->at(0).total_count), "View All", BTN_IDX___search_album, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->search_album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->search_album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_pdAlbum[1] == true){
                this->widget_pdAlbum = new QWidget();
                this->widget_pdAlbum = this->setUIControl_subTitle_withSideBtn("뮤직PD앨범" + QString(" (%1)").arg(this->list_pdAlbum->at(0).total_count), "View All", BTN_IDX___search_pd_album, this->vBox_pdAlbum);

                this->vBox_pdAlbum->addSpacing(10);

                this->hBox_pdAlbum = this->setUIControl_hBoxLayout_forPDAlbum(this->vBox_pdAlbum);

                for(int i = 0; i < this->list_pdAlbum->size(); i++){
                    this->search_pdAlbum[i]->setData(this->list_pdAlbum->at(i));
                    this->hBox_pdAlbum->addWidget(this->search_pdAlbum[i]);
                }

                this->box_main_contents->addLayout(this->vBox_pdAlbum);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_video[1] == true){
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn("영상" + QString(" (%1)").arg(this->list_video->at(0).total_count), "View All", BTN_IDX___search_video, this->vBox_video);

                this->vBox_video->addSpacing(10);

                this->hBox_video = this->setUIControl_hBoxLayout_forVideo(this->vBox_video);

                for(int i = 0; i < this->list_video->size(); i++){
                    this->search_video[i]->setData(this->list_video->at(i));
                    this->hBox_video->addWidget(this->search_video[i]);
                }

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_artist[1] == false && flag_album[1] == false && flag_track[1] == false && flag_pdAlbum[1] == false && flag_video[1] == false){

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::SearchResult_NoData);
                noData_widget->setFixedSize(1500, 700);

                GSCommon::clearLayout(this->box_main_contents);
                this->box_main_contents->addWidget(noData_widget, 0, Qt::AlignTop);
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    QWidget* BugsSearchMain::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_main_contents 에 담을 widget, layout 생성.  box_main_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0, 0, 0, 0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0, 0, 0, 0);
        widget_box_subTitle->setLayout(tmp_hBox);

        this->lb_subTitle[btnId]->setText(subTitle);
        this->lb_subTitle[btnId]->setStyleSheet("font-size:24px;font-weight:bold;color:#FFFFFF;");
        tmp_hBox->addWidget(this->lb_subTitle[btnId], 0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            this->btnView_all[btnId]->setText(btnText);
            this->btnView_all[btnId]->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            this->btnView_all[btnId]->setProperty("idx", btnId);
            this->btnView_all[btnId]->setCursor(Qt::PointingHandCursor);
            connect(this->btnView_all[btnId], SIGNAL(clicked()), this, SLOT(slot_clickBtn_subTitle_viewAll()));

            tmp_hBox->addWidget(this->btnView_all[btnId], 1, Qt::AlignVCenter | Qt::AlignRight);
        }

        // Apply Main Layout with spacing
        p_layout->addWidget(widget_box_subTitle);

        return widget_box_subTitle;
    }


    QHBoxLayout* BugsSearchMain::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    QHBoxLayout* BugsSearchMain::setUIControl_hBoxLayout_forArtist(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(260);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        QHBoxLayout *hBox_total = new QHBoxLayout();
        hBox_total->setSpacing(0);
        hBox_total->setContentsMargins(0,0,0,0);
        hBox_total->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        hBox_total->setSizeConstraint(QLayout::SetFixedSize);
        hBox_total->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    QHBoxLayout* BugsSearchMain::setUIControl_hBoxLayout_forPDAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(305);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    QHBoxLayout* BugsSearchMain::setUIControl_hBoxLayout_forVideo(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Slots
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 서브 타이틀별 우측 View All ---------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의 : 각 파트별로 All View 페이지 진입
     *      검색 상세 페이지 같은 경우에는 검색(Filtering) 및 추가 기능들이 차후 붙을 가능성이 있어서, 페이지를 각각 구분함
     * @param btnId
     */
    void BugsSearchMain::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        // 공통
        QJsonObject jsonObj_move = common::ConvertData_common::getObjectJson_searchData(this->data_search);

        if(btnId == BTN_IDX___search_track){
            // Bugs 검색 - Track - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SEARCH_TRACK_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_artist){
            // Bugs 검색 - Artist - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SEARCH_ARTIST_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_album){
            // Bugs 검색 - Album - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SEARCH_ALBUM_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_pd_album){
            // Bugs 검색 - PD's Album - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SEARCH_PD_ALBUM_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_video){
            // Bugs 검색 - Video - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___SEARCH_VIDEO_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }

    }



    // 데이터 세팅 ---------------------------------------------------------------------------------------------------

    /**
     * @brief BugsSearchMain::slot_applyResult_tracks
     * @param list_data
     * @param jsonObj_track
     */
    void BugsSearchMain::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            if(list_data.size() > 5)
            {
                for(int i = 0; i < 5; i++){
                    this->jsonArr_tracks_toPlay.append(jsonArr_dataToPlay.at(i).toObject());
                }
            }
            else{
                this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;
            }

            this->list_track->append(list_data);
            this->flag_track[1] = true;
        }

        this->flag_track[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief BugsSearchMain::slot_applyResult_artists
     * @param list_data
     * @param flag_lastPage
     */
    void BugsSearchMain::slot_applyResult_artists(const QList<bugs::ArtistItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_artist->append(list_data);
            this->flag_artist[1] = true;
        }

        this->flag_artist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief BugsSearchMain::slot_applyResult_albums
     * @param list_data
     * @param flag_lastPage
     */
    void BugsSearchMain::slot_applyResult_albums(const QList<bugs::AlbumItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_album->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief BugsSearchMain::slot_applyResult_pd_albums
     * @param list_data
     * @param flag_lastPage
     */
    void BugsSearchMain::slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_pdAlbum->append(list_data);
            this->flag_pdAlbum[1] = true;
        }

        this->flag_pdAlbum[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief BugsSearchMain::slot_applyResult_videos
     * @param list_data
     * @param jsonArr_dataToPlay
     * @param flag_lastPage
     */
    void BugsSearchMain::slot_applyResult_videos(const QList<bugs::VideoItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            if(list_data.size() > 15)
            {
                for(int i = 0; i < 15; i++){
                    this->jsonArr_videos_toPlay.append(jsonArr_dataToPlay.at(i).toObject());
                }
            }
            else{
                this->jsonArr_videos_toPlay = jsonArr_dataToPlay;
            }

            this->list_video->append(list_data);
            this->flag_video[1] = true;
        }

        this->flag_video[0] = true;
        this->setUIControl_appendWidget();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void BugsSearchMain::slot_bugs_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Bugs favorite toggle check
            /*if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzSearchMain::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else{

            }*/
        }
    }


    void BugsSearchMain::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        if(jsonObj.contains("id")){

            int id = ProcJsonEasy::getInt(jsonObj, "id");

            if(id > 0){
                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &BugsSearchMain::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);
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
                connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsSearchMain::slot_applyResult_addRating_track);
                proc_fav_track->request_rose_addRating_Track(json);
            }
        }
        else{

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void BugsSearchMain::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){

            if(jsonObj.contains("message") && (jsonObj["message"].toString() == "정상")){
                this->search_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);

            }
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void BugsSearchMain::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->search_track[i]->setFavoritesIds(flag, star);
                }

            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->search_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param idx
     * @param clickMode
     */
    void BugsSearchMain::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___search_track;

        // ClickMode 별로 처리
        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){

                this->track_star_fav = this->search_track[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = false;
                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;

                    this->track_idx_fav = idx;
                    this->flag_track_fav = true;
                }

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Bugs Favorite
                    ItemPositionData itemPosData;
                    itemPosData.section = section;
                    itemPosData.index = idx;
                    itemPosData.data_id = QString("%1").arg(this->list_track->at(idx).track_id);

                    ProcBugsAPI *proc = new ProcBugsAPI(this);
                    connect(proc, &bugs::ProcBugsAPI::completeReq_favarite_track, this, &BugsSearchMain::slot_bugs_completeReq_listAll_myFavoritesIds);

                    if(this->track_star_fav == 0){
                        itemPosData.likes_yn = false;

                        proc->request_bugs_deleteFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }
                    else if(this->track_star_fav == 1){
                        itemPosData.likes_yn = true;

                        proc->request_bugs_addFavorite_track(this->list_track->at(idx).track_id, ConvertData_forBugs::getObjectJson_itemPositionData(itemPosData));
                    }

                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &BugsSearchMain::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("BUGS", QString("%1").arg(this->list_track->at(idx).track_id));

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief ItemArtist_bugs 클릭
     */
    void BugsSearchMain::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
    }

    /**
     * @brief ItemAlbum_bugs 클릭
     */
    void BugsSearchMain::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }

    /**
     * @brief ItemPD_Album_bugs 클릭
     */
    void BugsSearchMain::slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPD_Album(this->list_pdAlbum, clickMode, index, section);
    }


    /**
     * @brief ItemVideobugs 클릭
     * @param clickMode
     */
    void BugsSearchMain::slot_clickedItemVideo(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemVideo(this->list_video, this->jsonArr_videos_toPlay, clickMode, index, section);
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
    void BugsSearchMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___search_track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_album){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_pd_album){
            this->proc_clicked_optMorePopup_fromPD_Album(this->list_pdAlbum, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_video){
            this->proc_clicked_optMorePopup_fromVideo(this->list_video, this->jsonArr_videos_toPlay, index, clickMode);
        }
    }


}
