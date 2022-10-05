#include "TagListWidget.h"
#include <QPushButton>
#include <QDebug>
#include "common/gscommon.h"

const char* KEY_INDEX_BTN = "idx";

/**
 * @brief TagListWidget::TagListWidget
 * @param parent
 */
TagListWidget::TagListWidget(QWidget *parent) : QWidget(parent)
{
    // UI
    this->setUIControl_basic();

    // init
    this->clear_tags();
}


/**
 * @brief TagListWidget::setUIControl_basic
 */
void TagListWidget::setUIControl_basic(){

    // main layout
    this->flowLayout_main = new FlowLayout(4, 6);
    this->flowLayout_main->setSizeConstraint(QLayout::SetMinimumSize);
    this->flowLayout_main->setContentsMargins(0,0,0,0);

    this->setLayout(this->flowLayout_main);

    this->setContentsMargins(0,0,0,0);
    this->setMinimumHeight(20);

}


/**
 * @brief [Slot] 태그 버튼 클릭 처리
 */
void TagListWidget::slot_btnTagClicked(){
    int index = sender()->property(KEY_INDEX_BTN).toInt();
    emit this->signal_clickedTags(index);
}


/**
 * @brief 태그 1개 추가. (중복 처리 하지 않음)
 * @param code
 * @param name
 */
void TagListWidget::add_tag(QString tag_nm){

    QPushButton *btn_tag = new QPushButton();
    btn_tag->setProperty(KEY_INDEX_BTN, this->num_tags);
    btn_tag->setText("#" + tag_nm);
    btn_tag->setStyleSheet("font-size:14px; background-color:#333333; color:#919191; padding: 2px 7px;");
    btn_tag->setCursor(Qt::PointingHandCursor);
    btn_tag->setFixedHeight(20);
    connect(btn_tag, &QPushButton::clicked, this, &TagListWidget::slot_btnTagClicked);

    this->num_tags += 1;
    this->flowLayout_main->addWidget(btn_tag);
}


/**
 * @brief 태그 여러개 추가 (중복 처리 하지 않음)
 * @param list_tag_nm
 */
void TagListWidget::add_tag_list(QList<QString> list_tag_nm){
    foreach(QString tmp_tag_nm, list_tag_nm){
        this->add_tag(tmp_tag_nm);
    }
}


/**
 * @brief FilterWidget::clear_tags
 */
void TagListWidget::clear_tags(){
    this->num_tags = 0;
    GSCommon::clearLayout(this->flowLayout_main);
}


