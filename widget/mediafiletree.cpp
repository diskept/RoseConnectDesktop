#include "mediafiletree.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/gscommon.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QPixmap>
#include "thread/mediaplayerthread.h"
#include "widget/toastmsg.h"
#include "widget/framewidget.h"
#include "login/dialogladd_network.h"
#include "login/dialogadd_favoritefolder.h"
#include <QDateTime>
#include <QUuid>
#include <QMetaObject>
#include <QJsonObject>
#include <QJsonArray>
#include <common/sqlitehelper.h>
#include <music/proc_uploadthumbnail.h>
#include "widget/optionpopup.h"

#include "common/rosesettings.h"
#include <QSettings>


/**
 * @brief MediaFileTree::MediaFileTree : 네트워크 파일 트리 화면
 * @param parent
 * @note
 *      프리징 오류 수정 @n
 *      QFileSystemModel 관련 버그 :: 특정 폴더(대용량)에 2번째 이동시 LOCK 발생 @n
 *      :: RootPath 변경시마다 부모객체에서 MediaFileTree new 생성
 *      :: 추후 QFileSystemModel 사용하지 않고 QAbstrackItemModel 구현 고려
 *      QFileSystemModel + QListWidget 사용 문제 :: 파일이 많은경우 LOCK 발생 @n
 *      :: QFileSystemModel 상속받은 @sa NetworkFileSystemModel + QListView 사용으로 변경
 */
MediaFileTree::MediaFileTree(QWidget *parent, bool p_flagIsMusic)
    : QWidget(parent)
    , flagIsMusic(p_flagIsMusic)
{
    linker = Linker::getInstance();
    setInit();
    setUIControl();
}

MediaFileTree::~MediaFileTree(){

}

/**
 * @brief MediaFileTree::setInit : 초기 세팅
 */
void MediaFileTree::setInit(){
    // --------------------------------------
    //  File Name Filter
    // --------------------------------------
    QStringList listNameFilter;
    if(flagIsMusic){
        //listNameFilter.append("*.3gp");
        listNameFilter.append("*.aac");
        listNameFilter.append("*.aac-adts");
        listNameFilter.append("*.act");
        listNameFilter.append("*.aif");
        listNameFilter.append("*.aiff");
        listNameFilter.append("*.alac");
        listNameFilter.append("*.amr");
        listNameFilter.append("*.amr-wb");
        listNameFilter.append("*.ape");
        listNameFilter.append("*.cda");
        listNameFilter.append("*.dff");
        listNameFilter.append("*.dsf");
        listNameFilter.append("*.ec3");
        listNameFilter.append("*.e-ec3");
        listNameFilter.append("*.flac");
        listNameFilter.append("*.m4a");
        listNameFilter.append("*.m4b");
        listNameFilter.append("*.m4p");
        listNameFilter.append("*.midi");
        listNameFilter.append("*.mp1");
        listNameFilter.append("*.mp2");
        listNameFilter.append("*.mp3");
        listNameFilter.append("*.mpc");
        listNameFilter.append("*.mpga");
        listNameFilter.append("*.imelody");
        listNameFilter.append("*.iso");
        listNameFilter.append("*.sp-midi");
        listNameFilter.append("*.ogg");
        listNameFilter.append("*.wav");
        listNameFilter.append("*.wave");
        listNameFilter.append("*.wma");
        listNameFilter.append("*.wv");
        listNameFilter.append("*.x-matroska");
        listNameFilter.append("*.x-ms-wma");
        listNameFilter.append("*.x-mpegurl");
        listNameFilter.append("*.x-wav");

    }
    else{
        listNameFilter.append("*.3gp");
        listNameFilter.append("*.3g2");
        listNameFilter.append("*.asf");
        listNameFilter.append("*.avchd");
        listNameFilter.append("*.avi");
        listNameFilter.append("*.dat");
        listNameFilter.append("*.divx");
        listNameFilter.append("*.evo");
        listNameFilter.append("*.flv");
        listNameFilter.append("*.f4v");
        listNameFilter.append("*.h264");
        listNameFilter.append("*.k3g");
        listNameFilter.append("*.mkv");
        listNameFilter.append("*.mov");
        listNameFilter.append("*.mp4");
        listNameFilter.append("*.mpe");
        listNameFilter.append("*.mpg");
        listNameFilter.append("*.mts");
        listNameFilter.append("*.m2p");
        listNameFilter.append("*.m2t");
        listNameFilter.append("*.m2ts");
        listNameFilter.append("*.m2v");
        listNameFilter.append("*.m4v");
        listNameFilter.append("*.mxf");
        listNameFilter.append("*.ogg");
        listNameFilter.append("*.ogm");
        listNameFilter.append("*.ogv");
        listNameFilter.append("*.ots");
        listNameFilter.append("*.rm");
        listNameFilter.append("*.rmvb");
        listNameFilter.append("*.swf");
        listNameFilter.append("*.tp");
        listNameFilter.append("*.trp");
        listNameFilter.append("*.ts");
        listNameFilter.append("*.vob");
        listNameFilter.append("*.vp6");
        listNameFilter.append("*.wbem");
        listNameFilter.append("*.webm");
        listNameFilter.append("*.wmv");

    }


    // --------------------------------------
    //  Set FileSystemModel
    // --------------------------------------
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
}

/**
 * @brief MediaFileTree::setUIControl : UI 세팅
 */
void MediaFileTree::setUIControl(){

    // ------------------------------------------------------
    //  전체재생, 셔플재생 UI
    // ------------------------------------------------------
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


    // ------------------------------------------------------
    //  파일 목록 ListView
    // ------------------------------------------------------
    listView = new QListView(this);
    listView->setItemDelegate(delegate);
    listView->setCursor(Qt::PointingHandCursor);
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setStyleSheet("border:none;");
    listView->setModel(model);



    // ------------------------------------------------------
    //  박스 구성 및 레이아웃 세팅
    // ------------------------------------------------------
    QVBoxLayout *boxContents = new QVBoxLayout;
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(33,0,28,0);
    boxContents->addLayout(box_option);
    boxContents->addWidget(listView);

    QVBoxLayout *box = new QVBoxLayout;
    box->setSpacing(0);
    box->setContentsMargins(0,0,0,0);
    box->setAlignment(Qt::AlignTop);
    box->addLayout(boxContents);

    setLayout(box);

    // 커넥트
    connect(btn_play, SIGNAL(clicked()), this, SLOT(slot_clickedPlayAll()));
    connect(btn_shuffle, SIGNAL(clicked()), this, SLOT(slot_clickedPlayShuffle()));
    //connect(this, SIGNAL(signal_isoplay(bool)), this, SLOT(slot_clickedPlayAllafterCheck(bool)));//cheon210617-http

}

