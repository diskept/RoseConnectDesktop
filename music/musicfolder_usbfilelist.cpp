#include "music/musicfolder_usbfilelist.h"
#include "music/proc_uploadthumbnail.h"

#include "common/filedownloader.h"
#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/rosesettings.h"
#include "common/sqlitehelper.h"

#include "data/datapopup.h"

#include "login/dialogadd_favoritefolder.h"

#include "widget/optionpopup.h"
#include "widget/toastmsg.h"

#include <QSettings>


const int HTTP_NETWORK_FOLDER_USB_FILELIST = 99;
const int HTTP_NETWORK_PLAY = 98;
const int HTTP_NETWORK_FAVORITE_GET = 97;
const int HTTP_NETWORK_FAVORITE_ADD = 96;
const int HTTP_NETWORK_FAVORITE_DELETE = 95;

/**
 * @brief MusicFolder_USBFileList::MusicFolder_USBFileList : 음악-폴더-USB 상세(파일 목록)
 * @param parent
 */
MusicFolder_USBFileList::MusicFolder_USBFileList(QWidget *parent, bool p_flagIsMusic)
    : QWidget(parent)
    , flagIsMusic(p_flagIsMusic)
{
    setInit();
    setUIControl();
}


/**
 * @brief MusicFolder_USBFileList::setInit : 초기 세팅
 */
void MusicFolder_USBFileList::setInit(){
    setInitMediaSrcData();

    linker = Linker::getInstance();
    listFileData = new QList<DataNetworkFolder*>();
    delegate = new USBFileDelegate(this);
    if(!flagIsMusic){
        delegate->setVideoFileMode();
    }

    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));

    procAdd_playlist = new Proc_AddTackInPlayList(this);
    connect(procAdd_playlist, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(procAdd_playlist, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));
}

/**
 * @brief MusicFolder_USBFileList::setInitMediaSrcData : 폴더 목록 구성에 필요한 media src 데이터 세팅
 */
void MusicFolder_USBFileList::setInitMediaSrcData(){

    if(flagIsMusic){
        jsonArrMediaSrc.append("aac");
        jsonArrMediaSrc.append("aac-adts");
        jsonArrMediaSrc.append("act");
        jsonArrMediaSrc.append("aif");
        jsonArrMediaSrc.append("aiff");
        jsonArrMediaSrc.append("alac");
        jsonArrMediaSrc.append("amr");
        jsonArrMediaSrc.append("amr-wb");
        jsonArrMediaSrc.append("ape");
        jsonArrMediaSrc.append("cda");
        jsonArrMediaSrc.append("dff");
        jsonArrMediaSrc.append("dsf");
        jsonArrMediaSrc.append("ec3");
        jsonArrMediaSrc.append("e-ec3");
        jsonArrMediaSrc.append("flac");
        jsonArrMediaSrc.append("m4a");
        jsonArrMediaSrc.append("m4b");
        jsonArrMediaSrc.append("m4p");
        jsonArrMediaSrc.append("midi");
        jsonArrMediaSrc.append("mp1");
        jsonArrMediaSrc.append("mp2");
        jsonArrMediaSrc.append("mp3");
        jsonArrMediaSrc.append("mpc");
        jsonArrMediaSrc.append("mpga");
        jsonArrMediaSrc.append("imelody");
        jsonArrMediaSrc.append("iso");
        jsonArrMediaSrc.append("sp-midi");
        jsonArrMediaSrc.append("ogg");
        jsonArrMediaSrc.append("wav");
        jsonArrMediaSrc.append("wave");
        jsonArrMediaSrc.append("wma");
        jsonArrMediaSrc.append("wv");
        jsonArrMediaSrc.append("x-matroska");
        jsonArrMediaSrc.append("x-ms-wma");
        jsonArrMediaSrc.append("x-mpegurl");
        jsonArrMediaSrc.append("x-wav");
        jsonArrMediaSrc.append("x-scpls");
    }
    else{
        jsonArrMediaSrc.append("3gp");
        jsonArrMediaSrc.append("3g2");
        jsonArrMediaSrc.append("asf");
        jsonArrMediaSrc.append("avchd");
        jsonArrMediaSrc.append("avi");
        jsonArrMediaSrc.append("dat");
        jsonArrMediaSrc.append("divx");
        jsonArrMediaSrc.append("evo");
        jsonArrMediaSrc.append("flv");
        jsonArrMediaSrc.append("f4v");
        jsonArrMediaSrc.append("h264");
        jsonArrMediaSrc.append("k3g");
        jsonArrMediaSrc.append("mkv");
        jsonArrMediaSrc.append("mov");
        jsonArrMediaSrc.append("mp4");
        jsonArrMediaSrc.append("mpe");
        jsonArrMediaSrc.append("mpg");
        jsonArrMediaSrc.append("mts");
        jsonArrMediaSrc.append("m2p");
        jsonArrMediaSrc.append("m2t");
        jsonArrMediaSrc.append("m2ts");
        jsonArrMediaSrc.append("m2v");
        jsonArrMediaSrc.append("m4v");
        jsonArrMediaSrc.append("mxf");
        jsonArrMediaSrc.append("ogg");
        jsonArrMediaSrc.append("ogm");
        jsonArrMediaSrc.append("ogv");
        jsonArrMediaSrc.append("ots");
        jsonArrMediaSrc.append("rm");
        jsonArrMediaSrc.append("rmvb");
        jsonArrMediaSrc.append("swf");
        jsonArrMediaSrc.append("tp");
        jsonArrMediaSrc.append("trp");
        jsonArrMediaSrc.append("ts");
        jsonArrMediaSrc.append("vob");
        jsonArrMediaSrc.append("vp6");
        jsonArrMediaSrc.append("wbem");
        jsonArrMediaSrc.append("webm");
        jsonArrMediaSrc.append("wmv");
    }
    /*
    model = new NetworkFileSystemModel(this);
    model->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    model->setNameFilters(listNameFilter);
    model->setNameFilterDisables(false);
    model->setReadOnly(false);

    list_fileLoadStruct = new QList<FileLoadStruct>();

    delegate = new FileDelegate(this);
    if(!flagIsMusic){
        delegate->setVideoFileMode();
    }

    procAdd_playlist = new Proc_AddTackInPlayList(this);
    connect(procAdd_playlist, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(procAdd_playlist, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));

    connect(model, SIGNAL(directoryLoaded(QString)), this, SLOT(slot_directoryLoaded(QString)));
    connect(model, SIGNAL(signal_reqThumbnail(int,QString,QString)), this, SLOT(slot_reqThumbnail(int,QString,QString)));
    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));

    // 음악인 경우에만 favorite signal connect
    if(flagIsMusic){
        connect(model, SIGNAL(signal_reqFavorites(int)), this, SLOT(slot_reqFavorites(int)));
    }
    */
}


