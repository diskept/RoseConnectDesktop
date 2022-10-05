#include "roseRadio/roseradio_Explore.h"

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

#include <QDebug>
#include <QScroller>


namespace roseRadio {

    const int SECTION_FOR_MORE_POPUP___GENRE = 0;

    const int GET_ITEM_SIZE___ONCE = 30;


    RoseRadioExplore::RoseRadioExplore(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(VerticalScroll_viewAll, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));
    }


    RoseRadioExplore::~RoseRadioExplore(){

        this->deleteLater();
    }

    /**
     * @brief RoseHome::setActivePage
     */
    void RoseRadioExplore::setActivePage(){

        if(this->flagNeedReload == false){

            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractRoseHomeSubWidget::setActivePage();
                this->flagNeedReload = true;

                this->box_contents->removeWidget(this->widget_explore_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->flow_explore_contents = new FlowLayout(0, 20, 20);
                this->flow_explore_contents->setSizeConstraint(QLayout::SetMinimumSize);

                this->widget_explore_contents = new QWidget();
                this->widget_explore_contents->setContentsMargins(0, 20, 0, 0);
                this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_explore_contents->setLayout(this->flow_explore_contents);
                this->widget_explore_contents->hide();

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_explore_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                if(global.updateCheckFlag){

                    //ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223
                    //QTimer::singleShot(10000, this, SLOT(slot_hide_msg()));

                    this->widget_explore_contents->hide();
                    this->widget_login_contents->show();
                    this->stackedWidget_Contents->setCurrentIndex(1);
                }
                else if(!global.user.getAccess_token().isEmpty()){

                    ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));              //cheon211114-01//c1223

                    this->request_more_genreData();

                    this->widget_explore_contents->show();
                    this->widget_login_contents->hide();
                    this->stackedWidget_Contents->setCurrentIndex(0);
                }
            }
            else{

                this->flagNeedReload = true;

                this->box_contents->removeWidget(this->widget_explore_contents);
                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->flow_explore_contents = new FlowLayout(0, 20, 20);
                this->flow_explore_contents->setSizeConstraint(QLayout::SetMinimumSize);

                this->widget_explore_contents = new QWidget();
                this->widget_explore_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_explore_contents->setLayout(this->flow_explore_contents);

                this->widget_login_contents = setUIControl_LoginBefore();
                this->widget_login_contents->hide();

                this->stackedWidget_Contents = new QStackedWidget();
                this->stackedWidget_Contents->addWidget(this->widget_explore_contents);
                this->stackedWidget_Contents->addWidget(this->widget_login_contents);

                this->box_contents->addWidget(this->stackedWidget_Contents);

                this->widget_explore_contents->hide();
                this->widget_login_contents->show();
                this->stackedWidget_Contents->setCurrentIndex(1);
            }
        }
    }


    QWidget* RoseRadioExplore::setUIControl_LoginBefore(){

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
        connect(btn_login, &QPushButton::clicked, this, &RoseRadioExplore::slot_showLoginPage);

        return widget_loginBefore;
    }


    void RoseRadioExplore::proc_wheelEvent_to_getMoreData(){

        if((this->genre_total_cnt > this->genre_draw_cnt) && (this->jsonArr_Genre.size() > this->genre_draw_cnt) && (this->flag_genre_draw == false)){

            this->flag_genre_draw = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_genreDraw();
        }
    }


    void RoseRadioExplore::request_more_genreData(){

        if(!this->flagReqMore_Genre && !this->flag_lastPage_Genre){
            this->flagReqMore_Genre = true;

            // next_offset
            if(this->genre_total_cnt == 0){
                this->next_offset = 0;
                this->jsonArr_Genre = QJsonArray();
            }
            else{
                this->next_offset++;
            }

            roseHome::ProcCommon *proc_Genre = new roseHome::ProcCommon(this);
            connect(proc_Genre, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &RoseRadioExplore::slot_applyResult_Genre);
            proc_Genre->request_rose_getList_radioGenre(this->next_offset, GET_ITEM_SIZE___ONCE);

            if(this->next_offset == 0){
                this->flag_genre_draw = true;
            }
        }
    }


    void RoseRadioExplore::request_more_genreDraw(){

        int start_index = this->genre_draw_cnt;
        int max_cnt = ((this->genre_total_cnt - this->genre_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->genre_total_cnt - this->genre_draw_cnt);
        this->genre_draw_cnt += max_cnt;

        for(int i = start_index; i < this->genre_draw_cnt; i++){
            this->explore_genre[i] = new roseRadio::ItemExplore(i, SECTION_FOR_MORE_POPUP___GENRE);
            connect(this->explore_genre[i], &roseRadio::ItemExplore::explore_clicked, this, &RoseRadioExplore::slot_clickedItemGenre);
        }

        for(int i = start_index; i < this->genre_draw_cnt; i++){
            this->explore_genre[i]->setData(this->jsonArr_Genre.at(i).toObject());

            this->flow_explore_contents->addWidget(this->explore_genre[i]);

            QCoreApplication::processEvents();
        }

        if(this->genre_total_cnt == this->genre_draw_cnt){
            this->box_contents->addSpacing(60);
        }

        this->flag_genre_draw = false;
        this->slot_hide_msg();
    }


    void RoseRadioExplore::slot_applyResult_Genre(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        Q_UNUSED(totalCount);

        if(jsonArr.size() > 0){
            this->flagReqMore_Genre = false;
            this->flag_lastPage_Genre = flag_lastPage;

            if(this->jsonArr_Genre.size() == 0){
                this->genre_total_cnt = totalCount;
                this->jsonArr_Genre = QJsonArray();
            }

            int start_index = this->jsonArr_Genre.size();

            if(start_index == 0){

                this->jsonArr_Genre = jsonArr;

                int countryCnt = 0;
                int langCnt = 0;
                for(int i = 0; i < this->jsonArr_Genre.size(); i++)
                {
                    QJsonObject tmpObj = this->jsonArr_Genre.at(i).toObject();

                    if(tmpObj.value("name").toString() == "Country"){
                        countryCnt++;
                    }

                    if(tmpObj.value("name").toString() == "Language"){
                        langCnt++;
                    }
                }

                if(langCnt == 0){
                    QJsonObject language;
                    language.insert("id", 0);
                    language.insert("name", "Country");
                    language.insert("sort", 100);
                    language.insert("imageUrl", ":/images/radio/radio_genre_ico1.png");

                    this->jsonArr_Genre.insert(0, language);

                    if(this->genre_total_cnt < this->jsonArr_Genre.size()){
                       this->genre_total_cnt = this->jsonArr_Genre.size();
                    }
                }

                if(langCnt == 0){
                    QJsonObject language;
                    language.insert("id", 0);
                    language.insert("name", "Language");
                    language.insert("sort", 100);
                    language.insert("imageUrl", ":/images/radio/radio_genre_ico2.png");

                    this->jsonArr_Genre.insert(1, language);

                    if(this->genre_total_cnt < this->jsonArr_Genre.size()){
                       this->genre_total_cnt = this->jsonArr_Genre.size();
                    }
                }
            }
            else{
                ProcJsonEasy::mergeJsonArray(this->jsonArr_Genre, jsonArr);
            }

            if(start_index == 0){
                int max_cnt = this->jsonArr_Genre.size();
                this->genre_draw_cnt = max_cnt;

                for(int i = start_index; i < this->genre_draw_cnt; i++){
                    this->explore_genre[i] = new roseRadio::ItemExplore(i, SECTION_FOR_MORE_POPUP___GENRE);
                    connect(this->explore_genre[i], &roseRadio::ItemExplore::explore_clicked, this, &RoseRadioExplore::slot_clickedItemGenre);
                }

                for(int i = start_index; i < this->genre_draw_cnt; i++){
                    this->explore_genre[i]->setData(this->jsonArr_Genre.at(i).toObject());

                    this->flow_explore_contents->addWidget(this->explore_genre[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_genre_draw = false;
            }

            if(this->genre_total_cnt == this->genre_draw_cnt){
                this->box_contents->addSpacing(60);
            }

            this->request_more_genreData();
            this->slot_hide_msg();
        }
        else{
            this->slot_hide_msg();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
            noData_widget->setFixedSize(1500, 290);

            this->flow_explore_contents->addWidget(noData_widget);
        }
    }


    void RoseRadioExplore::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    /**
     * @brief RoseHome::slot_showLoginPage [SLOT] 로그인 버튼 클릭시
     */
    void RoseRadioExplore::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    /**
     * @brief RoseHome::slot_getMyInfo_loginAfter [SLOT] Rosehome Contents Display
     */
    void RoseRadioExplore::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseRadioExplore::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            this->flagNeedReload = false;
            this->setActivePage();
        }
    }


    void RoseRadioExplore::slot_clickedItemGenre(const int section, const int index, const roseRadio::ItemExplore::ClickMode_radio clickMode){

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___GENRE){

            if(clickMode == roseRadio::ItemExplore::ClickMode_radio::AllBox){

                QJsonObject genreObj = this->jsonArr_Genre.at(index).toObject();

                int id = ProcJsonEasy::getInt(genreObj, "id");
                QString name = ProcJsonEasy::getString(genreObj, "name");

                if(name == "Country"){
                    roseHome::PageInfo_RadioAllView data_page;
                    data_page.pathTitle = name;
                    data_page.api_subPath = "/region?";
                    data_page.genre_id = id;
                    data_page.type = "Country";

                    this->movePage_radio_allView(data_page);
                }
                else if(name == "Language"){
                    roseHome::PageInfo_RadioAllView data_page;
                    data_page.pathTitle = name;
                    data_page.api_subPath = "/language?";
                    data_page.genre_id = id;
                    data_page.type = "Language";

                    this->movePage_radio_allView(data_page);
                }
                else{
                    roseHome::PageInfo_RadioAllView data_page;
                    data_page.pathTitle = name;
                    data_page.api_subPath = "/channel?";
                    data_page.genre_id = id;
                    data_page.type = "Genre";

                    this->movePage_radio_allView(data_page);
                }
            }
        }
    }
}
