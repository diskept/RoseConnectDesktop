#include "qobuz/QobuzRecentlyListDelete.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "tidal/ProcCommon.h"

#include "bugs/ConvertData_forBugs.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/ProcJsonEasy.h"

#include "widget/NoData_Widget.h"
#include "widget/dialogconfirm.h"
//#include "widget/toastmsg.h"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QSettings>
#include <QScrollBar>


namespace qobuz {

    /**
     * @brief "ROSEHOME > RECENTLY > DELETE" 화면을 위한 생성자.
     * @see PAGECODE_RH_RECENTLY_LIST_DELETE
     * @param parent
     */
    QobuzRecentlyListDelete::QobuzRecentlyListDelete(QWidget *parent) : AbstractQobuzSubWidget(VerticalScroll_rosefilter, parent) {

        this->linker = Linker::getInstance();

        // UI
        this->setUIControl_lists();
    }


    /**
    * @brief 소멸자.
    */
    QobuzRecentlyListDelete::~QobuzRecentlyListDelete(){

        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void QobuzRecentlyListDelete::setJsonObject_forData(const QJsonObject &jsonObj){

        this->contents_type = ProcJsonEasy::getString(jsonObj, "contents_type");
        this->list_type = ProcJsonEasy::getString(jsonObj, "list_type");
        this->api_subPath = ProcJsonEasy::getString(jsonObj, "api_subPath");
        this->filter_type = ProcJsonEasy::getString(jsonObj, "filter_type");

        // init
        this->jsonArr_list_data = QJsonArray();

        this->list_totalCount = 0;
        this->list_drawCount = 0;
        this->before_scrollMax = 0;

        this->flag_draw = false;

        // request HTTP API
        this->flagReqMore_data = false;
        this->flag_lastPage_data = false;

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        this->request_more_listData();

        this->flagNeedReload = true;
    }


    /**
     * @brief QobuzRecentlyListDelete::setActivePage
     */
    void QobuzRecentlyListDelete::setActivePage(){

        if(this->flagNeedReload){
            // 항상 부모클래스의 함수 먼저 호출
            AbstractQobuzSubWidget::setActivePage();

            // UI
            this->setUIControl_lists();

            if(this->flag_widget_visible == true){
                this->widget_main_contents->hide();
                this->box_contents->removeWidget(this->widget_main_contents);

                this->flag_widget_visible = false;
            }
            GSCommon::clearLayout(this->box_contents);
            this->box_contents->setAlignment(Qt::AlignTop);
            this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            this->box_main_contents = new QVBoxLayout();
            this->box_main_contents->setSpacing(0);
            this->box_main_contents->setContentsMargins(0, 0, 0, 0);
            this->box_main_contents->setAlignment(Qt::AlignTop);

            this->widget_main_contents = new QWidget();
            this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
            this->widget_main_contents->setLayout(this->box_main_contents);

            this->box_contents->addWidget(widget_main_contents);

            this->listWidget_recentlyList = new QListWidget(this);
            this->listWidget_recentlyList->setCursor(Qt::PointingHandCursor);
            this->listWidget_recentlyList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            this->listWidget_recentlyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            this->listWidget_recentlyList->setStyleSheet("background-color:transparent;");

            this->box_main_contents->addWidget(this->listWidget_recentlyList);
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
    void QobuzRecentlyListDelete::setUIControl_lists(){

        GSCommon::clearLayout(this->box_mainTitle);

        int left = 0;

        this->widget_mainTitle = new QWidget();
        this->widget_mainTitle->setStyleSheet("background-color:#171717;");
        this->widget_mainTitle->setMinimumSize(800, 69);
        this->widget_mainTitle->resize(global.LmtCnt, 69);

        this->label_mainTitle = new QLabel(this->widget_mainTitle);
        this->label_mainTitle->setStyleSheet("background-color:transparent; font-size:26px; color:#FFFFFF;");
        this->label_mainTitle->setText(this->contents_type);
        this->label_mainTitle->setGeometry(33, 15, this->label_mainTitle->sizeHint().width(), this->label_mainTitle->sizeHint().height());


        this->btn_deleteAll = new QPushButton(this->widget_mainTitle);
        this->btn_deleteAll->setObjectName("btn_deleteAll");
        this->btn_deleteAll->setCursor(Qt::PointingHandCursor);
        this->btn_deleteAll->setStyleSheet("background-color:transparent;");

        connect(this->btn_deleteAll, &QPushButton::clicked, this, &QobuzRecentlyListDelete::slot_clicked_DeleteAll);

        this->label_deleteAll = new QLabel(this->btn_deleteAll);
        this->label_deleteAll->setText(tr("Delete All"));
        this->label_deleteAll->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_deleteAll->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_deleteAll->setGeometry(0, 23, this->label_deleteAll->sizeHint().width(), this->label_deleteAll->sizeHint().height());

        left = global.LmtCnt - (this->label_deleteAll->sizeHint().width() + 53);
        this->btn_deleteAll->setGeometry(left, 0, this->label_deleteAll->sizeHint().width(), 50);


        this->btn_delete = new QPushButton(this->widget_mainTitle);
        this->btn_delete->setObjectName("btn_delete");
        this->btn_delete->setCursor(Qt::PointingHandCursor);
        this->btn_delete->setStyleSheet("background-color:transparent;");

        connect(this->btn_delete, &QPushButton::clicked, this, &QobuzRecentlyListDelete::slot_clicked_Delete);

        this->label_delete = new QLabel(this->btn_delete);
        this->label_delete->setText(tr("Delete"));
        this->label_delete->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_delete->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_delete->setGeometry(0, 23, this->label_delete->sizeHint().width(), this->label_delete->sizeHint().height());

        left = global.LmtCnt - (this->label_deleteAll->sizeHint().width() + 53 + this->label_delete->sizeHint().width() + 50);
        this->btn_delete->setGeometry(left, 0, this->label_delete->sizeHint().width(), 50);


        this->btn_selectAll = new QPushButton(this->widget_mainTitle);
        this->btn_selectAll->setObjectName("btn_delete");
        this->btn_selectAll->setCursor(Qt::PointingHandCursor);
        this->btn_selectAll->setStyleSheet("background-color:transparent;");

        connect(this->btn_selectAll, &QPushButton::clicked, this, &QobuzRecentlyListDelete::slot_clicked_btnSelectAll);

        this->checkbox_selectAll = new QCheckBox(this->btn_selectAll);
        this->checkbox_selectAll->setGeometry(0, 10, 50, 50);
        this->checkbox_selectAll->setCursor(Qt::PointingHandCursor);
        this->checkbox_selectAll->setStyleSheet("QCheckBox::indicator {width:50px; height:50px;}"
                                                "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

        connect(this->checkbox_selectAll, &QCheckBox::stateChanged, this, &QobuzRecentlyListDelete::slot_clickedSelected);

        this->label_selectAll = new QLabel(this->btn_selectAll);
        this->label_selectAll->setText(tr("Select All"));
        this->label_selectAll->setStyleSheet("background-color:transparent;font-size:20px;color:#FFFFFF;");
        this->label_selectAll->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        this->label_selectAll->setGeometry(50, 23, (this->label_selectAll->sizeHint().width() + 50), this->label_selectAll->sizeHint().height());

        left = global.LmtCnt - (this->label_deleteAll->sizeHint().width() + 53 + this->label_delete->sizeHint().width() + 50 + this->label_selectAll->sizeHint().width() + 50 + 50);
        this->btn_selectAll->setGeometry(left, 0, this->label_selectAll->sizeHint().width() + 50, 50);

        this->box_mainTitle->addWidget(this->widget_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void QobuzRecentlyListDelete::proc_wheelEvent_to_getMoreData(){

        if((this->list_totalCount > this->list_drawCount) && (this->flag_draw == false) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())
                && (this->before_scrollMax != this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;
            this->before_scrollMax = this->scrollArea_main->verticalScrollBar()->maximum();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->request_more_listDraw();
        }
    }


    // MARK : Request HTTP API  ------------------------------------------------------------------------------------------

    /**
     * @brief Track 데이터를 더 요청한다.
     * @param page
     */
    void QobuzRecentlyListDelete::request_more_listData(){

        if(!this->flagReqMore_data && !this->flag_lastPage_data){
            this->flagReqMore_data = true;

            // next_offset
            if(this->list_drawCount == 0){
                this->next_offset = 0;
            }
            else{
                this->next_offset++;
            }

            if(this->list_type == "ALBUM"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_albums, this, &QobuzRecentlyListDelete::slot_applyResult_albumlists);
                proc->request_rose_getList_recentlyAlbums(this->api_subPath, this->filter_type, this->next_offset, 50);
            }
            else if(this->list_type == "PLAYLIST"){
                // request HTTP API
                roseHome::ProcCommon *proc_recently = new roseHome::ProcCommon(this);
                connect(proc_recently, &roseHome::ProcCommon::completeReq_list_playlists, this, &QobuzRecentlyListDelete::slot_applyResult_playlistLists);
                proc_recently->request_rose_getList_recentlyPlaylists(this->api_subPath, this->filter_type, this->next_offset, 50);
            }
            else if(this->list_type == "YOUTUBE"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_rosetubes, this, &QobuzRecentlyListDelete::slot_applyResult_rosetubelists);
                proc->request_rose_getList_recentlyRosetube("member/track/recent", "YOUTUBE", this->next_offset, 50);
            }
            else if(this->list_type == "TRACK"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &QobuzRecentlyListDelete::slot_applyResult_tracklists);
                proc->request_rose_getList_recentlyTracks(this->api_subPath, this->filter_type, this->next_offset, 50);
            }
            else if(this->list_type == "ARTIST"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeReq_list_artists, this, &QobuzRecentlyListDelete::slot_applyResult_artistlists);
                proc->request_rose_getList_recentlyArtists(this->api_subPath, this->filter_type, this->next_offset, 50);
            }
        }
    }


    void QobuzRecentlyListDelete::request_more_listDraw(){

        int start_index = this->list_drawCount;
        int max_cnt = ((this->list_totalCount - this->list_drawCount) > 50 ) ? 50 : (this->list_totalCount - this->list_drawCount);
        this->list_drawCount += max_cnt;

        int height = this->list_drawCount * 70;
        this->listWidget_recentlyList->setFixedHeight(height);

        QJsonArray tmpArray = QJsonArray();
        for(int i = start_index; i < this->list_drawCount; i++){
            if(this->list_type == "ALBUM"){
                QJsonObject tmpObj = this->jsonArr_list_data.at(i).toObject();
                QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(tmpObj, "album");

                QString convertType = ProcJsonEasy::getString(tmpAlbum, "type");
                int id = ProcJsonEasy::getInt(tmpAlbum, "id");

                QString thumbnail = ProcJsonEasy::getString(tmpAlbum, "thumbnail");
                QString title = ProcJsonEasy::getString(tmpAlbum, "title");

                QJsonObject trackInfo;
                trackInfo.insert("type", convertType);
                trackInfo.insert("thumbnail", thumbnail);
                trackInfo.insert("title", title);
                trackInfo.insert("id", id);

                tmpArray.append(trackInfo);
            }
            else if(this->list_type == "PLAYLIST"){
                QJsonObject tmpObj = this->jsonArr_list_data.at(i).toObject();
                QJsonObject tmpPlaylist = ProcJsonEasy::getJsonObject(tmpObj, "playlist");

                QString convertType = ProcJsonEasy::getString(tmpPlaylist, "type");
                int id = ProcJsonEasy::getInt(tmpPlaylist, "id");

                QString thumbnail = ProcJsonEasy::getString(tmpPlaylist, "thumbnail");
                QString title = ProcJsonEasy::getString(tmpPlaylist, "title");

                QJsonObject trackInfo;
                trackInfo.insert("type", convertType);
                trackInfo.insert("thumbnail", thumbnail);
                trackInfo.insert("title", title);
                trackInfo.insert("id", id);

                tmpArray.append(trackInfo);
            }
            else if(this->list_type == "YOUTUBE"){
                QJsonObject tmpObj = this->jsonArr_list_data.at(i).toObject();

                QString convertType = ProcJsonEasy::getString(tmpObj, "type");
                int id = ProcJsonEasy::getInt(tmpObj, "id");

                QString thumbnail = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                QString title = ProcJsonEasy::getString(tmpObj, "title");

                QString artist = "";

                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(tmpObj, "artists");

                for(int i = 0; i < tmpArtists.count(); i++){

                    QString tmpArtsit = tmpArtists.at(i).toString();
                    if(tmpArtsit == "null"){
                        tmpArtsit = "";
                    }

                    if(i == 0){
                        artist = tmpArtsit;
                    }
                    else{
                        artist += "," + tmpArtsit;
                    }
                }

                if(artist.isEmpty()){
                    QJsonObject data = ProcJsonEasy::getJsonObject(tmpObj, "data");
                    QJsonObject snippet = ProcJsonEasy::getJsonObject(data, "snippet");

                    artist = ProcJsonEasy::getString(snippet, "channelTitle");
                }

                QJsonObject trackInfo;
                trackInfo.insert("type", convertType);
                trackInfo.insert("thumbnail", thumbnail);
                trackInfo.insert("title", title);
                trackInfo.insert("artist", artist);
                trackInfo.insert("id", id);

                tmpArray.append(trackInfo);
            }
            else if(this->list_type == "TRACK"){

                QJsonObject tmpObj = this->jsonArr_list_data.at(i).toObject();
                QJsonObject convertData = ProcJsonEasy::getJsonObject(tmpObj, "data");

                QString convertType = ProcJsonEasy::getString(tmpObj, "type");
                int id = ProcJsonEasy::getInt(tmpObj, "id");

                QString thumbnail = ProcJsonEasy::getString(tmpObj, "thumbnailUrl");
                QString title = ProcJsonEasy::getString(tmpObj, "title");

                QString artist = "";
                QString album = "";
                QString bitrate = "";

                double sampling = 0;

                bool hires = false;
                bool explicit_flag = false;

                QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(tmpObj, "artists");

                for(int i = 0; i < tmpArtists.count(); i++){

                    QString tmpArtsit = tmpArtists.at(i).toString();
                    if(tmpArtsit == "null"){
                        tmpArtsit = "";
                    }

                    if(i == 0){
                        artist = tmpArtsit;
                    }
                    else{
                        artist += "," + tmpArtsit;
                    }
                }


                if(tmpObj.contains("albums")){
                    QJsonArray tmpAlbums = ProcJsonEasy::getJsonArray(tmpObj, "albums");

                    for(int i = 0; i < tmpAlbums.count(); i++){

                        QString tmpAlbum = tmpAlbums.at(i).toString();
                        if(tmpAlbum == "null"){
                            tmpAlbum = "";
                        }

                        if(i == 0){
                            album = tmpAlbum;
                        }
                        else{
                            album += "," + tmpAlbum;
                        }
                    }
                }

                if(convertType == "MUSIC"){

                }
                else if(convertType == "YOUTUBE"){

                    if(artist.isEmpty()){
                        if(tmpObj.contains("snippet")){
                            QJsonObject snippet_obj = ProcJsonEasy::getJsonObject(tmpObj, "snippet");

                            artist = ProcJsonEasy::getString(snippet_obj, "channelTitle");
                        }
                    }
                }
                else if(convertType == "TIDAL"){

                    QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");

                    if(thumbnail.isEmpty()){


                        QString imagePath = ProcJsonEasy::getString(tmpAlbum, "cover");

                        if(imagePath.isEmpty()){
                            thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(convertData, "imageId"), 750);
                        }
                        else{
                            thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(tmpAlbum, "cover"), 750);
                        }
                    }

                    if(album.isEmpty()){
                        if(tmpAlbum.contains("title")){
                            album = ProcJsonEasy::getString(tmpAlbum, "title");
                        }
                        else{
                            album = title;
                        }
                    }

                    if(artist.isEmpty()){
                        if(convertData.contains("artist")){
                            QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                            artist = ProcJsonEasy::getString(tmpArtist, "name");
                        }
                    }

                    if(ProcJsonEasy::getString(convertData, "audioQuality") == "HI_RES"){
                        hires = true;
                    }
                    else{
                        hires = false;
                    }

                    explicit_flag = ProcJsonEasy::getBool(convertData, "explicit");
                }
                else if(convertType == "BUGS"){

                    QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");
                    if(thumbnail.isEmpty()){

                        QJsonObject albumImg = ProcJsonEasy::getJsonObject(tmpAlbum, "image");

                        QString url = ProcJsonEasy::getString(albumImg, "url");
                        QString path = ProcJsonEasy::getString(albumImg, "path");
                        thumbnail = url.replace("https://", "http://");
                        thumbnail += QString("%1").arg(static_cast<int>(bugs::ConvertData_forBugs::ImageSize_Square::Size_350x350)) + path;
                    }

                    if(album.isEmpty()){
                        album = ProcJsonEasy::getString(tmpAlbum, "title");
                    }

                    if(artist.isEmpty()){
                        if(convertData.contains("artists")){
                            QJsonArray tmpArtists = ProcJsonEasy::getJsonArray(convertData, "artists");

                            for(int i = 0; i < tmpArtists.size(); i++){
                                QJsonObject tmpArtistInfo = tmpArtists.at(i).toObject();
                                if(i == 0){
                                    artist = ProcJsonEasy::getString(tmpArtistInfo, "artist_nm");
                                }
                                else{
                                    artist += "," + ProcJsonEasy::getString(tmpArtistInfo, "artist_nm");
                                }
                            }
                        }
                    }

                    QJsonArray bitrates1 = ProcJsonEasy::getJsonArray(convertData, "bitrates");//cheon210605

                    if(bitrates1.contains("flac") == true){
                        bitrate = "FLAC";
                    }
                    else if(bitrates1.contains("aac320") == true){
                        bitrate = "AAC320";
                    }
                    else if(bitrates1.contains("320k") == true || bitrates1.contains("mp3") == true){
                        bitrate = "MP3";
                    }
                    else if(bitrates1.contains("aac") == true){
                        bitrate = "AAC";
                    }
                    else if(bitrates1.contains("fullhd") == true){
                        bitrate = "FULL HD";
                    }
                    else if(bitrates1.contains("hd") == true){
                        bitrate = "HD";
                    }
                    else if(bitrates1.contains("sd") == true){
                        bitrate = "SD";
                    }

                    explicit_flag = ProcJsonEasy::getBool(convertData, "adult_yn");
                }
                else if(convertType == "QOBUZ"){

                    QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");

                    if(thumbnail.isEmpty()){
                        QJsonObject tmpImage = ProcJsonEasy::getJsonObject(tmpAlbum, "image");
                        thumbnail = ProcJsonEasy::getString(tmpImage, "thumbnail");
                    }

                    if(album.isEmpty()){
                        album = ProcJsonEasy::getString(tmpAlbum, "title");
                    }

                    if(artist.isEmpty()){
                        if(convertData.contains("artist")){
                            QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                            artist = ProcJsonEasy::getString(tmpArtist, "name");
                        }
                        else{
                            if(tmpAlbum.contains("artist")){
                                QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(tmpAlbum, "artist");
                                artist = ProcJsonEasy::getString(tmpArtist, "name");
                            }
                            else{
                                QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "composer");
                                artist = ProcJsonEasy::getString(tmpArtist, "name");
                            }
                        }
                    }

                    hires = ProcJsonEasy::getBool(convertData, "hires");
                    sampling = ProcJsonEasy::getDouble(convertData, "maximum_sampling_rate");

                    explicit_flag = ProcJsonEasy::getBool(convertData, "parental_warning");
                }
                else if(convertType == "APPLE_MUSIC"){

                    QJsonObject tmpAttributes = ProcJsonEasy::getJsonObject(convertData, "attributes");

                    if(thumbnail.isEmpty()){
                        QJsonObject tmpArtwork = ProcJsonEasy::getJsonObject(tmpAttributes, "artwork");
                        thumbnail = ProcJsonEasy::getString(tmpArtwork, "url");
                        thumbnail.replace("{w}x{h}", "{60}x{60}");
                    }
                }

                QJsonObject trackInfo;
                trackInfo.insert("type", convertType);
                trackInfo.insert("thumbnail", thumbnail);
                trackInfo.insert("title", title);
                trackInfo.insert("artist", artist);
                trackInfo.insert("album", album);
                trackInfo.insert("bitrates", bitrate);
                trackInfo.insert("hires", hires);
                trackInfo.insert("sampling", sampling);
                trackInfo.insert("explicit", explicit_flag);
                trackInfo.insert("id", id);

                tmpArray.append(trackInfo);
            }
            else if(this->list_type == "ARTIST"){
                QJsonObject tmpObj = this->jsonArr_list_data.at(i).toObject();

                QString convertType = ProcJsonEasy::getString(tmpObj, "type");
                int id = ProcJsonEasy::getInt(tmpObj, "id");

                QJsonArray tmpThumbnail = ProcJsonEasy::getJsonArray(tmpObj, "thumbnail");
                QString thumbnail = tmpThumbnail.at(0).toString();

                QString artist = ProcJsonEasy::getString(tmpObj, "name");

                QJsonObject trackInfo;
                trackInfo.insert("type", convertType);
                trackInfo.insert("thumbnail", thumbnail);
                trackInfo.insert("artist", artist);
                trackInfo.insert("id", id);

                tmpArray.append(trackInfo);
            }
        }