/**
 * @brief MusicFolder_USBFileList::setUIControl : UI 세팅
 */
void MusicFolder_USBFileList::setUIControl(){

    this->select_index = 0;
    this->topDirPath = new Folder_TopDirPath(this);

    QPixmap pixmap_play(":/images/icon_list_allplay.png");
    this->btn_play = new QPushButton;
    this->btn_play->setIcon(pixmap_play);
    this->btn_play->setIconSize(pixmap_play.rect().size());
    this->btn_play->setText(tr("Play All"));
    //btn_play->setText(tr("전체재생"));
    this->btn_play->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->btn_play->setCursor(Qt::PointingHandCursor);


    QPixmap pixmap_shuffle(":/images/icon_list_shuffle.png");
    this->btn_shuffle = new QPushButton;
    this->btn_shuffle->setIcon(pixmap_shuffle);
    this->btn_shuffle->setIconSize(pixmap_play.rect().size());
    this->btn_shuffle->setText(tr("Shuffle Play"));
    //btn_shuffle->setText(tr("셔플재생"));
    this->btn_shuffle->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->btn_shuffle->setCursor(Qt::PointingHandCursor);

    //if(!flagIsMusic){
        this->btn_play->hide();
        this->btn_shuffle->hide();
    //}

    QHBoxLayout *box_option = new QHBoxLayout;
    box_option->setAlignment(Qt::AlignLeft);
    box_option->setSpacing(30);
    box_option->setContentsMargins(0,10,0,10);
    box_option->addWidget(btn_play);
    box_option->addWidget(btn_shuffle);


    listWidget = new QListWidget(this);
    listWidget->setItemDelegate(delegate);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("border:none;");

    QVBoxLayout *boxContents = new QVBoxLayout();
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(33,0,28,0);
    boxContents->setAlignment(Qt::AlignTop);
    boxContents->addLayout(box_option);
    boxContents->addWidget(listWidget);

    QVBoxLayout *box = new QVBoxLayout;
    box->setSpacing(0);
    box->setContentsMargins(0,0,0,0);
    box->setAlignment(Qt::AlignTop);
    box->addWidget(topDirPath);
    box->addLayout(boxContents);

    setLayout(box);

    // 커넥트
    connect(topDirPath, SIGNAL(signal_changedDirPath(QString,QString)), this, SLOT(slot_changedRootPath(QString,QString)));
    connect(this->btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlayAll()));
    connect(this->btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedPlayShuffle()));

}


/**
 * @brief MusicFolder_USBFileList::setData : 데이터 세팅
 * @param p_jsonData
 */
void MusicFolder_USBFileList::setData(const QJsonObject &p_jsonData){

    listFileData->clear();
    listWidget->clear();

    data = new DataNetworkFolder(this);
    data->setData(p_jsonData);

    topDirPath->clearDirName();
    topDirPath->addDirName(data->getName(), data->getPath());

    requestFileList();
}

/**
 * @brief MusicFolder_USBFileList::appendFileUI : 파일 UI 추가
 * @param p_jsonObject QJsonObject 파일정보
 */
