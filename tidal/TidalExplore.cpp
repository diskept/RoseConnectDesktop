#include "tidal/TidalExplore.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"

#include <QScroller>


namespace tidal {

    const int BTN_IDX_SUBTITLE_genreChoose = 1;
    const int BTN_IDX_SUBTITLE_suggestNewTracks = 2;
    const int BTN_IDX_SUBTITLE_suggestNewAlbums = 3;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___tracks = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM = 15;
    const int GET_ITEM_SIZE___ONCE_FOR_MAIN___TRACK = 5;


    // API 관련
    const QString TIDAL_API_SUGGESTED_ALBUM = "featured/recommended/albums";
    const QString TIDAL_API_SUGGESTED_TRACK = "featured/recommended/tracks";


    /**
     * @brief "TIDAL > 탐색" 화면을 위한 생성자. @see PAGECODE_T_EXPLORE
     * @param parent
     */
    TidalExplore::TidalExplore(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->list_track = new QList<tidal::TrackItemData>();
        this->list_album = new QList<tidal::AlbumItemData>();
    }

    /**
     * @brief 소멸자.
     */
    TidalExplore::~TidalExplore(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalExplore::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        this->flagSet_genre = false;
        this->flagSet_mood = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            this->list_album->clear();
            this->list_track->clear();

            this->jsonArr_tracks_toPlay = QJsonArray();

            this->flag_album[0] = false;
            this->flag_track[0] = false;

            this->flag_album[1] = false;
            this->flag_track[1] = false;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 타이달 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalExplore::setActivePage(){

        if(this->flagNeedReload == true){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_explore_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_explore_contents = new QVBoxLayout();
            this->box_explore_contents->setSpacing(0);
            this->box_explore_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_explore_contents = new QWidget();
            this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_explore_contents->setLayout(this->box_explore_contents);

            this->box_contents->addWidget(this->widget_explore_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // sub Title UI
            for(int i = 0; i < 5; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            // init UI
            this->setUIControl_chooseGenres();
            this->setUIControl_chooseMoods();
            this->setUIControl_btnH_part();

            // 데이터로 UI 적용 : 세팅이 안되어 있는 경우에만
            if(this->flagSet_genre == false){
                this->applyUIControl_chooseGenres_fromlistData();
            }
            if(this->flagSet_mood == false){
                this->applyUIControl_chooseMoods_fromlistData();
            }

            this->vBox_newTrack = new QVBoxLayout();
            this->vBox_newAlbum = new QVBoxLayout();

            this->hBox_newAlbum = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_newTrack);
            GSCommon::clearLayout(this->vBox_newAlbum);

            GSCommon::clearLayout(this->hBox_newAlbum);

            this->listWidget_track = new QListWidget(this);
            this->listWidget_track->setCursor(Qt::PointingHandCursor);
            this->listWidget_track->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            this->listWidget_track->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            this->listWidget_track->setFixedSize(QSize(1550, 350));

            this->listWidget_track->clear();

            for(int i = 0; i < 5; i++){
                this->listWidget_trackItem[i] = new QListWidgetItem();
                this->listWidget_trackItem[i]->setSizeHint(QSize(1550, 70));

                this->listWidget_track->addItem(this->listWidget_trackItem[i]);

            }

            for(int i = 0; i < 5; i++) {
                this->explore_newTrack[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->explore_newTrack[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &TidalExplore::slot_clickedItemTrack_inList);
                this->explore_newTrack[i]->setProperty("index", i);
                this->explore_newTrack[i]->setObjectName("newTrack");
            }

            for(int i = 0; i < 15; i++){
                this->explore_newAlbum[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_newAlbum[i], &tidal::ItemAlbum::signal_clicked, this, &TidalExplore::slot_clickedItemAlbum);
            }

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            tidal::ProcCommon *proc_album = new tidal::ProcCommon(this);
            connect(proc_album, &ProcCommon::completeReq_list_albums, this, &TidalExplore::slot_applyResult_albums);
            proc_album->request_tidal_getList_albums(TIDAL_API_SUGGESTED_ALBUM, GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM, 0);

            tidal::ProcCommon *proc_track = new tidal::ProcCommon(this);
            connect(proc_track, &ProcCommon::completeReq_list_tracks, this, &TidalExplore::slot_applyResult_tracks);
            proc_track->request_tidal_getList_tracks(TIDAL_API_SUGGESTED_TRACK, GET_ITEM_SIZE___ONCE_FOR_MAIN___TRACK, 0);
        }
    }


    void TidalExplore::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 장르 선택하는 부분. 최상단.
     */
    void TidalExplore::setUIControl_chooseGenres(){

        // add subTitle
        this->setUIControl_subTitle_withSideBtn("Genres", "View All", BTN_IDX_SUBTITLE_genreChoose, this->box_explore_contents);

        //----------------------------------------------------------------------------------------------------  BODY : START
        flowLayout_genre = new FlowLayout(0, 20, 20);
        flowLayout_genre->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(flowLayout_genre);
        widget_content->setContentsMargins(0, 10, 0, 0);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout
        this->box_explore_contents->addWidget(widget_content);
        this->box_explore_contents->addSpacing(30);
    }


    /**
     * @brief 상단에 Moods & Activities 선택 부분
     */
    void TidalExplore::setUIControl_chooseMoods(){

        // add subTitle
        this->setUIControl_subTitle_withSideBtn("Moods & Activities", "", 0, this->box_explore_contents);

        //----------------------------------------------------------------------------------------------------  BODY : START
        flowLayout_mood = new FlowLayout(0, 20, 20);
        flowLayout_mood->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(flowLayout_mood);
        widget_content->setContentsMargins(0, 10, 0, 0);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout
        this->box_explore_contents->addWidget(widget_content);
        this->box_explore_contents->addSpacing(30);
    }



    /**
     * @brief 장르 종류 UI 적용하기 (데이터를 활용)
     */
    void TidalExplore::applyUIControl_chooseGenres_fromlistData(){

        QString btn_style = this->getStyle_pushButton_optionChoose();

        int data_length = ProcCommon::getCount_listGenreInfo();
        if(data_length > 0){
            flagSet_genre = true;
        }

        for(int i = 0; i < data_length; i++){

            QString genre_name = ProcCommon::getGenreInfo(i).name;

            if(genre_name.contains("&")){
                genre_name.replace("&", "&&");
            }

            QPushButton *btn = new QPushButton();
            btn->setText(genre_name);
            btn->setStyleSheet(btn_style);
            btn->setProperty("idx", i);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, SIGNAL(clicked()), this, SLOT(slot_clickBtn_choseDetailGenre()));

            flowLayout_genre->addWidget(btn);
        }
    }


    /**
     * @brief Mood & Activity 종류 UI 적용하기 (데이터를 활용)
     */
    void TidalExplore::applyUIControl_chooseMoods_fromlistData(){

        // 버튼 공통 스타일
        QString btn_style = this->getStyle_pushButton_optionChoose();

        int data_length = ProcCommon::getCount_listMoodsList();
        if(data_length > 0){
            flagSet_mood = true;
        }

        for(int i = 0; i < data_length; i++){

            QPushButton *btn = new QPushButton;
            btn->setText(ProcCommon::getMoodInfo(i).name);
            btn->setStyleSheet(btn_style);
            btn->setProperty("idx", i);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, SIGNAL(clicked()), this, SLOT(slot_clickBtn_choseDetailMood()));

            flowLayout_mood->addWidget(btn);
        }
    }




    /**
     * @brief NEW, TOP, Rising 가로 버튼을 추가한다.
     */
    void TidalExplore::setUIControl_btnH_part(){

        QList<QString> list_img = QList<QString>{ ":/images/tidal/tidal-new-ico.png", ":/images/tidal/tidal-top-ico.png", ":/images/tidal/tidal-rising-ico.png" };
        QList<QString> list_name = QList<QString>{ "NEW", "TOP", "TIDAL Rising" };
        QList<TidalFeaturedPart> list_code = QList<TidalFeaturedPart>{ New, Top, Rising };

        for(int i = 0; i < list_img.count(); i++){

            //---------------------------------------------------------------------------------------- 버튼 영역 START

            QLabel *label_image = GSCommon::getUILabelImg(list_img.at(i));
            QLabel *label_text = new QLabel(list_name.at(i));
            label_text->setStyleSheet("font-size:18px; color:#FFFFFF;");

            QHBoxLayout *tmp_hBox_new = new QHBoxLayout();
            tmp_hBox_new->setContentsMargins(0, 0, 0, 0);
            tmp_hBox_new->setSpacing(0);
            tmp_hBox_new->setAlignment(Qt::AlignLeft);
            tmp_hBox_new->addWidget(label_image);
            tmp_hBox_new->addSpacing(30);
            tmp_hBox_new->addWidget(label_text, 1, Qt::AlignVCenter | Qt::AlignLeft);

            QPushButton *btn_clickableBox = new QPushButton();
            btn_clickableBox->setContentsMargins(10, 16, 10, 9);
            btn_clickableBox->setProperty("code", list_code.at(i));
            btn_clickableBox->setLayout(tmp_hBox_new);
            btn_clickableBox->setStyleSheet("QLabel{ background-color:transparent; } QPushButton:hover { background-color:#4d4d4d; } ");
            btn_clickableBox->setFixedHeight(70);
            btn_clickableBox->setCursor(Qt::PointingHandCursor);
            connect(btn_clickableBox, SIGNAL(clicked()), this, SLOT(slot_clickBtn_specificPart()));

            //---------------------------------------------------------------------------------------- 버튼 영역 END


            // 버튼 하단에 라인 넣기
            QWidget *widget_lineB = new QWidget();
            widget_lineB->setStyleSheet("background-color:#333333;");
            widget_lineB->setFixedHeight(1);

            QVBoxLayout *tmp_vBox_new = new QVBoxLayout();
            tmp_vBox_new->setContentsMargins(0, 0, 0, 0);
            tmp_vBox_new->setSpacing(0);
            tmp_vBox_new->addWidget(btn_clickableBox);
            tmp_vBox_new->addWidget(widget_lineB);


            QWidget *widget_hBtn = new QWidget();
            widget_hBtn->setLayout(tmp_vBox_new);
            widget_hBtn->setContentsMargins(0, 0, 0, 0);

            this->box_explore_contents->addWidget(widget_hBtn);
        }

        this->box_explore_contents->addSpacing(30);
    }


    void TidalExplore::setUIControl_appendWidget(){

        if(this->flag_track[0] == true && this->flag_album[0] == true){

            if(this->flag_track[1] == true){
                this->widget_newTrack = new QWidget();
                this->widget_newTrack = this->setUIControl_subTitle_withSideBtn(QString("Staff New Tracks For You (%1)").arg(this->list_track->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_suggestNewTracks, this->vBox_newTrack);

                this->vBox_newTrack->addSpacing(10);
                this->vBox_newTrack->addWidget(this->listWidget_track);

                int maxCount = this->list_track->size();
                if(this->list_track->size() > 5){
                    maxCount = 5;
                }

                QList<QString> tmp_clientkey;
                for(int i = 0; i < maxCount; i++){
                    this->explore_newTrack[i]->setDataTrackInfo_Tidal(this->list_track->at(i));
                    this->explore_newTrack[i]->resize(1550, 70);

                    //this->vBox_newTrack->addWidget(this->explore_newTrack[i]);
                    this->listWidget_track->setItemWidget(this->listWidget_trackItem[i], this->explore_newTrack[i]);

                    tmp_clientkey.append(QString("%1").arg(this->list_track->at(i).id));
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalExplore::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_getRating_Track("TIDAL", tmp_clientkey);

                this->box_explore_contents->addLayout(this->vBox_newTrack);
                this->box_explore_contents->addSpacing(30);
            }

            if(this->flag_album[1] == true){
                this->widget_newAlbum = new QWidget();
                this->widget_newAlbum = this->setUIControl_subTitle_withSideBtn(QString("Staff New Albums For You (%1)").arg(this->list_album->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_suggestNewAlbums, this->vBox_newAlbum);

                this->vBox_newAlbum->addSpacing(10);

                this->hBox_newAlbum = this->setUIControl_hBoxLayout_forAlbum(this->vBox_newAlbum);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->explore_newAlbum[i]->setData(this->list_album->at(i));
                    this->hBox_newAlbum->addWidget(this->explore_newAlbum[i]);
                }

                this->box_explore_contents->addLayout(this->vBox_newAlbum);
                this->box_explore_contents->addSpacing(30);
            }

            this->slot_hide_msg();
        }
    }


    QWidget* TidalExplore::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* TidalExplore::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : 페이지 이동 처리
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief 장르선택 우측에 "View as List" 클릭 - 장르목록 선택화면으로 이동 처리
     */
    void TidalExplore::movePage_genre_viewAsList(){

        QJsonObject tmp_data;
        tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_GENRES_CHOOSE_ALL);

        // 부모에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedMovePage(tmp_data);
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
    void TidalExplore::slot_applyResult_tracks(const QList<tidal::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0){

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->jsonArr_tracks_toPlay = jsonArr_dataToPlay;

            this->list_track->append(list_data);
            this->flag_track[1] = true;
        }

        this->flag_track[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalExplore::slot_applyResult_albums(const QList<tidal::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_album->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalExplore::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_genreChoose){
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_GENRES_CHOOSE_ALL);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_suggestNewTracks){
            PageInfo_TrackAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     // "Staff New Tracks For you";
            data_page.api_subPath = TIDAL_API_SUGGESTED_TRACK;
            this->movePage_track_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_suggestNewAlbums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     // "Staff New Albums For You";
            data_page.api_subPath = TIDAL_API_SUGGESTED_ALBUM;
            this->movePage_album_allView(data_page);
        }
    }



    /**
     * @brief 장르선택 뷰에서 "장르" 1개 선택한 경우임. 버튼 click과 연결되어 있는 slot
     */
    void TidalExplore::slot_clickBtn_choseDetailGenre(){

        int idx = sender()->property("idx").toInt();
        tidal::GenreInfo tmp_option = ProcCommon::getGenreInfo(idx);

        QJsonObject tmp_data;
        tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_GENRES_MAIN);
        tmp_data.insert("name", tmp_option.name);
        tmp_data.insert("path", tmp_option.path);

        // 부모에게 페이지 변경하라고 시그널 보냄
        emit this->signal_clickedMovePage(tmp_data);
    }


    /**
     * @brief 무드선택 뷰에서 "무드" 1개 선택한 경우임. 버튼 click과 연결되어 있는 slot
     */
    void TidalExplore::slot_clickBtn_choseDetailMood(){
        int idx = sender()->property("idx").toInt();
        tidal::MoodInfo tmp_option = ProcCommon::getMoodInfo(idx);

        QJsonObject tmp_data;
        tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_MOODS_MAIN);
        tmp_data.insert("name", tmp_option.name);
        tmp_data.insert("path", tmp_option.path);

        emit this->signal_clickedMovePage(tmp_data);            // 부모에게 페이지 변경하라고 시그널 보냄
    }


