#include "roseRadio/roseradio_SearchMain.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include <QScrollBar>


namespace roseRadio {

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


    const int HTTP_SET_QUEUE = 0;

    const int SECTION_FOR_MORE_POPUP___SEARCH = 0;

    const int GET_ITEM_SIZE___ONCE = 30;


    roseRadioSearchMain::roseRadioSearchMain(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseradio, parent) {

        this->linker = Linker::getInstance();
        connect(linker, SIGNAL(signal_logined()), SLOT(slot_getMyInfo_loginAfter()));
        connect(linker, SIGNAL(signal_change_device_state(QString)), SLOT(slot_change_device_state(QString)));

        this->list_genreInfo = new QList<roseRadio::RoseRadioGenreInfo>();
        this->list_countryInfo = new QList<roseRadio::RoseRadioRegionInfo>();
        this->list_languageInfo = new QList<roseRadio::RoseRadioLanguageInfo>();

        this->setUIControl_Search();
    }


    roseRadioSearchMain::~roseRadioSearchMain(){

        this->deleteLater();
    }


    void roseRadioSearchMain::setJsonObject_forData(const QJsonObject& jsonObj){

        common::SearchData tmp_data = common::ConvertData_common::convertData_searchData(jsonObj);
        this->flagNeedReload = false;

        if(tmp_data.search_word != this->data_search.search_word){
            this->flagNeedReload = true;

            this->data_search = common::ConvertData_common::convertData_searchData(jsonObj);
            this->label_mainTitle->setText(QString("'%1' ").arg(this->data_search.search_word) + tr("Search result"));
        }
    }


    void roseRadioSearchMain::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;

                if(this->search_draw_cnt > 0){
                    for(int i = 0; i < this->search_draw_cnt; i++){
                        this->viewAll_search[i]->hide();
                        this->viewAll_search[i]->disconnect();
                        this->flowLayout_search->removeWidget(this->viewAll_search[i]);
                    }
                }
                GSCommon::clearLayout(this->flowLayout_search);