void MusicFolder_USBFileList::appendFileUI(const QJsonObject &p_jsonObject){

    DataNetworkFolder *data = new DataNetworkFolder(this);
    data->setData(p_jsonObject);

    listFileData->append(data);

    if(data->getIsDir()){
        QJsonObject json;
        json.insert("index",listWidget->count());
        json.insert("filePath", data->getPath());
        json.insert("fileName", data->getName());
        json.insert("duration","");
        json.insert("author","");
        json.insert("albumTitle","");
        json.insert("album_art","");
        json.insert("title", data->getName());
        json.insert("isFile", false);


        QListWidgetItem *item1 = new QListWidgetItem;
        item1->setData(Qt::UserRole, json);
        item1->setData(Qt::DecorationRole, QPixmap(":images/icon_folder_bic.png"));

        listWidget->addItem(item1);
    }else{

        QJsonObject jsonDBData = getFileInfoFromDB(data->getPath());
        QString tmp_albumImg = jsonDBData["album_art"].toString();

        QJsonObject json;
        json.insert("index",listWidget->count());
        json.insert("filePath", data->getPath());
        json.insert("fileName", data->getName());
        json.insert("duration",GSCommon::convertTimeFormat(jsonDBData["duration"].toInt()));
        json.insert("author",jsonDBData["artist"].toString());
        json.insert("albumTitle",jsonDBData["album"].toString());
        json.insert("title", jsonDBData["title"].toString());
        json.insert("isFile", true);
        json.insert("album_art", tmp_albumImg);
        json.insert("favorites", false);
        json.insert("id", jsonDBData["id"].toInt(0));


        QListWidgetItem *item1 = new QListWidgetItem;
        item1->setData(Qt::UserRole, json);
        item1->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));

        listWidget->addItem(item1);

        FileDownloader *fileDownloader = new FileDownloader(this);
        fileDownloader->setProperty("index", listWidget->count()-1);
        fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
        connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));


        requestGetTrackFavorites(listWidget->count()-1, data->getPath());
    }

}

/**
 * @brief MusicFolder_USBFileList::getFileInfoFromDB : Get Thumbnail PATH FROM DB
 * @param p_fileName QString 파일 패스
 * @return QJsonObject fileInfo
 */
QJsonObject MusicFolder_USBFileList::getFileInfoFromDB(const QString &p_path){

    QJsonObject json;

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();

    if(err.type() == QSqlError::NoError){


        QString strQuery = "";
        strQuery = "SELECT A._id AS id, A.title, A.artist, A.album, A.duration, ART._data AS album_art, A.album_id, A._data AS data  ";
        strQuery += " FROM audio A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
        strQuery += " WHERE A._data='%1'";

        QVariantList data;
        sqlite->exec(strQuery.arg(p_path), data);

        if(data.size() > 0){
            json = data.at(0).toJsonObject();

        }
    }
    sqlite->close();
    delete sqlite;

    return json;
}


/**
 * @brief MusicFolder_USBFileList::playMusic : 음원 재생
 * @param jsonArrMusic QJsonArray 재생음원 정보
 */
void MusicFolder_USBFileList::playMusic(QJsonArray jsonArrMusic, bool flagPlayShuffle){//c220714

    int playType = global.device.getMusicPlayType();

    OptMorePopup::ClickMode curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
    switch(playType)
    {
        case 13:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne;
            break;
        case 12:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne;
            break;
        case 15:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne;
            break;
        case 16:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne;
            break;
        case 17:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere;
            break;
        case 18:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty;
            break;
        case 19:
            curr_clickMode = OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last;
            break;
    }

    QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(jsonArrMusic, this->select_index, curr_clickMode);

    QJsonObject examobj, examobjtmp;//cheon120812-iso
    examobjtmp = jsonArrMusic[0].toObject();//cheon120812-iso
    //QJsonDocument doc(examobjtmp);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MusicFolder_USBFileList::playMusic---" << strJson;
    QString tmp_split = ProcJsonEasy::getString(examobjtmp, "name");
    qDebug() << "tmp_split=" << tmp_split;
    QStringList splitToken = tmp_split.split(".");
    QString ttt="";
    if(splitToken.size() > 1){
        ttt = splitToken.at(1);
    }
    ttt = ttt.toLower();
    qDebug() << "ttt=" << ttt;
    qDebug() <<  "MusicFolder_USBFileList::playMusic--ext : " << examobjtmp["ext"].toString();//cheon120812-iso
    qDebug() << "MusicFolder_USBFileList::playMusic---global.isoflag=" << global.isoflag;//cheon120812-iso
    qDebug() << "global.device.CurrPlayType=" << global.device.CurrPlayType;//cheon120812-iso
    if(examobjtmp["ext"].toString() =="iso" || ttt =="iso"){//c220711

        print_debug();
        if(global.device.CurrPlayType =="CD"){

            //ToastMsg::show(this, "", tr("The CD PLAY has stopped playing...CD PLAY has stoped..."),1000);//cheon210812-iso
            QJsonObject tmp_json;
            qDebug() << "cdplaying.....................stop cmd executeed" ;
            //tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MusicFolder_USBFileList::playMusic---" << strJson;
            //QJsonObject tmp_json;

            dialog_comfirmCD_Stop();//c220711
            /*
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_stop(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.play.stop"), tmp_json, true, true);
    */

        }else{
            print_debug();
            QJsonObject tmp_json;

            ToastMsg::show(this, "", tr("ISO(CD) file have been played."));//cheon210812-iso
            for (int i = 0; i < jsonArrMusic.size(); ++i)
            {
                    examobj = jsonArrMusic[i].toObject();
                    if(examobj["isServer"].toBool()){

                        examobj.remove("isServer");
                        examobj.insert("isServer",false);
                    }
                    if(!examobj["isThumbNailGetting"].toBool()){

                        examobj.remove("isThumbNailGetting");
                        examobj.insert("isThumbNailGetting",true);
                    }

                    tmp_json.insert("data", examobj);
            }

            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());


           // tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
            qDebug()<<"88442332322";
            QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MusicFolder_USBFileList::playMusic---" << strJson;
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.iso.play"), tmp_json, true, true);

        }

    }else{//cheon120812-iso
        QJsonObject tmp_json;
        tmp_json.insert("musicPlayType", playType);
        tmp_json.insert("music", jsonArr_toPlayReal);
        tmp_json.insert("musicType", 11);
        tmp_json.insert("playType", flagIsMusic ? 20 : 21);
        tmp_json.insert("shuffle", flagPlayShuffle ? 1 : 0);
        tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
        tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);

        if(global.Queue_track_count != 0) {
            print_debug();
            emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug();
        emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_toPlayReal.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        if(flagIsMusic){
            print_debug();
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);
        }else{
            print_debug();
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("external_usb"), tmp_json, true, true);
        }
    }

}
void MusicFolder_USBFileList::slot_responseHttp_play(const int &p_id, const QJsonObject &p_jsonObject){//cheon210812-iso

    global.isoflag = false;//cheon210812-iso
    qDebug() << "*end-global.isoflag---: " << global.isoflag;
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "MusicFolder_USBFileList::slot_responseHttp---" << strJson;

    if(p_id == HTTP_NETWORK_PLAY){
        if(p_jsonObject.contains("code")){//cheon210812-iso
            QString tmp_code = p_jsonObject["code"].toString();
            bool tmp_codeOk = p_jsonObject["flagOk"].toBool();
            if(tmp_code =="G0000" && tmp_codeOk){

                //emit signal_isoplay(false);
                global.isoflag = false;
                //ToastMsg::show(this, "", tr("play--Select a song again to play CD PLAY"),3000);//cheon210812-iso
            }
            // else global.isoflag = true;
        }
    }

    sender()->deleteLater();
}