/**
 * @brief MediaFileTree::setConvertSMBPath : SMB Path Convert
 * @param p_smbPath
 * @note Path 마지막 문자열 검사('/' seperator 유무 확인)
 */
void MediaFileTree::setConvertedSMBPath(QString p_smbPath){

    if(p_smbPath.back()=="/"){
        currSMBPath = p_smbPath;
    }else{
        currSMBPath = p_smbPath+"/";
    }
}

/**
 * @brief MediaFileTree::setData : 데이터 세팅
 * @param p_rootName QString 루트 디렉토리명
 * @param p_rootPath QString 루트 디렉토리 경로
 */
void MediaFileTree::setData(const QString &p_rootName, const QString &p_smbPath){

    QString path = convertSMBPathToFilePath(p_smbPath);

    // ------------------------------------------------------
    //  path가 다른 경우에만 re draw
    // ------------------------------------------------------
    if(currRootPath != path){
        loadMediaLastIndex = 0;
        list_fileLoadStruct->clear();
        setConvertedSMBPath(p_smbPath);
        currRootPath = path;
        uuid = QUuid::createUuid();
        //model->setRootPath(currRootPath);
        model->clearData();
        listView->setRootIndex(model->setRootPath(currRootPath));
    }

    emit signal_clickedDirName(p_rootName, p_smbPath);
}

/**
 * @brief MediaFileTree::convertSMBPathToFilePath : SMB PATH => File PAth
 * @return
 */
QString MediaFileTree::convertSMBPathToFilePath(const QString &p_smbPath){
    QStringList tmp_pathList = p_smbPath.split("@");
    QString path;
    #if defined(Q_OS_WINDOWS)
        if(tmp_pathList.size() > 0){
            path = tmp_pathList.last();
            if(path.startsWith("smb://", Qt::CaseInsensitive)){
                path.replace("smb://","");
            }else if(path.startsWith("//")){
                path.replace("//","");
            }

            path.prepend("//");

            // 마지막 / 제거
            if(path.split("/").last().isEmpty()){
                path.chop(1);
            }
        }
    #elif defined(Q_OS_MAC)//cheon210831-network
        path = p_smbPath;
        // 마지막 / 제거
        if(path.split("/").last().isEmpty()){
            path.chop(1);
        }

    #endif

    return path;
}

/**
 * @brief MediaFileTree::getSMBID : SMB ID 반환
 * @return QString
 * @note smbPath로 부터
 */
QString MediaFileTree::getSMBID(){

    QString tmp_smbPath = currSMBPath.split("//").last();

    if(tmp_smbPath.contains(":") && tmp_smbPath.contains("@")){
        return tmp_smbPath.split(":").first();
    }

    return "";
}

/**
 * @brief MediaFileTree::getSMBPW : SMB PW 반환
 * @return QString
 * @note smbPath로 부터
 */
QString MediaFileTree::getSMBPW(){

    QString tmp_smbPath = currSMBPath.split("//").last();

    if(tmp_smbPath.contains(":") && tmp_smbPath.contains("@")){
        return tmp_smbPath.split(":").last().split("@").first();
    }

    return "";
}

/**
 * @brief MediaFileTree::convertSMBPath : SMB Path 생성
 * @return
 */
QString MediaFileTree::createSMBPath(bool flagAddIdPw){

    QString tmp_smbPath = currRootPath;

    if(tmp_smbPath.startsWith("//")){
        tmp_smbPath.remove(0,2);
    }

    if(flagAddIdPw){
        tmp_smbPath = "smb://"+getSMBID()+":"+getSMBPW()+"@"+tmp_smbPath;
    }else{
        tmp_smbPath = "smb://"+tmp_smbPath;
    }

    return tmp_smbPath;
}

/**
 * @brief MediaFileTree::getPlayMusicInfoObject : 재생음원 정보 JsonObject 형태 반환
 * @param p_index int index
 * @param p_dataPopup
 * @return QJsonObject
 */
