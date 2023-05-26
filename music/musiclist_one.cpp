#include "music/Musiclist_one.h"

#include "roseHome/ProcCommon_forRosehome.h"

#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"

#include "login/dialog_playlist_onRose.h"

#include "widget/toastmsg.h"

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>


namespace music {

    const QString ICON_PATH___favorite_off = ":/images/rosehome/heart_ico0.png";
    const QString ICON_PATH___favorite_on = ":/images/rosehome/heart_ico1.png";
    const QString ICON_PATH___favorite_on2 = ":/images/rosehome/heart_ico2.png";
    const QString ICON_PATH___favorite_on3 = ":/images/rosehome/heart_ico3.png";

    const int SECTION_FOR_MORE_POPUP___tracks = 1;

    const int GET_MAX_ITEM_SIZE___ONCE = 100;


    /**
     * @brief MusicList_One::MusicList_One : 음악-분류-전체
     * @param parent
     * @note 검색-트랙-View All 에서도 사용
     */
    MusicList_One::MusicList_One(QWidget *parent) : roseHome::AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_musicviewAll, parent)
    {
        this->linker = Linker::getInstance();
        this->settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

        setUIControl();
    }


    /**
         * @brief MusicList_One:: ~MusicList_One
         */
    MusicList_One::~MusicList_One(){

        this->deleteLater();
    }


    /**
     * @brief MusicList_One::setUIControl : UI 세팅
     */
    void MusicList_One::setUIControl(){

        this->listWidget_tracks = new QListWidget();
        this->listWidget_tracks->setCursor(Qt::PointingHandCursor);
        this->listWidget_tracks->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->listWidget_tracks->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->listWidget_tracks->setStyleSheet("background-color:transparent; border:0px;");

        connect(this->listWidget_tracks, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));

        this->vbox_trackList = new QVBoxLayout();
        this->vbox_trackList->setSpacing(0);
        this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
        this->vbox_trackList->setAlignment(Qt::AlignTop);
        this->vbox_trackList->addWidget(this->listWidget_tracks);

        this->box_main_contents = new QVBoxLayout();
        this->box_main_contents->setSpacing(0);
        this->box_main_contents->setContentsMargins(0, 0, 0, 0);
        this->box_main_contents->addLayout(this->vbox_trackList);

        this->widget_main_contents = new QWidget();
        this->widget_main_contents->setStyleSheet("background:#212121; border:0px;");
        this->widget_main_contents->setLayout(this->box_main_contents);

        this->box_contents->addWidget(widget_main_contents, 0, Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        this->totalCnt = getAlbumDataFromDBforFullsongTotalCount();
    }


    /**
         * @brief MusicList_One::setDataFromDB DB에서 가져와서 전체 데이터 세팅
         */
    void MusicList_One::setDataFromDB(){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        this->strArg = "";

        this->listWidget_tracks->clear();

        this->offsetCnt = 0;
        this->getTrackDataFromDB();

        this->scrollArea_main->verticalScrollBar()->setValue(0);
    }


    /**
         * @brief MusicList_One::setDataFromDB DB에서 가져와서 데이터 세팅
         * @param p_arg
         */
    void MusicList_One::setDataFromDB(const QString &p_arg){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        this->strArg = p_arg;

        this->listWidget_tracks->clear();

        this->offsetCnt = 0;
        this->getTrackDataFromDB();
    }


    void MusicList_One::setDataFromDB_Alpa(const QString &p_arg){

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

        this->strArg = p_arg;

        this->listWidget_tracks->clear();

        this->offsetCnt = 0;
        this->getTrackDataFromDB();
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void MusicList_One::proc_wheelEvent_to_getMoreData(){

        if((this->totalCnt > this->offsetCnt) && (this->offsetCnt == this->listWidget_tracks->count()) && (this->flag_draw == false)
                && (scroll_maxium != this->scrollArea_main->verticalScrollBar()->maximum())
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_draw = true;
            this->scroll_maxium = this->scrollArea_main->verticalScrollBar()->maximum();

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
            this->getTrackDataFromDB();
        }
    }


    /**
         * @brief MusicList_One::getTrackDataFromDB : 트랙 데이터 가져오기
         */
    void MusicList_One::getTrackDataFromDB(){

        // DB 쿼리
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        QJsonArray contentArr;

        if(err.type() == QSqlError::NoError){

            QString strQuery = "";
            strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";

            if(!this->strArg.isEmpty()){

                print_debug();
                qDebug() << "strArg= " << this->strArg;
                strQuery += " WHERE artist LIKE '%" + this->strArg + "%' OR title LIKE '%" + this->strArg + "%'";
                strQuery += " ORDER BY A.title ";
            }
            else{
                QString setWhere = "";
                QString setOrder = "";

                if(this->settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                    setWhere += " ((A.mime_type == 'audio/ape') || (A.mime_type == 'audio/flac') || (A.mime_type == 'audio/dff') || (A.mime_type == 'audio/dsf') || (A.mime_type == 'audio/wav') || (A.mime_type == 'audio/x-wav')) ";
                }
                if(this->settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.bitdepth >= 24) ";
                    }
                    else{
                        setWhere += " AND (A.bitdepth >= 24) ";
                    }
                }
                if(this->settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                    if(this->settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.samplerate >= 88200) ";
                        }
                        else{
                            setWhere += " AND (A.samplerate >= 88200) ";
                        }
                    }
                    else if(this->settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.samplerate >= 176400) ";
                        }
                        else{
                            setWhere += " AND (A.samplerate >= 176400) ";
                        }
                    }
                }
                /*if(this->settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                    if(this->settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 0) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 0) ";
                        }
                    }
                    else{
                        switch (this->settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt()) {
                        case 0:
                        case 6:
                            break;
                        case 1:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 5) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 5) ";
                            }
                            break;
                        case 2:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 4) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 4) ";
                            }
                            break;
                        case 3:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 3) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 3) ";
                            }
                            break;
                        case 4:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 2) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 2) ";
                            }
                            break;
                        case 5:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 1) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 1) ";
                            }
                            break;
                        }
                    }
                }*/
                if(this->settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 0){
                    setOrder += " ORDER BY A.title ASC";
                }
                else if(this->settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 1){
                    setOrder += " ORDER BY A.title DESC";
                }
                else if(this->settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 2){
                    setOrder += " ORDER BY A.artist ";
                }
                else{//c220511
                    setOrder += " ORDER BY A.title ASC";
                }

                if(!setWhere.isEmpty()){
                    strQuery += " WHERE";
                    strQuery += setWhere;
                }
                if(!setOrder.isEmpty()){
                    strQuery += setOrder;
                }
            }

            strQuery += QString(" LIMIT %1 OFFSET ").arg(GET_MAX_ITEM_SIZE___ONCE) + QString::number(this->offsetCnt);

            QVariantList data;
            sqlite->exec(strQuery, data);

            this->fav_start_idx = this->offsetCnt;
            this->offsetCnt += data.size();

            int height = this->offsetCnt * 70;
            this->listWidget_tracks->setFixedHeight(height);

            if(data.size() > 0){

                for(int i = 0 ; i < data.size(); i++){

                    QJsonObject tmp_data = data.at(i).toJsonObject();

                    QString tmp_split = "";
                    QStringList splitToken;
                    QString tmp_type = "";

                    QString tmp_mime = "";
                    int tmp_sample = 0;
                    int tmp_bit = 0;
                    QString snd_mime = "";

                    tmp_sample = tmp_data["samplerate"].toInt();
                    tmp_bit = tmp_data["bitdepth"].toInt();

                    tmp_split = tmp_data["mime_type"].toString();
                    splitToken = tmp_split.split("/");

                    if(splitToken.size() > 1){
                        tmp_type = splitToken.at(1);

                        if(tmp_type == "x-wav" || tmp_type == "wav"){
                            tmp_mime = "WAV";
                        }
                        else if (tmp_type == "wv") {
                            tmp_mime = "WV";
                        }
                        else if(tmp_type == "flac"){
                            tmp_mime = "FLAC";
                        }
                        else if(tmp_type == "dff" || tmp_type == "dsf"){
                            tmp_mime = "DSD";
                        }
                        else if(tmp_type == "mp4"){
                            tmp_mime = "ALAC";
                        }
                        else if(tmp_type == "mpeg"){
                            tmp_mime = "MP3";
                        }
                        else if(tmp_type == "amr"){
                            tmp_mime = "AMR";
                        }
                        else if(tmp_type == "amr-wb"){
                            tmp_mime = "AWB";
                        }
                        else if(tmp_type == "x-ms-wma"){
                            tmp_mime = "WMA";
                        }
                        else if(tmp_type == "ape"){
                            tmp_mime = "APE";
                        }
                        else if(tmp_type == "ogg"){
                            tmp_mime = "OGG";
                        }
                        else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                            tmp_mime = "AAC";
                        }
                        else if(tmp_type == "x-matroska"){
                            tmp_mime = "MKA";
                        }
                        else if(tmp_type == "midi"){
                            tmp_mime = "MIDI";
                        }
                        else if(tmp_type == "sp-midi"){
                            tmp_mime = "SMF";
                        }
                        else if(tmp_type == "imelody"){
                            tmp_mime = "IMY";
                        }
                        else if(tmp_type == "x-mpegurl"){
                            tmp_mime = "M3U8";
                        }
                        else if(tmp_type == "x-scpls"){
                            tmp_mime = "PLS";
                        }
                        else if(tmp_type == "aiff"){
                            tmp_mime = "AIFF";
                        }
                    }

                    if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                            || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                        snd_mime = tmp_mime;
                    }
                    else if(tmp_mime == "DSD"){
                        if(tmp_sample == 88200 || tmp_sample == 2822400){
                            snd_mime = tmp_mime + " 64";
                        }
                        else if(tmp_sample == 176400 || tmp_sample == 5644800){
                            snd_mime = tmp_mime + " 128";
                        }
                        else if(tmp_sample == 352800 || tmp_sample == 11289600){
                            snd_mime = tmp_mime + " 256";
                        }
                        else if(tmp_sample == 705600 || tmp_sample == 22579200){
                            snd_mime = tmp_mime + " 512";
                        }
                        else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                            snd_mime = tmp_mime + " 1024";
                        }
                        else{
                            snd_mime = tmp_mime;
                        }
                    }
                    else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                        if(tmp_bit == 0 || tmp_bit == 16){
                            snd_mime = tmp_mime;
                        }
                        else if(tmp_bit > 16){
                            snd_mime = tmp_mime + QString(" %1").arg(tmp_bit);
                        }
                    }
                    tmp_data.insert("album_mime", snd_mime);

                    this->jsonArr_tracks_toPlay.append(tmp_data);

                    DataPopup *tmp_dataPopup = new DataPopup();
                    tmp_dataPopup->setJsonData(tmp_data);

                    QString tmp_albumImg = "";
                    if(tmp_data.contains("album_art")){
                        tmp_albumImg = tmp_data["album_art"].toString();
                    }

                    QString tmp_strDuration="";
                    int tmp_duration = tmp_data["duration"].toInt();
                    if(tmp_duration >=0){
                        QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                        tmp_strDuration = tmp_time.toString("mm:ss");
                    }

                    QJsonObject json;
                    json.insert("index", this->fav_start_idx + i);
                    json.insert("fileName", tmp_dataPopup->getTitle());
                    json.insert("duration", tmp_strDuration);
                    json.insert("author", tmp_dataPopup->getArtist());
                    json.insert("albumTitle", tmp_dataPopup->getAlbum());
                    json.insert("title", tmp_dataPopup->getTitle());
                    json.insert("isFile", true);
                    json.insert("album_art", tmp_albumImg);
                    json.insert("album_mime", snd_mime);

                    this->setDataTrackInfo_Music_Item(json, this->fav_start_idx + i);

                    int id = ProcJsonEasy::getInt(tmp_data, "id");
                    QJsonObject content;
                    content.insert("clientKey", QString("%1").arg(id));
                    content.insert("macAddress", global.device.getDeviceID());
                    content.insert("path", ProcJsonEasy::getString(tmp_data, "data"));

                    contentArr.append(content);
                }
            }
        }

        sqlite->close();
        delete sqlite;

        QJsonObject rateObj;
        rateObj.insert("contents", contentArr);
        rateObj.insert("mediaType", "MUSIC");
        rateObj.insert("last", false);
        rateObj.insert("offset", 0);
        rateObj.insert("page", 0);
        rateObj.insert("size", 0);
        rateObj.insert("totalCount", 0);
        rateObj.insert("result", false);

        // track favorite check
        roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
        connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &MusicList_One::slot_applyResult_getRating_track);
        proc_fav_track->request_rose_getRating_Track("MUSIC", rateObj);

        this->flag_draw = false;
        ContentLoadingwaitingMsgHide();
    }


    int MusicList_One::getAlbumDataFromDBforFullsongTotalCount(){

        int ret_cnt = 0;

        // DB 쿼리
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            QString strQuery = "";

            // strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth ";
            strQuery += " SELECT A._id AS id,  AI._data AS album_art, A.album_id, A._data AS data ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";

            QString setWhere = "";
            if(!this->strArg.isEmpty()){
                strQuery += " WHERE artist LIKE '%" + this->strArg + "%' OR title LIKE '%" + this->strArg + "%'";
            }
            else{
                if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                    setWhere += " ((A.mime_type == 'audio/ape') || (A.mime_type == 'audio/flac') || (A.mime_type == 'audio/dff') || (A.mime_type == 'audio/dsf') || (A.mime_type == 'audio/wav') || (A.mime_type == 'audio/x-wav')) ";
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.bitdepth >= 24) ";
                    }
                    else{
                        setWhere += " AND (A.bitdepth >= 24) ";
                    }
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                    if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.samplerate >= 88200) ";
                        }
                        else{
                            setWhere += " AND (A.samplerate >= 88200) ";
                        }
                    }
                    else if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.samplerate >= 176400) ";
                        }
                        else{
                            setWhere += " AND (A.samplerate >= 176400) ";
                        }
                    }
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                    if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 0) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 0) ";
                        }
                    }
                    else{
                        switch (settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt()) {
                        case 0:
                        case 6:
                            break;
                        case 1:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 5) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 5) ";
                            }
                            break;
                        case 2:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 4) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 4) ";
                            }
                            break;
                        case 3:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 3) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 3) ";
                            }
                            break;
                        case 4:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 2) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 2) ";
                            }
                            break;
                        case 5:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 1) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 1) ";
                            }
                            break;
                        }
                    }
                }
            }

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }

            QVariantList data;
            sqlite->exec(strQuery, data);
            ret_cnt = data.size();
            if(ret_cnt == 0){//c220402
                ToastMsg::show(this, "", tr("There is no song."), 2000);
            }
            else{
                ToastMsg::show(this, "", tr("Content is being loaded. Please wait."), 2000);
            }
        }
        sqlite->close();
        delete sqlite;

        return ret_cnt;
    }


    void MusicList_One::setDataTrackInfo_Music_Item(const QJsonObject &trackInfo, const int &idx){

        QString thumbnail = QString("http://%1:").arg(global.device.getDeviceIP()) + QString("%1").arg(global.port_img) + QString("%1").arg(ProcJsonEasy::getString(trackInfo, "album_art"));
        QString title = ProcJsonEasy::getString(trackInfo, "title");
        QString artist = ProcJsonEasy::getString(trackInfo, "author");
        QString album  = ProcJsonEasy::getString(trackInfo, "albumTitle");
        QString mime = ProcJsonEasy::getString(trackInfo, "album_mime");
        QString duration = ProcJsonEasy::getString(trackInfo, "duration");

        QWidget *widget_track_info = new QWidget();
        widget_track_info->setFixedSize(1500, 70);
        widget_track_info->setObjectName("widget_track_info");
        widget_track_info->setStyleSheet("#widget_track_info {background-color:transparent; border-bottom:1px solid #333333; }"
                                         "#widget_track_info:hover{background-color:#333333;} ");

        QImage img_album;
        QPixmap *pixmap_image = new QPixmap();
        if(img_album.load(":/images/def_mus_300.png")){
            *pixmap_image = QPixmap::fromImage(img_album);                                        //이미지를 버퍼에 옮긴다.
            *pixmap_image = pixmap_image->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        this->lb_trackImg[idx] = new QLabel(widget_track_info);
        this->lb_trackImg[idx]->setFixedSize(60, 60);
        this->lb_trackImg[idx]->setGeometry(5, 5, 0, 0);
        this->lb_trackImg[idx]->setStyleSheet("background-color:transparent;");
        this->lb_trackImg[idx]->setPixmap(*pixmap_image);

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", idx);
        fileDownloader->setImageURL(thumbnail);
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownload_tracks()));

        QLabel *tmp_title = new QLabel();
        tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        tmp_title->setText(title);

        int title_width = 0;
        title_width = tmp_title->sizeHint().width();

        QLabel *lb_title = new QLabel(widget_track_info);
        lb_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
        lb_title->setFont(tmp_title->font());
        if(title_width > 720){
            lb_title->setText(GSCommon::getTextCutFromLabelWidth(title, 720, lb_title->font()));
            if(lb_title->text().contains("…")){
                lb_title->setToolTip(title);
                lb_title->setToolTipDuration(2000);
            }
            lb_title->setGeometry(80, 10, 720, 25);
        }
        else{
            lb_title->setText(title);
            lb_title->setGeometry(80, 10, title_width, 25);
        }

        qDebug() << "title_width: "<<title_width;
        qDebug() << "title: "<<title;


        QLabel *tmp_artist = new QLabel();
        tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        tmp_artist->setText(artist);

        int artist_width = 0;
        artist_width = tmp_artist->sizeHint().width();

        QLabel *lb_artist = new QLabel(widget_track_info);
        lb_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
        if(artist_width > 720){
            lb_artist->setText(GSCommon::getTextCutFromLabelWidth(artist, 720, lb_title->font()));
            if(lb_artist->text().contains("…")){
                lb_artist->setToolTip(artist);
                lb_artist->setToolTipDuration(2000);
            }
            lb_artist->setGeometry(80, 34, 720, 25);
        }
        else{
            lb_artist->setText(artist);
            lb_artist->setGeometry(80, 34, artist_width, 25);
        }

        QLabel *lb_bitrate = new QLabel(widget_track_info);
        lb_bitrate->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        lb_bitrate->hide();

        if(!mime.isEmpty()){
            lb_bitrate->setText(mime);
            lb_bitrate->setAlignment(Qt::AlignCenter);

            int width = lb_bitrate->sizeHint().width() + 20;
            int left = 0;
            if(title_width > 720){
                left = lb_title->geometry().left() + lb_title->sizeHint().width() + 15;
            }
            else{
                left = lb_title->geometry().left() + title_width + 15;
            }

            lb_bitrate->setGeometry(left, 15, width, 16);
            lb_bitrate->show();
        }

        QLabel *tmp_album = new QLabel();
        tmp_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        tmp_album->setText(album);

        int album_title_width = 0;
        album_title_width = tmp_album->sizeHint().width();

        QLabel *lb_album = new QLabel(widget_track_info);
        lb_album->setStyleSheet("font-size:14px;font-weight:300;color:#FFFFFF;");
        lb_album->setWordWrap(true);
        lb_album->setGeometry(937, 14, 220, 40);

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
                lb_album->setText(GSCommon::getTextCutFromLabelWidth(album, album_title_width, lb_album->font()));
                if(lb_album->text().contains("…")){
                    lb_album->setToolTip(album);
                    lb_album->setToolTipDuration(2000);
                }
            }
            else{
                lb_album->setText(album);
            }
            lb_album->setGeometry(937, 14, 220, 40);
        }
        else{
            lb_album->setText(album);
            lb_album->setGeometry(937, 24, 220, 20);
        }

        QLabel *lb_duration = new QLabel(widget_track_info);
        lb_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
        lb_duration->setGeometry(1231, 24, 65, 20);
        lb_duration->setText(duration);

        QPushButton *btn_play_list_add = GSCommon::getUIBtnImg("btn_play_list_add", ":/images/play_list_add_icon.png", widget_track_info);
        btn_play_list_add->setGeometry(1350, 10, 50, 50);
        btn_play_list_add->setCursor(Qt::PointingHandCursor);
        btn_play_list_add->setProperty("idx", idx);

        connect(btn_play_list_add, &QPushButton::clicked, this, &MusicList_One::slot_clicked_btnPlaylistAdd);

        this->btn_list_fav[idx] = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, widget_track_info);
        this->btn_list_fav[idx]->setGeometry(1400, 6, 50, 58);
        this->btn_list_fav[idx]->setCursor(Qt::PointingHandCursor);
        this->btn_list_fav[idx]->setProperty("idx", idx);

        connect(this->btn_list_fav[idx], &QPushButton::clicked, this, &MusicList_One::slot_clicked_btnFavorite);

        this->track_star_count[idx] = 0;

        QPushButton *btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", widget_track_info);
        btn_menu->setGeometry(1450, 6, 50, 58);
        btn_menu->setCursor(Qt::PointingHandCursor);
        btn_menu->setProperty("idx", idx);

        connect(btn_menu, &QPushButton::clicked, this, &MusicList_One::slot_clicked_btnMenu);

        QListWidgetItem *item = new QListWidgetItem();
        item->setWhatsThis(QString("IDX:%1").arg(idx));
        item->setSizeHint(QSize(90, 70));

        this->listWidget_tracks->addItem(item);

        this->listWidget_tracks->setItemWidget(item, widget_track_info);
    }


    void MusicList_One::slot_thumbnailDownload_tracks(){

        FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
        int tmp_index = fileDownloader->property("index").toInt();

        if(tmp_index >= 0 && tmp_index < 999999){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());
            if(flagLoaded){
                QPixmap pixmapIMG = QPixmap(QSize(60, 60));
                pixmapIMG.fill(Qt::transparent);

                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                QPainter painter (&pixmapIMG);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
                painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

                QPainterPath path = QPainterPath();
                path.addRoundedRect(0, 0, 60, 60, 2, 2);

                int leftValue = (60 - tmp_pixmap.width()) / 2;
                int topValue = (60 - tmp_pixmap.height()) / 2;

                painter.setClipPath(path);
                painter.drawPixmap(leftValue, topValue, tmp_pixmap);
                painter.end();

                this->lb_trackImg[tmp_index]->setPixmap(pixmapIMG);
            }
        }

        fileDownloader->deleteLater();
    }


    void MusicList_One::slot_listwidgetItemPressed(QListWidgetItem *item){

        QString tmpStr = item->whatsThis();

        QStringList strIndex = tmpStr.split(":");
        int idx = strIndex.at(1).toInt();

        QJsonArray trackData = QJsonArray();
        QJsonObject tmp_json;

        if(global.device.getMusicPlayType() == 15){
            trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 15);
        }
        else if(global.device.getMusicPlayType() == 13){
            trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 13);
        }
        else if(global.device.getMusicPlayType() == 12){
            trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 12);
        }
        else if(global.device.getMusicPlayType() == 16){
            trackData.append(this->jsonArr_tracks_toPlay.at(idx).toObject());

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 16);
        }
        else if(global.device.getMusicPlayType() == 17){
            for(int i = idx; i < this->jsonArr_tracks_toPlay.size(); i++){
                trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
            }

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 17);
        }
        else if(global.device.getMusicPlayType() == 18){
            for(int i = idx; i < this->jsonArr_tracks_toPlay.size(); i++){
                trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
            }

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 18);
        }
        else if(global.device.getMusicPlayType() == 19){
            for(int i = idx; i < this->jsonArr_tracks_toPlay.size(); i++){
                trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
            }

            tmp_json.insert("music", trackData);
            tmp_json.insert("musicPlayType", 19);
        }

        if(global.Queue_track_count != 0) {
           emit linker->signal_checkQueue(27, "");

            return;
        }

        emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += trackData.count();     // 220419 queue count

        tmp_json.insert("currentPosition", 0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

        NetworkHttp *network = new NetworkHttp;
        network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
    }


    void MusicList_One::slot_clicked_btnPlaylistAdd(){

        int idx = sender()->property("idx").toInt();

        QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(idx).toObject();

        QJsonArray tmpArr;
        tmpArr.append(tmpTrack);

        QJsonObject json = QJsonObject();
        json.insert("tracks", tmpArr);

        Dialog::Dialog_Playlist_onRose *dialog_playlist = new Dialog::Dialog_Playlist_onRose(Dialog::Dialog_Playlist_onRose::MUSIC, json, this);
        dialog_playlist->request_playlist_fetch();
        connect(dialog_playlist, &Dialog::Dialog_Playlist_onRose::signal_clicked_playlist, this, &MusicList_One::slot_add_rosePlaylist_withMusic);
        int result = dialog_playlist->exec();

        if(result == QDialog::Accepted){
            delete dialog_playlist;
        }
    }


    void MusicList_One::slot_clicked_btnFavorite(){

        int idx = sender()->property("idx").toInt();

        if(this->flag_check_track == false){
            if(this->track_star_count[idx] == 3){
                this->track_star_count[idx] = 0;

                this->track_idx_fav = idx;
                this->flag_track_fav = false;
            }
            else if(this->track_star_count[idx] >= 0 && this->track_star_count[idx] < 3){
                this->track_star_count[idx]++;

                this->track_idx_fav = idx;
                this->flag_track_fav = true;
            }

            QJsonObject trackObj = this->jsonArr_tracks_toPlay.at(idx).toObject();

            QJsonObject data = QJsonObject();
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject track = QJsonObject();
            track.insert("data", data);
            track.insert("playUrl", ProcJsonEasy::getString(trackObj, "data"));
            track.insert("type", "MUSIC");

            QJsonArray tracks = QJsonArray();
            tracks.append(track);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicList_One::slot_applyResult_checkRating_track);
            proc_fav_track->request_rose_checkRating_Track("MUSIC", tracks);
        }

        /*if(this->track_star_count[idx] > 0){
            switch (this->track_star_count[idx]) {
                case 1:
                    this->btn_list_fav[idx]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                case 2:
                    this->btn_list_fav[idx]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                case 3:
                    this->btn_list_fav[idx]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
            }
        }
        else{
            this->btn_list_fav[idx]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                          ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
        }*/
    }


    void MusicList_One::slot_clicked_btnMenu(){

        int idx = sender()->property("idx").toInt();

        QJsonObject track = this->jsonArr_tracks_toPlay.at(idx).toObject();

        QString img = "http://" + global.device.getDeviceIP() + ":" + global.port_img + ProcJsonEasy::getString(track, "album_art");

        // OptMorePopup 띄우기 필요
        OptMorePopup::HeaderData data_header;
        data_header.main_title = ProcJsonEasy::getString(track, "title");
        data_header.sub_title = ProcJsonEasy::getString(track, "artist");
        data_header.imageUrl = img;
        data_header.data_pk = QString("%1").arg(ProcJsonEasy::getInt(track, "id"));
        data_header.type = "MUSIC";     //j220906 share link
        data_header.flagProcStar = false;
        data_header.isShare = false;    //j220906 share link

        // OptMorePopup 띄우기
        this->makeObj_optMorePopup(OptMorePopup::Music_Track, data_header, idx, SECTION_FOR_MORE_POPUP___tracks, true);
    }


    void MusicList_One::slot_applyResult_checkRating_track(const QJsonObject &jsonObj){

        int rcv_id = ProcJsonEasy::getInt(jsonObj, "id");

        if(rcv_id > 0){
            QJsonObject track;
            track.insert("duration", 0);
            track.insert("favorite", false);
            track.insert("id", rcv_id);
            track.insert("ownerId", 0);
            track.insert("sort", 0);
            track.insert("star", 0);
            track.insert("type", "MUSIC");

            QJsonObject ratingInfo;
            ratingInfo.insert("favorite", this->flag_track_fav);
            ratingInfo.insert("star", this->track_star_count[this->track_idx_fav]);
            ratingInfo.insert("thumbup", false);
            ratingInfo.insert("type", "MUSIC");

            QJsonObject json;
            json.insert("track", track);
            json.insert("ratingInfo", ratingInfo);

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_track = new roseHome::ProcCommon(this);
            connect(proc_fav_track, &roseHome::ProcCommon::completeReq_rating_track, this, &MusicList_One::slot_applyResult_getRating_track);
            proc_fav_track->request_rose_setRating_Track(json, this->flag_track_fav, this->track_star_count[this->track_idx_fav]);
        }
        else if(rcv_id == 0){
            QJsonObject tmpTrack = this->jsonArr_tracks_toPlay.at(this->track_idx_fav).toObject();

            QJsonObject data;
            data.insert("macAddress", global.device.getDeviceID());
            data.insert("trackId", 0);

            QJsonObject track;
            track.insert("duration", ProcJsonEasy::getInt(tmpTrack, "duration"));
            track.insert("data", data);
            track.insert("favorite", true);
            track.insert("id", ProcJsonEasy::getInt(tmpTrack, "id"));
            track.insert("star", 1);
            track.insert("title", ProcJsonEasy::getInt(tmpTrack, "title"));
            track.insert("type", "MUSIC");

            // request HTTP API - get favorite for Rose Server
            roseHome::ProcCommon *proc_fav_album = new roseHome::ProcCommon(this);
            connect(proc_fav_album, &roseHome::ProcCommon::completeCheck_rating_track, this, &MusicList_One::slot_applyResult_addRating_track);
            proc_fav_album->request_rose_addRating_Track(track);

            this->flag_check_track = true;
        }
    }


    void MusicList_One::slot_applyResult_addRating_track(const QJsonObject &jsonObj){

        if(jsonObj.contains("flagOk")){
            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
        else{
            this->flag_check_track = false;
        }
    }


    void MusicList_One::slot_applyResult_getRating_track(const QJsonArray &contents){

        if(contents.size() > 0){
            QJsonObject tmpInfo = contents.at(0).toObject();

            if(tmpInfo.contains("star")){
                for(int i = 0; i < contents.size(); i++){
                    QJsonObject track_info = contents.at(i).toObject();

                    int star = ProcJsonEasy::getInt(track_info, "star");

                    this->track_star_count[this->fav_start_idx + i] = star;

                    if(this->track_star_count[this->fav_start_idx + i] > 0){
                        switch (this->track_star_count[this->fav_start_idx + i]) {
                            case 1:
                                this->btn_list_fav[this->fav_start_idx + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                            case 2:
                                this->btn_list_fav[this->fav_start_idx + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                            case 3:
                                this->btn_list_fav[this->fav_start_idx + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                              ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        }
                    }
                    else{
                        this->btn_list_fav[this->fav_start_idx + i]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                      ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                    }
                }
            }
            else if(tmpInfo.value("flagOk").toBool()){
                if(this->track_star_count[this->track_idx_fav] > 0){
                    switch (this->track_star_count[this->track_idx_fav]) {
                        case 1:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        case 2:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on2 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                        case 3:
                            this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                          ICON_PATH___favorite_on3 + "'); background-repeat:no-repeat; background-position:center center; } ");  break;
                    }
                }
                else{
                    this->btn_list_fav[this->track_idx_fav]->setStyleSheet("#btn_list_fav { background-color:rgba(0,0,0,0); background-image: url('" +
                                                  ICON_PATH___favorite_off + "'); background-repeat:no-repeat; background-position:center center; } ");
                }
            }

            if(this->flag_check_track == true){
                this->flag_check_track = false;
            }
        }
    }


    void MusicList_One::slot_add_rosePlaylist_withMusic(const int &idx, const QJsonObject &dataObj){

        QString view_type = "";
        if(idx < 0){
            view_type = "create";
        }
        else{
            view_type = "add";
        }

        QJsonObject jsonObj_move = dataObj;
        jsonObj_move.insert("view_type", view_type);
        jsonObj_move.insert("type", "MUSIC");
        jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_M_ADDPLAYLIST);

        emit linker->signal_clickedMovePage(jsonObj_move);
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void MusicList_One::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        if(section == SECTION_FOR_MORE_POPUP___tracks){

            QJsonArray trackData = QJsonArray();
            QJsonObject tmp_json;

            if(clickMode == OptMorePopup::ClickMode::Share){//c220823
                print_debug();
                setUIShare();
                qDebug() << "this->shareLink="<<this->shareLink;

            }
            if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 15);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 13);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 12);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 15);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne){
                trackData.append(this->jsonArr_tracks_toPlay.at(index).toObject());

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 16);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere){
                for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                    trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 17);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty){
                for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                    trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 18);
            }
            else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last){
                for(int i = index; i < this->jsonArr_tracks_toPlay.size(); i++){
                    trackData.append(this->jsonArr_tracks_toPlay.at(i).toObject());
                }

                tmp_json.insert("music", trackData);
                tmp_json.insert("musicPlayType", 19);
            }

            tmp_json.insert("currentPosition", 0);
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            if(global.Queue_track_count != 0) {
                emit linker->signal_checkQueue(27, "");
                return;
            }

            emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += trackData.count();     // 220419 queue count

            NetworkHttp *network = new NetworkHttp;
            network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
        }
    }
}
