#include "tidal/TidalVideo.h"

#include "tidal/ConvertData.h"
#include "tidal/ProcCommon.h"
#include "tidal/ProcRosePlay_withTidal.h"

#include "common/gscommon.h"
#include "common/ProcJsonEasy.h"

#include <QScroller>


namespace tidal {


    const int BTN_IDX_SUBTITLE_VIDEO_HIPHOP = 1;
    const int BTN_IDX_SUBTITLE_VIDEO_RNB = 2;
    const int BTN_IDX_SUBTITLE_VIDEO_COUNTRY = 3;
    const int BTN_IDX_SUBTITLE_VIDEO_POP = 4;
    const int BTN_IDX_SUBTITLE_VIDEO_LATIN = 5;
    const int BTN_IDX_SUBTITLE_VIDEO_DANCE = 6;
    const int BTN_IDX_SUBTITLE_VIDEO_ELECTRO = 7;
    const int BTN_IDX_SUBTITLE_VIDEO_INDIE = 8;

    const int SECTION_FOR_MORE_POPUP___video_hiphop = 1;
    const int SECTION_FOR_MORE_POPUP___video_rnb = 2;
    const int SECTION_FOR_MORE_POPUP___video_country = 3;
    const int SECTION_FOR_MORE_POPUP___video_pop = 4;
    const int SECTION_FOR_MORE_POPUP___video_latin = 5;
    const int SECTION_FOR_MORE_POPUP___video_dance = 6;
    const int SECTION_FOR_MORE_POPUP___video_electro = 7;
    const int SECTION_FOR_MORE_POPUP___video_indie = 8;

    const char* KEY_SECTION = "section";


    QString SUB_TITLE___HIPHOP = "Hip Hop / Rap Music Video";
    QString SUB_TITLE___RNB = "R&B / Soul Music Video";
    QString SUB_TITLE___COUNTRY = "Country Music Video";
    QString SUB_TITLE___POP = "Pop Music Video";
    QString SUB_TITLE___LATIN = "Latin Music Video";
    QString SUB_TITLE___DANCE = "Dance Music Video";
    QString SUB_TITLE___ELECTRO = "Electronic Music Video";
    QString SUB_TITLE___INDIE = "Indie / Rock Music Video";

    QString TIDAL_API_PATH___HIPHOP = "genres/Hiphop/videos";
    QString TIDAL_API_PATH___RNB = "genres/Funk/videos";
    QString TIDAL_API_PATH___COUNTRY = "genres/Country/videos";
    QString TIDAL_API_PATH___POP = "genres/Pop/videos";
    QString TIDAL_API_PATH___LATIN = "genres/Latin/videos";
    QString TIDAL_API_PATH___DANCE = "genres/Dance/videos";
    QString TIDAL_API_PATH___ELECTRO = "genres/Electronic/videos";
    QString TIDAL_API_PATH___INDIE = "genres/Rock/videos";

    const int GET_ITEM_SIZE___ONCE_FOR_MAIN = 15;


    /**
     * @brief "타이달 > 비디오" 화면의 생성자
     * @param parent
     */
    TidalVideo::TidalVideo(QWidget *parent) : AbstractTidalSubWidget(MainUIType::VerticalScroll_filter, parent) {

        this->list_video_HipHop = new QList<tidal::VideoItemData>();
        this->list_video_RNB = new QList<tidal::VideoItemData>();
        this->list_video_Country = new QList<tidal::VideoItemData>();
        this->list_video_Pop = new QList<tidal::VideoItemData>();
        this->list_video_Latin = new QList<tidal::VideoItemData>();
        this->list_video_Dance = new QList<tidal::VideoItemData>();
        this->list_video_Electro = new QList<tidal::VideoItemData>();
        this->list_video_Indie = new QList<tidal::VideoItemData>();
    }


