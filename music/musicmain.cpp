#include "music/musicmain.h"

#include "common/common_struct.h"
#include "common/global.h"
#include <QDebug>

#include <QApplication>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <qapplication.h>
#include <widget/toastmsg.h>

#include <common/gscommon.h>


namespace music {

    /**
     * @brief MusicMain::MusicMain 음악
     * @details 음악 메뉴(홈,분류,앨범,폴더,My collection) 관리
     * @param parent QWidget
     */
    MusicMain::MusicMain(QWidget *parent) : AbstractMainContent(parent)
    {

        this->setInit();
        this->setUIControl();
    }


    MusicMain::~MusicMain(){

        this->deleteLater();
    }

    void MusicMain::slot_overrideSigalSearch(bool b){//c220728
        print_debug();
        if(b){
            emit linker->signal_clickedMovePageRoseTubeSearch();
        }else{
            slot_dragEnterEvent_hide_show(false);
            emit linker->signal_checkQueue(30, "");//c220729
        }

    }

    void MusicMain::slot_dragEnterEvent_hide_show(bool show){//c220826_1
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


    void MusicMain::slot_dragEnterEvent_restore(){//c220826_1
        print_debug();
        this->curr_widget = this->stackedWidget_content->currentWidget();//c220824_4
        this->le_search_back->clear();
        emit linker->signal_searchBarFocusChanged(false);
        this->le_search_back->clearFocus();
        this->curr_widget->show();
        this->le_search_back->hide();
    }

    /**
     * @brief MusicMain::setInit : 초기 세팅
     */
    void MusicMain::setInit(){
        currMainMenuCode = GSCommon::MainMenuCode::Music;
        print_debug();
        connect(linker, SIGNAL(signal_search(QString)), SLOT(slot_search(QString)));
        connect(linker, SIGNAL(signal_goFolder(QString)), SLOT(slot_goFolder(QString)));//c220909_1
        connect(linker, SIGNAL(signal_menumoveMusic()), SLOT(slot_totalsearch()));//cheon211008

        connect(linker, SIGNAL(signal_dragEnterEvent_hide_show(bool)), SLOT(slot_dragEnterEvent_hide_show(bool)));//c220730
        connect(linker, SIGNAL(signal_dropEvent_hide_show(bool)), SLOT(slot_dropEvent_hide_show(bool)));//c220730

        make_CustomLineEdit();//c220730
    }


    /**
     * @brief MusicMain::setUIControl : UI 세팅
     */
    void MusicMain::setUIControl(){

        // 메뉴 페이지
        //this->musicHome = new MusicHome();
        this->musicHome = new MusicHome(this->topMenuBar);
        // 스택에 추가
        this->stackedWidget_content->addWidget(this->musicHome);
        this->stackedWidget_content->addWidget(le_search_back);//c220730

        // 커넥트
        connect(this->musicHome, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));

        // default : 첫번째 서브메뉴
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_M_HOME;
        this->goToMoveNewOrderPage(tmp_data);
    }

