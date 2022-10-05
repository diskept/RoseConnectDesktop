#include "rosetube/rosetube_MyFavorite.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"
#include "widget/VerticalScrollArea.h"

#include <QScroller>
#include <QPixmapCache>


namespace rosetube {

    const int HTTP_CACHE = 10;

    const int SECTION_FOR_MORE_POPUP___FavoriteTrack = 0;
    const int SECTION_FOR_MORE_POPUP___FavoriteALBUM = 1;
    const int SECTION_FOR_MORE_POPUP___FavoritePLAYLIST = 2;

    const QString STEP_TRACK = "track";
    const QString STEP_ALBUM = "album";
    const QString STEP_PLAYLIST = "playlist";

    const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";
    const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";


    /**
     * @brief "MUSIC > My Collection > Favorite" 화면을 위한 생성자.
     * @see PAGECODE_RH_FAVORITE
     * @param parent
     */
    RoseTubeFavorite::RoseTubeFavorite(QWidget *parent) : AbstractRoseHomeSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();

        // 기본 UI 세팅
        this->btnStyle_normal = "QPushButton{ ";
        this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:15px;";
        this->btnStyle_normal += "border:1px solid #707070; border-radius:20px;}";
        this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:15px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:20px;}";

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
    RoseTubeFavorite::~RoseTubeFavorite(){

        this->deleteLater();
    }


    void RoseTubeFavorite::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;
        }
    }


    void RoseTubeFavorite::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // 데이터로 UI 적용 : 세팅이 안되어 있는 경우에만
            if(this->flagSet_order == false){
                if(global.user.flag_favMusic_order == false){
                    global.user.flag_favMusic_order = true;
                    for(int j = 0; j < 10; j++){
                        global.user.enable_favMusic_order[j] = 0;
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
    void RoseTubeFavorite::setUIControl_chooseFilter(){

        // 상단 필터
        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

        this->box_order_filter  = new QHBoxLayout();
        this->box_order_filter->setContentsMargins(0,0,0,0);
        this->box_order_filter->setSpacing(0);
        this->box_order_filter->setAlignment(Qt::AlignLeft);
        this->box_order_filter->addWidget(this->btn_filter_ico);
        this->box_order_filter->setSpacing(10);

        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121;}");
        widget_btnFilter->setLayout(this->box_order_filter);


        QWidget *widget_btnStep = new QWidget();
        widget_btnStep->setObjectName("widget_btnStep");
        widget_btnStep->setStyleSheet("#widget_btnStep { background-color:#212121;}");
        widget_btnStep->setFixedSize(405, 70);
        widget_btnStep->setContentsMargins(0, 0, 0, 5);

        this->btn_step_track = new QPushButton(tr("Track"), widget_btnStep);
        this->btn_step_track->setObjectName("btn_step_track");
        this->btn_step_track->setFixedSize(135, 40);
        //this->btn_step_track->setGeometry(1, 15, 0, 0);
        this->btn_step_track->setGeometry(135, 15, 0, 0);
        this->btn_step_track->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), STEP_TRACK);
        this->btn_step_track->setCursor(Qt::PointingHandCursor);

        this->btn_step_album = new QPushButton(tr("Album"), widget_btnStep);
        this->btn_step_album->setObjectName("btn_step_album");
        this->btn_step_album->setFixedSize(135, 40);
        this->btn_step_album->setGeometry(135, 15, 0, 0);
        this->btn_step_album->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), STEP_ALBUM);
        this->btn_step_album->setCursor(Qt::PointingHandCursor);
        this->btn_step_album->hide();

        this->btn_step_playlist = new QPushButton(tr("Playlist"), widget_btnStep);
        this->btn_step_playlist->setObjectName("btn_step_playlist");
        this->btn_step_playlist->setFixedSize(135, 40);
        this->btn_step_playlist->setGeometry(269, 15, 0, 0);
        this->btn_step_playlist->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), STEP_PLAYLIST);
        this->btn_step_playlist->setCursor(Qt::PointingHandCursor);

        this->box_filter->addWidget(widget_btnFilter, 0, Qt::AlignLeft);
        this->box_filter->addWidget(widget_btnStep, 1, Qt::AlignRight);


        this->btn_order_cnt = this->orderTypeList.size();

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
        //connect(this->btn_step_album, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
        connect(this->btn_step_playlist, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));
    }


    void RoseTubeFavorite::setUIControl_chooseFilter_fromData(){

        QPushButton *btn_close_order = new QPushButton();
        btn_close_order->setText(tr("Select Sort Type"));
        btn_close_order->setFixedSize(470, 50);
        btn_close_order->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_order->setContentsMargins(20, 0, 0, 0);
        btn_close_order->setCursor(Qt::PointingHandCursor);

        connect(btn_close_order, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0,0,0,0);
        box_header->setSpacing(0);
        box_header->addWidget(btn_close_order);

        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFixedHeight(1);
        line->setLineWidth(1);
        line->setStyleSheet("background-color:#4D4D4D;border:none;");

        this->flowLayout_order = new FlowLayout(0, 0, 20, 20);
        this->flowLayout_order->setContentsMargins(10, 10, 10, 10);
        this->flowLayout_order->setSizeConstraint(QLayout::SetMinimumSize);

        if(this->btn_order_cnt > 0){
            this->flagSet_order = true;
        }

        int check_btn = 0;
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
                this->box_order_filter->addWidget(this->lb_order);
                this->flag_order[j] = true;
                check_btn++;
            }

            connect(this->btn_order[j], SIGNAL(clicked()), this, SLOT(slot_clickBtn_chooseDetailMedia()));

            this->flowLayout_order->addWidget(this->btn_order[j]);
        }

        if(check_btn == 0){
            this->btn_order[0]->setStyleSheet(this->btnStyle_selected);
            this->lb_order->setText(this->str_order_name[0]);
            this->box_order_filter->addWidget(this->lb_order);
            this->flag_order[0] = true;
            global.user.enable_favorite_order[0] = 1;
        }


        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addWidget(line);
        box_total->addLayout(this->flowLayout_order, 1);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(470, 180);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setLayout(box_total);
        btn_total->setFixedSize(470, 180);
        btn_total->setGeometry(0, 0, 0, 0);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    void RoseTubeFavorite::setUIControl_chooseStep(QString type){

        this->step_change = false;
        if(this->contentStep != type){
            this->contentStep = "";
            this->contentStep = type;
            this->step_change = true;

            // 버튼 스타일 초기화
            this->btn_step_track->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_track").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
            //this->btn_step_album->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_step_album").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
            this->btn_step_playlist->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_playlist").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

            for(int sel_idx = 0; sel_idx < this->btn_order_cnt; sel_idx++){
                this->btn_order[sel_idx]->setStyleSheet(this->btnStyle_normal);
                this->flag_order[sel_idx] = false;
                global.user.enable_favorite_order[sel_idx] = 0;
            }

            GSCommon::clearLayout(box_order_filter);

            this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
            this->btn_filter_ico->setFixedSize(80, 70);
            this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

            connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

            this->lb_order = new QLabel();
            this->lb_order->setContentsMargins(20,0,20,0);
            this->lb_order->setFixedHeight(30);
            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_order->setAlignment(Qt::AlignCenter);

            this->box_order_filter->addWidget(this->btn_filter_ico);
            this->box_order_filter->setSpacing(10);

            if(this->contentStep == STEP_TRACK){
                this->btn_order[5]->hide();

                this->btn_order[global.user.enable_favMTrack_order]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[global.user.enable_favMTrack_order] = true;
                global.user.enable_favorite_order[global.user.enable_favMTrack_order] = 1;

                this->lb_order->setText(this->str_order_name[global.user.enable_favMTrack_order]);
                this->box_order_filter->addWidget(this->lb_order);
                this->flag_order[global.user.enable_favMTrack_order] = true;
            }
            else if(this->contentStep == STEP_ALBUM){
                this->btn_order[5]->hide();

                this->btn_order[global.user.enable_favMAlbum_order]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[global.user.enable_favMAlbum_order] = true;
                global.user.enable_favorite_order[global.user.enable_favMAlbum_order] = 1;

                this->lb_order->setText(this->str_order_name[global.user.enable_favMAlbum_order]);
                this->box_order_filter->addWidget(this->lb_order);
                this->flag_order[global.user.enable_favMAlbum_order] = true;
            }
            else if(this->contentStep == STEP_PLAYLIST){
                this->btn_order[5]->show();

                this->btn_order[global.user.enable_favMPlaylist_order]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[global.user.enable_favMPlaylist_order] = true;
                global.user.enable_favorite_order[global.user.enable_favMPlaylist_order] = 1;

                this->lb_order->setText(this->str_order_name[global.user.enable_favMPlaylist_order]);
                this->box_order_filter->addWidget(this->lb_order);
                this->flag_order[global.user.enable_favMPlaylist_order] = true;
            }
        }

        this->stackedWidget_body->hide();

        if(type == STEP_TRACK){
            this->btn_step_track->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_step_track").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));

            if(this->step_change == true){

                if(this->order_change == true){
                    this->order_change = false;

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
                if(this->order_change == true){
                    this->order_change = false;

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

                if(this->order_change == true){
                    this->order_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoriteAlbum->size() == 0){

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else{

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
            else{

                if(this->order_change == true){
                    this->order_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_album);

                    this->list_favoriteAlbum->clear();

                    this->flag_lastPage_album = false;
                    this->flagReqMore_album = false;
                    this->flag_draw_album = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoriteAlbum->size() == 0){

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

                if(this->order_change == true){
                    this->order_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoritePlaylist->size() == 0){

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else{

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
            else{

                if(this->order_change == true){
                    this->order_change = false;

                    GSCommon::clearLayout(this->flowLayout_favorite_playlist);

                    this->list_favoritePlaylist->clear();

                    this->flagReqMore_playlist = false;
                    this->flag_lastPage_playlist = false;
                    this->flag_draw_playlist = false;

                    this->request_more_Data(type);
                }
                else if(this->list_favoritePlaylist->size() == 0){

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


    void RoseTubeFavorite::request_more_Data(QString type){

        QString orderType = "";

        if(type == STEP_TRACK){

            for(int i = 0; i < this->btn_order_cnt; i++){
                if(global.user.enable_favorite_order[i] == 1){

                    switch (i) {
                        case 0:     orderType = "TRACK_RECENT";         break;
                        case 1:     orderType = "TRACK_PREFERENCE";     break;
                        case 2:     orderType = "TRACK_PLAY_FREQUENCY"; break;
                        case 3:     orderType = "TITLE_ASC"; break;
                        case 4:     orderType = "TITLE_DESC"; break;
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
                connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &RoseTubeFavorite::slot_applyResult_favoriteTracks);
                proc->request_rose_getList_favoriteTracks(orderType, "YOUTUBE", this->track_next_offset, 20);

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
                        case 3:     orderType = "TITLE_ASC"; break;
                        case 4:     orderType = "TITLE_DESC"; break;
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
                connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &RoseTubeFavorite::slot_applyResult_favoriteAlbums);
                proc->request_rose_getList_favoriteAlbums(orderType, "YOUTUBE", this->track_next_offset, 20);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
        }
        else if(type == STEP_PLAYLIST){

            for(int i = 0; i < this->btn_order_cnt; i++){
                if(global.user.enable_favorite_order[i] == 1){

                    switch (i) {
                        case 0:     orderType = "PLAYLIST_RECENT";         break;
                        case 1:     orderType = "PLAYLIST_PREFERENCE";     break;
                        case 2:     orderType = "PLAYLIST_PLAY_FREQUENCY"; break;
                        case 3:     orderType = "TITLE_ASC"; break;
                        case 4:     orderType = "TITLE_DESC"; break;
                        case 5:     orderType = "PLAYLIST_THUMBUP"; break;
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
                connect(proc, &roseHome::ProcCommon::completeReq_list_playlists, this, &RoseTubeFavorite::slot_applyResult_favoritePlaylists);
                proc->request_rose_getList_favoritePlaylists(orderType, "YOUTUBE", this->track_next_offset, 20);

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }
        }
    }


    void RoseTubeFavorite::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_CACHE){
            if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
            }
        }
    }


    void RoseTubeFavorite::proc_wheelEvent_to_getMoreData(){

        if(this->contentStep == STEP_TRACK && (!this->flag_lastPage_track && !this->flagReqMore_track) && (this->flag_draw_track == false)){
            this->flag_draw_track = true;

            this->request_more_Data(STEP_TRACK);
        }
        else if(this->contentStep == STEP_ALBUM && (!this->flag_lastPage_album && !this->flagReqMore_album) && (this->flag_draw_album == false)){
            this->flag_draw_album = true;

            this->request_more_Data(STEP_ALBUM);
        }
        else if(this->contentStep == STEP_PLAYLIST && (!this->flag_lastPage_playlist && !this->flagReqMore_playlist) && (this->flag_draw_playlist == false)){
            this->flag_draw_playlist = true;

            this->request_more_Data(STEP_PLAYLIST);
        }
    }


    void RoseTubeFavorite::slot_changedSubTabUI(const QJsonObject &p_data){

        Q_UNUSED(p_data);
    }


    void RoseTubeFavorite::slot_changedSubTabUI(){

        QString tmp_contentStep = this->sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();
        this->setUIControl_chooseStep(tmp_contentStep);
    }


    void RoseTubeFavorite::slot_clickBtn_Filter(){

        if(this->contentStep == STEP_TRACK || this->contentStep == STEP_ALBUM){
            this->btn_order[5]->hide();
        }
        else if(this->contentStep == STEP_PLAYLIST){
            this->btn_order[5]->show();
        }

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void RoseTubeFavorite::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void RoseTubeFavorite::slot_clickBtn_chooseDetailMedia(){

        QString type = sender()->objectName();
        int idx = sender()->property("idx").toInt();

        bool changeOk = false;
        int order_idx = 0;

        this->order_change = false;

        if(type == "order"){

            if(global.user.enable_favorite_order[idx] == 0){
                for(int sel_idx = 0; sel_idx < this->btn_order_cnt; sel_idx++){
                    this->btn_order[sel_idx]->setStyleSheet(this->btnStyle_normal);
                    this->flag_order[sel_idx] = false;
                    global.user.enable_favorite_order[sel_idx] = 0;
                }

                this->btn_order[idx]->setStyleSheet(this->btnStyle_selected);
                this->flag_order[idx] = true;
                global.user.enable_favorite_order[idx] = 1;

                order_idx = idx;
                changeOk = true;
            }
            else{
                for(int i = 0; i < this->btn_order_cnt; i++){
                    if(global.user.enable_favorite_order[i] == 1){
                        order_idx = i;
                        break;
                    }
                }
            }
        }

        if(this->contentStep == STEP_TRACK){
            global.user.enable_favMTrack_order = order_idx;
        }
        else if(this->contentStep == STEP_ALBUM){
            global.user.enable_favMAlbum_order = order_idx;
        }
        else if(this->contentStep == STEP_PLAYLIST){
            global.user.enable_favMPlaylist_order = order_idx;
        }


        if(changeOk == true){
            GSCommon::clearLayout(box_order_filter);

            this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
            this->btn_filter_ico->setFixedSize(80, 70);
            this->btn_filter_ico->setCursor(Qt::PointingHandCursor);

            connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

            this->lb_order = new QLabel();
            this->lb_order->setContentsMargins(20,0,20,0);
            this->lb_order->setFixedHeight(30);
            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->lb_order->setAlignment(Qt::AlignCenter);

            this->box_order_filter->addWidget(this->btn_filter_ico);
            this->box_order_filter->setSpacing(10);

            this->lb_order->setText(this->str_order_name[order_idx]);
            this->box_order_filter->addWidget(this->lb_order);
            this->flag_order[order_idx] = true;

            this->widget_filter->setVisible(false);

            this->order_change = true;
            this->setUIControl_chooseStep(this->contentStep);
        }
        else{
            this->widget_filter->setVisible(false);
        }
    }


    void RoseTubeFavorite::slot_applyResult_favoriteTracks(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_track = flag_lastPage;
            this->flagReqMore_track = false;

            if(this->list_favoriteTrack->count() == 0){
                this->track_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_favoriteTrack->count();

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
                connect(this->favorite_track[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &RoseTubeFavorite::slot_clickedItemTrack_inList);
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


    void RoseTubeFavorite::slot_applyResult_favoriteAlbums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_favoriteAlbum->count() == 0){
                this->album_totalCount = list_data.at(0).totalCount;

                if(this->album_totalCount > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_favoriteAlbum->count();

            this->list_favoriteAlbum->append(list_data);

            int max_cnt = this->list_favoriteAlbum->size();

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_album[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___FavoriteALBUM, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                connect(this->favorite_album[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseTubeFavorite::slot_clickedItemAlbum);
            }

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_album[i]->setData(this->list_favoriteAlbum->at(i));
                this->flowLayout_favorite_album->addWidget(this->favorite_album[i]);

                QCoreApplication::processEvents();
            }

            if(this->stackedWidget_body->isVisible() == false){

                int height = 0;
                if((this->album_totalCount % 7) == 0){
                    height = (this->album_totalCount / 7) * 289 + 20;
                }
                else{
                    height = ((this->album_totalCount / 7) + 1) * 289 + 20;
                }
                this->stackedWidget_body->setFixedHeight(height);

                this->stackedWidget_body->setCurrentIndex(1);
                this->stackedWidget_body->show();
            }
            else{

                int height = 0;
                if((this->album_totalCount % 7) == 0){
                    height = (this->album_totalCount / 7) * 289 + 20;
                }
                else{
                    height = ((this->album_totalCount / 7) + 1) * 289 + 20;
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


    void RoseTubeFavorite::slot_applyResult_favoritePlaylists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_playlist = flag_lastPage;
            this->flagReqMore_playlist = false;

            if(this->list_favoritePlaylist->count() == 0){
                this->playlist_totalCount = list_data.at(0).totalCount;

                if(this->playlist_totalCount > 50){
                    this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                }
            }

            int start_index = this->list_favoritePlaylist->count();

            this->list_favoritePlaylist->append(list_data);

            int max_cnt = this->list_favoritePlaylist->size();

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_playlist[i] = new roseHome::ItemPlaylist_rosehome(i, SECTION_FOR_MORE_POPUP___FavoritePLAYLIST, tidal::AbstractItem::ImageSizeMode::Square_200x200, 1, true);
                connect(this->favorite_playlist[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &RoseTubeFavorite::slot_clickedItemPlaylist);
            }

            for(int i = start_index; i < max_cnt; i++){
                this->favorite_playlist[i]->setData(this->list_favoritePlaylist->at(i));
                this->flowLayout_favorite_playlist->addWidget(this->favorite_playlist[i]);

                QCoreApplication::processEvents();
            }

            if(this->stackedWidget_body->isVisible() == false){

                int height = 0;
                if((this->playlist_totalCount % 7) == 0){
                    height = (this->playlist_totalCount / 7) * 321 + 20;
                }
                else{
                    height = ((this->playlist_totalCount / 7) + 1) * 321 + 20;
                }
                this->stackedWidget_body->setFixedHeight(height);

                this->stackedWidget_body->setCurrentIndex(2);
                this->stackedWidget_body->show();
            }
            else{

                int height = 0;
                if((this->playlist_totalCount % 7) == 0){
                    height = (this->playlist_totalCount / 7) * 321 + 20;
                }
                else{
                    height = ((this->playlist_totalCount / 7) + 1) * 321 + 20;
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


    void RoseTubeFavorite::slot_applyResult_getRating_track(const QJsonArray &contents){

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


    void RoseTubeFavorite::slot_clickedItemTrack_inList(const int idx, const PlaylistTrackDetailInfo_RHV::ClickMode clickMode){

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
                connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &RoseTubeFavorite::slot_applyResult_getRating_track);
                proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_fav);

                this->flag_check_track = true;
            }
        }
        else{
            // ClickMode 별로 처리
            if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AllBox){
                // Track 바로 재생 - clicked 항목만 재생
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

                roseHome::TrackItemData data_playlist;
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(data_playlist, this->jsonArr_tracks_toPlay, idx, playType);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::AddCollectionBtn){
                // 플레이리스트 담기 - Track
                //this->showDialog_toAddMyCollection(data_track.id, Dialog_ChoosePlaylist_forQobuz::ItemType_forAddCollection::Track);
            }
            else if(clickMode == PlaylistTrackDetailInfo_RHV::ClickMode::MoreBtn){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

                QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                OptMorePopup::HeaderData data_header;
                data_header.main_title = ProcJsonEasy::getString(tmpObj, "title");
                data_header.sub_title = ProcJsonEasy::getString(snippet, "channelTitle");
                data_header.imageUrl = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(tmpObj, "id"));
                data_header.type = "YOUTUBE";      //j220906 share link
                data_header.flagProcStar = false;
                data_header.isShare = true;      //j220906 share link

                // OptMorePopup 띄우기 필요
                this->makeObj_optMorePopup(OptMorePopup::Rosetube_Video, data_header, idx, SECTION_FOR_MORE_POPUP___FavoriteTrack);
            }
        }
    }


    void RoseTubeFavorite::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___FavoriteALBUM){
            roseHome::AlbumItemData data_album = this->list_favoriteAlbum->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___FavoriteALBUM, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Album 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Album Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert("id", data_album.id);
                jsonData.insert("star", data_album.star);
                jsonData.insert("type", data_album.type);
                jsonData.insert("title", data_album.title);
                jsonData.insert("thumbnail", data_album.thumbnail);
                jsonData.insert("playTime", data_album.playTime);
                jsonData.insert("clientKey", data_album.clientKey);
                jsonData.insert("trackId", data_album.trackId);
                jsonData.insert("macAddress", data_album.macAddress);
                jsonData.insert("favorite", data_album.favorite);

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTALBUMINFO);

                emit linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void RoseTubeFavorite::slot_clickedItemPlaylist(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___FavoritePLAYLIST){
            roseHome::PlaylistItemData data_playlist = this->list_favoritePlaylist->at(index);

            if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
                print_debug();
                // OptMorePopup 띄우기
                this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_playlist), index, SECTION_FOR_MORE_POPUP___FavoritePLAYLIST, true);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
                // Playlist 바로 재생
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
            else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

                // Playlist Detail 페이지 진입
                QJsonObject tmp_data;
                tmp_data.insert("id", data_playlist.id);
                tmp_data.insert("ownerId", data_playlist.ownerId);
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_VA_HOME_MYDETAIL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_VA_HOME_MYDETAIL);

                // 부모에게 페이지 변경하라고 시그널 보냄
                emit this->signal_clickedViewAll(tmp_data);
            }
        }
    }


    void RoseTubeFavorite::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___FavoriteTrack){
            //this->proc_clicked_optMorePopup_fromTrack(this->list_favoriteTrack, this->jsonArr_tracks_toPlay, index, clickMode);
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
                // Rose Play 요청
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byTracks(this->list_favoriteTrack->at(index), this->jsonArr_tracks_toPlay, index, clickMode);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubscribInfo){
                QJsonObject tmpObj = this->jsonArr_tracks_toPlay.at(index).toObject();
                QJsonObject dataObj = ProcJsonEasy::getJsonObject(tmpObj, "data");
                QJsonObject snippet = ProcJsonEasy::getJsonObject(dataObj, "snippet");
                QString channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                QString channelId = ProcJsonEasy::getString(snippet, "channelId");

                QJsonObject tmp_data;
                tmp_data.insert(KEY_PAGE_CODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert(KEY_VIEWALL_MODE, PAGECODE_SUB_CHANNEL);
                tmp_data.insert("title", channelName);
                tmp_data.insert("channel_id", channelId);

                emit this->signal_clickedViewAll(tmp_data);
            }
            else if(clickMode == OptMorePopup::ClickMode::Caching_ahead){
                QJsonObject trackInfo = this->jsonArr_tracks_toPlay.at(index).toObject();

                QString channelId = "";
                QString channelName = "";
                QString duration = "";
                QString id = "";
                QString thumbnailUrl = "";
                QString title = "";

                if(trackInfo.contains("data")){
                    QJsonObject trackData = ProcJsonEasy::getJsonObject(trackInfo, "data");
                    QJsonObject detailContents = ProcJsonEasy::getJsonObject(trackData, "detailContents");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(trackData, "snippet");
                    QJsonObject trackStatistic = ProcJsonEasy::getJsonObject(trackData, "detailContents");

                    QString tmpDuration = ProcJsonEasy::getString(detailContents, "duration");

                    QString time = tmpDuration;
                    QString hour;
                    QString minute;
                    QString second;

                    if(tmpDuration.indexOf("PT") >= 0){
                        time = tmpDuration.right(tmpDuration.size() - 2);
                    }
                    if(time.indexOf("H") >= 0){
                        hour = time.section("H", 0, -2);
                        time = time.mid(time.indexOf("H") + 1);
                    }
                    if(time.indexOf("M") >= 0){
                        minute = time.section("M", 0, -2);
                        time = time.mid(time.indexOf("M") + 1);
                    }
                    if(time.indexOf("S") >= 0){
                        second = time.section("S", 0, -2);
                    }

                    if(hour.isEmpty()){
                        if(minute.isEmpty()){
                            duration = "0:" + second.rightJustified(2, '0');
                        }
                        else{
                            duration = minute + ":" + second.rightJustified(2, '0');
                        }
                    }
                    else{
                        duration = hour + ":" + minute.rightJustified(2, '0') + ":" + second.rightJustified(2, '0');
                    }

                    channelId = ProcJsonEasy::getString(snippet, "channelId");
                    channelName = ProcJsonEasy::getString(snippet, "channelTitle");
                    id = ProcJsonEasy::getString(trackInfo, "clientKey");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }
                else{
                    if(trackInfo.contains("channelTitle")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelTitle");
                    }
                    else if(trackInfo.contains("channelName")){
                        channelName = ProcJsonEasy::getString(trackInfo, "channelName");
                    }

                    int time = ProcJsonEasy::getInt(trackInfo, "duration");
                    if(time == 0){
                        QString tmpDur = ProcJsonEasy::getString(trackInfo, "duration");

                        if(tmpDur.isEmpty()){
                            duration = "";
                        }
                        else{
                            duration = tmpDur;
                        }
                    }
                    else if(time >= 3600){
                         duration = QDateTime::fromTime_t(time).toUTC().toString("hh:mm:ss");
                    }
                    else{
                        duration = QDateTime::fromTime_t(time).toUTC().toString("mm:ss");
                    }

                    channelId = ProcJsonEasy::getString(trackInfo, "channelId");
                    id = ProcJsonEasy::getString(trackInfo, "id");
                    thumbnailUrl = ProcJsonEasy::getString(trackInfo, "thumbnailUrl");
                    title = ProcJsonEasy::getString(trackInfo, "title");
                }

                QJsonObject data;
                data.insert("channelId", channelId);
                data.insert("channelName", channelName);
                data.insert("duration", duration);
                data.insert("id", id);
                data.insert("isFavorite", false);
                data.insert("isLiveStream", false);
                data.insert("thumbnailUrl", thumbnailUrl);
                data.insert("thumbsUpPercentage", 0);
                data.insert("title", title);
                data.insert("viewsCount", "-1");

                QJsonObject json;
                json.insert("data", data);
                json.insert("isDelete", false);

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/tube_cache_download_item";
                network->request(HTTP_CACHE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___FavoriteALBUM){

            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                roseHome::AlbumItemData data_album = this->list_favoriteAlbum->at(index);

                // Rose Play 요청
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byAlbumID(data_album.id, clickMode);
            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___FavoritePLAYLIST){

            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                roseHome::PlaylistItemData data_playlist = this->list_favoritePlaylist->at(index);

                // Rose Play 요청
                roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
                procRosePlay->requestPlayRose_byPlaylistID(data_playlist.id, OptMorePopup::ClickMode::Play_RightNow);
            }
        }
    }
}
