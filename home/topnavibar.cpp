#include "common/gscommon.h"
#include "common/global.h"//searchtextcopy
#include "topnavibar.h"

#include <QLabel>

//const int MAX_LENGTH_SEARCH = 30;
const int TOP_NAVIBAR_H = 70;

TopNaviBar::TopNaviBar(QWidget *parent) : QWidget(parent)
{

    linker = Linker::getInstance();//c220628

    this->setFixedHeight(TOP_NAVIBAR_H);

    // Search Bar
    this->searchRemoteBar = new SearchRemoteBar();

    this->btn_back = GSCommon::getUIBtnImg("btn_back", ":/images/icon_back_g.png");
    this->btn_next = GSCommon::getUIBtnImg("btn_next", ":/images/icon_next_g.png");
    this->btn_back->setFixedSize(80,70);
    this->btn_next->setFixedSize(80,70);
    this->btn_back->setCursor(Qt::ArrowCursor);
    this->btn_next->setCursor(Qt::ArrowCursor);


    QHBoxLayout *hlayout_etc = new QHBoxLayout();
    hlayout_etc->setContentsMargins(0,0,0,0);
    hlayout_etc->setSpacing(0);
    hlayout_etc->addWidget(this->btn_back, 0, Qt::AlignLeft);
    hlayout_etc->addWidget(this->btn_next, 0, Qt::AlignLeft);
    hlayout_etc->addStretch(10);
    hlayout_etc->addWidget(this->searchRemoteBar, 8, Qt::AlignRight);

    QWidget *widget = new QWidget();
    widget->setObjectName("topNaviBar");
    widget->setLayout(hlayout_etc);
    //widget->setStyleSheet("#topNaviBar { background-color:transparent; border-bottom:1px solid transparent; }");
    widget->setStyleSheet("#topNaviBar { background-color:transparent; }");

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget);

    this->setLayout(vl_total);

    // 커넥트
    connect(this->btn_back, SIGNAL(clicked()), this, SLOT(clickedBackBtn()));
    connect(this->btn_next, SIGNAL(clicked()), this, SLOT(clickedNextBtn()));
    connect(this->searchRemoteBar, SIGNAL(clickedRemote(bool)), this, SIGNAL(clickedRemote(bool)));
    connect(linker, SIGNAL(signal_clickListBtn()), this, SLOT(slot_searchBarClear()));//c220628
    connect(linker, SIGNAL(signal_text_copy()), this, SLOT(slot_search_text_copy()));//c220726
}

void TopNaviBar::slot_searchBarClear(){//c220628
    print_debug();
    this->searchRemoteBar->searchbarClear();
}
void TopNaviBar::search_text_copy(){//searchtextcopy
    this->searchRemoteBar->setsearchtext(global.search_text);//c220726
}

void TopNaviBar::search_text_clear(){
    print_debug();
    this->searchRemoteBar->setsearchtext("");
}
void TopNaviBar::slot_search_text_clear(){//c220726
    print_debug();
    this->searchRemoteBar->setsearchtext("");
}
void TopNaviBar::slot_search_text_copy(){//c220726
    print_debug();
    this->searchRemoteBar->setsearchtext(global.search_text);//c220726
}

void TopNaviBar::hideRemoveWidget(){
    this->searchRemoteBar->setInitUIRemoteBtn();
}
void TopNaviBar::showRemoveWidget(){
    this->searchRemoteBar->setSelectedRemoteBtnStyle();
}

/**
 * @brief TopNaviBar::setEnabledBtnBack 이전버튼 활성화 유무 세팅
 * @param p_flagEnabled
 */
void TopNaviBar::setEnabledBtnBack(bool p_flagEnabled){
    QString tmp_style = this->btn_back->styleSheet();
    this->flagEnabledBack = p_flagEnabled;
    if(this->flagEnabledBack==true){
        tmp_style.replace("/images/icon_back_g.png", "/images/icon_back_w.png");
        this->btn_back->setCursor(Qt::PointingHandCursor);
    }else{
        tmp_style.replace("/images/icon_back_w.png", "/images/icon_back_g.png");
        this->btn_back->setCursor(Qt::ArrowCursor);
    }
    this->btn_back->setStyleSheet(tmp_style);
}

/**
 * @brief TopNaviBar::setEnabledBtnNext 다음버튼 활성화 유무 세팅
 * @param p_flagEnabled
 */
void TopNaviBar::setEnabledBtnNext(bool p_flagEnabled){
    QString tmp_style = this->btn_next->styleSheet();
    this->flagEnabledNext = p_flagEnabled;
    if(this->flagEnabledNext==true){
        tmp_style.replace("/images/icon_next_g.png", "/images/icon_next_w.png");
        this->btn_next->setCursor(Qt::PointingHandCursor);
    }else{
        tmp_style.replace("/images/icon_next_w.png", "/images/icon_next_g.png");
        this->btn_next->setCursor(Qt::ArrowCursor);
    }
    this->btn_next->setStyleSheet(tmp_style);
}

/**
 * @brief TopNaviBar::clickedBackBtn [SLOT] 이전 아이콘 클릭시
 */
void TopNaviBar::clickedBackBtn(){
    if(this->flagEnabledBack==true){
        emit clickedBack();
    }
}
/**
 * @brief TopNaviBar::clickedNextBtn [SLOT] 다음 아이콘 클릭시
 */
void TopNaviBar::clickedNextBtn(){
    if(this->flagEnabledNext==true){
        emit clickedNext();
    }
}

/**
 * @brief TopNaviBar::resizeEvent [override]
 * @param event
 */
void TopNaviBar::resizeEvent(QResizeEvent * /*event*/){
    /*if(this->size().width()<=1150){
        this->searchRemoteBar->resetFixedWidth(450);
    }else{
        this->searchRemoteBar->resetFixedWidth(600);
    }*/

    this->searchRemoteBar->resetFixedWidth(600);
}
