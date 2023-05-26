#include "qobuz/QobuzExplore.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QDebug>
#include <QScroller>


namespace qobuz {

    const int BTN_IDX_SUBTITLE_idealDiscography = 0;
    const int BTN_IDX_SUBTITLE_stillTranding = 1;
    const int BTN_IDX_SUBTITLE_pressAwards = 2;
    const int BTN_IDX_SUBTITLE_qobuzIssime = 3;
    const int BTN_IDX_SUBTITLE_topRelease = 4;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___idealdiscography = 0;
    const int SECTION_FOR_MORE_POPUP___stilltrending = 1;
    const int SECTION_FOR_MORE_POPUP___pressawards = 2;
    const int SECTION_FOR_MORE_POPUP___qobuzissime = 3;
    const int SECTION_FOR_MORE_POPUP___toprelease = 4;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM = 15;
    const int GET_ITEM_SIZE___ONCE_FOR_MAIN___TRACK = 5;


    // API 관련
    QString QOBUZ_API_SUGGESTED_ALBUM = "album/getFeatured";

    QString QOBUZ_API_IDEAL_DISCOGRAPHY = "ideal-discography";
    QString QOBUZ_API_STILL_TRANDING = "recent-releases";
    QString QOBUZ_API_PRESS_AWARDS = "press-awards";
    QString QOBUZ_API_QOBUZ_ISSIME = "qobuzissims";
    QString QOBUZ_API_TOP_TOP_RELEASE = "most-streamed";


    /**
     * @brief "TIDAL > 탐색" 화면을 위한 생성자. @see PAGECODE_T_EXPLORE
     * @param parent
     */
    QobuzExplore::QobuzExplore(QWidget *parent) : AbstractQobuzSubWidget(MainUIType::VerticalScroll_filter, parent) {

        // 기본 UI 세팅
        this->btnStyle_normal = "QPushButton{ ";
        this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:15px;";
        this->btnStyle_normal += "border:1px solid #707070; border-radius:20px;}";
        this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:15px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:20px;}";

        this->setUIControl_chooseGenres();