        for(int i = start_index; i < this->list_drawCount; i++){
            this->iniDataInfo_RoseList_Item(tmpArray.at(i-start_index).toObject(), i);
            this->setDataInfo_RoseList_Item(tmpArray.at(i-start_index).toObject(), i);
        }

        if(this->list_totalCount == this->list_drawCount){
            this->box_main_contents->addSpacing(30);
        }

        ContentLoadingwaitingMsgHide();
        this->flag_draw = false;
    }


    inline void QobuzRecentlyListDelete::iniDataInfo_RoseList_Item(const QJsonObject &trackInfo, const int &p_index){

        QString type = ProcJsonEasy::getString(trackInfo, "type");
        QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");

        int id = ProcJsonEasy::getInt(trackInfo, "id");

        QWidget *widget_track_info = new QWidget();
        widget_track_info->setFixedSize(1550, 70);
        widget_track_info->setObjectName("widget_track_info");
        widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                            "#widget_track_info:hover{background-color:#333333;} ");

        this->checkbox_Selected[p_index] = new QCheckBox(widget_track_info);
        this->checkbox_Selected[p_index]->setFixedSize(50, 50);
        this->checkbox_Selected[p_index]->setGeometry(0, 10, 0, 0);
        this->checkbox_Selected[p_index]->setProperty("type", this->list_type);
        this->checkbox_Selected[p_index]->setProperty("index", p_index);
        this->checkbox_Selected[p_index]->setStyleSheet("QCheckBox::indicator {width:50px; height:50px;}"
                                                        "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                        "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

        QString default_path = "";
        QString type_path = "";

        if(type == "MUSIC"){
            default_path = ":/images/def_mus_300.png";
            type_path = ":/images/rosehome/home_music_s.png";
        }
        else if(type == "VIDEO"){
            default_path = ":/images/def_mus_300.png";
            type_path = ":/images/rosehome/home_video_s.png";
        }
        else if(type == "YOUTUBE"){
            default_path = ":/images/def_mus_300.png";
            type_path = ":/images/rosehome/home_tube_s.png";
        }
        else if(type == "TIDAL"){
            default_path = ":/images/tidal/tidal_def_400.png";
            type_path = ":/images/rosehome/home_tidal_s.png";
        }
        else if(type == "BUGS"){
            default_path = ":/images/bugs/bugs_def_430x2.png";
            type_path = ":/images/rosehome/home_bugs_s.png";
        }
        else if(type == "QOBUZ"){
            default_path = ":/images/qobuz/qobuz_default_400.png";
            type_path = ":/images/rosehome/home_qobuz_s.png";
        }
        else if(type == "APPLE_MUSIC"){
            default_path = ":/images/apple/apple_def.png";
            type_path = ":/images/rosehome/home_apple.png";
        }
        else{
            default_path = ":/images/def_mus_300.png";
            type_path = ":/images/rosehome/home_music_s.png";
        }

        if(this->list_type == "ARTIST"){
            default_path = ":/images/rosehome/artist_def.png";
        }

        QImage img_album;
        QPixmap pixmap_image;
        if(img_album.load(default_path)){
            QPixmap pixmapIMG = QPixmap(QSize(60, 60));
            if(this->list_type == "ARTIST"){
                pixmapIMG.fill(Qt::transparent);
            }
            else{
                pixmapIMG.fill(Qt::black);
            }

            pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
            pixmap_image = pixmap_image.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPainter painter (&pixmapIMG);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

            QPainterPath path = QPainterPath();
            if(this->list_type == "ARTIST"){
                path.addRoundedRect(0, 0, 60, 60, 30, 30);
            }
            else{
                path.addRoundedRect(0, 0, 60, 60, 2, 2);
            }

            int leftValue = (60 - pixmap_image.width()) / 2;
            int topValue = (60 - pixmap_image.height()) / 2;

            painter.setClipPath(path);
            painter.drawPixmap(leftValue, topValue, pixmap_image);
            painter.end();
        }

        this->label_Img_recently[p_index] = new QLabel(widget_track_info);
        this->label_Img_recently[p_index]->setFixedSize(60, 60);
        this->label_Img_recently[p_index]->setGeometry(60, 5, 0, 0);
        this->label_Img_recently[p_index]->setStyleSheet("background-color:transparent;");
        this->label_Img_recently[p_index]->setPixmap(pixmap_image);

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", p_index);
        fileDownloader->setImageURL(thumbnail);
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_list()));

        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
        *pixmap_icon = pixmap_icon->scaled(40, 40, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QLabel *lb_track_type = new QLabel(widget_track_info);
        lb_track_type->setPixmap(*pixmap_icon);
        lb_track_type->setGeometry(128, 15, 40, 40);
        lb_track_type->setStyleSheet("background-color:transparent;");

        this->lb_explicit[p_index] = new QLabel(widget_track_info);
        this->lb_explicit[p_index]->setFixedSize(30, 30);
        this->lb_explicit[p_index]->setGeometry(180, 20, 0, 0);
        this->lb_explicit[p_index]->hide();

        this->lb_title[p_index] = new QLabel(widget_track_info);
        this->lb_title[p_index]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        this->lb_title[p_index]->setGeometry(180, 10, 720, 49);

        this->lb_album[p_index] = new QLabel(widget_track_info);
        this->lb_album[p_index]->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        this->lb_album[p_index]->setWordWrap(true);
        this->lb_album[p_index]->setGeometry(997, 14, 300, 40);

        this->lb_hires[p_index] = new QLabel(widget_track_info);
        this->lb_hires[p_index]->setStyleSheet("background-color:transparent;border:0px");
        this->lb_hires[p_index]->setGeometry(900, 10, 20, 49);
        this->lb_hires[p_index]->hide();

        this->lb_resolution[p_index] = new QLabel(widget_track_info);
        this->lb_resolution[p_index]->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        this->lb_resolution[p_index]->setGeometry(900, 10, 20, 49);
        this->lb_resolution[p_index]->hide();

        this->lb_artist[p_index] = new QLabel(widget_track_info);
        this->lb_artist[p_index]->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");

        QString tmpProperty = QString("TYPE:%1,").arg(this->list_type) + QString("IDX:%1,").arg(p_index) + QString("ID:%1").arg(id);

        QListWidgetItem *item = new QListWidgetItem();
        item->setWhatsThis(tmpProperty);
        item->setSizeHint(QSize(90, 70));

        this->listWidget_recentlyList->addItem(item);

        this->listWidget_recentlyList->setItemWidget(item, widget_track_info);
    }


    inline void QobuzRecentlyListDelete::setDataInfo_RoseList_Item(const QJsonObject &trackInfo, const int &p_index){

        QString type = ProcJsonEasy::getString(trackInfo, "type");
        QString title = ProcJsonEasy::getString(trackInfo, "title");
        QString artist = ProcJsonEasy::getString(trackInfo, "artist");
        QString album  = ProcJsonEasy::getString(trackInfo, "album");
        QString bitrates = ProcJsonEasy::getString(trackInfo, "bitrates");

        double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

        bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
        bool explicit_flag = ProcJsonEasy::getBool(trackInfo, "explicit");

        QString hires_path = "";

        if(type == "TIDAL"){
            hires_path = ":/images/tidal/mqa_ico.png";
        }if(type == "QOBUZ"){
            hires_path = ":/images/qobuz/hires_ico.png";
        }

        if(this->list_type == "ALBUM"){
            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->lb_title[p_index]->font()));
            if(this->lb_title[p_index]->text().contains("…")){
                this->lb_title[p_index]->setToolTip(title);
                this->lb_title[p_index]->setToolTipDuration(2000);
            }
            this->lb_title[p_index]->setGeometry(180, 10, 720, 49);
        }
        else if(this->list_type == "PLAYLIST"){
            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->lb_title[p_index]->font()));
            if(this->lb_title[p_index]->text().contains("…")){
                this->lb_title[p_index]->setToolTip(title);
                this->lb_title[p_index]->setToolTipDuration(2000);
            }
            this->lb_title[p_index]->setGeometry(180, 10, 720, 49);
        }
        else if(this->list_type == "YOUTUBE"){
            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->lb_title[p_index]->font()));
            if(this->lb_title[p_index]->text().contains("…")){
                this->lb_title[p_index]->setToolTip(title);
                this->lb_title[p_index]->setToolTipDuration(2000);
            }
            this->lb_title[p_index]->setGeometry(180, 10, 720, 49);

            QLabel *tmp_album = new QLabel();
            tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
            tmp_album->setText(artist);

            int album_title_width = 0;
            album_title_width = tmp_album->sizeHint().width();

            if(album_title_width > 300){
                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_album_set = "";

                tmp_split = artist;
                splitToken = tmp_split.split(" ");

                tmp_album->setText("");
                int i = 0;
                if(splitToken.size() > 1){

                    for(i = 0; i < splitToken.count(); i++){
                        if(i == 0){
                            tmp_album_set = splitToken.at(i);
                        }
                        else{
                            tmp_album_set += " " + splitToken.at(i);
                        }
                        tmp_album->setText(tmp_album_set);

                        if(tmp_album->sizeHint().width() >= 300){
                            tmp_album->setText("");
                            tmp_album_set.replace(splitToken.at(i), "");
                            break;
                        }
                    }
                }

                tmp_album->setText("");
                tmp_album->setText(tmp_album_set);

                album_title_width = tmp_album->sizeHint().width() + 300;

                QString strAlbum = album;
                QLabel *tmpAlbum = new QLabel();
                tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
                tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

                int tmpAlbum_width = tmpAlbum->sizeHint().width();

                if(tmpAlbum_width > 300){
                    this->lb_album[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, album_title_width, this->lb_album[p_index]->font()));
                    if(this->lb_album[p_index]->text().contains("…")){
                        this->lb_album[p_index]->setToolTip(artist);
                        this->lb_album[p_index]->setToolTipDuration(2000);
                    }
                }
                else{
                    this->lb_album[p_index]->setText(artist);
                }
                this->lb_album[p_index]->setGeometry(997, 14, 300, 40);
            }
            else{
                this->lb_album[p_index]->setText(artist);
                this->lb_album[p_index]->setGeometry(997, 24, 300, 20);
            }
        }
        else if(this->list_type == "TRACK"){
            QLabel *tmp_title = new QLabel();
            tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
            tmp_title->setText(title);

            int title_width = 0;
            int title_width_resize = 0;
            title_width = tmp_title->sizeHint().width();

            QLabel *tmp_artist = new QLabel();
            tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
            tmp_artist->setText(artist);

            int artist_width = 0;
            artist_width = tmp_artist->sizeHint().width();

            int width = 0;
            int left = 0;
            if(explicit_flag == true){
                QString adult_certification_path = "";

                if(global.lang == 0){
                    adult_certification_path = ":/images/tidal/tidal-e-ico.png";
                }
                else{
                    adult_certification_path = ":/images/bugs/ico-19.png";
                }

                QImage img;
                QPixmap *img_adult_certification = new QPixmap();
                if(img.load(adult_certification_path)){
                    *img_adult_certification = QPixmap::fromImage(img);
                    *img_adult_certification = img_adult_certification->scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                this->lb_explicit[p_index]->setPixmap(*img_adult_certification);
                this->lb_explicit[p_index]->setFixedSize(30, 30);
                this->lb_explicit[p_index]->setGeometry(180, 20, 0, 0);
                this->lb_explicit[p_index]->show();

                if(hires == true){
                    QString hires_path = "";

                    if(type == "TIDAL"){

                        hires_path = ":/images/tidal/mqa_ico.png";

                        title_width += (15 + 36);

                        if(title_width > 630){
                            title_width_resize = 630;
                            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                            if(this->lb_title[p_index]->text().contains("…")){
                                this->lb_title[p_index]->setToolTip(title);
                                this->lb_title[p_index]->setToolTipDuration(2000);
                            }
                            this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                        }
                        else{
                            title_width_resize = title_width;   // - (15 + 36);
                            this->lb_title[p_index]->setText(title);
                            this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                        }

                        QImage img;
                        QPixmap *img_hires = new QPixmap();
                        if(img.load(hires_path)){
                            *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                            *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                        }

                        this->lb_hires[p_index]->setPixmap(*img_hires);
                        this->lb_hires[p_index]->setFixedSize(36, 13);

                        left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                        this->lb_hires[p_index]->setGeometry(left, 18, 0, 0);
                        this->lb_hires[p_index]->show();
                    }
                    else if(type == "QOBUZ"){

                        hires_path = ":/images/qobuz/hires_ico.png";

                        if(sampling > 0){
                            this->lb_resolution[p_index]->setText(QString("%1 kHz").arg(sampling));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width + 15 + 20);

                            if(title_width > 630){
                                title_width_resize = 630;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width + 15 + 20);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }

                            QImage img;
                            QPixmap *img_hires = new QPixmap();
                            if(img.load(hires_path)){
                                *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                                *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                            }

                            this->lb_hires[p_index]->setPixmap(*img_hires);
                            this->lb_hires[p_index]->setFixedSize(20, 20);

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_hires[p_index]->setGeometry(left, 14, 0, 0);
                            this->lb_hires[p_index]->show();

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15 + 20 + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            title_width += (15 + 20);

                            if(title_width > 650){
                                title_width_resize = 650;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 16, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                    }
                    else if(type == "BUGS"){

                        if(!bitrates.isEmpty()){
                            this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width);

                            if(title_width > 650){
                                title_width_resize = 650;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            if(title_width > 670){
                                title_width_resize = 670;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                        }
                    }
                }
                else{
                    if(type == "QOBUZ"){

                        if(sampling > 0){
                            this->lb_resolution[p_index]->setText(QString("%1 kHz").arg(sampling));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width);

                            if(title_width > 650){
                                title_width_resize = 650;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            if(title_width > 670){
                                title_width_resize = 670;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                        }
                    }
                    else if(type == "BUGS"){

                        if(!bitrates.isEmpty()){
                            this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width);

                            if(title_width > 650){
                                title_width_resize = 650;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            if(title_width > 670){
                                title_width_resize = 670;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                            }
                        }
                    }
                    else{
                        if(title_width > 670){
                            title_width_resize = 670;
                            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                            if(this->lb_title[p_index]->text().contains("…")){
                                this->lb_title[p_index]->setToolTip(title);
                                this->lb_title[p_index]->setToolTipDuration(2000);
                            }
                            this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                        }
                        else{
                            title_width_resize = title_width;   // - (15 + width);
                            this->lb_title[p_index]->setText(title);
                            this->lb_title[p_index]->setGeometry(230, 10, title_width_resize, 25);
                        }
                    }
                }

                if(artist_width > 600){
                    this->lb_artist[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 600, this->lb_artist[p_index]->font()));
                    if(this->lb_artist[p_index]->text().contains("…")){
                        this->lb_artist[p_index]->setToolTip(artist);
                        this->lb_artist[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_artist[p_index]->setGeometry(230, 34, 600, 25);
                }
                else{
                    this->lb_artist[p_index]->setText(artist);
                    this->lb_artist[p_index]->setGeometry(230, 34, artist_width, 25);
                }
            }
            else{
                if(hires == true){
                    QString hires_path = "";

                    if(type == "TIDAL"){

                        hires_path = ":/images/tidal/mqa_ico.png";

                        title_width += (15 + 36);

                        if(title_width > 700){
                            title_width_resize = 700;
                            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                            if(this->lb_title[p_index]->text().contains("…")){
                                this->lb_title[p_index]->setToolTip(title);
                                this->lb_title[p_index]->setToolTipDuration(2000);
                            }
                            this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                        }
                        else{
                            title_width_resize = title_width;   // - (15 + 36);
                            this->lb_title[p_index]->setText(title);
                            this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                        }

                        QImage img;
                        QPixmap *img_hires = new QPixmap();
                        if(img.load(hires_path)){
                            *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                            *img_hires = img_hires->scaled(36, 13, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                        }

                        this->lb_hires[p_index]->setPixmap(*img_hires);
                        this->lb_hires[p_index]->setFixedSize(36, 13);

                        left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                        this->lb_hires[p_index]->setGeometry(left, 18, 0, 0);
                        this->lb_hires[p_index]->show();
                    }
                    else if(type == "QOBUZ"){

                        hires_path = ":/images/qobuz/hires_ico.png";

                        if(sampling > 0){
                            this->lb_resolution[p_index]->setText(QString("%1 kHz").arg(sampling));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width + 15 + 20);

                            if(title_width > 680){
                                title_width_resize = 680;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width + 15 + 20);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }

                            QImage img;
                            QPixmap *img_hires = new QPixmap();
                            if(img.load(hires_path)){
                                *img_hires = QPixmap::fromImage(img);                                                           //이미지를 버퍼에 옮긴다.
                                *img_hires = img_hires->scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);          //이미지 사이즈 조절
                            }

                            this->lb_hires[p_index]->setPixmap(*img_hires);
                            this->lb_hires[p_index]->setFixedSize(20, 20);

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_hires[p_index]->setGeometry(left, 14, 0, 0);
                            this->lb_hires[p_index]->show();

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15 + 20 + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            title_width += (15 + 20);

                            if(title_width > 700){
                                title_width_resize = 700;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 16, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                    }
                    else if(type == "BUGS"){

                        if(!bitrates.isEmpty()){
                            this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width);

                            if(title_width > 680){
                                title_width_resize = 680;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            if(title_width > 700){
                                title_width_resize = 700;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                        }
                    }
                }
                else{
                    if(type == "QOBUZ"){

                        if(sampling > 0){
                            this->lb_resolution[p_index]->setText(QString("%1 kHz").arg(sampling));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width);

                            if(title_width > 680){
                                title_width_resize = 680;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            if(title_width > 700){
                                title_width_resize = 700;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                        }
                    }
                    else if(type == "BUGS"){

                        if(!bitrates.isEmpty()){
                            this->lb_resolution[p_index]->setText(QString("%1").arg(bitrates));
                            this->lb_resolution[p_index]->setAlignment(Qt::AlignCenter);

                            width = this->lb_resolution[p_index]->sizeHint().width() + 20;

                            title_width += (15 + width);

                            if(title_width > 680){
                                title_width_resize = 680;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }

                            left = this->lb_title[p_index]->geometry().left() + this->lb_title[p_index]->sizeHint().width() + 15;

                            this->lb_resolution[p_index]->setGeometry(left, 15, width, 16);
                            this->lb_resolution[p_index]->show();
                        }
                        else{
                            if(title_width > 700){
                                title_width_resize = 700;
                                this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                                if(this->lb_title[p_index]->text().contains("…")){
                                    this->lb_title[p_index]->setToolTip(title);
                                    this->lb_title[p_index]->setToolTipDuration(2000);
                                }
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                            else{
                                title_width_resize = title_width;   // - (15 + width);
                                this->lb_title[p_index]->setText(title);
                                this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                            }
                        }
                    }
                    else{
                        if(title_width > 720){
                            title_width_resize = 720;
                            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(title, title_width_resize, this->lb_title[p_index]->font()));
                            if(this->lb_title[p_index]->text().contains("…")){
                                this->lb_title[p_index]->setToolTip(title);
                                this->lb_title[p_index]->setToolTipDuration(2000);
                            }
                            this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                        }
                        else{
                            title_width_resize = title_width;   // - (15 + width);
                            this->lb_title[p_index]->setText(title);
                            this->lb_title[p_index]->setGeometry(180, 10, title_width_resize, 25);
                        }
                    }
                }

                if(artist_width > 650){
                    this->lb_artist[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 650, this->lb_artist[p_index]->font()));
                    if(this->lb_artist[p_index]->text().contains("…")){
                        this->lb_artist[p_index]->setToolTip(artist);
                        this->lb_artist[p_index]->setToolTipDuration(2000);
                    }
                    this->lb_artist[p_index]->setGeometry(180, 34, 650, 25);
                }
                else{
                    this->lb_artist[p_index]->setText(artist);
                    this->lb_artist[p_index]->setGeometry(180, 34, artist_width, 25);
                }
            }

            QLabel *tmp_album = new QLabel();
            tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
            tmp_album->setText(album);

            int album_title_width = 0;
            album_title_width = tmp_album->sizeHint().width();

            if(album_title_width > 220){
                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_album_set = "";

                tmp_split = album;
                splitToken = tmp_split.split(" ");

                tmp_album->setText("");
                int i = 0;
                if(splitToken.size() > 1){

                    for(i = 0; i < splitToken.count(); i++){
                        if(i == 0){
                            tmp_album_set = splitToken.at(i);
                        }
                        else{
                            tmp_album_set += " " + splitToken.at(i);
                        }
                        tmp_album->setText(tmp_album_set);

                        if(tmp_album->sizeHint().width() >= 220){
                            tmp_album->setText("");
                            tmp_album_set.replace(splitToken.at(i), "");
                            break;
                        }
                    }
                }

                tmp_album->setText("");
                tmp_album->setText(tmp_album_set);

                album_title_width = tmp_album->sizeHint().width() + 220;

                QString strAlbum = album;
                QLabel *tmpAlbum = new QLabel();
                tmpAlbum->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
                tmpAlbum->setText(strAlbum.replace(tmp_album_set, ""));

                int tmpAlbum_width = tmpAlbum->sizeHint().width();

                if(tmpAlbum_width >= 220){
                    this->lb_album[p_index]->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, this->lb_album[p_index]->font()));
                    if(this->lb_album[p_index]->text().contains("…")){
                        this->lb_album[p_index]->setToolTip(album);
                        this->lb_album[p_index]->setToolTipDuration(2000);
                    }
                }
                else{
                    this->lb_album[p_index]->setText(album);
                }
                this->lb_album[p_index]->setGeometry(997, 14, 220, 40);
            }
            else{
                this->lb_album[p_index]->setText(album);
                this->lb_album[p_index]->setGeometry(997, 24, 220, 20);
            }
        }
        else if(this->list_type == "ARTIST"){
            this->lb_title[p_index]->setText(GSCommon::getTextCutFromLabelWidth(artist, 720, this->lb_title[p_index]->font()));
            if(this->lb_title[p_index]->text().contains("…")){
                this->lb_title[p_index]->setToolTip(artist);
                this->lb_title[p_index]->setToolTipDuration(2000);
            }
            this->lb_title[p_index]->setGeometry(180, 10, 720, 49);
        }
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : User Event Handler (slots)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] Album 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzRecentlyListDelete::slot_applyResult_albumlists(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_data = flag_lastPage;
            this->flagReqMore_data = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_drawCount == 0){
                this->jsonArr_list_data = QJsonArray();
                this->list_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_drawCount;

            ProcJsonEasy::mergeJsonArray(this->jsonArr_list_data, jsonArr_dataToPlay);

            if(start_index == 0){
                this->flag_widget_visible = true;
                this->request_more_listDraw();
            }

            if(flag_lastPage == false){
                this->request_more_listData();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_totalCount <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] Playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzRecentlyListDelete::slot_applyResult_playlistLists(const QList<roseHome::PlaylistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_data = flag_lastPage;
            this->flagReqMore_data = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_drawCount == 0){
                this->jsonArr_list_data = QJsonArray();
                this->list_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_drawCount;

            ProcJsonEasy::mergeJsonArray(this->jsonArr_list_data, jsonArr_dataToPlay);

            if(start_index == 0){
                this->flag_widget_visible = true;
                this->request_more_listDraw();
            }

            if(flag_lastPage == false){
                this->request_more_listData();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_totalCount <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] Playlist 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzRecentlyListDelete::slot_applyResult_rosetubelists(const QJsonArray &jsonArr_dataToPlay, const int &totalCnt, const bool flag_lastPage){

        if(jsonArr_dataToPlay.size() > 0){
            this->flag_lastPage_data = flag_lastPage;
            this->flagReqMore_data = false;

            if(jsonArr_dataToPlay.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_drawCount == 0){
                this->jsonArr_list_data = QJsonArray();
                this->list_totalCount = totalCnt;
            }

            int start_index = this->list_drawCount;

            ProcJsonEasy::mergeJsonArray(this->jsonArr_list_data, jsonArr_dataToPlay);

            if(start_index == 0){
                this->flag_widget_visible = true;
                this->request_more_listDraw();
            }

            if(flag_lastPage == false){
                this->request_more_listData();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_totalCount <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzRecentlyListDelete::slot_applyResult_tracklists(const QList<roseHome::TrackItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_data = flag_lastPage;
            this->flagReqMore_data = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_drawCount == 0){
                this->jsonArr_list_data = QJsonArray();
                this->list_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_drawCount;

            ProcJsonEasy::mergeJsonArray(this->jsonArr_list_data, jsonArr_dataToPlay);

            if(start_index == 0){
                this->flag_widget_visible = true;
                this->request_more_listDraw();
            }

            if(flag_lastPage == false){
                this->request_more_listData();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_totalCount <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Track_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    /**
     * @brief [slot] Track 데이터를 받아서 처리함
     * @param list_data
     */
    void QobuzRecentlyListDelete::slot_applyResult_artistlists(const QList<roseHome::ArtistItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        if(list_data.size() > 0){
            this->flag_lastPage_data = flag_lastPage;
            this->flagReqMore_data = false;

            if(list_data.size() == 0){
                ContentLoadingwaitingMsgHide();
                return;
            }

            if(this->list_drawCount == 0){
                this->jsonArr_list_data = QJsonArray();
                this->list_totalCount = list_data.at(0).totalCount;
            }

            int start_index = this->list_drawCount;

            ProcJsonEasy::mergeJsonArray(this->jsonArr_list_data, jsonArr_dataToPlay);

            if(start_index == 0){
                this->flag_widget_visible = true;
                this->request_more_listDraw();
            }

            if(flag_lastPage == false){
                this->request_more_listData();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            if(this->list_totalCount <= 0){
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Artist_NoData);
                noData_widget->setFixedSize(1500, 300);

                this->box_main_contents->addWidget(noData_widget);
            }
        }
    }


    void QobuzRecentlyListDelete::slot_thumbnailDownload_list(){

        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();

        if(tmp_index >= 0 && tmp_index < this->jsonArr_list_data.size()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
            if(flagLoaded){
                QPixmap pixmapIMG = QPixmap(QSize(60, 60));
                if(this->list_type == "ARTIST"){
                    pixmapIMG.fill(Qt::transparent);
                }
                else{
                    pixmapIMG.fill(Qt::black);
                }

                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                if(this->list_type == "ARTIST"){
                    tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                }
                else{
                    tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                QPainter painter (&pixmapIMG);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
                painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

                QPainterPath path = QPainterPath();
                if(this->list_type == "ARTIST"){
                    path.addRoundedRect(0, 0, 60, 60, 30, 30);
                }
                else{
                    path.addRoundedRect(0, 0, 60, 60, 2, 2);
                }

                int leftValue = (60 - tmp_pixmap.width()) / 2;
                int topValue = (60 - tmp_pixmap.height()) / 2;

                painter.setClipPath(path);
                painter.drawPixmap(leftValue, topValue, tmp_pixmap);
                painter.end();

                this->label_Img_recently[tmp_index]->setPixmap(pixmapIMG);
            }
            else{
                if(this->list_type == "ARTIST"){
                    QString default_path = ":/images/rosehome/artist_def.png";

                    if(image.load(default_path)){
                        QPixmap pixmapIMG = QPixmap(QSize(60, 60));
                        pixmapIMG.fill(Qt::transparent);

                        QPixmap tmp_pixmap;
                        tmp_pixmap = tmp_pixmap.fromImage(image);
                        tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

                        QPainter painter (&pixmapIMG);
                        painter.setRenderHint(QPainter::Antialiasing, true);
                        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
                        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

                        QPainterPath path = QPainterPath();
                        if(this->list_type == "ARTIST"){
                            path.addRoundedRect(0, 0, 60, 60, 30, 30);
                        }
                        else{
                            path.addRoundedRect(0, 0, 60, 60, 2, 2);
                        }

                        int leftValue = (60 - tmp_pixmap.width()) / 2;
                        int topValue = (60 - tmp_pixmap.height()) / 2;

                        painter.setClipPath(path);
                        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
                        painter.end();

                        this->label_Img_recently[tmp_index]->setPixmap(pixmapIMG);
                    }
                }
            }
        }

        fileDownloader->deleteLater();
    }


    void QobuzRecentlyListDelete::slot_clickedSelected(int state){

        this->selectIds.clear();

        if(state == Qt::Checked){
            for(int i = 0; i < this->listWidget_recentlyList->count(); i++){
                QString splitIndex = this->listWidget_recentlyList->item(i)->whatsThis();
                QStringList listIndex = splitIndex.split(",");
                QStringList strIndex = listIndex.at(1).split(":");
                int idx = strIndex.at(1).toInt();

                this->checkbox_Selected[idx]->setCheckState(Qt::Checked);
            }
        }
        else{
            for(int i = 0; i < this->listWidget_recentlyList->count(); i++){
                QString splitIndex = this->listWidget_recentlyList->item(i)->whatsThis();
                QStringList listIndex = splitIndex.split(",");
                QStringList strIndex = listIndex.at(1).split(":");
                int idx = strIndex.at(1).toInt();

                this->checkbox_Selected[idx]->setCheckState(Qt::Unchecked);
            }
        }
    }


    void QobuzRecentlyListDelete::slot_clicked_btnSelectAll(){

        if(this->checkbox_selectAll->checkState() == Qt::Checked){
            this->checkbox_selectAll->setCheckState(Qt::Unchecked);
        }
        else if(this->checkbox_selectAll->checkState() == Qt::Unchecked){
            this->checkbox_selectAll->setCheckState(Qt::Checked);
        }
    }


    void QobuzRecentlyListDelete::slot_clicked_Delete(){

        this->selectIds.clear();

        for(int i = 0; i < this->listWidget_recentlyList->count(); i++){
            QString splitIndex = this->listWidget_recentlyList->item(i)->whatsThis();
            QStringList listIndex = splitIndex.split(",");
            QStringList strIndex = listIndex.at(1).split(":");
            int idx = strIndex.at(1).toInt();
            QStringList strIds = listIndex.at(2).split(":");
            int ids = strIds.at(1).toInt();

            if(this->checkbox_Selected[idx]->checkState() == Qt::Checked){
                this->selectIds.append(QString("%1").arg(ids));
            }
        }

        if(this->selectIds.count() == 0){

            if(this->selectDelete_DialogConfirm("None")){
                emit this->linker->signal_addPlaylistFinish();
            }
        }
        else if(this->selectDelete_DialogConfirm("")){
            if(this->list_type == "ALBUM"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "album", "", "Ids", this->selectIds);
            }
            else if(this->list_type == "PLAYLIST"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "playlist", "", "Ids", this->selectIds);
            }
            else if(this->list_type == "YOUTUBE"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("YOUTUBE", "track", "YOUTUBE", "Ids", this->selectIds);
            }
            else if(this->list_type == "TRACK"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "track", "", "Ids", this->selectIds);
            }
            else if(this->list_type == "ARTIST"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "artist", "", "Ids", this->selectIds);
            }
        }
    }


    void QobuzRecentlyListDelete::slot_clicked_DeleteAll(){

        if(this->listWidget_recentlyList->count() == 0){

            if(this->selectDelete_DialogConfirm("None")){
                emit this->linker->signal_addPlaylistFinish();
            }
        }
        else if(this->selectDelete_DialogConfirm("All")){
            if(this->list_type == "ALBUM"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "album", this->filter_type, "All", this->selectIds);
            }
            else if(this->list_type == "PLAYLIST"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "playlist", this->filter_type, "All", this->selectIds);
            }
            else if(this->list_type == "YOUTUBE"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("YOUTUBE", "track", this->filter_type, "All", this->selectIds);
            }
            else if(this->list_type == "TRACK"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "track", this->filter_type, "All", this->selectIds);
            }
            else if(this->list_type == "ARTIST"){
                // request HTTP API
                roseHome::ProcCommon *proc = new roseHome::ProcCommon(this);
                connect(proc, &roseHome::ProcCommon::completeDel_recently_list, this, &QobuzRecentlyListDelete::slot_applyResult_getDeleteList);
                proc->request_rose_recently_delete("QOBUZ", "artist", this->filter_type, "All", this->selectIds);
            }
        }
    }


    bool QobuzRecentlyListDelete::selectDelete_DialogConfirm(const QString type){

        DialogConfirm *dlg = new DialogConfirm(this);
        dlg->setTitle(tr("Delete Notice"));
        if(type == "None"){
            dlg->setText(tr("There are no lists to delete."));
        }
        else if(type == "All"){
            dlg->setText(tr("Are you sure you want to delete all lists?"));
        }
        else{
            dlg->setText(tr("Are you sure you want to delete selected lists?"));
        }
        dlg->setFixedSize(600, 550);

        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
        int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
        int left = global.left_mainwindow + ((latestWidth - dlg->sizeHint().width()) / 2);
        int top = global.top_mainwindow + ((latestHeight - dlg->sizeHint().height()) / 2);
        dlg->setGeometry(left, top, 0, 0);

        int result = dlg->exec();

        if(result==QDialog::Accepted){
            return result;
        }

        delete dlg;

        return result;
    }

    void QobuzRecentlyListDelete::slot_applyResult_getDeleteList(const bool flag){

        if(flag == true){
            emit this->linker->signal_addPlaylistFinish();
        }
    }


    /**
     * @brief QobuzRecentlyListDelete::resizeEvent
     * @param event
     */
    void QobuzRecentlyListDelete::resizeEvent(QResizeEvent *event){
        Q_UNUSED(event);

        int left = 0;

        this->widget_mainTitle->resize(global.LmtCnt, 69);

        left = global.LmtCnt - (this->label_deleteAll->sizeHint().width() + 53);
        this->btn_deleteAll->setGeometry(left, 0, this->label_deleteAll->sizeHint().width(), 50);

        left = global.LmtCnt - (this->label_deleteAll->sizeHint().width() + 53 + this->label_delete->sizeHint().width() + 50);
        this->btn_delete->setGeometry(left, 0, this->label_delete->sizeHint().width(), 50);

        left = global.LmtCnt - (this->label_deleteAll->sizeHint().width() + 53 + this->label_delete->sizeHint().width() + 50 + this->label_selectAll->sizeHint().width() + 50 + 50);
        this->btn_selectAll->setGeometry(left, 0, this->label_selectAll->sizeHint().width() + 50, 50);
    }
}
