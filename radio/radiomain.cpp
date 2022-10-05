#include "radio/radiomain.h"

#include "common/gscommon.h"
#include "common/rosesettings.h"

#include "widget/toastmsg.h"

#include <QDebug>



namespace radio {

    RadioMain::RadioMain(QWidget *parent) : AbstractMainContent(parent)
    {

        setInit();
        settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        this->setUIControl();
    }

    RadioMain::~RadioMain(){
        delete settings;
    }

    /**
     * @brief RadioMain::setInit : 초기 세팅
     */
    void RadioMain::setInit(){
        currMainMenuCode = GSCommon::MainMenuCode::Radio;

        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730
    }

    void RadioMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void RadioMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
        print_debug();
        qDebug()<<"##########################################################################################################";
        if(show){
            if(global.width_mainwindow==0){
                this->le_search_back->setFixedSize(800,500);
            }else{
                this->le_search_back->setFixedSize(global.width_mainwindow-200,global.height_mainwindow);
            }
            this->curr_widget = this->stackedWidget_content->currentWidget();
            this->curr_widget->hide();
            this->le_search_back->show();
            QTimer::singleShot(3000, this, SLOT(slot_dragEnterEvent_restore()));

        }else{
            print_debug();
            this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
            this->le_search_back->clear();
            emit linker->signal_searchBarFocusChanged(false);
            this->le_search_back->clearFocus();
            this->curr_widget->show();
            this->le_search_back->hide();
        }

    }


    void RadioMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

    /**
     * @brief RadioMain::setUIControl : UI 세팅
     */
    void RadioMain::setUIControl(){

        // 메뉴 페이지
        this->radioHome = new RadioHome(this->topMenuBar);
    //    this->radioCountry = new RadioCountry(this);
    //    this->radioUser = new RadioUser(this->topMenuBar);
    //    this->radioEdit = new RadioEdit(this->topMenuBar);
    //    this->radioSearch = new RadioSearch(this->topMenuBar);

        // 스택에 추가
        this->stackedWidget_content->addWidget(this->radioHome);
    //    this->stackedWidget_content->addWidget(this->radioCountry);
    //    this->stackedWidget_content->addWidget(this->radioUser);
    //    this->stackedWidget_content->addWidget(this->radioEdit);
    //    this->stackedWidget_content->addWidget(this->radioSearch);
        this->stackedWidget_content->addWidget(le_search_back);//c220730

    //    // 커넥트
    //    connect(radioUser, SIGNAL(signal_clickedEdit(QJsonObject)), this, SLOT(slot_reqNewPage(QJsonObject)));
          connect(this->topMenuBar, SIGNAL(signal_clickedSameSubMenu(QString)), this, SLOT(slot_showRadioCountryComboBox(QString)));
    //    connect(this->radioCountry, SIGNAL(signal_changedCurrSubMenuName(QString)), this->topMenuBar, SLOT(slot_changedSubMenuNameText(QString)));

    //    // 커넥트 : TopMenuBar의 타이틀 변경
    //    connect(this->radioSearch, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));

        // TEST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //    RadioHome *radioHome = new RadioHome(this->topMenuBar);
    //    radioHome->hide_topBtnControl();
    //    this->stackedWidget_content->addWidget(radioHome);
        // TEST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        // default : 첫번째 서브메뉴
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_R_FAVORITE;
        this->goToMoveNewOrderPage(tmp_data);
    }


    /**
     * @brief RadioMain::slot_search : [슬롯] 검색 요청
     * @param p_text
     */
    void RadioMain::slot_search(const QString &p_text){

        if(p_text.size() >= 2){
            QJsonObject tmp_data;
            tmp_data[KEY_PAGE_CODE] = PAGECODE_R_SEARCH;
            tmp_data[KEY_DATA] = p_text;
            this->goToMoveNewOrderPage(tmp_data);
        }else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
        }
    }



    /**
     * @brief RadioMain::slot_reqNewPage : [슬롯] 신규 페이지 생성 요청
     * @param p_jsonData QjsonObject 신규 페이지 생성에 필요한 데이터
     */
    void RadioMain::slot_reqNewPage(const QJsonObject &p_jsonData){
        this->goToMoveNewOrderPage(p_jsonData);
    }

    /**
     * @brief RadioMain::slot_showRadioCountryComboBox [SLOT] 국가 콤보박스 show
     * @param p_subMenuCode
     */
    void RadioMain::slot_showRadioCountryComboBox(QString p_subMenuCode){
        if(p_subMenuCode==PAGECODE_R_COUNTRY){

            if(radioCountry==nullptr){
                radioCountry = new RadioCountry(this);
                this->stackedWidget_content->addWidget(radioCountry);
                connect(this->radioCountry, SIGNAL(signal_changedCurrSubMenuName(QString)), this->topMenuBar, SLOT(slot_changedSubMenuNameText(QString)));
            }
            this->radioCountry->showComboBoxPopup();
        }
    }

    /**
     * @brief RadioMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void RadioMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){
        // top widget all hide
        if(radioHome!=nullptr){
            this->radioHome->hide_topBtnControl();
        }
        if(radioCountry!=nullptr){
            this->radioCountry->hide_comboBoxWidget(true);
            this->radioCountry->hide_topBtnControl();
        }
        if(radioUser!=nullptr){
            this->radioUser->hide_topBtnControl();
        }

        QString countryName = settings->value(rosesettings.SETTINGS_RADIO_NAME,tr("KOREA")).toString();
        QJsonObject p_jsonObject_titleMain;
        QJsonArray *p_jsonArray_titlSub = new QJsonArray();

        p_jsonObject_titleMain["name"] = tr("Radio");
        //p_jsonObject_titleMain["name"] = "라디오";
        p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Radio;
        QJsonObject sub1 { {"name", tr("Favorites")}, {"code", PAGECODE_R_FAVORITE} };
        //QJsonObject sub1 { {"name", "즐겨찾기"}, {"code", PAGECODE_R_FAVORITE} };
        QJsonObject sub2 { {"name", countryName}, {"code", PAGECODE_R_COUNTRY} };
        QJsonObject sub3 { {"name", tr("UserChannel")}, {"code", PAGECODE_R_USER} };
        //QJsonObject sub3 { {"name", "사용자채널"}, {"code", PAGECODE_R_USER} };
        p_jsonArray_titlSub->push_back(sub1);
        p_jsonArray_titlSub->push_back(sub2);
        p_jsonArray_titlSub->push_back(sub3);

        this->topMenuBar->setVisible(true);

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);

        // 자식 override 함수 서술
        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
        if(p_pageCode==PAGECODE_R_FAVORITE){

            if(radioHome==nullptr){
                this->radioHome = new RadioHome(this->topMenuBar);
                this->stackedWidget_content->addWidget(radioHome);
            }

            radioHome->requestData();
            radioHome->show_topBtnControl();
            stackedWidget_content->setCurrentWidget(radioHome);

        }else if(p_pageCode==PAGECODE_R_COUNTRY){

            if(radioCountry==nullptr){
                radioCountry = new RadioCountry(this);
                this->stackedWidget_content->addWidget(radioCountry);
                connect(this->radioCountry, SIGNAL(signal_changedCurrSubMenuName(QString)), this->topMenuBar, SLOT(slot_changedSubMenuNameText(QString)));
            }

            radioCountry->requestData();
            radioCountry->show_topBtnControl();
            stackedWidget_content->setCurrentWidget(radioCountry);

        }else if(p_pageCode==PAGECODE_R_USER){

            if(radioUser==nullptr){
                radioUser = new RadioUser(this->topMenuBar);
                this->stackedWidget_content->addWidget(radioUser);
                connect(radioUser, SIGNAL(signal_clickedEdit(QJsonObject)), this, SLOT(slot_reqNewPage(QJsonObject)));
            }

            radioUser->requestData(true);
            radioUser->show_topBtnControl();
            stackedWidget_content->setCurrentWidget(radioUser);

        }else if(p_pageCode==PAGECODE_R_EDIT){

            if(radioEdit==nullptr){
                radioEdit = new RadioEdit(this->topMenuBar);
                this->stackedWidget_content->addWidget(radioEdit);
            }
            if(p_data.contains("title")){
                p_jsonObject_titleMain["name"] = p_data["title"];
            }

            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());

            radioEdit->requestData();
            stackedWidget_content->setCurrentWidget(radioEdit);

        }else if(p_pageCode==PAGECODE_R_SEARCH){
            // -------------------------------------
            //  라디오 < 검색결과 화면
            // -------------------------------------
            if(radioSearch==nullptr){
                radioSearch = new RadioSearch(this->topMenuBar);
                this->stackedWidget_content->addWidget(radioSearch);
                connect(this->radioSearch, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
            radioSearch->search(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(radioSearch);

        }

    }
}
