#include "bugs/BugsExplore.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/DataCommon_Bugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "tidal/AbstractItem.h"

#include <QDebug>


namespace bugs {

    // subTitle의 사이드 버튼의 btnId 로 사용
    const int BTN_IDX___pdAlbum_popular = 1;
    const int BTN_IDX___pdAlbum_recent = 2;
    const int BTN_IDX___theme = 3;

    const int SECTION_FOR_MORE_POPUP___pd_album_popular = 1;
    const int SECTION_FOR_MORE_POPUP___pd_album_recent = 2;

    const QString BUGS_API_SUBPAHT___pdAlbum = "musicpd/album";

    const char* KEY_INDEX_THEME = "idx";


    /**
     * @brief "TIDAL > 홈" 화면을 위한 생성자.
     * @param parent
     */
    BugsExplore::BugsExplore(QWidget *parent) : AbstractBugsSubWidget(VerticalScroll_filter, parent) {

        // Data
        this->list_pd_album_popular = new QList<bugs::PD_AlbumItemData>();
        this->list_pd_album_recent = new QList<bugs::PD_AlbumItemData>();

        this->flagReg_album_recent = false;
        this->flagReg_album_popular = false;
        this->flagReg_theme = false;
    }



    /**
     * @brief BugsExplore::applyLoginSuccessState
     */
    void BugsExplore::applyLoginSuccessState(){

        if(this->is_bugs_logined()){
            if(!this->m_flagCurrLogined){
                // 로그아웃 ---> 로그인
                this->m_flagCurrLogined = true;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                this->setUIControl_logined();

            }

            //--------- 항상 새로 가져온다. ------------ (좋아요 동기화)

            // 서브메뉴 광클 대비
            if(this->flagReg_album_recent == false){
                this->flagReg_album_recent = true;

                this->list_pd_album_recent->clear();

                GSCommon::clearLayout(this->hBox_pd_album_recent);

                ProcBugsAPI *proc_recent = new ProcBugsAPI(this);
                proc_recent->setProperty("section", SECTION_FOR_MORE_POPUP___pd_album_recent);
                connect(proc_recent, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsExplore::slot_applyResult_pd_albums);
                proc_recent->request_bugs_getList_pd_albums(10, 0, ProcBugsAPI::BugsPDAlbum_Sort::Recent);
            }

            if(this->flagReg_album_popular == false){
                this->flagReg_album_popular = true;

                this->list_pd_album_popular->clear();

                GSCommon::clearLayout(this->hBox_pd_album_popular);

                ProcBugsAPI *proc_popular = new ProcBugsAPI(this);
                proc_popular->setProperty("section", SECTION_FOR_MORE_POPUP___pd_album_popular);
                connect(proc_popular, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsExplore::slot_applyResult_pd_albums);
                proc_popular->request_bugs_getList_pd_albums(10, 0, ProcBugsAPI::BugsPDAlbum_Sort::Popular);
            }


            if(this->flagReg_theme == false){
                this->flagReg_theme = true;

                // theme 관련
                DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
                if(dataCommon_bugs.list_theme.count() == 0){
                    ProcBugsAPI *proc_theme = new ProcBugsAPI(this);
                    connect(proc_theme, &ProcBugsAPI::completeReq_list_theme_setDone, this, &BugsExplore::slot_list_theme_setDone);
                    proc_theme->request_bugs_getListThemes();
                }
                else{
                    this->slot_list_theme_setDone();
                }
            }

        }
        else{
            if(this->m_flagCurrLogined){
                // 로그인 ---> 로그아웃
                this->m_flagCurrLogined = false;

                GSCommon::clearLayout(this->box_contents);      // Layout 초기화
                this->setUIControl_notLogin();
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void BugsExplore::setUIControl_logined(){

        // 인기 뮤직PD앨범
        this->addUIControl_subTitle_withSideBtn_common("인기 뮤직PD앨범", "View All", BTN_IDX___pdAlbum_popular);
        this->hBox_pd_album_popular = this->get_addUIControl_hBoxLayout_forPD_Album();

        // 최신 뮤직PD앨범
        this->addUIControl_subTitle_withSideBtn_common("최신 뮤직PD앨범", "View All", BTN_IDX___pdAlbum_recent);
        this->hBox_pd_album_recent = this->get_addUIControl_hBoxLayout_forPD_Album();

        // 장르/테마
        this->addUIControl_subTitle_withSideBtn_common("장르/테마", "View All", BTN_IDX___theme);
        this->flowLayout_theme = this->get_addUIControl_flowLayout(20, 20);
        this->flowLayout_theme->setSizeConstraint(QLayout::SetMinimumSize);

    }






    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Theme 설정이 완료되었을 때, 처리하는 함수
     */
    void BugsExplore::slot_list_theme_setDone(){

        this->flagReg_theme = false;

        GSCommon::clearLayout(this->flowLayout_theme);
        QString btn_style = this->getStyle_pushButton_optionChoose();

        DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
        int data_length = dataCommon_bugs.list_theme.count();

        for(int i=0 ; i<data_length ; i++){

            QPushButton *btn = new QPushButton();
            btn->setText(dataCommon_bugs.list_theme.at(i).opt_name);
            btn->setStyleSheet(btn_style);
            btn->setProperty(KEY_INDEX_THEME, i);
            btn->setCursor(Qt::PointingHandCursor);
            connect(btn, &QPushButton::clicked, this, &BugsExplore::slot_clicked_themeButton);

            this->flowLayout_theme->addWidget(btn);
        }

    }


    /**
     * @brief BugsExplore::slot_clicked_themeButton
     */
    void BugsExplore::slot_clicked_themeButton(){
        int idx = sender()->property(KEY_INDEX_THEME).toInt();
        DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
        if(idx>=0 && idx<dataCommon_bugs.list_theme.count()){

            bugs::FilterOptionData tmp_data_filterOpt = dataCommon_bugs.list_theme.at(idx);

            QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_filterOptionData(tmp_data_filterOpt);
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___EXPLORE___CHOOSE_THEME);

            emit this->signal_clickedMovePage(jsonObj_move);
        }
    }



    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void BugsExplore::slot_clickBtn_subTitle_side(const int btnId){

        if(btnId == BTN_IDX___pdAlbum_popular){
            // 인기PD앨범 - All View 이동
            PageInfo_PDAlbumAllView data_pageInfo;
            data_pageInfo.mainTitle = "인기 뮤직PD앨범";
            data_pageInfo.sortOpt = QVariant::fromValue(ProcBugsAPI::BugsPDAlbum_Sort::Popular).toString();
            this->movePage_pd_album_allView(data_pageInfo);
        }
        else if(btnId == BTN_IDX___pdAlbum_recent){
            // 인기PD앨범 - All View 이동
            PageInfo_PDAlbumAllView data_pageInfo;
            data_pageInfo.mainTitle = "최신 뮤직PD앨범";
            data_pageInfo.sortOpt = QVariant::fromValue(ProcBugsAPI::BugsPDAlbum_Sort::Recent).toString();
            this->movePage_pd_album_allView(data_pageInfo);
        }
        else if(btnId == BTN_IDX___theme){
            // 장르/테마 선택
            DataCommon_Bugs& dataCommon_bugs = DataCommon_Bugs::instance();
            if(dataCommon_bugs.list_theme.count()>0){
                bugs::FilterOptionData data_filterOpt = dataCommon_bugs.list_theme.at(0);
                QJsonObject jsonObj_move = ConvertData_forBugs::getObjectJson_filterOptionData(data_filterOpt);
                jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___EXPLORE___CHOOSE_THEME);
                emit this->signal_clickedMovePage(jsonObj_move);
            }
        }
    }




    /**
     * @brief BugsExplore::slot_applyResult_pd_albums
     */
    void BugsExplore::slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>& list_data, const bool flag_lastPage){
        Q_UNUSED(flag_lastPage);
        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___pd_album_popular){
            this->flagReg_album_popular = false;
            this->list_pd_album_popular->append(list_data);
            this->createList_itemPD_Album_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_pd_album_popular, 0, SECTION_FOR_MORE_POPUP___pd_album_popular);
        }
        else if(section == SECTION_FOR_MORE_POPUP___pd_album_recent){
            this->flagReg_album_recent = false;
            this->list_pd_album_recent->append(list_data);
            this->createList_itemPD_Album_applyingWithData(list_data, tidal::AbstractItem::ImageSizeMode::Square_200x200, this->hBox_pd_album_recent, 0, SECTION_FOR_MORE_POPUP___pd_album_recent);
        }
    }



    /**
     * @brief BugsExplore::slot_clickedItemPD_Album
     */
    void BugsExplore::slot_clickedItemPD_Album(const tidal::AbstractItem::ClickMode clickMode){
        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        if(section == SECTION_FOR_MORE_POPUP___pd_album_popular){
            this->proc_clicked_itemPD_Album(this->list_pd_album_popular, clickMode, index, section);
        }
        else if(section == SECTION_FOR_MORE_POPUP___pd_album_recent){
            this->proc_clicked_itemPD_Album(this->list_pd_album_recent, clickMode, index, section);
        }
    }


    /**
     * @brief BugsExplore::proc_preUpdate_favorite_pd_album
     * @param data_itemPos
     */
    void BugsExplore::proc_preUpdate_favorite_pd_album(const ItemPositionData& data_itemPos){
        if(data_itemPos.section == SECTION_FOR_MORE_POPUP___pd_album_popular){
            // Update Data
            bugs::PD_AlbumItemData data_pd_album = this->list_pd_album_popular->at(data_itemPos.index);
            data_pd_album.likes_yn = data_itemPos.likes_yn;
            this->list_pd_album_popular->replace(data_itemPos.index, data_pd_album);
        }
        else if(data_itemPos.section == SECTION_FOR_MORE_POPUP___pd_album_recent){
            // Update Data
            bugs::PD_AlbumItemData data_pd_album = this->list_pd_album_recent->at(data_itemPos.index);
            data_pd_album.likes_yn = data_itemPos.likes_yn;
            this->list_pd_album_recent->replace(data_itemPos.index, data_pd_album);
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] OptMorePopup (더보기팝업에서 발생한 signal 처리)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void BugsExplore::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___pd_album_popular){
            this->proc_clicked_optMorePopup_fromPD_Album(this->list_pd_album_popular, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___pd_album_recent){
            this->proc_clicked_optMorePopup_fromPD_Album(this->list_pd_album_recent, index, clickMode);
        }
    }



}
