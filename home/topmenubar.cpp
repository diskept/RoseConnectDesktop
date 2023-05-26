#include "common/gscommon.h"
#include "topmenubar.h"
#include "topmenusubitem.h"
#include "common/global.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QPushButton>

const int TOP_NAVIBAR_H = 70;


const QString SORT_STEP_MUSIC = "music";//cheon211008
const QString SORT_STEP_BUGS = "bugs";//cheon211008
const QString SORT_STEP_TIDAL = "tidal";//cheon211008
const QString SORT_STEP_QOBUZ = "qobuz";//cheon211008
const QString SORT_STEP_RoseTube = "rosetube";//cheon211008

const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";//cheon211008
//const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01
const QString tmp_btnStyleHover = "background-color:#CCCCCC;color:#FFFFFF;";//cheon211115-01
const QString tmp_btnStyleHover_selected = "background-color:#B18658;color:#FFFFFF;";//cheon211115-01

/**
 * @brief TopMenuBar::TopMenuBar 생성자
 * @param p_flagShowMainTitle 메인메뉴명 타이틀을 보이게할지 말지 여부 설정
 * @param parent
 */
/*TopMenuBar::TopMenuBar(ShowMenuStyle p_showMenuStyle, QWidget *parent) : QWidget(parent)
{
    linker = Linker::getInstance();//cheon211008
    this->showMenuStyle = p_showMenuStyle;
    if(!global.searchMenuFlag){//cheon211008
        this->setFixedHeight(TOP_NAVIBAR_H);//cheon211008
    }else{
        this->setFixedHeight(TOP_NAVIBAR_H*2);//cheon211008
    }

    this->lb_mainName = new QLabel();
    this->lb_mainName->setContentsMargins(33,0,49,0);
    this->lb_mainName->setStyleSheet("color:white;font-size:26px;font:bold;");

    this->hlayout_menuSub = new QHBoxLayout();
    this->hlayout_menuSub->setContentsMargins(0,0,0,0);
    //this->hlayout_menuSub->setSpacing(12);
    this->hlayout_menuSub->setSpacing(25);
    this->hlayout_menu = new QHBoxLayout();
    this->hlayout_menu->setContentsMargins(0,0,0,0);
    this->hlayout_menu->setSpacing(0);
    this->hlayout_menu->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->hlayout_menu->addWidget(this->lb_mainName);
    this->hlayout_menu->addLayout(this->hlayout_menuSub);



    widget_topMenuBar = new QWidget();
    widget_topMenuBar->setObjectName("topMenuBar");
    //widget_topMenuBar->setFixedHeight(70);//cheon211008
    widget_topMenuBar->setLayout(this->hlayout_menu);

    widget_topMenuBar->setStyleSheet("background-color:#171717;");

    line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    line_bottom_1->setStyleSheet("background-color:#FFFFFF;");
    line_bottom_2 = new QWidget();
    line_bottom_2->setFixedHeight(1);
    line_bottom_2->setStyleSheet("background-color:#282828;");


//cheon211008--start

    // 상단 필터
    QPushButton *btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
    btn_filter->setFixedSize(50,70);
    btn_filter->setCursor(Qt::PointingHandCursor);

    QLabel *lb_title = new QLabel(tr("Filter"));
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;font:bold;");

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(0,0,0,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);

    QWidget *widget_title = new QWidget();
    widget_title->setObjectName("widget_title");
    widget_title->setContentsMargins(0,0,0,0);
    widget_title->setLayout(hl_lb);
    widget_title->setStyleSheet("#widget_title { background-color:transparent; } QLabel { background-color:transparent; }");

    this->hl_filter = new QHBoxLayout();
    this->hl_filter->setContentsMargins(0,0,90,0);
    this->hl_filter->setSpacing(0);
    this->hl_filter->setAlignment(Qt::AlignRight);
    this->hl_filter->addWidget(btn_filter);
    this->hl_filter->addWidget(widget_title);


    widget_btnFilter = new QWidget();//cheon211008
    widget_btnFilter->setObjectName("widget_btnFilter");
    widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121; }");
    widget_btnFilter->setFixedHeight(70);

    widget_btnFilter->setLayout(hl_filter);

    QWidget *widget_line = new QWidget();
    widget_line->setFixedHeight(1);
    widget_line->setStyleSheet("background-color:#171717;");
    widget_line->setContentsMargins(0,0,0,0);

//cheon211008--end


    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);

    vl_total->addWidget(widget_topMenuBar);//cheon211008
    if(global.searchMenuFlag){//cheon211008
        vl_total->addWidget(widget_btnFilter);//cheon211008
    }


    vl_total->addWidget(line_bottom_1);
    vl_total->addWidget(line_bottom_2);

    this->setLayout(vl_total);

    //if(this->flagShowMainTitle==false){
    if(this->showMenuStyle!=ShowMenuStyle::TopSubMenuBar){
        // 앨범상세화면에서 사용하는 서브메뉴 Tab
        this->lb_mainName->setVisible(false);
        this->hlayout_menuSub->setContentsMargins(27,0,0,0);
        this->hlayout_menuSub->setSpacing(30);
        if(this->showMenuStyle==ShowMenuStyle::AlbumDetail){
            this->hlayout_menu->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            line_bottom_1->setFixedHeight(0);
            line_bottom_2->setStyleSheet("background-color:#373737;");
        }
        else if(this->showMenuStyle==ShowMenuStyle::ListSubMenuBar){
            this->hlayout_menu->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            line_bottom_1->setFixedHeight(0);
            line_bottom_2->setFixedHeight(0);
        }
        widget_topMenuBar->setStyleSheet("background-color:transparent;");
        this->setFixedHeight(50);
        if(this->showMenuStyle==ShowMenuStyle::FriendManager){
            this->hlayout_menuSub->setContentsMargins(65,10,0,0);
            this->hlayout_menuSub->setSpacing(5);
            line_bottom_2->setFixedHeight(5);
            line_bottom_2->setStyleSheet("background-color:#333333;");
            this->setFixedHeight(60);
        }
    }
}*/