QJsonObject MediaFileTree::getPlayMusicInfoObject(const int &p_index, DataPopup *p_dataPopup){//cheon210825-network
//print_debug();
#if defined(Q_OS_WIN)
    QJsonObject tmp_jsonMusic;
qDebug() << "MediaFileTree::getPlayMusicInfoObject------001-334";
    if(p_index >= 0 && p_index < model->rowCount(listView->rootIndex())){

        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
        QMap<QString,QVariant> map = modelIndex.data(Qt::UserRole).toMap();
        QString tmp_fileName = map["fileName"].toString();
        QString tmp_filePath = map["filePath"].toString();
        bool tmp_flagIsFile = map["isFile"].toBool();

        QString tmp_path = createSMBPath(true)+"/";

        if(tmp_flagIsFile){

            QString tmp_smbPath = currRootPath;
            if(tmp_smbPath.startsWith("//")){
                tmp_smbPath.remove(0,2);
            }
            QString tmp_ip = "";
            QStringList strList_smb = tmp_smbPath.split("/");
            if(strList_smb.count() > 0){
                tmp_ip = strList_smb.first();
            }

            QString smbParamPath = "smb://"+getSMBID()+":"+getSMBPW()+"@"+tmp_ip + "/";
            QString thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + p_dataPopup->getAlbum_art();

            /*tmp_jsonMusic.insert("ext",tmp_filePath.split(".").last());
            tmp_jsonMusic.insert("id",getSMBID());
            //tmp_jsonMusic.insert("id",getSMBID());//cheon210825-network
            tmp_jsonMusic.insert("pw",getSMBPW());
            //tmp_jsonMusic.insert("pw","Citech1234");//cheon210825-network
            tmp_jsonMusic.insert("ip",tmp_ip);
            //tmp_jsonMusic.insert("ip","192.168.1.4");//cheon210825-network
            tmp_jsonMusic.insert("isCloud",false);
            tmp_jsonMusic.insert("isDir",false);
            tmp_jsonMusic.insert("isFile",true);
            tmp_jsonMusic.insert("isNetworkAdd", false);
            tmp_jsonMusic.insert("isServer",true);
            tmp_jsonMusic.insert("isThumbNailGetting", false);
            tmp_jsonMusic.insert("isUp",false);
            tmp_jsonMusic.insert("name",tmp_fileName);
            tmp_jsonMusic.insert("path",tmp_path);
            //tmp_jsonMusic.insert("path","smb://rose:Citech1234@192.168.1.4/Media1/Music1/pop/[24bit 96khz] Sarah McLachlan - [2002] Touch   [FLAC]/");//cheon210825-network
            tmp_jsonMusic.insert("smbParamPath", smbParamPath);
            //tmp_jsonMusic.insert("smbParamPath", "smb://rose:Citech1234@192.168.1.4/");//cheon210825-network
            tmp_jsonMusic.insert("thumbnail", thumbnail);*/

            tmp_jsonMusic.insert("albumId", "");
            tmp_jsonMusic.insert("albumKey", "");
            tmp_jsonMusic.insert("artist", "");
            tmp_jsonMusic.insert("duration", "");
            tmp_jsonMusic.insert("ext", tmp_filePath.split(".").last());
            tmp_jsonMusic.insert("id", getSMBID());
            tmp_jsonMusic.insert("ip", tmp_ip);
            tmp_jsonMusic.insert("isCloud", false);
            tmp_jsonMusic.insert("isDir", false);
            tmp_jsonMusic.insert("isFile", true);
            tmp_jsonMusic.insert("isNetworkAdd", false);
            tmp_jsonMusic.insert("isServer", true);
            tmp_jsonMusic.insert("isShare", false);
            tmp_jsonMusic.insert("isThumbNailGetting", false);
            tmp_jsonMusic.insert("isUp", false);
            tmp_jsonMusic.insert("name",tmp_fileName);
            tmp_jsonMusic.insert("path",tmp_path);
            tmp_jsonMusic.insert("pw",getSMBPW());
            tmp_jsonMusic.insert("rcloneServe", "");
            tmp_jsonMusic.insert("smbParamPath", smbParamPath);
            tmp_jsonMusic.insert("thumbnail", thumbnail);
            tmp_jsonMusic.insert("title", "");
        }
    }
#elif defined(Q_OS_MAC)//cheon210831-network
//print_debug();
        //QString p_rootName = global.p_rootName ;
        QString p_path = global.p_path ;
        QString tmp_id ="", tmp_pw="";
        qDebug() << "MediaFileTree::getPlayMusicInfoObject-001--p_path : " << p_path;
        QString tmp_smbPath = p_path.split("//").last();
        qDebug() << "MediaFileTree::getPlayMusicInfoObject--002--tmp_smbPath : " << tmp_smbPath;
        if(tmp_smbPath.contains(":") && tmp_smbPath.contains("@")){
                qDebug() << "MediaFileTree::getPlayMusicInfoObject--003--tmp_smbPath.split(:).first() : " << tmp_smbPath.split(":").first();
                tmp_id = tmp_smbPath.split(":").first();
        }

        QString tmp_smbPath2 = p_path.split("//").last();

        if(tmp_smbPath.contains(":") && tmp_smbPath.contains("@")){
                tmp_pw =  tmp_smbPath2.split(":").last().split("@").first();
        }


        QStringList tmp_dirpath = tmp_smbPath2.split("@");
        QStringList tmp_dirpath2 = tmp_dirpath[1].split("/");

        qDebug() << "tmp_id: " << tmp_id << "tmp_pw: " << tmp_pw ;
        //qDebug() << "tmp_dirpath[0]: " << tmp_dirpath[0]<< "tmp_dirpath[1]: " << tmp_dirpath[1];
        //qDebug() << "tmp_dirpath2[0]: " << tmp_dirpath2[0]<< "tmp_dirpath2[1]: " << tmp_dirpath2[1];
        QString tmp_ip = tmp_dirpath2[0];

        QJsonObject tmp_jsonMusic;
    qDebug() << "MediaFileTree::getPlayMusicInfoObject------001-334";
        if(p_index >= 0 && p_index < model->rowCount(listView->rootIndex())){

            QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
            QMap<QString,QVariant> map = modelIndex.data(Qt::UserRole).toMap();
            QString tmp_fileName = map["fileName"].toString();
            QString tmp_filePath = map["filePath"].toString();
            bool tmp_flagIsFile = map["isFile"].toBool();
            qDebug() << "tmp_filePath : " << tmp_filePath;
            QStringList tmp_dirpath2 = tmp_smbPath2.split("@");


            QStringList strList_smb = tmp_filePath.split("/");
            qDebug() << "strList_smb.size()--" << strList_smb.count();
            QString tmp_path = p_path;
            if(strList_smb.count()> 0){
                for( int i = 3; i < strList_smb.count()-1; i++){
                    qDebug() << "strList_smb[i]" << i << "," << strList_smb[i];
                    tmp_path = tmp_path + strList_smb[i]+"/";

                }

            }


            qDebug() << "MediaFileTree::getPlayMusicInfoObject--006--tmp_Path : " << tmp_path;

            if(tmp_flagIsFile){
/*
                QString tmp_smbPath = currRootPath;
                if(tmp_smbPath.startsWith("//")){
                    tmp_smbPath.remove(0,2);
                }
                QString tmp_ip = "";
                QStringList strList_smb = tmp_smbPath.split("/");
                if(strList_smb.count()>0){
                    tmp_ip = strList_smb.first();
                }
*/
                //QString smbParamPath = "smb://"+getSMBID()+":"+getSMBPW()+"@"+tmp_ip + "/";
                QString smbParamPath = p_path;

                QString thumbnail = "http://" + global.device.getDeviceIP() + ":" + global.port_img + p_dataPopup->getAlbum_art();

                tmp_jsonMusic.insert("ext",tmp_filePath.split(".").last());
                tmp_jsonMusic.insert("id",tmp_id);
                //tmp_jsonMusic.insert("id",getSMBID());
                tmp_jsonMusic.insert("pw",tmp_pw);
                //tmp_jsonMusic.insert("pw","Citech1234");
                tmp_jsonMusic.insert("ip",tmp_ip);
                //tmp_jsonMusic.insert("ip","192.168.1.4");
                tmp_jsonMusic.insert("isCloud",false);
                tmp_jsonMusic.insert("isDir",false);
                tmp_jsonMusic.insert("isFile",true);
                tmp_jsonMusic.insert("isNetworkAdd", false);
                tmp_jsonMusic.insert("isServer",true);
                tmp_jsonMusic.insert("isThumbNailGetting", false);
                tmp_jsonMusic.insert("isUp",false);
                tmp_jsonMusic.insert("name",tmp_fileName);
                tmp_jsonMusic.insert("path",tmp_path);
                //tmp_jsonMusic.insert("path","smb://rose:Citech1234@192.168.1.4/Media1/Music1/pop/[24bit 96khz] Sarah McLachlan - [2002] Touch   [FLAC]/");
                tmp_jsonMusic.insert("smbParamPath", smbParamPath);
                //tmp_jsonMusic.insert("smbParamPath", "smb://rose:Citech1234@192.168.1.4/");
                tmp_jsonMusic.insert("thumbnail", thumbnail);
            }
        }
#endif

    return tmp_jsonMusic;
}

