#include "SegmentTabWidget.h"

#include "common/gscommon.h"
#include <QVariant>


/**
 * @brief SegmentTabWidget::SegmentTabWidget
 * @param parent
 */
SegmentTabWidget::SegmentTabWidget(QWidget *parent) : QWidget(parent)
{
    this->selected_index = -1;
    this->max_index = -1;

    this->list_btn = new QList<QPushButton*>();

    this->setUIControl();
}


/**
 * @brief SegmentTabWidget::setUIControl
 */
void SegmentTabWidget::setUIControl(){

    // layout
    this->hBox_main = new QHBoxLayout;
    this->hBox_main->setContentsMargins(0,0,0,0);
    this->hBox_main->setSpacing(0);


    this->setLayout(this->hBox_main);
    this->setContentsMargins(0,0,0,0);
    this->setFixedHeight(40);

}


/**
 * @brief SegmentTabWidget::setList_tabName
 * @param list_name
 */
void SegmentTabWidget::setList_tabName(QList<QString>& list_name){

    this->max_index = list_name.length() - 1;
    this->list_btn->clear();
    GSCommon::clearLayout(this->hBox_main);

    for(int i=0 ; i<list_name.length() ; i++){

        QPushButton *tmp_btn = new QPushButton();
        tmp_btn->setContentsMargins(0,0,0,0);
        tmp_btn->setText(list_name.at(i));
        tmp_btn->setProperty("index", i);
        tmp_btn->setCursor(Qt::PointingHandCursor);
        connect(tmp_btn, &QPushButton::clicked, this, &SegmentTabWidget::slot_clickBtns);

        // append layout
        this->hBox_main->addWidget(tmp_btn);

        // append data
        this->list_btn->append(tmp_btn);
    }

    this->applyBtnStyle();
}


/**
 * @brief [Slot] 탭 버튼 클릭 -> 시그널 발생
 */
void SegmentTabWidget::slot_clickBtns(){
    int index = sender()->property("index").toInt();

    this->set_selectedIndex(index);
    emit this->signal_clicked(index);
}


/**
 * @brief SegmentTabWidget::applyBtnStyle
 */
void SegmentTabWidget::applyBtnStyle(){

    const QString STYLE_base = "font-size:18px; border:1px solid #707070; padding-left:20px; padding-right:20px;";

    const QString STYLE_normal = STYLE_base + "background-color:transparent; color:#cccccc;";
    const QString STYLE_hover = STYLE_base + "background-color:#B18658; color:#FFFFFF;";
    const QString STYLE_selected = STYLE_base + "background-color:#B18658; color:#FFFFFF;";

    const QString STYLE_radius_right = "border-top-right-radius:20px;border-bottom-right-radius:20px;";
    const QString STYLE_radius_left = "border-top-left-radius:20px;border-bottom-left-radius:20px;";

    for(int i = 0; i <= this->max_index; i++){
        QString tmp_style = "";
        if(i == 0){
            // first
            tmp_style = i == this->selected_index ? STYLE_selected : STYLE_normal;
            tmp_style += STYLE_radius_left;
        }
        else if(i == this->max_index){
            // end
            tmp_style = i == this->selected_index ? STYLE_selected : STYLE_normal;
            tmp_style += STYLE_radius_right;
        }
        else{
            // middle
            tmp_style = i == this->selected_index ? STYLE_selected : STYLE_normal;
        }

        this->list_btn->at(i)->setStyleSheet(QString(".QPushButton{%1} .QPushButton:hover{%2}").arg(tmp_style).arg(STYLE_hover));
        this->list_btn->at(i)->setFixedHeight(40);

    }
}


/**
 * @brief SegmentTabWidget::set_selectedIndex
 * @param index
 */
void SegmentTabWidget::set_selectedIndex(int index){

    if(index < 0 || index > this->max_index){
        index = 0;
    }

    if(index != this->selected_index){
        this->selected_index = index;
        this->applyBtnStyle();
    }

}


/**
 * @brief 버튼 사이즈를 고정한다.  @see setList_tabName 함수 호출 후에 사용할 것!
 * @param size
 */
void SegmentTabWidget::set_fixedTabBtnSize(const QSize& size){

    for(int i = 0; i < this->list_btn->count(); i++){
        this->list_btn->at(i)->setFixedSize(size);
    }
}
