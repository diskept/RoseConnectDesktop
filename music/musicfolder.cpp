#include "common/global.h"
#include "musicfolder.h"

#include "widget/toastmsg.h"

#include <common/gscommon.h>
#include <common/networkhttp.h>
#include <QScroller>

const int MENU_BAR_LEFT_W = 707;
const int MENU_BAR_HEIGHT = 70;
const int BTN_FAV_W = 125;
const int BTN_FAV_H = 40;

const int HTTP_FAVORITE_LIST = 99;

/**
 * @brief The MusicFolder class 음악 > 폴더 메뉴 화면
 * @details 크게 2개의 위젯을 StackWidget으로 관리하고 있다. 2개의 위젯은 다음과 같이 구성되어있다.
 * 하나는 탐색(USB)관련 화면의 위젯이고 다른 하나는 즐겨찾기관련 화면의 위젯이다.
 * @note 즐겨찾기 각각 버튼 클릭할때 마다 페이지가 변경되었다는 시그널을 발생시킨다.
 * 탐색(USB) depth가 늘어날때마다 페이지가 변경되었다는 시그널을 발생시킨다.
 * @param parent
 */
MusicFolder::MusicFolder(QWidget *parent, bool p_flagIsMusic)
    : AbstractPage(parent)
    , flagIsMusic(p_flagIsMusic)
{

    setInit();
    setUIFarovite(parent);//c221213
    setUIControl();
    //setUIFarovite(parent);//c221213
}


MusicFolder::~MusicFolder(){

    this->deleteLater();
}


/**
 * @brief MusicFolder::hide_topBtnFavorite
 */
void MusicFolder::hide_topBtnFavorite(){
    widget_favoriteGroup->hide();
}

/**
 * @brief MusicFolder::setInit : 초기 세팅 함수
 */
void MusicFolder::setInit(){
    linker = Linker::getInstance();

    connect(linker, SIGNAL(signal_favoriteFolderChanged()), this, SLOT(slot_favoriteFolderChanged()));
}

/**
 * @brief MusicFolder::setUIControl : UI 세팅
 */
void MusicFolder::setUIControl(){

    this->music_usb = new MusicFolder_USB(this, flagIsMusic);
    this->music_file = new MusicFolder_FileList(this, flagIsMusic);
    this->music_usbFile = new MusicFolder_USBFileList(this, flagIsMusic);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->music_usb);
    this->stackedwidget->addWidget(this->music_file);
    this->stackedwidget->addWidget(this->music_usbFile);

    this->vl_total= new QVBoxLayout();
    this->vl_total->setContentsMargins(0,0,0,0);
    this->vl_total->setSpacing(0);
    this->vl_total->setAlignment(Qt::AlignTop);

    this->vl_total->addWidget(this->widget_favoriteGroup);//c221213
    this->vl_total->addWidget(this->stackedwidget);

    setLayout(this->vl_total);

    connect(music_usb, SIGNAL(clickedNetworkFolder(QString, QString, QString)), SLOT(slot_openNetworkFolderFileList(QString, QString, QString)));
    connect(music_usb, SIGNAL(clickedUSB(QString, QJsonObject)), SLOT(slot_openUSBFileList(QString, QJsonObject)));
}

/**
 * @brief MusicFolder::setUIFarovite : 상단 즐겨찾기 UI 세팅
 * @param parent
 */
