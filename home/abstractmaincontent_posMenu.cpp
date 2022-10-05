#include "common/global.h"
#include "abstractmaincontent.h"

#include <QDebug>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>

#include <common/gscommon.h>
#include <common/gscommon.h>
#include <common/networkhttp.h>


const int HTTP_FAVORITE_ADD = 99;

AbstractMainContent::AbstractMainContent(QWidget *parent) : QWidget(parent)
{
    linker = Linker::getInstance();

    this->indexPageOrdering_curr = -1;
    this->list_pageCodeOrdering = new QList<QString>;
    this->list_pageDataOrdering = new QList<QJsonObject>;

    /*
    lb_roseTubeBG = new QLabel();
    lb_roseTubeBG->setStyleSheet("background-color:transparent;");
    lb_roseTubeBG->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QVBoxLayout *vl_roseTubeBG = new QVBoxLayout();
    vl_roseTubeBG->setContentsMargins(0,0,0,0);
    vl_roseTubeBG->setSpacing(0);
    vl_roseTubeBG->addWidget(this->lb_roseTubeBG);
    */

    // 로즈튜브에서 사용하는 배경 위젯
    widget_roseTubeBG = new QWidget();
    widget_roseTubeBG->setObjectName("widget_roseTubeBG");
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG { background-color:#171717; }");
    widget_roseTubeBG->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //widget_roseTubeBG->setLayout(vl_roseTubeBG);

    lb_roseTubeBG = new QLabel(widget_roseTubeBG);
    lb_roseTubeBG->setStyleSheet("background-color:transparent;");
    lb_roseTubeBG->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lb_roseTubeBG->hide();


    // 네비 Bar
    topNaviBar = new TopNaviBar(widget_roseTubeBG);
    widget_roseTubeBG->setFixedHeight(topNaviBar->height());

    // 상단 메뉴 Bar
    topMenuBar = new TopMenuBar();

    // 실 컨텐츠 레이아웃
    stackedWidget_content = new QStackedWidget();

    // 음악/비디오/라디오/로즈튜브 등등 전체 메인메뉴에서 공통으로 사용하는 Page
    this->playFullScreenRelation = new PlayFullScreenRelation();
    this->stackedWidget_content->addWidget(this->playFullScreenRelation);

    // 레이아웃
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    //vlayout->addWidget(this->topNaviBar, 0, Qt::AlignTop);
    vlayout->addWidget(this->widget_roseTubeBG, 0, Qt::AlignTop);
    vlayout->addSpacing(1);
    vlayout->addWidget(this->topMenuBar, 0, Qt::AlignTop);
    vlayout->addSpacing(1);
    vlayout->addWidget(this->stackedWidget_content, 9);

    QWidget *widget = new QWidget();
    widget->setObjectName("sectionContent");
    widget->setStyleSheet("background-color:#212121;");
    widget->setLayout(vlayout);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget);

    this->setLayout(vl_total);


    //this->optionPopupNew = new OptionPopup();
    //this->optionPopupOld = new OptionPopupOld(this);

    // 커넥트
    //connect(this->optionPopupNew, SIGNAL(signal_clickedMovePage(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)));
    connect(this->topMenuBar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)) );
    connect(this->topNaviBar, SIGNAL(clickedBack()), this, SLOT(goToBackPage()));
    connect(this->topNaviBar, SIGNAL(clickedNext()), this, SLOT(goToNextPage()));
    connect(this->topNaviBar, SIGNAL(clickedRemote(bool)), this, SIGNAL(showRemoteWidget(bool)));
    //connect(this->remoteWidget, SIGNAL(clickedVolumnSlide(int)), this, SLOT(changedVolumnSlide(int)));

    connect(linker, SIGNAL(signal_clickedHoverItem(QString, QJsonObject)), this, SLOT(slot_clickedHoverItem(QString, QJsonObject)));
    connect(linker, SIGNAL(signal_showPlayFullScreen(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)));
    connect(linker, SIGNAL(signal_changeRoseTubeBG(QPixmap)), this, SLOT(slot_changeRoseTubeBG(QPixmap)));

}


