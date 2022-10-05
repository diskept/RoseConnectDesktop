#include "roseHome/rosehomeFavorite.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"

#include <QScrollBar>
#include <QScroller>


namespace roseHome {

    const int SECTION_FOR_MORE_POPUP___FavoriteTrack = 0;
    const int SECTION_FOR_MORE_POPUP___FavoriteALBUM = 1;
    const int SECTION_FOR_MORE_POPUP___FavoritePLAYLIST = 2;

    const QString STEP_TRACK = "track";
    const QString STEP_ALBUM = "album";
    const QString STEP_PLAYLIST = "playlist";

    const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";
    const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";

    /**
     * @brief "ROSEHOME > Favorite" 화면을 위한 생성자. @see PAGECODE_RH_FAVORITE
     * @param parent
     */
    RoseFavorite::RoseFavorite(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        // 기본 UI 세팅
        this->btnStyle_normal = "QPushButton{ ";
        this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:15px;";
        this->btnStyle_normal += "border:1px solid #707070; border-radius:20px;}";
        this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:15px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:20px;}";

        this->mediaTypeList << tr("All Music") << tr("Mix") << tr("Music") << tr("RoseTube") << tr("Tidal") << tr("Bugs") << tr("Qobuz") << tr("Apple Music");      // << "VIDEO" << "SPOTIFY";
        this->orderTypeList << tr("Recent") << tr("Prefrence") << tr("Play Frequency") << tr("Title ASC") << tr("Title DESC") << tr("Favorites");

        this->setUIControl_chooseFilter();