void MusicFolder_USBFileList::dialog_comfirmCD_Stop()
{
    DialogConfirm *dlgConfirmCD = new DialogConfirm(this);
    dlgConfirmCD->setTitle(tr("CD PLAY"));
    dlgConfirmCD->setTextHeight(150);
    dlgConfirmCD->setText(tr("Stop the selected song directly on CD PLAY."));//c221007_1
    //dlgConfirmCD->setGeometry(this->width()/3,this->height()/2, 350,400);//c221007_1
    int left = global.left_mainwindow+global.width_mainwindow/4;//c221007_1
    int top = global.top_mainwindow+global.height_mainwindow/4;//c221007_1
    dlgConfirmCD->setGeometry(left,top, 350,400);//c221007_1
    dlgConfirmCD->setAlertMode();
    dlgConfirmCD->setProperty("flagShown",false);
    if(dlgConfirmCD->property("flagShown").toBool()==false){
        dlgConfirmCD->setProperty("flagShown",true);

        int result = dlgConfirmCD->exec();

        if(result == QDialog::Accepted){
            QJsonObject tmp_json;
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_stop(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.play.stop"), tmp_json, true, true);



        }
    }
    delete dlgConfirmCD;
}

void MusicFolder_USBFileList::slot_responseHttp_stop(const int &p_id, const QJsonObject &p_jsonObject){//c220714
    global.isoflag = false;//cheon210812-iso
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "MediaFileTree::slot_responseHttp---" << strJson;
    if(p_id == HTTP_NETWORK_PLAY){
        if(p_jsonObject.contains("code")){//cheon210812-iso
            QString tmp_code = p_jsonObject["code"].toString();
            bool tmp_codeOk = p_jsonObject["flagOk"].toBool();
            if(tmp_code =="CDPLAY" && tmp_codeOk){
                //qDebug() << "tmp_code-CDPLAYING---";
                //emit signal_isoplay(true);
                global.isoflag = true;
            }
            else if(tmp_code =="G0000"&& tmp_codeOk){
                        qDebug() << "MediaFileTree::setResultOfNetworkPlay----tmp_code ==G0000&& tmp_codeOk";
                        //emit signal_isoplay(false);
                        QTimer::singleShot(2000, this, SLOT(slot_iso_play()));
                        global.isoflag = false;
                       ToastMsg::show(this, "", tr("CD PLAY has stoped. \nPlaese Select a song again to play CD PLAY."),3000);//cheon210812-iso

            }


        }else {//cheon210812-iso

        }

    }


    sender()->deleteLater();
}
/**
 * @brief MusicFolder_USBFileList::getPlayMusicInfo : 재생음원 정보 JsonArray 형태 반환
 * @param p_index if p_index < 0, 전체 재생
 * @return QJsonArray
 */
QJsonArray MusicFolder_USBFileList::getPlayMusicInfoArray(const int &p_index){

    QJsonArray jsonArrPlay;
    if(p_index < 0){
        for(int i = 0 ; i < listWidget->count(); i++){
            QJsonObject jsonMusicObject = getPlayMusicInfoObject(i);

            if(!jsonMusicObject.empty()){
                jsonArrPlay.append(jsonMusicObject);
            }
        }
    }else{
        jsonArrPlay.append(getPlayMusicInfoObject(p_index));
    }
    return jsonArrPlay;
}

/**
 * @brief MusicFolder_USBFileList::getPlayMusicInfoObject : 재생음원 정보 JsonObject 형태 반환
 * @param p_index int index
 * @param p_dataPopup
 * @return QJsonObject
 */
QJsonObject MusicFolder_USBFileList::getPlayMusicInfoObject(const int &p_index){
    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_filePath = map["filePath"].toString();
    QString tmp_albumArt = map["album_art"].toString();
    bool tmp_flagIsFile = map["isFile"].toBool();

    QJsonObject tmp_jsonMusic;
    if(tmp_flagIsFile){

        QString thumbnail = "";
        if(flagIsMusic && !tmp_albumArt.isEmpty()){
            thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + tmp_albumArt;
        }

        tmp_jsonMusic.insert("ext","");
        tmp_jsonMusic.insert("id","");
        tmp_jsonMusic.insert("pw","");
        tmp_jsonMusic.insert("ip","");
        tmp_jsonMusic.insert("isCloud",false);
        tmp_jsonMusic.insert("isDir",false);
        tmp_jsonMusic.insert("isFile",true);
        tmp_jsonMusic.insert("isNetworkAdd", false);
        tmp_jsonMusic.insert("isServer",false);
        tmp_jsonMusic.insert("isThumbNailGetting", thumbnail.isEmpty() ? true : false);
        tmp_jsonMusic.insert("isUp",false);
        tmp_jsonMusic.insert("name",tmp_fileName);
        tmp_jsonMusic.insert("path",tmp_filePath);
        tmp_jsonMusic.insert("smbParamPath", "");
        tmp_jsonMusic.insert("thumbnail", thumbnail);
        tmp_jsonMusic.insert("rcloneServer","");
    }


    return tmp_jsonMusic;
}

/**
 * @brief MusicFolder_USBFileList::requestAddTrackInPlaylist : 플레이리스트 담기 요청
 * @param p_index int index
 */
void MusicFolder_USBFileList::requestAddTrackInPlaylist(const int &p_index){
    // 현재 음악만 플레이리스트 담기 지원
    QJsonObject jsonTrack = getJsonObject_TrackData(p_index);
    jsonTrack.insert("isFavorite", false);
    jsonTrack.insert("isSelected", false);
    jsonTrack.insert("mac_address", global.device.getDeviceID());
    jsonTrack.insert("type", "music");

    QJsonArray jsonArr;
    jsonArr.append(jsonTrack);

    procAdd_playlist->setProperty("thumbPath", jsonTrack["thumbnail"].toString());
    procAdd_playlist->setProperty("playurl", jsonTrack["url"].toString());

    //procAdd_playlist->setTrackJsonArray(jsonArr);
    procAdd_playlist->setProperty("track", jsonArr);
    procAdd_playlist->showDlgOfPlayList();
}

/**
 * @brief MusicFolder_USBFileList::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void MusicFolder_USBFileList::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){

    if(p_index >=0 && p_index < listWidget->count()){
        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        map["favorites"] = p_flagFavor;
        item->setData(Qt::UserRole, map);
    }
}

/**
 * @brief MusicFolder_USBFileList::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void MusicFolder_USBFileList::changeTrackFavoritesUI(const int &p_index){

    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = !map["favorites"].toBool();
    item->setData(Qt::UserRole, map);
}

/**
 * @brief MusicFolder_USBFileList::showDlgOfAddFaroviteFolder : 네트워크 폴더 즐겨찾기 추가 다이얼로그 Show
 * @param p_dirName QString Dir명(기본 즐겨찾기 이름)
 * @param p_path QString path
 * @note 네트워크 폴더 Path 구성 시, 현재 Path 마지막 문자열 확인 필요<br>
 * Path 구분자 유무 확인
 */

void MusicFolder_USBFileList::showDlgOfAddFaroviteFolder(const QString &p_name, const QString &p_path){


   // show 즐겨찾기 Add Dialog
    Dialog::DialogAdd_FavoriteFolder *dlg_favorite = new Dialog::DialogAdd_FavoriteFolder(this);
    dlg_favorite->setFavoriteData(p_name, p_path);
    int result = dlg_favorite->exec();

    if(result == QDialog::Accepted){

    }

}



/**
 * @brief MusicFolder_USBFileList::showOptionPopup 옵션팝업 띄우기
 */
void MusicFolder_USBFileList::showOptionPopup(int p_index){

    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_filePath = map["filePath"].toString();
    QString tmp_albumArt = map["album_art"].toString();
    QString tmp_album = map["albumTitle"].toString();
    QString tmp_title = map["title"].toString();
    QString tmp_author = map["author"].toString();
    //bool tmp_flagIsFile = map["isFile"].toBool();

    //if(!tmp_title.isEmpty()){
        QJsonObject tmp_data = this->getPlayMusicInfoObject(p_index);
        QJsonArray tmp_array;
        tmp_array.append(tmp_data);

        QJsonObject tmp_jsonObject;
        tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, tmp_title);
        tmp_jsonObject.insert(KEY_OP_TITLE_SUB, tmp_author);
        tmp_jsonObject.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Folder);
        tmp_jsonObject.insert(KEY_OP_albumImg, tmp_data["thumbnail"].toString());
        tmp_jsonObject.insert(KEY_OP_cntStar, 0);
        tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
        //tmp_jsonObject.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, tmp_jsonArrTrack);
        tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
        tmp_jsonObject.insert("isAlbum", false); // 앨범이 아니라 곡임..

        emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, tmp_jsonObject);
    //}
}