/**
 * @brief AbstractMainContent::slot_clickedHoverItem [SLOT] 앨범 hover 위 more/play/fav 아이콘을 클릭시 처리
 * @param p_code 더보기/재생/하트 (more/play/fav)
 * @param p_jsonData
 */
//void AbstractMainContent::slot_clickedHoverItem(const QString &p_code, const QJsonObject &p_jsonData){
void AbstractMainContent::slot_clickedHoverItem(QString p_code, QJsonObject p_jsonData){
    //qDebug() << " ";
    //qDebug()<< "AbstractMainContent::slot_clickedHoverItem(QString p_code, QJsonObject p_jsonData)" << p_code;
    //qDebug() << "       " << p_jsonData;
    //qDebug() << QCursor::pos() << QWidget::mapFromGlobal(QCursor::pos()) << " QMainWindow (x,y) " << global.x_mainwindow << global.y_mainwindow;
    //qDebug() << QCursor::pos() << QWidget::mapFromGlobal(QCursor::pos()) << QWidget::mapFromGlobal(this->pos());
    //qDebug() << "QCursor::pos " << QCursor::pos() << "  / QWidget::mapFromGlobal(this->pos()) " << QWidget::mapFromGlobal(this->pos());


    // hide가 안되고 새로 생성되는 경우도 잇어서..항상 생성 직전에 delete
    for(int i=0; i<this->list_popup.count(); i++){
        this->list_popup.at(i)->hide();
        delete this->list_popup.at(i);
        this->list_popup.clear();
    }
    // OptionPopup 멤버변수로 선언하여 불필요한 지속적인 생성을 막으려 했지만..
    // QMenu 특성(?)으로 인한 SizeHint를 이용하여 pos 값을 결정하는데 SizeHint가 제대로 세팅되지 않아 어려움이 있어(show전후로 크기가 다름뿐 아니라 값도 정확치않음) 매번 새로 생성하는것으로 대체하여 SizeHint가 예상된 값으로 나온다.
    OptionPopup *optionPopupNew = new OptionPopup();
    connect(optionPopupNew, SIGNAL(signal_clickedMovePage(QJsonObject)), this, SLOT(goToMoveNewOrderPage(QJsonObject)));
    this->list_popup.append(optionPopupNew);

    // 옵션팝업에 데이터 세팅
    if(p_jsonData.contains(KEY_MAIN_CODE)==false){
        p_jsonData.insert(KEY_MAIN_CODE, this->topMenuBar->getCurrMainMenuCode());  // key 추가
    }
    optionPopupNew->setDataPopup(p_jsonData);



    if(p_code=="more"){

        /*
        QPoint pos = QCursor::pos();
        int tmp_posX = pos.x();
        int tmp_posY = pos.y();
        if( (global.height_mainwindow - tmp_posY) < optionPopupNew->height() ){
            tmp_posY = tmp_posY - optionPopupNew->height();
        }
        if( (global.width_mainwindow - tmp_posX) < optionPopupNew->width() ){
            tmp_posX = tmp_posX - optionPopupNew->width();
        }
        qDebug() << "    " << tmp_posX << tmp_posY;
        // 위치 계산 예외상황 : 스크롤 하단으로 내린상태에서 맨 아래 에서 클릭할때 스크롤이 쭉 올라가버린다.
        // 우선 항상 클릭한 위치에 팝업 보이게 한다.

        //optionPopupOld->setGeometry(tmp_posX, tmp_posY, optionPopupOld->width(), optionPopupOld->height());
        //optionPopupOld->setPositionOptionPopup(tmp_posX, tmp_posY);
        //optionPopupOld->show();
        */

        QPoint pos_mainWindow = QWidget::mapFromGlobal(this->pos());
        QPoint pos = QCursor::pos();
        int tmp_posX = pos.x();
        int tmp_posY = pos.y();

        //qDebug() << "OptionPopup size : " << optionPopupNew->size() <<  " sizeHint : " << optionPopupNew->sizeHint();
        //qDebug() << "QCursor (x,y) " << tmp_posX << tmp_posY << "  QMainWindow RECT = " << pos_mainWindow << global.width_mainwindow << global.height_mainwindow << " OptionPopup.w " << this->optionPopupNew->sizeHint().width();
        if(  (tmp_posX - (-pos_mainWindow.x()) + optionPopupNew->sizeHint().width() ) > global.width_mainwindow ){
            //qDebug() << "   가로 넘치는 경우임 ";
            tmp_posX = tmp_posX - optionPopupNew->sizeHint().width();
        }
        if( tmp_posY + optionPopupNew->sizeHint().height() > (-pos_mainWindow.y()) + global.height_mainwindow){
            // 옵션팝업띄워지는 Y좌표 재조정 필요함... 옵션팝업 띄우기 전 외부에서는 height값을 접근하기 어려워서 width값을 이용한 좌표 변경은 외부에서 하고 height값을 이용한 좌표 변경은 내부에서 show 이후에 하는것으로 처리함.
            // height가 MainWindow밖으로 넘쳐버리는 경우
            // 또 다른 문제.. height 를 show 된 이후 sizeHint().height 를 이용했는데 show한 다음 서브메뉴를 펼치고 닫으면 그 이후의 sizeHint는 서브메뉴의 값이 찍혀버림... ㅜㅜ
            // So, 항상 새로 객체를 생성하여 show 전후로 값이 동일하게 하여 이런 고민 안해도 되게 함.
            //qDebug() << "    세로 넘치는 경우임  height가 MainWindow밖으로 넘쳐버리는 경우 ";
            int tmp_over = (tmp_posY + optionPopupNew->sizeHint().height()) - ((-pos_mainWindow.y()) + global.height_mainwindow);
            tmp_posY = tmp_posY - tmp_over;
        }
        optionPopupNew->showMenu(tmp_posX, tmp_posY);




    }else if(p_code=="play"){


        qDebug()<< "AbstractMainContent::slot_clickedHoverItem  :: clicked play :: Nothing.... 미개발 ";

        // 옵션팝업에 데이터를 more과 동일하게 세팅해서 옵션팝업의 바로 듣기와 동일한 함수 호출하게 함
        optionPopupNew->slot_clickedPlay();



    }else if(p_code=="fav"){


        qDebug()<< "AbstractMainContent::slot_clickedHoverItem  :: clicked fav :: Nothing... 미개발 ";
        //qDebug() << "           " << p_jsonData;

        DataPopup *dataPopup = optionPopupNew->getFirstDataPopup();
        requestFavoriteAdd(p_jsonData, dataPopup, optionPopupNew->getAlbumImg());



    }else if(p_code=="addPlayList"){


        qDebug()<< "AbstractMainContent::slot_clickedHoverItem  :: clicked addPlayList :: Nothing... 미개발 ";



    }
}

