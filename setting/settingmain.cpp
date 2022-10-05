#include "settingmain.h"

#include <QScrollArea>

#include <common/gscommon.h>
#include "widget/toastmsg.h"

const int PAGE_MAX_WIDTH = 990;
const int PAGE_MIN_WIDTH = 704;
/**
 * @brief SettingMain::SettingMain 생성자
 * @param parent
 */
SettingMain::SettingMain(QWidget *parent) : AbstractMainContent(parent)
{

    this->resetLayoutUIfromParent();    // 부모의 레이아웃 수정
    this->setUIControl();               // 기본 UI 세팅

    if(global.enable_section_left == true){
        global.enable_section_left = false;
    }
}

SettingMain::~SettingMain(){
    this->vl_total->removeWidget(this->stackedWidget_content);
    this->deleteLater();
}

/**
 * @brief SettingMain::resetLayoutUI 설정 메인메뉴인 경우에만 메인UI레이아웃구조 변경 (LeftMenuBar가 컨텐츠와함게 ScrollArea안에 들어가있어야하므로)
 * @note 부모의 레이아웃을 살짝 수정한다.
 */
void SettingMain::resetLayoutUIfromParent(){

    // StackWidget 레이아웃 변경하기 위해 제거
    this->vl_total->removeWidget(this->stackedWidget_content);

    this->leftMenuBar = new LeftMenuBar();

    // 스크롤 : 왼쪽 메뉴 스크롤 따로 , 오른쪽 컨텐츠 파트의 스크롤 따로,, 별도로 동작하게끔 구현함
    scrollArea_leftMenuBar = new QScrollArea();
    scrollArea_leftMenuBar->setObjectName("scrollArea");
    scrollArea_leftMenuBar->setWidget(this->leftMenuBar);
    scrollArea_leftMenuBar->setWidgetResizable(false);
    scrollArea_leftMenuBar->setStyleSheet("#scrollArea { border:0px; }");
    scrollArea_leftMenuBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_leftMenuBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_leftMenuBar->setFixedWidth(this->leftMenuBar->width());

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(0);
    hl_total->addWidget(this->scrollArea_leftMenuBar);
    hl_total->addWidget(this->stackedWidget_content, 9);

    wg_settingTotal = new QWidget();
    wg_settingTotal->setContentsMargins(0,0,0,0);
    wg_settingTotal->setLayout(hl_total);


    //this->vl_total->addLayout(hl_total, 9);
    this->vl_total->addWidget(wg_settingTotal,9);

    // --------------------------------------------
    //  미디어 라이브러리 서브 페이지 추가
    // --------------------------------------------
    settingMediaLibAddDel = new MusicFolder(this);
    settingMediaLibAddDel->hide();
    this->vl_total->addWidget(settingMediaLibAddDel);



    // 커넥션
    connect(this->leftMenuBar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)) );
}

/**
 * @brief SettingMain::setUIControl : 설정 메인메뉴에서 사용하는 컨텐츠 UI 세팅
 */
