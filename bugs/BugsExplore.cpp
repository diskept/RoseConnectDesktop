#include "bugs/BugsExplore.h"

#include "bugs/ConvertData_forBugs.h"
#include "bugs/ProcBugsAPI.h"
#include "bugs/ProcRoseAPI_withBugs.h"
#include "bugs/DataCommon_Bugs.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/toastmsg.h"

#include "bugs/BugsChooseFilterOpt.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include <QDebug>
#include <QScroller>


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
    }

    BugsExplore::~BugsExplore(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void BugsExplore::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data for Bugs
            this->list_pd_album_recent->clear();
            this->list_pd_album_popular->clear();

            flagReg_album_recent[0] = false;
            flagReg_album_popular[0] = false;
            flagReg_theme[0] = false;

            flagReg_album_recent[1] = false;
            flagReg_album_popular[1] = false;
            flagReg_theme[1] = false;

            this->setUIControl_request();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }

    /**
     * @brief BugsExplore::setActivePage
     */
    void BugsExplore::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractBugsSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            //---------------------//c220729  start


            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);


            //--------------------------------------------//c220729  end
            // init UI

            this->vBox_pd_album_recent = new QVBoxLayout();
            this->vBox_pd_album_popular = new QVBoxLayout();
            this->vBox_theme = new QVBoxLayout();

            this->hBox_pd_album_recent = new QHBoxLayout();
            this->hBox_pd_album_popular = new QHBoxLayout();

            // init UI
            GSCommon::clearLayout(this->vBox_pd_album_recent);
            GSCommon::clearLayout(this->vBox_pd_album_popular);
            GSCommon::clearLayout(this->vBox_theme);


            GSCommon::clearLayout(this->hBox_pd_album_recent);
            GSCommon::clearLayout(this->hBox_pd_album_popular);



            // sub Title UI
            for(int i = 0; i < 20; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 10; i++){
                this->explore_recent[i] = new bugs::ItemPD_Album_bugs(i, SECTION_FOR_MORE_POPUP___pd_album_recent, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_recent[i], &bugs::ItemPD_Album_bugs::signal_clicked, this, &BugsExplore::slot_clickedItemPD_Album);
            }

            for (int i = 0; i < 10; i++) {
                this->explore_popular[i] = new bugs::ItemPD_Album_bugs(i, SECTION_FOR_MORE_POPUP___pd_album_popular, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->explore_popular[i], &bugs::ItemPD_Album_bugs::signal_clicked, this, &BugsExplore::slot_clickedItemPD_Album);
            }

            this->flowLayout_theme = new FlowLayout(0,20,0);
            this->flowLayout_theme->setSizeConstraint(QLayout::SetMinimumSize);

        }
    }



    void BugsExplore::setUIControl_request(){

        ProcBugsAPI *proc_recent = new ProcBugsAPI(this);
        proc_recent->setProperty("section", SECTION_FOR_MORE_POPUP___pd_album_recent);
        connect(proc_recent, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsExplore::slot_applyResult_pd_albums);
        proc_recent->request_bugs_getList_pd_albums(10, 0, ProcBugsAPI::BugsPDAlbum_Sort::Recent);

        ProcBugsAPI *proc_popular = new ProcBugsAPI(this);
        proc_popular->setProperty("section", SECTION_FOR_MORE_POPUP___pd_album_popular);
        connect(proc_popular, &ProcBugsAPI::completeReq_list_pd_albums, this, &BugsExplore::slot_applyResult_pd_albums);
        proc_popular->request_bugs_getList_pd_albums(10, 0, ProcBugsAPI::BugsPDAlbum_Sort::Popular);


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




    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------


    void BugsExplore::slot_applyResult_pd_albums(const QList<bugs::PD_AlbumItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        int section = sender()->property("section").toInt();
        if(section == SECTION_FOR_MORE_POPUP___pd_album_popular){
            if(list_data.length() > 0)
            {
                this->list_pd_album_popular->append(list_data);
                this->flagReg_album_popular[1] = true;
            }
            this->flagReg_album_popular[0] = true;
            this->setUIControl_appendWidget();
        }
        else if(section == SECTION_FOR_MORE_POPUP___pd_album_recent){
            if(list_data.length() > 0)
            {
                this->list_pd_album_recent->append(list_data);
                this->flagReg_album_recent[1] = true;
            }
            this->flagReg_album_recent[0] = true;
            this->setUIControl_appendWidget();
        }

    }

    void BugsExplore::slot_list_theme_setDone(){

        this->flagReg_theme[1] = true;

        this->flagReg_theme[0] = true;
        this->setUIControl_appendWidget();
    }

    void BugsExplore::setUIControl_appendWidget(){

        if(this->flagReg_album_popular[0] == true && this->flagReg_album_recent[0] == true && this->flagReg_theme[0] == true ){
            QString title = "";
            if(this->flagReg_album_popular[1] == true){
                title = "인기 뮤직PD앨범";
                this->widget_pd_album_popular = new QWidget();
                this->widget_pd_album_popular = this->setUIControl_subTitle_withSideBtn(title, "View all", BTN_IDX___pdAlbum_popular, this->vBox_pd_album_popular);

                this->vBox_pd_album_popular->addSpacing(10);

                this->hBox_pd_album_popular = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_pd_album_popular);

                for(int i = 0; i < this->list_pd_album_popular->size(); i++){
                    this->explore_popular[i]->setData(this->list_pd_album_popular->at(i));
                    this->hBox_pd_album_popular->addWidget(this->explore_popular[i]);
                }

                this->box_main_contents->addLayout(this->vBox_pd_album_popular);
                this->box_main_contents->addSpacing(45);
            }
            else{
                this->widget_pd_album_popular = new QWidget();
                this->widget_pd_album_popular = this->setUIControl_subTitle_withSideBtn(QString("인기 뮤직PD앨범"), "View All", BTN_IDX___pdAlbum_popular, this->vBox_pd_album_popular);

                this->vBox_pd_album_popular->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(305);
                this->vBox_pd_album_popular->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_pd_album_popular);
                this->box_main_contents->addSpacing(30);
            }


            if(this->flagReg_album_recent[1] == true){
                title = "최신 뮤직PD앨범";
                this->widget_pd_album_recent = new QWidget();
                this->widget_pd_album_recent = this->setUIControl_subTitle_withSideBtn(title, "View All", BTN_IDX___pdAlbum_recent, this->vBox_pd_album_recent);

                this->vBox_pd_album_recent->addSpacing(10);

                this->hBox_pd_album_recent = this->setUIControl_hBoxLayout_forPlaylist(this->vBox_pd_album_recent);

                for(int i = 0; i < this->list_pd_album_recent->size(); i++){
                    this->explore_recent[i]->setData(this->list_pd_album_recent->at(i));
                    this->hBox_pd_album_recent->addWidget(this->explore_recent[i]);
                }

                this->box_main_contents->addLayout(this->vBox_pd_album_recent);
                this->box_main_contents->addSpacing(45);
            }
            else{
                this->widget_pd_album_recent = new QWidget();
                this->widget_pd_album_recent = this->setUIControl_subTitle_withSideBtn(QString("최신 뮤직PD앨범"), "View All", BTN_IDX___pdAlbum_recent, this->vBox_pd_album_recent);

                this->vBox_pd_album_recent->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedHeight(275);
                this->vBox_pd_album_recent->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_pd_album_recent);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flagReg_theme[1] == true){
                this->widget_theme = new QWidget();
                this->widget_theme = this->setUIControl_subTitle_withSideBtn(QString("장르/테마"), "View All", BTN_IDX___theme, this->vBox_theme);

                this->vBox_theme->addSpacing(25);

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
                this->vBox_theme->addLayout(flowLayout_theme);

                this->box_main_contents->addLayout(this->vBox_theme);
                this->box_main_contents->addSpacing(45);
            }
        }
    }

    QWidget* BugsExplore::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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

    QHBoxLayout* BugsExplore::setUIControl_hBoxLayout_forPlaylist(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,0,0,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(305);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        QHBoxLayout *hBox_background = new QHBoxLayout();
        hBox_background->addWidget(tmp_scrollArea);

        QWidget *widget_background = new QWidget();
        widget_background->setLayout(hBox_background);
        widget_background->setContentsMargins(0,0,0,0);
        widget_background->setStyleSheet("background-color:#212121; border:0px;");

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
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
    void BugsExplore::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

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
