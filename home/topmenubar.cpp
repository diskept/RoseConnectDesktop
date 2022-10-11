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
    //if(global.searchMenuFlag){//cheon211008
        //vl_total->addWidget(widget_btnFilter);//cheon211008
    //}


    //vl_total->addWidget(line_bottom_1);//cheon211018
    //vl_total->addWidget(line_bottom_2);//cheon211018

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
}

/**
 * @brief TopMenuBar::setData
 * @param p_jsonObject_main
 * @param p_jsonArray_sub
 */
void TopMenuBar::setDataTopMenuBar(QJsonObject p_jsonObject_main,  QJsonArray *p_jsonArray_sub){

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
    }else if(this->showMenuStyle==ShowMenuStyle::FriendSubmenubar){//cheon211018
        tmp_uiMode = TopMenuSubItem::UIMode::FriendSubmenubar;
    }

    //for(int i=(p_jsonArray_sub.count()-1); i>=0; i--){
    for(int i = 0; i < p_jsonArray_sub->count(); i++){
        QString tmp_code = p_jsonArray_sub->at(i).toObject()["code"].toString();
        qDebug() << "tmp_code: " << tmp_code;
        TopMenuSubItem *subMenu = new TopMenuSubItem(p_jsonArray_sub->at(i).toObject(), tmp_uiMode, this);
        if(subMenu->getSubMenuCode()==this->curr_subMenuCode){
            subMenu->setSelectedStyle(true);
        }

        this->list_subMenuItem.append(subMenu);
        this->hlayout_menuSub->addWidget(subMenu);
        connect(subMenu, SIGNAL(clickedBtn(QString)), this, SLOT(clickedSubMenu(QString)));
    }
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



