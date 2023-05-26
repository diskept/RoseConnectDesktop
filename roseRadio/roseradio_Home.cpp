#include "roseRadio/roseradio_Home.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"

#include "login/dialoglogin.h"

#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"
#include "widget/toastmsg.h"//c220727

#include <QDebug>
#include <QScroller>


namespace roseRadio {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX_SUBTITLE_FeaturedChannel = 0;
    const int BTN_IDX_SUBTITLE_RecentlyChannel = 1;
    const int BTN_IDX_SUBTITLE_FavoriteChannnel = 2;
    const int BTN_IDX_SUBTITLE_LocalPouplarChannel = 3;
    const int BTN_IDX_SUBTITLE_LocalChannel = 4;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___FeaturedChannel = 0;
    const int SECTION_FOR_MORE_POPUP___RecentlyChannel = 1;
    const int SECTION_FOR_MORE_POPUP___FavoriteChannnel = 2;
    const int SECTION_FOR_MORE_POPUP___LocalPouplarChannel = 3;
    const int SECTION_FOR_MORE_POPUP___LocalChannel = 4;

    const int HTTP_SET_QUEUE = 0;


    RoseRadioHome::RoseRadioHome(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_filter, parent) {

        global.isDrawingMainContent = false;

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
        connect(linker, SIGNAL(signal_message_show_timeout()), SLOT(slot_time_out()));
    }


    RoseRadioHome::~RoseRadioHome(){

        this->deleteLater();
    }

    /**
     * @brief RoseHome::setActivePage
     */
    void RoseRadioHome::setActivePage(){

        if(this->flagNeedReload == false){
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303

            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();
                this->flagNeedReload = true;

                this->box_contents->removeWidget(this->stackedWidget_Contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 10, 0, 0);
                this->box_rose_contents->setAlignment(Qt::AlignTop);

                this->widget_rose_contents = new QWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);
                this->widget_rose_contents->hide();

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();


                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);