void MusicFolder::setUIFarovite(QWidget *parent){

    this->hl_btnFavorite = new QHBoxLayout();
    this->hl_btnFavorite->setSpacing(0);
    this->hl_btnFavorite->setContentsMargins(0,0,0,0);
    this->hl_btnFavorite->setAlignment(Qt::AlignLeft);



    QWidget *widget_btnFavorite = new QWidget;
    widget_btnFavorite->setObjectName("widget_btnFavorite");
    widget_btnFavorite->setLayout(this->hl_btnFavorite);
    widget_btnFavorite->setStyleSheet("background-color:transparent;");

    scrollFavorite = new QScrollArea();
    scrollFavorite->setWidget(widget_btnFavorite);
    scrollFavorite->setWidgetResizable(true);
    scrollFavorite->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollFavorite->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollFavorite->setFixedHeight(BTN_FAV_H);
    scrollFavorite->setStyleSheet("border:none;");

    QScroller::grabGesture(scrollFavorite, QScroller::LeftMouseButtonGesture);


    QHBoxLayout *layt = new QHBoxLayout;
    layt->addWidget(scrollFavorite);
    layt->setContentsMargins(0,0,0,0);

    QWidget *widget_scroll = new QWidget;
    widget_scroll->setContentsMargins(20,0,20,0);
    widget_scroll->setLayout(layt);
    //widget_scroll->setStyleSheet("border:1px solid #707070;border-radius:20px;");//c221213



    // ----------------------------------------------------------------
    //   즐겨찾기 편집 BTN
    // ----------------------------------------------------------------
    QPushButton *btn_edit = GSCommon::getUIBtnImg("btn_edit", ":/images/fol_edit_ico.png");
    btn_edit->setCursor(Qt::PointingHandCursor);
    btn_edit->setFixedSize(50,70);


    // ----------------------------------------------------------------
    //   박스 구성 및 즐겨찾기 그룹 Widget 세팅
    // ----------------------------------------------------------------
    hl_favoriteGroup = new QHBoxLayout;
    hl_favoriteGroup->setSpacing(14);
    hl_favoriteGroup->setContentsMargins(0,0,0,0);
    hl_favoriteGroup->addWidget(widget_scroll,1,Qt::AlignVCenter);
    hl_favoriteGroup->addWidget(btn_edit);
    hl_favoriteGroup->addSpacing(13);

    widget_favoriteGroup = new QWidget(parent);
    widget_favoriteGroup->setContentsMargins(0,0,0,0);
    widget_favoriteGroup->setLayout(hl_favoriteGroup);
    widget_favoriteGroup->setStyleSheet("background-color:transparent;");
    widget_favoriteGroup->hide();


    // 커넥트
    connect(btn_edit, SIGNAL(clicked()), this, SLOT(slot_clickedBtnEditFavorite()));

}

/**
 * @brief MusicFolder::requestBtnListFavorite 상단 즐겨찾기 버튼 목록 생성하기 위해 API 호출
 */
void MusicFolder::requestBtnListFavorite(){

    if(!global.device.getDeviceIP().isEmpty()){

        QJsonObject tmp_json;
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_LIST, QString("http://%1:%2/media.album.folder.favs.get")
                         .arg(global.device.getDeviceIP()).arg(global.port), tmp_json, true);
    }
}



/**
 * @brief MusicFolder::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicFolder::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_FAVORITE_LIST){
        // 상단 즐겨찾기 버튼 목록 생성 및 세팅
        this->setResultOfFavoriteList(p_jsonObject);
    }

    sender()->deleteLater();
}


/**
 * @brief MusicFolder::setUIControl_btnListFavorite 상단 즐겨찾기 버튼 목록 생성 및 세팅
 * @param p_jsonObject
 */
