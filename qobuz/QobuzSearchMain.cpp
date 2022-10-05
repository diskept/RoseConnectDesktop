#include "qobuz/QobuzSearchMain.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"

#include "widget/NoData_Widget.h"

#include <QDebug>
#include <QScroller>


namespace qobuz {

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
    const int BTN_IDX___search_mostPopular = 1;
    const int BTN_IDX___search_artist = 2;
    const int BTN_IDX___search_album = 3;
    const int BTN_IDX___search_track = 4;
    const int BTN_IDX___search_playlist = 5;


    const int SECTION_FOR_MORE_POPUP___search_mostPopular_artist = 0;
    const int SECTION_FOR_MORE_POPUP___search_mostPopular_album = 1;
    const int SECTION_FOR_MORE_POPUP___search_mostPopular_playlist = 2;
    const int SECTION_FOR_MORE_POPUP___search_artist = 3;
    const int SECTION_FOR_MORE_POPUP___search_album = 4;
    const int SECTION_FOR_MORE_POPUP___search_track = 5;
    const int SECTION_FOR_MORE_POPUP___search_playlist = 6;


    /**
     * @brief 생성자
     * @param parent
     */
    QobuzSearchMain::QobuzSearchMain(QWidget *parent) : AbstractQobuzSubWidget(VerticalScroll_filter, parent) {

        // data
        this->list_mostPopular_artist = new QList<qobuz::ArtistItemData>();
        this->list_mostPopular_album = new QList<qobuz::AlbumItemData>();
        this->list_mostPopular_track = new QList<qobuz::TrackItemData>();
        this->list_mostPopular_playlist = new QList<qobuz::PlaylistItemData>();
        this->list_artist = new QList<qobuz::ArtistItemData>();
        this->list_album = new QList<qobuz::AlbumItemData>();
        this->list_track = new QList<qobuz::TrackItemData>();
        this->list_playlist = new QList<qobuz::PlaylistItemData>();

        // 기본 UI 세팅
        this->btnStyle_normal = "QPushButton{ ";
        this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:15px;";
        this->btnStyle_normal += "border:1px solid #707070; border-radius:20px;}";
        this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:15px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:20px;}";

        this->filterList << "Hi-Res" << "New Release" << tr("Main Artist") << tr("Composer") << tr("Performer") << tr("Release Name") << tr("Label");

        for(int i = 0; i < this->filterList.count(); i++){
            this->enable_filter_id[i] = 0;
            global.user_forQobuz.enable_search_filter[i] = 0;
        }

