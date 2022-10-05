#include "podcast/podcastmain.h"

#include "common/gscommon.h"

#include "widget/toastmsg.h"

#include <QDebug>


namespace podcast {

    PodCastMain::PodCastMain(QWidget *parent) : AbstractMainContentForPodcast(parent)
    {

        this->setInit();
        this->setUIControl();
    }


    PodCastMain::~PodCastMain(){

        this->deleteLater();
    }


    /**
     * @brief PodCastMain::setInit : 초기세팅
     */
    void PodCastMain::setInit(){
        currMainMenuCode = GSCommon::MainMenuCode::PodCast;

        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
    }


    /**
     * @brief PodCastMain::setUIControl : UI 세팅
     */
    void PodCastMain::setUIControl(){

        podcastHome = new Podcast_Home();

        stackedWidget_content->addWidget(podcastHome);


        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_PC_HOME;
        goToMoveNewOrderPage(tmp_data);
    }

    /**
     * @brief MusicMain::slot_search : [슬롯] 검색 요청
     * @param p_text
     */
    void PodCastMain::slot_search(const QString &p_text){

        if(p_text.size() >= 2){
            QJsonObject tmp_data;
            tmp_data[KEY_PAGE_CODE] = PAGECODE_PC_SEARCH;
            tmp_data[KEY_DATA] = p_text;
            this->goToMoveNewOrderPage(tmp_data);
        }else{
            ToastMsg::show(this, "", tr("Please enter at least two letters for the search word."));
            //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
        }
    }


    /**
     * @brief PodCastMain::slot_clickedViewAll [SLOT] View All 클릭시
     * @param p_viewAllMode
     */
    void PodCastMain::slot_clickedViewAll(const QJsonObject &p_jsonData){
        // 페이지 히스토리 관리 관련된 Abs함수
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_VIEWALL_MODE].toString();
        tmp_data[KEY_DATA] = p_jsonData;
        this->goToMoveNewOrderPage(tmp_data);
    }


    /**
     * @brief PodCastMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void PodCastMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        hidePodcastChannelBG();

        QJsonObject p_jsonObject_titleMain;
        QJsonArray *p_jsonArray_titlSub = new QJsonArray();

        p_jsonObject_titleMain["name"] = tr("Podcast");
        //p_jsonObject_titleMain["name"] = "팟캐스트";
        p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::PodCast;
        QJsonObject sub1 { {"name", tr("Popular")}, {"code", PAGECODE_PC_HOME} };
        //QJsonObject sub1 { {"name", tr("인기")}, {"code", PAGECODE_PC_HOME} };
        QJsonObject sub2 { {"name", tr("CATEGORY")}, {"code", PAGECODE_PC_CATEGORY} };
        //QJsonObject sub2 { {"name", tr("카테고리")}, {"code", PAGECODE_PC_CATEGORY} };
        QJsonObject sub3 { {"name", tr("SUBSCRIBE")}, {"code", PAGECODE_PC_SUBSCRIBE} };
        //QJsonObject sub3 { {"name", tr("구독")}, {"code", PAGECODE_PC_SUBSCRIBE} };
        QJsonObject sub4 { {"name", tr("User Channel")}, {"code", "user"} };
        //QJsonObject sub4 { {"name", tr("사용자채널")}, {"code", "user"} };
        QJsonObject sub5 { {"name", tr("My Collection")}, {"code", "my"} };
        p_jsonArray_titlSub->push_back(sub1);
        p_jsonArray_titlSub->push_back(sub2);
        p_jsonArray_titlSub->push_back(sub3);

        this->topMenuBar->setVisible(true);

        AbstractMainContent::movePageOnly(p_pageCode, p_data);


        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
        if(p_pageCode==PAGECODE_PC_HOME){

            if(podcastHome==nullptr){
                podcastHome = new Podcast_Home();
                stackedWidget_content->addWidget(podcastHome);
            }
            podcastHome->requestData();
            stackedWidget_content->setCurrentWidget(this->podcastHome);
        }
        else if(p_pageCode==PAGECODE_PC_CATEGORY){
            if(podcastCategory==nullptr){
                podcastCategory = new Podcast_Category();
                stackedWidget_content->addWidget(podcastCategory);
                connect(podcastCategory, SIGNAL(signal_reqSubPage(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
            }
            stackedWidget_content->setCurrentWidget(podcastCategory);
            podcastCategory->requestData();
        }
        else if(p_pageCode==PAGECODE_PC_SUBSCRIBE){
            if(podcastSubscribe==nullptr){
                podcastSubscribe = new Podcast_Subscription();
                stackedWidget_content->addWidget(podcastSubscribe);
            }
            stackedWidget_content->setCurrentWidget(podcastSubscribe);
            podcastSubscribe->requestData(true);
        }
        else if(p_pageCode==PAGECODE_PC_CATEGORY_DETAIL){

            if(viewAll_category==nullptr){
                viewAll_category = new Podcast_Category_Detail();
                stackedWidget_content->addWidget(viewAll_category);
            }
            int tmp_genreID =1;
            if(p_data.contains(KEY_DATA)){
                QJsonObject jsonData = p_data[KEY_DATA].toObject();
                if(jsonData.contains("collectionName")){
                    p_jsonObject_titleMain["name"] = jsonData["collectionName"].toString();
                }

                if(jsonData.contains("genreID")){
                    tmp_genreID = jsonData["genreID"].toInt();
                }
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
            stackedWidget_content->setCurrentWidget(viewAll_category);
            viewAll_category->requestData(tmp_genreID);
        }
        else if(p_pageCode==PAGECODE_PC_SUB_CHANNEL){

            if(podcastChannel==nullptr){
                podcastChannel = new Podcast_Channel();
                stackedWidget_content->addWidget(podcastChannel);
            }
            QJsonObject jsonData;
            if(p_data.contains(KEY_DATA)){
                 jsonData = p_data[KEY_DATA].toObject();
                 if(jsonData.contains("collectionName")){
                     p_jsonObject_titleMain["name"] = jsonData["collectionName"].toString();
                 }
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain,new QJsonArray());
            stackedWidget_content->setCurrentWidget(podcastChannel);
            podcastChannel->setData(jsonData);
        }
        else if(p_pageCode==PAGECODE_PC_SEARCH){
            if(podcastSearch==nullptr){
                podcastSearch = new Podcast_Search();
                stackedWidget_content->addWidget(podcastSearch);
                connect(this->podcastSearch, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            podcastSearch->search(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(podcastSearch);
        }
    }


    void PodCastMain::mousePressEvent(QMouseEvent *event){
        Q_UNUSED(event);


    }
}