TopMenuBar::TopMenuBar(ShowMenuStyle p_showMenuStyle, QWidget *parent) : QWidget(parent)
{
    linker = Linker::getInstance();//cheon211008

    this->showMenuStyle = p_showMenuStyle;

    if(!global.searchMenuFlag){//cheon211008
        this->setFixedHeight(TOP_NAVIBAR_H);//cheon211008
    }
    else{
        this->setFixedHeight(TOP_NAVIBAR_H*2);//cheon211008
    }

    this->lb_mainName = new QLabel();
    this->lb_mainName->setContentsMargins(33,0,49,0);
    this->lb_mainName->setStyleSheet("color:white;font-size:26px;font:bold;");



    this->hlayout_menuSub = new QHBoxLayout();
    this->hlayout_menuSub->setContentsMargins(0,0,0,0);
    //this->hlayout_menuSub->setSpacing(12);
    this->hlayout_menuSub->setSpacing(25);

    this->hlayout_menu = new QHBoxLayout();
    this->hlayout_menu->setContentsMargins(0,0,0,0);
    this->hlayout_menu->setSpacing(0);

    this->hlayout_menu->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->hlayout_menu->addWidget(this->lb_mainName);
    this->hlayout_menu->addLayout(this->hlayout_menuSub);



    widget_topMenuBar = new QWidget();
    widget_topMenuBar->setObjectName("topMenuBar");
    //widget_topMenuBar->setFixedHeight(70);//cheon211008
    widget_topMenuBar->setLayout(this->hlayout_menu);

    widget_topMenuBar->setStyleSheet("background-color:#171717;");
    line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    //line_bottom_1->setStyleSheet("background-color:#FFFFFF;");

    line_bottom_2 = new QWidget();
    line_bottom_2->setFixedHeight(1);
    //line_bottom_2->setStyleSheet("background-color:#282828;");


//cheon211008--start

    // 상단 필터
    QPushButton *btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
    btn_filter->setFixedSize(50,70);
    btn_filter->setCursor(Qt::PointingHandCursor);

    QLabel *lb_title = new QLabel(tr("Filter"));
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;font:bold;");

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(0,0,0,0);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);

    QWidget *widget_title = new QWidget();
    widget_title->setObjectName("widget_title");
    widget_title->setContentsMargins(0,0,0,0);
    widget_title->setLayout(hl_lb);
    widget_title->setStyleSheet("#widget_title { background-color:transparent; } QLabel { background-color:transparent; }");

    this->hl_filter = new QHBoxLayout();
    this->hl_filter->setContentsMargins(0,0,90,0);
    this->hl_filter->setSpacing(0);
    this->hl_filter->setAlignment(Qt::AlignRight);
    this->hl_filter->addWidget(btn_filter);
    this->hl_filter->addWidget(widget_title);


    widget_btnFilter = new QWidget();//cheon211008
    widget_btnFilter->setObjectName("widget_btnFilter");
    widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#212121; }");
    widget_btnFilter->setFixedHeight(70);

    widget_btnFilter->setLayout(hl_filter);

    QWidget *widget_line = new QWidget();
    widget_line->setFixedHeight(1);
    widget_line->setStyleSheet("background-color:#171717;");
    widget_line->setContentsMargins(0,0,0,0);