/**
 * @brief AbstractMainContent::requestFavoriteAdd 하트 설정하는 API 호출
 * @param p_jsonObject
 */
void AbstractMainContent::requestFavoriteAdd(QJsonObject p_jsonObject, DataPopup *p_dataPopup, QString p_albumImg){
    qDebug()<< "AbstractMainContent::requestFavoriteAdd(QJsonObject) 하트 설정하는 API 호출 ";

    if(!global.device.getDeviceIP().isEmpty()){

        //DataPopup *p_dataPopup = this->optionPopupNew->getFirstDataPopup();

        QJsonArray tmp_jsonArrayThumb;
        QJsonObject tmp_dataThumb;
        tmp_dataThumb.insert("url", p_albumImg);
        tmp_jsonArrayThumb.append(tmp_dataThumb);
        QJsonArray tmp_jsonArrayArtist;
        QJsonObject tmp_dataArtist;
        tmp_dataArtist.insert("name", p_dataPopup->getArtist());
        tmp_jsonArrayArtist.append(tmp_dataArtist);
        QJsonArray tmp_jsonArray;
        QJsonObject tmp_data;
        //tmp_data.insert("album", dataPopup->getAlbum());
        //tmp_data.insert("artists", tmp_jsonArrayArtist);
        //tmp_data.insert("client_id", dataPopup->getId());
        //tmp_data.insert("isFavorite", false);
        //tmp_data.insert("isSelected", false);
        tmp_data.insert("mac_address", global.device.getDeviceID());
        //tmp_data.insert("thumbnails", tmp_jsonArrayThumb);
        //tmp_data.insert("title", dataPopup->getTitle());
        tmp_data.insert("type", "music");
        tmp_data.insert("url", p_dataPopup->getData());
        tmp_jsonArray.append(tmp_data);

        QJsonObject tmp_json;
        tmp_json.insert("favorite",true);
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("tracks",tmp_jsonArray);
        qDebug() << "   하트 (앨범) Set API 호출 전 input : " << tmp_json;

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));
        network->request(HTTP_FAVORITE_ADD, QString("%1%2").arg(global.legacy_v3).arg("/track/favorite/add"), tmp_json, true, true);

    }
}