    /**
     * @brief 소멸자.
     */
    TidalVideo::~TidalVideo(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void TidalVideo::setJsonObject_forData(const QJsonObject &jsonObj){

        QString pageCode = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(pageCode != this->page){
            this->flagNeedReload = true;

            this->page = pageCode;

            this->list_video_HipHop->clear();
            this->list_video_RNB->clear();
            this->list_video_Country->clear();
            this->list_video_Pop->clear();
            this->list_video_Latin->clear();
            this->list_video_Dance->clear();
            this->list_video_Electro->clear();
            this->list_video_Indie->clear();

            this->flag_HipHop[0] = false;
            this->flag_RNB[0] = false;
            this->flag_Country[0] = false;
            this->flag_Pop[0] = false;
            this->flag_Latin[0] = false;
            this->flag_Dance[0] = false;
            this->flag_Electro[0] = false;
            this->flag_Indie[0] = false;

            this->flag_HipHop[1] = false;
            this->flag_RNB[1] = false;
            this->flag_Country[1] = false;
            this->flag_Pop[1] = false;
            this->flag_Latin[1] = false;
            this->flag_Dance[1] = false;
            this->flag_Electro[1] = false;
            this->flag_Indie[1] = false;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 화면 활성화
     */
    void TidalVideo::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractTidalSubWidget::setActivePage();

            this->box_contents->removeWidget(this->widget_video_contents);
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->box_video_contents = new QVBoxLayout();
            this->box_video_contents->setSpacing(0);
            this->box_video_contents->setContentsMargins(0, 0, 0, 0);

            this->widget_video_contents = new QWidget();
            this->widget_video_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_video_contents->setLayout(this->box_video_contents);

            this->box_contents->addWidget(this->widget_video_contents, 0, Qt::AlignTop);

            // init UI
            this->vBox_video_HipHop = new QVBoxLayout();
            this->vBox_video_RNB = new QVBoxLayout();
            this->vBox_video_Country = new QVBoxLayout();
            this->vBox_video_Pop = new QVBoxLayout();
            this->vBox_video_Latin = new QVBoxLayout();
            this->vBox_video_Dance = new QVBoxLayout();
            this->vBox_video_Electro = new QVBoxLayout();
            this->vBox_video_Indie = new QVBoxLayout();

            GSCommon::clearLayout(this->vBox_video_HipHop);
            GSCommon::clearLayout(this->vBox_video_RNB);
            GSCommon::clearLayout(this->vBox_video_Country);
            GSCommon::clearLayout(this->vBox_video_Pop);
            GSCommon::clearLayout(this->vBox_video_Latin);
            GSCommon::clearLayout(this->vBox_video_Dance);
            GSCommon::clearLayout(this->vBox_video_Electro);
            GSCommon::clearLayout(this->vBox_video_Indie);

            this->hBox_video_HipHop = new QHBoxLayout();
            this->hBox_video_RNB = new QHBoxLayout();
            this->hBox_video_Country = new QHBoxLayout();
            this->hBox_video_Pop = new QHBoxLayout();
            this->hBox_video_Latin = new QHBoxLayout();
            this->hBox_video_Dance = new QHBoxLayout();
            this->hBox_video_Electro = new QHBoxLayout();
            this->hBox_video_Indie = new QHBoxLayout();

            GSCommon::clearLayout(this->hBox_video_HipHop);
            GSCommon::clearLayout(this->hBox_video_RNB);
            GSCommon::clearLayout(this->hBox_video_Country);
            GSCommon::clearLayout(this->hBox_video_Pop);
            GSCommon::clearLayout(this->hBox_video_Latin);
            GSCommon::clearLayout(this->hBox_video_Dance);
            GSCommon::clearLayout(this->hBox_video_Electro);
            GSCommon::clearLayout(this->hBox_video_Indie);

            // sub Title UI
            for(int i = 0; i < 10; i++){
                this->lb_subTitle[i] = new QLabel();
                this->btnView_all[i] = new QPushButton();
            }

            for(int i = 0; i < 15; i++){
                this->video_HipHop[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_hiphop, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_HipHop[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_RNB[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_rnb, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_RNB[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_Country[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_country, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_Country[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_Pop[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_pop, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_Pop[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_Latin[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_latin, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_Latin[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_Dance[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_dance, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_Dance[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_Electro[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_electro, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_Electro[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            for(int i = 0; i < 15; i++){
                this->video_Indie[i] = new tidal::ItemVideo(i, SECTION_FOR_MORE_POPUP___video_indie, tidal::AbstractItem::ImageSizeMode::Ractangle_360x200);
                connect(this->video_Indie[i], &tidal::ItemVideo::signal_clicked, this, &TidalVideo::slot_clickedItemVideo);
            }

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//cheontidal

            // request HTTP API
            ProcCommon *proc_hip = new ProcCommon(this);
            //proc_hip->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_hiphop);
            connect(proc_hip, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___hiphop);
            proc_hip->request_tidal_getList_videos(TIDAL_API_PATH___HIPHOP, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_rnb = new ProcCommon(this);
            //proc_rnb->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_rnb);
            connect(proc_rnb, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___rnb);
            proc_rnb->request_tidal_getList_videos(TIDAL_API_PATH___RNB, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_country = new ProcCommon(this);
            //proc_country->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_country);
            connect(proc_country, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___country);
            proc_country->request_tidal_getList_videos(TIDAL_API_PATH___COUNTRY, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_pop = new ProcCommon(this);
            //proc_pop->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_pop);
            connect(proc_pop, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___pop);
            proc_pop->request_tidal_getList_videos(TIDAL_API_PATH___POP, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_latin = new ProcCommon(this);
            //proc_latin->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_latin);
            connect(proc_latin, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___latin);
            proc_latin->request_tidal_getList_videos(TIDAL_API_PATH___LATIN, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_dance = new ProcCommon(this);
            //proc_dance->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_dance);
            connect(proc_dance, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___dance);
            proc_dance->request_tidal_getList_videos(TIDAL_API_PATH___DANCE, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_elect = new ProcCommon(this);
            //proc_elect->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_electro);
            connect(proc_elect, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___electro);
            proc_elect->request_tidal_getList_videos(TIDAL_API_PATH___ELECTRO, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);

            ProcCommon *proc_indie = new ProcCommon(this);
            //proc_indie->setProperty(KEY_SECTION, SECTION_FOR_MORE_POPUP___video_indie);
            connect(proc_indie, &ProcCommon::completeReq_list_videos, this, &TidalVideo::slot_applyResult_videos___indie);
            proc_indie->request_tidal_getList_videos(TIDAL_API_PATH___INDIE, GET_ITEM_SIZE___ONCE_FOR_MAIN, 0);
        }
    }


    void TidalVideo::setUIControl_appendWidget(){

        if(flag_HipHop[0] == true && flag_RNB[0] == true && flag_Country[0] == true && flag_Pop[0] == true
                && flag_Latin[0] == true && flag_Dance[0] == true && flag_Electro[0] == true && flag_Indie[0] == true){

            if(flag_HipHop[1] == true){
                this->widget_video_HipHop = new QWidget();
                this->widget_video_HipHop = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___HIPHOP + QString(" (%1)").arg(this->list_video_HipHop->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_HIPHOP, this->vBox_video_HipHop);

                this->vBox_video_HipHop->addSpacing(10);

                this->hBox_video_HipHop = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_HipHop);

                for(int i = 0; i < this->list_video_HipHop->size(); i++){
                    this->video_HipHop[i]->setData(this->list_video_HipHop->at(i));
                    this->hBox_video_HipHop->addWidget(this->video_HipHop[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_HipHop);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_RNB[1] == true){
                this->widget_video_RNB = new QWidget();
                this->widget_video_RNB = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___RNB + QString(" (%1)").arg(this->list_video_RNB->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_RNB, this->vBox_video_RNB);

                this->vBox_video_RNB->addSpacing(10);

                this->hBox_video_RNB = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_RNB);

                for(int i = 0; i < this->list_video_RNB->size(); i++){
                    this->video_RNB[i]->setData(this->list_video_RNB->at(i));
                    this->hBox_video_RNB->addWidget(this->video_RNB[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_RNB);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_Country[1] == true){
                this->widget_video_Country = new QWidget();
                this->widget_video_Country = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___COUNTRY + QString(" (%1)").arg(this->list_video_Country->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_COUNTRY, this->vBox_video_Country);

                this->vBox_video_Country->addSpacing(10);

                this->hBox_video_Country = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_Country);

                for(int i = 0; i < this->list_video_Country->size(); i++){
                    this->video_Country[i]->setData(this->list_video_Country->at(i));
                    this->hBox_video_Country->addWidget(this->video_Country[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_Country);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_Pop[1] == true){
                this->widget_video_Pop = new QWidget();
                this->widget_video_Pop = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___POP + QString(" (%1)").arg(this->list_video_Pop->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_POP, this->vBox_video_Pop);

                this->vBox_video_Pop->addSpacing(10);

                this->hBox_video_Pop = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_Pop);

                for(int i = 0; i < this->list_video_Pop->size(); i++){
                    this->video_Pop[i]->setData(this->list_video_Pop->at(i));
                    this->hBox_video_Pop->addWidget(this->video_Pop[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_Pop);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_Latin[1] == true){
                this->widget_video_Latin = new QWidget();
                this->widget_video_Latin = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___LATIN + QString(" (%1)").arg(this->list_video_Latin->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_LATIN, this->vBox_video_Latin);

                this->vBox_video_Latin->addSpacing(10);

                this->hBox_video_Latin = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_Latin);

                for(int i = 0; i < this->list_video_Latin->size(); i++){
                    this->video_Latin[i]->setData(this->list_video_Latin->at(i));
                    this->hBox_video_Latin->addWidget(this->video_Latin[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_Latin);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_Dance[1] == true){
                this->widget_video_Dance = new QWidget();
                this->widget_video_Dance = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___DANCE + QString(" (%1)").arg(this->list_video_Dance->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_DANCE, this->vBox_video_Dance);

                this->vBox_video_Dance->addSpacing(10);

                this->hBox_video_Dance = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_Dance);

                for(int i = 0; i < this->list_video_Dance->size(); i++){
                    this->video_Dance[i]->setData(this->list_video_Dance->at(i));
                    this->hBox_video_Dance->addWidget(this->video_Dance[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_Dance);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_Electro[1] == true){
                this->widget_video_Electro = new QWidget();
                this->widget_video_Electro = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___ELECTRO + QString(" (%1)").arg(this->list_video_Electro->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_ELECTRO, this->vBox_video_Electro);

                this->vBox_video_Electro->addSpacing(10);

                this->hBox_video_Electro = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_Electro);

                for(int i = 0; i < this->list_video_Electro->size(); i++){
                    this->video_Electro[i]->setData(this->list_video_Electro->at(i));
                    this->hBox_video_Electro->addWidget(this->video_Electro[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_Electro);
                this->box_video_contents->addSpacing(30);
            }

            if(flag_Indie[1] == true){
                this->widget_video_Indie = new QWidget();
                this->widget_video_Indie = this->setUIControl_subTitle_withSideBtn(SUB_TITLE___INDIE + QString(" (%1)").arg(this->list_video_Indie->at(0).totalCount), "View All", BTN_IDX_SUBTITLE_VIDEO_INDIE, this->vBox_video_Indie);

                this->vBox_video_Indie->addSpacing(10);

                this->hBox_video_Indie = this->setUIControl_hBoxLayout_forVideo(this->vBox_video_Indie);

                for(int i = 0; i < this->list_video_Indie->size(); i++){
                    this->video_Indie[i]->setData(this->list_video_Indie->at(i));
                    this->hBox_video_Indie->addWidget(this->video_Indie[i]);
                }

                this->box_video_contents->addLayout(this->vBox_video_Indie);
                this->box_video_contents->addSpacing(30);
            }

            ContentLoadingwaitingMsgHide();
        }
    }


    QWidget* TidalVideo::setUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const int btnId, QLayout *p_layout){

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


    QHBoxLayout* TidalVideo::setUIControl_hBoxLayout_forVideo(QLayout *p_layout){

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
        tmp_scrollArea->setFixedHeight(285);

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


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : [Slot] functions
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 타이달 다이얼로그에서 로그인 성공 이벤트 발생에 대한 처리
     * @details 토스트 메시지를 띄우고, 로그인한 화면으로 전환
     */
    void TidalVideo::slot_acceptedDialogLogin(){
        // 부모 함수 호출
        AbstractTidalSubWidget::slot_acceptedDialogLogin();

        // 로그인한 UI로 변경함
        this->setActivePage();
    }


    void TidalVideo::slot_applyResult_videos___hiphop(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_HipHop->append(list_data);
            this->flag_HipHop[1] = true;
        }

        this->flag_HipHop[0] = true;
        this->setUIControl_appendWidget();
    }


    void TidalVideo::slot_applyResult_videos___rnb(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_RNB->append(list_data);
            this->flag_RNB[1] = true;
        }

        this->flag_RNB[0] = true;
        this->setUIControl_appendWidget();
    }


    void TidalVideo::slot_applyResult_videos___country(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_Country->append(list_data);
            this->flag_Country[1] = true;
        }

        this->flag_Country[0] = true;
        this->setUIControl_appendWidget();
    }

    void TidalVideo::slot_applyResult_videos___pop(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_Pop->append(list_data);
            this->flag_Pop[1] = true;
        }

        this->flag_Pop[0] = true;
        this->setUIControl_appendWidget();
    }

    void TidalVideo::slot_applyResult_videos___latin(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_Latin->append(list_data);
            this->flag_Latin[1] = true;
        }

        this->flag_Latin[0] = true;
        this->setUIControl_appendWidget();
    }

    void TidalVideo::slot_applyResult_videos___dance(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_Dance->append(list_data);
            this->flag_Dance[1] = true;
        }

        this->flag_Dance[0] = true;
        this->setUIControl_appendWidget();
    }

    void TidalVideo::slot_applyResult_videos___electro(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_Electro->append(list_data);
            this->flag_Electro[1] = true;
        }

        this->flag_Electro[0] = true;
        this->setUIControl_appendWidget();
    }

    void TidalVideo::slot_applyResult_videos___indie(const QList<tidal::VideoItemData>& list_data, const bool flag_lastPage){

        Q_UNUSED(flag_lastPage);

        if(list_data.size() > 0){
            this->list_video_Indie->append(list_data);
            this->flag_Indie[1] = true;
        }

        this->flag_Indie[0] = true;
        this->setUIControl_appendWidget();
    }


    /**
     * @brief [slot] override - 사이드 버튼 클릭에 대한 Slot 재정의
     * @param btnId
     */
    void TidalVideo::slot_clickBtn_subTitle_viewAll(){

        int btnId = sender()->property("idx").toInt();

        if(btnId == BTN_IDX_SUBTITLE_VIDEO_HIPHOP){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___HIPHOP;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_RNB){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___RNB;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_COUNTRY){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___COUNTRY;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_POP){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___POP;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_LATIN){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___LATIN;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_DANCE){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___DANCE;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_ELECTRO){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___ELECTRO;
            this->movePage_video_allView(data_page);
        }
        else if(btnId == BTN_IDX_SUBTITLE_VIDEO_INDIE){
            // Video All View 화면으로 이동
            PageInfo_VideoAllView data_page;
            data_page.pathTitle = this->lb_subTitle[btnId]->text();
            data_page.api_subPath = TIDAL_API_PATH___INDIE;
            this->movePage_video_allView(data_page);
        }
    }


    /**
     * @brief [slot] override - ItemVideo 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void TidalVideo::slot_clickedItemVideo(const AbstractItem::ClickMode clickMode){

        int index = ((AbstractItem*)sender())->index();
        int section = ((AbstractItem*)sender())->section();

        if(clickMode == AbstractItem::ClickMode::PlayBtn || clickMode == AbstractItem::ClickMode::AllBox){

            int curr_clickMode = global.device.getMusicPlayType();

            OptMorePopup::ClickMode playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
            switch(curr_clickMode)
            {
                case 13:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
                    break;
                case 12:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
                    break;
                case 15:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
                    break;
                case 16:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
                    break;
                case 17:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
                    break;
                case 18:
                    playType = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
                    break;
                case 19:
                    playType = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
                    break;
            }

            if(playType == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || playType == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || playType == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
            {
                QJsonArray tmpJsonArr = QJsonArray();
                // ClickMode 별로 처리 - Video 파트가 여러개임
                if(section == SECTION_FOR_MORE_POPUP___video_hiphop){
                    for(int i = index; i < this->list_video_HipHop->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_HipHop->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_rnb){
                    for(int i = index; i < this->list_video_RNB->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_RNB->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_country){
                    for(int i = index; i < this->list_video_Country->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Country->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_pop){
                    for(int i = index; i < this->list_video_Pop->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Pop->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_latin){
                    for(int i = index; i < this->list_video_Latin->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Latin->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_dance){
                    for(int i = index; i < this->list_video_Dance->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Dance->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_electro){
                    for(int i = index; i < this->list_video_Electro->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Electro->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_indie){
                    for(int i = index; i < this->list_video_Indie->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Indie->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_videolist(tmpJsonArr, playType);
            }
            else{
                QJsonObject tmpJsonObj = QJsonObject();
                if(section == SECTION_FOR_MORE_POPUP___video_hiphop){
                    tmpJsonObj = this->list_video_HipHop->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_rnb){
                    tmpJsonObj = this->list_video_RNB->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_country){
                    tmpJsonObj = this->list_video_Country->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_pop){
                    tmpJsonObj = this->list_video_Pop->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_latin){
                    tmpJsonObj = this->list_video_Latin->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_dance){
                    tmpJsonObj = this->list_video_Dance->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_electro){
                    tmpJsonObj = this->list_video_Electro->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_indie){
                    tmpJsonObj = this->list_video_Indie->at(index).jsonObj_toPlay;
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_video(tmpJsonObj, playType);
            }
        }
        else{
            // ClickMode 별로 처리 - Video 파트가 여러개임
            if(section == SECTION_FOR_MORE_POPUP___video_hiphop){
                this->proc_clicked_itemVideo(this->list_video_HipHop, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_rnb){
                this->proc_clicked_itemVideo(this->list_video_RNB, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_country){
                this->proc_clicked_itemVideo(this->list_video_Country, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_pop){
                this->proc_clicked_itemVideo(this->list_video_Pop, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_latin){
                this->proc_clicked_itemVideo(this->list_video_Latin, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_dance){
                this->proc_clicked_itemVideo(this->list_video_Dance, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_electro){
                this->proc_clicked_itemVideo(this->list_video_Electro, clickMode, index, section);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_indie){
                this->proc_clicked_itemVideo(this->list_video_Indie, clickMode, index, section);
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
    void TidalVideo::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last

                )
        {
            if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
                    || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last)
            {
                QJsonArray tmpJsonArr = QJsonArray();
                // ClickMode 별로 처리 - Video 파트가 여러개임
                if(section == SECTION_FOR_MORE_POPUP___video_hiphop){
                    for(int i = index; i < this->list_video_HipHop->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_HipHop->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_rnb){
                    for(int i = index; i < this->list_video_RNB->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_RNB->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_country){
                    for(int i = index; i < this->list_video_Country->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Country->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_pop){
                    for(int i = index; i < this->list_video_Pop->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Pop->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_latin){
                    for(int i = index; i < this->list_video_Latin->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Latin->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_dance){
                    for(int i = index; i < this->list_video_Dance->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Dance->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_electro){
                    for(int i = index; i < this->list_video_Electro->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Electro->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_indie){
                    for(int i = index; i < this->list_video_Indie->size(); i++){
                        QJsonObject tmpJsonObj = this->list_video_Indie->at(i).jsonObj_toPlay;
                        tmpJsonArr.append(tmpJsonObj);
                    }
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_videolist(tmpJsonArr, clickMode);
            }
            else{
                QJsonObject tmpJsonObj = QJsonObject();
                if(section == SECTION_FOR_MORE_POPUP___video_hiphop){
                    tmpJsonObj = this->list_video_HipHop->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_rnb){
                    tmpJsonObj = this->list_video_RNB->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_country){
                    tmpJsonObj = this->list_video_Country->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_pop){
                    tmpJsonObj = this->list_video_Pop->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_latin){
                    tmpJsonObj = this->list_video_Latin->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_dance){
                    tmpJsonObj = this->list_video_Dance->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_electro){
                    tmpJsonObj = this->list_video_Electro->at(index).jsonObj_toPlay;
                }
                else if(section == SECTION_FOR_MORE_POPUP___video_indie){
                    tmpJsonObj = this->list_video_Indie->at(index).jsonObj_toPlay;
                }

                // Rose Play 요청
                ProcRosePlay_withTidal *procRosePlay = new ProcRosePlay_withTidal(this);
                procRosePlay->requestPlayRose_video(tmpJsonObj, clickMode);
            }
        }
        else if(clickMode == OptMorePopup::ClickMode::Share){//c220903_2
            print_debug();
            setUIShare();
            qDebug() << "this->shareLink="<<this->shareLink;
        }
        else{
            if(section == SECTION_FOR_MORE_POPUP___video_hiphop){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_HipHop, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_rnb){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_RNB, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_country){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_Country, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_pop){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_Pop, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_latin){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_Latin, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_dance){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_Dance, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_electro){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_Electro, index, clickMode);
            }
            else if(section == SECTION_FOR_MORE_POPUP___video_indie){
                this->proc_clicked_optMorePopup_fromVideo(this->list_video_Indie, index, clickMode);
            }
        }

    }

}