        // data
        this->list_favoriteTrack = new QList<roseHome::TrackItemData>();
        this->list_favoriteAlbum = new QList<roseHome::AlbumItemData>();
        this->list_favoritePlaylist = new QList<roseHome::PlaylistItemData>();
    }


    /**
    * @brief 소멸자.
    */
    RoseFavorite::~RoseFavorite(){

        this->deleteLater();
    }


    void RoseFavorite::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;
        }
    }

    /**
     * @brief RoseFavorite::setActivePage
     */
    void RoseFavorite::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // 데이터로 UI 적용 : 세팅이 안되어 있는 경우에만
            if(this->flagSet_media == false){
                if(global.user.flag_favMusic_order == false){
                    global.user.flag_favorite_filter = true;
                    for(int i = 0; i < 30; i++){
                        global.user.enable_favorite_filter[i] = 0;
                        this->flag_filter[i] = false;
                    }
                    for(int j = 0; j < 10; j++){
                        global.user.enable_favorite_order[j] = 0;
                        this->flag_order[j] = false;
                    }
                }
                this->setUIControl_chooseFilter_fromData();
            }

            GSCommon::clearLayout(this->box_favorite_track);
            GSCommon::clearLayout(this->flowLayout_favorite_album);
            GSCommon::clearLayout(this->flowLayout_favorite_playlist);

            this->contentStep = "";
            this->setUIControl_chooseStep(STEP_TRACK);
        }
    }


    /**
     * @brief Filter 선택하는 부분. 최상단.
     */
    void RoseFavorite::setUIControl_chooseFilter(){

        // 상단 필터
        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

        this->box_media_filter  = new QHBoxLayout();
        this->box_media_filter->setContentsMargins(0,0,0,0);
        this->box_media_filter->setSpacing(0);
        this->box_media_filter->setAlignment(Qt::AlignLeft);
        this->box_media_filter->addWidget(this->btn_filter_ico);
        this->box_media_filter->setSpacing(10);

        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121; }");
        widget_btnFilter->setLayout(this->box_media_filter);


        QWidget *widget_btnStep = new QWidget();
        widget_btnStep->setObjectName("widget_btnStep");
        widget_btnStep->setStyleSheet("#widget_btnStep { background-color:#212121; }");
        widget_btnStep->setFixedSize(405, 70);
        widget_btnStep->setContentsMargins(0, 0, 0, 5);

        this->btn_step_track = new QPushButton(tr("Track"), widget_btnStep);
        this->btn_step_track->setObjectName("btn_step_track");
        this->btn_step_track->setFixedSize(135, 40);
        this->btn_step_track->setGeometry(1, 15, 0, 0);
        this->btn_step_track->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), STEP_TRACK);
        this->btn_step_track->setCursor(Qt::PointingHandCursor);

        this->btn_step_album = new QPushButton(tr("Album"), widget_btnStep);
        this->btn_step_album->setObjectName("btn_step_album");
        this->btn_step_album->setFixedSize(135, 40);
        this->btn_step_album->setGeometry(135, 15, 0, 0);
        this->btn_step_album->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), STEP_ALBUM);
        this->btn_step_album->setCursor(Qt::PointingHandCursor);

        this->btn_step_playlist = new QPushButton(tr("Playlist"), widget_btnStep);
        this->btn_step_playlist->setObjectName("btn_step_playlist");
        this->btn_step_playlist->setFixedSize(135, 40);
        this->btn_step_playlist->setGeometry(269, 15, 0, 0);
        this->btn_step_playlist->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), STEP_PLAYLIST);
        this->btn_step_playlist->setCursor(Qt::PointingHandCursor);

        this->box_filter->addWidget(widget_btnFilter, 0, Qt::AlignLeft);
        this->box_filter->addWidget(widget_btnStep, 1, Qt::AlignRight);


        this->btn_filter_cnt = this->mediaTypeList.size();
        this->btn_order_cnt = this->orderTypeList.size();

        this->lb_media = new QLabel();
        this->lb_media->setContentsMargins(20,0,20,0);
        this->lb_media->setFixedHeight(30);
        this->lb_media->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
        this->lb_media->setAlignment(Qt::AlignCenter);

        this->lb_order = new QLabel();
        this->lb_order->setContentsMargins(20,0,20,0);
        this->lb_order->setFixedHeight(30);
        this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
        this->lb_order->setAlignment(Qt::AlignCenter);


        this->box_favorite_track = new QVBoxLayout();
        this->box_favorite_track->setContentsMargins(5, 0, 5, 0);
        this->box_favorite_track->setSpacing(0);
        this->box_favorite_track->setAlignment(Qt::AlignTop);

        this->flowLayout_favorite_album = new FlowLayout(0, 0, 20);
        this->flowLayout_favorite_album->setSizeConstraint(QLayout::SetMinimumSize);

        this->flowLayout_favorite_playlist = new FlowLayout(0, 0, 20);
        this->flowLayout_favorite_playlist->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_tab_track = new QWidget();
        this->widget_tab_track->setLayout(this->box_favorite_track);

        this->widget_tab_album = new QWidget();
        this->widget_tab_album->setLayout(this->flowLayout_favorite_album);
        this->widget_tab_album->setContentsMargins(0, 20, 0, 0);
        this->widget_tab_album->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->widget_tab_album->setStyleSheet("background-color:transparent;");

        this->widget_tab_playlist = new QWidget();
        this->widget_tab_playlist->setLayout(this->flowLayout_favorite_playlist);
        this->widget_tab_playlist->setContentsMargins(0, 20, 0, 0);
        this->widget_tab_playlist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->widget_tab_playlist->setStyleSheet("background-color:transparent;");

        this->stackedWidget_body = new QStackedWidget();
        this->stackedWidget_body->addWidget(this->widget_tab_track);
        this->stackedWidget_body->addWidget(this->widget_tab_album);
        this->stackedWidget_body->addWidget(this->widget_tab_playlist);
        this->stackedWidget_body->hide();

        this->box_main_contents = new QVBoxLayout();
        this->box_main_contents->setSpacing(0);
        this->box_main_contents->setContentsMargins(0, 0, 0, 0);
        this->box_main_contents->setAlignment(Qt::AlignTop);
        this->box_main_contents->addWidget(this->stackedWidget_body);

        this->box_contents->addLayout(this->box_main_contents);
        this->box_contents->addSpacing(30);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // 커넥션
        connect(this->btn_step_track, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
        connect(this->btn_step_album, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
        connect(this->btn_step_playlist, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));
    }


    void RoseFavorite::setUIControl_chooseFilter_fromData(){

        QPushButton *btn_close_media = new QPushButton();
        btn_close_media->setText(tr("Select Media Type"));
        btn_close_media->setFixedSize(470, 50);
        btn_close_media->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_media->setContentsMargins(20, 0, 0, 0);
        btn_close_media->setCursor(Qt::PointingHandCursor);

        connect(btn_close_media, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(btn_close_media);

        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFixedHeight(1);
        line->setLineWidth(1);
        line->setStyleSheet("background-color:#4D4D4D;border:none;");

        this->flowLayout_media = new FlowLayout(0, 20, 20);
        this->flowLayout_media->setContentsMargins(10, 10, 10, 20);
        this->flowLayout_media->setSizeConstraint(QLayout::SetMinimumSize);

        if(this->btn_filter_cnt > 0){
            this->flagSet_media = true;
        }

        int check_btn = 0;
        for(int i = 0 ; i < this->btn_filter_cnt; i++){

            this->str_media_name[i] = this->mediaTypeList.at(i);

            this->enable_media_id[i] = 0;

            this->btn_filter[i] = new QPushButton();
            this->btn_filter[i]->setText(this->str_media_name[i]);
            this->btn_filter[i]->setFixedSize(QSize(135, 40));
            this->btn_filter[i]->setStyleSheet(global.user.enable_favorite_filter[i] == 1 ? this->btnStyle_selected : this->btnStyle_normal);
            this->btn_filter[i]->setObjectName("media");
            this->btn_filter[i]->setProperty("idx", i);
            this->btn_filter[i]->setCursor(Qt::PointingHandCursor);

            if(global.user.enable_favorite_filter[i] == 1){
                this->lb_media->setText(this->str_media_name[i]);
                this->box_media_filter->addWidget(this->lb_media);
                this->flag_filter[i] = true;
                check_btn++;
            }

            connect(this->btn_filter[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseDetailMedia()));

            this->flowLayout_media->addWidget(this->btn_filter[i]);
        }

        if(check_btn == 0){
            this->btn_filter[0]->setStyleSheet(this->btnStyle_selected);
            this->lb_media->setText(this->str_media_name[0]);
            this->box_media_filter->addWidget(this->lb_media);
            this->flag_filter[0] = true;
            global.user.enable_favorite_filter[0] = 1;
        }


        QFrame *line2 = new QFrame;
        line2->setFrameShape(QFrame::HLine);
        line2->setFixedHeight(1);
        line2->setLineWidth(1);
        line2->setStyleSheet("background-color:#4D4D4D;border:none;");

        QPushButton *btn_close_order = new QPushButton();
        btn_close_order->setText(tr("Select Sort Type"));
        btn_close_order->setFixedSize(470, 50);
        btn_close_order->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_order->setContentsMargins(20, 0, 0, 0);
        btn_close_order->setCursor(Qt::PointingHandCursor);

        connect(btn_close_order, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *box_body = new QHBoxLayout;
        box_body->setContentsMargins(0, 0, 0, 0);
        box_body->setSpacing(0);
        box_body->addWidget(btn_close_order);

        QFrame *line3 = new QFrame;
        line3->setFrameShape(QFrame::HLine);
        line3->setFixedHeight(1);
        line3->setLineWidth(1);
        line3->setStyleSheet("background-color:#4D4D4D;border:none;");

        this->flowLayout_order = new FlowLayout(0, 20, 20);
        this->flowLayout_order->setContentsMargins(10, 10, 10, 10);
        this->flowLayout_order->setSizeConstraint(QLayout::SetMinimumSize);

        if(this->btn_order_cnt > 0){
            this->flagSet_order = true;
        }

        check_btn = 0;
        for(int j = 0; j < this->btn_order_cnt; j++){
            this->str_order_name[j] = this->orderTypeList.at(j);

            this->enable_order_id[j] = 0;

            this->btn_order[j] = new QPushButton();
            this->btn_order[j]->setText(this->str_order_name[j]);
            this->btn_order[j]->setFixedSize(QSize(135, 40));
            this->btn_order[j]->setStyleSheet(global.user.enable_favorite_order[j] == 1 ? this->btnStyle_selected : this->btnStyle_normal);
            this->btn_order[j]->setObjectName("order");
            this->btn_order[j]->setProperty("idx", j);
            this->btn_order[j]->setCursor(Qt::PointingHandCursor);

            if(global.user.enable_favorite_order[j] == 1){
                this->lb_order->setText(this->str_order_name[j]);
                this->box_media_filter->addWidget(this->lb_order);
                this->flag_order[j] = true;
                check_btn++;
            }

            connect(this->btn_order[j], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseDetailMedia()));

            this->flowLayout_order->addWidget(this->btn_order[j]);
        }

        if(check_btn == 0){
            this->btn_order[0]->setStyleSheet(this->btnStyle_selected);
            this->lb_order->setText(this->str_order_name[0]);
            this->box_media_filter->addWidget(this->lb_order);
            this->flag_order[0] = true;
            global.user.enable_favorite_order[0] = 1;
        }


        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addWidget(line);
        box_total->addLayout(this->flowLayout_media, 0);
        box_total->addWidget(line2);
        box_total->addLayout(box_body);
        box_total->addWidget(line3);
        box_total->addLayout(this->flowLayout_order, 1);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(470, 420);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);
        //this->widget_filter->setLayout(box_total);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setLayout(box_total);
        btn_total->setFixedSize(470, 420);
        btn_total->setGeometry(0, 0, 0, 0);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    void RoseFavorite::setUIControl_chooseStep(QString type){

        this->step_change = false;
        if(this->contentStep != type){
            this->contentStep = "";
            this->contentStep = type;
            this->step_change = true;

            // 버튼 스타일 초기화
            this->btn_step_track->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_track").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
            this->btn_step_album->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_step_album").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
            this->btn_step_playlist->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_playlist").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

            for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
                this->flag_filter[sel_idx] = false;
                global.user.enable_favorite_filter[sel_idx] = 0;
            }

            for(int sel_idx = 0; sel_idx < this->btn_order_cnt; sel_idx++){
                this->btn_order[sel_idx]->setStyleSheet(this->btnStyle_normal);
                this->flag_order[sel_idx] = false;
                global.user.enable_favorite_order[sel_idx] = 0;
            }

            GSCommon::clearLayout(box_media_filter);

            this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
            this->btn_filter_ico->setFixedSize(80, 70);
            this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

            connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

            this->lb_media = new QLabel();
            this->lb_media->setContentsMargins(20,0,20,0);
            this->lb_media->setFixedHeight(30);
            this->lb_media->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_media->setAlignment(Qt::AlignCenter);

            this->lb_order = new QLabel();
            this->lb_order->setContentsMargins(20,0,20,0);
            this->lb_order->setFixedHeight(30);
            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_order->setAlignment(Qt::AlignCenter);

            this->box_media_filter->addWidget(this->btn_filter_ico);
            this->box_media_filter->setSpacing(10);

            if(this->contentStep == STEP_TRACK){
                this->btn_order[5]->hide();

                this->btn_filter[global.user.enable_favRTrack_filter]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[global.user.enable_favRTrack_filter] = true;
                global.user.enable_favorite_filter[global.user.enable_favRTrack_filter] = 1;

                this->btn_order[global.user.enable_favRTrack_order]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[global.user.enable_favRTrack_order] = true;
                global.user.enable_favorite_order[global.user.enable_favRTrack_order] = 1;

                this->lb_media->setText(this->str_media_name[global.user.enable_favRTrack_filter]);
                this->box_media_filter->addWidget(this->lb_media);
                this->flag_filter[global.user.enable_favRTrack_filter] = true;

                this->lb_order->setText(this->str_order_name[global.user.enable_favRTrack_order]);
                this->box_media_filter->addWidget(this->lb_order);
                this->flag_order[global.user.enable_favRTrack_order] = true;
            }
            else if(this->contentStep == STEP_ALBUM){
                this->btn_order[5]->hide();

                this->btn_filter[global.user.enable_favRAlbum_filter]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[global.user.enable_favRAlbum_filter] = true;
                global.user.enable_favorite_filter[global.user.enable_favRAlbum_filter] = 1;

                this->btn_order[global.user.enable_favRAlbum_order]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[global.user.enable_favRAlbum_order] = true;
                global.user.enable_favorite_order[global.user.enable_favRAlbum_order] = 1;

                this->lb_media->setText(this->str_media_name[global.user.enable_favRAlbum_filter]);
                this->box_media_filter->addWidget(this->lb_media);
                this->flag_filter[global.user.enable_favRAlbum_filter] = true;

                this->lb_order->setText(this->str_order_name[global.user.enable_favRAlbum_order]);
                this->box_media_filter->addWidget(this->lb_order);
                this->flag_order[global.user.enable_favRAlbum_order] = true;
            }
            else if(this->contentStep == STEP_PLAYLIST){
                this->btn_order[5]->show();

                this->btn_filter[global.user.enable_favRPlaylist_filter]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[global.user.enable_favRPlaylist_filter] = true;
                global.user.enable_favorite_filter[global.user.enable_favRPlaylist_filter] = 1;

                this->btn_order[global.user.enable_favRPlaylist_order]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[global.user.enable_favRPlaylist_order] = true;
                global.user.enable_favorite_order[global.user.enable_favRPlaylist_order] = 1;

                this->lb_media->setText(this->str_media_name[global.user.enable_favRPlaylist_filter]);
                this->box_media_filter->addWidget(this->lb_media);
                this->flag_filter[global.user.enable_favRPlaylist_filter] = true;

                this->lb_order->setText(this->str_order_name[global.user.enable_favRPlaylist_order]);
                this->box_media_filter->addWidget(this->lb_order);
                this->flag_order[global.user.enable_favRPlaylist_order] = true;
            }
        }

        this->stackedWidget_body->hide();

        if(type == STEP_TRACK){
            this->btn_step_track->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_track").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));

            if(this->step_change == true){

                if(this->filter_change == true){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->box_favorite_track);

                    this->list_favoriteTrack->clear();
                    this->jsonArr_tracks_toPlay = QJsonArray();

                    this->flag_lastPage_track = false;
                    this->flagReqMore_track = false;
                    this->flag_draw_track = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoriteTrack->size() == 0){

                    GSCommon::clearLayout(this->box_favorite_track);

                    this->list_favoriteTrack->clear();
                    this->jsonArr_tracks_toPlay = QJsonArray();

                    this->flag_lastPage_track = false;
                    this->flagReqMore_track = false;
                    this->flag_draw_track = false;

                    this->request_more_Data(type);
                }
                else{

                    if(this->stackedWidget_body->isVisible() == false){
                        if(this->list_favoriteTrack->count() > 0){
                            int height = this->list_favoriteTrack->count() * 70;
                            this->stackedWidget_body->setFixedHeight(height);
                        }
                        else{
                            this->stackedWidget_body->setFixedHeight(500);
                        }

                        this->stackedWidget_body->setCurrentIndex(0);
                        this->stackedWidget_body->show();
                    }
                }
            }
            else{

                if(this->filter_change == true){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->box_favorite_track);

                    this->list_favoriteTrack->clear();
                    this->jsonArr_tracks_toPlay = QJsonArray();

                    this->flag_lastPage_track = false;
                    this->flagReqMore_track = false;
                    this->flag_draw_track = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoriteTrack->size() == 0){

                    GSCommon::clearLayout(this->box_favorite_track);

                    this->list_favoriteTrack->clear();
                    this->jsonArr_tracks_toPlay = QJsonArray();

                    this->flag_lastPage_track = false;
                    this->flagReqMore_track = false;
                    this->flag_draw_track = false;

                    this->request_more_Data(type);
                }
                else{

                    if(this->stackedWidget_body->isVisible() == false){
                        if(this->list_favoriteTrack->count() > 0){
                            int height = this->list_favoriteTrack->count() * 70;
                            this->stackedWidget_body->setFixedHeight(height);
                        }
                        else{
                            this->stackedWidget_body->setFixedHeight(500);
                        }

                        this->stackedWidget_body->setCurrentIndex(0);
                        this->stackedWidget_body->show();
                    }
                }
            }
        }
        else if(type == STEP_ALBUM){
            this->btn_step_album->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_step_album").arg(tmp_btnStyle + tmp_btnStyleHover).arg(tmp_btnStyleHover));

            if(this->step_change == true){

                if(this->filter_change == true){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoriteAlbum->size() == 0){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else{

                    if(this->stackedWidget_body->isVisible() == false){
                        if(this->album_totalCount > 0){
                            int height = 0;
                            if((this->album_totalCount % 7) == 0){
                                height = (this->album_totalCount / 7) * 289 + 20;
                            }
                            else{
                                height = ((this->album_totalCount / 7) + 1) * 289 + 20;
                            }
                            this->stackedWidget_body->setFixedHeight(height);
                        }
                        else{
                            this->stackedWidget_body->setFixedHeight(500);
                        }

                        this->stackedWidget_body->setCurrentIndex(1);
                        this->stackedWidget_body->show();
                    }
                }
            }
            else{

                if(this->filter_change == true){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoriteAlbum->size() == 0){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else{

                    if(this->stackedWidget_body->isVisible() == false){
                        if(this->album_totalCount > 0){
                            int height = 0;
                            if((this->album_totalCount % 7) == 0){
                                height = (this->album_totalCount / 7) * 289 + 20;
                            }
                            else{
                                height = ((this->album_totalCount / 7) + 1) * 289 + 20;
                            }
                            this->stackedWidget_body->setFixedHeight(height);
                        }
                        else{
                            this->stackedWidget_body->setFixedHeight(500);
                        }

                        this->stackedWidget_body->setCurrentIndex(1);
                        this->stackedWidget_body->show();
                    }
                }
            }
        }
        else if(type == STEP_PLAYLIST){
            this->btn_step_playlist->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_playlist").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

            if(this->step_change == true){

                if(this->filter_change == true){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoritePlaylist->size() == 0){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else{

                    if(this->stackedWidget_body->isVisible() == false){

                        if(this->playlist_totalCount > 0){
                            int height = 0;
                            if((this->playlist_totalCount % 7) == 0){
                                height = (this->playlist_totalCount / 7) * 321 + 20;
                            }
                            else{
                                height = ((this->playlist_totalCount / 7) + 1) * 321 + 20;
                            }
                            this->stackedWidget_body->setFixedHeight(height);
                        }
                        else{
                            this->stackedWidget_body->setFixedHeight(500);
                        }

                        this->stackedWidget_body->setCurrentIndex(2);
                        this->stackedWidget_body->show();
                    }
                }
            }
            else{

                if(this->filter_change == true){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoritePlaylist->size() == 0){
                    this->filter_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else{

                    if(this->stackedWidget_body->isVisible() == false){

                        if(this->playlist_totalCount > 0){
                            int height = 0;
                            if((this->playlist_totalCount % 7) == 0){
                                height = (this->playlist_totalCount / 7) * 321 + 20;
                            }
                            else{
                                height = ((this->playlist_totalCount / 7) + 1) * 321 + 20;
                            }
                            this->stackedWidget_body->setFixedHeight(height);
                        }
                        else{
                            this->stackedWidget_body->setFixedHeight(500);
                        }

                        this->stackedWidget_body->setCurrentIndex(2);
                        this->stackedWidget_body->show();
                    }
                }
            }
        }
    }


    void RoseFavorite::request_more_Data(QString type){

        QString mediaType = "";
        QString orderType = "";

        for(int i = 0; i < this->btn_filter_cnt; i++){
            if(global.user.enable_favorite_filter[i] == 1){

                switch (i) {
                    case 0:     mediaType = "";         break;
                    case 1:     mediaType = "UNITED";   break;
                    case 2:     mediaType = "MUSIC";    break;
                    case 3:     mediaType = "YOUTUBE";  break;
                    case 4:     mediaType = "TIDAL";    break;
                    case 5:     mediaType = "BUGS";     break;
                    case 6:     mediaType = "QOBUZ";    break;
                    case 7:     mediaType = "APPLE_MUSIC";    break;
                }
                break;
            }
        }

        if(type == STEP_TRACK){

            for(int i = 0; i < this->btn_order_cnt; i++){
                if(global.user.enable_favorite_order[i] == 1){

                    switch (i) {
                        case 0:     orderType = "TRACK_RECENT";         break;
                        case 1:     orderType = "TRACK_PREFERENCE";     break;
                        case 2:     orderType = "TRACK_PLAY_FREQUENCY"; break;
                        case 3:     orderType = "TITLE_ASC";            break;
                        case 4:     orderType = "TITLE_DESC";           break;
                        case 5:     orderType = "TRACK_PLAY_FREQUENCY"; break;
                    }
                    break;
                }
            }

            if(!this->flagReqMore_track && !this->flag_lastPage_track){
                this->flagReqMore_track = true;

                // next_offset
                if(this->list_favoriteTrack->size() == 0){
                    this->track_next_offset = 0;
                }
                else{
                    this->track_next_offset++;
                }

                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseFavorite::slot_applyResult_favoriteTracks);
                proc->request_rose_getList_favoriteTracks(orderType, mediaType, this->track_next_offset, 20);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
        }
        else if(type == STEP_ALBUM){

            for(int i = 0; i < this->btn_order_cnt; i++){
                if(global.user.enable_favorite_order[i] == 1){

                    switch (i) {
                        case 0:     orderType = "ALBUM_RECENT";         break;
                        case 1:     orderType = "ALBUM_PREFERENCE";     break;
                        case 2:     orderType = "ALBUM_PLAY_FREQUENCY"; break;
                        case 3:     orderType = "TITLE_ASC";            break;
                        case 4:     orderType = "TITLE_DESC";           break;
                        case 5:     orderType = "ALBUM_PLAY_FREQUENCY"; break;
                    }
                    break;
                }
            }

            if(!this->flagReqMore_album && !this->flag_lastPage_album){
                this->flagReqMore_album = true;

                // next_offset
                if(this->list_favoriteAlbum->size() == 0){
                    this->album_next_offset = 0;
                }
                else{
                    this->album_next_offset++;
                }

                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseFavorite::slot_applyResult_favoriteAlbums);
                proc->request_rose_getList_favoriteAlbums(orderType, mediaType, this->album_next_offset, 20);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
        }
        else if(type == STEP_PLAYLIST){

            for(int i = 0; i < this->btn_order_cnt; i++){
                if(global.user.enable_favorite_order[i] == 1){

                    switch (i) {
                        case 0:     orderType = "PLAYLIST_RECENT";          break;
                        case 1:     orderType = "PLAYLIST_PREFERENCE";      break;
                        case 2:     orderType = "PLAYLIST_PLAY_FREQUENCY";  break;
                        case 3:     orderType = "TITLE_ASC";                break;
                        case 4:     orderType = "TITLE_DESC";               break;
                        case 5:     orderType = "PLAYLIST_THUMBUP";         break;
                    }
                    break;
                }
            }

            if(!this->flagReqMore_playlist && !this->flag_lastPage_playlist){
                this->flagReqMore_playlist = true;

                // next_offset
                if(this->list_favoritePlaylist->size() == 0){
                    this->playlist_next_offset = 0;
                }
                else{
                    this->playlist_next_offset++;
                }

                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseFavorite::slot_applyResult_favoritePlaylists);
                proc->request_rose_getList_favoritePlaylists(orderType, mediaType, this->playlist_next_offset, 20);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
        }
    }


    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void RoseFavorite::proc_wheelEvent_to_getMoreData(){

        if(this->contentStep == STEP_TRACK && (!this->flag_lastPage_track && !this->flagReqMore_track) && (this->flag_draw_track == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw_track = true;

            this->request_more_Data(STEP_TRACK);
        }
        else if(this->contentStep == STEP_ALBUM && (!this->flag_lastPage_album && !this->flagReqMore_album) && (this->flag_draw_album == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw_album = true;

            this->request_more_Data(STEP_ALBUM);
        }
        else if(this->contentStep == STEP_PLAYLIST && (!this->flag_lastPage_playlist && !this->flagReqMore_playlist) && (this->flag_draw_playlist == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw_playlist = true;

            this->request_more_Data(STEP_PLAYLIST);
        }
    }


    /**
     * @brief RoseFavorite::changedSubTab [SLOT][override]
     * @param p_data
     */
    void RoseFavorite::slot_changedSubTabUI(const QJsonObject &p_data){

        Q_UNUSED(p_data);
    }


    /**
     * @brief RoseFavorite::changedSubTab [SLOT]
     */
    void RoseFavorite::slot_changedSubTabUI(){

        QString tmp_contentStep = this->sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();
        this->setUIControl_chooseStep(tmp_contentStep);
    }


    void RoseFavorite::slot_clickBtn_Filter(){

        if(this->contentStep == STEP_TRACK || this->contentStep == STEP_ALBUM){
            this->btn_order[5]->hide();
        }
        else if(this->contentStep == STEP_PLAYLIST){
            this->btn_order[5]->show();
        }

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void RoseFavorite::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void RoseFavorite::slot_clickBtn_chooseDetailMedia(){

        QString type = sender()->objectName();
        int idx = sender()->property("idx").toInt();

        this->filter_change = false;
        int media_idx = 0;
        int order_idx = 0;

        if(type == "media"){

            if(global.user.enable_favorite_filter[idx] == 0){
                for(int sel_idx = 0; sel_idx < this->btn_filter_cnt; sel_idx++){
                    this->btn_filter[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    this->flag_filter[sel_idx] = false;
                    global.user.enable_favorite_filter[sel_idx] = 0;
                }

                this->btn_filter[idx]->setStyleSheet(this->btnStyle_selected);
                this->flag_filter[idx] = true;
                global.user.enable_favorite_filter[idx] = 1;

                media_idx = idx;
                this->filter_change = true;
            }
            else{
                for(int i = 0; i < this->btn_filter_cnt; i++){
                    if(global.user.enable_favorite_filter[i] == 1){
                        media_idx = i;
                        break;
                    }
                }
            }

            for(int i = 0; i < this->btn_order_cnt; i++){
                if(global.user.enable_favorite_order[i] == 1){
                    order_idx = i;
                    break;
                }
            }
        }
        else if(type == "order"){

            if(global.user.enable_favorite_order[idx] == 0){
                for(int sel_idx = 0; sel_idx < this->btn_order_cnt; sel_idx++){
                    this->btn_order[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    this->flag_order[sel_idx] = false;
                    global.user.enable_favorite_order[sel_idx] = 0;
                }

                this->btn_order[idx]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[order_idx] = true;
                global.user.enable_favorite_order[idx] = 1;

                order_idx = idx;
                this->filter_change = true;
            }
            else{
                for(int i = 0; i < this->btn_order_cnt; i++){
                    if(global.user.enable_favorite_order[i] == 1){
                        order_idx = i;
                        break;
                    }
                }
            }

            for(int i = 0; i < this->btn_filter_cnt; i++){
                if(global.user.enable_favorite_filter[i] == 1){
                    media_idx = i;
                    break;
                }
            }
        }

        if(this->contentStep == STEP_TRACK){
            global.user.enable_favRTrack_filter = media_idx;
            global.user.enable_favRTrack_order = order_idx;
        }
        else if(this->contentStep == STEP_ALBUM){
            global.user.enable_favRAlbum_filter = media_idx;
            global.user.enable_favRAlbum_order = order_idx;
        }
        else if(this->contentStep == STEP_PLAYLIST){
            global.user.enable_favRPlaylist_filter = media_idx;
            global.user.enable_favRPlaylist_order = order_idx;
        }

        if(this->filter_change == true){
            GSCommon::clearLayout(box_media_filter);

            this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
            this->btn_filter_ico->setFixedSize(80, 70);
            this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

            connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

            this->lb_media = new QLabel();
            this->lb_media->setContentsMargins(20,0,20,0);
            this->lb_media->setFixedHeight(30);
            this->lb_media->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_media->setAlignment(Qt::AlignCenter);

            this->lb_order = new QLabel();
            this->lb_order->setContentsMargins(20,0,20,0);
            this->lb_order->setFixedHeight(30);
            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_order->setAlignment(Qt::AlignCenter);

            this->box_media_filter->addWidget(this->btn_filter_ico);
            this->box_media_filter->setSpacing(10);

            this->lb_media->setText(this->str_media_name[media_idx]);
            this->box_media_filter->addWidget(this->lb_media);
            this->flag_filter[media_idx] = true;

            this->lb_order->setText(this->str_order_name[order_idx]);
            this->box_media_filter->addWidget(this->lb_order);
            this->flag_order[order_idx] = true;

            this->widget_filter->setVisible(false);

            this->setUIControl_chooseStep(this->contentStep);
        }
    }


    void RoseFavorite::slot_applyResult_favoriteTracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(this->list_favoriteTrack->count() == 0){
                this->track_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_favoriteTrack->size();

            this->list_favoriteTrack->append(list_data);
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArr_dataToPlay);

            if(this->stackedWidget_body->isVisible() == false){
                int height = this->list_favoriteTrack->count() * 70;
                this->stackedWidget_body->setFixedHeight(height);

                this->stackedWidget_body->setCurrentIndex(0);
                this->stackedWidget_body->show();
            }
            else{
                int height = this->list_favoriteTrack->count() * 70;
                this->stackedWidget_body->setFixedHeight(height);
            }

            int max_cnt = this->list_favoriteTrack->size();

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_track[i] = new PlaylistTrackDetailInfo_RHV();
                connect(this->favorite_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseFavorite::slot_clickedItemTrack_inList);
                this->favorite_track[i]->setProperty("index", i);

                this->box_favorite_track->addWidget(this->favorite_track[i]);
            }

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_track[i]->setDataTrackInfo_Rose(this->list_favoriteTrack->at(i));
                this->favorite_track[i]->setFavoritesIds(this->list_favoriteTrack->at(i).favorite, this->list_favoriteTrack->at(i).star);

                QCoreApplication::processEvents();
            }

            ContentLoadingwaitingMsgHide();
            if(this->flag_lastPage_track == false && this->track_totalCount > this->list_favoriteTrack->size()){
                this->flag_draw_track = false;
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->box_favorite_track->addWidget(noData_widget, 0, Qt::AlignTop);

            this->stackedWidget_body->setFixedHeight(500);

            this->stackedWidget_body->setCurrentIndex(0);
            this->stackedWidget_body->show();
        }
    }


    void RoseFavorite::slot_applyResult_favoriteAlbums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_favoriteAlbum->count() == 0){
                this->album_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_favoriteAlbum->count();

            this->list_favoriteAlbum->append(list_data);

            int max_cnt = this->list_favoriteAlbum->size();

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___FavoriteALBUM, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->favorite_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseFavorite::slot_clickedItemAlbum);
            }

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_album[i]->setData(this->list_favoriteAlbum->at(i));
                this->flowLayout_favorite_album->addWidget(this->favorite_album[i]);

                QCoreApplication::processEvents();
            }

            if(this->stackedWidget_body->isVisible() == false){

                int height = 0;
                if((max_cnt % 7) == 0){
                    height = (max_cnt / 7) * 289 + 20;
                }
                else{
                    height = ((max_cnt / 7) + 1) * 289 + 20;
                }
                this->stackedWidget_body->setFixedHeight(height);

                this->stackedWidget_body->setCurrentIndex(1);
                this->stackedWidget_body->show();
            }
            else{

                int height = 0;
                if((max_cnt % 7) == 0){
                    height = (max_cnt / 7) * 289 + 20;
                }
                else{
                    height = ((max_cnt / 7) + 1) * 289 + 20;
                }
                this->stackedWidget_body->setFixedHeight(height);
            }

            if(this->flag_lastPage_album == false && this->album_totalCount > this->list_favoriteAlbum->size()){
                this->flag_draw_album = false;
            }

            ContentLoadingwaitingMsgHide();
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->flowLayout_favorite_album->addWidget(noData_widget);

            this->stackedWidget_body->setFixedHeight(500);

            this->stackedWidget_body->setCurrentIndex(1);
            this->stackedWidget_body->show();
        }
    }


    void RoseFavorite::slot_applyResult_favoritePlaylists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_favoritePlaylist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_favoritePlaylist->count();

            this->list_favoritePlaylist->append(list_data);

            int max_cnt = this->list_favoritePlaylist->size();

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___FavoritePLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 1, true);
                connect(this->favorite_playlist[i], &roseHome::ItemPlaylist_rosehome::signal_clicked, this, &RoseFavorite::slot_clickedItemPlaylist);
            }

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_playlist[i]->setData(this->list_favoritePlaylist->at(i));
                this->flowLayout_favorite_playlist->addWidget(this->favorite_playlist[i]);

                QCoreApplication::processEvents();
            }

            if(this->stackedWidget_body->isVisible() == false){

                int height = 0;
                if((max_cnt % 7) == 0){
                    height = (max_cnt / 7) * 321 + 20;
                }
                else{
                    height = ((max_cnt / 7) + 1) * 321 + 20;
                }
                this->stackedWidget_body->setFixedHeight(height);

                this->stackedWidget_body->setCurrentIndex(2);
                this->stackedWidget_body->show();
            }
            else{

                int height = 0;
                if((max_cnt % 7) == 0){
                    height = (max_cnt / 7) * 321 + 20;
                }
                else{
                    height = ((max_cnt / 7) + 1) * 321 + 20;
                }
                this->stackedWidget_body->setFixedHeight(height);
            }

            if(this->flag_lastPage_playlist == false && this->playlist_totalCount > this->list_favoritePlaylist->size()){
                this->flag_draw_playlist = false;
            }

            ContentLoadingwaitingMsgHide();
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
            noData_widget->setFixedSize(1500, 500);

            this->flowLayout_favorite_playlist->addWidget(noData_widget);

            this->stackedWidget_body->setFixedHeight(500);

            this->stackedWidget_body->setCurrentIndex(2);
            this->stackedWidget_body->show();
        }
    }


    void RoseFavorite::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject track_info = contents.at(0).toObject();

            if(track_info.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");
                    bool flag = star > 0 ? true : false;

                    this->favorite_track[i]->setFavoritesIds(flag, star);
                }
            }
            else if(track_info.contains("message") && (track_info["message"].toString() == "정상")){
                this->favorite_track[this->track_idx_fav]->setFavoritesIds(this->flag_track_fav, this->track_star_fav);
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void RoseFavorite::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

        if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::FavBtn){
            if(this->flag_check_track == false){
                this->track_star_fav = this->favorite_track[idx]->getFavoritesStars();

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

                QJsonObject ratingInfo;
                ratingInfo.insert("favorite", this->flag_track_fav);
                ratingInfo.insert("star", this->track_star_fav);
                ratingInfo.insert("thumbup", false);
                ratingInfo.insert("type", this->list_favoriteTrack->at(idx).type);

                QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject json;
                json.insert("ratingInfo", ratingInfo);
                json.insert("track", track);

                // request HTTP API - get favorite for Rose Server
                roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseFavorite::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            this->proc_clicked_itemTrack_inList(this->list_favoriteTrack, this->jsonArr_tracks_toPlay, clickMode, idx, SECTION_FOR_MORE_POPUP___FavoriteTrack);
        }
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void RoseFavorite::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_favoriteAlbum, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void RoseFavorite::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_favoritePlaylist, clickMode, index, section);
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void RoseFavorite::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___FavoriteTrack){
            this->proc_clicked_optMorePopup_fromTrack(this->list_favoriteTrack, this->jsonArr_tracks_toPlay, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___FavoriteALBUM){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_favoriteAlbum, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___FavoritePLAYLIST){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_favoritePlaylist, index, section, clickMode);
        }
    }

}