void SettingMain::setUIControl(){

    // 메뉴 페이지
    this->settingMyPage = new SettingMyPage();
//    this->settingFriendPage = new SettingFriendPage();
//    this->settingLib = new SettingLib();
//    this->settingDisplay = new SettingDisplay();
//    this->settingTrackOption = new SettingTrackOption();
//    this->settingCountry = new SettingCountry();
//    this->settingRoseTube = new SettingRoseTube();
//    this->settingVideo = new SettingVideo();
//    this->settingVU = new SettingVU();
//    this->settingInputOutput = new SettingInputOutput();
//    this->settingRoseMenu = new SettingRoseMenu();
//    this->settingAppMenu = new SettingAppMenu();
//    this->settingAppInfo = new SettingAppInfo();

//    // 팝업옵션 관련 페이지
//    this->albumDetail = new AlbumDetail();
//    this->musice_playlist = new Musice_playlist();
//    this->rosetube_playlist = new RoseTube_PlayList();
//    this->metaInfo = new MetaInfo();
//    this->artistInfo = new ArtistInfo();

//    // View All 관련 페이지
//    this->viewAll_playListMusic = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_MYPLAYLIST);
//    this->viewAll_playListRoseTube = new RoseTube_ViewAll();

    // 실 컨텐츠 레이아웃
    stackedWidget_content->setMinimumWidth(PAGE_MIN_WIDTH);

    // 스택에 추가
    this->stackedWidget_content->addWidget(this->settingMyPage);
//    this->stackedWidget_content->addWidget(this->settingFriendPage);
//    this->stackedWidget_content->addWidget(this->settingLib);
//    this->stackedWidget_content->addWidget(this->settingDisplay);
//    this->stackedWidget_content->addWidget(this->settingTrackOption);
//    this->stackedWidget_content->addWidget(this->settingCountry);
//    this->stackedWidget_content->addWidget(this->settingRoseTube);
//    this->stackedWidget_content->addWidget(this->settingVideo);
//    this->stackedWidget_content->addWidget(this->settingVU);
//    this->stackedWidget_content->addWidget(this->settingInputOutput);
//    this->stackedWidget_content->addWidget(this->settingRoseMenu);
//    this->stackedWidget_content->addWidget(this->settingAppMenu);
//    this->stackedWidget_content->addWidget(this->settingAppInfo);
//    this->stackedWidget_content->addWidget(this->viewAll_playListMusic);
//    this->stackedWidget_content->addWidget(this->viewAll_playListRoseTube);
//    this->stackedWidget_content->addWidget(this->albumDetail);
//    this->stackedWidget_content->addWidget(this->musice_playlist);
//    this->stackedWidget_content->addWidget(this->rosetube_playlist);
//    this->stackedWidget_content->addWidget(this->metaInfo);
//    this->stackedWidget_content->addWidget(this->artistInfo);


    // Min Max 사이즈
    //settingMyPage->setMaximumWidth(PAGE_MAX_WIDTH);
    settingMyPage->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingFriendPage->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingFriendPage->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingLib->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingLib->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingDisplay->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingDisplay->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingTrackOption->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingTrackOption->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingCountry->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingCountry->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingRoseTube->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingRoseTube->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingVideo->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingVideo->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingVU->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingVU->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingInputOutput->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingInputOutput->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingRoseMenu->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingRoseMenu->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingAppMenu->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingAppMenu->setMinimumWidth(PAGE_MIN_WIDTH);
//    settingAppInfo->setMaximumWidth(PAGE_MAX_WIDTH);
//    settingAppInfo->setMinimumWidth(PAGE_MIN_WIDTH);

    // 커넥션
    connect(this->settingMyPage, SIGNAL(signal_clickedViewAll_music(QString)), this, SLOT(slot_clickedViewAll(QString)));
    connect(this->settingMyPage, SIGNAL(signal_clickedViewAll_rosetube(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
    connect(this->settingMyPage, SIGNAL(signal_changeMyInfoData()), this, SIGNAL(signal_changeMyInfo()));
//    connect(this->settingFriendPage, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
//    connect(this->settingLib, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)) );
//    connect(this->albumDetail, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));

    if(global.enable_section_left == true){
        global.enable_section_left = false;
    }

    // default : 첫번째 서브메뉴
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_S_MYPAGE;
    this->goToMoveNewOrderPage(tmp_data);
}

/**
 * @brief SettingMain::changedPageVU VU 설정 페이지로 이동하라
 * @note 리모콘위젯의 VU버튼 클릭시 외부에서 호출하는 함수이다.
 */
void SettingMain::changedPageVU(){
    this->leftMenuBar->setSelectedSubMenu(PAGECODE_S_VU);

    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_S_VU;
    this->goToMoveNewOrderPage(tmp_data);
}
/**
 * @brief SettingMain::changedPageInputOutput 입출력 설정 페이지로 이동하라
 * @note 리모콘위젯의 입출력버튼 클릭시 외부에서 호출하는 함수이다.
 */
void SettingMain::changedPageInputOutput(){
    this->leftMenuBar->setSelectedSubMenu(PAGECODE_S_INPUTOUTPUT);

    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_S_INPUTOUTPUT;
    this->goToMoveNewOrderPage(tmp_data);
}
/**
 * @brief SettingMain::changedPageInputOutput 입출력 설정 페이지로 이동하라
 * @note 리모콘위젯의 입출력버튼 클릭시 외부에서 호출하는 함수이다.
 */
void SettingMain::changedPageHDMI(){//cheon29_src
    //print_debug(); //cheon29_src
    this->leftMenuBar->setSelectedSubMenu(PAGECODE_S_HDMI);

    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_S_HDMI;
    this->goToMoveNewOrderPage(tmp_data);
    //print_debug(); //cheon29_src
}

/**
 * @brief SettingMain::changedPageInputOutput 입출력 설정 페이지로 이동하라
 * @note 리모콘위젯의 입출력버튼 클릭시 외부에서 호출하는 함수이다.
 */
void SettingMain::changedPageTimer(){//cheon01_src
    //print_debug(); //cheon01_src
    this->leftMenuBar->setSelectedSubMenu(PAGECODE_S_TIMER);

    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = PAGECODE_S_TIMER;
    this->goToMoveNewOrderPage(tmp_data);
    //print_debug(); //cheon01_src
}

/**
 * @brief SettingMain::slot_clickedViewAll [SLOT] View All 클릭시
 * @param p_jsonData
 */
void SettingMain::slot_clickedViewAll(QString p_viewAllMode){

    // 페이지 히스토리 관리 관련된 Abs함수
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = p_viewAllMode;
    this->goToMoveNewOrderPage(tmp_data);
}
/**
 * @brief SettingMain::slot_clickedViewAll [SLOT] View All 클릭시
 * @param p_viewAllMode
 */
void SettingMain::slot_clickedViewAll(const QJsonObject &p_jsonData){

    // 페이지 히스토리 관리 관련된 Abs함수
    QJsonObject tmp_data;
    tmp_data[KEY_PAGE_CODE] = p_jsonData[KEY_VIEWALL_MODE].toString();
    tmp_data[KEY_DATA] = p_jsonData;
    this->goToMoveNewOrderPage(tmp_data);
}

/**
 * @brief SettingMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
 * @param p_pageCode
 */
void SettingMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

    QJsonObject p_jsonObject_titleMain;
    p_jsonObject_titleMain["name"] = tr("Setting");
    // p_jsonObject_titleMain["name"] = "설정";
    p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Setting;

    this->topMenuBar->setVisible(true);
    this->scrollArea_leftMenuBar->setVisible(true);

    // 부모 virtual 함수 호출
    AbstractMainContent::movePageOnly(p_pageCode, p_data);

    // 자식 override 함수 서술
    this->leftMenuBar->setSelectedSubMenu(p_pageCode);
    this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

    if(p_pageCode==PAGECODE_S_SUB_MEDIALIB){
        // -----------------------------------------
        //  미디어 라이브러리 Show
        // -----------------------------------------
        settingMediaLibAddDel->setUIControl_USB();
        settingMediaLibAddDel->show();
        wg_settingTotal->hide();

    }else{
        // -----------------------------------------
        //  미디어 라이브러리 Hide
        // -----------------------------------------
        settingMediaLibAddDel->hide();
        wg_settingTotal->show();

        if(p_pageCode==PAGECODE_ETC_PLAYFULLSCREEN){

            this->scrollArea_leftMenuBar->setVisible(false);    // 사이드메뉴 hide

        }else if(p_pageCode==PAGECODE_S_MYPAGE){

            if(settingMyPage==nullptr){
                this->settingMyPage = new SettingMyPage();
                this->stackedWidget_content->addWidget(this->settingMyPage);
                settingMyPage->setMinimumWidth(PAGE_MIN_WIDTH);
                connect(this->settingMyPage, SIGNAL(signal_clickedViewAll_music(QString)), this, SLOT(slot_clickedViewAll(QString)));
                connect(this->settingMyPage, SIGNAL(signal_clickedViewAll_rosetube(QJsonObject)), this, SLOT(slot_clickedViewAll(QJsonObject)));
                connect(this->settingMyPage, SIGNAL(signal_changeMyInfoData()), this, SIGNAL(signal_changeMyInfo()));
            }

            this->settingMyPage->setDataMyPage();
            stackedWidget_content->setCurrentWidget(this->settingMyPage);

        }else if(p_pageCode==PAGECODE_S_FRIEND){

            if(settingFriendPage==nullptr){
                settingFriendPage = new SettingFriendPage();
                this->stackedWidget_content->addWidget(this->settingFriendPage);
                settingFriendPage->setMaximumWidth(PAGE_MAX_WIDTH);
                settingFriendPage->setMinimumWidth(PAGE_MIN_WIDTH);
                connect(this->settingFriendPage, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
            }
            this->settingFriendPage->setDataABSPage(p_data);
            stackedWidget_content->setCurrentWidget(this->settingFriendPage);

        }else if(p_pageCode==PAGECODE_S_MEDIALIB){

            if(settingLib==nullptr){
                settingLib = new SettingLib();
                this->stackedWidget_content->addWidget(this->settingLib);
                settingLib->setMaximumWidth(PAGE_MAX_WIDTH);
                settingLib->setMinimumWidth(PAGE_MIN_WIDTH);
                connect(this->settingLib, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)) );
            }
            stackedWidget_content->setCurrentWidget(this->settingLib);
            settingLib->requestMediaLibrary();

        }else if(p_pageCode==PAGECODE_S_DISPLAY){

            if(settingDisplay==nullptr){
                settingDisplay = new SettingDisplay();
                this->stackedWidget_content->addWidget(this->settingDisplay);
                settingDisplay->setMaximumWidth(PAGE_MAX_WIDTH);
                settingDisplay->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            this->settingDisplay->requestGetValueDisplay();
            stackedWidget_content->setCurrentWidget(this->settingDisplay);

        }else if(p_pageCode==PAGECODE_S_TRACK){
            if(settingTrackOption==nullptr){
                settingTrackOption = new SettingTrackOption();
                this->stackedWidget_content->addWidget(this->settingTrackOption);
                settingTrackOption->setMaximumWidth(PAGE_MAX_WIDTH);
                settingTrackOption->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            this->settingTrackOption->getSettingInfo();
            stackedWidget_content->setCurrentWidget(this->settingTrackOption);

        }else if(p_pageCode==PAGECODE_S_COUNTRY){
            if(settingCountry==nullptr){
                settingCountry = new SettingCountry();
                this->stackedWidget_content->addWidget(this->settingCountry);
                settingCountry->setMaximumWidth(PAGE_MAX_WIDTH);
                settingCountry->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            this->settingCountry->getSettingInfo();
            stackedWidget_content->setCurrentWidget(this->settingCountry);

        }else if(p_pageCode==PAGECODE_S_ROSETUBE){
            if(settingRoseTube==nullptr){
                settingRoseTube = new SettingRoseTube();
                this->stackedWidget_content->addWidget(this->settingRoseTube);
                settingRoseTube->setMaximumWidth(PAGE_MAX_WIDTH);
                settingRoseTube->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            stackedWidget_content->setCurrentWidget(this->settingRoseTube);

        }else if(p_pageCode==PAGECODE_S_VIDEO){
            if(settingVideo==nullptr){
                settingVideo = new SettingVideo();
                this->stackedWidget_content->addWidget(this->settingVideo);
                settingVideo->setMaximumWidth(PAGE_MAX_WIDTH);
                settingVideo->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            this->settingVideo->requestVideoSetData();
            stackedWidget_content->setCurrentWidget(this->settingVideo);

        }else if(p_pageCode==PAGECODE_S_HDMI){  //cheon29_src
            //print_debug();//cheon29_src
            if(settingHDMI==nullptr){//cheon29_src
                settingHDMI = new SettingHDMI();//cheon29_src
                this->stackedWidget_content->addWidget(this->settingHDMI);//cheon29_src
                settingHDMI->setMaximumWidth(PAGE_MAX_WIDTH);//cheon29_src
                settingHDMI->setMinimumWidth(PAGE_MIN_WIDTH);//cheon29_src
            }//cheon29_src
            this->settingHDMI->requestGetValueHDMI();//cheon29_src
            stackedWidget_content->setCurrentWidget(this->settingHDMI);//cheon29_src

        }else if(p_pageCode==PAGECODE_S_TIMER){  //cheon01_src
            //print_debug();//cheon29_src
            if(settingTimer==nullptr){//cheon01_src
                //print_debug();//cheon01_src
                settingTimer = new SettingTimer();//cheon01_src
                this->stackedWidget_content->addWidget(this->settingTimer);//cheon01_src
                settingTimer->setMaximumWidth(PAGE_MAX_WIDTH);//cheon01_src
                settingTimer->setMinimumWidth(PAGE_MIN_WIDTH);//cheon01_src
            }//cheon29_src
            this->settingTimer->requestGetValueTimer();//cheon01_src
            stackedWidget_content->setCurrentWidget(this->settingTimer);//cheon29_src

        }else if(p_pageCode==PAGECODE_OP_PLAYLISTINFO){
            int tmp_type = OptionPopup::TypeMenu::Music_Home_MyPlayList;
            if(p_data.contains(KEY_OP_TYPE)){
                 tmp_type = p_data[KEY_OP_TYPE].toInt();
            }

            this->scrollArea_leftMenuBar->setVisible(false);
            if(tmp_type==OptionPopup::TypeMenu::Music_Home_MyPlayList){
                if(this->musice_playlist==nullptr){
                    this->musice_playlist = new Musice_playlist();
                    this->stackedWidget_content->addWidget(this->musice_playlist);
                }

                this->musice_playlist->setPlayListData(p_data);
                stackedWidget_content->setCurrentWidget(this->musice_playlist);              // OP : 플레이리스트상세페이지 (음악)

            }else if(tmp_type==OptionPopup::TypeMenu::RoseTube_Home_PlayList_My){
                if(rosetube_playlist==nullptr){
                    this->rosetube_playlist = new rosetube::RoseTube_PlayList();
                    this->stackedWidget_content->addWidget(this->rosetube_playlist);
                }

                QJsonObject jsonPlayListInfo;
                if(p_data.contains("playList_info")){
                    jsonPlayListInfo = p_data["playList_info"].toObject();
                }
                //this->rosetube_playlist->setPlayListData(jsonPlayListInfo);
                stackedWidget_content->setCurrentWidget(this->rosetube_playlist);              // OP : 플레이리스트상세페이지 (로즈튜브)
            }

        }/*else if(p_pageCode==PAGECODE_OP_PLAYLISTINFO){
            //print_debug(); //cheon06-mypage
                       AbstractMainContent::movePageOnly(p_pageCode, p_data);
               //print_debug(); //cheon06-mypage
                       if(this->musice_playlist==nullptr){
                           this->musice_playlist = new Musice_playlist();
                           this->stackedWidget_content->addWidget(musice_playlist);
                           connect(this->musice_playlist, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
                       }

                       p_jsonObject_titleMain = QJsonObject();
                       this->topMenuBar->setSelectedSubMenu(p_pageCode);
                       this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
                       musice_playlist->setPlayListData(p_data);
                       stackedWidget_content->setCurrentWidget(musice_playlist);
                       //print_debug(); //cheon06-mypage

        }*/else if(p_pageCode==PAGECODE_S_LANGUAGE){  //cheon07_language
            //print_debug();//cheon29_src
            if(settingLanguage==nullptr){//cheon01_src
                //print_debug();//cheon01_src
                settingLanguage = new SettingLanguage();//cheon01_src
                this->stackedWidget_content->addWidget(this->settingLanguage);//cheon01_src
                settingLanguage->setMaximumWidth(PAGE_MAX_WIDTH);//cheon01_src
                settingLanguage->setMinimumWidth(PAGE_MIN_WIDTH);//cheon01_src
            }//cheon29_src
       //     this->settingLanguage->requestGetValueLanguage();//cheon01_src
            stackedWidget_content->setCurrentWidget(this->settingLanguage);//cheon29_src

        }else if(p_pageCode==PAGECODE_S_VU){
            if(settingVU==nullptr){
                settingVU = new SettingVU();
                this->stackedWidget_content->addWidget(this->settingVU);
                settingVU->setMaximumWidth(PAGE_MAX_WIDTH);
                settingVU->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            this->settingVU->requestVUData();
            stackedWidget_content->setCurrentWidget(this->settingVU);

        }else if(p_pageCode==PAGECODE_S_INPUTOUTPUT){
            QString deviceType = global.device.getDeviceType();
            //qDebug() << "settingmain-deviceType : " << deviceType;
            if( deviceType == "RS150" || deviceType == "RS150B" || deviceType == "RS201" || deviceType == "RS250" || deviceType == "RS250A" || deviceType == "RS520" ){
               // if(settingInputOutput==nullptr){
                    settingInputOutput = new SettingInputOutput();
                    this->stackedWidget_content->addWidget(this->settingInputOutput);
                    settingInputOutput->setMaximumWidth(PAGE_MAX_WIDTH);
                    settingInputOutput->setMinimumWidth(PAGE_MIN_WIDTH);
               // }
                this->settingInputOutput->requestInputOutputMode();
                stackedWidget_content->setCurrentWidget(this->settingInputOutput);
                //qDebug() << "=================================================0000000";
            }else{
                ToastMsg::show(this, "", tr("Currently, only ROSE201, ROSE150, ROSE150B, ROSE250 are in service for setting function."), 4000);//c220921_1
                //qDebug() << "=================================================0000001";
            }

        }else if(p_pageCode==PAGECODE_S_ROSEMENUEDIT){
            if(settingRoseMenu==nullptr){
                settingRoseMenu = new SettingRoseMenu();
                this->stackedWidget_content->addWidget(this->settingRoseMenu);
                settingRoseMenu->setMaximumWidth(PAGE_MAX_WIDTH);
                settingRoseMenu->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            stackedWidget_content->setCurrentWidget(this->settingRoseMenu);

        }else if(p_pageCode==PAGECODE_S_APPMENUEDIT){
            if(settingAppMenu==nullptr){
                settingAppMenu = new SettingAppMenu();
                this->stackedWidget_content->addWidget(this->settingAppMenu);
                settingAppMenu->setMaximumWidth(PAGE_MAX_WIDTH);
                settingAppMenu->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            stackedWidget_content->setCurrentWidget(this->settingAppMenu);

        }else if(p_pageCode==PAGECODE_S_APPINFO){

            if(settingAppInfo==nullptr){
                settingAppInfo = new SettingAppInfo();
                this->stackedWidget_content->addWidget(this->settingAppInfo);
                settingAppInfo->setMaximumWidth(PAGE_MAX_WIDTH);
                settingAppInfo->setMinimumWidth(PAGE_MIN_WIDTH);
            }
            stackedWidget_content->setCurrentWidget(this->settingAppInfo);

        }else if(p_pageCode==PAGECODE_VA_MYPLAYLIST){

            if(viewAll_playListMusic==nullptr){
                viewAll_playListMusic = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_MYPLAYLIST);
                this->stackedWidget_content->addWidget(this->viewAll_playListMusic);
            }
            this->scrollArea_leftMenuBar->setVisible(false);    // 사이드메뉴 hide
            this->viewAll_playListMusic->setDataFromDB();
            stackedWidget_content->setCurrentWidget(this->viewAll_playListMusic);       // 음악 플레이리스트 View All

        }else if(p_pageCode==PAGECODE_VA_HOME){

            if(viewAll_playListRoseTube==nullptr){
                viewAll_playListRoseTube = new RoseTube_ViewAll();
                this->stackedWidget_content->addWidget(this->viewAll_playListRoseTube);
            }
            this->scrollArea_leftMenuBar->setVisible(false);    // 사이드메뉴 hide
            if(p_data.contains(KEY_DATA)){
                QJsonObject jsonData = p_data[KEY_DATA].toObject();
                this->viewAll_playListRoseTube->setData(jsonData);
            }
            stackedWidget_content->setCurrentWidget(this->viewAll_playListRoseTube);    // 로즈튜브 플레이리스트 View All

        }else if(p_pageCode==PAGECODE_OP_ALBUMINFO){

            if(albumDetail==nullptr){
                this->albumDetail = new AlbumDetail();
                this->stackedWidget_content->addWidget(this->albumDetail);
                connect(this->albumDetail, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
            }
            this->scrollArea_leftMenuBar->setVisible(false);
            this->albumDetail->setDataABSPage(p_data);
            stackedWidget_content->setCurrentWidget(this->albumDetail);                 // OP : 앨범상세페이지

        }else if(p_pageCode==PAGECODE_OP_ARTISTINFO){

            if(artistInfo==nullptr){
                artistInfo = new ArtistInfo();
                this->stackedWidget_content->addWidget(this->artistInfo);
            }

            this->scrollArea_leftMenuBar->setVisible(false);
            this->artistInfo->setDataArtistInfo(p_data);
            stackedWidget_content->setCurrentWidget(this->artistInfo);                  // OP : 아티스트상세페이지


        }else if(p_pageCode==PAGECODE_OP_FILEINFO){

            // Nothing..

        }else if(p_pageCode==PAGECODE_OP_METAINFO){

            if(metaInfo==nullptr){
                metaInfo = new MetaInfo();
                this->stackedWidget_content->addWidget(this->metaInfo);
            }

            this->scrollArea_leftMenuBar->setVisible(false);
            this->metaInfo->setData(p_data);
            stackedWidget_content->setCurrentWidget(this->metaInfo);                    // OP : 메타정보페이지
        }
    }
}

/**
 * @brief SettingMain::showPlayScreenBG : AbstractmainContent override
 * @note 설정 메뉴는 다른 메뉴와 UI 구조가 다름@\n
 * 전체재생 화면 변경시 background color 변경 필요
 * @see SettingMain::resetLayoutUIfromParent();
 */
void SettingMain::showPlayScreenBG(){
    AbstractMainContent::showPlayScreenBG();
    wg_settingTotal->setStyleSheet("background-color:transparent;");
}

/**
 * @brief SettingMain::hidePlayScreenBG : AbstractmainContent override
 * @note 설정 메뉴는 다른 메뉴와 UI 구조가 다름@\n
 * * 전체재생 화면 변경시 background color 변경 필요
 * @see SettingMain::resetLayoutUIfromParent();
 */
void SettingMain::hidePlayScreenBG(){
    AbstractMainContent::hidePlayScreenBG();
    wg_settingTotal->setStyleSheet("background-color:#212121;");
}