                this->box_contents->addWidget(this->stackedWidget_Contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                // init UI
                this->vBox_featured = new QVBoxLayout();
                this->vBox_featured->setAlignment(Qt::AlignTop);

                this->vBox_recently = new QVBoxLayout();
                this->vBox_recently->setAlignment(Qt::AlignTop);

                this->vBox_favorite = new QVBoxLayout();
                this->vBox_favorite->setAlignment(Qt::AlignTop);

                this->vBox_popular = new QVBoxLayout();
                this->vBox_popular->setAlignment(Qt::AlignTop);

                this->vBox_local = new QVBoxLayout();
                this->vBox_local->setAlignment(Qt::AlignTop);

                GSCommon::clearLayout(this->vBox_featured);
                GSCommon::clearLayout(this->vBox_recently);
                GSCommon::clearLayout(this->vBox_favorite);
                GSCommon::clearLayout(this->vBox_popular);
                GSCommon::clearLayout(this->vBox_local);

                this->hBox_featured = new QHBoxLayout();
                this->hBox_recently = new QHBoxLayout();
                this->hBox_favorite = new QHBoxLayout();
                this->hBox_popular = new QHBoxLayout();
                this->hBox_local = new QHBoxLayout();

                GSCommon::clearLayout(this->hBox_featured);
                GSCommon::clearLayout(this->hBox_recently);
                GSCommon::clearLayout(this->hBox_favorite);
                GSCommon::clearLayout(this->hBox_popular);
                GSCommon::clearLayout(this->hBox_local);

                // sub Title UI
                for(int i = 0; i < 10; i++){
                    this->lb_subTitle[i] = new QLabel();
                    this->btnView_all[i] = new QPushButton();
                }

                for(int i = 0; i < 15; i++){
                    this->home_featured[i] = new roseRadio::ItemRectangle(i, SECTION_FOR_MORE_POPUP___FeaturedChannel, tidal::AbstractItem::ImageSizeMode::Ractangle_450x120);
                    connect(this->home_featured[i], &roseRadio::ItemRectangle::rectangle_clicked, this, &RoseRadioHome::slot_clickedItemRoseRadio);
                }

                for(int i = 0; i < 15; i++){
                    this->home_recently[i] = new roseRadio::ItemSquare(i, SECTION_FOR_MORE_POPUP___RecentlyChannel, tidal::AbstractItem::ImageSizeMode::Square_284x284);
                    connect(this->home_recently[i], &roseRadio::ItemSquare::square_clicked, this, &RoseRadioHome::slot_clickedItemRoseRadio);
                }

                for(int i = 0; i < 15; i++){
                    this->home_favorite[i] = new roseRadio::ItemSquare(i, SECTION_FOR_MORE_POPUP___FavoriteChannnel, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->home_favorite[i], &roseRadio::ItemSquare::square_clicked, this, &RoseRadioHome::slot_clickedItemRoseRadio);
                }

                for(int i = 0; i < 15; i++){
                    this->home_popular[i] = new roseRadio::ItemSquare(i, SECTION_FOR_MORE_POPUP___LocalPouplarChannel, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->home_popular[i], &roseRadio::ItemSquare::square_clicked, this, &RoseRadioHome::slot_clickedItemRoseRadio);
                }

                for(int i = 0; i < 15; i++){
                    this->home_local[i] = new roseRadio::ItemSquare(i, SECTION_FOR_MORE_POPUP___LocalChannel, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                    connect(this->home_local[i], &roseRadio::ItemSquare::square_clicked, this, &RoseRadioHome::slot_clickedItemRoseRadio);
                }

                if(global.updateCheckFlag){

                    //print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223
                    //QTimer::singleShot(10000, this, SLOT(slot_hide_msg()));

                    this->widget_rose_contents->hide();
                    this->widget_login_contents->show();
                    this->stackedWidget_Contents->setCurrentIndex(1);

                    if(global.enable_section_left == true){
                        global.enable_section_left = false;
                    }
                }
                else if(!global.user.getAccess_token().isEmpty()){

                    global.enable_message_count = 0;
                    global.enable_message_flag = true;
                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                    this->setUIControl_requestRose();

                    this->widget_rose_contents->show();
                    this->widget_login_contents->hide();
                    this->stackedWidget_Contents->setCurrentIndex(0);
                }
            }
            else{

                this->flagNeedReload = true;

                this->box_contents->removeWidget(this->widget_rose_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->box_rose_contents = new QVBoxLayout();
                this->box_rose_contents->setSpacing(0);
                this->box_rose_contents->setContentsMargins(0, 0, 0, 0);
                this->box_rose_contents->setAlignment(Qt::AlignTop);



                this->widget_rose_contents = new MyQWidget();
                this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_rose_contents->setLayout(this->box_rose_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);


                this->box_contents->addWidget(this->stackedWidget_Contents);



                this->widget_rose_contents->hide();
                this->widget_login_contents->show();
                this->stackedWidget_Contents->setCurrentIndex(1);

                if(global.enable_section_left == true){
                    global.enable_section_left = false;
                }
            }
        }
    }



    QWidget* RoseRadioHome::setUIControl_LoginBefore(){

        QWidget *widget_loginBefore = new QWidget();
        widget_loginBefore->setObjectName("widget_loginBefore");
        widget_loginBefore->setFixedHeight(500);
        widget_loginBefore->setStyleSheet("#widget_loginBefore { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");

        int left, top = 0;
        QLabel *lb_text = new QLabel(widget_loginBefore);
        lb_text->setText(tr("You can use various services through Login."));
        lb_text->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:30px;");

        left = (1500 - lb_text->sizeHint().width()) / 2;
        top = (500 - 55 - 30 - lb_text->sizeHint().height()) / 2;
        lb_text->setGeometry(left, top, lb_text->sizeHint().width(), lb_text->sizeHint().height());

        QPushButton *btn_login = new QPushButton(widget_loginBefore);
        btn_login->setText(tr("Log-in"));
        btn_login->setCursor(Qt::PointingHandCursor);
        btn_login->setStyleSheet("background-color:transparent;color:#CCCCCC;font-size:20px;border:3px solid #CCCCCC;border-radius:25;");
        btn_login->setFixedSize(230, 55);
        btn_login->setGeometry(635, top + lb_text->sizeHint().height() + 30, 230, 55);


        // 커넥션
        connect(btn_login, &QPushButton::clicked, this, &RoseRadioHome::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseRadioHome::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        if(p_id == HTTP_SET_QUEUE){

           if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
           }
        }

        sender()->deleteLater();
    }

    void RoseRadioHome::setUIControl_requestRose(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303

        roseHome::ProcCommon *proc_featured = new roseHome::ProcCommon(this);
        connect(proc_featured, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioHome::slot_applyResult_Featured);
        proc_featured->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_FeaturedChannel, "/recommend?", 0, "", 0, 15);