void MusicFolder::setResultOfFavoriteList(QJsonObject p_jsonObject){

    // clear layout
    GSCommon::clearLayout(this->hl_btnFavorite);

    QLabel *lb_favor_title = new QLabel;//c221213
    lb_favor_title->setContentsMargins(20,0,20,0);
    lb_favor_title->setFixedWidth(130);
    lb_favor_title->setText("Favorite : ");
    lb_favor_title->setStyleSheet("color:#CCCCCC;font-size:18px;");
    this->hl_btnFavorite->addWidget(lb_favor_title);//c221213

    //set data
    if(p_jsonObject.contains("data")){
        jsonArrFavor = p_jsonObject["data"].toArray();

        for(int i=0 ; i<jsonArrFavor.count(); i++){

            QString tmp_albumName = "";
            QString tmp_albumFavPath = "";
            QJsonObject tmp_data = jsonArrFavor.at(i).toObject();
            if(tmp_data.contains("albumFavName")){
                tmp_albumName = tmp_data["albumFavName"].toString();
            }
            if(tmp_data.contains("albumFavPath")){
                tmp_albumFavPath = tmp_data["albumFavPath"].toString();
            }

            QPushButton *btn_fav = new QPushButton();
            btn_fav->setText(GSCommon::getTextCutFromLabelWidth(tmp_albumName, BTN_FAV_W-10, btn_fav->font()));
            if(btn_fav->text().contains("…")){
                btn_fav->setToolTip(tmp_albumName);
                btn_fav->setToolTipDuration(2000);
            }
            btn_fav->setFixedSize(BTN_FAV_W, BTN_FAV_H-2);
            btn_fav->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), QString("fav_%1_%2").arg(tmp_albumFavPath.startsWith("smb:",Qt::CaseInsensitive) ? "network" : "usb").arg(i));
            btn_fav->setProperty("albumFavName", tmp_albumName);
            btn_fav->setProperty("albumFavPath", tmp_albumFavPath);
            btn_fav->setCursor(Qt::PointingHandCursor);
print_debug();
qDebug() << "jsonArrFavor.size()=" << jsonArrFavor.size();
            if(i < (jsonArrFavor.size()-1)){
               //btn_fav->setStyleSheet("color:#CCCCCC;font-size:18px;border:none;border-right:1px solid #707070;");//c221213
               btn_fav->setStyleSheet("color:#CCCCCC;font-size:18px;border:1px solid #707070;border-radius:17px;");//c221213
            }else{
               //btn_fav->setStyleSheet("color:#CCCCCC;font-size:18px;");//c221213
               btn_fav->setStyleSheet("color:#CCCCCC;font-size:18px;;border:1px solid #707070;border-radius:17px;");//c221213
            }

            this->hl_btnFavorite->addSpacing(5);
            this->hl_btnFavorite->addWidget(btn_fav, 0, Qt::AlignLeft);
            connect(btn_fav, SIGNAL(clicked()), this, SLOT(slot_changedSubTab_favorite()));
        }

        // 즐겨찾기 위젯 위치 설정
        setGeometryFavoriteUI();

        // 즐겨찾기 항목 있는 경우 Show
        if(hl_btnFavorite->count() > 0){
            this->widget_favoriteGroup->show();
        }else{//c221213
            this->widget_favoriteGroup->hide();
        }
    }
}




/**
 * @brief MusicFolder::setUIControl_USB USB페이지 관련 UI 세팅
 */
void MusicFolder::setUIControl_USB(){
    // 1) 데이터 세팅
    this->music_usb->setData();

    // 2) 스택 변경
    this->stackedwidget->setCurrentWidget(this->music_usb);

}

/**
 * @brief MusicFolder::setUIControl_favorite 즐겨찾기 관련 UI 세팅
 * @param p_favoriteType 즐겨찾기 유형 (=즐겨찾기 각 버튼의 code값)
 * @param p_favName 즐겨찾기명
 * @param p_albumFavPath QString 폴더 경로 :: smb://{id}:{pw}@{path}
 * @note WINDOWS, MAC 구분 처리 필요
 */
void MusicFolder::setUIControl_favorite(QString p_favoriteType, QString p_favName, QString p_albumFavPath){

    // 1) 데이터 세팅
    this->contentStep = p_favoriteType;
    this->music_file->setData(p_favName,p_albumFavPath);

    // 2) 스택 변경
    this->stackedwidget->setCurrentWidget(music_file);
}

void MusicFolder::setUIControl_usbFileList(QString p_contentStep, QJsonObject p_json){
   this->contentStep = p_contentStep;

   music_usbFile->setData(p_json);
   stackedwidget->setCurrentWidget(music_usbFile);
}

/**
 * @brief MusicFolder::slot_openUSBFileList : [슬롯]USB 폴더 파일목록 페이지 이동
 * @param p_jsonObject
 */
void MusicFolder::slot_openUSBFileList(const QString &p_contentStep, const QJsonObject &p_jsonObject){
    //print_debug();
#if defined(Q_OS_WIN)
    setUIControl_usbFileList(p_contentStep, p_jsonObject);

    QJsonObject json = getDataJson();
    json.insert(KEY_DATA, p_jsonObject);


    this->slot_changedSubTabUI(json);

#elif defined(Q_OS_MAC)//cheon210831-network
    setUIControl_usbFileList(p_contentStep, p_jsonObject);

    QJsonObject json = getDataJson();
    json.insert(KEY_DATA, p_jsonObject);


    this->slot_changedSubTabUI(json);
    //ToastMsg::show(this, "", tr("*The current version is not supported by MAC Desktop.\n It will be supported in a future version.."));//cheon210825-network
#endif
}

/**
 * @brief MusicFolder::openNetworkFolderFileList : [슬롯]네트워크 폴더 파일목록 페이지 이동
 * @param p_contentStep
 * @param p_path
 * @not
 */
