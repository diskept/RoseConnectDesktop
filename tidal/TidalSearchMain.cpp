#include "tidal/TidalSearchMain.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QDebug>
#include <QScroller>


namespace tidal {

    const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";//cheon211008
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
    const int BTN_IDX___search_topHit = 1;
    const int BTN_IDX___search_track = 2;
    const int BTN_IDX___search_artist = 3;
    const int BTN_IDX___search_album = 4;
    const int BTN_IDX___search_playlist = 5;
    const int BTN_IDX___search_video = 6;

    const int SECTION_FOR_MORE_POPUP___search_topHit = 1;
    const int SECTION_FOR_MORE_POPUP___search_track = 2;
    const int SECTION_FOR_MORE_POPUP___search_artist = 3;
    const int SECTION_FOR_MORE_POPUP___search_album = 4;
    const int SECTION_FOR_MORE_POPUP___search_playlist = 5;
    const int SECTION_FOR_MORE_POPUP___search_video = 6;


    /**
     * @brief 생성자
     * @param parent
     */
    TidalSearchMain::TidalSearchMain(QWidget *parent) : AbstractTidalSubWidget(VerticalScroll_filter, parent) {

        // data
        this->list_topHit = new QList<tidal::ArtistItemData>();
        this->list_track = new QList<tidal::TrackItemData>();
        this->list_artist = new QList<tidal::ArtistItemData>();
        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_playlist = new QList<tidal::PlaylistItemData>();
        this->list_video = new QList<tidal::VideoItemData>();

        // 기본 UI 세팅
        this->setUIControl_basic();
    }