        roseHome::ProcCommon *proc_recently = new roseHome::ProcCommon(this);
        connect(proc_recently, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioHome::slot_applyResult_Recently);
        proc_recently->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Recently_Radio, "/my/recent?", 0, "", 0, 15);

        roseHome::ProcCommon *proc_favorite = new roseHome::ProcCommon(this);
        connect(proc_favorite, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioHome::slot_applyResult_Favorite);
        proc_favorite->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Favorite_Radio, "/my/favorite?", 0, "", 0, 15);

        roseHome::ProcCommon *proc_popular = new roseHome::ProcCommon(this);
        connect(proc_popular, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioHome::slot_applyResult_Popular);
        proc_popular->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_LocalPopular, "/popular?", 0, "", 0, 15);

        roseHome::ProcCommon *proc_local = new roseHome::ProcCommon(this);
        connect(proc_local, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioHome::slot_applyResult_Local);
        proc_local->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Item_Local, "/channel?", 0, "", 0, 15);

        print_debug();ContentLoadingwaitingMsgHide();//c230322_3
    }


    void RoseRadioHome::setUIControl_appendWidget_rose(){

        if(this->flag_featured[0] == true && this->flag_recently[0] == true && this->flag_favorite[0] == true
                && flag_popular[0] == true && flag_local[0] == true){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//c230303

            if(this->flag_featured[0] == true){

                this->widget_featured = new QWidget();
                this->widget_featured = this->setUIControl_subTitle_withSideBtn(tr("Rose Featured channel"), "View All", BTN_IDX_SUBTITLE_FeaturedChannel, this->vBox_featured);

                this->vBox_featured->addSpacing(10);

                if(this->flag_featured[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_featured->setSpacing(0);
                    this->hBox_featured->setContentsMargins(0, 0, 0, 0);
                    this->hBox_featured->setAlignment(Qt::AlignTop);
                    this->hBox_featured->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_featured);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(120);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_featured->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_featured.size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_featured.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_featured[i]->setData(this->jsonArr_featured.at(i).toObject());
                        this->hBox_featured->addWidget(this->home_featured[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 120);

                    this->vBox_featured->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_featured);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_recently[0] == true){

                this->widget_recently = new QWidget();
                this->widget_recently = this->setUIControl_subTitle_withSideBtn(tr("Recently played channels"), "View All", BTN_IDX_SUBTITLE_RecentlyChannel, this->vBox_recently);

                this->vBox_recently->addSpacing(10);

                if(this->flag_recently[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_recently->setSpacing(0);
                    this->hBox_recently->setContentsMargins(0, 0, 0, 0);
                    this->hBox_recently->setAlignment(Qt::AlignTop);
                    this->hBox_recently->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_recently);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(371);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_recently->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_recently.size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_recently.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_recently[i]->setData(this->jsonArr_recently.at(i).toObject());
                        this->hBox_recently->addWidget(this->home_recently[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 287);

                    this->vBox_recently->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_recently);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_favorite[0] == true){

                this->widget_favorite = new QWidget();
                this->widget_favorite = this->setUIControl_subTitle_withSideBtn("Favorites", "View All", BTN_IDX_SUBTITLE_FavoriteChannnel, this->vBox_favorite);

                this->vBox_favorite->addSpacing(10);

                //----------------------------------------------------------------------------------------------------  BODY : START
                this->hBox_favorite->setSpacing(0);
                this->hBox_favorite->setContentsMargins(0, 0, 0, 0);
                this->hBox_favorite->setAlignment(Qt::AlignTop);
                this->hBox_favorite->setSizeConstraint(QLayout::SetFixedSize);

                QWidget *widget_content = new QWidget;
                widget_content->setLayout(this->hBox_favorite);
                widget_content->setContentsMargins(0, 0, 0, 0);

                QScrollArea *playlist_scrollArea = new QScrollArea();
                playlist_scrollArea->setWidget(widget_content);
                playlist_scrollArea->setWidgetResizable(false);
                playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                playlist_scrollArea->setFixedHeight(287);

                QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                //----------------------------------------------------------------------------------------------------  BODY : END

                // Apply Main Layout with spacing
                this->vBox_favorite->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                if(this->flag_favorite[1] == true){

                    int maxCount = 0;
                    if(this->jsonArr_favorite.size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_favorite.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_favorite[i]->setData(this->jsonArr_favorite.at(i).toObject());
                        this->hBox_favorite->addWidget(this->home_favorite[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 287);

                    this->hBox_favorite->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_favorite);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_popular[0] == true){

                this->widget_popular = new QWidget();
                this->widget_popular = this->setUIControl_subTitle_withSideBtn(tr("Local popular channels"), "View All", BTN_IDX_SUBTITLE_LocalPouplarChannel, this->vBox_popular);

                this->vBox_popular->addSpacing(10);

                if(this->flag_popular[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_popular->setSpacing(0);
                    this->hBox_popular->setContentsMargins(0, 0, 0, 0);
                    this->hBox_popular->setAlignment(Qt::AlignTop);
                    this->hBox_popular->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_popular);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(287);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_popular->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_popular.size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_popular.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_popular[i]->setData(this->jsonArr_popular.at(i).toObject());
                        this->hBox_popular->addWidget(this->home_popular[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 287);

                    this->vBox_popular->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_popular);
                this->box_rose_contents->addSpacing(60);
            }

            if(this->flag_local[0] == true){

                this->widget_local = new QWidget();
                this->widget_local = this->setUIControl_subTitle_withSideBtn(tr("Local channels"), "View All", BTN_IDX_SUBTITLE_LocalChannel, this->vBox_local);

                this->vBox_local->addSpacing(10);

                if(this->flag_local[1] == true){

                    //----------------------------------------------------------------------------------------------------  BODY : START
                    this->hBox_local->setSpacing(0);
                    this->hBox_local->setContentsMargins(0, 0, 0, 0);
                    this->hBox_local->setAlignment(Qt::AlignTop);
                    this->hBox_local->setSizeConstraint(QLayout::SetFixedSize);

                    QWidget *widget_content = new QWidget;
                    widget_content->setLayout(this->hBox_local);
                    widget_content->setContentsMargins(0, 0, 0, 0);

                    QScrollArea *playlist_scrollArea = new QScrollArea();
                    playlist_scrollArea->setWidget(widget_content);
                    playlist_scrollArea->setWidgetResizable(false);
                    playlist_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    playlist_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
                    playlist_scrollArea->setContentsMargins(0, 0, 0, 0);
                    playlist_scrollArea->setFixedHeight(287);

                    QScroller::grabGesture(playlist_scrollArea, QScroller::LeftMouseButtonGesture);
                    //----------------------------------------------------------------------------------------------------  BODY : END

                    // Apply Main Layout with spacing
                    this->vBox_local->addWidget(playlist_scrollArea, 0, Qt::AlignTop);

                    int maxCount = 0;
                    if(this->jsonArr_local.size() > 15){
                        maxCount = 15;
                    }
                    else{
                        maxCount = this->jsonArr_local.size();
                    }

                    for(int i = 0; i < maxCount; i++){
                        this->home_local[i]->setData(this->jsonArr_local.at(i).toObject());
                        this->hBox_local->addWidget(this->home_local[i]);
                    }
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
                    noData_widget->setFixedSize(1500, 287);

                    this->vBox_local->addWidget(noData_widget);
                }

                this->box_rose_contents->addLayout(this->vBox_local);
                this->box_rose_contents->addSpacing(30);
            }

            this->slot_hide_msg();
            global.isDrawingMainContent = true;
            print_debug();ContentLoadingwaitingMsgHide();//c230322_3
        }
    }


    QWidget* RoseRadioHome::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
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


    void RoseRadioHome::slot_applyResult_Featured(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_featured[1] = true;
            this->jsonArr_featured = QJsonArray();
            this->jsonArr_featured = jsonArr;
        }

        this->flag_featured[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseRadioHome::slot_applyResult_Recently(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_recently[1] = true;
            this->jsonArr_recently = QJsonArray();
            this->jsonArr_recently = jsonArr;
        }

        this->flag_recently[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseRadioHome::slot_applyResult_Favorite(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_favorite[1] = true;
            this->jsonArr_favorite = QJsonArray();
            this->jsonArr_favorite = jsonArr;
        }

        this->flag_favorite[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseRadioHome::slot_applyResult_Popular(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_popular[1] = true;
            this->jsonArr_popular = QJsonArray();
            this->jsonArr_popular = jsonArr;
        }

        this->flag_popular[0] = true;
        this->setUIControl_appendWidget_rose();
    }


    void RoseRadioHome::slot_applyResult_Local(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        if(jsonArr.size() > 0){
            this->flag_local[1] = true;
            this->jsonArr_local = QJsonArray();
            this->jsonArr_local = jsonArr;
        }

        this->flag_local[0] = true;
        this->setUIControl_appendWidget_rose();
    }



    void RoseRadioHome::slot_applyResult_Favorite_state(const QJsonObject &jsonObj){

        if(jsonObj.contains("code") && jsonObj.value("code").toString() == "200"){

            for(int i = 0; i < 15; i++){

                if((this->jsonArr_featured.size() != 0) && (i <= this->jsonArr_featured.size())){
                    if(this->jsonArr_featured.at(i).toObject().value("id").toInt() == this->fav_id){
                        this->home_featured[i]->setFavorite(this->fav_flag);
                    }
                }

                if((this->jsonArr_recently.size() != 0) && (i <= this->jsonArr_recently.size())){
                    if(this->jsonArr_recently.at(i).toObject().value("id").toInt() == this->fav_id){
                        this->home_recently[i]->setFavorite(this->fav_flag);
                    }
                }

                if((this->jsonArr_favorite.size() != 0) && (i <= this->jsonArr_favorite.size())){
                    if(this->jsonArr_favorite.at(i).toObject().value("id").toInt() == this->fav_id){
                        this->home_favorite[i]->setFavorite(this->fav_flag);
                    }
                }

                if((this->jsonArr_popular.size() != 0) && (i <= this->jsonArr_popular.size())){
                    if(this->jsonArr_popular.at(i).toObject().value("id").toInt() == this->fav_id){
                        this->home_popular[i]->setFavorite(this->fav_flag);
                    }
                }

                if((this->jsonArr_local.size() != 0) && (i <= this->jsonArr_local.size())){
                    if(this->jsonArr_local.at(i).toObject().value("id").toInt() == this->fav_id){
                        this->home_local[i]->setFavorite(this->fav_flag);
                    }
                }
            }

            roseHome::ProcCommon *proc_favorite = new roseHome::ProcCommon(this);
            connect(proc_favorite, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioHome::slot_applyResult_Favorite_reload);
            proc_favorite->request_rose_getList_radioChannel(roseHome::ProcCommon::HttpRequestType::GetList_Favorite_Radio, "/my/favorite?", 0, "", 0, 15);
        }
    }


    void RoseRadioHome::slot_applyResult_Favorite_reload(const QJsonArray &jsonArr, const int totalCount, const bool flag){

        Q_UNUSED(totalCount);
        Q_UNUSED(flag);

        for(int i = 0; i < this->jsonArr_favorite.size(); i++){
            this->home_favorite[i]->hide();
            this->home_favorite[i]->disconnect();
            this->hBox_favorite->removeWidget(this->home_favorite[i]);
        }
        GSCommon::clearLayout(this->hBox_favorite);

        this->jsonArr_favorite = QJsonArray();
        this->jsonArr_favorite = jsonArr;

        if(this->jsonArr_favorite.size() > 0){

            int maxCount = 0;
            if(this->jsonArr_favorite.size() > 15){
                maxCount = 15;
            }
            else{
                maxCount = this->jsonArr_favorite.size();
            }

            for(int i = 0; i < maxCount; i++){
                this->home_favorite[i] = new roseRadio::ItemSquare(i, SECTION_FOR_MORE_POPUP___FavoriteChannnel, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->home_favorite[i], &roseRadio::ItemSquare::square_clicked, this, &RoseRadioHome::slot_clickedItemRoseRadio);
            }

            for(int i = 0; i < maxCount; i++){
                this->home_favorite[i]->setData(this->jsonArr_favorite.at(i).toObject());
                this->hBox_favorite->addWidget(this->home_favorite[i]);
            }
        }
        else{
            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Radio_NoData);
            noData_widget->setFixedSize(1500, 287);

            this->hBox_favorite->addWidget(noData_widget);
        }
    }


    void RoseRadioHome::slot_hide_msg(){

        //ContentLoadingwaitingMsgHide();

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
    }


    void RoseRadioHome::slot_time_out(){

        if(global.enable_message_flag == true){
            global.enable_message_flag = false;
            global.enable_message_count = 0;

            this->box_contents->removeWidget(this->widget_rose_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_rose_contents = new QVBoxLayout();
            this->box_rose_contents->setSpacing(0);
            this->box_rose_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_rose_contents = new QWidget();
            this->widget_rose_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_rose_contents->setLayout(this->box_rose_contents);

            this->widget_login_contents = setUIControl_LoginBefore();
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_rose_contents);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);

            this->box_contents->addWidget(this->stackedWidget_Contents);

            this->widget_rose_contents->hide();
            this->widget_login_contents->show();
            this->stackedWidget_Contents->setCurrentIndex(1);
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }

        ContentLoadingwaitingMsgHide();
    }


    /**
     * @brief RoseHome::slot_showLoginPage [SLOT] 로그인 버튼 클릭시
     */
    void RoseRadioHome::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    /**
     * @brief RoseHome::slot_getMyInfo_loginAfter [SLOT] Rosehome Contents Display
     */
    void RoseRadioHome::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseRadioHome::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseRadioHome::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_FeaturedChannel){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/recommend?";
            data_page.type = "FeaturedChannel";

            this->movePage_radio_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_RecentlyChannel){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/my/recent?";
            data_page.type = "RecentlyChannel";

            this->movePage_radio_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_FavoriteChannnel){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/my/favorite?";
            data_page.type = "FavoriteChannnel";

            this->movePage_radio_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_LocalPouplarChannel){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/popular?";
            data_page.type = "LocalPouplarChannel";

            this->movePage_radio_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_LocalChannel){

            roseHome::PageInfo_RadioAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = "/channel?";
            data_page.type = "LocalChannel";

            this->movePage_radio_allView(data_page);
        }
    }


    void RoseRadioHome::slot_clickedItemRoseRadio(const int section, const int index, const roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio clickMode){

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___FeaturedChannel){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_featured.at(index).toObject();

                QJsonArray data;
                data.append(channel);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("data", data);
                json.insert("currentPosition", 0);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/rose_radio_play";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                QJsonObject channel = this->jsonArr_featured.at(index).toObject();

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = ProcJsonEasy::getInt(channel, "id");
                this->fav_flag = true;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", this->fav_id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_featured.at(index).toObject();

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = ProcJsonEasy::getInt(channel, "id");
                this->fav_flag = false;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", this->fav_id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___RecentlyChannel){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_recently.at(index).toObject();

                QJsonArray data;
                data.append(channel);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("data", data);
                json.insert("currentPosition", 0);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/rose_radio_play";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                QJsonObject channel = this->jsonArr_recently.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = true;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_recently.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = false;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___FavoriteChannnel){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_favorite.at(index).toObject();

                QJsonArray data;
                data.append(channel);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("data", data);
                json.insert("currentPosition", 0);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/rose_radio_play";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                QJsonObject channel = this->jsonArr_favorite.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = true;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_favorite.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = false;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___LocalPouplarChannel){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_popular.at(index).toObject();

                QJsonArray data;
                data.append(channel);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("data", data);
                json.insert("currentPosition", 0);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/rose_radio_play";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                QJsonObject channel = this->jsonArr_popular.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = true;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_popular.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = false;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
        else if(section == SECTION_FOR_MORE_POPUP___LocalChannel){

            if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_local.at(index).toObject();

                QJsonArray data;
                data.append(channel);

                // Rose Play 요청
                QJsonObject json = QJsonObject();
                json.insert("data", data);
                json.insert("currentPosition", 0);
                json.insert("roseToken", global.device.getDeviceRoseToken());

                NetworkHttp *network = new NetworkHttp();
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

                QString Url = "http://" + global.device.getDeviceIP() + ":" +global.port + "/rose_radio_play";
                network->request(HTTP_SET_QUEUE,
                                 Url,
                                 json,
                                 true,
                                 true);
            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                QJsonObject channel = this->jsonArr_local.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = true;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
            else if(clickMode == roseRadio::ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                QJsonObject channel = this->jsonArr_local.at(index).toObject();

                int id = ProcJsonEasy::getInt(channel, "id");

                this->fav_section = section;
                this->fav_index = index;
                this->fav_id = id;
                this->fav_flag = false;

                QJsonObject radioChannel;
                radioChannel.insert("favorite", this->fav_flag);
                radioChannel.insert("id", id);

                QJsonObject json;
                json.insert("radioChannel", radioChannel);

                roseHome::ProcCommon *proc_fav = new roseHome::ProcCommon(this);
                connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &RoseRadioHome::slot_applyResult_Favorite_state);
                proc_fav->request_rose_setFavorite_Radio(json);

            }
        }
    }
}