/*void TopMenuBar::slot_changedMenuNameText_search(const QString &p_menuName, const QString &menu){//cheon211008
print_debug();
    this->title_org = p_menuName;
    if(this->showMenuStyle==ShowMenuStyle::TopSubMenuBar){
        this->mainMenuCode = 0;
        QString tmp_name = GSCommon::getTextCutFromLabelWidth(this->title_org, (this->width()-100), this->lb_mainName->font());
        //start 상단 step 버튼//cheon211008-search-----------------------------
        global.btn_Music= new QPushButton(tr("Music"));
        global.btn_Music->setObjectName("btn_Music");
        global.btn_Music->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_MUSIC);

        global.btn_Bugs= new QPushButton(tr("Bugs"));
        global.btn_Bugs->setObjectName("btn_Bugs");
        global.btn_Bugs->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_BUGS);

        global.btn_Tidal = new QPushButton(tr("Tidal"));
        global.btn_Tidal->setObjectName("btn_Tidal");
        global.btn_Tidal->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_TIDAL);

        global.btn_Qobuz = new QPushButton(tr("Qobuz"));
        global.btn_Qobuz->setObjectName("btn_Qobuz");
        global.btn_Qobuz->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_QOBUZ);

        global.btn_RoseTube = new QPushButton(tr("RoseTube"));
        global.btn_RoseTube->setObjectName("btn_RoseTube");
        global.btn_RoseTube->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_RoseTube);

        global.btn_Music->setFixedSize(125,40);
        global.btn_Bugs->setFixedSize(125,40);
        global.btn_Tidal->setFixedSize(125,40);
        global.btn_Qobuz->setFixedSize(125,40);
        global.btn_RoseTube->setFixedSize(125,40);
        global.btn_Music->setCursor(Qt::PointingHandCursor);
        global.btn_Bugs->setCursor(Qt::PointingHandCursor);
        global.btn_Tidal->setCursor(Qt::PointingHandCursor);
        global.btn_Qobuz->setCursor(Qt::PointingHandCursor);
        global.btn_RoseTube->setCursor(Qt::PointingHandCursor);

        global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
        global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Bugs->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Bugs").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
        global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle).arg(tmp_btnStyleHover));

        global.btn_Qobuz->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Qobuz").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

print_debug();
qDebug() << "menu=" << menu;
        if(menu == "Music"){

            global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover_selected));//cheon211115-01

            //global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));//cheon211115-01
        }else if(menu == "RoseTube"){

            global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle + tmp_btnStyleHover_selected).arg(tmp_btnStyleHover_selected));//cheon211115-01
            //global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle + tmp_btnStyleHover).arg(tmp_btnStyleHover));//cheon211115-01
        }

        //if(menu == "Music"){
        //
        //    global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle).arg(tmp_btnStyleHover_selected).arg(tmp_btnStyleHover_selected));//cheon211208
        //    //global.btn_Music->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));//cheon211115-01
        //}else if(menu == "RoseTube"){
        //
        //    global.btn_RoseTube->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_RoseTube").arg(tmp_btnStyle).arg(tmp_btnStyleHover_selected).arg(tmp_btnStyleHover_selected));//cheon211208         //global.btn_RoseTube->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_RoseTube").arg(tmp_btnStyle + tmp_btnStyleHover).arg(tmp_btnStyleHover));//cheon211115-01
        //}

        //end  상단 step 버튼 //--------------------------------------------------------------
        this->lb_mainName = this->get_addUIControl_mainTitle(tmp_name, global.btn_Music, global.btn_RoseTube, global.btn_Tidal, global.btn_Bugs, global.btn_Qobuz);

        //this->lb_mainName->setText(tmp_name);
        this->lb_mainName->setVisible(true);
    }
}

QLabel* TopMenuBar::get_addUIControl_mainTitle(const QString mainTitle, QPushButton *a, QPushButton *b, QPushButton *c, QPushButton *d, QPushButton *e){//cheon211008-serach
print_debug();

// 버튼 스타일 초기화

    QLabel *label_mainTitle = new QLabel();
    label_mainTitle->setText(mainTitle);
    label_mainTitle->setStyleSheet("font-size:26px; color:#FFFFFF; background-color:#171717;");
    label_mainTitle->setContentsMargins(33,20,33,19);

//mainTitle_submenu->addWidget(a,1, Qt::AlignVCenter | Qt::AlignRight);
    QHBoxLayout *mainTitle_submenu = new QHBoxLayout();
    mainTitle_submenu->setSpacing(0);
    mainTitle_submenu->setContentsMargins(0,0,55,0);
    //mainTitle_submenu->setAlignment(Qt::AlignRight);
    mainTitle_submenu->addWidget(a);
    mainTitle_submenu->addWidget(b);
    mainTitle_submenu->addWidget(c);
    mainTitle_submenu->addWidget(d);
    mainTitle_submenu->addWidget(e);

    QWidget *ttt = new QWidget();
    ttt->setContentsMargins(0,0,0,0);
    //ttt->setObjectName("widget_myInfoEdit");
    //ttt->setStyleSheet("#widget_myInfoEdit { background:qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1, stop:0 #222222, stop:1 #464646); }");
    ttt->setLayout(mainTitle_submenu);

    QHBoxLayout *box_mainTitle_submenu = new QHBoxLayout();
    box_mainTitle_submenu->setSpacing(0);
    box_mainTitle_submenu->setContentsMargins(0,0,0,0);
    //box_mainTitle_submenu->setAlignment(Qt::AlignTop);
    box_mainTitle_submenu->addWidget(label_mainTitle);
    box_mainTitle_submenu->addWidget(ttt,1, Qt::AlignVCenter | Qt::AlignRight);


    hlayout_menuSub->addLayout(box_mainTitle_submenu);

    //---------------------------------------------------
    //a->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
    //b->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
    //c->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
    //d->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
    //e->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
    //a->setCursor(Qt::PointingHandCursor);

    connect(a, SIGNAL(clicked()), this, SLOT(slot_clickedMusic()));
    connect(b, SIGNAL(clicked()), this, SLOT(slot_clickedRoseTube()));
    connect(c, SIGNAL(clicked()), this, SLOT(slot_clickedTidal()));
    connect(d, SIGNAL(clicked()), this, SLOT(slot_clickedBugs()));
    connect(e, SIGNAL(clicked()), this, SLOT(slot_clickedQobuz()));
    //---------------------------------------------------

    // 하단에 구분 라인
    this->addUIControl_dividedLine_onMain();

    return label_mainTitle;
}
void TopMenuBar::addUIControl_dividedLine_onMain(){//cheon211008
    // 하단에 구분 라인
    QWidget *widget_line = new QWidget();
    widget_line->setFixedHeight(1);
    widget_line->setStyleSheet("background-color:#333333;");
    widget_line->setContentsMargins(0,0,0,0);
    hlayout_menuSub->addWidget(widget_line);
}
void TopMenuBar::slot_clickedMusic(){//cheon211008-serach

    print_debug();
    global.searchMenuFlag = true;
    emit linker->signal_clickedMovePageMusicSearch();

}

void TopMenuBar::slot_clickedBugs(){//cheon211008-serach
    print_debug();
    global.searchMenuFlag = true;
    //emit linker->signal_clickedMovePageBugsSearch();

}
void TopMenuBar::slot_clickedTidal(){//cheon211008-serach
    print_debug();
    global.searchMenuFlag = true;
    emit linker->signal_clickedMovePageTidalSearch();

}
void TopMenuBar::slot_clickedQobuz(){//cheon211008-serach
    print_debug();
    global.searchMenuFlag = true;
    emit linker->signal_clickedMovePageQobuzSearch();

}
void TopMenuBar::slot_clickedRoseTube(){//cheon211008-serach
    print_debug();
    global.searchMenuFlag = true;
    emit linker->signal_clickedMovePageRoseTubeSearch();

}*/

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
        widget_topMenuBar->setStyleSheet("background-color:transparent;");
        line_bottom_1->setStyleSheet("background-color:transparent;");
        line_bottom_2->setStyleSheet("background-color:transparent;");
    }else{
        widget_topMenuBar->setStyleSheet("background-color:#171717;");
        line_bottom_1->setStyleSheet("background-color:#232323;");
        line_bottom_2->setStyleSheet("background-color:#282828;");
    }
}

/**
 * @brief TopMenuBar::resizeEvent [override] 리사이즈 이벤트
 * @param event
 */
void TopMenuBar::resizeEvent(QResizeEvent *event){

    Q_UNUSED(event);
    if(this->showMenuStyle==ShowMenuStyle::TopSubMenuBar){
        QString tmp_name = GSCommon::getTextCutFromLabelWidth(this->title_org, (this->width()-100), this->lb_mainName->font());
        this->lb_mainName->setText(tmp_name);
    }
}