void MusicFolder::slot_openNetworkFolderFileList(const QString &p_contentStep, const QString &p_rootName, const QString &p_path){//c220913_1

    //print_debug();
#if defined(Q_OS_WIN)
    this->setUIControl_favorite(p_contentStep, p_rootName, p_path);

    QJsonObject json = getDataJson();
    json.insert("favName", p_rootName);
    json.insert("albumFavPath", p_path);


    // 2) 시그널 발생
    this->slot_changedSubTabUI(json);

#elif defined(Q_OS_MAC)//cheon210831-network
    //print_debug();
    QString tmp_id ="", tmp_pw="", tmp_p_rootName="",tmp_p_path = "";
    qDebug() << "MusicFolder::slot_openNetworkFolderFileList--p_path : " << p_path;
    QString tmp_smbPath = p_path.split("//").last();
    qDebug() << "MusicFolder::slot_openNetworkFolderFileList--002--tmp_smbPath : " << tmp_smbPath;
    if(tmp_smbPath.contains(":") && tmp_smbPath.contains("@")){
        qDebug() << "MusicFolder::slot_openNetworkFolderFileList--003--tmp_smbPath.split(:).first() : " << tmp_smbPath.split(":").first();
        tmp_id = tmp_smbPath.split(":").first();
    }

    QString tmp_smbPath2 = p_path.split("//").last();

    if(tmp_smbPath.contains(":") && tmp_smbPath.contains("@")){
        tmp_pw =  tmp_smbPath2.split(":").last().split("@").first();
    }


    QStringList tmp_dirpath = tmp_smbPath2.split("/");
    //print_debug();
    qDebug() << "tmp_id: " << tmp_id << ", tmp_pw: " << tmp_pw << ", tmp_dirpath[1]: " << tmp_dirpath[1];

    global.p_rootName = p_rootName;//cheon210825-network
    global.p_path = p_path;//cheon210825-network
    //
    //p_rootName: Media1
    //p_path: smb://rose:Citech1234@192.168.1.4/Media1/

    //qDebug() << "MusicFolder::slot_openNetworkFolderFileList---001-309" ;
    //QString p_rootNametmp ="Media1";
    //QString p_pathtmp ="/Volumes/Media1/";
    QString p_rootNametmp =tmp_dirpath[1];
    QString ip = p_path.split("@").last().split("/").first();//c220913_2
    QString p_pathtmp ="/Volumes/" + ip+"-"+tmp_dirpath[1] + "/";//c220913_2
    //QString p_pathtmp ="/Volumes/" + tmp_dirpath[1] + "/";


    //this->setUIControl_favorite(p_contentStep,p_rootName,p_path);
    this->setUIControl_favorite(p_contentStep,p_rootNametmp,p_pathtmp);
    QJsonObject json = getDataJson();
    //print_debug();
    qDebug() << "p_pathtmp" << p_pathtmp;
    //json.insert("favName", p_rootName);
    json.insert("favName", p_pathtmp);
    //json.insert("albumFavPath", p_path);
    json.insert("albumFavPath", p_pathtmp);

    // 2) 시그널 발생
    this->slot_changedSubTabUI(json);

#endif
}

/**
 * @brief MusicFolder::slot_clickedBtnEditFavorite [SLOT] 즐겨찾기 편집 아이콘 클릭시
 */
void MusicFolder::slot_clickedBtnEditFavorite(){
    emit this->signal_clickedFavoriteFolderEdit(jsonArrFavor);
}
/**
 * @brief MusicFolder::slot_changedSubTab_favorite [SLOT] 즐겨찾기 버튼 클릭시 페이지 변경
 */
void MusicFolder::slot_changedSubTab_favorite(){
print_debug();
    // 1) 페이지 변경 및 데이터 세팅
    QString tmp_contentStep = this->sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();
    QString tmp_favName = this->sender()->property("albumFavName").toString();
    QString tmp_albumFavPath = this->sender()->property("albumFavPath").toString();

    if(tmp_albumFavPath.startsWith("smb://", Qt::CaseInsensitive)){
        slot_openNetworkFolderFileList(tmp_contentStep, tmp_favName, tmp_albumFavPath);
    }else{
        // USB
        QJsonObject json;
        json.insert("ext","");
        json.insert("id","");
        json.insert("ip","");
        json.insert("isCloud",false);
        json.insert("isFile",false);
        json.insert("isServer",false);
        json.insert("isShare",false);
        json.insert("isUp",false);
        json.insert("name", tmp_favName);
        json.insert("path", tmp_albumFavPath);
        json.insert("pw","");
        json.insert("rcloneServe","");
        json.insert("smbParamPath","");
        json.insert("thumbnail","");

        slot_openUSBFileList(tmp_contentStep,json);
    }


}