    /**
     * @brief NEW, TOP, Rising 버튼 클릭 - 각 파트별로 상세페이지 이동 처리
     */
    void TidalExplore::slot_clickBtn_specificPart(){

        TidalFeaturedPart part = static_cast<TidalFeaturedPart>(sender()->property("code").toInt());

        if(part == TidalFeaturedPart::New){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_NEW_MAIN);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }
        else if(part == TidalFeaturedPart::Top){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_TOP_MAIN);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }
        else if(part == TidalFeaturedPart::Rising){
            QJsonObject tmp_data;
            tmp_data.insert(KEY_PAGE_CODE, PAGECODE_T_EXPLORE_RISING_MAIN);
            emit this->signal_clickedMovePage(tmp_data);        // 부모에게 페이지 변경하라고 시그널 보냄
        }
    }


    void TidalExplore::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){
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
            track.insert("type", "TIDAL");

            QJsonArray tracks;
            tracks.append(track);

            QJsonObject json;
            json.insert("tracks", tracks);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalExplore::slot_applyResult_addRating_track);
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
            track.insert("type", "TIDAL");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "TIDAL");

            QJsonObject json;
            json.insert("ratingInfo", ratingInfo);
            json.insert("track", track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &TidalExplore::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

        }
    }


    void TidalExplore::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }

    void TidalExplore::slot_tidal_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){
        // Favorite 정보를 전달해줌. 알아서 처리하라고.
        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){

            // Tidal favorite check
            if(this->flag_send_track == true){
                QVariantList arr_myFavoriteIds = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
                bool status = arr_myFavoriteIds.contains(this->track_id_fav);

                if(status == true && this->flag_track_fav == false){
                    // Tidal track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalExplore::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(status == false && this->flag_track_fav == true){
                    // Tidal track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &tidal::ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalExplore::slot_tidal_completeReq_listAll_myFavoritesIds);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                this->flag_send_track = false;
            }
            else{

            }
        }
    }

    void TidalExplore::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->explore_newTrack[i]->setFavoritesIds(flag, star);
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
    void TidalExplore::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___tracks;

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_track == false){
                this->track_star_fav = this->explore_newTrack[idx]->getFavoritesStars();
                this->flag_track_fav = false;

                if(this->track_star_fav == 3){
                    this->track_star_fav = 0;
                    this->flag_track_fav = false;

                }
                else if(this->track_star_fav >= 0 && this->track_star_fav < 3){
                    this->track_star_fav++;
                    this->flag_track_fav = true;
                }

                this->track_idx_fav = idx;
                this->track_id_fav = this->list_track->at(idx).id;

                if(this->track_star_fav == 1){
                    // Tidal Track Favorite add
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_set_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }
                else if(this->track_star_fav == 0){
                    // Tidal Track Favorite del
                    ProcCommon *proc = new ProcCommon(this);
                    proc->request_tidal_del_favorite("tracks", QString("%1").arg(this->track_id_fav));
                }

                // favorite 정보 가져오기
                ProcCommon *proc_fav = new ProcCommon(this);
                connect(proc_fav, &ProcCommon::completeReq_listAll_myFavoritesIds, this, &TidalExplore::slot_tidal_completeReq_listAll_myFavoritesIds);
                proc_fav->request_tidal_getAll_favorites();
                this->flag_send_track = true;

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                connect(proc_favCheck_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &TidalExplore::slot_applyResult_checkRating_track);
                proc_favCheck_track->request_rose_checkRating_Track("TIDAL", QString("%1").arg(this->track_id_fav));
                this->flag_check_track = true;

                this->explore_newTrack[idx]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_track, this->jsonArr_tracks_toPlay, clickMode, idx, section);
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalExplore::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
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
    void TidalExplore::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___tracks){
            this->proc_clicked_optMorePopup_fromTrack(this->list_track, this->jsonArr_tracks_toPlay, index, clickMode);
        }
    }

}
