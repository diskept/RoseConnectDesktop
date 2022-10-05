#include "cdplay/CDplay_Main.h"

#include "common/common_struct.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMetaObject>
#include <QResizeEvent>


namespace CDplay {

    /**
     * @brief RoseRadio 메인 생성자
     * @param parent
     */
    CDplayMain::CDplayMain(QWidget *parent) : AbstractMainContent(parent){

        linker = Linker::getInstance();
        connect(linker, &Linker::signal_clicked_movePage, this, &CDplayMain::goToMoveNewOrderPage);
        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730

        this->setUIControl();
    }


    CDplayMain::~CDplayMain(){

        this->deleteLater();
    }


    void CDplayMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void CDplayMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void CDplayMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }


    /**
     * @brief 검색어에 대하여 CD 검색 메인 페이지를 보이도록 한다.
     */
    void CDplayMain::process_searchWord(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_RR_SEARCHMAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }



    void CDplayMain::slot_search(const QString &search_word){

        if(search_word.size() >= 2){
            global.search_text = search_word;//searchtextcopy

            common::SearchData data_search;
            data_search.search_word = search_word;

            QJsonObject jsonObj_search;
            jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
            jsonObj_search[KEY_PAGE_CODE] = PAGECODE_RR_SEARCHMAIN;

            this->goToMoveNewOrderPage(jsonObj_search);
        }
        else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        }
    }


    void CDplayMain::setUIControl(){

        QString pageCode_firstPage = PAGECODE_CD_HOME;
        QJsonObject jsonObj_first;
        jsonObj_first[KEY_PAGE_CODE] = pageCode_firstPage;

        this->goToMoveNewOrderPage(jsonObj_first);
    }


    /**
     * @brief RoseRadio 각 하위 페이지 클래스 마다, 발생하는 signal_clickedMovePage 시그널을 공통으로 처리한다.
     * @param p_jsonData : 요청한 페이지에 대한 정보가 담겨있다. JsonObject 형식. @n
     *    @li KEY_PAGE_CODE : 해당 페이지 클래스에 대응되는 Unique한 값이다. @see gscommon.h 파일에 정의.
     *    @li KEY_DATA : 옵션 JsonObject. 해당 페이지 클래스에 그대로 넘겨서 보내줄 데이터이다. {} 일 수 있음.
     */
    void CDplayMain::slot_clickedMoveSubPage(const QJsonObject &p_jsonData){

        // 페이지 히스토리 관리 관련된 Abstract 함수 호출
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_PAGE_CODE].toString();
        tmp_data[KEY_DATA] = p_jsonData;

        this->goToMoveNewOrderPage(tmp_data);
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Main 타이틀 정보를 JsonObject 로 생성하여 반환
     * @return QJsonObject
     */
    QJsonObject CDplayMain::getJsonObject_forMainTitle(){

        QJsonObject jsonObj;
        jsonObj["name"] = tr("CD");
        jsonObj[KEY_MAIN_CODE] = GSCommon::MainMenuCode::CDplay;

        return jsonObj;
    }


    /**
     * @brief TopMenuBar 객체에게 전달하기 위한 Sub 타이틀 정보 여러개를 JsonArray 로 생성하여 반환
     * @return QJsonArray* :
     */
    QJsonArray CDplayMain::getJsonArray_forSubTitles(){

        QJsonArray p_jsonArray_titlSub ;
        p_jsonArray_titlSub.push_back(QJsonObject{ {"name", tr("Playing")}, {"code", PAGECODE_CD_HOME} });

        return p_jsonArray_titlSub;
    }


    /**
     * @brief RoseRadioMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void CDplayMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        QString pageCode = "";
        QJsonObject data;

        if(global.user.isValid() == false && (p_pageCode != PAGECODE_CD_HOME)){

            if(p_pageCode == PAGECODE_RR_SEARCHMAIN){
                pageCode = p_pageCode;
                data = p_data;
            }
            else{
                pageCode = PAGECODE_CD_HOME;
                data[KEY_PAGE_CODE] = PAGECODE_CD_HOME;
            }
        }
        else{
            pageCode = p_pageCode;
            data = p_data;
        }

        QJsonObject tmp_jsonObj_titleMain = this->getJsonObject_forMainTitle();
        QJsonArray tmp_jsonArr_titlSub = this->getJsonArray_forSubTitles();

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(pageCode, data);
        if(this->sub_home != nullptr){
            this->sub_home->hide_topBtnControl();
        }

        if(p_pageCode == "playfullscreen"){
            tmp_jsonObj_titleMain = QJsonObject();
            tmp_jsonArr_titlSub = QJsonArray();

            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        else{
            // 자식 override 함수 서술
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(tmp_jsonObj_titleMain, &tmp_jsonArr_titlSub);
        }
        this->topMenuBar->setVisible(true);


        // 서브 메뉴페이지에 대한 처리
        if(pageCode == PAGECODE_CD_HOME){
            this->sub_home = this->procCommon_showSubPage<CDplay::CDplayHome>(true, this->sub_home, data);
            this->sub_home->show_topBtnControl();
        }

    }

    /**
     * @brief RoseRadioMain::procCommon_showSubPage
     * @param flagShow_topBar
     * @param sub_widget
     */
    template<typename T_CDplaySub> T_CDplaySub* CDplayMain::procCommon_showSubPage(const bool flagShow_topBar, T_CDplaySub* sub_widget, const QJsonObject &jsonObj_data){

        this->topMenuBar->setVisible(flagShow_topBar);

        if(sub_widget == nullptr){

            if(ProcJsonEasy::getString(jsonObj_data, KEY_PAGE_CODE) == "CD_HOME"){
                sub_widget = new T_CDplaySub(this->topMenuBar);
                this->stackedWidget_content->addWidget(sub_widget);
                this->stackedWidget_content->addWidget(le_search_back);//c220730
            }
            else{
                sub_widget = new T_CDplaySub();
            }

            sub_widget->setConnectSiganl_toMovePage(this, SLOT(slot_clickedMoveSubPage(const QJsonObject&)));
        }
        stackedWidget_content->setCurrentWidget(sub_widget);

        sub_widget->setJsonObject_forData(ProcJsonEasy::getJsonObject(jsonObj_data, KEY_DATA));
        sub_widget->setActivePage();

        return sub_widget;
    }
}