/**
 * @brief AbstractMainContent::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void AbstractMainContent::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    qDebug() << "void AbstractMainContent::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject)";
    qDebug() << "       " << p_jsonObject;


    if(p_id == HTTP_FAVORITE_ADD){


    }

    sender()->deleteLater();
}
/**
 * @brief AbstractMainContent::goToBackPage [SLOT][virtual] 이전 페이지 이동
 */
void AbstractMainContent::goToBackPage(){
    //qDebug() << "AbstractMainContent::goToBackPage()";
    this->indexPageOrdering_curr--;
    QString tmp_pageCode = this->list_pageCodeOrdering->at(this->indexPageOrdering_curr);
    QJsonObject tmp_data = this->list_pageDataOrdering->at(this->indexPageOrdering_curr);

    // 기존페이지로 이동 및 기데이터 set
    this->movePageOnly(tmp_pageCode, tmp_data);

    if(this->indexPageOrdering_curr<=0){
        this->topNaviBar->setEnabledBtnBack(false);
    }
    if((this->list_pageCodeOrdering->count()-1) <= this->indexPageOrdering_curr){
        this->topNaviBar->setEnabledBtnNext(false);
    }else{
        this->topNaviBar->setEnabledBtnNext(true);
    }
}

/**
 * @brief AbstractMainContent::goToNextPage [SLOT][virtual] 다음 페이지 이동
 */
void AbstractMainContent::goToNextPage(){
    //qDebug() << "AbstractMainContent::goToNextPage()";
    this->indexPageOrdering_curr++;
    QString tmp_pageCode = this->list_pageCodeOrdering->at(this->indexPageOrdering_curr);
    QJsonObject tmp_data = this->list_pageDataOrdering->at(this->indexPageOrdering_curr);

    // 기존페이지로 이동 및 기데이터 set
    //this->movePageOnly(tmp_pageCode);
    this->movePageOnly(tmp_pageCode, tmp_data);

    if(this->indexPageOrdering_curr<=0){
        this->topNaviBar->setEnabledBtnBack(false);
    }else{
        this->topNaviBar->setEnabledBtnBack(true);
    }
    if((this->list_pageCodeOrdering->count()-1) <= this->indexPageOrdering_curr){
        this->topNaviBar->setEnabledBtnNext(false);
    }else{
        this->topNaviBar->setEnabledBtnNext(true);
    }
}

/**
 * @brief AbstractMainContent::goToMoveNewOrderPage [SLOT] 페이지 이동 (기존 list_pageCodeOrdering 없는 신규 페이지 이동)
 * @details 내부적으로 movePageOnly함수를 호출한다. 즉 순수 페이지 이동하는 것과 함께 back/next버튼 스타일과 list_pageCodeOrdering 관련 처리가 함께 서술됨
 * @param p_pageCode
 */
