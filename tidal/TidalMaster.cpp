#include "TidalMaster.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"

#include <QScroller>

namespace tidal {


    const int BTN_IDX_SUBTITLE_playlists= 1;
    const int BTN_IDX_SUBTITLE_albums = 2;

    // section 정의할 때에는 숫자값은 상관 없음. 중복 없게만 하면 됨
    const int SECTION_FOR_MORE_POPUP___playlists = 1;
    const int SECTION_FOR_MORE_POPUP___albums = 2;

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;

    const QString TIDAL_API_PATH_ALBUM = "master/recommended/albums";
    const QString TIDAL_API_PATH_PLAYLIST = "master/recommended/playlists";

    const QString TIDAL_MAIN_TITLE = "Master";
    const QString TIDAL_SUB_TITLE___album = "New Albums";
    const QString TIDAL_SUB_TITLE___playlist = "New Playlist";


    /**
     * @brief "TIDAL > 탐색 > NEW 메인" 화면의 생성자
     * @param parent
     */
    TidalMaster::TidalMaster(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent){

        this->list_album = new QList<tidal::AlbumItemData>();
        this->list_playlist = new QList<tidal::PlaylistItemData>();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalMaster::setJsonObject_forData(const QJsonObject &jsonObj){
        // set the main title and Master's path

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            // init data
            this->list_album->clear();
            this->list_playlist->clear();

            this->flag_album[0] = false;
            this->flag_playlist[0] = false;

            this->flag_album[1] = false;
            this->flag_playlist[1] = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            ProcCommon *proc_album = new ProcCommon(this);
            connect(proc_album, &ProcCommon::completeReq_list_albums, this, &TidalMaster::slot_applyResult_albums);
            proc_album->request_tidal_getList_albums(TIDAL_API_PATH_ALBUM, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);


            ProcCommon *proc_playlist = new ProcCommon(this);
            connect(proc_playlist, &ProcCommon::completeReq_list_playlists, this, &TidalMaster::slot_applyResult_playlists);
            proc_playlist->request_tidal_getList_playlists(TIDAL_API_PATH_PLAYLIST,GET_ITEM_SIZE___ONCE_FOR_MAIN,0);
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }

    /**
     * @brief 소멸자.
     */
    TidalMaster::~TidalMaster(){
        this->deleteLater();
    }

    /**
     * @brief 타이달 > 탐색 > "장르" 1개 선택이 되었을 때, 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void TidalMaster::setActivePage(){

        if(this->flagNeedReload == true){
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_main_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // init UI
            this->vBox_album = new QVBoxLayout();
            this->vBox_playlist = new QVBoxLayout();

            this->hBox_album = new QHBoxLayout();
            this->hBox_playlist = new QHBoxLayout();

            // clear UI
            GSCommon::clearLayout(this->vBox_album);
            GSCommon::clearLayout(this->vBox_playlist);

            GSCommon::clearLayout(this->hBox_album);
            GSCommon::clearLayout(this->hBox_playlist);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }


            for(int i = 0; i < 15; i++){
                this->master_album[i] = new tidal::ItemAlbum(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->master_album[i], &tidal::ItemAlbum::signal_clicked, this, &TidalMaster::slot_clickedItemAlbum);
            }


            for(int i = 0; i < 15; i++){
                this->master_playlist[i] = new tidal::ItemPlaylist(i, SECTION_FOR_MORE_POPUP___playlists, tidal::AbstractItem::ImageSizeMode::Square_200x200);
                connect(this->master_playlist[i], &tidal::ItemPlaylist::signal_clicked, this, &TidalMaster::slot_clickedItemPlaylist);
            }

        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    void TidalMaster::setUIControl_appendWidget(){

        if(this->flag_album[0] == true &&  this->flag_playlist[0] == true ){

            if(this->flag_album[1] == true){
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("Albums (%1)").arg(this->list_album->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                this->hBox_album = this->setUIControl_hBoxLayout_forAlbum(this->vBox_album);

                for(int i = 0; i < this->list_album->size(); i++){
                    this->master_album[i]->setData(this->list_album->at(i));
                    this->hBox_album->addWidget(this->master_album[i]);
                }

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_album = new QWidget();
                this->widget_album = this->setUIControl_subTitle_withSideBtn(QString("Albums (0)"), "View All", BTN_IDX_SUBTITLE_albums, this->vBox_album);

                this->vBox_album->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedHeight(285);
                this->vBox_album->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_album);
                this->box_main_contents->addSpacing(30);
            }

            if(this->flag_playlist[1] == true){
                this->widget_playlist = new QWidget();
                this->widget_playlist = this->setUIControl_subTitle_withSideBtn(QString("Playlists (%1)").arg(this->list_playlist->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_playlists, this->vBox_playlist);
                this->vBox_playlist->addSpacing(10);

                this->hBox_playlist = this->setUIControl_hBoxLayout_forPlaylists(this->vBox_playlist);

                for(int i = 0; i < this->list_playlist->size(); i++){
                    this->master_playlist[i]->setData(this->list_playlist->at(i));
                    this->hBox_playlist->addWidget(this->master_playlist[i]);
                }

                this->box_main_contents->addLayout(this->vBox_playlist);
                this->box_main_contents->addSpacing(30);
            }
            else{
                this->widget_playlist = new QWidget();
                this->widget_playlist = this->setUIControl_subTitle_withSideBtn(QString("Playlists (0)"), "View All", BTN_IDX_SUBTITLE_playlists, this->vBox_playlist);

                this->vBox_playlist->addSpacing(10);

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedHeight(305);
                this->vBox_playlist->addWidget(noData_widget, 0, Qt::AlignCenter);

                this->box_main_contents->addLayout(this->vBox_playlist);
                this->box_main_contents->addSpacing(30);
            }


            ContentLoadingwaitingMsgHide();
            this->widget_main_contents->show();
        }
    }

    QWidget* TidalMaster::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

        // box_main_contents 에 담을 widget, layout 생성.  box_main_contents에 먼저 담는다.
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

    QHBoxLayout* TidalMaster::setUIControl_hBoxLayout_forAlbum(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,0,0,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(285);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_album;
    }

    QHBoxLayout* TidalMaster::setUIControl_hBoxLayout_forPlaylists(QLayout *p_layout){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0, 0, 0, 0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0, 0, 0, 0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("background-color:transparent; border:0px;");
        tmp_scrollArea->setContentsMargins(0, 0, 0 ,0);
        tmp_scrollArea->setFixedHeight(305);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        // Apply Main Layout with spacing
        p_layout->addWidget(tmp_scrollArea);

        // return
        return hBox_playlist;
    }

    void TidalMaster::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_albums){
            PageInfo_AlbumAllView data_page;
            data_page.pathTitle = QString("New Albums");
            data_page.api_subPath = TIDAL_API_PATH_ALBUM;
            this->movePage_album_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_playlists){
            PageInfo_PlaylistAllView data_page;
            data_page.pathTitle = QString("New Playlists");
            data_page.api_subPath = TIDAL_API_PATH_PLAYLIST;
            this->movePage_playlist_allView(data_page);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------


    /**
     * @brief [slot] 앨범 데이터를 받아서 처리함
     * @param list_data
     */
    void TidalMaster::slot_applyResult_albums(const QList<tidal::AlbumItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_album->append(list_data);
            this->flag_album[1] = true;
        }

        this->flag_album[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] playlist 데이터를 받아서 처리함
     * @param list_data
     * @param jsonArr_dataToPlay
     * @param flag_lastPage
     */
    void TidalMaster::slot_applyResult_playlists(const QList<tidal::PlaylistItemData>& list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_playlist->append(list_data);
            this->flag_playlist[1] = true;
        }

        this->flag_playlist[0] = true;
        this->setUIControl_appendWidget();
    }



    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMaster::slot_clickedItemAlbum(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);
    }


    /**
     * @brief [slot] override - ItemPlaylist 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalMaster::slot_clickedItemPlaylist(const ItemPlaylist::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        this->proc_clicked_itemPlaylist(this->list_playlist, clickMode, index, section);
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
    void TidalMaster::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
        else if(section == SECTION_FOR_MORE_POPUP___playlists){
            this->proc_clicked_optMorePopup_fromPlaylist(this->list_playlist, index, section, clickMode);
        }
    }

}