//cheon211008--end


    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(line_bottom_1);//cheon211018
    vl_total->addWidget(line_bottom_2);//cheon211018
    vl_total->addWidget(widget_topMenuBar);//cheon211008
    //if(global.searchMenuFlag){//cheon211008
        //vl_total->addWidget(widget_btnFilter);//cheon211008
    //}

    this->setLayout(vl_total);

    //if(this->flagShowMainTitle==false){
    if(this->showMenuStyle!=ShowMenuStyle::TopSubMenuBar){
        // 앨범상세화면에서 사용하는 서브메뉴 Tab
        this->lb_mainName->setVisible(false);
        this->hlayout_menuSub->setContentsMargins(27,0,0,0);
        this->hlayout_menuSub->setSpacing(30);

        if(this->showMenuStyle==ShowMenuStyle::AlbumDetail){
            this->hlayout_menu->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
            line_bottom_1->setStyleSheet("background-color: transparent;");
            line_bottom_1->hide();
            line_bottom_2->setStyleSheet("background-color: transparent;");
            line_bottom_2->hide();
            //line_bottom_1->setFixedHeight(0);
            //line_bottom_2->setStyleSheet("background-color:#373737;");
        }
        else if(this->showMenuStyle==ShowMenuStyle::ListSubMenuBar){
            this->hlayout_menu->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            line_bottom_1->setStyleSheet("background-color: transparent;");
            line_bottom_1->hide();
            line_bottom_2->setStyleSheet("background-color: transparent;");
            line_bottom_2->hide();
            //line_bottom_1->setFixedHeight(0);
            //line_bottom_2->setFixedHeight(0);
        }

        widget_topMenuBar->setStyleSheet("background-color: transparent;");
        this->setFixedHeight(50);

        if(this->showMenuStyle==ShowMenuStyle::FriendManager){
            this->hlayout_menuSub->setContentsMargins(65,10,0,0);
            this->hlayout_menuSub->setSpacing(5);
            line_bottom_1->setStyleSheet("background-color: transparent;");
            line_bottom_1->hide();
            line_bottom_2->setStyleSheet("background-color: transparent;");
            line_bottom_2->hide();
            //line_bottom_2->setFixedHeight(5);
            //line_bottom_2->setStyleSheet("background-color:#333333;");
            this->setFixedHeight(60);
        }
    }
}

/**
 * @brief TopMenuBar::setData
 * @param p_jsonObject_main
 * @param p_jsonArray_sub
 */