void AbstractMainContent::goToMoveNewOrderPage(const QJsonObject &p_data){
    //qDebug() << "AbstractMainContent::goToMoveNewOrderPage(QJsonObject)" << p_data;

    QString tmp_pageCode = p_data[KEY_PAGE_CODE].toString();
    QString tmp_contentStep = "";
    if( p_data.contains(KEY_CONTENT_STEP)){
        tmp_contentStep = p_data[KEY_CONTENT_STEP].toString();
    }

    /*
    // 1) 페이지 중복 검사
    bool flagSavePage = true;
    if(this->list_pageCodeOrdering->count()>0){
        QString tmp_pageCodeBefore = this->list_pageCodeOrdering->last();
        QString tmp_contentStepBefore = "";
        if( tmp_pageCode == tmp_pageCodeBefore ){
            QJsonObject tmp_dataBefore = this->list_pageDataOrdering->last();
            if(tmp_dataBefore.contains(KEY_CONTENT_STEP)){
                tmp_contentStepBefore = tmp_dataBefore[KEY_CONTENT_STEP].toString();
            }
            if(tmp_contentStep == tmp_contentStepBefore){
                // no save
                flagSavePage = false;
            }
        }
        qDebug() <<  tmp_pageCode << "vs" << tmp_pageCodeBefore;
        qDebug() <<  tmp_contentStep << "vs" << tmp_contentStepBefore;
    }

    qDebug() << "  ★ flagSavePage : " << flagSavePage;

    if(flagSavePage==true)*/{
        // 2) 페이지 기억
        this->goToMoveNewOrderPage_subStep(p_data);
        // 3) 기존페이지가 아닌 새페이지 이동 및 데이터 init
        this->movePageOnly(tmp_pageCode, p_data);
    }
}
/**
 * @brief AbstractMainContent::goToMoveNewOrderPage_subStep
 * @param p_data
 */
void AbstractMainContent::goToMoveNewOrderPage_subStep(const QJsonObject &p_data){
    //qDebug() << "★ AbstractMainContent::goToMoveNewOrderPage_subStep(QJsonObject) ★";

    QString tmp_pageCode = p_data[KEY_PAGE_CODE].toString();
    QString tmp_contentStep = "";
    if( p_data.contains(KEY_CONTENT_STEP)){
        tmp_contentStep = p_data[KEY_CONTENT_STEP].toString();
    }
    //qDebug() << "     pageCode : " << tmp_pageCode;
    //qDebug() << "     contentStep : " << p_data[KEY_CONTENT_STEP].toString();


    /*
    // 1) 페이지 중복 검사
    bool flagSavePage = true;
    if(this->list_pageCodeOrdering->count()>0){
        QString tmp_pageCodeBefore = this->list_pageCodeOrdering->last();
        QString tmp_contentStepBefore = "";
        if( tmp_pageCode == tmp_pageCodeBefore ){
            QJsonObject tmp_dataBefore = this->list_pageDataOrdering->last();
            if(tmp_dataBefore.contains(KEY_CONTENT_STEP)){
                tmp_contentStepBefore = tmp_dataBefore[KEY_CONTENT_STEP].toString();
            }
            if(tmp_contentStep == tmp_contentStepBefore){
                // no save
                flagSavePage = false;
            }
        }
        qDebug() << "[Sub] " <<  tmp_pageCode << "vs" << tmp_pageCodeBefore;
        qDebug() << "[Sub] " <<  tmp_contentStep << "vs" << tmp_contentStepBefore;
    }
    qDebug() << "  ★ [Sub] flagSavePage : " << flagSavePage;

    if(flagSavePage==true)*/{

        // back하여 중간에서 새로 시작되는 경우 처리
        if(this->indexPageOrdering_curr < (this->list_pageCodeOrdering->count()-1)){
            // 현재 index로부터 이후에 담겨있는 orderPage 삭제
            for(int i=(this->list_pageCodeOrdering->count()-1); i>this->indexPageOrdering_curr; i--){
                this->list_pageCodeOrdering->removeAt(i);
                this->list_pageDataOrdering->removeAt(i);
            }
        }

        bool flagAddMemoryPage = true;
        // 예외상황 : playfullscreen 은 중복으로 Page 선택해도 중복 저장 안되게 처리한다.
        if(tmp_pageCode=="playfullscreen"){
            if(this->list_pageCodeOrdering->last()==tmp_pageCode){
                // playfullscreen 중복
                flagAddMemoryPage = false;
            }
        }

        if(flagAddMemoryPage==true){
            this->list_pageCodeOrdering->append(tmp_pageCode);
            this->list_pageDataOrdering->append(p_data);
            this->indexPageOrdering_curr++;
            if(this->list_pageCodeOrdering->count()>1){
                this->topNaviBar->setEnabledBtnBack(true);
            }
            this->topNaviBar->setEnabledBtnNext(false);
        }
    }

    /*
    qDebug() << "      curr Index : " << this->indexPageOrdering_curr;
    for(int i=0; i<this->list_pageCodeOrdering->count(); i++){
        qDebug() << "           [" << i << "] " <<  this->list_pageCodeOrdering->at(i);
    }
    */
}