/**
 * @brief MediaFileTree::getPlayMusicInfo : 재생음원 정보 JsonArray 형태 반환
 * @param p_index if p_index < 0, 전체 재생
 * @return QJsonArray
 */
QJsonArray MediaFileTree::getPlayMusicInfoArray(const int &p_index){

    QJsonArray jsonArrPlay;
    if(p_index < 0){
        for(int i =0 ; i < model->rowCount(listView->rootIndex()); i++){
            DataPopup *dataPopup = getTrackData(i);
            QJsonObject jsonMusicObject = getPlayMusicInfoObject(i,dataPopup);

            if(!jsonMusicObject.empty()){
                jsonArrPlay.append(jsonMusicObject);
            }
        }
    }else{
        DataPopup *dataPopup = getTrackData(p_index);
        jsonArrPlay.append(getPlayMusicInfoObject(p_index,dataPopup));
    }

    return jsonArrPlay;
}

/**
 * @brief MediaFileTree::playMusic : 음원 재생 if(jsonObj.contains(key)) {
       return jsonObj[key].toDouble(defValue);
   }
 * @param jsonArrMusic QJsonArray 재생음원 정보
 */
void MediaFileTree::playMusic(QJsonArray jsonArrMusic, bool flagPlayShuffle){//cheon120812-iso
    print_debug();
    emit linker->signal_playMusic_desktop(jsonArrMusic, flagPlayShuffle);//c220617
    QJsonObject tmp_json;
    QJsonObject examobj, examobjtmp;//cheon120812-iso
    jsonArrMusic_curr = jsonArrMusic;
    //for(int i = 0; i < jsonArrMusic.count(); i++){
    //    qDebug() << "i=" << jsonArrMusic[i].toObject();;
    //}
    examobjtmp = jsonArrMusic[0].toObject();//cheon120812-iso
    //QJsonDocument doc(examobjtmp);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;
    QString tmp_split = ProcJsonEasy::getString(examobjtmp, "name");
    qDebug() << "tmp_split=" << tmp_split;
    QStringList splitToken = tmp_split.split(".");
    QString ttt="";
    if(splitToken.size() > 1){
        ttt = splitToken.at(1);
    }
    ttt = ttt.toLower();
    qDebug() << "ttt=" << ttt;
    qDebug() <<  "MediaFileTree::playMusic--ext : " << examobjtmp["ext"].toString();//cheon120812-iso
    qDebug() << "MediaFileTree::playMusic---global.isoflag=" << global.isoflag;//cheon120812-iso
    qDebug() << "global.device.CurrPlayType=" << global.device.CurrPlayType;//cheon120812-iso
    if(examobjtmp["ext"].toString() =="iso" || ttt =="iso"){//c220711
        global.device.CurrExtType = "iso";//c220903_1

        if(global.device.CurrPlayType =="CD"){

            //ToastMsg::show(this, "", tr("The CD PLAY has stopped playing...CD PLAY has stoped..."),1000);//cheon210812-iso
            QJsonObject tmp_json;
            qDebug() << "cdplaying.....................stop cmd executeed" ;
            //tmp_json.insert("roseToken",global.device.getDeviceRoseToken());

            //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;
            //QJsonObject tmp_json;

            dialog_comfirmCD_Stop();//c220711
            /*
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_stop(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.play.stop"), tmp_json, true, true);
    */

        }else{


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
            qDebug()<<"4449999888877776666";
            QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.iso.play"), tmp_json, true, true);

        }

    }else{//cheon120812-iso
       // if(global.device.CurrPlayType =="CD"){
       //     print_debug();
       //     dialog_comfirmCD_Stop();//c220711

       // }else{
            global.device.CurrExtType = "NONE";//c220903_1
            ToastMsg::show(this, "", tr("player has started."));//cheon210812-iso
            tmp_json.insert("musicPlayType",15);
            tmp_json.insert("music", jsonArrMusic);
            tmp_json.insert("playType",flagIsMusic ? 20 : 21);
            tmp_json.insert("shuffle",flagPlayShuffle ? 1 : 0);
            tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
            tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);

            if(global.Queue_track_count != 0) {
                print_debug();emit linker->signal_checkQueue(27, "");

                return;
            }
            print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += jsonArrMusic.count();     // 220419 queue count
            qDebug()<<"MediaFileTree::playMusic--not iso song ---4449999888877776666";
            //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);

       // }
    }

}

void MediaFileTree::playMusic_afterStopCheck(QJsonArray jsonArrMusic, bool flagPlayShuffle){//cheon120812-iso
    QJsonObject tmp_json;
    QJsonObject examobj, examobjtmp;//cheon120812-iso
    examobjtmp = jsonArrMusic[0].toObject();//cheon120812-iso
    QJsonDocument doc(examobjtmp);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic_afterStopCheck---" << strJson;

    qDebug() << "MediaFileTree::playMusic_afterCheck---" << examobjtmp["ext"].toString();//cheon120812-iso
    qDebug() << "MediaFileTree::playMusic_afterCheck---global.isoflag=" << global.isoflag;//cheon120812-iso

    QString tmp_split = ProcJsonEasy::getString(examobjtmp, "name");
    qDebug() << "tmp_split=" << tmp_split;
    QStringList splitToken = tmp_split.split(".");
    QString ttt="";
    if(splitToken.size() > 1){
        ttt = splitToken.at(1);
    }
    ttt = ttt.toLower();
    qDebug() << "ttt=" << ttt;
    if(examobjtmp["ext"].toString() == "iso"  || ttt =="iso"){//cheon120812-iso

            ToastMsg::show(this, "", tr("ISO file has playing..."));//cheon210812-iso
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
            qDebug()<<"4449999888877776666";
            QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic_afterStopCheck---" << strJson;
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("cd.iso.play"), tmp_json, true, true);

       // }

    }else{//cheon120812-iso

        tmp_json.insert("musicPlayType", 15);
        tmp_json.insert("music", jsonArrMusic);
        tmp_json.insert("playType",flagIsMusic ? 20 : 21);
        tmp_json.insert("shuffle",flagPlayShuffle ? 1 : 0);
        tmp_json.insert("roseToken",global.device.getDeviceRoseToken());
        tmp_json.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArrMusic.count();     // 220419 queue count
        //qDebug()<<"4449999888877776666";
        //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"MediaFileTree::playMusic---" << strJson;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
        network->request(HTTP_NETWORK_PLAY, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_network_play"), tmp_json, true, true);

    }

}



/**
 * @brief MediaFileTree::showDlgOfAddNetworkFolder : 네트워크 폴더 추가 다이얼로그 Show
 * @note 네트워크 폴더 Path 구성 시, 현재 Path 마지막 문자열 확인 필요<br>
 * Path 구분자 유무 확인
 */