void TopMenuBar::setDataTopMenuBar(QJsonObject p_jsonObject_main,  QJsonArray *p_jsonArray_sub){//c230323

    print_debug();
    emit linker->signal_clickListBtn();//c220726
    emit linker->signal_text_clear();//c220726

    // 메인메뉴
    if(p_jsonObject_main.isEmpty()==false){
        if(p_jsonObject_main.contains(KEY_MAIN_CODE)){
            this->mainMenuCode = p_jsonObject_main[KEY_MAIN_CODE].toInt();  // 옵션팝업에서 사용함..
        }
        if(p_jsonObject_main.contains("name")){
            this->title_org = p_jsonObject_main["name"].toString();
            this->lb_mainName->setText(this->title_org);

            this->lb_mainName->setVisible(true);
        }
    }
    else{
        this->lb_mainName->setVisible(false);
    }

    // 서브메뉴
    this->list_subMenuItem.clear();
    this->hlayout_menu->removeWidget(this->lb_mainName);
    this->hlayout_menu->removeItem(this->hlayout_menuSub);
    //this->hlayout_menu->removeItem(this->hlayout_etc);
    GSCommon::clearLayout(this->hlayout_menuSub);
    this->hlayout_menu->addWidget(this->lb_mainName);
    this->hlayout_menu->addLayout(this->hlayout_menuSub);
    //this->hlayout_menu->addLayout(this->hlayout_etc, 5);

    TopMenuSubItem::UIMode tmp_uiMode = TopMenuSubItem::UIMode::TopMenu;
    //if(this->flagShowMainTitle==false){
    if(this->showMenuStyle==ShowMenuStyle::AlbumDetail){
        tmp_uiMode = TopMenuSubItem::UIMode::AlbumDetail;
    }
    else if(this->showMenuStyle==ShowMenuStyle::FriendManager){
        tmp_uiMode = TopMenuSubItem::UIMode::FriendManager;
    }
    else if(this->showMenuStyle==ShowMenuStyle::ListSubMenuBar){
        tmp_uiMode = TopMenuSubItem::UIMode::ListSubMenuBar;
    }
    else if(this->showMenuStyle==ShowMenuStyle::FriendSubmenubar){//cheon211018
        tmp_uiMode = TopMenuSubItem::UIMode::FriendSubmenubar;
    }
    menu_width_sum = 0;//
    //for(int i=(p_jsonArray_sub.count()-1); i>=0; i--){
    for(int i = 0; i < p_jsonArray_sub->count(); i++){//
        QString tmp_code = p_jsonArray_sub->at(i).toObject()["code"].toString();
        qDebug() << "tmp_code: " << tmp_code;
        TopMenuSubItem *subMenu = new TopMenuSubItem(p_jsonArray_sub->at(i).toObject(), tmp_uiMode, this);
        if(subMenu->getSubMenuCode()==this->curr_subMenuCode){
            subMenu->setSelectedStyle(true);
        }

        //subMenu->setToolTip(subMenu->getSubMenuName());//c230321
        //subMenu->setToolTipDuration(2000);//c230321
        //subMenu->setStyleSheet("background-color: transparent;color:white;font-size:26px;font:bold;");//c230321

        this->list_subMenuItem.append(subMenu);
        this->hlayout_menuSub->addWidget(subMenu);
        menu_width_sum += subMenu->width();
        connect(subMenu, SIGNAL(clickedBtn(QString)), this, SLOT(clickedSubMenu(QString)));

    }

    int menu_width = menu_width_sum +(p_jsonArray_sub->count()*25) + 100;//
    //qDebug() << "menu_width=" << menu_width;
    for(int i=0; i<this->list_subMenuItem.count(); i++){
        int width = 130;
        if(global.LmtCnt < menu_width+30){
            //print_debug();
                if(global.lang == 0){
                    if(this->list_subMenuItem[i]->getSubMenuName().size() < 8){
                        width = (this->list_subMenuItem[i]->getSubMenuName().size()+3)*15;
                    }else{
                        //if(global.LmtCnt-menu_width > 0){

                        //}
                        width = 10*15;
                    }

                }else{
                    if(this->list_subMenuItem[i]->getSubMenuName().size() < 5){
                        width = (this->list_subMenuItem[i]->getSubMenuName().size()+3)*20;
                    }else{
                        width = 7*20;
                    }
                }

        }else{
            //print_debug();
            if(global.lang == 0){
                width = (this->list_subMenuItem[i]->getSubMenuName().size()+6)*15;
            }else{
                width = (this->list_subMenuItem[i]->getSubMenuName().size()+6)*20;
            }
        }
        if(width < 100) width = 100;
        //qDebug() << "this->list_subMenuItem[i]->getSubMenuName()=" << this->list_subMenuItem[i]->getSubMenuName();
        QString str_subMenuName = GSCommon::getTextCutFromLabelWidth(this->list_subMenuItem[i]->getSubMenuName(), width, this->lb_mainName->font());
        this->list_subMenuItem[i]->setSubMenuText(str_subMenuName);

    }
    print_debug();
    qDebug() << "this->lb_mainName->width()=" << this->lb_mainName->width();
    qDebug() << "sum = " << menu_width_sum +(p_jsonArray_sub->count()*25) + 100;



}
/**
 * @brief TopMenuBar::setSelectedSubMenu 해당 메뉴를 선택된 스타일로 세팅하고 나머지는 스타일 해제
 * @param p_subMenuCode
 */
void TopMenuBar::setSelectedSubMenu(QString p_subMenuCode){

    this->curr_subMenuCode = p_subMenuCode;
    for(int i = 0; i < this->list_subMenuItem.count(); i++){
        if(this->list_subMenuItem[i]->getSubMenuCode()==this->curr_subMenuCode){
            this->list_subMenuItem[i]->setSelectedStyle(true);
        }else{
            this->list_subMenuItem[i]->setSelectedStyle(false);
        }
    }
}

/**
 * @brief TopMenuBar::slot_changedMenuNameText [SLOT] 오직 메뉴명 타이틀 텍스트 수정한다.
 * @param p_menuName
 */
void TopMenuBar::slot_changedMenuNameText(const QString &p_menuName){

    this->title_org = p_menuName;
    if(this->showMenuStyle==ShowMenuStyle::TopSubMenuBar){
        this->mainMenuCode = 0;
        QString tmp_name = GSCommon::getTextCutFromLabelWidth(this->title_org, (this->width()-100), this->lb_mainName->font());
        this->lb_mainName->setText(tmp_name);
        this->lb_mainName->setVisible(true);
    }
}