/**
 * @brief MusicFolder_USBFileList::getJsonObject_TrackData : 트랙정보 데이터 반환
 * @param p_index int
 * @return QJsonObject 트랙정보 데이터 for favroite
 */
QJsonObject MusicFolder_USBFileList::getJsonObject_TrackData(const int &p_index){

    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    int tmp_id = map["id"].toInt();
//    bool tmp_isFavor = map["favorites"].toBool();
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_filePath = map["filePath"].toString();
    QString tmp_albumArt = map["album_art"].toString();
    QString tmp_album = map["albumTitle"].toString();
    QString tmp_title = map["title"].toString();
    QString tmp_author = map["author"].toString();

    QJsonObject jsonTrack;

    // 앨범
    jsonTrack.insert("album", tmp_album);

    // 아티스트
    QJsonObject jsonArtist;
    jsonArtist.insert("name", tmp_author);
    QJsonArray jsonArrArtists;
    jsonArrArtists.append(jsonArtist);
    jsonTrack.insert("artists", jsonArrArtists);

    // 클라이언트 아이디 = 오디오 아이디
    jsonTrack.insert("client_id", tmp_id);

    // 썸네일
    QJsonObject jsonThumb;
    jsonThumb.insert("url", tmp_albumArt);

    QJsonArray jsonArrThumbs;
    jsonArrThumbs.append(jsonThumb);
    jsonTrack.insert("thumbnails", jsonArrThumbs);

    // 타이틀
    jsonTrack.insert("title", tmp_title);

    // 재생 URL
    jsonTrack.insert("url", tmp_filePath);

    jsonTrack.insert("isFavorite", false);
    jsonTrack.insert("isSelected", false);
    jsonTrack.insert("mac_address", global.device.getDeviceID());
    jsonTrack.insert("type","music");


    jsonTrack.insert("thumbnail", tmp_albumArt);     // 썸네일 업로드를 위한 추가

    return jsonTrack;
}

