#include "music/MusicAlbumAll.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ProcRosePlay_withRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/optionpopup.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace music {

    const int SECTION_FOR_MORE_POPUP___albums = 0;

    const int HTTP_NETWORK_PLAY = 99;


    /**
     * @brief AlbumListAll::AlbumListAll
     * @param parent
     */
    AlbumListAll::AlbumListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    AlbumListAll::~AlbumListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void AlbumListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_AlbumAllView data_pageInfo = roseHome::ConvertData::convertData_pageInfo_albumAllView(jsonObj);
        this->flagNeedReload = false;

        if(this->curr_api_subPath != data_pageInfo.api_subPath || this->curr_api_mainTitle != data_pageInfo.pathTitle){
            this->flagNeedReload = true;

            this->curr_api_mainTitle = data_pageInfo.pathTitle;
            this->curr_api_subPath = data_pageInfo.api_subPath;

            this->label_mainTitle->setText(this->curr_api_mainTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
            connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &AlbumListAll::slot_applyResult_albumlistCheck);
            proc->request_rose_getList_recentlyAlbums(this->curr_api_subPath, "MUSIC", 0, 50);
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AlbumListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_albums);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void AlbumListAll::setUIControl_albums(){

        GSCommon::clearLayout(this->box_mainTitle);

        this->widget_mainTitle = new QWidget();
        this->widget_mainTitle->setStyleSheet("background-color:#171717;");
        this->widget_mainTitle->setMinimumSize(800, 69);
        this->widget_mainTitle->resize(global.LmtCnt, 69);

        this->label_mainTitle = new QLabel(this->widget_mainTitle);
        this->label_mainTitle->setStyleSheet("background-color:transparent; font-size:26px; color:#FFFFFF;");
        this->label_mainTitle->setText("");
        this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

        this->btn_mainTitle = new QPushButton(this->widget_mainTitle);
        this->btn_mainTitle->setObjectName("btn_mainTitle");
        this->btn_mainTitle->setCursor(Qt::PointingHandCursor);
        this->btn_mainTitle->setStyleSheet("background-color:transparent;");

        connect(this->btn_mainTitle, &QPushButton::clicked, this, &AlbumListAll::slot_btnClicked_Delete);

        this->label_delete_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", this->btn_mainTitle);
        this->label_delete_icon->setGeometry(0, 10, 50, 50);

        this->label_delete = new QLabel(this->btn_mainTitle);
        this->label_delete->setText(tr("Delete"));
        this->label_delete->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_delete->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_delete->setGeometry(60, 23, this->label_delete->sizeHint().width(), this->label_delete->sizeHint().height());

        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);

        this->box_mainTitle->addWidget(this->widget_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void AlbumListAll::initAll(){

        // init
        this->next_offset = 0;
        this->album_total_cnt = 0;
        this->album_draw_cnt = 0;

        this->list_album->clear();

        // request HTTP API
        this->flagReqMore_album = false;
        this->flag_lastPage_album = false;

        this->flag_album_draw = false;

        GSCommon::clearLayout(this->flowLayout_albums);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void AlbumListAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_album && !this->flag_lastPage_album) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_album = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Album 데이터를 더 요청한다.
     * @param page
     */
    void AlbumListAll::request_more_albumData(){

        // next_offset
        if(this->list_album->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // request HTTP API
        roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
        connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &AlbumListAll::slot_applyResult_albums);
        proc->setProperty("idx", this->next_offset);
        proc->request_rose_getList_recentlyAlbums(this->curr_api_subPath, "MUSIC", next_offset, 200);

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }
    }


    void AlbumListAll::request_more_albumDraw(){

        this->flag_album_draw = true;

        int f_width = this->flowLayout_albums->geometry().width();

        if(this->flowLayout_albums->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
        int f_mod = this->album_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->album_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AlbumListAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void AlbumListAll::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] RoseHomeAlbumListAll::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);

                if(max_cnt > this->list_album->count()){
                    max_cnt = this->list_album->count();
                }
                this->album_draw_cnt += max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AlbumListAll::slot_clickedItemAlbum);
                }

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }

        }
        else{

            ContentLoadingwaitingMsgHide();

            if(this->list_album->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_albums->addWidget(noData_widget);
            }

        }
    }


    void AlbumListAll::slot_applyResult_albumlistCheck(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        int change_flag = 0;

        if(list_data.size() > 0){
            if(this->list_album->count() == 0){
                QString tmpTitle = tr("Recently Played Album") + QString(" (%1)").arg(list_data.at(0).totalCount);
                this->label_mainTitle->setText(tmpTitle);
                this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                this->initAll();

                print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                this->request_more_albumData();

                this->flagNeedReload = true;
                this->setActivePage();
            }
            else{
                for(int i = 0; i < list_data.count(); i++){
                    if(list_data.at(i).title != this->list_album->at(i).title){
                        change_flag++;
                    }
                }

                if((list_data.at(0).totalCount != this->list_album->at(0).totalCount) || (change_flag > 0)){

                    QString tmpTitle = tr("Recently Played Album") + QString(" (%1)").arg(list_data.at(0).totalCount);
                    this->label_mainTitle->setText(tmpTitle);
                    this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

                    this->initAll();

                    print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
                    this->request_more_albumData();

                    this->flagNeedReload = true;
                    this->setActivePage();
                }
                else{
                    ContentLoadingwaitingMsgHide();
                }
            }
        }
        else if(list_data.size() == 0){

            QString tmpTitle = tr("Recently Played Album (0)");
            this->label_mainTitle->setText(tmpTitle);
            this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());

            this->initAll();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_albumData();

            this->flagNeedReload = true;
            this->setActivePage();
        }
        else{
            ContentLoadingwaitingMsgHide();
        }
    }


    void AlbumListAll::slot_btnClicked_Delete(){

        QStringList tmpTitle = curr_api_mainTitle.split("(");

        QJsonObject tmpObj;
        tmpObj.insert("contents_type", tmpTitle.at(0));
        tmpObj.insert("list_type", "ALBUM");
        tmpObj.insert("api_subPath", this->curr_api_subPath);
        tmpObj.insert("filter_type", "MUSIC");

        QJsonObject tmpObj_move;
        tmpObj_move.insert(KEY_PAGE_CODE, PAGECODE_VA_RECENTLY_LIST_DELETE);
        tmpObj_move.insert(KEY_DATA, tmpObj);

        emit this->linker->signal_clickedMovePage(tmpObj_move);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void AlbumListAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        roseHome::AlbumItemData data_album = this->list_album->at(index);

        if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptMorePopup 띄우기
            this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, section, true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
            // Album 바로 재생
            roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
            procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

            // Album Detail 페이지 진입
            QJsonObject jsonData;
            jsonData.insert("id", data_album.id);
            jsonData.insert("star", data_album.star);
            jsonData.insert("type", data_album.type);
            jsonData.insert("title", data_album.title);
            jsonData.insert("thumbnail", data_album.thumbnail);
            jsonData.insert("playTime", data_album.playTime);
            jsonData.insert("clientKey", data_album.clientKey);
            jsonData.insert("trackId", data_album.trackId);
            jsonData.insert("macAddress", data_album.macAddress);
            jsonData.insert("favorite", data_album.favorite);

            jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTALBUMINFO);

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void AlbumListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }


    /**
     * @brief AlbumListAll::resizeEvent
     * @param event
     */
    void AlbumListAll::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);

        this->widget_mainTitle->resize(global.LmtCnt, 69);
        this->btn_mainTitle->setGeometry(global.LmtCnt - (this->label_delete->sizeHint().width() + 60 + 53), 0, (this->label_delete->sizeHint().width() + 60), 50);
    }






    /**
     * @brief ArtistAlbumListAll::ArtistAlbumListAll
     * @param parent
     */
    ArtistAlbumListAll::ArtistAlbumListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_artistAlbums = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    ArtistAlbumListAll::~ArtistAlbumListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void ArtistAlbumListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpTitle = ProcJsonEasy::getString(jsonObj, "title");
        QString tmpArtist = ProcJsonEasy::getString(jsonObj, "artist");
        this->flagNeedReload = false;

        if(this->curr_api_mainTitle != tmpTitle){
            this->flagNeedReload = true;

            this->artist = tmpArtist;
            this->curr_api_mainTitle = tmpTitle;
            this->label_mainTitle->setText(this->curr_api_mainTitle);

            // init
            this->list_artistAlbums->clear();

            this->jsonArr_Album = QJsonArray();
            this->jsonArr_Album = ProcJsonEasy::getJsonArray(jsonObj, "data");

            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_flow_draw = false;
            this->flag_album_draw = false;
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void ArtistAlbumListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_albums);

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->setDataAlbumFromDB();
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void ArtistAlbumListAll::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Albums");

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    void ArtistAlbumListAll::setDataAlbumFromDB(){

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        QList<roseHome::AlbumItemData> tmp_list_album;
        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
            strQuery += " FROM album_info AS AI ";
            strQuery += " LEFT OUTER JOIN album_art AS AA ";
            strQuery += " ON AI._id=AA.album_id ";

            if(this->artist.front() == "'" || this->artist.back() == "'" || this->artist.contains("'")){
                strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist=\"%1\") ORDER BY AI.album ";
            }
            else{
                strQuery += " WHERE AI._id IN (SELECT album_id FROM audio WHERE artist='%1') ORDER BY AI.album ";
            }

            QVariantList data;
            sqlite->exec(strQuery.arg(this->artist), data);

            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    QMap<QString, QVariant> map = data.at(i).toMap();

                    QString tmpMime = map["mime_type"].toString();
                    int tmpbitDepth = map["bitdepth"].toInt();
                    int tmpsampleRate = map["samplerate"].toInt();

                    if(!tmpMime.isEmpty()){
                        QString tmp_data = tmpMime;
                        QStringList splitToken = tmp_data.split("/");

                        if(splitToken.size() > 1){
                            QString tmp_type = splitToken.at(1);
                            if(tmp_type == "x-wav" || tmp_type == "wav"){
                                tmpMime = "WAV";
                            }
                            else if (tmp_type == "wv") {
                                tmpMime = "WV";
                            }
                            else if(tmp_type == "flac"){
                                tmpMime = "FLAC";
                            }
                            else if(tmp_type == "dff" || tmp_type == "dsf"){
                                tmpMime = "DSD";
                            }
                            else if(tmp_type == "mp4"){
                                tmpMime = "ALAC";
                            }
                            else if(tmp_type == "mpeg"){
                                tmpMime = "MP3";
                            }
                            else if(tmp_type == "amr"){
                                tmpMime = "AMR";
                            }
                            else if(tmp_type == "amr-wb"){
                                tmpMime = "AWB";
                            }
                            else if(tmp_type == "x-ms-wma"){
                                tmpMime = "WMA";
                            }
                            else if(tmp_type == "ape"){
                                tmpMime = "APE";
                            }
                            else if(tmp_type == "ogg"){
                                tmpMime = "OGG";
                            }
                            else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                                tmpMime = "AAC";
                            }
                            else if(tmp_type == "x-matroska"){
                                tmpMime = "MKA";
                            }
                            else if(tmp_type == "midi"){
                                tmpMime = "MIDI";
                            }
                            else if(tmp_type == "sp-midi"){
                                tmpMime = "SMF";
                            }
                            else if(tmp_type == "imelody"){
                                tmpMime = "IMY";
                            }
                            else if(tmp_type == "x-mpegurl"){
                                tmpMime = "M3U8";
                            }
                            else if(tmp_type == "x-scpls"){
                                tmpMime = "PLS";
                            }
                            else if(tmp_type == "aiff"){
                                tmpMime = "AIFF";
                            }
                        }
                    }

                    QString mimeType = "";
                    if(tmpMime == "MP3" || tmpMime == "AMR" || tmpMime == "AWB" || tmpMime == "WMA" || tmpMime == "APE" || tmpMime == "OGG" || tmpMime == "AAC" || tmpMime == "MKA"
                            || tmpMime == "MIDI" || tmpMime == "SMF" || tmpMime == "IMY" || tmpMime == "M3U8" || tmpMime == "PLS" || tmpMime == "AIFF" || tmpMime == "WV"){

                        mimeType = tmpMime;
                    }
                    else if(tmpMime == "DSD"){
                        if(tmpsampleRate == 88200 || tmpsampleRate == 2822400){
                            mimeType = tmpMime + " 64";
                        }
                        else if(tmpsampleRate == 176400 || tmpsampleRate == 5644800){
                            mimeType = tmpMime + " 128";
                        }
                        else if(tmpsampleRate == 352800 || tmpsampleRate == 11289600){
                            mimeType = tmpMime + " 256";
                        }
                        else if(tmpsampleRate == 705600 || tmpsampleRate == 22579200){
                            mimeType = tmpMime + " 512";
                        }
                        else if(tmpsampleRate == 1411200 || tmpsampleRate == 45158400){
                            mimeType = tmpMime + " 1024";
                        }
                        else{
                            mimeType = tmpMime;
                        }
                    }
                    else if(tmpMime == "WAV" || tmpMime == "FLAC" || tmpMime == "ALAC"){
                        if(tmpbitDepth == 0 || tmpbitDepth == 16){
                            mimeType = tmpMime;
                        }
                        else if(tmpbitDepth > 16){
                            mimeType = tmpMime + QString(" %1").arg(tmpbitDepth);
                        }
                    }

                    QString tmpImg = map["album_art"].toString();
                    if(!tmpImg.isEmpty()){
                        tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                    }

                    roseHome::AlbumItemData data_output;
                    data_output.id = map["album_id"].toInt();
                    //data_output.totalCount = ProcJsonEasy::getInt(jsonObj, "totalCount");
                    data_output.trackCount = map["num_of_song"].toInt();

                    data_output.type = "MUSIC";
                    data_output.title = map["album"].toString();
                    data_output.artist = map["artist"].toString();
                    data_output.playTime = map["album_art"].toString();
                    data_output.thumbnail = tmpImg;
                    data_output.mime = mimeType;

                    tmp_list_album.append(data_output);
                }
            }
        }

        sqlite->close();
        delete sqlite;

        this->slot_applyResult_albums(tmp_list_album, QJsonArray(), true);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void ArtistAlbumListAll::initAll(){

        GSCommon::clearLayout(this->flowLayout_albums);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void ArtistAlbumListAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_artistAlbums->size() > this->album_draw_cnt) && (this->flag_album_draw == false)){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void ArtistAlbumListAll::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        Q_UNUSED(p_id);
        Q_UNUSED(p_jsonObject);

        /*QJsonDocument doc(p_jsonObject);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "jsonObj = " << strJson;*/
    }


    void ArtistAlbumListAll::request_more_albumDraw(){

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > 70 ) ? 70 : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &ArtistAlbumListAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_artistAlbums->at(i));
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

            QCoreApplication::processEvents();
        }

        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void ArtistAlbumListAll::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_artistAlbums->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_artistAlbums->size();
            this->list_artistAlbums->append(list_data);

            if(start_index == 0){
                int max_cnt = (this->list_artistAlbums->size() > 70) ? 70 : this->list_artistAlbums->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &ArtistAlbumListAll::slot_clickedItemAlbum);
                }

                ContentLoadingwaitingMsgHide();

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_artistAlbums->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->flowLayout_albums->addWidget(noData_widget);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void ArtistAlbumListAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        roseHome::AlbumItemData data_album = this->list_artistAlbums->at(index);

        if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptMorePopup 띄우기
            this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, section, true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

            QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);
            // Album 바로 재생
            QJsonObject tmp_json;
            tmp_json.insert("musicPlayType", 15);
            tmp_json.insert("music", jsonArr_tracks);
            tmp_json.insert("currentPosition", 0);
            tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

            /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
            QJsonDocument doc(tmp_json);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            qDebug() << "jsonObj = " << strJson;*/

            if(global.Queue_track_count != 0) {
                emit linker->signal_checkQueue(27, "");

                return;
            }
            emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_PLAY
                             , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                             , tmp_json
                             , true
                             , true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

            // Album Detail 페이지 진입
            QJsonObject jsonData;
            jsonData.insert(KEY_OP_TITLE_MAIN, data_album.title);
            jsonData.insert(KEY_OP_TITLE_SUB, data_album.artist);
            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Album);
            jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
            jsonData.insert(KEY_OP_MIME, data_album.mime);
            jsonData.insert(KEY_OP_ARTIST, data_album.artist);
            jsonData.insert(KEY_OP_cntStar, 0);
            jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));
            jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
            jsonData.insert("isAlbum", true);

            jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    QJsonArray ArtistAlbumListAll::getDataForPlayMusic(const int album_id){

        QJsonArray jsonArray;

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            /*strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.track ";*/
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._display_name AS orderName, A._data AS data, A.title, A.artist, A.duration, A.bookmark, A.track, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.bookmark ASC, A.track ASC, orderName ASC ";

            QVariantList dataDB;
            sqlite->exec(strQuery.arg(album_id), dataDB);
            if(dataDB.size() > 0){
                for(int i = 0 ; i < dataDB.size(); i++){
                    DataPopup *tmp_dataPopup = new DataPopup(this);
                    tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                    jsonArray.append(tmp_dataPopup->getJsonData());
                }
            }
        }

        sqlite->close();
        delete sqlite;

        return jsonArray;
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void ArtistAlbumListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            //this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);

            roseHome::AlbumItemData data_album = this->list_artistAlbums->at(index);

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                int playType = 0;

                if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                    playType = 15;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last){
                    playType = 13;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty){
                    playType = 12;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){
                    playType = 16;
                }
                else{
                    playType = 15;
                }

                // Album 바로 재생
                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);

                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
                QJsonDocument doc(tmp_json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "jsonObj = " << strJson;*/

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                // artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit this->linker->signal_clickedMovePage(jsonData);
            }
        }
    }





    /**
     * @brief AlbumHistoryAll::RoseHomeAlbumListAll
     * @param parent
     */
    AlbumHistoryAll::AlbumHistoryAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    AlbumHistoryAll::~AlbumHistoryAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void AlbumHistoryAll::setJsonObject_forData(const QJsonObject &jsonObj){

        roseHome::PageInfo_AlbumAllView tmp_pageInfo = roseHome::ConvertData::convertData_pageInfo_albumAllView(jsonObj);
        this->flagNeedReload = false;

        if(tmp_pageInfo.api_subPath != this->data_pageInfo.api_subPath || tmp_pageInfo.pathTitle != this->data_pageInfo.pathTitle || tmp_pageInfo.filter_type != this->data_pageInfo.filter_type){
            this->flagNeedReload = true;

            this->data_pageInfo = tmp_pageInfo;

            this->label_mainTitle->setText(this->data_pageInfo.pathTitle);

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            // request HTTP API
            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            // request HTTP API
            this->request_more_albumData();
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void AlbumHistoryAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_albums);

            if(this->flag_flow_draw == true){
                this->flag_flow_draw = false;
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void AlbumHistoryAll::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("");

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] RoseHomeAlbumListAll::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }

    /**
     * @brief 스크롤 Bottom에 대하여 Get more data 처리
     */
    void AlbumHistoryAll::proc_wheelEvent_to_getMoreData(){

        if((!this->flagReqMore_album && !this->flag_lastPage_album) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){
            this->flagReqMore_album = true;

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     * @param event
     */
    void AlbumHistoryAll::resizeEvent(QResizeEvent *event){

        AbstractRoseHomeSubWidget::resizeEvent(event);

        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
    }

    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Album 데이터를 더 요청한다.
     * @param page
     */
    void AlbumHistoryAll::request_more_albumData(){

        // next_offset
        if(this->list_album->size() == 0){
            this->next_offset = 0;
        }
        else{
            this->next_offset++;
        }


        // request HTTP API
        roseHome::ProcCommon *proc_album = new roseHome::ProcCommon(this);
        connect(proc_album, &roseHome::ProcCommon::completeReq_list_albums, this, &AlbumHistoryAll::slot_applyResult_historyAlbum);
        proc_album->setProperty("idx", this->next_offset);
        proc_album->request_rose_getList_historyAlbums(this->data_pageInfo.api_subPath, this->data_pageInfo.filter_type, this->next_offset, 200);

        if(this->next_offset == 0){
            this->flag_album_draw = true;
        }
    }

    void AlbumHistoryAll::request_more_albumDraw(){

        this->flag_album_draw = true;

        int f_width = this->flowLayout_albums->geometry().width();

        if(this->flowLayout_albums->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
        int f_mod = this->album_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->album_widget_cnt = f_wg_cnt * 10;
        }
        else{
            this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] RoseHomeAlbumListAll::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;


        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AlbumHistoryAll::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void AlbumHistoryAll::slot_applyResult_historyAlbum(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        int idx = sender()->property("idx").toInt();

        if(idx != this->next_offset){
            return;
        }

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                int start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
                this->album_draw_cnt += max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &AlbumHistoryAll::slot_clickedItemAlbum);
                }

                this->setFlowLayoutResize(this,this->flowLayout_albums,this->album_widget_width,this->album_widget_margin);

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_album->size() <= 0){
                // noData widget change - by diskept j230317 start
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(f_width, 500);
                // noData widget change - by diskept j230317 finish

                this->flowLayout_albums->addWidget(noData_widget);
            }
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void AlbumHistoryAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        //int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        //this->proc_clicked_itemAlbum(this->list_album, clickMode, index, section);

        roseHome::AlbumItemData data_album = this->list_album->at(index);

        if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptMorePopup 띄우기
            this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, SECTION_FOR_MORE_POPUP___albums, true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){
            // Album 바로 재생
            roseHome::ProcRosePlay_withRosehome *procRosePlay = new roseHome::ProcRosePlay_withRosehome(this);
            procRosePlay->requestPlayRose_byAlbumID(data_album.id, OptMorePopup::ClickMode::Play_RightNow);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

            // Album Detail 페이지 진입
            QJsonObject jsonData;
            jsonData.insert("id", data_album.id);
            jsonData.insert("star", data_album.star);
            jsonData.insert("type", data_album.type);
            jsonData.insert("title", data_album.title);
            jsonData.insert("thumbnail", data_album.thumbnail);
            jsonData.insert("playTime", data_album.playTime);
            jsonData.insert("clientKey", data_album.clientKey);
            jsonData.insert("trackId", data_album.trackId);
            jsonData.insert("macAddress", data_album.macAddress);
            jsonData.insert("favorite", data_album.favorite);

            jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_RECENTALBUMINFO);

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void AlbumHistoryAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);
        }
    }





    /**
     * @brief AlbumListAll::AlbumListAll
     * @param parent
     */
    SearchAlbumListAll::SearchAlbumListAll(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_viewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }

    /**
     * @brief 소멸자.
     */
    SearchAlbumListAll::~SearchAlbumListAll(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void SearchAlbumListAll::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpTitle = ProcJsonEasy::getString(jsonObj, "title");
        this->flagNeedReload = false;

        if(this->curr_api_mainTitle != tmpTitle){
            this->flagNeedReload = true;

            this->curr_api_mainTitle = tmpTitle;
            this->label_mainTitle->setText(this->curr_api_mainTitle);

            // init
            this->list_album->clear();

            this->jsonArr_Album = QJsonArray();
            this->jsonArr_Album = ProcJsonEasy::getJsonArray(jsonObj, "data");

            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->flagReqMore_album = false;
            this->flag_lastPage_album = false;

            this->flag_flow_draw = false;
            this->flag_album_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void SearchAlbumListAll::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_albums);

            QList<roseHome::AlbumItemData> tmp_list_album;
            for(int i = 0; i < this->jsonArr_Album.size(); i++){

                QJsonObject tmpObj = this->jsonArr_Album.at(i).toObject();

                QString tmpMime = ProcJsonEasy::getString(tmpObj, "mime_type");
                int tmpbitDepth = ProcJsonEasy::getInt(tmpObj, "bitdepth");
                int tmpsampleRate = ProcJsonEasy::getInt(tmpObj, "samplerate");

                if(!tmpMime.isEmpty()){
                    QString tmp_data = tmpMime;
                    QStringList splitToken = tmp_data.split("/");

                    if(splitToken.size() > 1){
                        QString tmp_type = splitToken.at(1);
                        if(tmp_type == "x-wav" || tmp_type == "wav"){
                            tmpMime = "WAV";
                        }
                        else if (tmp_type == "wv") {
                            tmpMime = "WV";
                        }
                        else if(tmp_type == "flac"){
                            tmpMime = "FLAC";
                        }
                        else if(tmp_type == "dff" || tmp_type == "dsf"){
                            tmpMime = "DSD";
                        }
                        else if(tmp_type == "mp4"){
                            tmpMime = "ALAC";
                        }
                        else if(tmp_type == "mpeg"){
                            tmpMime = "MP3";
                        }
                        else if(tmp_type == "amr"){
                            tmpMime = "AMR";
                        }
                        else if(tmp_type == "amr-wb"){
                            tmpMime = "AWB";
                        }
                        else if(tmp_type == "x-ms-wma"){
                            tmpMime = "WMA";
                        }
                        else if(tmp_type == "ape"){
                            tmpMime = "APE";
                        }
                        else if(tmp_type == "ogg"){
                            tmpMime = "OGG";
                        }
                        else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                            tmpMime = "AAC";
                        }
                        else if(tmp_type == "x-matroska"){
                            tmpMime = "MKA";
                        }
                        else if(tmp_type == "midi"){
                            tmpMime = "MIDI";
                        }
                        else if(tmp_type == "sp-midi"){
                            tmpMime = "SMF";
                        }
                        else if(tmp_type == "imelody"){
                            tmpMime = "IMY";
                        }
                        else if(tmp_type == "x-mpegurl"){
                            tmpMime = "M3U8";
                        }
                        else if(tmp_type == "x-scpls"){
                            tmpMime = "PLS";
                        }
                        else if(tmp_type == "aiff"){
                            tmpMime = "AIFF";
                        }
                    }
                }

                QString mimeType = "";
                if(tmpMime == "MP3" || tmpMime == "AMR" || tmpMime == "AWB" || tmpMime == "WMA" || tmpMime == "APE" || tmpMime == "OGG" || tmpMime == "AAC" || tmpMime == "MKA"
                        || tmpMime == "MIDI" || tmpMime == "SMF" || tmpMime == "IMY" || tmpMime == "M3U8" || tmpMime == "PLS" || tmpMime == "AIFF" || tmpMime == "WV"){

                    mimeType = tmpMime;
                }
                else if(tmpMime == "DSD"){
                    if(tmpsampleRate == 88200 || tmpsampleRate == 2822400){
                        mimeType = tmpMime + " 64";
                    }
                    else if(tmpsampleRate == 176400 || tmpsampleRate == 5644800){
                        mimeType = tmpMime + " 128";
                    }
                    else if(tmpsampleRate == 352800 || tmpsampleRate == 11289600){
                        mimeType = tmpMime + " 256";
                    }
                    else if(tmpsampleRate == 705600 || tmpsampleRate == 22579200){
                        mimeType = tmpMime + " 512";
                    }
                    else if(tmpsampleRate == 1411200 || tmpsampleRate == 45158400){
                        mimeType = tmpMime + " 1024";
                    }
                    else{
                        mimeType = tmpMime;
                    }
                }
                else if(tmpMime == "WAV" || tmpMime == "FLAC" || tmpMime == "ALAC"){
                    if(tmpbitDepth == 0 || tmpbitDepth == 16){
                        mimeType = tmpMime;
                    }
                    else if(tmpbitDepth > 16){
                        mimeType = tmpMime + QString(" %1").arg(tmpbitDepth);
                    }
                }

                QString tmpImg = ProcJsonEasy::getString(tmpObj, "album_art");
                if(!tmpImg.isEmpty()){
                    tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + tmpImg;
                }

                roseHome::AlbumItemData data_output;
                data_output.id = ProcJsonEasy::getInt(tmpObj, "album_id");
                data_output.trackCount = ProcJsonEasy::getInt(tmpObj, "num_of_song");
                data_output.totalCount = this->jsonArr_Album.size();

                data_output.type = "MUSIC";
                data_output.title = ProcJsonEasy::getString(tmpObj, "album");
                data_output.artist =ProcJsonEasy::getString(tmpObj, "artist");
                data_output.playTime = ProcJsonEasy::getString(tmpObj, "album_art");
                data_output.thumbnail = tmpImg;
                data_output.mime = mimeType;

                tmp_list_album.append(data_output);
            }

            this->slot_applyResult_albums(tmp_list_album, QJsonArray(), true);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void SearchAlbumListAll::setUIControl_albums(){

        this->label_mainTitle = this->get_addUIControl_mainTitle("Albums");

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void SearchAlbumListAll::initAll(){

        GSCommon::clearLayout(this->flowLayout_albums);

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void SearchAlbumListAll::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->list_album->size() > this->album_draw_cnt) && (this->flag_album_draw == false)){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    void SearchAlbumListAll::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

        Q_UNUSED(p_id);
        Q_UNUSED(p_jsonObject);

        /*QJsonDocument doc(p_jsonObject);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << "jsonObj = " << strJson;*/
    }


    void SearchAlbumListAll::request_more_albumDraw(){

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > 70 ) ? 70 : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, false);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &SearchAlbumListAll::slot_clickedItemAlbum);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void SearchAlbumListAll::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                int max_cnt = (this->list_album->size() > 70) ? 70 : this->list_album->size();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &SearchAlbumListAll::slot_clickedItemAlbum);
                }

                for(int i = start_index; i < max_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

                    QCoreApplication::processEvents();
                }
                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }


        }
        else{
            ContentLoadingwaitingMsgHide();

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(1500, 300);

            this->flowLayout_albums->addWidget(noData_widget);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void SearchAlbumListAll::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        roseHome::AlbumItemData data_album = this->list_album->at(index);

        if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptMorePopup 띄우기
            this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, section, true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

            QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);
            // Album 바로 재생
            QJsonObject tmp_json;
            tmp_json.insert("musicPlayType", 15);
            tmp_json.insert("music", jsonArr_tracks);
            tmp_json.insert("currentPosition", 0);
            tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

            /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
            QJsonDocument doc(tmp_json);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            qDebug() << "jsonObj = " << strJson;*/

            if(global.Queue_track_count != 0) {
                emit linker->signal_checkQueue(27, "");

                return;
            }
            emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_PLAY
                             , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                             , tmp_json
                             , true
                             , true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

            // Album Detail 페이지 진입
            QJsonObject jsonData;
            jsonData.insert(KEY_OP_TITLE_MAIN, data_album.title);
            jsonData.insert(KEY_OP_TITLE_SUB, data_album.artist);
            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Album);
            jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
            jsonData.insert(KEY_OP_MIME, data_album.mime);
            jsonData.insert(KEY_OP_ARTIST, data_album.artist);
            jsonData.insert(KEY_OP_cntStar, 0);
            jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));
            jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
            jsonData.insert("isAlbum", true);

            jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    QJsonArray SearchAlbumListAll::getDataForPlayMusic(const int album_id){

        QJsonArray jsonArray;

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            /*strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.track ";*/
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._display_name AS orderName, A._data AS data, A.title, A.artist, A.duration, A.bookmark, A.track, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.bookmark ASC, A.track ASC, orderName ASC ";

            QVariantList dataDB;
            sqlite->exec(strQuery.arg(album_id), dataDB);
            if(dataDB.size() > 0){
                for(int i = 0 ; i < dataDB.size(); i++){
                    DataPopup *tmp_dataPopup = new DataPopup(this);
                    tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                    jsonArray.append(tmp_dataPopup->getJsonData());
                }
            }
        }

        sqlite->close();
        delete sqlite;

        return jsonArray;
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void SearchAlbumListAll::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            //this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);

            roseHome::AlbumItemData data_album = this->list_album->at(index);

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                int playType = 0;

                if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                    playType = 15;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last){
                    playType = 13;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty){
                    playType = 12;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){
                    playType = 16;
                }
                else{
                    playType = 15;
                }

                // Album 바로 재생
                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);

                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
                QJsonDocument doc(tmp_json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "jsonObj = " << strJson;*/

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                // artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit this->linker->signal_clickedMovePage(jsonData);
            }
        }
    }
}