/**
 * @brief TopMenuBar::slot_changedSubMenuNameText [SLOT] 오직 현재 보고있는 서브메뉴명 텍스트 수정한다.
 * @param p_subMenuName
 */
void TopMenuBar::slot_changedSubMenuNameText(const QString &p_subMenuName){


    for(int i=0; i<this->list_subMenuItem.count(); i++){
        if(this->list_subMenuItem[i]->getSubMenuCode()==this->curr_subMenuCode){
            this->list_subMenuItem[i]->setSubMenuText(p_subMenuName);
        }
    }
}

/**
 * @brief MainWindow::clickedSubMenu [SLOT] 상단 서브 메뉴 클릭
 * @param p_menuCode
 */
void TopMenuBar::clickedSubMenu(QString p_subMenuCode){
    if(this->curr_subMenuCode == p_subMenuCode){

        // 현재 선택된 동일한 서브 메뉴를 클릭한 경우
        emit signal_clickedSameSubMenu(p_subMenuCode);

    }else{

        this->setSelectedSubMenuNoSignal(p_subMenuCode);

        // 현재 선택된 메뉴가 다른 경우에만 메뉴 변경되었다고 시그널 알림
        QJsonObject tmp_data;
        tmp_data[KEY_PAGE_CODE] = this->curr_subMenuCode;
        emit changedSubMenu(tmp_data);
    }
}


void TopMenuBar::setSelectedSubMenuNoSignal(QString p_subMenuCode){
    this->setSelectedSubMenu(p_subMenuCode);
}

/**
 * @brief TopMenuBar::setBGTransparent : 배경 투명화
 * @param p_flagTransparent
 */
void TopMenuBar::setBGTransparent(bool p_flagTransparent){

    if(p_flagTransparent){
        widget_topMenuBar->setStyleSheet("background-color: transparent;");
        line_bottom_1->setStyleSheet("background-color: transparent;");
        line_bottom_2->setStyleSheet("background-color: transparent;");
    }
    else{
        widget_topMenuBar->setStyleSheet("background-color: #171717;");
        line_bottom_1->setStyleSheet("background-color: #000000;");
        line_bottom_2->setStyleSheet("background-color: #333333;");
    }
}

/**
 * @brief TopMenuBar::resizeEvent [override] 리사이즈 이벤트
 * @param event
 */
void TopMenuBar::resizeEvent(QResizeEvent *event){//c230409

    Q_UNUSED(event);
    if(this->showMenuStyle==ShowMenuStyle::TopSubMenuBar){
        QString tmp_name = GSCommon::getTextCutFromLabelWidth(this->title_org, (this->width()-100), this->lb_mainName->font());
        this->lb_mainName->setText(tmp_name);
    }
    print_debug();//widget_topMenuBar

    //qDebug() << "global.LmtCnt=" << global.LmtCnt;
    //qDebug() << "widget_topMenuBar->maximumWidth()=" << widget_topMenuBar->maximumWidth();

    if(menu_width_sum>200){//
        int menu_width = menu_width_sum +(this->list_subMenuItem.count()*25) + 100;
        for(int i=0; i<this->list_subMenuItem.count(); i++){
            int width = 130;
            if(global.LmtCnt < menu_width+30){
                //print_debug();
                if(global.lang == 0){
                    if(this->list_subMenuItem[i]->getSubMenuName().size() < 8){
                        width = (this->list_subMenuItem[i]->getSubMenuName().size()+3)*15;
                    }else{
                        //if(global.LmtCnt-menu_width > 0){

                        //}
                        width = 10*15;
                    }

                }else{
                    if(this->list_subMenuItem[i]->getSubMenuName().size() < 5){
                        width = (this->list_subMenuItem[i]->getSubMenuName().size()+3)*20;
                    }else{
                        width = 7*20;
                    }
                }

            }else{
                //print_debug();
                if(global.lang == 0){
                    width = (this->list_subMenuItem[i]->getSubMenuName().size()+6)*15;
                }else{
                    width = (this->list_subMenuItem[i]->getSubMenuName().size()+6)*20;
                }
            }
            if(width < 100) width = 100;
            //qDebug() << "this->list_subMenuItem[i]->getSubMenuName()=" << this->list_subMenuItem[i]->getSubMenuName();
            QString str_subMenuName = GSCommon::getTextCutFromLabelWidth(this->list_subMenuItem[i]->getSubMenuName(), width, this->lb_mainName->font());
            this->list_subMenuItem[i]->setSubMenuText(str_subMenuName);

        }

        global.menu_width = menu_width+100;
    }

    //qDebug() << "global.menu_width=" << global.menu_width;
    //qDebug() << "menu_width_sum=" << menu_width_sum;

    //QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}