void MediaFileTree::showDlgOfAddNetworkFolder(const QString &p_dirName){


    // show 네트워크 폴더 Add Dialog

    Dialog::DialogAdd_Network *dlg_network = new Dialog::DialogAdd_Network(this);
    dlg_network->setFolderInfo(createSMBPath(false)+"/"+p_dirName, getSMBID(), getSMBPW());
    int result = dlg_network->exec();

    if(result == QDialog::Accepted){
        //requestNetworkFolderList(true);
    }

    delete dlg_network;
}

/**
 * @brief MediaFileTree::showDlgOfAddFaroviteFolder : 네트워크 폴더 즐겨찾기 추가 다이얼로그 Show
 * @param p_dirName QString Dir명(기본 즐겨찾기 이름)
 * @param p_smbPath QString smbPath(ID,PW 포함)
 * @note 네트워크 폴더 Path 구성 시, 현재 Path 마지막 문자열 확인 필요<br>
 * Path 구분자 유무 확인
 */
void MediaFileTree::showDlgOfAddFaroviteFolder(const QString &p_dirName){


   // show 즐겨찾기 Add Dialog
    Dialog::DialogAdd_FavoriteFolder *dlg_favorite = new Dialog::DialogAdd_FavoriteFolder(this);
    dlg_favorite->setFavoriteData(p_dirName, createSMBPath(true)+"/"+p_dirName);
    int result = dlg_favorite->exec();

    if(result == QDialog::Accepted){

    }
}

/**
 * @brief MediaFileTree::slot_delegateClicked : [슬롯] delegateClick
 * @details delegate item click 처리<br>
 * @param p_index
 * @param p_btnType
 */
void MediaFileTree::slot_delegateClicked(const int &p_index, const int &p_btnType){

    if(p_index >=0 && p_index < model->rowCount(listView->rootIndex())){

        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());

        pixmapThumb = qvariant_cast<QPixmap>(modelIndex.data(Qt::DecorationRole));
        QMap<QString,QVariant> map = modelIndex.data(Qt::UserRole).toMap();
        QString tmp_fileName = map["fileName"].toString();
        QString tmp_filePath = map["filePath"].toString();
        QString tmp_title = map["title"].toString();
        QString tmp_author = map["author"].toString();
        QString tmp_album = map["albumTitle"].toString();
        QString tmp_duration = map["duration"].toString();
        bool tmp_flagIsFile = map["isFile"].toBool();

        if(!tmp_flagIsFile){
            if(p_btnType == FileDelegate::BtnType::etc){

                // 디렉토리 이동
                loadMediaLastIndex = 0;
                list_fileLoadStruct->clear();
                currRootPath = tmp_filePath;
                uuid = QUuid::createUuid();
                //model->setRootPath(tmp_filePath);
                model->clearData();
                listView->setRootIndex(model->setRootPath(tmp_filePath));

                QDirIterator chk(tmp_filePath, QDir::Files);

                int file_cnt = 0;
                while(chk.hasNext()){
                    if(chk.next() > 0){
                        file_cnt++;
                        break;
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

                // 상단 타이틀 디렉토리 리스트 변경 요청
                emit signal_clickedDirName(tmp_fileName, tmp_filePath);
            }
            else if(p_btnType == FileDelegate::BtnType::folderFav){
                showDlgOfAddFaroviteFolder(tmp_fileName);
            }
            else if(p_btnType == FileDelegate::BtnType::folderAdd){
                showDlgOfAddNetworkFolder(tmp_fileName);
            }
        }
        else{
            if(p_btnType == FileDelegate::BtnType::etc){

                // 음원 재생
                playMusic(getPlayMusicInfoArray(p_index));
            }
            else if(p_btnType == FileDelegate::BtnType::playListAdd){

                if(flagIsMusic){
                    requestAddTrackInPlaylist(tmp_fileName);
                }
            }
            else if(p_btnType == FileDelegate::BtnType::trackHeart){
                changeTrackFavoritesUI(p_index);
                requestSetTrackFavorites(p_index);
            }else if(p_btnType == FileDelegate::BtnType::more){
                // 옵션팝업 show
                this->showOptionPopup(p_index);
            }
        }
    }
}

/**
 * @brief MediaFileTree::requestAddTrackInPlaylist : 플레이리스트 담기 요청
 * @param p_fileName QString 추가할 파일명
 */
void MediaFileTree::requestAddTrackInPlaylist(QString p_fileName){
    // 현재 음악만 플레이리스트 담기 지원
    QJsonObject jsonTrack;
    jsonTrack.insert("isFavorite", false);
    jsonTrack.insert("isSelected", false);
    jsonTrack.insert("mac_address", global.device.getDeviceID());
    jsonTrack.insert("type", "music");

    QJsonArray jsonArr;
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = "SELECT AU.album, AU.artist, AU._id, AU.title,AU._data, AA._data AS art FROM audio AS AU";
        strQuery += " LEFT OUTER JOIN album_art AS AA";
        strQuery += " ON AU.album_id=AA.album_id";
        strQuery += " WHERE AU._display_name='"+p_fileName+"'";

        QVariantList data;
        sqlite->exec(strQuery, data);

        if(data.size() > 0){
            QMap<QString, QVariant> map = data.at(0).toMap();

            jsonTrack.insert("album", map["album"].toString());
            jsonTrack.insert("client_id", map["_id"].toString());
            jsonTrack.insert("title", map["title"].toString());
            jsonTrack.insert("url", map["_data"].toString());

            QJsonArray jsonArrArtist;
            QJsonObject jsonArtist;
            jsonArtist.insert("name", map["artist"].toString());
            jsonArrArtist.append(jsonArtist);
            jsonTrack.insert("artists", jsonArrArtist);

            QJsonArray jsonArrThumbs;
            QJsonObject jsonThumb;
            jsonThumb.insert("url", map["art"].toString());
            jsonArrThumbs.append(jsonThumb);
            jsonTrack.insert("thumbnails", jsonArrThumbs);

            jsonArr.append(jsonTrack);

            procAdd_playlist->setProperty("thumbPath", map["art"].toString());
            procAdd_playlist->setProperty("playurl", map["_data"].toString());
        }
    }

    sqlite->close();
    delete sqlite;

    procAdd_playlist->setProperty("track", jsonArr);
    procAdd_playlist->showDlgOfPlayList();
}