/**
 * @brief MusicFolder::changedSubTab [SLOT][override]
 * @param p_data
 */
void MusicFolder::slot_changedSubTabUI(const QJsonObject &p_data){


    emit clickedSubTab(p_data);
}

/**
 * @brief MusicFolder::slot_goToInitiPageFolder [SLOT] 폴더의 초기 페이지로 이동하라
 * @param p_subMenuCode
 */
void MusicFolder::slot_goToInitiPageFolder(QString p_subMenuCode){

    if(p_subMenuCode==PAGECODE_M_FOLDER || p_subMenuCode==PAGECODE_V_FOLDER){
        this->music_usb->goInitPage();
        this->stackedwidget->setCurrentWidget(this->music_usb);
    }
}


/**
 * @brief MusicFolder::setData [override]
 * @param p_data
 */
void MusicFolder::setDataABSPage(QJsonObject p_data){
    // 공통 데이터 세팅
    AbstractPage::setDataABSPage(p_data);


    // 상단 즐겨찾기 버튼 목록 생성하기 위해 API 호출
    if(flagIsMusic){
        this->requestBtnListFavorite();
    }

    // step 있는경우
    if(this->contentStep==""){
        // USB 탐색 StackWidget
        this->setUIControl_USB();
    }
    else if(contentStep.startsWith("netfolder") || contentStep.startsWith("fav_network")){
        // netforlder_x : 네트워크 폴더, favx : 즐겨찾기 폴더
        QString tmp_favName;
        QString tmp_ablumFavPath;
        if(p_data.contains("albumFavPath")){
            tmp_ablumFavPath = p_data["albumFavPath"].toString();
        }
        if(p_data.contains("favName")){
            tmp_favName = p_data["favName"].toString();
        }
        // 즐겨찾기 StackWidget
        this->setUIControl_favorite(this->contentStep,tmp_favName,tmp_ablumFavPath);
    }
    else if(contentStep.startsWith("usb") || contentStep.startsWith("fav_usb")){
        // usb_x : 유에스비 드라이브
        QJsonObject jsonUsb = p_data[KEY_DATA].toObject();

        this->setUIControl_usbFileList(contentStep, jsonUsb);
    }

}

/**
 * @brief MusicFolder::getDataJson [override]
 * @return
 */
QJsonObject MusicFolder::getDataJson(){

    // 공통 데이터 반환
    QJsonObject tmp_data = AbstractPage::getDataJson();

    return tmp_data;
}

/**
 * @brief MusicFolder::setGeometryFavoriteUI : 즐겨찾기 UI 위치 설정
 */
void MusicFolder::setGeometryFavoriteUI(){

    int tmp_cntFavor = hl_btnFavorite->count();
    int tmp_favorGroup_w = tmp_cntFavor*BTN_FAV_W+120;

    if(tmp_favorGroup_w > this->width()-MENU_BAR_LEFT_W ){
        tmp_favorGroup_w = this->width()-MENU_BAR_LEFT_W;
    }


    //this->widget_favoriteGroup->setGeometry(width()-tmp_favorGroup_w, 0, tmp_favorGroup_w, MENU_BAR_HEIGHT);//c221213
    //this->widget_favoriteGroup->setGeometry(width()-tmp_favorGroup_w, 0, tmp_favorGroup_w, MENU_BAR_HEIGHT);//c221213
}


/**
 * @brief MusicFolder::resizeEvent 리사이즈 될때 처리
 * @param event QResizeEvent*
 */
void MusicFolder::resizeEvent(QResizeEvent* event){
    Q_UNUSED(event);
    setGeometryFavoriteUI();
}

/**
 * @brief MusicFolder::slot_favoriteFolderAdded : [슬롯] 즐겨찾기 추가 등록
 */
void MusicFolder::slot_favoriteFolderChanged(){
    requestBtnListFavorite();
}