        this->setUIControl_basic();
        this->setUIControl_chooseFilter();
        this->setUIControl_chooseFilter_makeData();
    }


    /**
     * @brief 소멸자.
     */
    QobuzSearchMain::~QobuzSearchMain(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzSearchMain::setJsonObject_forData(const QJsonObject &jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            if(this->flag_mostPopular[1] == true || this->flag_artist[1] == true || this->flag_album[1] == true || this->flag_track[1] == true || this->flag_playlist[1] == true){
                this->widget_main_contents->hide();
            }

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);

            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result"));

            // init data
            this->list_mostPopular_artist->clear();
            this->list_mostPopular_album->clear();
            this->list_mostPopular_track->clear();
            this->list_mostPopular_playlist->clear();
            this->list_artist->clear();
            this->list_album->clear();
            this->list_track->clear();
            this->list_playlist->clear();
            this->jsonArr_tracks_toPlay = QJsonArray();

            this->mostPopular_type = "";

            this->flag_mostPopular[0] = false;
            this->flag_artist[0] = false;
            this->flag_album[0] = false;
            this->flag_track[0] = false;
            this->flag_playlist[0] = false;

            this->flag_mostPopular[1] = false;
            this->flag_artist[1] = false;
            this->flag_album[1] = false;
            this->flag_track[1] = false;
            this->flag_playlist[1] = false;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->data_search.search_keyword = this->data_search.search_word;

            if(global.user_forQobuz.enable_search_filter[0] == 1){
                this->data_search.search_keyword += "#HiRes";
            }

            if(global.user_forQobuz.enable_search_filter[1] == 1){
                this->data_search.search_keyword += "#NewRelease";
            }

            if(global.user_forQobuz.enable_search_filter[2] == 1){
                this->data_search.search_keyword += "#ByMainArtist";
            }

            if(global.user_forQobuz.enable_search_filter[3] == 1){
                this->data_search.search_keyword += "#ByComposer";
            }

            if(global.user_forQobuz.enable_search_filter[4] == 1){
                this->data_search.search_keyword += "#ByPerformer";
            }

            if(global.user_forQobuz.enable_search_filter[5] == 1){
                this->data_search.search_keyword += "#ByReleaseName";
            }

            if(global.user_forQobuz.enable_search_filter[6] == 1){
                this->data_search.search_keyword += "#ByLabel";
            }


            // request HTTP API for Suggested
            ProcCommon *proc = new ProcCommon(this);
            connect(proc, &ProcCommon::completeReq_list_mostPopular, this, &QobuzSearchMain::slot_applyResult_mostPopular);
            connect(proc, &ProcCommon::completeReq_list_artists, this, &QobuzSearchMain::slot_applyResult_artists);
            connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzSearchMain::slot_applyResult_albums);
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &QobuzSearchMain::slot_applyResult_tracks);
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &QobuzSearchMain::slot_applyResult_playlists);
            proc->request_qobuz_searchAllItems(this->data_search.search_keyword, 15, 0, ProcCommon::QobuzSearch_ContentType::MostPopular);

            /*
            //Most Popular
            connect(proc, &ProcCommon::completeReq_list_mostPopular, this, &QobuzSearchMain::slot_applyResult_mostPopular);
            proc->request_qobuz_searchAllItems(this->data_search.search_word, 15, 0, ProcCommon::QobuzSearch_ContentType::MostPopular);

            //Artists
            connect(proc, &ProcCommon::completeReq_list_artists, this, &QobuzSearchMain::slot_applyResult_artists);
            proc->request_qobuz_searchAllItems(this->data_search.search_word, 15, 0, ProcCommon::QobuzSearch_ContentType::Artists);

            //release
            connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzSearchMain::slot_applyResult_albums);
            proc->request_qobuz_searchAllItems(this->data_search.search_word, 15, 0, ProcCommon::QobuzSearch_ContentType::Albums);

            //Tracks
            connect(proc, &ProcCommon::completeReq_list_tracks, this, &QobuzSearchMain::slot_applyResult_tracks);
            proc->request_qobuz_searchAllItems(this->data_search.search_word, 5, 0, ProcCommon::QobuzSearch_ContentType::Tracks);

            //playlists
            connect(proc, &ProcCommon::completeReq_list_playlists, this, &QobuzSearchMain::slot_applyResult_playlists);
            proc->request_qobuz_searchAllItems(this->data_search.search_word, 15, 0, ProcCommon::QobuzSearch_ContentType::Playlists);
            */
        }
    }


    /**
     * @brief QobuzSearchMain::setActivePage
     */
    void QobuzSearchMain::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

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
            this->vBox_mostPopular = new QVBoxLayout();
            this->vBox_artist = new QVBoxLayout();
            this->vBox_album = new QVBoxLayout();
            this->vBox_track = new QVBoxLayout();
            this->vBox_playlist = new QVBoxLayout();

            this->hBox_mostPopular = new QHBoxLayout();
            this->hBox_artist = new QHBoxLayout();
            this->hBox_album = new QHBoxLayout();
            this->hBox_playlist = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_mostPopular);
            GSCommon::clearLayout(this->vBox_artist);
            GSCommon::clearLayout(this->vBox_album);
            GSCommon::clearLayout(this->vBox_track);
            GSCommon::clearLayout(this->vBox_playlist);

            GSCommon::clearLayout(this->hBox_mostPopular);
            GSCommon::clearLayout(this->hBox_artist);
            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_playlist);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->search_artist[i] = new qobuz::ItemArtist_qobuz(i, SECTION_FOR_MORE_POPUP___search_artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_artist[i], &qobuz::ItemArtist_qobuz::signal_clicked, this, &QobuzSearchMain::slot_clickedItemArtist);
            }

            for(int i = 0; i < 15; i++){
                this->search_album[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___search_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->search_album[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzSearchMain::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->search_track[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->search_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzSearchMain::slot_clickedItemTrack_inList);
                this->search_track[i]->setProperty("index", i);
                this->search_track[i]->setObjectName("top_tracks");
            }

            for(int i = 0; i < 15; i++){
                this->search_playlist[i] = new qobuz::ItemPlaylist_qobuz(i, SECTION_FOR_MORE_POPUP___search_playlist, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
                connect(this->search_playlist[i], &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzSearchMain::slot_clickedItemPlaylist);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Filter 선택하는 부분. 최상단.
     */
    void QobuzSearchMain::setUIControl_chooseFilter(){

        // 상단 필터
        this->widget_btnFilter = new QWidget();
        this->widget_btnFilter->setObjectName("widget_btnFilter");
        this->widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
        this->widget_btnFilter->setFixedSize(global.LmtCnt, 75);//c220805

        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png", this->widget_btnFilter);
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);
        this->btn_filter_ico->setGeometry(3, 0, 0, 0);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

        this->flow_option_filter = new FlowLayout(0, 10, 5);
        this->flow_option_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_option_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_option_filter = new QWidget(this->widget_btnFilter);
        this->widget_option_filter->setFixedSize(global.LmtCnt - 180, 75);
        this->widget_option_filter->setStyleSheet("background:transparent;");
        this->widget_option_filter->setLayout(this->flow_option_filter);
        this->widget_option_filter->setGeometry(90, 23, 0, 0);

        this->box_filter->addWidget(widget_btnFilter);

        this->btn_filter_cnt = 7;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            this->lb_filter[i] = new QLabel();
            this->lb_filter[i]->setContentsMargins(20, 0, 20, 0);
            this->lb_filter[i]->setFixedHeight(30);
            this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_filter[i]->setAlignment(Qt::AlignCenter);
        }
    }


    void QobuzSearchMain::setUIControl_chooseFilter_makeData(){

        this->flag_hiRes = false;
        this->flag_newRelease = false;

        QPushButton *btn_close_tag = new QPushButton();
        btn_close_tag->setText(tr("Add tags to your search"));
        btn_close_tag->setFixedSize(470, 50);
        btn_close_tag->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_tag->setContentsMargins(0, 0, 0, 0);
        btn_close_tag->setCursor(Qt::PointingHandCursor);

        connect(btn_close_tag, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *hbox_header = new QHBoxLayout();
        hbox_header->setContentsMargins(0, 0, 0, 0);
        hbox_header->setSpacing(0);
        hbox_header->addWidget(btn_close_tag);

        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFixedHeight(1);
        line->setLineWidth(1);
        line->setStyleSheet("background-color:#4D4D4D;border:none;");


        QWidget *widget_tag = new QWidget();
        widget_tag->setFixedSize(470, 60);
        widget_tag->setStyleSheet("background-color:transparent;");

        this->btn_HiRes = new QPushButton(widget_tag);
        this->btn_HiRes->setObjectName("btn_HiRes");
        this->btn_HiRes->setStyleSheet("background-color:transparent;");
        this->btn_HiRes->setFixedSize(200, 50);
        this->btn_HiRes->setGeometry(20, 5, 0, 0);

        connect(this->btn_HiRes, &QPushButton::clicked, this, &QobuzSearchMain::slot_clicked_btnSelect);

        this->lb_HiRes = GSCommon::getUILabelImg(":/images/rosehome/list_check_off.png", this->btn_HiRes);
        this->lb_HiRes->setFixedSize(50, 50);
        this->lb_HiRes->setGeometry(0, 0, 0, 0);

        QLabel *lb_HiRes_Title = new QLabel(this->btn_HiRes);
        lb_HiRes_Title->setFixedSize(150, 26);
        lb_HiRes_Title->setGeometry(50, 12, 0, 0);
        lb_HiRes_Title->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        lb_HiRes_Title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        lb_HiRes_Title->setText("Hi-Res");

        this->btn_NewRelease = new QPushButton(widget_tag);
        this->btn_NewRelease->setObjectName("btn_NewRelease");
        this->btn_NewRelease->setStyleSheet("background-color:transparent;");
        this->btn_NewRelease->setFixedSize(200, 50);
        this->btn_NewRelease->setGeometry(250, 5, 0, 0);

        connect(this->btn_NewRelease, &QPushButton::clicked, this, &QobuzSearchMain::slot_clicked_btnSelect);

        this->lb_NewRelease = GSCommon::getUILabelImg(":/images/rosehome/list_check_off.png", this->btn_NewRelease);
        this->lb_NewRelease->setFixedSize(50, 50);
        this->lb_NewRelease->setGeometry(0, 0, 0, 0);

        QLabel *lb_NewRelease_Title = new QLabel(this->btn_NewRelease);
        lb_NewRelease_Title->setFixedSize(150, 26);
        lb_NewRelease_Title->setGeometry(50, 12, 0, 0);
        lb_NewRelease_Title->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
        lb_NewRelease_Title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        lb_NewRelease_Title->setText("New Release");

        QHBoxLayout *hbox_tag = new QHBoxLayout();
        hbox_tag->setContentsMargins(0, 0, 0, 0);
        hbox_tag->setSpacing(0);
        hbox_tag->addWidget(widget_tag);


        QFrame *line1 = new QFrame;
        line1->setFrameShape(QFrame::HLine);
        line1->setFixedHeight(1);
        line1->setLineWidth(1);
        line1->setStyleSheet("background-color:#4D4D4D;border:none;");

        QPushButton *btn_close_filter = new QPushButton();
        btn_close_filter->setText(tr("Search in"));
        btn_close_filter->setFixedSize(470, 50);
        btn_close_filter->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_filter->setContentsMargins(0, 0, 0, 0);
        btn_close_filter->setCursor(Qt::PointingHandCursor);

        connect(btn_close_tag, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *hbox_filter = new QHBoxLayout();
        hbox_filter->setContentsMargins(0, 0, 0, 0);
        hbox_filter->setSpacing(0);
        hbox_filter->addWidget(btn_close_filter);

        QFrame *line2 = new QFrame;
        line2->setFrameShape(QFrame::HLine);
        line2->setFixedHeight(1);
        line2->setLineWidth(1);
        line2->setStyleSheet("background-color:#4D4D4D;border:none;");


        FlowLayout *flow_filter = new FlowLayout(0, 20, 20);
        flow_filter->setContentsMargins(10, 20, 10, 20);
        flow_filter->setSizeConstraint(QLayout::SetMinimumSize);

        for(int i = 2; i < this->btn_filter_cnt; i++){

            this->btn_filter[i] = new QPushButton();
            this->btn_filter[i]->setText(this->filterList[i]);
            this->btn_filter[i]->setFixedSize(QSize(135, 40));
            this->btn_filter[i]->setStyleSheet(global.user_forQobuz.enable_search_filter[i] == 1 ? this->btnStyle_selected : this->btnStyle_normal);
            this->btn_filter[i]->setObjectName("btn_filter");
            this->btn_filter[i]->setProperty("idx", i);
            this->btn_filter[i]->setCursor(Qt::PointingHandCursor);

            connect(this->btn_filter[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseFilter()));

            flow_filter->addWidget(this->btn_filter[i]);
        }


        QPushButton *btn_apply = new QPushButton();
        btn_apply->setObjectName("btn_apply");
        btn_apply->setFixedSize(170, 40);
        btn_apply->setStyleSheet("#btn_apply{background:transparent;color:#FFFFFF;font-size:20px;border:1px solid #707070;border-radius:20px;} #btn_apply:hover{background:#BE905F;color:#FFFFFF;font-size:20px;border:1px solid #BE905F;border-radius:20px;}");
        btn_apply->setText(tr("Apply"));
        btn_apply->setCursor(Qt::PointingHandCursor);

        connect(btn_apply, SIGNAL(clicked()), this, SLOT(slot_clickBtn_apply()));

        QHBoxLayout *hbox_apply = new QHBoxLayout;
        hbox_apply->setContentsMargins(0, 0, 0, 0);
        hbox_apply->setSpacing(0);
        hbox_apply->setAlignment(Qt::AlignCenter);
        hbox_apply->addWidget(btn_apply);


        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(hbox_header);
        box_total->addWidget(line);
        box_total->addLayout(hbox_tag);
        box_total->addWidget(line1);
        box_total->addLayout(hbox_filter);
        box_total->addWidget(line2);
        box_total->addLayout(flow_filter, 0);
        box_total->addLayout(hbox_apply, 1);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(470, 380);
        this->widget_filter->move(70, 85);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setLayout(box_total);
        btn_total->setFixedSize(470, 380);
        btn_total->setGeometry(0, 0, 0, 0);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    /**
     * @brief 기본화면 구성
     */
    void QobuzSearchMain::setUIControl_basic(){//cheon211008-search-----------------------------

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
        //end  상단 step 버튼 //--------------------------------------------------------------
        global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover_selected));
        global.btn_RoseRadio->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseRadio").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle).arg(tmp_btnStyleHover));//cheon211115-01
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

        // Main Title
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ", global.btn_Music, global.btn_RoseRadio, global.btn_RoseTube, global.btn_Tidal,  global.btn_Bugs, global.btn_Qobuz );//cheon211008-search
    }


    void QobuzSearchMain::setUIControl_appendWidget(){

        if(flag_mostPopular[0] == true && flag_artist[0] == true && flag_album[0] == true
                && flag_track[0] == true && flag_playlist[0] == true){

            if(flag_mostPopular[1] == true){
                if(this->mostPopular_type == "tracks"){
                    return;
                }

                this->widget_mostPopular = new QWidget();
                this->widget_mostPopular = this->setUIControl_subTitle_withSideBtn("Most popular", "", BTN_IDX___search_mostPopular, this->vBox_mostPopular);

                this->vBox_mostPopular->addSpacing(10);

                if(this->mostPopular_type == "artists"){
                    this->hBox_mostPopular = this->setUIControl_hBoxLayout_forArtist(this->vBox_mostPopular);

                    this->search_mostPopular_artist = new qobuz::ItemArtist_qobuz(0, SECTION_FOR_MORE_POPUP___search_mostPopular_artist, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->search_mostPopular_artist, &qobuz::ItemArtist_qobuz::signal_clicked, this, &QobuzSearchMain::slot_clickedItemArtist);

                    this->search_mostPopular_artist->setData(this->list_mostPopular_artist->at(0));
                    this->hBox_mostPopular->addWidget(this->search_mostPopular_artist);
                }
                else if(this->mostPopular_type == "albums"){
                    this->hBox_mostPopular = this->setUIControl_hBoxLayout_forAlbum(this->vBox_mostPopular);

                    this->search_mostPopular_album = new qobuz::ItemAlbum_qobuz(0, SECTION_FOR_MORE_POPUP___search_mostPopular_album, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->search_mostPopular_album, &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzSearchMain::slot_clickedItemAlbum);

                    this->search_mostPopular_album->setData(this->list_mostPopular_album->at(0));
                    this->hBox_mostPopular->addWidget(this->search_mostPopular_album);
                }
                else if(this->mostPopular_type == "tracks"){

                }
                else if(this->mostPopular_type == "playlists"){
                    this->hBox_mostPopular = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_mostPopular);

                    this->search_mostPopular_playlist = new qobuz::ItemPlaylist_qobuz(0, SECTION_FOR_MORE_POPUP___search_mostPopular_playlist, tidal::AbstractItem::ImageSizeMode::Ractangle_360x176);
                    connect(this->search_mostPopular_playlist, &qobuz::ItemPlaylist_qobuz::signal_clicked, this, &QobuzSearchMain::slot_clickedItemPlaylist);

                    this->search_mostPopular_playlist->setData(this->list_mostPopular_playlist->at(0));
                    this->hBox_mostPopular->addWidget(this->search_mostPopular_playlist);
                }

                this->box_main_contents->addLayout(this->vBox_mostPopular);
                this->box_main_contents->addSpacing(30);
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
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("Releases (%1)").arg(this->list_album->at(0).album_total_cnt), "View All", BTN_IDX___search_album, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->search_album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->search_album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_track[1] == true){
                this->widget_track = new QWidget();
                this->widget_track = this->setUIControl_subTitle_withSideBtn(QString("Tracks (%1)").arg(this->list_track->at(0).appearson_count), "View All", BTN_IDX___search_track, this->vBox_track);

                this->vBox_track->addSpacing(10);

                QList<QString> tmp_clientkey;
                int maxCount = this->list_track->size();
                if(this->list_track->size() > 5){
                    maxCount = 5;
                }

                for(int i = 0; i < maxCount; i++){
                    this->search_track[i]->setDataTrackInfo_Qobuz(this->list_track->at(i));
                    this->search_track[i]->resize(1550, 70);

                    this->vBox_track->addWidget(this->search_track[i]);

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                this->box_main_contents->addLayout(this->vBox_track);
                this->box_main_contents->addSpacing(30);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &QobuzSearchMain::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("QOBUZ", tmp_clientkey);
            }

            if(flag_playlist[1] == true){
                this->widget_playlist = new QWidget();
                this->widget_playlist = this->setUIControl_subTitle_withSideBtn(QString("Playlists (%1)").arg(this->list_playlist->at(0).album_total_cnt), "View All", BTN_IDX___search_playlist, this->vBox_playlist);

                this->vBox_playlist->addSpacing(10);

                this->hBox_playlist = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_playlist);

                for(int i = 0; i < this->list_playlist->size(); i++){
                    this->search_playlist[i]->setData(this->list_playlist->at(i));
                    this->hBox_playlist->addWidget(this->search_playlist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_playlist);
                this->box_main_contents->addSpacing(30);
            }

            if(flag_mostPopular[1] == false && flag_artist[1] == false && flag_album[1] == false
                    && flag_track[1] == false && flag_playlist[1] == false){

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


    QWidget* QobuzSearchMain::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* QobuzSearchMain::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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


    QHBoxLayout* QobuzSearchMain::setUIControl_hBoxLayout_forArtist(QLayout *p_layout){

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


    QHBoxLayout* QobuzSearchMain::setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(291);

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
    void QobuzSearchMain::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        // 공통
        QJsonObject jsonObj_move = common::ConvertData_common::getObjectJson_searchData(this->data_search);

        if(btnId == BTN_IDX___search_track){
            // Qobuz 검색 - Track - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SEARCH_TRACK_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_artist){
            // Qobuz 검색 - Artist - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SEARCH_ARTIST_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_album){
            // Qobuz 검색 - Album - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SEARCH_ALBUM_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX___search_playlist){
            // Qobuz 검색 - Playlist - All View 이동
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_SEARCH_PLAYLIST_ALL_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }


    void QobuzSearchMain::slot_clickBtn_Filter(){

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void QobuzSearchMain::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void QobuzSearchMain::slot_clickBtn_chooseFilter(){

        QString type = sender()->objectName();
        int idx = sender()->property("idx").toInt();

        int change_cnt = 0;
        int enable_cnt = 0;

        if(type == "btn_filter"){

            if(this->filter_change == false){
                for(int i = 2; i < this->btn_filter_cnt; i++){
                    this->enable_filter_id[i] = global.user_forQobuz.enable_search_filter[i];
                }
            }

            for(int i = 2; i < this->btn_filter_cnt; i++){
                this->btn_filter[i]->setStyleSheet(this->btnStyle_normal);
                this->flag_filter[i] = false;

                if(i == idx){
                    if(this->enable_filter_id[i] == 1){
                        this->enable_filter_id[i] = 0;
                    }
                    else if(this->enable_filter_id[i] == 0){
                        this->enable_filter_id[i] = 1;

                        this->btn_filter[i]->setStyleSheet(this->btnStyle_selected);
                        this->flag_filter[i] = true;
                    }
                }
                else{
                    this->enable_filter_id[i] = 0;
                }

                change_cnt += this->enable_filter_id[i] == global.user_forQobuz.enable_search_filter[i] ? 0 : 1;
                enable_cnt += this->enable_filter_id[i] == 1 ? 1 : 0;
            }

            if((change_cnt > 0 && enable_cnt > 0) || (change_cnt > 0 && enable_cnt == 0)){
                this->filter_change = true;
            }
            else{
                this->filter_change = false;
            }
        }
    }


    void QobuzSearchMain::slot_clickBtn_apply(){

        if(this->filter_change == true){

            for(int i = 0; i < this->btn_filter_cnt; i++){

                global.user_forQobuz.enable_search_filter[i] = this->enable_filter_id[i];

                this->lb_filter[i]->setText("");
                this->lb_filter[i]->setStyleSheet("");

                if(this->flag_filter[i] == true){
                    this->flow_option_filter->removeWidget(this->lb_filter[i]);
                }

                this->flag_filter[i] = false;
            }

            GSCommon::clearLayout(this->flow_option_filter);
            GSCommon::clearLayout(this->box_filter);
            this->setUIControl_chooseFilter();

            for(int i = 0; i < this->btn_filter_cnt; i++){

                if(global.user_forQobuz.enable_search_filter[i] == 1){
                    this->lb_filter[i]->setText(this->filterList[i]);
                    this->flow_option_filter->addWidget(this->lb_filter[i]);

                    this->flag_filter[i] = true;
                }
            }

            QJsonObject jsonObj;
            jsonObj.insert("search_word", this->data_search.search_word);
            this->data_search.search_word = "";

            this->setJsonObject_forData(jsonObj);
            this->setActivePage();
        }

        this->filter_change = false;

        this->widget_filter->setVisible(false);
    }


    void QobuzSearchMain::slot_clicked_btnSelect(){

        QString type = sender()->objectName();

        if(type == "btn_HiRes"){
            if(this->flag_hiRes == false){
                this->flag_hiRes = true;

                QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_on.png");
                this->lb_HiRes->setPixmap(*pixmap_icon);
                this->lb_HiRes->resize(pixmap_icon->width(), pixmap_icon->height());

                this->enable_filter_id[0] = 1;
            }
            else if(this->flag_hiRes == true){
                this->flag_hiRes = false;

                QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_off.png");
                this->lb_HiRes->setPixmap(*pixmap_icon);
                this->lb_HiRes->resize(pixmap_icon->width(), pixmap_icon->height());

                this->enable_filter_id[0] = 0;
            }
        }
        else if(type == "btn_NewRelease"){
            if(this->flag_newRelease == false){
                this->flag_newRelease = true;

                QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_on.png");
                this->lb_NewRelease->setPixmap(*pixmap_icon);
                this->lb_NewRelease->resize(pixmap_icon->width(), pixmap_icon->height());

                this->enable_filter_id[1] = 1;
            }
            else if(this->flag_newRelease == true){
                this->flag_newRelease = false;

                QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_off.png");
                this->lb_NewRelease->setPixmap(*pixmap_icon);
                this->lb_NewRelease->resize(pixmap_icon->width(), pixmap_icon->height());

                this->enable_filter_id[1] = 0;
            }
        }

        if((global.user_forQobuz.enable_search_filter[0] != this->enable_filter_id[0]) || (global.user_forQobuz.enable_search_filter[1] != this->enable_filter_id[1])){
            this->filter_change = true;
        }
    }


    // 데이터 세팅 ---------------------------------------------------------------------------------------------------

    /**
     * @brief QobuzSearchMain::slot_applyResult_mostPopular
     * @param list_data
     * @param flag_lastPage
     */
    void QobuzSearchMain::slot_applyResult_mostPopular(const QJsonObject &jsonObj, const QString type){

        if(!jsonObj.empty()){

            this->mostPopular_type = type;

            if(type == "artists"){
                qobuz::ArtistItemData tmp_data = ConvertData::make_artistData_fromQobuzJsonObj(jsonObj);
                this->list_mostPopular_artist->append(tmp_data);
            }
            else if(type == "albums"){
                qobuz::AlbumItemData tmp_data = ConvertData::make_albumData_fromQobuzJsonObj(jsonObj);
                this->list_mostPopular_album->append(tmp_data);
            }
            else if(type == "tracks"){
                qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj);
                this->list_mostPopular_track->append(tmp_data);
            }
            else if(type == "playlists"){
                qobuz::PlaylistItemData tmp_data = ConvertData::make_playlistData_fromQobuzJsonObj(jsonObj);
                this->list_mostPopular_playlist->append(tmp_data);
            }

            this->flag_mostPopular[1] = true;
        }

        this->flag_mostPopular[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief QobuzSearchMain::slot_applyResult_artists
     * @param list_data
     * @param flag_lastPage
     */
    void QobuzSearchMain::slot_applyResult_artists(const QList<qobuz::ArtistItemData> &list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_artist->append(list_data);
            this->flag_artist[1] = true;
        }

        this->flag_artist[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief QobuzSearchMain::slot_applyResult_albums
     * @param list_data
     * @param jsonArr_data
     * @param flag_lastPage
     */
    void QobuzSearchMain::slot_applyResult_albums(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray& jsonArr_data, const bool flag_lastPage){

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
     * @brief QobuzSearchMain::slot_applyResult_tracks
     * @param list_data
     * @param jsonObj_track
     */
    void QobuzSearchMain::slot_applyResult_tracks(const QList<qobuz::TrackItemData> &list_data, const QJsonArray& jsonArr_dataToPlay, const bool flag_lastPage){

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
     * @brief QobuzSearchMain::slot_applyResult_playlists
     * @param list_data
     * @param flag_lastPage
     */
    void QobuzSearchMain::slot_applyResult_playlists(const QList<qobuz::PlaylistItemData> &list_data, const QJsonArray& jsonArr_data, const bool flag_lastPage){

        Q_UNUSED(jsonArr_data);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_playlist->append(list_data);
            this->flag_playlist[1] = true;
        }

        this->flag_playlist[0] = true;
        this->setUIControl_appendWidget();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void QobuzSearchMain::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

            // Qobuz favorite toggle check
            if(this->flag_send_track == true){
                if((status == true && this->flag_track_fav == false) || (status == false && this->flag_track_fav == true)){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzSearchMain::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }


    void QobuzSearchMain::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
        int id = ProcJsonEasy::getInt(jsonObj, "id");

        QJsonObject jsonObj_track = QJsonObject();
        jsonObj_track = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

        if(id <= 0){

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

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
            proc_fav_track->request_rose_addRating_Track(json);
        }
        else{

            QJsonObject tmp_album = ProcJsonEasy::getJsonObject(jsonObj_track, "album");

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
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void QobuzSearchMain::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void QobuzSearchMain::slot_applyResult_getRating_track(const QJsonArray &contents){

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
    void QobuzSearchMain::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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

                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzSearchMain::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("track", QString("%1").arg(this->track_id_fav), this->flag_track_fav);
                    this->flag_send_track = true;
                }

                this->track_idx_fav = idx;
                QJsonObject jsonObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &QobuzSearchMain::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("QOBUZ", QString("%1").arg(ProcJsonEasy::getInt(jsonObj, "id")));
                this->flag_check_track = true;

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
    void QobuzSearchMain::slot_clickedItemArtist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_mostPopular_artist){
            this->proc_clicked_itemArtist(this->list_mostPopular_artist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_artist){
            this->proc_clicked_itemArtist(this->list_artist, clickMode, index, section);
        }
    }

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzSearchMain::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_mostPopular_album){
            this->proc_clicked_itemAlbum(this->list_mostPopular_album, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_album){
            this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
        }
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzSearchMain::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___search_mostPopular_playlist){
            this->proc_clicked_itemPlaylist(this->list_mostPopular_playlist, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_playlist){
            this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
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
    void QobuzSearchMain::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___search_track){
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
        else if(section == SECTION_FOR_MORE_POPUP___search_mostPopular_artist){
            this->proc_clicked_optMorePopup_fromArtist(this->list_mostPopular_artist, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_mostPopular_album){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_mostPopular_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___search_mostPopular_playlist){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_mostPopular_playlist, index, section, clickMode);
        }
    }

}