/**
 * @brief MediaFileTree::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void MediaFileTree::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){

    if(p_index >=0 && p_index < model->rowCount(listView->rootIndex())){
        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
        QJsonObject json = modelIndex.data(Qt::UserRole).toJsonObject();
        json.insert("favorites", p_flagFavor);
        model->setData(modelIndex, json, Qt::UserRole);
    }
}

/**
 * @brief MediaFileTree::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void MediaFileTree::changeTrackFavoritesUI(const int &p_index){
    if(p_index >=0 && p_index < model->rowCount(listView->rootIndex())){
        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
        QJsonObject json = modelIndex.data(Qt::UserRole).toJsonObject();
        json.insert("favorites", !json["favorites"].toBool());
        model->setData(modelIndex, json, Qt::UserRole);
    }
}


/**
 * @brief MediaFileTree::showOptionPopup 옵션팝업 띄우기
 */
void MediaFileTree::showOptionPopup(int p_index){

    DataPopup *dataPopup = getTrackData(p_index);
    QJsonArray tmp_jsonArrTrack;
    tmp_jsonArrTrack.append(convertDataPopupToJsonObject(dataPopup));

    if(!dataPopup->getTitle().isEmpty()){
//        QJsonObject tmp_data = dataPopup->getJsonData();
        QJsonObject tmp_data = this->getPlayMusicInfoObject(p_index, dataPopup);
        QJsonArray tmp_array;
        tmp_array.append(tmp_data);

        QJsonObject tmp_jsonObject;
        tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, dataPopup->getTitle());
        tmp_jsonObject.insert(KEY_OP_TITLE_SUB, dataPopup->getArtist());
        tmp_jsonObject.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Folder);
        tmp_jsonObject.insert(KEY_OP_albumImg, dataPopup->getAlbum_art());
        tmp_jsonObject.insert(KEY_OP_cntStar, 0);
        tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
        tmp_jsonObject.insert(KEY_OP_MUSIC_PLAYLIST_ADD_DATA, tmp_jsonArrTrack);
        tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::MusicNetwork);
        tmp_jsonObject.insert("isAlbum", false); // 앨범이 아니라 곡임..
        emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, tmp_jsonObject);
    }
}

/**
 * @brief MusicList_One::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_index
 */
void MediaFileTree::requestGetTrackFavorites(const int &p_index){

    if(!global.user.getAccess_token().isEmpty()
            && p_index < model->rowCount(listView->rootIndex())){
        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
        QMap<QString,QVariant> map = modelIndex.data(Qt::UserRole).toMap();
        //QString tmp_filename = map["fileName"].toString();
        QStringList tmp_arrFilePath = map["filePath"].toString().split("/");
        tmp_arrFilePath.removeFirst();
        QString tmp_filePath = tmp_arrFilePath.join("/");


        QString tmp_url = "";
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery = "SELECT _data FROM audio ";
            //strQuery += " WHERE _display_name='"+tmp_filename+"'";
            strQuery += " WHERE _data LIKE '%"+tmp_filePath+"'";

            QVariantList data;
            sqlite->exec(strQuery, data);

            if(data.size() > 0){
                QMap<QString, QVariant> map = data.at(0).toMap();

               tmp_url = map["_data"].toString();
            }
        }

        sqlite->close();
        delete sqlite;

        if(!tmp_url.isEmpty()){
            QJsonObject tmp_json;
            tmp_json.insert("local_ip",global.device.getDeviceIP());
            tmp_json.insert("mac_address",global.device.getDeviceID());
            tmp_json.insert("url",tmp_url);

            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);
        }
    }
}

/**
 * @brief MediaFileTree::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void MediaFileTree::requestSetTrackFavorites(const int &p_index){
    if(!global.user.getAccess_token().isEmpty()
            && p_index < model->rowCount(listView->rootIndex())){

        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
        QMap<QString,QVariant> map = modelIndex.data(Qt::UserRole).toMap();
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
            //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_FAVORITE_ADD,  QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);

            QString tmp_thumb = tmp_jsonTrack["thumbnail"].toString();
            QString tmp_url = tmp_jsonTrack["url"].toString();

            // ----------------------------------------------------
            //  썸네일 업로드
            // ----------------------------------------------------
            if(!tmp_thumb.isEmpty() && !tmp_url.isEmpty()){
                QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_thumb);
                Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
                proc_uploadThumb->uploadThumnail(imgPath, tmp_url);
                connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
            }
        }else{
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            //connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp_play(int, QJsonObject)));
            network->request(HTTP_NETWORK_FAVORITE_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);
        }
    }
}

/**
 * @brief MediaFileTree::getTrackData : 트랙 데이터 반환
 * @param p_index int 트랙 인덱스
 * @return DataPopup*
 */
DataPopup* MediaFileTree::getTrackData(const int &p_index){

    DataPopup *dataPopup = new DataPopup(this);

    if(p_index >=0 && p_index < model->rowCount(listView->rootIndex())){
        QModelIndex modelIndex = model->index(p_index,0, listView->rootIndex());
        QMap<QString,QVariant> map = modelIndex.data(Qt::UserRole).toMap();
        QString tmp_filename = map["fileName"].toString();
        bool tmp_flagIsFile = map["isFile"].toBool();

        if(tmp_flagIsFile){
            SqliteHelper *sqlite = new SqliteHelper(this);
            QSqlError err = sqlite->addConnectionRose();
            if(err.type() == QSqlError::NoError){
                QString strQuery = "";
        //        strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data ";
        //        strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";
        //        strQuery += " WHERE _display_name='"+tmp_filename+"'";
                strQuery += "SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._data AS data, A.title, A.artist, A.duration, ART._data AS album_art ";
                strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
                strQuery += " WHERE _display_name='"+tmp_filename+"'";
                QVariantList data;
                sqlite->exec(strQuery, data);
                if(data.size() > 0){
                    dataPopup->setJsonData(data.at(0).toJsonObject());
                }
            }

            sqlite->close();
            delete sqlite;
        }
    }

    return dataPopup;
}

/**
 * @brief MediaFileTree::convertDataPopupToJsonObject : DataPopup 데이터를 QJsonObject 변환
 * @note 트랙정보를 플레이리스트담기용 QJsonObject로 변환한다.
 * @param p_dataPopup DataPopup 트랙정보 데이터
 * @return QJsonObject
 */