    /**
     * @brief MusicMain::slot_search : [슬롯] 검색 요청
     * @param p_text
     */
    void MusicMain::slot_search(const QString &search_word){

        if(search_word.size() >= 2){
            if(search_word.size() >= 2){
                global.search_text = search_word;//searchtextcopy

                common::SearchData data_search;
                data_search.search_word = search_word;

                QJsonObject jsonObj_search;
                jsonObj_search[KEY_DATA] = common::ConvertData_common::getObjectJson_searchData(data_search);
                jsonObj_search[KEY_PAGE_CODE] = PAGECODE_M_SEARCH;

                this->goToMoveNewOrderPage(jsonObj_search);
            }
        }else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
        }
    }
    void MusicMain::slot_totalsearch(){//cheon211008
        print_debug();
        if(global.search_text.size() >= 2){
            QJsonObject tmp_data;
            tmp_data[KEY_PAGE_CODE] = PAGECODE_M_SEARCH;
            tmp_data[KEY_DATA] = global.search_text;
            this->goToMoveNewOrderPage(tmp_data);
        }else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            //ToastMsg::show(this, "", tr("검색어는 2글자 이상 입력해주세요."));
        }
    }
    /**
     * @brief MusicMain::slot_clickedViewAllForSearch [SLOT] View All 클릭시
     * @param p_searchText
     * @param p_viewAllMode
     * @note 검색화면에서 클릭시
     */
    void MusicMain::slot_clickedViewAllForSearch(const QString &p_searchText, const QString &p_viewAllMode){

        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_viewAllMode;
        tmp_data[KEY_DATA] = p_searchText;
        this->goToMoveNewOrderPage(tmp_data);
    }

    /**
     * @brief MusicMain::slot_clickedViewAll [SLOT] View All 클릭시
     * @param p_jsonData
     */
    void MusicMain::slot_clickedViewAll(QString p_viewAllMode){

        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_viewAllMode;
        this->goToMoveNewOrderPage(tmp_data);
    }

    void MusicMain::slot_clickedViewAll_Recommand(QString p_viewAllMode, QJsonObject jsonObj){
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = p_viewAllMode;
        tmp_data.insert("data", jsonObj);
        this->goToMoveNewOrderPage(tmp_data);
    }

    /**
     * @brief MusicMain::slot_clickedFavoriteFolderEdit [SLOT] 음악>폴더 의 즐겨찾기 편집 화면 클릭시
     */
    void MusicMain::slot_clickedFavoriteFolderEdit(const QJsonArray &QJsonArray){

        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = PAGECODE_ETC_EDIT_FAVORITE;
        tmp_data.insert(KEY_DATA, QJsonArray);

        this->goToMoveNewOrderPage(tmp_data);
    }

    void MusicMain::slot_goFolder(QString str){//c220909_1

        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = str;
        //tmp_data.insert(KEY_DATA, QJsonArray);

        this->goToMoveNewOrderPage(tmp_data);
    }

    /**
     * @brief MusicMain::movePageOnly [override] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
     * @param p_pageCode
     */
    void MusicMain::movePageOnly(QString p_pageCode, const QJsonObject &p_data){

        if(this->musicFolder!=nullptr){
            this->musicFolder->hide_topBtnFavorite();
        }

        QJsonObject p_jsonObject_titleMain;
        p_jsonObject_titleMain["name"] = tr("Music");
        p_jsonObject_titleMain[KEY_MAIN_CODE] = GSCommon::MainMenuCode::Music;

        QJsonArray *p_jsonArray_titlSub = new QJsonArray();
        QJsonObject sub1 { {"name", tr("Home")}, {"code", PAGECODE_M_HOME} };
        QJsonObject sub2 { {"name", tr("Recommendation")}, {"code", PAGECODE_M_RECOMMEND} };
        QJsonObject sub3 { {"name", tr("Album")}, {"code", PAGECODE_M_ALBUM} };
        QJsonObject sub4 { {"name", tr("Classification")}, {"code", PAGECODE_M_CATEGORY} };
        QJsonObject sub5 { {"name", tr("Folder")}, {"code", PAGECODE_M_FOLDER} };
        QJsonObject sub6 { {"name", tr("My collection")}, {"code", PAGECODE_M_COLLECTION} };

        p_jsonArray_titlSub->push_back(sub1);
        p_jsonArray_titlSub->push_back(sub2);
        p_jsonArray_titlSub->push_back(sub3);
        p_jsonArray_titlSub->push_back(sub4);
        p_jsonArray_titlSub->push_back(sub5);
        p_jsonArray_titlSub->push_back(sub6);

        this->topMenuBar->setVisible(true);

        // 부모 virtual 함수 호출
        AbstractMainContent::movePageOnly(p_pageCode, p_data);
        musicHome->hide_topLabelControl();

        // 자식 override 함수 서술
        if(p_pageCode==PAGECODE_M_HOME){

            if(this->musicHome==nullptr){
                this->musicHome = new MusicHome(this->topMenuBar);
                this->stackedWidget_content->addWidget(musicHome);
                connect(this->musicHome, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));
                connect(this->musicHome, SIGNAL(signal_clickedViewAll_slot_clickedViewAll_Recommended(QString, QJsonObject)), this, SLOT(slot_clickedViewAll_Recommand(QString, QJsonObject)));
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);

            this->musicHome->setJsonObject_forData(p_data);
            this->musicHome->setActivePage();

            stackedWidget_content->setCurrentWidget(this->musicHome);
            if(global.device.getIsDbScanning() == false){
                this->musicHome->show_topLabelControl();
            }

        }else if(p_pageCode==PAGECODE_M_RECOMMEND){

            if(this->musicRecommend==nullptr){
                this->musicRecommend = new MusicRecommend(this);
                this->stackedWidget_content->addWidget(musicRecommend);
                connect(this->musicRecommend, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            this->musicRecommend->setJsonObject_forData(p_data);
            this->musicRecommend->setActivePage();

            stackedWidget_content->setCurrentWidget(this->musicRecommend);

        }else if(p_pageCode==PAGECODE_M_ALBUM){

            if(this->musicAlbum==nullptr){
                this->musicAlbum = new MusicAlbum(this);
                this->stackedWidget_content->addWidget(musicAlbum);
                connect(this->musicAlbum, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));

                /*if(ProcJsonEasy::getString(p_data, "pageCode2") != "albumInfo"){
                    musicAlbum->setDataFromDB();
                }*/
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            this->musicAlbum->setJsonObject_forData(p_data);
            this->musicAlbum->setActivePage();

            stackedWidget_content->setCurrentWidget(musicAlbum);

        }else if(p_pageCode==PAGECODE_M_CATEGORY){

            if(this->musicCategory==nullptr){
                this->musicCategory = new MusicCategory(this);
                this->stackedWidget_content->addWidget(musicCategory);
                connect(this->musicCategory, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            this->musicCategory->setJsonObject_forData(p_data);
            this->musicCategory->setActivePage();

            //musicCategory->setDataABSPage(p_data);
            stackedWidget_content->setCurrentWidget(musicCategory);

        }else if(p_pageCode==PAGECODE_M_FOLDER){

            if(this->musicFolder==nullptr){
                this->musicFolder = new MusicFolder(this->topMenuBar);
                //musicFolder->hide_topBtnFavorite();
                this->stackedWidget_content->addWidget(musicFolder);
                connect(musicFolder, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
                connect(musicFolder, SIGNAL(signal_clickedFavoriteFolderEdit(QJsonArray)), this, SLOT(slot_clickedFavoriteFolderEdit(QJsonArray) ));
                connect(this->topMenuBar, SIGNAL(signal_clickedSameSubMenu(QString)), this->musicFolder, SLOT(slot_goToInitiPageFolder(QString)));
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            musicFolder->setDataABSPage(p_data);
            stackedWidget_content->setCurrentWidget(musicFolder);

        }else if(p_pageCode==PAGECODE_M_COLLECTION){

            /*if(this->musicCollection==nullptr){
                this->musicCollection = new MusicCollection();
                this->stackedWidget_content->addWidget(musicCollection);
                connect(musicCollection, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            musicCollection->requestFlagLoginInfo();
            stackedWidget_content->setCurrentWidget(musicCollection);*/

            if(this->musicMyCollection==nullptr){
                this->musicMyCollection = new MusicMyCollection(this);
                this->stackedWidget_content->addWidget(this->musicMyCollection);
                connect(musicMyCollection, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));
            }

            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, p_jsonArray_titlSub);
            this->musicMyCollection->setJsonObject_forData(p_data);
            this->musicMyCollection->setActivePage();
            stackedWidget_content->setCurrentWidget(this->musicMyCollection);

        }else if(p_pageCode==PAGECODE_VA_PLAYALBUM){

            if(this->viewAll_playAlbum==nullptr){
                this->viewAll_playAlbum = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_PLAYALBUM);
                this->stackedWidget_content->addWidget(viewAll_playAlbum);
            }

            p_jsonObject_titleMain["name"] = tr("Recently played album");
            //p_jsonObject_titleMain["name"] = tr("최근 재생 앨범");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            viewAll_playAlbum->setDataFromDB();
            stackedWidget_content->setCurrentWidget(viewAll_playAlbum);

        }else if(p_pageCode==PAGECODE_VA_PLAYPLAYLIST){

            if(this->viewAll_playPlaylist==nullptr){
                this->viewAll_playPlaylist = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_PLAYPLAYLIST);
                this->stackedWidget_content->addWidget(viewAll_playPlaylist);
            }

            p_jsonObject_titleMain["name"] = tr("Recently played playlist");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            viewAll_playPlaylist->setDataFromDB();
            stackedWidget_content->setCurrentWidget(viewAll_playPlaylist);

        }else if(p_pageCode==PAGECODE_VA_ADDEDALBUM){

            if(this->viewAll_addedAlbum==nullptr){
                this->viewAll_addedAlbum = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_ADDEDALBUM);
                this->stackedWidget_content->addWidget(viewAll_addedAlbum);
            }

            p_jsonObject_titleMain["name"] = tr("Recently added albums");
            //p_jsonObject_titleMain["name"] = tr("최근 추가 된 앨범");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

            if(ProcJsonEasy::getString(p_data, "pageCode2") != "albumInfo"){
                viewAll_addedAlbum->setDataFromDB();
            }

            stackedWidget_content->setCurrentWidget(viewAll_addedAlbum);

        }else if(p_pageCode == PAGECODE_VA_RECOMMANDARTIST){

            if(this->artistListAll==nullptr){
                this->artistListAll = new ArtistListAll(this);
                this->stackedWidget_content->addWidget(this->artistListAll);
            }

            this->topMenuBar->setVisible(false);

            this->artistListAll->setJsonObject_forData(p_data);
            this->artistListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->artistListAll);

        }else if(p_pageCode == PAGECODE_VA_HISTORY_LIST_VIEW){

            if(this->historyListAll==nullptr){
                this->historyListAll = new HistoryListAll(this);
                this->stackedWidget_content->addWidget(this->historyListAll);
            }

            this->topMenuBar->setVisible(false);

            this->historyListAll->setJsonObject_forData(p_data);
            this->historyListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->historyListAll);

        }else if(p_pageCode == PAGECODE_VA_HISTORY_DETAIL){

            if(this->historyDetail==nullptr){
                this->historyDetail = new HistoryDetail(this);
                this->stackedWidget_content->addWidget(this->historyDetail);
                //connect(this->historyDetail, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));
            }

            this->topMenuBar->setVisible(false);

            this->historyDetail->setJsonObject_forData(p_data);
            this->historyDetail->setActivePage();
            stackedWidget_content->setCurrentWidget(this->historyDetail);

        }else if(p_pageCode==PAGECODE_VA_MYPLAYLIST){

            /*if(this->viewAll_myPlayList==nullptr){
                this->viewAll_myPlayList = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_MYPLAYLIST);
                this->stackedWidget_content->addWidget(viewAll_myPlayList);
            }

            p_jsonObject_titleMain["name"] = tr("My Playlists");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            viewAll_myPlayList->setDataFromDB();
            stackedWidget_content->setCurrentWidget(viewAll_myPlayList);*/

            if(this->myplaylistListAll==nullptr){
                this->myplaylistListAll = new MyPlaylistAll(this);
                this->stackedWidget_content->addWidget(this->myplaylistListAll);
            }

            this->topMenuBar->setVisible(false);

            this->myplaylistListAll->setJsonObject_forData(p_data);
            this->myplaylistListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->myplaylistListAll);

        }else if(p_pageCode==PAGECODE_VA_USERPLAYLIST){

            if(this->userplaylistListAll==nullptr){
                this->userplaylistListAll = new UserPlaylistAll(this);
                this->stackedWidget_content->addWidget(this->userplaylistListAll);
            }

            this->topMenuBar->setVisible(false);

            this->userplaylistListAll->setJsonObject_forData(p_data);
            this->userplaylistListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->userplaylistListAll);

        }else if(p_pageCode==PAGECODE_VA_FRIENDPLAYLIST){

            if(this->viewAll_friendPlayList==nullptr){
                this->viewAll_friendPlayList = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_FRIENDPLAYLIST);
                this->stackedWidget_content->addWidget(viewAll_friendPlayList);
            }

            p_jsonObject_titleMain["name"] = tr("Friend playlist");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            viewAll_friendPlayList->setDataFromDB();
            stackedWidget_content->setCurrentWidget(viewAll_friendPlayList);

        }else if(p_pageCode==PAGECODE_VA_RECOMMENDARTIST){

            if(this->viewAll_recommendArtist==nullptr){
                this->viewAll_recommendArtist = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_RECOMMENDARTIST);
                this->stackedWidget_content->addWidget(viewAll_recommendArtist);
            }

            p_jsonObject_titleMain["name"] = tr("Recommended Artist");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

            if(ProcJsonEasy::getString(p_data, "pageCode2") != "artistInfo"){
                viewAll_recommendArtist->setDataFromDB();
            }

            stackedWidget_content->setCurrentWidget(viewAll_recommendArtist);

        }else if(p_pageCode==PAGECODE_VA_COLLECTION_TRACK){

            if(this->viewAll_collectionTrack==nullptr){
                this->viewAll_collectionTrack = new MusicList_CollectionTrack(this);
                this->stackedWidget_content->addWidget(viewAll_collectionTrack);
            }
            p_jsonObject_titleMain["name"] = tr("Track");
            //p_jsonObject_titleMain["name"] = tr("트랙");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            viewAll_collectionTrack->requestInitData();
            stackedWidget_content->setCurrentWidget(viewAll_collectionTrack);

        }else if(p_pageCode==PAGECODE_VA_SEARCH_TRACK){

            /*if(this->viewAll_searchTrack==nullptr){
                this->viewAll_searchTrack = new MusicList_One(this);
                this->stackedWidget_content->addWidget(viewAll_searchTrack);
            }
            p_jsonObject_titleMain["name"] = p_data[KEY_DATA].toString();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            this->viewAll_searchTrack->setDataFromDB(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(this->viewAll_searchTrack);*/

            if(this->searchTrackListAll==nullptr){
                this->searchTrackListAll = new SearchTrackListAll(this);
                this->stackedWidget_content->addWidget(this->searchTrackListAll);
            }

            this->topMenuBar->setVisible(false);

            this->searchTrackListAll->setJsonObject_forData(p_data);
            this->searchTrackListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->searchTrackListAll);

        }else if(p_pageCode==PAGECODE_VA_SEARCH_ALBUM){

            /*if(this->viewAll_searchAlbum==nullptr){
                this->viewAll_searchAlbum = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_SEARCHALBUM);
                this->stackedWidget_content->addWidget(viewAll_searchAlbum);
            }
            p_jsonObject_titleMain["name"] = p_data[KEY_DATA].toString();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            this->viewAll_searchAlbum->setDataFromDB(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(this->viewAll_searchAlbum);*/

            if(this->searchAlbumListAll==nullptr){
                this->searchAlbumListAll = new SearchAlbumListAll(this);
                this->stackedWidget_content->addWidget(this->searchAlbumListAll);
            }

            this->topMenuBar->setVisible(false);

            this->searchAlbumListAll->setJsonObject_forData(p_data);
            this->searchAlbumListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->searchAlbumListAll);

        }else if(p_pageCode==PAGECODE_VA_SEARCH_ARTIST){

            if(this->viewAll_searchArtist==nullptr){
                this->viewAll_searchArtist = new MusicList_Group(MusicList_Group::GroupMode::VIEWALL_SEARCHARTIST);
                this->stackedWidget_content->addWidget(viewAll_searchArtist);
            }

            p_jsonObject_titleMain["name"] = p_data[KEY_DATA].toString();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            this->viewAll_searchArtist->setDataFromDB(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(this->viewAll_searchArtist);

        }else if(p_pageCode==PAGECODE_VA_ARTIST_TRACKLIST){

            /*if(this->viewAll_artistTrack==nullptr){
                 this->viewAll_artistTrack = new MusicList_ArtistTrack(this);
                 this->stackedWidget_content->addWidget(viewAll_artistTrack);
             }
             QString tmp_artist = ProcJsonEasy::getString(p_data, "artist");

             p_jsonObject_titleMain["name"] = tmp_artist + " - Track";
             this->topMenuBar->setSelectedSubMenu(p_pageCode);
             this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
             this->viewAll_artistTrack->requestInitData(p_data);
             stackedWidget_content->setCurrentWidget(this->viewAll_artistTrack);*/

            if(this->artistTrackListAll==nullptr){
                this->artistTrackListAll = new ArtistTrackListAll(this);
                this->stackedWidget_content->addWidget(this->artistTrackListAll);
            }

            this->topMenuBar->setVisible(false);

            this->artistTrackListAll->setJsonObject_forData(p_data);
            this->artistTrackListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->artistTrackListAll);

        }else if(p_pageCode==PAGECODE_VA_ALBUM_HISTORY_ALL_VIEW){

            if(this->histroyAlbumListAll==nullptr){
                this->histroyAlbumListAll = new AlbumHistoryAll(this);
                this->stackedWidget_content->addWidget(this->histroyAlbumListAll);
            }

            this->topMenuBar->setVisible(false);

            this->histroyAlbumListAll->setJsonObject_forData(p_data);
            this->histroyAlbumListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->histroyAlbumListAll);

        }else if(p_pageCode==PAGECODE_VA_PLAYLIST_HISTORY_ALL_VIEW){

            if(this->histroyPlaylistListAll==nullptr){
                this->histroyPlaylistListAll = new PlaylistHistoryAll(this);
                this->stackedWidget_content->addWidget(this->histroyPlaylistListAll);
            }

            this->topMenuBar->setVisible(false);

            this->histroyPlaylistListAll->setJsonObject_forData(p_data);
            this->histroyPlaylistListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->histroyPlaylistListAll);

        }else if(p_pageCode==PAGECODE_VA_TRACK_HISTORY_ALL_VIEW){

            if(this->histroyTrackListAll==nullptr){
                this->histroyTrackListAll = new TrackHistoryAll(this);
                this->stackedWidget_content->addWidget(this->histroyTrackListAll);
            }

            this->topMenuBar->setVisible(false);

            this->histroyTrackListAll->setJsonObject_forData(p_data);
            this->histroyTrackListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->histroyTrackListAll);

        }else if(p_pageCode==PAGECODE_VA_ARTIST_ALBUMLIST){

            /*if(this->viewAll_artistAlbum==nullptr){
                 this->viewAll_artistAlbum = new MusicList_ArtistAlbum(this);
                 this->stackedWidget_content->addWidget(viewAll_artistAlbum);
             }
             QString tmp_artist = ProcJsonEasy::getString(p_data, "artist");

             p_jsonObject_titleMain["name"] = tmp_artist + " - Album";
             this->topMenuBar->setSelectedSubMenu(p_pageCode);
             this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
             this->viewAll_artistAlbum->requestInitData(p_data);
             stackedWidget_content->setCurrentWidget(this->viewAll_artistAlbum);*/

            if(this->artistAlbumListAll==nullptr){
                this->artistAlbumListAll = new ArtistAlbumListAll(this);
                this->stackedWidget_content->addWidget(this->artistAlbumListAll);
            }

            this->topMenuBar->setVisible(false);

            this->artistAlbumListAll->setJsonObject_forData(p_data);
            this->artistAlbumListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->artistAlbumListAll);

        }else if(p_pageCode==PAGECODE_VA_GENRE_ALBUMLIST){

            if(this->viewAll_genreAlbum==nullptr){
                this->viewAll_genreAlbum = new MusicList_GenreAlbum(this);
                this->stackedWidget_content->addWidget(viewAll_genreAlbum);
            }
            QString tmp_artist = ProcJsonEasy::getString(p_data, "artist");

            p_jsonObject_titleMain["name"] = tmp_artist + " - Album";
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            this->viewAll_genreAlbum->requestInitData(p_data);
            stackedWidget_content->setCurrentWidget(this->viewAll_genreAlbum);

        }else if(p_pageCode==PAGECODE_M_SEARCH){

            /*if(this->musicSearch==nullptr){
                this->musicSearch = new MusicSearch();
                this->stackedWidget_content->addWidget(musicSearch);
                connect(this->musicSearch, SIGNAL(signal_clickedViewAll(QString,QString)), this, SLOT(slot_clickedViewAllForSearch(QString,QString)));
                connect(this->musicSearch, SIGNAL(signal_changedMenuName(QString, QString)), this->topMenuBar, SLOT(slot_changedMenuNameText_search(QString, QString)));//cheon211008
            }

            // -------------------------------------
            //  음악 < 검색결과 화면
            // -------------------------------------
            p_jsonObject_titleMain = QJsonObject();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            musicSearch->search(p_data[KEY_DATA].toString());
            stackedWidget_content->setCurrentWidget(musicSearch);*/

            if(this->musicSearch==nullptr){
                this->musicSearch = new MusicSearch(this);
                this->stackedWidget_content->addWidget(this->musicSearch);
                //connect(this->musicSearch, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));
                //connect(this->musicSearch, SIGNAL(signal_changedMenuName(QString, QString)), this->topMenuBar, SLOT(slot_changedMenuNameText_search(QString, QString)));
            }

            this->topMenuBar->setVisible(false);

            this->musicSearch->setJsonObject_forData(ProcJsonEasy::getJsonObject(p_data, KEY_DATA));
            this->musicSearch->setActivePage();
            stackedWidget_content->setCurrentWidget(this->musicSearch);

        }else if(p_pageCode==PAGECODE_M_FAVORITE){

            if(this->musicFavorite==nullptr){
                this->musicFavorite = new MusicFavorite();
                this->stackedWidget_content->addWidget(this->musicFavorite);
                //connect(this->musicFavorite, SIGNAL(signal_clickedViewAll(QString)), this, SLOT(slot_clickedViewAll(QString)));
            }

            p_jsonObject_titleMain["name"] = tr("My Favorites");
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());

            this->musicFavorite->setJsonObject_forData(p_data);
            this->musicFavorite->setActivePage();
            stackedWidget_content->setCurrentWidget(this->musicFavorite);

        }else if(p_pageCode==PAGECODE_M_ADDPLAYLIST){

            if(this->musicAddPlaylsit==nullptr){
                this->musicAddPlaylsit = new MusicAddPlaylist(this);
                this->stackedWidget_content->addWidget(this->musicAddPlaylsit);
            }

            this->topMenuBar->setVisible(false);

            this->musicAddPlaylsit->setJsonObject_forData(p_data);
            this->musicAddPlaylsit->setActivePage();
            stackedWidget_content->setCurrentWidget(this->musicAddPlaylsit);

        }else if(p_pageCode==PAGECODE_VA_RECENTLY_LIST_DELETE){

            if(this->musicRecenltyDelete==nullptr){
                this->musicRecenltyDelete = new MusicRecentlyListDelete(this);
                this->stackedWidget_content->addWidget(this->musicRecenltyDelete);
            }

            this->topMenuBar->setVisible(false);

            this->musicRecenltyDelete->setJsonObject_forData(ProcJsonEasy::getJsonObject(p_data, "data"));
            this->musicRecenltyDelete->setActivePage();
            stackedWidget_content->setCurrentWidget(this->musicRecenltyDelete);

        }else if(p_pageCode==PAGECODE_OP_ALBUMINFO){

            /*if(this->albumDetail==nullptr){
                this->albumDetail = new AlbumDetail();
                this->stackedWidget_content->addWidget(albumDetail);
                connect(albumDetail, SIGNAL(clickedSubTab(QJsonObject)), this, SLOT(goToMoveNewOrderPage_subStep(QJsonObject)));
                connect(albumDetail, SIGNAL(signal_clickedMovePage(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)));
                connect(this->albumDetail, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            p_jsonObject_titleMain = QJsonObject();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            albumDetail->setDataABSPage(p_data);
            stackedWidget_content->setCurrentWidget(albumDetail);*/

            if(this->albumDetail==nullptr){
                this->albumDetail = new AlbumDetail(this);
                this->stackedWidget_content->addWidget(this->albumDetail);
                //connect(this->albumDetail, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            stackedWidget_content->hide();
            this->topMenuBar->setVisible(false);

            this->albumDetail->setJsonObject_forData(p_data);
            this->albumDetail->setActivePage();
            stackedWidget_content->setCurrentWidget(this->albumDetail);
            stackedWidget_content->show();

        }else if(p_pageCode==PAGECODE_OP_RECENTALBUMINFO){

            if(this->albumDetail_rose==nullptr){
                this->albumDetail_rose = new AlbumDetail_Rose(this);
                this->stackedWidget_content->addWidget(this->albumDetail_rose);
                //connect(this->albumDetail_rose, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            this->topMenuBar->setVisible(false);

            p_jsonObject_titleMain = QJsonObject();
            this->albumDetail_rose->setJsonObject_forData(p_data);
            this->albumDetail_rose->setActivePage();
            stackedWidget_content->setCurrentWidget(this->albumDetail_rose);

        }else if(p_pageCode==PAGECODE_VA_PLAYALBUMALL){

            if(this->albumListAll==nullptr){
                this->albumListAll = new AlbumListAll(this);
                this->stackedWidget_content->addWidget(this->albumListAll);
            }

            this->topMenuBar->setVisible(false);

            p_jsonObject_titleMain = QJsonObject();
            this->albumListAll->setJsonObject_forData(p_data);
            this->albumListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->albumListAll);

        }else if(p_pageCode==PAGECODE_OP_RECENTPLAYLISTINFO){

            if(this->playlistDetail_rose==nullptr){
                this->playlistDetail_rose = new PlaylistDetail_Rose(this);
                this->stackedWidget_content->addWidget(this->playlistDetail_rose);
                connect(this->playlistDetail_rose, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            this->topMenuBar->setVisible(false);

            p_jsonObject_titleMain = QJsonObject();
            this->playlistDetail_rose->setJsonObject_forData(p_data);
            this->playlistDetail_rose->setActivePage();
            stackedWidget_content->setCurrentWidget(this->playlistDetail_rose);

        }else if(p_pageCode==PAGECODE_VA_PLAYPLAYLISTALL){

            if(this->playlistListAll==nullptr){
                this->playlistListAll = new PlaylistAll(this);
                this->stackedWidget_content->addWidget(this->playlistListAll);
            }

            this->topMenuBar->setVisible(false);

            p_jsonObject_titleMain = QJsonObject();
            this->playlistListAll->setJsonObject_forData(p_data);
            this->playlistListAll->setActivePage();
            stackedWidget_content->setCurrentWidget(this->playlistListAll);

        }else if(p_pageCode==PAGECODE_VA_PLAYTRACKALL){

            if(this->trackListAll_rose==nullptr){
                this->trackListAll_rose = new TrackListAll(this);
                this->stackedWidget_content->addWidget(this->trackListAll_rose);
                connect(this->trackListAll_rose, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            this->topMenuBar->setVisible(false);

            p_jsonObject_titleMain = QJsonObject();
            this->trackListAll_rose->setJsonObject_forData(p_data);
            this->trackListAll_rose->setActivePage();
            stackedWidget_content->setCurrentWidget(this->trackListAll_rose);

        }else if(p_pageCode==PAGECODE_OP_PLAYLISTINFO){

            /*if(this->musice_playlist==nullptr){
                this->musice_playlist = new Musice_playlist(this);
                this->stackedWidget_content->addWidget(this->musice_playlist);
                connect(this->musice_playlist, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            p_jsonObject_titleMain = QJsonObject();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            musice_playlist->setPlayListData(p_data);
            stackedWidget_content->setCurrentWidget(musice_playlist);*/

            if(this->playlistDetail_rose==nullptr){
                this->playlistDetail_rose = new PlaylistDetail_Rose(this);
                this->stackedWidget_content->addWidget(this->playlistDetail_rose);
                connect(this->playlistDetail_rose, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            this->topMenuBar->setVisible(false);

            p_jsonObject_titleMain = QJsonObject();
            this->playlistDetail_rose->setJsonObject_forData(p_data);
            this->playlistDetail_rose->setActivePage();
            stackedWidget_content->setCurrentWidget(this->playlistDetail_rose);

        }else if(p_pageCode==PAGECODE_OP_ARTISTINFO){

            /*if(this->artistInfo==nullptr){
                this->artistInfo = new ArtistInfo(this);
                this->stackedWidget_content->addWidget(artistInfo);
                connect(this->artistInfo, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            p_jsonObject_titleMain = QJsonObject();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            artistInfo->setDataArtistInfo(p_data);
            stackedWidget_content->setCurrentWidget(artistInfo);*/

            if(this->artistDetail==nullptr){
                this->artistDetail = new ArtistDetail(this);
                this->stackedWidget_content->addWidget(this->artistDetail);
                //connect(this->artistDetail, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            this->topMenuBar->setVisible(false);

            this->artistDetail->setJsonObject_forData(p_data);
            this->artistDetail->setActivePage();
            stackedWidget_content->setCurrentWidget(this->artistDetail);

        }else if(p_pageCode==PAGECODE_OP_GENREINFO){

            if(this->genreInfo==nullptr){
                this->genreInfo = new GenreInfo(this);
                this->stackedWidget_content->addWidget(genreInfo);
                connect(this->genreInfo, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            p_jsonObject_titleMain = QJsonObject();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            genreInfo->setDataGenreInfo(p_data);
            stackedWidget_content->setCurrentWidget(genreInfo);

        }else if(p_pageCode==PAGECODE_OP_COMPOSERINFO){

            if(this->composerInfo==nullptr){
                this->composerInfo = new ComposerInfo(this);
                this->stackedWidget_content->addWidget(composerInfo);
                connect(this->composerInfo, SIGNAL(signal_changedMenuName(QString)), this->topMenuBar, SLOT(slot_changedMenuNameText(QString)));
            }

            p_jsonObject_titleMain = QJsonObject();
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            composerInfo->setDataComposerInfo(p_data);
            stackedWidget_content->setCurrentWidget(composerInfo);

        }else if(p_pageCode==PAGECODE_OP_FILEINFO){
            // Nothing..
        }else if(p_pageCode==PAGECODE_OP_MULTI){
            // Nothing..
        }else if(p_pageCode==PAGECODE_OP_METAINFO){

            if(this->metaInfo==nullptr){
                this->metaInfo = new MetaInfo(this);
                this->stackedWidget_content->addWidget(metaInfo);
            }

            p_jsonObject_titleMain["name"] = tr("Meta Info");
            //p_jsonObject_titleMain["name"] = "메타정보";
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            metaInfo->setData(p_data);
            stackedWidget_content->setCurrentWidget(metaInfo);

        }else if(p_pageCode==PAGECODE_ETC_EDIT_FAVORITE){

            if(this->musicFavoriteEdit==nullptr){
                this->musicFavoriteEdit = new MusicFolder_EditFavor();
                this->stackedWidget_content->addWidget(musicFavoriteEdit);
            }

            p_jsonObject_titleMain["name"] = tr("Favorite Edit");
            //p_jsonObject_titleMain["name"] = "즐겨찾기 편집";
            this->topMenuBar->setSelectedSubMenu(p_pageCode);
            this->topMenuBar->setDataTopMenuBar(p_jsonObject_titleMain, new QJsonArray());
            stackedWidget_content->setCurrentWidget(musicFavoriteEdit);
            musicFavoriteEdit->setFavorData(p_data[KEY_DATA].toArray());
        }



    }

    void MusicMain::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        print_debug();
        Q_UNUSED(p_jsonObj);
        if(p_id == 3344){//c220727
            print_debug();
            if(ProcJsonEasy::get_flagOk(p_jsonObj)){
                print_debug();
                ToastMsg::show(this,"", tr("Play the selected YouTube content."), 2000, 0);

            }else{
                print_debug();
                ToastMsg::show(this,"", tr("Failed to perform playback of the selected YouTube content."), 2000, 0);

            }
            slot_overrideSigalSearch(false);

        }
        else if(p_id == 5678){//c220803
            print_debug();
            if(ProcJsonEasy::get_flagOk(p_jsonObj)){
                qDebug() << "test";
                print_debug();

                bool p_flagShuffle = false;
                QJsonArray jsonArrYoutube;

                QJsonArray tmp_jsonArrYoutube = p_jsonObj["track"].toArray();
                //QJsonObject tmp_json;
                QJsonObject jsonTrackData;
                jsonTrackData.insert("playurl",global.dragDrop_pathStr);
                qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;
                //---//c220802
                QString videoID;
                QString tmp_Path4 = global.dragDrop_pathStr.split("&").at(0);
                if((global.dragDrop_pathStr.contains("&")&& global.dragDrop_pathStr.split("&").at(1).size()>0)){
                    videoID = tmp_Path4.split("?v=").last();
                }else{
                    videoID = jsonTrackData["playurl"].toString().split("?v=").last();
                }
                //---------------------
                if(tmp_jsonArrYoutube.count()>0){
                    for(int i = 0 ; i < tmp_jsonArrYoutube.count(); i++){

                        jsonTrackData = tmp_jsonArrYoutube.at(i).toObject();
                        QString videoID = jsonTrackData["url"].toString().split("?v=").last();

                        QJsonObject jsonYoutube;
                        jsonYoutube.insert("channelId", p_jsonObj["channelId"].toString());
                        jsonYoutube.insert("channelName", p_jsonObj["uploaderName"].toString());
                        jsonYoutube.insert("duration", QString("%1").arg(p_jsonObj["duration"].toInt()));
                        jsonYoutube.insert("id", videoID);
                        jsonYoutube.insert("isFavorite", p_jsonObj["favorites"].toString()=="1" ? true : false);
                        jsonYoutube.insert("isLiveStream",p_jsonObj["streamType"].toString()=="LIVE_STREAM" ? true : false);
                        jsonYoutube.insert("thumbnailUrl", p_jsonObj["thumbnailUrl"].toString());
                        jsonYoutube.insert("thumbsUpPercentage",0);
                        jsonYoutube.insert("title", p_jsonObj["name"].toString());
                        jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObj["viewCount"].toInt()));

                        // jsonArrYoutube.append(jsonYoutube);
                    }
                }else{
                    qDebug()<<"videoID="<<videoID;
                    QJsonObject jsonYoutube;
                    jsonYoutube.insert("channelId", p_jsonObj["channelId"].toString());
                    jsonYoutube.insert("channelName", p_jsonObj["uploaderName"].toString());
                    jsonYoutube.insert("duration", QString("%1").arg(p_jsonObj["duration"].toInt()));
                    jsonYoutube.insert("id", videoID);
                    jsonYoutube.insert("isFavorite", p_jsonObj["favorites"].toString()=="1" ? true : false);
                    jsonYoutube.insert("isLiveStream",p_jsonObj["streamType"].toString()=="LIVE_STREAM" ? true : false);
                    jsonYoutube.insert("thumbnailUrl", p_jsonObj["thumbnailUrl"].toString());
                    jsonYoutube.insert("thumbsUpPercentage",0);
                    jsonYoutube.insert("title", p_jsonObj["name"].toString());
                    jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObj["viewCount"].toInt()));

                    jsonArrYoutube.append(jsonYoutube);
                }

                QJsonObject jsonParam;
                jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
                jsonParam.insert("shuffle", p_flagShuffle ? 1 : 0);
                jsonParam.insert("youtubePlayType", 15);
                jsonParam.insert("youtube", jsonArrYoutube);

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArrYoutube.count();     // 220419 queue count
                emit linker->signal_checkQueue(32, QString(tr(" Play %1 : %2")).arg(p_jsonObj["uploaderName"].toString()).arg(p_jsonObj["name"].toString().left(20)));//c220802


                NetworkHttp *network = new NetworkHttp(this);
                connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

                network->request(3344, QString("http://%1:%2/youtubePlay.playlist.add")
                                 .arg(global.device.getDeviceIP()).arg(global.port)
                                 , jsonParam, true);
            }else{

                print_debug();
                qDebug() << "ProcJsonEasy::get_flagOk(p_jsonObj) = "<< ProcJsonEasy::get_flagOk(p_jsonObj);

                emit linker->signal_checkQueue(33, "");//c220803

            }


            slot_overrideSigalSearch(false);
        }

        sender()->deleteLater();
    }
}