        this->list_idealDiscography = new QList<qobuz::AlbumItemData>();
        this->list_still_trending = new QList<qobuz::AlbumItemData>();
        this->list_press_awards = new QList<qobuz::AlbumItemData>();
        this->list_qobuz_issime = new QList<qobuz::AlbumItemData>();
        this->list_top_release = new QList<qobuz::AlbumItemData>();
    }


    /**
     * @brief 소멸자.
     */
    QobuzExplore::~QobuzExplore(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzExplore::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            if(this->flag_ideal[1] == true || this->flag_still[1] == true || this->flag_press[1] == true || this->flag_qobuz[1] == true || this->flag_top[1] == true){
                this->widget_explore_contents->hide();
            }

            this->page = pageCode;

            // init data
            this->list_idealDiscography->clear();
            this->list_still_trending->clear();
            this->list_press_awards->clear();
            this->list_qobuz_issime->clear();
            this->list_top_release->clear();

            this->jsonArr_albums = QJsonArray();

            this->flag_ideal[0] = false;
            this->flag_still[0] = false;
            this->flag_press[0] = false;
            this->flag_qobuz[0] = false;
            this->flag_top[0] = false;

            this->flag_ideal[1] = false;
            this->flag_still[1] = false;
            this->flag_press[1] = false;
            this->flag_qobuz[1] = false;
            this->flag_top[1] = false;

            this->flag_album_fav_chk = false;

            this->album_id_fav = "";
            this->album_idx_fav = 0;
            this->album_star_fav = 0;
            this->topRelease_idx = 0;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 코부즈 > 탐색 메뉴가 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void QobuzExplore::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            // 데이터로 UI 적용 : 세팅이 안되어 있는 경우에만
            if(flagSet_genre == false){
                if(global.user_forQobuz.flag_explore_filter == false){
                    global.user_forQobuz.flag_explore_filter = true;
                    for(int i = 0; i < 30; i++){
                        global.user_forQobuz.enable_explore_filter[i] = 0;
                        this->flag_filter[i] = false;
                    }
                }
                this->applyUIControl_chooseGenres_fromlistData();
            }

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

            // init UI
            this->vBox_idealDiscography = new QVBoxLayout();
            this->vBox_stillTranding = new QVBoxLayout();
            this->vBox_pressAwards = new QVBoxLayout();
            this->vBox_qobuzIssime = new QVBoxLayout();
            this->vBox_topRelease = new QVBoxLayout();

            this->hBox_idealDiscography = new QHBoxLayout();
            this->hBox_stillTranding = new QHBoxLayout();
            this->hBox_pressAwards = new QHBoxLayout();
            this->hBox_qobuzIssime = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_idealDiscography);
            GSCommon::clearLayout(this->vBox_stillTranding);
            GSCommon::clearLayout(this->vBox_pressAwards);
            GSCommon::clearLayout(this->vBox_qobuzIssime);
            GSCommon::clearLayout(this->vBox_topRelease);

            GSCommon::clearLayout(this->hBox_idealDiscography);
            GSCommon::clearLayout(this->hBox_stillTranding);
            GSCommon::clearLayout(this->hBox_pressAwards);
            GSCommon::clearLayout(this->hBox_qobuzIssime);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->explore_idealDiscography[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___idealdiscography, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_idealDiscography[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->explore_stillTranding[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___stilltrending, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_stillTranding[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->explore_pressAwards[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___pressawards, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_pressAwards[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
            }

            for(int i = 0; i < 15; i++){
                this->explore_qobuzIssime[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___qobuzissime, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_qobuzIssime[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
            }

            for (int i = 0; i < 5; i++) {
                this->explore_topRelease[i] = new PlaylistTrackDetailInfo_RHV;
                connect(this->explore_topRelease[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzExplore::slot_clickedItemTrack_inList);
                this->explore_topRelease[i]->setProperty("index", i);
                this->explore_topRelease[i]->setObjectName("topRelease");
            }

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_requestGenres();
        }
    }


    void QobuzExplore::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();

        for(int i = 0 ; i < this->btn_filter_cnt ; i++){

            this->btn_filter[i]->setEnabled(true);
        }
        this->btn_filter_clear->setEnabled(true);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 장르 선택하는 부분. 최상단.
     */
    void QobuzExplore::setUIControl_chooseGenres(){

        // 상단 필터
        /*this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        this->btn_filter_ico->setFixedSize(80,70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

        this->flow_genre_filter = new FlowLayout(0, 0, 10, 5);
        this->flow_genre_filter->setContentsMargins(10, 20, 10, 20);
        this->flow_genre_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->box_genre_filter = new QHBoxLayout();
        this->box_genre_filter->setContentsMargins(0,0,0,0);
        this->box_genre_filter->setSpacing(0);
        this->box_genre_filter->setAlignment(Qt::AlignLeft);
        this->box_genre_filter->addWidget(this->btn_filter_ico);
        this->box_genre_filter->setSpacing(10);
        this->box_genre_filter->addLayout(this->flow_genre_filter);

        this->btn_filter_cnt = ProcCommon::getCount_listGenreInfo();

        for(int i = 0; i < this->btn_filter_cnt; i++){
            this->lb_filter[i] = new QLabel();
            this->lb_filter[i]->setContentsMargins(20,0,20,0);
            this->lb_filter[i]->setFixedHeight(30);
            this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_filter[i]->setAlignment(Qt::AlignCenter);
        }

        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
        widget_btnFilter->setFixedHeight(70);
        widget_btnFilter->setLayout(this->box_genre_filter);

        box_filter->addWidget(widget_btnFilter);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));*/

        // 상단 필터
        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color: transparent; }");
        widget_btnFilter->setFixedSize(1680, 75);

        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png", widget_btnFilter);
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);
        this->btn_filter_ico->setGeometry(3, 0, 0, 0);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

        this->flow_genre_filter = new FlowLayout(0, 0, 10, 5);
        this->flow_genre_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_genre_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_genre_filter = new QWidget(widget_btnFilter);
        this->widget_genre_filter->setFixedSize(1500, 70);
        this->widget_genre_filter->setStyleSheet("background:transparent;");
        this->widget_genre_filter->setLayout(this->flow_genre_filter);
        this->widget_genre_filter->setGeometry(90, 5, 0, 0);

        this->box_filter->addWidget(widget_btnFilter);

        this->btn_filter_cnt = ProcCommon::getCount_listGenreInfo();

        for(int i = 0; i < this->btn_filter_cnt; i++){
            this->lb_filter[i] = new QLabel();
            this->lb_filter[i]->setContentsMargins(20, 0, 20, 0);
            this->lb_filter[i]->setFixedHeight(30);
            this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_filter[i]->setAlignment(Qt::AlignCenter);
        }
    }


    void QobuzExplore::setUIControl_requestGenres(){

        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        this->list_idealDiscography->clear();
        this->list_still_trending->clear();
        this->list_press_awards->clear();
        this->list_qobuz_issime->clear();

        this->list_top_release->clear();

        QString genre_ids = "";

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[0] == 1){
                genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(0).id);
                break;
            }
            else if(global.user_forQobuz.enable_explore_filter[i] == 1){
                if(genre_ids.isEmpty()){
                    genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(i).id);
                }
                else{
                    genre_ids += QString(", %1").arg(ProcCommon::getGenreInfo(i).id);
                }
            }
        }

        if(genre_ids.isEmpty()){
            genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(0).id);
        }

        // request HTTP API for Suggested
        qobuz::ProcCommon *proc_i = new qobuz::ProcCommon(this);
        connect(proc_i, &ProcCommon::completeReq_list_albums, this, &QobuzExplore::slot_applyResult_idealDiscography);
        proc_i->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_IDEAL_DISCOGRAPHY, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM, 0);

        qobuz::ProcCommon *proc_s = new qobuz::ProcCommon(this);
        connect(proc_s, &ProcCommon::completeReq_list_albums, this, &QobuzExplore::slot_applyResult_stillTrending);
        proc_s->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_STILL_TRANDING, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM, 0);

        qobuz::ProcCommon *proc_p = new qobuz::ProcCommon(this);
        connect(proc_p, &ProcCommon::completeReq_list_albums, this, &QobuzExplore::slot_applyResult_pressAwards);
        proc_p->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_PRESS_AWARDS, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM, 0);

        qobuz::ProcCommon *proc_q = new qobuz::ProcCommon(this);
        connect(proc_q, &ProcCommon::completeReq_list_albums, this, &QobuzExplore::slot_applyResult_qobuzIssime);
        proc_q->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_QOBUZ_ISSIME, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN___ALBUM, 0);

        qobuz::ProcCommon *proc = new qobuz::ProcCommon(this);
        connect(proc, &ProcCommon::completeReq_list_albums, this, &QobuzExplore::slot_applyResult_topRelease);
        proc->request_qobuz_getList_albums(QOBUZ_API_SUGGESTED_ALBUM, QOBUZ_API_TOP_TOP_RELEASE, genre_ids, GET_ITEM_SIZE___ONCE_FOR_MAIN___TRACK, 0);
    }


    /**
     * @brief 장르 종류 UI 적용하기 (데이터를 활용)
     */
    void QobuzExplore::applyUIControl_chooseGenres_fromlistData(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(735, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_genre = new QPushButton(widget_header);
        btn_close_genre->setText(tr("Select one or more genres"));
        btn_close_genre->setFixedSize(735, 50);
        btn_close_genre->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_genre->setGeometry(0, 0, 0, 0);
        btn_close_genre->setCursor(Qt::PointingHandCursor);

        connect(btn_close_genre, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QString btnStyle_clear = "QPushButton{ ";
        btnStyle_clear += "background:transparent;color:#FFFFFF;font-size:15px;";
        btnStyle_clear += "border:1px solid #707070; border-radius:15px;}";
        btnStyle_clear += "QPushButton:hover{background-color:#BE905F;}";

        this->btn_filter_clear = new QPushButton(btn_close_genre);
        this->btn_filter_clear->setGeometry(635, 8, 80, 30);
        this->btn_filter_clear->setText(tr("Clear"));
        this->btn_filter_clear->setStyleSheet(btnStyle_clear);
        this->btn_filter_clear->setCursor(Qt::PointingHandCursor);

        connect(this->btn_filter_clear, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_clear()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);

        this->flowLayout_genre = new FlowLayout(0, 0, 20, 20);
        this->flowLayout_genre->setContentsMargins(10, 20, 10, 20);
        this->flowLayout_genre->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(735, 210);
        widget_body->setStyleSheet("background:transparent;");
        widget_body->setLayout(this->flowLayout_genre);

        QHBoxLayout *box_body = new QHBoxLayout;
        box_body->setContentsMargins(0, 0, 0, 0);
        box_body->setSpacing(0);
        box_body->addWidget(widget_body);

        if(this->btn_filter_cnt > 0){
            this->flagSet_genre = true;
        }

        int select_count = 0;
        for(int i = 0 ; i < this->btn_filter_cnt ; i++){

            if(ProcCommon::getGenreInfo(i).name.contains("&")){
                this->str_genre_name[i] = ProcCommon::getGenreInfo(i).name.replace("&", "&&");
            }
            else{
                this->str_genre_name[i] = ProcCommon::getGenreInfo(i).name;
            }

            this->enable_genre_id[i] = 0;

            this->btn_filter[i] = new QPushButton();
            this->btn_filter[i]->setText(this->str_genre_name[i]);
            this->btn_filter[i]->setFixedSize(QSize(125, 40));
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                //this->btn_filter[i]->setStyleSheet(global.user_forQobuz.enable_explore_filter[i] == 1 ? this->btnStyle_selected : this->btnStyle_normal);
                this->btn_filter[i]->setStyleSheet(this->btnStyle_selected);
                select_count++;
            }
            else{
                this->btn_filter[i]->setStyleSheet(this->btnStyle_normal);
            }

            this->btn_filter[i]->setProperty("idx", i);
            this->btn_filter[i]->setCursor(Qt::PointingHandCursor);

            /*if(global.user_forQobuz.enable_explore_filter[i] == 1){
                this->lb_filter[i]->setText(this->str_genre_name[i]);
                this->box_genre_filter->addWidget(this->lb_filter[i]);
                this->flag_filter[i] = true;
            }*/

            connect(this->btn_filter[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseDetailGenre()));

            this->flowLayout_genre->addWidget(this->btn_filter[i]);
        }

        /*if(this->box_genre_filter->count() == 1){
            this->lb_filter[0]->setText(this->str_genre_name[0]);
            this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;
        }*/

        if(select_count == 0 || select_count == this->btn_filter_cnt){
            this->lb_filter[0]->setText(this->str_genre_name[0]);
            //this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flow_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;

            this->widget_genre_filter->setGeometry(90, 23, 0, 0);
        }
        else{
            int label_size = 0;
            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user_forQobuz.enable_explore_filter[i] == 1){
                    this->lb_filter[i]->setText(this->str_genre_name[i]);
                    //this->box_genre_filter->addWidget(this->lb_filter[i]);
                    this->flow_genre_filter->addWidget(this->lb_filter[i]);
                    this->flag_filter[i] = true;

                    label_size += (this->lb_filter[i]->sizeHint().width() + 10);
                }
            }

            if(label_size > 1500){
                this->widget_genre_filter->setGeometry(90, 5, 0, 0);
            }
            else{
                this->widget_genre_filter->setGeometry(90, 23, 0, 0);
            }
        }

        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addLayout(box_body);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(735, 260);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setFixedSize(735, 260);
        btn_total->setGeometry(0, 0, 0, 0);
        btn_total->setLayout(box_total);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    void QobuzExplore::setUIControl_appendWidget(){

        if(this->flag_ideal[0] == true && this->flag_still[0] == true && this->flag_press[0] == true && this->flag_qobuz[0] == true && this->flag_top[0] == true){

            if(this->flag_ideal[1] == true){
                this->widget_idealDiscography = new QWidget();
                this->widget_idealDiscography = this->setUIControl_subTitle_withSideBtn(QString("Ideal discography (%1)").arg(this->list_idealDiscography->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_idealDiscography, this->vBox_idealDiscography);

                this->vBox_idealDiscography->addSpacing(10);

                this->hBox_idealDiscography = this->setUIControl_hBoxLayout_forAlbum(this->vBox_idealDiscography);

                for(int i = 0; i < this->list_idealDiscography->size(); i++){
                    this->explore_idealDiscography[i]->setData(this->list_idealDiscography->at(i));
                    this->hBox_idealDiscography->addWidget(this->explore_idealDiscography[i]);
                }

                this->box_explore_contents->addLayout(this->vBox_idealDiscography);
                this->box_explore_contents->addSpacing(30);
            }

            if(this->flag_still[1] == true){
                this->widget_stillTranding = new QWidget();
                this->widget_stillTranding = this->setUIControl_subTitle_withSideBtn(QString("Still Tranding (%1)").arg(this->list_still_trending->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_stillTranding, this->vBox_stillTranding);

                this->vBox_stillTranding->addSpacing(10);

                this->hBox_stillTranding = this->setUIControl_hBoxLayout_forAlbum(this->vBox_stillTranding);

                for(int i = 0; i < this->list_still_trending->size(); i++){
                    this->explore_stillTranding[i]->setData(this->list_still_trending->at(i));
                    this->hBox_stillTranding->addWidget(this->explore_stillTranding[i]);
                }

                this->box_explore_contents->addLayout(this->vBox_stillTranding);
                this->box_explore_contents->addSpacing(30);
            }

            if(this->flag_press[1] == true){
                this->widget_pressAwards = new QWidget();
                this->widget_pressAwards = this->setUIControl_subTitle_withSideBtn(QString("Press awards (%1)").arg(this->list_press_awards->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_pressAwards, this->vBox_pressAwards);

                this->vBox_pressAwards->addSpacing(10);

                this->hBox_pressAwards = this->setUIControl_hBoxLayout_forAlbum(this->vBox_pressAwards);

                for(int i = 0; i < this->list_press_awards->size(); i++){
                    this->explore_pressAwards[i]->setData(this->list_press_awards->at(i));
                    this->hBox_pressAwards->addWidget(this->explore_pressAwards[i]);
                }

                this->box_explore_contents->addLayout(this->vBox_pressAwards);
                this->box_explore_contents->addSpacing(30);
            }

            if(this->flag_qobuz[1] == true){
                this->widget_qobuzIssime = new QWidget();
                this->widget_qobuzIssime = this->setUIControl_subTitle_withSideBtn(QString("Qoubzissme (%1)").arg(this->list_qobuz_issime->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_qobuzIssime, this->vBox_qobuzIssime);

                this->vBox_qobuzIssime->addSpacing(10);

                this->hBox_qobuzIssime = this->setUIControl_hBoxLayout_forAlbum(this->vBox_qobuzIssime);

                for(int i = 0; i < this->list_qobuz_issime->size(); i++){
                    this->explore_qobuzIssime[i]->setData(this->list_qobuz_issime->at(i));
                    this->hBox_qobuzIssime->addWidget(this->explore_qobuzIssime[i]);
                }

                this->box_explore_contents->addLayout(this->vBox_qobuzIssime);
                this->box_explore_contents->addSpacing(30);
            }

            if(this->flag_top[1] == true){
                this->widget_topRelease = new QWidget();
                this->widget_topRelease = this->setUIControl_subTitle_withSideBtn(QString("Top release (%1)").arg(this->list_top_release->at(0).album_total_cnt), "View All", BTN_IDX_SUBTITLE_topRelease, this->vBox_topRelease);

                this->vBox_topRelease->addSpacing(10);

                for(int i = 0; i < this->list_top_release->size(); i++){
                    this->explore_topRelease[i]->setDataAlbumInfo_Qobuz(this->list_top_release->at(i));
                    this->explore_topRelease[i]->resize(1550, 70);

                    this->vBox_topRelease->addWidget(this->explore_topRelease[i]);
                }

                this->box_explore_contents->addLayout(this->vBox_topRelease);
                this->box_explore_contents->addSpacing(30);

                if(this->flag_album_fav_chk == false){
                    this->album_idx_fav = 0;

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzExplore::slot_applyResult_getRating_album);
                    proc_favCheck_track->request_rose_getRating_Album("QOBUZ", QString("%1").arg(this->list_top_release->at(this->album_idx_fav).id));
                }
            }

            this->slot_hide_msg();
        }
    }


    void QobuzExplore::setUIControl_initialized(){

        if(this->flag_ideal[1] == true || this->flag_still[1] == true || this->flag_press[1] == true || this->flag_qobuz[1] == true || this->flag_top[1] == true){
            this->widget_explore_contents->hide();
        }

        // init data
        this->list_idealDiscography->clear();
        this->list_still_trending->clear();
        this->list_press_awards->clear();
        this->list_qobuz_issime->clear();
        this->list_top_release->clear();

        this->jsonArr_albums = QJsonArray();

        this->flag_ideal[0] = false;
        this->flag_still[0] = false;
        this->flag_press[0] = false;
        this->flag_qobuz[0] = false;
        this->flag_top[0] = false;

        this->flag_ideal[1] = false;
        this->flag_still[1] = false;
        this->flag_press[1] = false;
        this->flag_qobuz[1] = false;
        this->flag_top[1] = false;

        this->flag_album_fav_chk = false;

        // clear UI
        GSCommon::clearLayout(this->hBox_idealDiscography);
        GSCommon::clearLayout(this->hBox_stillTranding);
        GSCommon::clearLayout(this->hBox_pressAwards);
        GSCommon::clearLayout(this->hBox_qobuzIssime);

        GSCommon::clearLayout(this->vBox_idealDiscography);
        GSCommon::clearLayout(this->vBox_stillTranding);
        GSCommon::clearLayout(this->vBox_pressAwards);
        GSCommon::clearLayout(this->vBox_qobuzIssime);
        GSCommon::clearLayout(this->vBox_topRelease);

        GSCommon::clearLayout(this->box_explore_contents);
        this->box_contents->removeWidget(this->widget_explore_contents);

        this->box_explore_contents = new QVBoxLayout();
        this->box_explore_contents->setSpacing(0);
        this->box_explore_contents->setContentsMargins(0, 0, 0, 0);

        this->widget_explore_contents = new QWidget();
        this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_explore_contents->setLayout(this->box_explore_contents);

        this->box_contents->addWidget(this->widget_explore_contents, 0, Qt::AlignTop);

        // init UI
        this->vBox_idealDiscography = new QVBoxLayout();
        this->vBox_stillTranding = new QVBoxLayout();
        this->vBox_pressAwards = new QVBoxLayout();
        this->vBox_qobuzIssime = new QVBoxLayout();
        this->vBox_topRelease = new QVBoxLayout();

        this->hBox_idealDiscography = new QHBoxLayout();
        this->hBox_stillTranding = new QHBoxLayout();
        this->hBox_pressAwards = new QHBoxLayout();
        this->hBox_qobuzIssime = new QHBoxLayout();

        this->album_id_fav = "";
        this->album_idx_fav = 0;
        this->album_star_fav = 0;
        this->topRelease_idx = 0;

        // sub Title UI
        for(int i = 0; i < 10; i++){
            this->lb_subTitle[i] = new QLabel();
            this->btnView_all[i] = new QPushButton();
        }

        for(int i = 0; i < 15; i++){
            this->explore_idealDiscography[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___idealdiscography, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->explore_idealDiscography[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->explore_stillTranding[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___stilltrending, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->explore_stillTranding[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->explore_pressAwards[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___pressawards, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->explore_pressAwards[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
        }

        for(int i = 0; i < 15; i++){
            this->explore_qobuzIssime[i] = new qobuz::ItemAlbum_qobuz(i, SECTION_FOR_MORE_POPUP___qobuzissime, tidal::AbstractItem::ImageSizeMode::Square_200x200);
            connect(this->explore_qobuzIssime[i], &qobuz::ItemAlbum_qobuz::signal_clicked, this, &QobuzExplore::slot_clickedItemAlbum);
        }

        for (int i = 0; i < 5; i++) {
            this->explore_topRelease[i] = new PlaylistTrackDetailInfo_RHV;
            connect(this->explore_topRelease[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &QobuzExplore::slot_clickedItemTrack_inList);
            this->explore_topRelease[i]->setProperty("index", i);
            this->explore_topRelease[i]->setObjectName("topRelease");
        }
    }


    QWidget* QobuzExplore::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* QobuzExplore::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

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
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzExplore::slot_applyResult_idealDiscography(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_idealDiscography->append(list_data);
            this->flag_ideal[1] = true;
        }

        this->flag_ideal[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzExplore::slot_applyResult_stillTrending(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_still_trending->append(list_data);
            this->flag_still[1] = true;
        }

        this->flag_still[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzExplore::slot_applyResult_pressAwards(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_press_awards->append(list_data);
            this->flag_press[1] = true;
        }

        this->flag_press[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzExplore::slot_applyResult_qobuzIssime(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.length() > 0)
        {
            this->list_qobuz_issime->append(list_data);
            this->flag_qobuz[1] = true;
        }

        this->flag_qobuz[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzExplore::slot_applyResult_topRelease(const QList<qobuz::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.length() > 0)
        {
            this->list_top_release->append(list_data);
            this->flag_top[1] = true;
        }

        this->flag_top[0] = true;
        this->setUIControl_appendWidget();
    }


    void QobuzExplore::slot_applyResult_tracks(const QList<qobuz::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);
        Q_UNUSED(list_data);

        if(list_data.length() > 0){

            this->jsonArr_albums = QJsonArray();

            for(int i = 0; i < 5; i++){
                if(this->topRelease_idx == i){
                    this->jsonArr_albums.append(jsonArr_dataToPlay);
                }
                else{
                    QJsonArray tmpjsonArr = QJsonArray();
                    this->jsonArr_albums.append(tmpjsonArr);
                }
            }

            this->proc_clicked_itemAlbum_inList(this->list_top_release, this->jsonArr_albums, PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn, this->topRelease_idx, SECTION_FOR_MORE_POPUP___toprelease);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void QobuzExplore::slot_qobuz_completeReq_listAll_myFavoritesIds(const QJsonObject& p_jsonObj){

        // Favorite 정보를 전달해줌. 알아서 처리하라고. => OptMorePopup 에서 하라고, 가려줌
//        if(p_jsonObj.contains("flagOk") && ProcJsonEasy::get_flagOk(p_jsonObj)){
//            bool status  = ProcJsonEasy::getBool(p_jsonObj, "status");

//            // Qobuz favorite toggle check
//            if(this->flag_send_track == true){
//                if((status == true && this->flag_album_fav == false) || (status == false && this->flag_album_fav == true)){
//                    // Qobuz Favorite toggle
//                    ProcCommon *proc = new ProcCommon(this);
//                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzExplore::slot_qobuz_completeReq_listAll_myFavoritesIds);
//                    proc->request_qobuz_set_favorite("album", QString("%1").arg(this->album_id_fav), this->flag_album_fav);
//                }
//                this->flag_send_track = false;
//            }
//            else{

//            }
//        }
    }


    void QobuzExplore::slot_applyResult_checkRating_album(const QJsonObject &jsonObj){

        int id = ProcJsonEasy::getInt(jsonObj, "id");
        QJsonObject jsonObj_album = this->jsonArr_albums.at(this->album_idx_fav).toObject();

        if(id <= 0){

            QJsonObject album;
            album.insert("clientKey", ProcJsonEasy::getString(jsonObj_album, "id"));
            album.insert("favorite", true);
            album.insert("star", 1);

            QJsonObject jsonObj_image = ProcJsonEasy::getJsonObject(jsonObj_album, "image");
            album.insert("thumbnail", ProcJsonEasy::getString(jsonObj_image, "small"));
            album.insert("title", ProcJsonEasy::getString(jsonObj_album, "title"));
            album.insert("trackCount", 0);
            album.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("album", album);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &QobuzExplore::slot_applyResult_addRating_album);
            proc_fav_album->request_rose_addRating_Album(json);
        }
        else{

            QJsonObject album;
            album.insert("clientKey", ProcJsonEasy::getString(jsonObj_album, "id"));
            album.insert("id", id);
            album.insert("star", 0);
            album.insert("trackCount", 0);
            album.insert("type", "QOBUZ");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_album_fav);
            ratingInfo.insert("star", this->album_star_fav);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "QOBUZ");

            QJsonObject json;
            json.insert("album", album);
            json.insert("ratingInfo", ratingInfo);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzExplore::slot_applyResult_getRating_album);
            proc_fav_album->request_rose_setRating_Album(json, this->flag_album_fav, this->album_star_fav);
        }
    }


    void QobuzExplore::slot_applyResult_addRating_album(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
        else{
            this->flag_check_album = false;
        }
    }


    void QobuzExplore::slot_applyResult_getRating_album(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject tmp_star = contents.at(0).toObject();

            if(tmp_star.contains("star")){
                int star = ProcJsonEasy::getInt(tmp_star, "star");
                bool flag = star > 0 ? true : false;

                qDebug() << this->album_idx_fav;
                this->explore_topRelease[this->album_idx_fav]->setFavoritesIds(flag, star);

                if(this->flag_album_fav_chk == false && this->album_idx_fav < (this->list_top_release->size() - 1)){
                    this->album_idx_fav++;

                    // request HTTP API - get favorite for Rose Server
                    roseHome::ProcCommon *proc_favCheck_track = new roseHome::ProcCommon(this);
                    connect(proc_favCheck_track, &roseHome::ProcCommon::completeReq_rating_album, this, &QobuzExplore::slot_applyResult_getRating_album);
                    proc_favCheck_track->request_rose_getRating_Album("QOBUZ", QString("%1").arg(this->list_top_release->at(this->album_idx_fav).id));

                    if(this->album_idx_fav == this->list_top_release->size()){
                        this->flag_album_fav_chk = true;
                    }
                }
            }

            if(this->flag_check_album == true){
                this->flag_check_album = false;
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    // 서브 타이틀별 우측 View All ---------------------------------------------------------------------------------------------------

    /**
     * @brief 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void QobuzExplore::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        QString genre_ids = "";

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                if(genre_ids.isEmpty()){
                    genre_ids = QString("%1").arg(ProcCommon::getGenreInfo(i).id);
                }
                else{
                    genre_ids += QString(", %1").arg(ProcCommon::getGenreInfo(i).id);
                }
            }
        }

        if(btnId == BTN_IDX_SUBTITLE_idealDiscography){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     //"Ideal discography";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_IDEAL_DISCOGRAPHY;
            data_page.genre_ids = genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_IDEAL_DISCOGRAPHY_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_stillTranding){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     //"Still trending";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_STILL_TRANDING;
            data_page.genre_ids = genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_STILL_TRENDING_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_pressAwards){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     //"Press awards";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_PRESS_AWARDS;
            data_page.genre_ids = genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_PRESS_AWARDS_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
        else if(btnId == BTN_IDX_SUBTITLE_qobuzIssime){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     //"Qobuzissime";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_QOBUZ_ISSIME;
            data_page.genre_ids = genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_QOBUZ_ISSIME_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);            // 페이지 이동 signal
        }
        else if(btnId == BTN_IDX_SUBTITLE_topRelease){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();     //"Top release";
            data_page.api_subPath = QOBUZ_API_SUGGESTED_ALBUM;
            data_page.type = QOBUZ_API_TOP_TOP_RELEASE;
            data_page.genre_ids = genre_ids;

            QJsonObject jsonObj_move = ConvertData::getObjectJson_pageInfo_albumAllView(data_page);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_TOPRELEASE_ALL_LIST_VIEW);
            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }


    void QobuzExplore::slot_clickBtn_Filter(){

        widget_filter->setVisible(!widget_filter->isVisible());
    }


    void QobuzExplore::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void QobuzExplore::slot_clickBtn_Filter_clear(){

        int filter_select = 0;
        int filter_total = 0;
        int filter_before = 0;

        for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){

            if(global.user_forQobuz.enable_explore_filter[sel_idx] == 0){
                filter_select++;
            }

            this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
            global.user_forQobuz.enable_explore_filter[sel_idx] = 0;
        }

        for(int i = 0; i < filter_select; i++){
            if(this->flag_filter[i] == true){
                this->lb_filter[i]->setText("");
                this->lb_filter[i]->setStyleSheet("");

                //this->box_genre_filter->removeWidget(this->lb_filter[i]);
                this->flow_genre_filter->removeWidget(this->lb_filter[i]);
                this->flag_filter[i] = false;
            }
        }

        //this->box_genre_filter->removeWidget(this->btn_filter_ico);
        //this->box_genre_filter->removeWidget(this->widget_genre_filter);
        //GSCommon::clearLayout(this->box_genre_filter);
        GSCommon::clearLayout(this->box_filter);
        this->setUIControl_chooseGenres();

        filter_select = 0;
        filter_total = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
        }

        if((filter_select == 0) || (filter_select == filter_total)){
            this->lb_filter[0]->setText(ProcCommon::getGenreInfo(0).name);
            this->lb_filter[0]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

            //this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flow_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;

            this->widget_genre_filter->setGeometry(90, 23, 0, 0);
        }
        else{
            int label_size = 0;
            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user_forQobuz.enable_explore_filter[i] == 1){
                    this->lb_filter[i]->setText(this->str_genre_name[i]);
                    this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

                    //this->box_genre_filter->addWidget(this->lb_filter[i]);
                    this->flow_genre_filter->addWidget(this->lb_filter[i]);
                    this->flag_filter[i] = true;
                    label_size += (this->lb_filter[i]->sizeHint().width() + 10);
                    filter_select += i;
                }
                filter_total += i;
            }

            if(label_size > 1500){
                this->widget_genre_filter->setGeometry(90, 5, 0, 0);
            }
            else{
                this->widget_genre_filter->setGeometry(90, 23, 0, 0);
            }
        }


        filter_select = 0;
        filter_total = 0;
        filter_before = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
            filter_before += this->enable_genre_id[i];
        }

        if((global.user_forQobuz.enable_explore_filter[0] == 0) && (filter_select == filter_total)){
            this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
            global.user_forQobuz.enable_explore_filter[0] = 1;
        }

        bool reload_frame = false;
        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(this->enable_genre_id[i] != global.user_forQobuz.enable_explore_filter[i]){
                if((filter_before == 0) && (filter_select == filter_total)){
                    reload_frame = false;
                }
                else if((filter_before == this->btn_filter_cnt) && (filter_select == 0)){
                    reload_frame = false;
                }
                else{
                    reload_frame = true;
                }
                break;
            }
        }

        if(reload_frame == true){

            for(int i = 0 ; i < this->btn_filter_cnt ; i++){

                this->btn_filter[i]->setEnabled(false);
            }
            this->btn_filter_clear->setEnabled(false);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_initialized();

            this->setUIControl_requestGenres();

            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_explore_filter[i];
            }
        }
        else{
            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_explore_filter[i];
            }
        }
    }


    /**
     * @brief 장르선택 뷰에서 "장르" 1개 선택한 경우임. 버튼 click과 연결되어 있는 slot
     */
    void QobuzExplore::slot_clickBtn_chooseDetailGenre(){

        int filter_select = 0;
        int filter_total = 0;
        int filter_before = 0;

        int idx = sender()->property("idx").toInt();

        if(idx == 0){
            if(global.user_forQobuz.enable_explore_filter[idx] == 0){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_selected);
                    global.user_forQobuz.enable_explore_filter[sel_idx] = 1;
                }
            }
            else if(global.user_forQobuz.enable_explore_filter[idx] == 1){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    global.user_forQobuz.enable_explore_filter[sel_idx] = 0;
                }
            }
        }
        else{
            if(global.user_forQobuz.enable_explore_filter[0] == 1){
                this->btn_filter[0]->setStyleSheet(this->btnStyle_normal);
                global.user_forQobuz.enable_explore_filter[0] = 0;
            }

            if(global.user_forQobuz.enable_explore_filter[idx] == 1){
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_normal);
                global.user_forQobuz.enable_explore_filter[idx] = 0;
            }
            else if(global.user_forQobuz.enable_explore_filter[idx] == 0){
                this->btn_filter[idx]->setStyleSheet(this->btnStyle_selected);
                global.user_forQobuz.enable_explore_filter[idx] = 1;
            }

        }

        //for(int i = 0; i <= this->btn_filter_cnt; i++){
            if(this->flag_filter[idx] == true){
                this->lb_filter[idx]->setText("");
                this->lb_filter[idx]->setStyleSheet("");

                //this->box_genre_filter->removeWidget(this->lb_filter[idx]);
                this->flow_genre_filter->addWidget(this->lb_filter[idx]);
                this->flag_filter[idx] = false;
            }
        //}

        //this->box_genre_filter->removeWidget(this->btn_filter_ico);
        //this->box_genre_filter->removeWidget(this->widget_genre_filter);
        //GSCommon::clearLayout(this->box_genre_filter);
        GSCommon::clearLayout(this->box_filter);
        this->setUIControl_chooseGenres();
        //this->slot_clickBtn_Filter_close();

        filter_select = 0;
        filter_total = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
        }

        if((filter_select == 0) || (filter_select == filter_total)){
            this->lb_filter[0]->setText(ProcCommon::getGenreInfo(0).name);
            this->lb_filter[0]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

            //this->box_genre_filter->addWidget(this->lb_filter[0]);
            this->flow_genre_filter->addWidget(this->lb_filter[0]);
            this->flag_filter[0] = true;

            this->widget_genre_filter->setGeometry(90, 23, 0, 0);
        }
        else{
            int label_size = 0;
            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user_forQobuz.enable_explore_filter[i] == 1){
                    this->lb_filter[i]->setText(this->str_genre_name[i]);
                    this->lb_filter[i]->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");

                    //this->box_genre_filter->addWidget(this->lb_filter[i]);
                    this->flow_genre_filter->addWidget(this->lb_filter[i]);
                    this->flag_filter[i] = true;
                    label_size += (this->lb_filter[i]->sizeHint().width() + 10);
                    filter_select += i;
                }
                filter_total += i;
            }

            if(label_size > 1500){
                this->widget_genre_filter->setGeometry(90, 5, 0, 0);
            }
            else{
                this->widget_genre_filter->setGeometry(90, 23, 0, 0);
            }
        }

        filter_select = 0;
        filter_total = 0;
        filter_before = 0;

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user_forQobuz.enable_explore_filter[i] == 1){
                filter_select += i;
            }
            filter_total += i;
            filter_before += this->enable_genre_id[i];
        }

        if((global.user_forQobuz.enable_explore_filter[0] == 0) && (filter_select == filter_total)){
            this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
            global.user_forQobuz.enable_explore_filter[0] = 1;
        }

        bool reload_frame = false;
        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(this->enable_genre_id[i] != global.user_forQobuz.enable_explore_filter[i]){
                if((filter_before == 0) && (filter_select == filter_total)){
                    reload_frame = false;
                }
                else if((filter_before == this->btn_filter_cnt) && (filter_select == 0)){
                    reload_frame = false;
                }
                else{
                    reload_frame = true;
                }
                break;
            }
        }

        if(reload_frame == true){

            for(int i = 0 ; i < this->btn_filter_cnt ; i++){

                this->btn_filter[i]->setEnabled(false);
            }
            this->btn_filter_clear->setEnabled(false);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_initialized();

            this->setUIControl_requestGenres();

            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_explore_filter[i];
            }
        }
        else{
            for(int i = 0; i < this->btn_filter_cnt; i++){
                this->enable_genre_id[i] = global.user_forQobuz.enable_explore_filter[i];
            }
        }
    }


    /**
     * @brief [slot] TrackDelegate로부터 signal 받은
     * @param index
     * @param clickMode
     */
    void QobuzExplore::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        int section = SECTION_FOR_MORE_POPUP___toprelease;

        // ClickMode 별로 처리
        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){

            if(this->flag_check_album == false){

                this->album_star_fav = this->explore_topRelease[idx]->getFavoritesStars();
                this->flag_album_fav = false;

                if(this->album_star_fav == 3){
                    this->album_star_fav = 0;
                    this->flag_album_fav = false;
                }
                else if(this->album_star_fav >= 0 && this->album_star_fav < 3){
                    this->album_star_fav++;
                    this->flag_album_fav = true;
                }

                if(this->album_star_fav == 0 || this->album_star_fav == 1){
                    // Qobuz Favorite toggle
                    ProcCommon *proc = new ProcCommon(this);
                    connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &QobuzExplore::slot_qobuz_completeReq_listAll_myFavoritesIds);
                    proc->request_qobuz_set_favorite("album", QString("%1").arg(this->list_top_release->at(idx).id), this->flag_album_fav);
                    this->flag_send_track = true;
                }

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_favCheck_album = new roseHome::ProcCommon(this);
                connect(proc_favCheck_album, &roseHome::ProcCommon::completeCheck_rating_album, this, &QobuzExplore::slot_applyResult_checkRating_album);
                proc_favCheck_album->request_rose_checkRating_Album("QOBUZ", this->list_top_release->at(idx).id);
                this->flag_check_album = true;

                this->explore_topRelease[idx]->setFavoritesIds(this->flag_album_fav, this->album_star_fav);
            }
        }
        else{
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){

                this->topRelease_idx = idx;

                // request HTTP API
                ProcCommon *proc_album = new ProcCommon(this);
                connect(proc_album, &ProcCommon::completeReq_list_items_of_album, this, &QobuzExplore::slot_applyResult_tracks);
                proc_album->request_qobuz_get_albuminfo(this->list_top_release->at(this->topRelease_idx).id);
            }
            else{
                this->proc_clicked_itemAlbum_inList(this->list_top_release, this->jsonArr_albums, clickMode, idx, section);
            }
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void QobuzExplore::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
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
    void QobuzExplore::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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