QJsonObject MediaFileTree::convertDataPopupToJsonObject(DataPopup *p_dataPopup){
    QJsonObject jsonTrack;

    // 앨범
    jsonTrack.insert("album", p_dataPopup->getAlbum());

    // 아티스트
    QJsonObject jsonArtist;
    jsonArtist.insert("name", p_dataPopup->getArtist());
    QJsonArray jsonArrArtists;
    jsonArrArtists.append(jsonArtist);
    jsonTrack.insert("artists", jsonArrArtists);

    // 클라이언트 아이디 = 오디오 아이디
    jsonTrack.insert("client_id", p_dataPopup->getId());

    // 썸네일
    QJsonObject jsonThumb;
    jsonThumb.insert("url", p_dataPopup->getAlbum_art());

    QJsonArray jsonArrThumbs;
    jsonArrThumbs.append(jsonThumb);
    jsonTrack.insert("thumbnails", jsonArrThumbs);

    // 타이틀
    jsonTrack.insert("title", p_dataPopup->getTitle());

    // 재생 URL
    jsonTrack.insert("url", p_dataPopup->getData());

    jsonTrack.insert("isFavorite", false);
    jsonTrack.insert("isSelected", false);
    jsonTrack.insert("mac_address", global.device.getDeviceID());
    jsonTrack.insert("type","music");


    jsonTrack.insert("thumbnail", p_dataPopup->getAlbum_art());     // 썸네일 업로드를 위한 추가


    return jsonTrack;
}

/**
 * @brief MediaFileTree::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject MediaFileTree::getJsonObject_TrackData(const int &p_index){

    DataPopup *dataPopup = getTrackData(p_index);
    return convertDataPopupToJsonObject(dataPopup);
}

/**
 * @brief MediaFileTree::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void MediaFileTree::setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_idx, tmp_favorite);
    }
}

/**
 * @brief MediaFileTree::setResultOfSetFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void MediaFileTree::setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData){
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
        ToastMsg::show(this,"",tr("Unable to change the Favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}

void MediaFileTree::dialog_comfirmCD()//cheon210812-iso
{
    dlgConfirmCD = new DialogConfirm(this);
    dlgConfirmCD->setTitle(tr("CD PLAY"));
    dlgConfirmCD->setTextHeight(150);
    dlgConfirmCD->setText(tr("Plays the selected song directly on CD PLAY."));
    dlgConfirmCD->setGeometry(this->width()/3,this->height()/2, 350,400);
    dlgConfirmCD->setAlertMode();
    dlgConfirmCD->setProperty("flagShown",false);
    if(dlgConfirmCD->property("flagShown").toBool()==false){
        dlgConfirmCD->setProperty("flagShown",true);

        int result = dlgConfirmCD->exec();

        if(result == QDialog::Accepted){
            PlayAllafterStopCheck(true);

        }
    }
    delete dlgConfirmCD;
}
void MediaFileTree::dialog_comfirmCD_Stop()//cheon210812-iso
{
    dlgConfirmCD = new DialogConfirm(this);
    dlgConfirmCD->setTitle(tr("CD PLAY"));
    dlgConfirmCD->setTextHeight(150);
    dlgConfirmCD->setText(tr("Stop the selected song directly on CD PLAY."));
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

/**
 * @brief MediaFileTree::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MediaFileTree::slot_responseHttp_stop(const int &p_id, const QJsonObject &p_jsonObject){//cheon210812-iso
    global.isoflag = false;//cheon210812-iso
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "MediaFileTree::slot_responseHttp---" << strJson;
    if(p_id == HTTP_NETWORK_PLAY){
        this->setResultOfNetworkPlay(p_jsonObject);
        //dialog_comfirmCD_Stop();//c220711


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
 * @brief MediaFileTree::setResultOfNetworkPlay : 네트워크 음원 재생 요청 결과 처리
 * @param p_jsonData QJsonObject response
 */