    /**
     * @brief 소멸자.
     */
    TidalSearchMain::~TidalSearchMain(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalSearchMain::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result"));

            // init data
            this->list_topHit->clear();
            this->list_artist->clear();
            this->list_album->clear();
            this->list_track->clear();
            this->list_playlist->clear();
            this->list_video->clear();

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flag_topHit[0] = false;
            this->flag_artist[0] = false;
            this->flag_album[0] = false;
            this->flag_track[0] = false;
            this->flag_playlist[0] = false;
            this->flag_video[0] = false;

            this->flag_topHit[1] = false;
            this->flag_artist[1] = false;
            this->flag_album[1] = false;
            this->flag_track[1] = false;
            this->flag_playlist[1] = false;
            this->flag_video[1] = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API for Suggested
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_topHit, this, &TidalSearchMain::slot_applyResult_topHit);
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &TidalSearchMain::slot_applyResult_tracks);
            connect(proc, &ProcCommon::completeReq_list_artists, this, &TidalSearchMain::slot_applyResult_artists);
            connect(proc, &ProcCommon::completeReq_list_albums, this, &TidalSearchMain::slot_applyResult_albums);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &TidalSearchMain::slot_applyResult_playlists);
            connect(proc, &ProcCommon::completeReq_list_videos, this, &TidalSearchMain::slot_applyResult_videos);
            proc->request_tidal_searchAllItems(this->data_search.search_word, 15, 0, ProcCommon::TidalSearch_ContentType::TopHit);

            /*connect(proc, &ProcCommon::completeReq_list_tracks, this, &TidalSearchMain::slot_applyResult_tracks);
            proc->request_tidal_searchAllItems(this->data_search.search_word, 5, 0, ProcCommon::TidalSearch_ContentType::Tracks);

            connect(proc, &ProcCommon::completeReq_list_artists, this, &TidalSearchMain::slot_applyResult_artists);
            proc->request_tidal_searchAllItems(this->data_search.search_word, 10, 0, ProcCommon::TidalSearch_ContentType::Artists);

            connect(proc, &ProcCommon::completeReq_list_albums, this, &TidalSearchMain::slot_applyResult_albums);
            proc->request_tidal_searchAllItems(this->data_search.search_word, 10, 0, ProcCommon::TidalSearch_ContentType::Albums);

            connect(proc, &ProcCommon::completeReq_list_playlists, this, &TidalSearchMain::slot_applyResult_playlists);
            proc->request_tidal_searchAllItems(this->data_search.search_word, 10, 0, ProcCommon::TidalSearch_ContentType::Playlists);

            connect(proc, &ProcCommon::completeReq_list_videos, this, &TidalSearchMain::slot_applyResult_videos);
            proc->request_tidal_searchAllItems(this->data_search.search_word, 10, 0, ProcCommon::TidalSearch_ContentType::Videos);*/
        }
    }


    /**
     * @brief TidalSearchMain::setActivePage
     */
    void TidalSearchMain::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_main_contents);
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
            this->vBox_topHit = new QVBoxLayout();
            this->vBox_artist = new QVBoxLayout();
            this->vBox_album = new QVBoxLayout();
            this->vBox_track = new QVBoxLayout();
            this->vBox_playlist = new QVBoxLayout();
            this->vBox_video = new QVBoxLayout();

            this->hBox_topHit = new QHBoxLayout();
            this->hBox_artist = new QHBoxLayout();
            this->hBox_album = new QHBoxLayout();
            this->hBox_playlist = new QHBoxLayout();
            this->hBox_video = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_topHit);
            GSCommon::clearLayout(this->vBox_artist);
            GSCommon::clearLayout(this->vBox_album);
            GSCommon::clearLayout(this->vBox_track);
            GSCommon::clearLayout(this->vBox_playlist);
            GSCommon::clearLayout(this->vBox_video);

            GSCommon::clearLayout(this->hBox_topHit);
            GSCommon::clearLayout(this->hBox_artist);
            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_playlist);
            GSCommon::clearLayout(this->hBox_video);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->search_artist[i] = new tidal::ItemArtist(i, SECTION_FOR_MORE_POPUP___search_artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_artist[i], &tidal::ItemArtist::signal_clicked, this, &TidalSearchMain::slot_clickedItemArtist);
            }

            for(int i = 0; i < 15; i++){
                this->search_album[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___search_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_album[i], &tidal::ItemAlbum::signal_clicked, this, &TidalSearchMain::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->search_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->search_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalSearchMain::slot_clickedItemTrack_inList);
                this->search_track[i]->setProperty("index", i);
                this->search_track[i]->setObjectName("top_tracks");
            }

            for(int i = 0; i < 15; i++){
                this->search_playlist[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___search_playlist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_playlist[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalSearchMain::slot_clickedItemPlaylist);
            }

            for(int i = 0; i < 15; i++){
                this->search_video[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___search_video, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->search_video[i], &tidal::ItemVideo::signal_clicked, this, &TidalSearchMain::slot_clickedItemVideo);
            }
        }
    }


    /**
     * @brief 기본화면 구성
     */
    void TidalSearchMain::setUIControl_basic(){//cheon211008-search-----------------------------


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
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

        // Main Title
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ", global.btn_Music, global.btn_RoseRadio, global.btn_RoseTube, global.btn_Tidal,  global.btn_Bugs, global.btn_Qobuz );//cheon211008-search
    }


    void TidalSearchMain::setUIControl_appendWidget(){

        if(flag_topHit[0] == true && flag_artist[0] == true && flag_album[0] == true
                && flag_track[0] == true && flag_playlist[0] == true && flag_video[0] == true){

            if(flag_topHit[1] == true){
                this->widget_topHit = new QWidget();
                this->widget_topHit = this->setUIControl_subTitle_withSideBtn("Top Hit", "", BTN_IDX___search_topHit, this->vBox_topHit);

                this->vBox_topHit->addSpacing(10);

                this->hBox_topHit = this->setUIControl_hBoxLayout_forArtist(this->vBox_topHit);

                this->search_topHit = new tidal::ItemArtist(0, SECTION_FOR_MORE_POPUP___search_topHit, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_topHit, &tidal::ItemArtist::signal_clicked, this, &TidalSearchMain::slot_clickedItemArtist);

                this->search_topHit->setData(this->list_topHit->at(0));
                this->hBox_topHit->addWidget(this->search_topHit);

                this->box_main_contents->addLayout(this->vBox_topHit);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_track[1] == true){
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString("Tracks (%1)").arg(this->list_track->at(0).totalCount), "View All", BTN_IDX___search_track, this->vBox_track);

                this->vBox_track->addSpacing(10);

                QList<QString> tmp_clientkey;
                int maxCount = this->list_track->size();
                if(this->list_track->size() > 5){
                    maxCount = 5;
                }

                for(int i = 0; i < maxCount; i++){
                    this->search_track[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                    this->search_track[i]->resize(1550, 70);

                    this->vBox_track->addWidget(this->search_track[i]);

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalSearchMain::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
            }

            if(flag_artist[1] == true){
                this->widget_artist = new QWidget();
                this->widget_artist = this->setUIControl_subTitle_withSideBtn(QString("Artists (%1)").arg(this->list_artist->at(0).artist_total_cnt), "View All", BTN_IDX___search_artist, this->vBox_artist);

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
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("Albums (%1)").arg(this->list_album->at(0).totalCount), "View All", BTN_IDX___search_album, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->search_album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->search_album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_playlist[1] == true){
                this->widget_playlist = new QWidget();
                this->widget_playlist = this->setUIControl_subTitle_withSideBtn(QString("Playlists (%1)").arg(this->list_playlist->at(0).totalCount), "View All", BTN_IDX___search_playlist, this->vBox_playlist);

                this->vBox_playlist->addSpacing(10);

                this->hBox_playlist = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_playlist);

                for(int i = 0; i < this->list_playlist->size(); i++){
                    this->search_playlist[i]->setData(this->list_playlist->at(i));
                    this->hBox_playlist->addWidget(this->search_playlist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_playlist);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_video[1] == true){
                this->widget_video = new QWidget();
                this->widget_video = this->setUIControl_subTitle_withSideBtn(QString("Music Video (%1)").arg(this->list_video->at(0).totalCount), "View All", BTN_IDX___search_video, this->vBox_video);

                this->vBox_video->addSpacing(10);

                this->hBox_video = this->setUIControl_hBoxLayout_forVideo(this->vBox_video);

                for(int i = 0; i < this->list_video->size(); i++){
                    this->search_video[i]->setData(this->list_video->at(i));
                    this->hBox_video->addWidget(this->search_video[i]);
                }

                this->box_main_contents->addLayout(this->vBox_video);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_topHit[1] == false && flag_artist[1] == false && flag_album[1] == false
                    && flag_track[1] == false && flag_playlist[1] == false && flag_video[1] == false){

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


    QWidget* TidalSearchMain::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* TidalSearchMain::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(275);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }


    QHBoxLayout* TidalSearchMain::setUIControl_hBoxLayout_forArtist(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(250);

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


    QHBoxLayout* TidalSearchMain::setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(295);

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


    QHBoxLayout* TidalSearchMain::setUIControl_hBoxLayout_forVideo(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(275);

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
    // [Slot] Functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 서브 타이틀별 우측 View All ---------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의 : 각 파트별로 All View 페이지 진입
     *      검색 상세 페이지 같은 경우에는 검색(Filtering) 및 추가 기능들이 차후 붙을 가능성이 있어서, 페이지를 각각 구분함
     * @param btnId
     */
    void TidalSearchMain::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        // 공통
        QJsonObject jsonObj_move = common::ConvertData_common::getObjectJson_searchData(this->data_search);

        if(btnId == BTN_IDX___search_track){
            // Tidal 검색 - Track - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_SEARCH_TRACK_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_artist){
            // Tidal 검색 - Artist - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_SEARCH_ARTIST_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_album){
            // Tidal 검색 - Album - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_SEARCH_ALBUM_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_playlist){
            // Tidal 검색 - Playlist - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_SEARCH_PLAYLIST_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_video){
            // Tidal 검색 - Video - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_SEARCH_VIDEO_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }

    }


    // 데이터 세팅 ---------------------------------------------------------------------------------------------------

    /**
     * @brief TidalSearchMain::TidalSearchMain
     * @param list_data
     * @param jsonObj_track
     */
    void TidalSearchMain::slot_applyResult_topHit(const QList<tidal::ArtistItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){

            this->list_topHit->append(list_data);
            this->flag_topHit[1] = true;
        }

        this->flag_topHit[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief TidalSearchMain::slot_applyResult_tracks
     * @param list_data
     * @param jsonObj_track
     */
    void TidalSearchMain::slot_applyResult_tracks(const QList<tidal::TrackItemData>& list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

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
     * @brief TidalSearchMain::slot_applyResult_artists
     * @param list_data
     * @param flag_lastPage
     */
    void TidalSearchMain::slot_applyResult_artists(const QList<tidal::ArtistItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_artist->append(list_data);
            this->flag_artist[1] = true;
        }

        this->flag_artist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief TidalSearchMain::slot_applyResult_albums
     * @param list_data
     * @param jsonArr_data
     * @param flag_lastPage
     */
    void TidalSearchMain::slot_applyResult_albums(const QList<tidal::AlbumItemData>& list_data, const QJsonArray& jsonArr_data, const bool flag_lastPage){

        Q_UNUSED(jsonArr_data);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_album->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief TidalSearchMain::slot_applyResult_playlists
     * @param list_data
     * @param flag_lastPage
     */
    void TidalSearchMain::slot_applyResult_playlists(const QList<tidal::PlaylistItemData>& list_data, const QJsonArray& jsonArr_data, const bool flag_lastPage){

        Q_UNUSED(jsonArr_data);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_playlist->append(list_data);
            this->flag_playlist[1] = true;
        }

        this->flag_playlist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief TidalSearchMain::slot_applyResult_videos
     * @param list_data
     * @param jsonArr_dataToPlay
     * @param flag_lastPage
     */
    void TidalSearchMain::slot_applyResult_videos(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
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

    void TidalSearchMain::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Tidal Favorite toggle
                    //ProcCommon *proc = new ProcCommon(this);
                    //connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalSearchMain::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    //proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void TidalSearchMain::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = QJsonObject();
        jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id <= 0){

            /*QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(tmp_album, "artist");
            QJsonObject artist;
            artist.insert("albums_count", ProcJsonEasy::getInt(tmp_artist, "albums_count"));
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));

            QJsonObject tmp_label = ProcJsonEasy::getJsonObject(tmp_album, "label");
            QJsonObject label;
            label.insert("albums_count", ProcJsonEasy::getInt(tmp_label, "albums_count"));
            label.insert("id", ProcJsonEasy::getInt(tmp_label, "id"));
            label.insert("name", ProcJsonEasy::getString(tmp_label, "name"));

            QJsonObject album;
            album.insert("artist", artist);
            album.insert("duration", ProcJsonEasy::getInt(tmp_album, "duration"));
            album.insert("favCnt", 0);
            album.insert("hires", false);
            album.insert("hires_streamable", false);
            album.insert("id", ProcJsonEasy::getString(tmp_album, "id"));
            album.insert("image", ProcJsonEasy::getJsonObject(tmp_album, "image"));
            album.insert("label", label);
            album.insert("streamable", false);
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));
            album.insert("tracks_count", 0);

            QJsonObject data;
            data.insert("album", album);
            data.insert("composer", ProcJsonEasy::getJsonObject(jsonObj_track, "composer"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            data.insert("favCnt", ProcJsonEasy::getInt(jsonObj_track, "favCnt"));
            data.insert("hires", ProcJsonEasy::getBool(jsonObj_track, "hires"));
            data.insert("hires_streamable", ProcJsonEasy::getBool(jsonObj_track, "hires_streamable"));
            data.insert("id", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            data.insert("maximum_bit_depth",  ProcJsonEasy::getInt(jsonObj_track, "maximum_bit_depth"));
            data.insert("maximum_sampling_rate",  ProcJsonEasy::getInt(jsonObj_track, "maximum_sampling_rate"));
            data.insert("media_number", ProcJsonEasy::getInt(jsonObj_track, "media_number"));
            data.insert("performer", ProcJsonEasy::getJsonObject(jsonObj_track, "performer"));
            data.insert("performers", ProcJsonEasy::getString(jsonObj_track, "performers"));
            data.insert("previewable", ProcJsonEasy::getBool(jsonObj_track, "previewable"));
            data.insert("purchasable", ProcJsonEasy::getBool(jsonObj_track, "purchasable"));
            data.insert("qobuz_id", ProcJsonEasy::getInt(jsonObj_track, "qobuz_id"));
            data.insert("sampleable", ProcJsonEasy::getBool(jsonObj_track, "sampleable"));
            data.insert("streamable", ProcJsonEasy::getBool(jsonObj_track, "streamable"));
            data.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            data.insert("track_number", ProcJsonEasy::getInt(jsonObj_track, "track_number"));
            data.insert("work", ProcJsonEasy::getString(jsonObj_track, "work"));

            QJsonObject tmp_thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnail = ProcJsonEasy::getString(tmp_thumbnail, "large");

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", 0);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 1);
            track.insert("thumbnailUrl", thumbnail);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "QOBUZ");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzSearchMain::slot_applyResult_addRating_track);
            proc_fav_track->request_rose_addRating_Track(json);*/
        }
        else{

            /*QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

            QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(tmp_album, "artist");
            QJsonObject artist;
            artist.insert("albums_count", ProcJsonEasy::getInt(tmp_artist, "albums_count"));
            artist.insert("id", ProcJsonEasy::getInt(tmp_artist, "id"));
            artist.insert("name", ProcJsonEasy::getString(tmp_artist, "name"));

            QJsonObject tmp_label = ProcJsonEasy::getJsonObject(tmp_album, "label");
            QJsonObject label;
            label.insert("albums_count", ProcJsonEasy::getInt(tmp_label, "albums_count"));
            label.insert("id", ProcJsonEasy::getInt(tmp_label, "id"));
            label.insert("name", ProcJsonEasy::getString(tmp_label, "name"));

            QJsonObject album;
            album.insert("duration", 0);
            album.insert("favCnt", 0);
            album.insert("hires", false);
            album.insert("hires_streamable", false);
            album.insert("id", ProcJsonEasy::getString(tmp_album, "id"));
            album.insert("image", ProcJsonEasy::getJsonObject(tmp_album, "image"));
            album.insert("streamable", false);
            album.insert("title", ProcJsonEasy::getString(tmp_album, "title"));
            album.insert("tracks_count", 0);

            QJsonObject data;
            data.insert("album", album);
            data.insert("artist", artist);
            data.insert("composer", ProcJsonEasy::getJsonObject(jsonObj_track, "composer"));
            data.insert("copyright", ProcJsonEasy::getString(jsonObj_track, "copyright"));
            data.insert("duration", ProcJsonEasy::getInt(jsonObj_track, "duration"));
            data.insert("favCnt", ProcJsonEasy::getInt(jsonObj_track, "favCnt"));
            data.insert("hires", ProcJsonEasy::getBool(jsonObj_track, "hires"));
            data.insert("hires_streamable", ProcJsonEasy::getBool(jsonObj_track, "hires_streamable"));
            data.insert("id", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            data.insert("maximum_bit_depth",  ProcJsonEasy::getInt(jsonObj_track, "maximum_bit_depth"));
            data.insert("maximum_sampling_rate",  ProcJsonEasy::getInt(jsonObj_track, "maximum_sampling_rate"));
            data.insert("media_number", ProcJsonEasy::getInt(jsonObj_track, "media_number"));
            data.insert("performer", ProcJsonEasy::getJsonObject(jsonObj_track, "performer"));
            data.insert("performers", ProcJsonEasy::getString(jsonObj_track, "performers"));
            data.insert("previewable", ProcJsonEasy::getBool(jsonObj_track, "previewable"));
            data.insert("purchasable", ProcJsonEasy::getBool(jsonObj_track, "purchasable"));
            data.insert("qobuz_id", ProcJsonEasy::getInt(jsonObj_track, "qobuz_id"));
            data.insert("sampleable", ProcJsonEasy::getBool(jsonObj_track, "sampleable"));
            data.insert("streamable", ProcJsonEasy::getBool(jsonObj_track, "streamable"));
            data.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            data.insert("track_number", ProcJsonEasy::getInt(jsonObj_track, "track_number"));

            QJsonObject tmp_thumbnail = ProcJsonEasy::getJsonObject(tmp_album, "image");
            QString thumbnail = ProcJsonEasy::getString(tmp_thumbnail, "large");

            QJsonObject track;
            track.insert("clientKey", QString("%1").arg(ProcJsonEasy::getInt(jsonObj_track, "id")));
            track.insert("data", data);
            track.insert("duration", 0);
            track.insert("favorite", this->flag_track_fav);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("thumbnailUrl", thumbnail);
            track.insert("title", ProcJsonEasy::getString(jsonObj_track, "title"));
            track.insert("type", "QOBUZ");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzSearchMain::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);*/

        }
    }


    void TidalSearchMain::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void TidalSearchMain::slot_applyResult_getRating_track(const QJsonArray &contents){

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
     * @param index
     * @param clickMode
     */
    void TidalSearchMain::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___search_track;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->search_track[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;

                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                if(this->track_star_fav == 0 || this->track_star_fav == 1){
                    // Qobuz Favorite toggle
                    this->track_id_fav = this->list_track->at(idx).id;

                    //ProcCommon *proc = new ProcCommon(this);
                    //connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalSearchMain::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    //proc->request_tidal_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    //this->flag_send_track = true;
                }

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                //roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                //connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalSearchMain::slot_applyResult_checkRating_track);
                //proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                //this->flag_check_track = true;

                this->search_track[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief [slot] override - ItemArtist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalSearchMain::slot_clickedItemArtist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_topHit){
            this->proc_clicked_itemArtist(this->list_topHit, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_artist){
            this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalSearchMain::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_album){
            this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalSearchMain::slot_clickedItemPlaylist(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_playlist){
            this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
        }
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalSearchMain::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_video){

            if(clickMode == AbstractItem::ClickMode::PlayBtn || clickMode == AbstractItem::ClickMode::AllBox){

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

                if(playType == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                        || playType == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                        || playType == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
                {
                    QJsonArray tmpJsonArr = QJsonArray();
                    for(int i = index; i < this->list_video->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_videolist(tmpJsonArr, playType);
                }
                else{
                    QJsonObject tmpJsonObj = this->list_video->at(index).jsonObj_toPlay;

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_video(tmpJsonObj, playType);
                }
            }
            else{
                this->proc_clicked_itemVideo(this->list_video, clickMode, index, section);
            }
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
    void TidalSearchMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___search_topHit){
            this->proc_clicked_optMorePopup_fromArtist(this->list_topHit, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_track){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_artist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_album){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_playlist){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_video){
            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;
            }
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
                if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                        || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                        || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
                {
                    QJsonArray tmpJsonArr = QJsonArray();
                    for(int i = index; i < this->list_video->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_videolist(tmpJsonArr, clickMode);
                }
                else{
                    QJsonObject tmpJsonObj = this->list_video->at(index).jsonObj_toPlay;

                    // Rose Play 요청
                    ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                    procRosePlay->requestPlayRose_video(tmpJsonObj, clickMode);
                }
            }
            else{
                this->proc_clicked_optMorePopup_fromVideo(this->list_video, index, clickMode);
            }
        }
    }

}