/**
 * @brief MusicFolder_USBFileList::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void MusicFolder_USBFileList::requestSetTrackFavorites(const int &p_index){
    if(!global.user.getAccess_token().isEmpty()){

        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        bool tmp_isFavor = map["favorites"].toBool();


        QJsonArray tmp_jsonArrTrack;
        QJsonObject tmp_jsonTrack = getJsonObject_TrackData(p_index);
        tmp_jsonTrack.insert("mac_address", global.device.getDeviceID());
        tmp_jsonTrack.insert("type", "music");
        tmp_jsonTrack.insert("isFavorite", tmp_isFavor);
        tmp_jsonArrTrack.append(tmp_jsonTrack);

        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",tmp_jsonTrack["url"].toString());
        tmp_json.insert("favorite", tmp_isFavor);
        tmp_json.insert("tracks", tmp_jsonArrTrack);

        if(tmp_isFavor){
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_FAVORITE_ADD,  QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);

            QString tmp_thumb = tmp_jsonTrack["thumbnail"].toString();
            QString tmp_url = tmp_jsonTrack["url"].toString();

            if(!tmp_thumb.isEmpty() && !tmp_url.isEmpty()){
                QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_thumb);
                Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
                proc_uploadThumb->uploadThumnail(imgPath, tmp_url);
                connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
            }
        }else{
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_FAVORITE_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);
        }
    }
}


/**
 * @brief MusicFolder_USBFileList::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_fielPath
 */
void MusicFolder_USBFileList::requestGetTrackFavorites(const int &p_index, const QString &p_fielPath){
    QJsonObject tmp_json;
    tmp_json.insert("local_ip",global.device.getDeviceIP());
    tmp_json.insert("mac_address",global.device.getDeviceID());
    tmp_json.insert("url",p_fielPath);

    NetworkHttp *network = new NetworkHttp;
    network->setProperty("index", p_index);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);
}

/**
 * @brief MusicFolder_USBFileList::requestFileList : 파일 목록 요청
 */