                this->box_contents->removeWidget(this->widget_search);
            }

            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            // layout for items
            this->flowLayout_search = new FlowLayout(0, 0, 20);
            this->flowLayout_search->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_search);

            this->widget_search = new QWidget();
            this->widget_search->setLayout(this->flowLayout_search);
            this->widget_search->setContentsMargins(0, 20, 0, 0);
            this->widget_search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_search->setStyleSheet("background-color:transparent;");
            this->widget_search->hide();

            this->widget_login_contents = setUIControl_LoginBefore();
            this->widget_login_contents->hide();

            this->stackedWidget_Contents = new QStackedWidget();
            this->stackedWidget_Contents->addWidget(this->widget_search);
            this->stackedWidget_Contents->addWidget(this->widget_login_contents);

            this->box_contents->addWidget(this->stackedWidget_Contents);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // layout for items
            /*this->flowLayout_search = new FlowLayout(0, 0, 20);
            this->flowLayout_search->setSizeConstraint(QLayout::SetMinimumSize);

            GSCommon::clearLayout(this->flowLayout_search);

            this->widget_search = new QWidget();
            this->widget_search->setLayout(this->flowLayout_search);
            this->widget_search->setContentsMargins(0, 20, 0, 0);
            this->widget_search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            this->widget_search->setStyleSheet("background-color:transparent;");

            this->box_contents->addWidget(this->widget_search);*/

            // init
            this->next_offset = 0;
            this->search_total_cnt = 0;
            this->search_draw_cnt = 0;

            this->jsonArr_search = QJsonArray();

            // request HTTP API
            this->flagReqMore_search = false;
            this->flag_lastPage_search = false;

            this->flag_flow_draw = false;

            this->genre_offset = 0;
            this->region_offset = 0;
            this->language_offset = 0;

            this->genre_total_count = 0;
            this->region_total_count = 0;
            this->language_total_count = 0;

            this->jsonArr_genre = QJsonArray();
            this->jsonArr_region = QJsonArray();
            this->jsonArr_language = QJsonArray();

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->setUIControl_Filter();

            this->list_genreInfo->clear();
            for(int i = 0; i < global.user_forRoseRadio.getGenreInfo()->size(); i++){
                this->list_genreInfo->append(global.user_forRoseRadio.getGenreInfo()->at(i));
            }

            this->list_countryInfo->clear();
            for(int i = 0; i < global.user_forRoseRadio.getRegionInfo()->size(); i++){
                this->list_countryInfo->append(global.user_forRoseRadio.getRegionInfo()->at(i));
            }

            this->list_languageInfo->clear();
            for(int i = 0; i < global.user_forRoseRadio.getLanguageInfo()->size(); i++){
                this->list_languageInfo->append(global.user_forRoseRadio.getLanguageInfo()->at(i));
            }

            this->flag_filter_ready = false;

            this->requestGetRoseRadioGenreInfo();
            this->requestGetRoseRadioRegionInfo();
            this->requestGetRoseRadioLanguageInfo();

            this->search_genreID = 0;
            this->search_countryID = 0;
            this->search_languageID = 0;

            // request HTTP API
            this->request_more_searchData();

            this->widget_search->show();
            this->widget_login_contents->hide();
            this->stackedWidget_Contents->setCurrentIndex(0);
        }
    }


    QWidget* roseRadioSearchMain::setUIControl_LoginBefore(){

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
        //connect(btn_login, &QPushButton::clicked, this, &roseRadioSearchMain::slot_showLoginPage);

        return widget_loginBefore;
    }


    void roseRadioSearchMain::setUIControl_Search(){

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
        global.btn_RoseRadio->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseRadio").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg(tmp_btnStyleHover));
        global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle).arg(tmp_btnStyleHover));//cheon211115-01
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

        // Main Title
        this->label_mainTitle = this->get_addUIControl_mainTitle(" ", global.btn_Music, global.btn_RoseRadio, global.btn_RoseTube, global.btn_Tidal,  global.btn_Bugs, global.btn_Qobuz );//cheon211008-search

        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    void roseRadioSearchMain::setUIControl_Filter(){

        if(this->flag_flow_draw == true){
            if(this->search_filter_cnt > 0){
                for(int i = 0; i < this->search_filter_cnt; i++)
                {
                    this->btn_filter[i]->hide();
                    this->btn_filter[i]->disconnect();
                    this->flow_filter->removeWidget(this->btn_filter[i]);
                }
            }
            GSCommon::clearLayout(this->flow_filter);
        }
        GSCommon::clearLayout(this->box_filter);

        this->filterTypeList.clear();
        this->filterTypeList << tr("All genre") << tr("All country") << tr("All language");

        this->btnStyle_normal = "QPushButton{ ";
        this->btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:16px;";
        this->btnStyle_normal += "border:1px solid #707070; border-radius:15px;}";
        this->btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:16px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:15px;}";

        // 상단 필터
        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121; }");
        widget_btnFilter->setFixedSize(1400, 50);

        this->flow_filter = new FlowLayout(0, 20, 0);
        this->flow_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_filter = new QWidget(widget_btnFilter);
        this->widget_filter->setFixedSize(1400, 50);
        this->widget_filter->setStyleSheet("background:transparent;");
        this->widget_filter->setLayout(this->flow_filter);
        this->widget_filter->setGeometry(0, 10, 0, 0);

        this->box_filter->addWidget(widget_btnFilter);

        search_filter_cnt = 0;
        for(int i = 0; i < 3; i++){
            this->btn_filter[i] = new QPushButton();
            this->btn_filter[i]->setContentsMargins(0, 0, 0, 0);
            this->btn_filter[i]->setFixedHeight(30);
            this->btn_filter[i]->setStyleSheet(this->btnStyle_normal);
            this->btn_filter[i]->setCursor(Qt::PointingHandCursor);
            this->btn_filter[i]->setProperty("idx", i);
            this->btn_filter[i]->setText(this->filterTypeList.at(i));

            int width = this->btn_filter[i]->sizeHint().width() + 30;
            this->btn_filter[i]->setFixedWidth(width);

            connect(this->btn_filter[i], SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));

            this->flow_filter->addWidget(this->btn_filter[i]);
            search_filter_cnt++;
        }
    }


    void roseRadioSearchMain::proc_wheelEvent_to_getMoreData(){

        this->request_more_searchData();
    }


    void roseRadioSearchMain::request_more_searchData(){

        if(!this->flagReqMore_search && !this->flag_lastPage_search
                && ((this->jsonArr_search.size() == this->search_draw_cnt) && (this->flag_search_draw == false))){
            this->flagReqMore_search = true;

            // next_offset
            if(this->search_total_cnt == 0){
                this->next_offset = 0;
                this->jsonArr_search = QJsonArray();
            }
            else{
                this->next_offset++;

                ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            }

            // request HTTP API
            roseHome::ProcCommon *proc_channel = new roseHome::ProcCommon(this);
            connect(proc_channel, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &roseRadioSearchMain::slot_applyResult_searchChannel);
            proc_channel->request_rose_getList_radioSearch(this->search_countryID, this->search_genreID, this->search_languageID, this->next_offset, GET_ITEM_SIZE___ONCE, "NAME_ASC", this->data_search.search_word);

            if(this->next_offset == 0){
                this->flag_search_draw = true;
            }
        }
    }


    void roseRadioSearchMain::request_more_searchDraw(){

        int start_index = this->search_draw_cnt;
        int max_cnt = ((this->search_total_cnt - this->search_draw_cnt) > GET_ITEM_SIZE___ONCE ) ? GET_ITEM_SIZE___ONCE : (this->search_total_cnt - this->search_draw_cnt);
        this->search_draw_cnt += max_cnt;

        for(int i = start_index; i < this->search_draw_cnt; i++){
            this->viewAll_search[i] = new roseRadio::ItemRectangle(i, SECTION_FOR_MORE_POPUP___SEARCH, tidal::AbstractItem::ImageSizeMode::Ractangle_480x120);
            connect(this->viewAll_search[i], &roseRadio::ItemRectangle::rectangle_clicked, this, &roseRadioSearchMain::slot_clickedItemRoseRadio);
        }

        for(int i = start_index; i < this->search_draw_cnt; i++){
            this->viewAll_search[i]->setData(this->jsonArr_search.at(i).toObject());

            this->flowLayout_search->addWidget(this->viewAll_search[i]);
        }

        if(this->search_total_cnt == this->search_draw_cnt){
            this->box_contents->addSpacing(60);
        }

        this->flag_search_draw = false;
        ContentLoadingwaitingMsgHide();
    }


    void roseRadioSearchMain::requestGetRoseRadioGenreInfo(){

        // next_offset
        if(this->genre_total_count == 0){
            this->genre_offset = 0;
            this->jsonArr_genre = QJsonArray();
        }
        else{
            this->genre_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc_Genre = new roseHome::ProcCommon(this);
        connect(proc_Genre, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &roseRadioSearchMain::setResultOfRoseRadioGenreInfo);
        proc_Genre->request_rose_getList_radioGenre(this->genre_offset, GET_ITEM_SIZE___ONCE);

    }


    void roseRadioSearchMain::requestGetRoseRadioRegionInfo(){

        // next_offset
        if(this->region_total_count == 0){
            this->region_offset = 0;
            this->jsonArr_region = QJsonArray();
        }
        else{
            this->region_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc_country = new roseHome::ProcCommon(this);
        connect(proc_country, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &roseRadioSearchMain::setResultOfRoseRadioRegionInfo);
        proc_country->request_rose_getList_radioCountry(this->region_offset, GET_ITEM_SIZE___ONCE);
    }


    void roseRadioSearchMain::requestGetRoseRadioLanguageInfo(){

        // next_offset
        if(this->language_total_count == 0){
            this->language_offset = 0;
            this->jsonArr_language = QJsonArray();
        }
        else{
            this->language_offset++;
        }

        // request HTTP API
        roseHome::ProcCommon *proc_language = new roseHome::ProcCommon(this);
        connect(proc_language, &roseHome::ProcCommon::completeReq_list_roseRadio, this, &roseRadioSearchMain::setResultOfRoseRadioLanguageInfo);
        proc_language->request_rose_getList_radioLanguage(this->language_offset, GET_ITEM_SIZE___ONCE);
    }


    void roseRadioSearchMain::setResultOfRoseRadioGenreInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            if(this->genre_total_count == 0){
                this->genre_total_count = totalCount;

                if(this->list_genreInfo->size() == this->genre_total_count){
                    roseRadio::RoseRadioGenreInfo allgenre;
                    allgenre.id = 0;
                    allgenre.sort = 0;
                    allgenre.name = "All genre";
                    allgenre.imageUrl = "";
                    this->list_genreInfo->insert(0, allgenre);

                    this->applyUIControl_genres_fromData();
                }
                else{
                    this->jsonArr_genre = jsonArr;

                    if(this->jsonArr_genre.size() < this->genre_total_count && flag_lastPage == false){
                        this->requestGetRoseRadioGenreInfo();
                    }
                }
            }
            else{
                ProcJsonEasy::mergeJsonArray(this->jsonArr_genre, jsonArr);

                if(this->jsonArr_genre.size() < this->genre_total_count && flag_lastPage == false){
                    this->requestGetRoseRadioGenreInfo();
                }
                else{
                    roseRadio::RoseRadioGenreInfo allgenre;
                    allgenre.id = 0;
                    allgenre.sort = 0;
                    allgenre.name = "All genre";
                    allgenre.imageUrl = "";
                    this->list_genreInfo->insert(0, allgenre);

                    this->applyUIControl_genres_fromData();
                }
            }
        }
    }


    void roseRadioSearchMain::setResultOfRoseRadioRegionInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            if(this->region_total_count == 0){
                this->region_total_count = totalCount;

                if(this->list_countryInfo->count() == this->region_total_count){
                    roseRadio::RoseRadioRegionInfo allcountry;
                    allcountry.id = 0;
                    allcountry.sort = 0;
                    allcountry.name = "All country";
                    this->list_countryInfo->insert(0, allcountry);

                    this->applyUIControl_country_fromData();
                }
                else{
                    this->jsonArr_region = jsonArr;

                    if(this->jsonArr_region.size() < this->region_total_count && flag_lastPage == false){
                        this->requestGetRoseRadioRegionInfo();
                    }
                }
            }
            else{
                ProcJsonEasy::mergeJsonArray(this->jsonArr_region, jsonArr);

                if(this->jsonArr_region.size() < this->region_total_count && flag_lastPage == false){
                    this->requestGetRoseRadioRegionInfo();
                }
                else{
                    roseRadio::RoseRadioRegionInfo allcountry;
                    allcountry.id = 0;
                    allcountry.sort = 0;
                    allcountry.name = "All country";
                    this->list_countryInfo->insert(0, allcountry);

                    this->applyUIControl_country_fromData();
                }
            }
        }
    }


    void roseRadioSearchMain::setResultOfRoseRadioLanguageInfo(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(jsonArr.size() > 0){
            if(this->language_total_count == 0){
                this->language_total_count = totalCount;

                this->applyUIControl_country_fromData();

                if(this->list_languageInfo->count() == this->language_total_count){

                    roseRadio::RoseRadioLanguageInfo alllanguage;
                    alllanguage.id = 0;
                    alllanguage.sort = 0;
                    alllanguage.name = "All language";
                    this->list_languageInfo->insert(0, alllanguage);

                    this->applyUIControl_language_fromData();
                }
                else{
                    this->jsonArr_language = jsonArr;

                    if(this->jsonArr_language.size() < this->language_total_count && flag_lastPage == false){
                        this->requestGetRoseRadioLanguageInfo();
                    }
                }
            }
            else{
                ProcJsonEasy::mergeJsonArray(this->jsonArr_language, jsonArr);

                if(this->jsonArr_language.size() < this->language_total_count && flag_lastPage == false){
                    this->requestGetRoseRadioLanguageInfo();
                }
                else{
                    roseRadio::RoseRadioLanguageInfo alllanguage;
                    alllanguage.id = 0;
                    alllanguage.sort = 0;
                    alllanguage.name = "All language";
                    this->list_languageInfo->insert(0, alllanguage);

                    this->applyUIControl_language_fromData();
                }
            }
        }
    }


    void roseRadioSearchMain::applyUIControl_genres_fromData(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(735, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_genre = new QPushButton(widget_header);
        btn_close_genre->setText(tr("Select genre"));
        btn_close_genre->setFixedSize(735, 50);
        btn_close_genre->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_genre->setGeometry(0, 0, 0, 0);
        btn_close_genre->setCursor(Qt::PointingHandCursor);

        connect(btn_close_genre, SIGNAL(clicked()), SLOT(slot_clickBtn_Genre_close()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);

        this->flow_genre = new FlowLayout(0, 20, 20);
        this->flow_genre->setContentsMargins(10, 20, 10, 20);
        this->flow_genre->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(735, 210);
        widget_body->setStyleSheet("background:transparent;");
        widget_body->setLayout(this->flow_genre);

        QHBoxLayout *box_body = new QHBoxLayout;
        box_body->setContentsMargins(0, 0, 0, 0);
        box_body->setSpacing(0);
        box_body->addWidget(widget_body);

        for(int i = 0 ; i < this->list_genreInfo->count() ; i++){

            QString name = this->list_genreInfo->at(i).name;

            this->btn_genre[i] = new QPushButton();
            this->btn_genre[i]->setText(name.replace("&", "&&"));
            this->btn_genre[i]->setFixedSize(QSize(125, 30));
            this->btn_genre[i]->setProperty("idx", i);
            this->btn_genre[i]->setCursor(Qt::PointingHandCursor);
            if(i == 0){
                this->btn_genre[i]->setStyleSheet(this->btnStyle_selected);
            }
            else{
                this->btn_genre[i]->setStyleSheet(this->btnStyle_normal);
            }

            connect(this->btn_genre[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_Genre_choose()));

            this->flow_genre->addWidget(this->btn_genre[i]);
        }

        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addLayout(box_body);

        this->widget_genre = new QWidget(this);
        this->widget_genre->setObjectName("widget_genre");
        this->widget_genre->setStyleSheet("#widget_genre { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_genre->setFixedSize(735, 220);
        this->widget_genre->move(80, 120);
        this->widget_genre->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_genre);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setFixedSize(735, 260);
        btn_total->setGeometry(0, 0, 0, 0);
        btn_total->setLayout(box_total);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Genre_close()));

        this->widget_genre->hide();
    }


    void roseRadioSearchMain::applyUIControl_country_fromData(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(450, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_country = new QPushButton(widget_header);
        btn_close_country->setText(tr("Select country"));
        btn_close_country->setFixedSize(450, 50);
        btn_close_country->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_country->setGeometry(0, 0, 0, 0);
        btn_close_country->setCursor(Qt::PointingHandCursor);

        connect(btn_close_country, SIGNAL(clicked()), SLOT(slot_clickBtn_Country_close()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);

        this->flow_country = new FlowLayout(0, 10, 0);
        this->flow_country->setSizeConstraint(QLayout::SetMinimumSize);
        this->flow_country->setContentsMargins(10, 10, 0, 10);

        int body_height = 0;
        if(this->list_countryInfo->count() % 2 == 0){
            body_height = ((this->list_countryInfo->count() / 2)) * 35 + 30;
        }
        else{
            body_height = (((this->list_countryInfo->count() / 2) + 1) * 35) + 30;
        }

        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(450, body_height);
        widget_body->setStyleSheet("background-color:transparent;");
        widget_body->setLayout(this->flow_country);

        for(int i = 0; i < this->list_countryInfo->count(); i++){

            QPushButton *btn_country = new QPushButton();
            btn_country->setFixedSize(200, 34);
            btn_country->setProperty("idx", i);
            btn_country->setCursor(Qt::PointingHandCursor);
            btn_country->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");

            connect(btn_country, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Country_choose()));

            QLabel *countryName = new QLabel(btn_country);
            countryName->setStyleSheet("background-color:transparent; color:#FFFFFF; font-size:18px; line-height:0.89; font-weight:normal;");
            countryName->setGeometry(34, 8, 150, 20);
            countryName->setText(this->list_countryInfo->at(i).name);

            QWidget *widget_line = new QWidget();
            widget_line->setFixedSize(200, 1);
            widget_line->setStyleSheet("background-color:#4d4d4d;");

            QVBoxLayout *vbox_country = new QVBoxLayout();
            vbox_country->setContentsMargins(0, 0, 0, 0);
            vbox_country->addWidget(btn_country);
            vbox_country->addWidget(widget_line);

            QWidget *country =  new QWidget();
            country->setStyleSheet("background-color:#333333;");
            country->setFixedSize(200, 35);
            country->setLayout(vbox_country);

            this->flow_country->addWidget(country);
        }

        QScrollArea *scrollArea_country = new QScrollArea();
        scrollArea_country->setObjectName("scrollArea_country");
        scrollArea_country->setWidget(widget_body);
        scrollArea_country->setWidgetResizable(false);
        scrollArea_country->setStyleSheet("#scrollArea_country { background-color:transparent; border:0px; }");
        scrollArea_country->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea_country->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea_country->setFixedSize(450, 300);

        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addWidget(scrollArea_country);

        this->widget_country = new QWidget(this);
        this->widget_country->setObjectName("widget_country");
        this->widget_country->setStyleSheet("#widget_country { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_country->setFixedSize(450, 350);
        this->widget_country->move(80 + this->btn_filter[1]->geometry().left(), 120);
        this->widget_country->setContentsMargins(1, 1, 1, 1);
        this->widget_country->setLayout(box_total);

        this->widget_country->hide();
    }


    void roseRadioSearchMain::applyUIControl_language_fromData(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(450, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_language = new QPushButton(widget_header);
        btn_close_language->setText(tr("Select language"));
        btn_close_language->setFixedSize(450, 50);
        btn_close_language->setStyleSheet("color:#CCCCCC;font-size:16px;background-color:transparent;");
        btn_close_language->setGeometry(0, 0, 0, 0);
        btn_close_language->setCursor(Qt::PointingHandCursor);

        connect(btn_close_language, SIGNAL(clicked()), SLOT(slot_clickBtn_Language_close()));

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);

        this->flow_language = new FlowLayout(0, 10, 0);
        this->flow_language->setSizeConstraint(QLayout::SetMinimumSize);
        this->flow_language->setContentsMargins(10, 10, 0, 10);

        int body_height = 0;
        if(this->list_languageInfo->count() % 2 == 0){
            body_height = ((this->list_languageInfo->count() / 2)) * 35 + 30;
        }
        else{
            body_height = (((this->list_languageInfo->count() / 2) + 1) * 35) + 30;
        }

        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(450, body_height);
        widget_body->setStyleSheet("background-color:transparent;");
        widget_body->setLayout(this->flow_language);

        for(int i = 0; i < this->list_languageInfo->count(); i++){

            QPushButton *btn_language = new QPushButton();
            btn_language->setFixedSize(200, 34);
            btn_language->setProperty("idx", i);
            btn_language->setCursor(Qt::PointingHandCursor);
            btn_language->setStyleSheet(".QPushButton{ background-color:transparent; } .QPushButton:hover { background-color:#4d4d4d; } ");

            connect(btn_language, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Language_choose()));

            QLabel *languageName = new QLabel(btn_language);
            languageName->setStyleSheet("background-color:transparent; color:#FFFFFF; font-size:18px; line-height:0.89; font-weight:normal;");
            languageName->setGeometry(34, 8, 150, 20);
            languageName->setText(this->list_languageInfo->at(i).name);

            QWidget *widget_line = new QWidget();
            widget_line->setFixedSize(200, 1);
            widget_line->setStyleSheet("background-color:#4d4d4d;");

            QVBoxLayout *vbox_language = new QVBoxLayout();
            vbox_language->setContentsMargins(0, 0, 0, 0);
            vbox_language->addWidget(btn_language);
            vbox_language->addWidget(widget_line);

            QWidget *language =  new QWidget();
            language->setStyleSheet("background-color:#333333;");
            language->setFixedSize(200, 35);
            language->setLayout(vbox_language);

            this->flow_language->addWidget(language);
        }

        QScrollArea *scrollArea_language = new QScrollArea();
        scrollArea_language->setObjectName("scrollArea_language");
        scrollArea_language->setWidget(widget_body);
        scrollArea_language->setWidgetResizable(false);
        scrollArea_language->setStyleSheet("#scrollArea_language { background-color:transparent; border:0px; }");
        scrollArea_language->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea_language->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea_language->setFixedSize(450, 300);

        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addWidget(scrollArea_language);

        this->widget_language = new QWidget(this);
        this->widget_language->setObjectName("widget_language");
        this->widget_language->setStyleSheet("#widget_language { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_language->setFixedSize(450, 350);
        this->widget_language->move(80 + this->btn_filter[2]->geometry().left(), 120);
        this->widget_language->setContentsMargins(1, 1, 1, 1);
        this->widget_language->setLayout(box_total);

        this->flag_filter_ready = true;

        ContentLoadingwaitingMsgHide();

        this->widget_language->hide();
    }


    void roseRadioSearchMain::setUIControl_clear_channel(){

        if(this->flag_flow_draw == true){
            this->flag_flow_draw = false;

            if(this->search_draw_cnt > 0){
                for(int i = 0; i < this->search_draw_cnt; i++){
                    this->viewAll_search[i]->hide();
                    this->viewAll_search[i]->disconnect();
                    this->flowLayout_search->removeWidget(this->viewAll_search[i]);
                }
            }
            GSCommon::clearLayout(this->flowLayout_search);

            this->box_contents->removeWidget(this->widget_search);
            GSCommon::clearLayout(this->box_contents);
        }

        // init
        this->next_offset = 0;
        this->search_total_cnt = 0;
        this->search_draw_cnt = 0;

        this->jsonArr_search = QJsonArray();

        // request HTTP API
        this->flagReqMore_search = false;
        this->flag_lastPage_search = false;

        this->flag_flow_draw = false;

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);

        // layout for items
        this->flowLayout_search = new FlowLayout(0, 0, 20);
        this->flowLayout_search->setSizeConstraint(QLayout::SetMinimumSize);

        GSCommon::clearLayout(this->flowLayout_search);

        this->widget_search = new QWidget();
        this->widget_search->setLayout(this->flowLayout_search);
        this->widget_search->setContentsMargins(0, 20, 0, 0);
        this->widget_search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->widget_search->setStyleSheet("background-color:transparent;");

        this->box_contents->addWidget(this->widget_search);

        ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        // request HTTP API
        this->request_more_searchData();
    }


    void roseRadioSearchMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        /*QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "p_jsonObj = " << strJson;*/

        if(p_id == HTTP_SET_QUEUE){

           if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
           }
        }

        sender()->deleteLater();
    }


    /**
     * @brief RoseHome::slot_showLoginPage [SLOT] 로그인 버튼 클릭시
     */
    void roseRadioSearchMain::slot_showLoginPage(){

        this->dlg_rose_login = new Dialog::DialogLogin(this);
        this->dlg_rose_login->exec();
    }


    /**
     * @brief RoseHome::slot_getMyInfo_loginAfter [SLOT] Rosehome Contents Display
     */
    void roseRadioSearchMain::slot_getMyInfo_loginAfter(){

        if(global.user.isValid() == true && this->stackedWidget_Contents->currentIndex() == 1){

            QJsonObject tmpJson;
            tmpJson.insert("search_word", this->data_search.search_word);
            this->data_search.search_word = "";

            this->setJsonObject_forData(tmpJson);
            this->setActivePage();
        }
    }


    void roseRadioSearchMain::slot_change_device_state(const QString &category){

        if(global.user.isChanged() && (category == SIGNAL_CATEGORY_ROSE)){

            global.user.setDeviceChange(false);

            QJsonObject tmpJson;
            tmpJson.insert("search_word", this->data_search.search_word);
            this->data_search.search_word = "";

            this->setJsonObject_forData(tmpJson);
            this->setActivePage();
        }
    }


    void roseRadioSearchMain::slot_applyResult_searchChannel(const QJsonArray &jsonArr, const int totalCount, const bool flag_lastPage){

        if(jsonArr.size() > 0){
            this->flagReqMore_search = false;
            this->flag_lastPage_search = flag_lastPage;

            if(this->search_total_cnt == 0){
                this->search_total_cnt = totalCount;
                this->jsonArr_search = QJsonArray();
            }

            int start_index = this->jsonArr_search.size();
            ProcJsonEasy::mergeJsonArray(this->jsonArr_search, jsonArr);

            if(start_index == 0){
                int max_cnt = this->jsonArr_search.size();
                this->search_draw_cnt = max_cnt;

                for(int i = start_index; i < this->search_draw_cnt; i++){
                    this->viewAll_search[i] = new roseRadio::ItemRectangle(i, SECTION_FOR_MORE_POPUP___SEARCH, tidal::AbstractItem::ImageSizeMode::Ractangle_480x120);
                    connect(this->viewAll_search[i], &roseRadio::ItemRectangle::rectangle_clicked, this, &roseRadioSearchMain::slot_clickedItemRoseRadio);
                }

                for(int i = start_index; i < this->search_draw_cnt; i++){
                    this->viewAll_search[i]->setData(this->jsonArr_search.at(i).toObject());

                    this->flowLayout_search->addWidget(this->viewAll_search[i]);
                }

                if(this->flag_filter_ready == true){
                    ContentLoadingwaitingMsgHide();
                }

                this->flag_flow_draw = true;
                this->flag_search_draw = false;

                if(this->search_total_cnt == this->search_draw_cnt){
                    this->box_contents->addSpacing(60);
                }

                //this->request_more_searchData();
            }
            else{
                if((this->search_total_cnt > this->search_draw_cnt) && (this->jsonArr_search.size() > this->search_draw_cnt) && (this->flag_search_draw == false)){

                    this->flag_search_draw = true;

                    this->request_more_searchDraw();
                }
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::SearchResult_NoData);
            noData_widget->setFixedSize(1500, 700);

            this->flowLayout_search->addWidget(noData_widget);

            this->flag_flow_draw = true;
            this->flag_search_draw = false;
        }
    }


    void roseRadioSearchMain::slot_applyResult_Favorite_state(const QJsonObject &jsonObj){

        if(jsonObj.contains("code") && jsonObj.value("code").toString() == "200"){

            this->viewAll_search[this->fav_index]->setFavorite(this->fav_flag);
        }
    }


    void roseRadioSearchMain::slot_clickBtn_Filter(){

        int idx = sender()->property("idx").toInt();

        if(this->flag_filter_ready == true){
            this->widget_genre->setVisible(false);
            this->widget_country->setVisible(false);
            this->widget_language->setVisible(false);

            if(idx == 0){
                this->widget_genre->setVisible(!this->widget_genre->isVisible());
            }
            else if(idx == 1){
                this->widget_country->setVisible(!this->widget_country->isVisible());
            }
            else if(idx == 2){
                this->widget_language->setVisible(!this->widget_language->isVisible());
            }
        }
    }


    void roseRadioSearchMain::slot_clickBtn_Genre_close(){

        this->widget_genre->setVisible(false);
    }


    void roseRadioSearchMain::slot_clickBtn_Genre_choose(){

        int idx = sender()->property("idx").toInt();

        for(int i = 0; i < this->list_genreInfo->count(); i++){
            this->btn_genre[i]->setStyleSheet(this->btnStyle_normal);
        }

        this->btn_genre[idx]->setStyleSheet(this->btnStyle_selected);

        QString genreName = this->list_genreInfo->at(idx).name;
        this->btn_filter[0]->setText(genreName.replace("&", "&&"));

        int width = this->btn_filter[0]->sizeHint().width() + 30;
        this->btn_filter[0]->setFixedWidth(width);

        this->search_genreID = this->list_genreInfo->at(idx).id;

        this->setUIControl_clear_channel();

        this->slot_clickBtn_Genre_close();
    }


    void roseRadioSearchMain::slot_clickBtn_Country_close(){

        this->widget_country->setVisible(false);
    }


    void roseRadioSearchMain::slot_clickBtn_Country_choose(){

        int idx = sender()->property("idx").toInt();

        QString countryName = this->list_countryInfo->at(idx).name;
        this->btn_filter[1]->setText(countryName);

        int width = this->btn_filter[1]->sizeHint().width() + 30;
        this->btn_filter[1]->setFixedWidth(width);

        this->search_countryID = this->list_countryInfo->at(idx).id;

        this->setUIControl_clear_channel();

        this->slot_clickBtn_Country_close();
    }


    void roseRadioSearchMain::slot_clickBtn_Language_close(){

        this->widget_language->setVisible(false);
    }


    void roseRadioSearchMain::slot_clickBtn_Language_choose(){

        int idx = sender()->property("idx").toInt();

        QString languageName = this->list_languageInfo->at(idx).name;
        this->btn_filter[2]->setText(languageName);

        int width = this->btn_filter[2]->sizeHint().width() + 30;
        this->btn_filter[2]->setFixedWidth(width);

        this->search_languageID = this->list_languageInfo->at(idx).id;

        this->setUIControl_clear_channel();

        this->slot_clickBtn_Language_close();
    }


    void roseRadioSearchMain::slot_clickedItemRoseRadio(const int section, const int index, const ItemImageRoseRadio::ClickMode_RoseRadio clickMode){

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___SEARCH){

            if(clickMode == ItemImageRoseRadio::ClickMode_RoseRadio::AllBox || clickMode == ItemImageRoseRadio::ClickMode_RoseRadio::PlayBtn){

                QJsonObject channel = this->jsonArr_search.at(index).toObject();

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
            else if(clickMode == ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Add){

                if(global.user.isValid() == true){
                    QJsonObject channel = this->jsonArr_search.at(index).toObject();

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
                    connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &roseRadioSearchMain::slot_applyResult_Favorite_state);
                    proc_fav->request_rose_setFavorite_Radio(json);
                }
                else{
                    ToastMsg::show(this, "", tr("You can use it after Login"), 3000);
                }
            }
            else if(clickMode == ItemImageRoseRadio::ClickMode_RoseRadio::FavBtn_Delete){

                if(global.user.isValid() == true){
                    QJsonObject channel = this->jsonArr_search.at(index).toObject();

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
                    connect(proc_fav, &roseHome::ProcCommon::completeReq_favorite_radio, this, &roseRadioSearchMain::slot_applyResult_Favorite_state);
                    proc_fav->request_rose_setFavorite_Radio(json);
                }
                else{
                    ToastMsg::show(this, "", tr("You can use it after Login"), 3000);
                }
            }
        }
    }
}