void MediaFileTree::setResultOfNetworkPlay(const QJsonObject &p_jsonData){//cheon210812-iso
    //Q_UNUSED(p_jsonData);
    global.isoflag = true;
    qDebug() << "start-global.isoflag---: " << global.isoflag;
    if(p_jsonData.contains("code")){//cheon210812-iso
        QString tmp_code = p_jsonData["code"].toString();
        bool tmp_codeOk = p_jsonData["flagOk"].toBool();
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
    qDebug() << "end-global.isoflag---: " << global.isoflag;

    // nothing
}


void MediaFileTree::slot_iso_play(){
    print_debug();
    playMusic_afterStopCheck(jsonArrMusic_curr, false);

}

void MediaFileTree::slot_responseHttp_play(const int &p_id, const QJsonObject &p_jsonObject){//cheon210812-iso

    global.isoflag = false;//cheon210812-iso
    qDebug() << "*end-global.isoflag---: " << global.isoflag;
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "MediaFileTree::slot_responseHttp---" << strJson;

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
    else if(p_id == HTTP_NETWORK_FAVORITE_GET){
        setResultOfGetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_FAVORITE_ADD){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_NETWORK_FAVORITE_DELETE){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    qDebug() << "*end-global.isoflag---: " << global.isoflag;
    sender()->deleteLater();
}

/*void MediaFileTree::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){//cheon210812-iso
    global.isoflag = false;
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "MediaFileTree::slot_responseHttp---" << strJson;

    if(p_id == HTTP_NETWORK_PLAY){
            this->setResultOfNetworkPlay_notiso(p_jsonObject);
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
    qDebug() << "*end-global.isoflag---: " << global.isoflag;
    sender()->deleteLater();
}

void MediaFileTree::setResultOfNetworkPlay_notiso(const QJsonObject &p_jsonData){
    Q_UNUSED(p_jsonData);
    // nothing
}*/

/**
 * @brief MediaFileTree::slot_reqThumbnail : [슬롯] 트랙 썸네일 요청
 * @param p_index int index
 * @param p_filePath QString 파일경로
 * @param p_fileName QString 파일명
 */
void MediaFileTree::slot_reqThumbnail(const int &p_index, const QString &p_filePath, const QString &p_fileName){

    QString tmp_filePath = p_filePath;
    QString tmp_fileName = p_fileName;
    QUuid tmp_uuid = uuid;
    int tmp_index = p_index;

    MediaPlayerThread *thread = new MediaPlayerThread(tmp_filePath,tmp_index, this);
    thread->setProperty("uuid", tmp_uuid);
    thread->setProperty("fileName", tmp_fileName);
    thread->setProperty("index",tmp_index);


    connect(thread, SIGNAL(signal_loadedMedia(QJsonObject,QPixmap)), this, SLOT(slot_loadedMediaFile(QJsonObject, QPixmap)));
    QObject::connect(thread, &MediaPlayerThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

/**
 * @brief MediaFileTree::slot_reqFavorites
 * @param p_index
 */
void MediaFileTree::slot_reqFavorites(const int &p_index){
    requestGetTrackFavorites(p_index);
}

/**
 * @brief MediaFileTree::slot_directoryLoaded : [슬롯] 파일시스템모델 로드 완료
 * @details 파일정보를 이용하여 트랙 목록 구성<br>
 * 미디어정보는 QMediaPlayer를 이용해 따로 요청및 구성한다.<br>
 * @param p_path
 */
void MediaFileTree::slot_directoryLoaded(QString p_path){

    model->sort(0, Qt::AscendingOrder);
    QModelIndex index = model->index(p_path);
    model->setFileInfoStruct(model->rowCount(index));
}

/**
 * @brief MediaFileTree::loadMediaMimeData : 미디어 파일 마임데이터 요청
 * @note 미사용, QListWidget -> QListView 로 변경하면서 미사용함
 */
void MediaFileTree::loadMediaMimeData(const int &p_index){

    if(p_index >=0 && p_index < list_fileLoadStruct->count()){

        loadMediaLastIndex = p_index > loadMediaLastIndex ? p_index : loadMediaLastIndex;

        QString tmp_filePath = list_fileLoadStruct->at(p_index).filePath;
        QString tmp_fileName = list_fileLoadStruct->at(p_index).fileName;
        QUuid tmp_uuid = list_fileLoadStruct->at(p_index).uuid;
        int tmp_index = list_fileLoadStruct->at(p_index).index;

        MediaPlayerThread *thread = new MediaPlayerThread(tmp_filePath,tmp_index, this);
        thread->setProperty("uuid", tmp_uuid);
        thread->setProperty("fileName", tmp_fileName);
        thread->setProperty("index",tmp_index);

        connect(thread, SIGNAL(signal_loadedMedia(QJsonObject,QPixmap)), this, SLOT(slot_loadedMediaFile(QJsonObject, QPixmap)));
        QObject::connect(thread, &MediaPlayerThread::finished, thread, &QObject::deleteLater);

        thread->start();
    }
}

/**
 * @brief MediaFileTree::slot_loadedMediaFile 미디오 파일 정보 로딩 완료
 * @param p_json QJsonObject 미디어 파일 정보
 * @param p_pixmap QPixmap 썸네일 이미지
 */
void MediaFileTree::slot_loadedMediaFile(const QJsonObject &p_json, const QPixmap &p_pixmap){


    QUuid tmp_uuid = sender()->property("uuid").toUuid();

    if(tmp_uuid == uuid){
        const QString jsonKey_author = "author";
        const QString jsonKey_albumTitle = "albumTitle";
        const QString jsonKey_duration = "duration";

        QString tmp_author = p_json[jsonKey_author].toString();
        QString tmp_albumTitle = p_json[jsonKey_albumTitle].toString();
        QString tmp_duration = p_json[jsonKey_duration].toString();
        QString tmp_fileName = sender()->property("fileName").toString();
        int tmp_index = p_json["index"].toInt();

        if(tmp_index >=0 && tmp_index < model->rowCount(listView->rootIndex())){
            QModelIndex modelIndex = model->index(tmp_index,0, listView->rootIndex());

            QJsonObject json = modelIndex.data(Qt::UserRole).toJsonObject();

            json.insert("fileName", tmp_fileName);
            json.insert("duration", tmp_duration);
            json.insert("author", tmp_author);
            json.insert("albumTitle", tmp_albumTitle);

            model->setData(modelIndex, json, Qt::UserRole);
            model->setData(modelIndex, p_pixmap, Qt::DecorationRole);
        }
    }
}

/**
 * @brief MediaFileTree::slot_changedRootPath : [슬롯] Root Path 변경
 * @details Path 변경 처리<br>
 * 목록 초기화, FieSystemModel RootPath 변경
 * @param p_dirName
 * @param p_dirPath
 */
void MediaFileTree::slot_changedRootPath(const QString &p_dirName, const QString &p_dirPath){

    QString tmp_dirPath = p_dirPath;

    // smb path => file path로 변환
    if(p_dirPath.startsWith("smb://", Qt::CaseInsensitive)){
        tmp_dirPath = convertSMBPathToFilePath(p_dirPath);
    }

    // 네트워크 파일 시작 문자열 검사
    if(!tmp_dirPath.startsWith("//")){
        tmp_dirPath = "//"+p_dirPath;
    }


    // 현재 path와 다른 경우에는 reDraw
    if(tmp_dirPath != currRootPath){
        loadMediaLastIndex = 0;
        list_fileLoadStruct->clear();
        currRootPath = tmp_dirPath;
        uuid = QUuid::createUuid();
        //model->setRootPath(tmp_dirPath);
        model->clearData();
        listView->setRootIndex(model->setRootPath(tmp_dirPath));
    }

    // Top 가로형 DIR Tree 변경을 위한 시그널 호출
    // ReDraw 하지 않아도 항시 호출 필요.
    // (Top 가로형 DIR Tree에서 DIR 클릭시 해당 DIR 삭제되기 떄문에 다시 추가해줘야함.)
    emit signal_clickedDirName(p_dirName, p_dirPath);
}

/**
 * @brief MediaFileTree::slot_clickedPlayAll : [슬롯] 전체 재생
 */
void MediaFileTree::slot_clickedPlayAll(){//cheon210812-iso
    qDebug() << "MediaFileTree::slot_clickedPlayAll--000000000000000000009999999999999999";
    playMusic(getPlayMusicInfoArray(-1));
}

void MediaFileTree::PlayAllafterStopCheck(const bool &flag){//cheon210812-iso

    qDebug() << "MediaFileTree::slot_clickedPlayAllafterCheck--000000000000000000009999999999999999="<< flag;
    //if(!flag){

        playMusic_afterStopCheck(jsonArrMusic_curr, false);
    //}
}

/**
 * @brief MediaFileTree::slot_clickedPlayShuffle : [슬롯]
 */
void MediaFileTree::slot_clickedPlayShuffle(){
    playMusic(getPlayMusicInfoArray(-1), true);
}

/**
 * @brief MediaFileTree::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void MediaFileTree::slot_playlistClicked(const int &p_playlistno){

    QJsonArray jsonArrTrack = sender()->property("track").toJsonArray();

    if(p_playlistno >0){
        procAdd_playlist->requestAddPlayListTrack(p_playlistno,jsonArrTrack);

        QString tmp_thumb = procAdd_playlist->property("thumbPath").toString();
        QString tmp_url = procAdd_playlist->property("playurl").toString();
        // ------------------------------------------------
        //  썸네일 업로드
        // ------------------------------------------------
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
 * @brief MediaFileTree::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void MediaFileTree::slot_newPlaylistNameEdited(QString p_name){

    QJsonArray jsonArrTrack = sender()->property("track").toJsonArray();
    QString tmp_playurl = sender()->property("playurl").toString();

    if(pixmapThumb.size().width() < 100){
        // def_mus_60.png 기본 이미지인 경우: 이미지 로딩 실패
        procAdd_playlist->requestNewPlaylistTrack(p_name, jsonArrTrack);
    }else{
        procAdd_playlist->requestNewPlaylistTrack(pixmapThumb, tmp_playurl, p_name, jsonArrTrack);
    }

}