void MusicFolder_USBFileList::requestFileList(){

    QJsonArray jsonArrMusci;
    QJsonObject jsonMusic = data->getData();
    jsonMusic.insert("isNetworkAdd", false);
    jsonMusic.insert("isThumbNailGetting", false);
    jsonArrMusci.append(jsonMusic);

    QJsonObject tmp_json;
    tmp_json.insert("roseToken", global.device.getDeviceRoseToken());
    tmp_json.insert("musicType", "10");
    tmp_json.insert("music", jsonArrMusci);
    tmp_json.insert("mediaSrcType", jsonArrMediaSrc);

    NetworkHttp *network = new NetworkHttp;
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    network->request(HTTP_NETWORK_FOLDER_USB_FILELIST, QString("http://%1:%2/external_usb")
                     .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
}

/**
 * @brief MusicFolder_USBFileList::setResultOfFileList : 파일목록 요청결과 처리
 * @param p_jsonObject
 */
void MusicFolder_USBFileList::setResultOfFileList(const QJsonObject &p_jsonObject){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_externalArr = "externalArr";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
        if(p_jsonObject.contains(jsonKey_externalArr)){
            QJsonArray jsonArrFile = p_jsonObject[jsonKey_externalArr].toArray();

            int file_cnt = 0;
            for(int i = 0 ; i < jsonArrFile.count(); i++){
                //appendFileUI(jsonArrFile.at(i).toObject());

                QJsonObject tmpObj = jsonArrFile.at(i).toObject();
                appendFileUI(tmpObj);

                if(ProcJsonEasy::getBool(tmpObj, "isFile")){
                    file_cnt++;
                }
            }

            if((file_cnt > 0) && (this->flagIsMusic == true)){
                this->btn_play->show();
                this->btn_shuffle->show();
            }
            else{
                this->btn_play->hide();
                this->btn_shuffle->hide();
            }
        }
    }
}

/**
 * @brief MusicFolder_USBFileList::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void MusicFolder_USBFileList::setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_idx, tmp_favorite);
    }
}

/**
 * @brief MusicFolder_USBFileList::setResultOfSetFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void MusicFolder_USBFileList::setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool tmp_flagOk = false;

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            tmp_flagOk = true;
        }
    }

    if(!tmp_flagOk){
        changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("You cannot change your favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}


/**
 * @brief MusicFolder_USBFileList::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicFolder_USBFileList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_NETWORK_FOLDER_USB_FILELIST){
        setResultOfFileList(p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_FAVORITE_GET){
        setResultOfGetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_FAVORITE_ADD){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_FAVORITE_DELETE){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }

    sender()->deleteLater();
}

/**
 * @brief MusicFolder_USBFileList::slot_changedRootPath : [슬롯] Root Path 변경
 * @details Path 변경 처리<br>
 * 목록 초기화, 파일 목록 요청
 * @param p_dirName
 * @param p_dirPath
 */
void MusicFolder_USBFileList::slot_changedRootPath(const QString &p_dirName, const QString &p_dirPath){

    data->setName(p_dirName);
    data->setPath(p_dirPath);

    topDirPath->addDirName(data->getName(), data->getPath());

    listWidget->clear();
    listFileData->clear();
    requestFileList();
}

/**
 * @brief MusicFolder_USBFileList::slot_delegateClicked : [슬롯] delegateClick
 * @details QlistWidget item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void MusicFolder_USBFileList::slot_delegateClicked(const int &p_index, const int &p_btnType){

    QListWidgetItem *item = listWidget->item(p_index);

    pixmapThumb = qvariant_cast<QPixmap>(item->data(Qt::DecorationRole));
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    QString tmp_fileName = map["fileName"].toString();
    QString tmp_filePath = map["filePath"].toString();
    QString tmp_title = map["title"].toString();
    QString tmp_author = map["author"].toString();
    QString tmp_album = map["albumTitle"].toString();
    QString tmp_duration = map["duration"].toString();
    int tmp_id = map["id"].toInt();
    bool tmp_flagIsFile = map["isFile"].toBool();

    if(!tmp_flagIsFile){
        if(p_btnType == USBFileDelegate::BtnType::etc){

            data = listFileData->at(p_index);

            topDirPath->addDirName(data->getName(), data->getPath());

            listWidget->clear();
            listFileData->clear();
            requestFileList();
        }
        else if(p_btnType == USBFileDelegate::BtnType::folderFav){
            showDlgOfAddFaroviteFolder(tmp_fileName, tmp_filePath);
        }
    }
    else{
        if(p_btnType == USBFileDelegate::BtnType::etc){
            print_debug();
            //playMusic(getPlayMusicInfoArray(p_index));
            this->select_index = p_index;
            playMusic(getPlayMusicInfoArray(-1));
        }
        else if(p_btnType == USBFileDelegate::BtnType::playListAdd){
            if(tmp_id > 0){
                // DB 에 해당 파일 정보 있는 경우에만 처리
                if(flagIsMusic){
                    requestAddTrackInPlaylist(p_index);
                }
            }
            else{
                ToastMsg::show(this,"",tr("You cannot add playlists."));
                //ToastMsg::show(this,"",tr("플레이리스트 담기를 할 수 없습니다."));
            }
        }
        else if(p_btnType == USBFileDelegate::BtnType::trackHeart){
            if(tmp_id >0){
                // DB 에 해당 파일 정보 있는 경우에만 처리
                changeTrackFavoritesUI(p_index);
                requestSetTrackFavorites(p_index);
            }
            else{
                ToastMsg::show(this,"",tr("You cannot change your favorites."));
                //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
            }
        }
        else if(p_btnType == USBFileDelegate::BtnType::more){
            // 옵션팝업 show
            this->showOptionPopup(p_index);
        }
    }
}

/**
 * @brief MusicFolder_USBFileList::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void MusicFolder_USBFileList::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index < listWidget->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}

/**
 * @brief MusicFolder_USBFileList::slot_clickedPlayAll : [슬롯] 전체 재생
 */