/**
 * @brief AbstractMainContent::movePageOnly [virtual] 어떤 StackWidget을 보일지에 세팅하고 상단 메뉴바 데이터를 재세팅한다.
 * @details virtual 에서는 공통적으로 이동되는 페이지만을 서술한다.
 *          override 에서는 각자 사용하는 페이지를 이동하는 내용을 서술한다.
 * @param p_pageCode
 * @param p_data
 */
void AbstractMainContent::movePageOnly(QString p_pageCode, const QJsonObject &p_data){
    //qDebug() << "void AbstractMainContent::movePageOnly(QString p_pageCode, const QJsonObject &p_data)";

    this->clearRoseTubeBG();

    if(p_pageCode==PAGECODE_ETC_PLAYFULLSCREEN){

        this->topMenuBar->setSelectedSubMenu(p_pageCode);
        this->topMenuBar->setDataTopMenuBar(QJsonObject(), new QJsonArray());
        this->topMenuBar->setVisible(false);

        // 이 화면은 외부에서 세팅하는 구조가 아니라 ..
        // 화면이 그려질때.. SectionBottom에게 물어봐서 현재 재생정보를 세팅하는 구조로 변경함..
        //this->playFullScreenRelation->setData(p_data);
        linker->signal_getCurrPlayData();
        stackedWidget_content->setCurrentWidget(this->playFullScreenRelation);
    }
}

/**
 * @brief AbstractMainContent::slot_changeRoseTubeBG 로즈튜브배경을 세팅한다.
 * @param p_pixmap
 */
void AbstractMainContent::slot_changeRoseTubeBG(const QPixmap &p_pixmap){

    this->flagSetRoseTubeBG = true;
    this->pixmap_roseTubeBG = new QPixmap(p_pixmap);
    // 배경 세팅
    this->widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG { background-color:transparent; }");
    //this->widget_roseTubeBG->setFixedSize(p_pixmap.width(), p_pixmap.height()); // 가로 Fix 하면 안된다.. 반응형 때문
    //this->widget_roseTubeBG->setFixedHeight(p_pixmap.height());
    this->lb_roseTubeBG->setPixmap(p_pixmap);
    //this->lb_roseTubeBG->resize(p_pixmap.width(), p_pixmap.height());


    // resize Event에 있는 함수와 동일한 로직
    /*
    QPixmap tmp_pixmap = this->pixmap_roseTubeBG->scaledToWidth(this->widget_roseTubeBG->size().width());
    this->lb_roseTubeBG->setPixmap(tmp_pixmap);
    //this->widget_roseTubeBG->setFixedHeight(tmp_pixmap.height());
    this->widget_roseTubeBG->setFixedHeight(200);
    this->lb_roseTubeBG->setFixedWidth(this->widget_roseTubeBG->size().width());

    */

    QPixmap tmp_pixmap = this->pixmap_roseTubeBG->scaled(this->widget_roseTubeBG->size().width(), 200, Qt::KeepAspectRatioByExpanding);
    this->lb_roseTubeBG->setPixmap(tmp_pixmap);
    //this->lb_roseTubeBG->setGeometry(0,)
    //this->widget_roseTubeBG->setFixedHeight(tmp_pixmap.height());
    this->widget_roseTubeBG->setFixedHeight(200);
    //this->lb_roseTubeBG->setFixedWidth(this->widget_roseTubeBG->size().width());
    this->lb_roseTubeBG->resize(this->widget_roseTubeBG->size());


    this->lb_roseTubeBG->show();
}

/**
 * @brief AbstractMainContent::clearRoseTubeBG 로즈튜브배경 클리어 (원상복구)
 */
void AbstractMainContent::clearRoseTubeBG(){

    this->flagSetRoseTubeBG = false;
    // 배경 세팅
    widget_roseTubeBG->setStyleSheet("#widget_roseTubeBG { background-color:#171717; }");
    widget_roseTubeBG->setFixedHeight(topNaviBar->height());
    this->lb_roseTubeBG->setPixmap(QPixmap());
    //this->lb_roseTubeBG->resize(topNaviBar->size());

    this->lb_roseTubeBG->hide();
}