void MusicFolder_USBFileList::slot_clickedPlayAll(){
    print_debug();
    playMusic(getPlayMusicInfoArray(-1));
}

/**
 * @brief MusicFolder_USBFileList::slot_clickedPlayShuffle : [슬롯]
 */
void MusicFolder_USBFileList::slot_clickedPlayShuffle(){
    print_debug();
    playMusic(getPlayMusicInfoArray(-1), true);
}

/**
 * @brief MusicFolder_USBFileList::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void MusicFolder_USBFileList::slot_playlistClicked(const int &p_playlistno){

    QJsonArray jsonArrTrack = sender()->property("track").toJsonArray();

    if(p_playlistno > 0){
        procAdd_playlist->requestAddPlayListTrack(p_playlistno,jsonArrTrack);

        QString tmp_thumb = procAdd_playlist->property("thumbPath").toString();
        QString tmp_url = procAdd_playlist->property("playurl").toString();
        if(!tmp_thumb.isEmpty() && !tmp_url.isEmpty()){
            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_thumb);
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, tmp_url);
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
        }

    }else{
        procAdd_playlist->showDlgOfNewPlayList();
    }



}

/**
 * @brief MusicFolder_USBFileList::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void MusicFolder_USBFileList::slot_newPlaylistNameEdited(QString p_name){

    QJsonArray jsonArrTrack = sender()->property("track").toJsonArray();
    QString tmp_playurl = sender()->property("playurl").toString();

    if(pixmapThumb.size().width() < 100){
        // def_mus_60.png 기본 이미지인 경우: 이미지 로딩 실패
        procAdd_playlist->requestNewPlaylistTrack(p_name, jsonArrTrack);
    }else{
        procAdd_playlist->requestNewPlaylistTrack(pixmapThumb, tmp_playurl, p_name, jsonArrTrack);
    }

}


/**
 * @brief OptMorePopup::ClickMode에 따라서 JsonArray(Track정보)를 재가공해서 반환한다.
 * @param jsonArr_toPlayAll
 * @param curr_index
 * @param clickMode
 * @return
 */
QJsonArray MusicFolder_USBFileList::get_rearrangeJsonArray_toPlayData(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode){
    if(clickMode == OptMorePopup::ClickMode::Play_RightNow_withReorder
            || clickMode == OptMorePopup::ClickMode::Play_RightNow
    ){
        if(curr_index > 0){
            // 전체이지만 re-ordering
            QJsonArray jsonArr_output = this->reorderJsonArray(jsonArr_toPlayAll, curr_index);
            return jsonArr_output;
        }
        else{
            return jsonArr_toPlayAll;
        }
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_RightNow_OnlyOne
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext_OnlyOne
    ){
        // 1곡만
        QJsonArray jsonArr_output;
        jsonArr_output.append(jsonArr_toPlayAll.at(curr_index).toObject());
        return jsonArr_output;
    }
    else if(clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere
            || clickMode == OptMorePopup::ClickMode::SubMenu_Play_FromHere_procEmpty
            || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_FromHere_Last
    ){
        // index ~ 마지막까지 구성
        if(curr_index > 0){
            QJsonArray jsonArr_output = this->subRange_JsonArray(jsonArr_toPlayAll, curr_index);
            return jsonArr_output;
        }
        else{
            return jsonArr_toPlayAll;
        }
    }
    else{
        // 그 이외의 경우
        return jsonArr_toPlayAll;
    }

}


/**
 * @brief AbstractProcRosePlay::subRange_JsonArray
 * @param p_jsonArr
 * @param startIndex
 * @return
 */
QJsonArray MusicFolder_USBFileList::subRange_JsonArray(const QJsonArray &p_jsonArr, const int startIndex){
    const int len_data = p_jsonArr.count();
    if(startIndex>0 && startIndex<len_data){
        QJsonArray jsonArr_subRange;
        for(int i=startIndex ; i<len_data ; i++){
            jsonArr_subRange.append(p_jsonArr.at(i));
        }
        return jsonArr_subRange;
    }
    else{
        // 유효하지 않는 경우, 그대로 반환
        return p_jsonArr;
    }
}


/**
 * @brief AbstractProcRosePlay::reorderJsonArray
 * @param p_jsonArr
 * @param startIndex
 * @return
 */
QJsonArray MusicFolder_USBFileList::reorderJsonArray(const QJsonArray &p_jsonArr, const int startIndex){
    const int len_data = p_jsonArr.count();
    if(startIndex>0 && startIndex<len_data){
        QJsonArray jsonArr_reorder;
        for(int i=startIndex ; i<len_data ; i++){
            jsonArr_reorder.append(p_jsonArr.at(i));
        }
        for(int i=0 ; i<startIndex ; i++){
            jsonArr_reorder.append(p_jsonArr.at(i));
        }
        return jsonArr_reorder;
    }
    else{
        // 유효하지 않는 경우, 그대로 반환
        return p_jsonArr;
    }
}
