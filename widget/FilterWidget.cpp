#include "FilterWidget.h"

#include <QPushButton>
#include "common/gscommon.h"

/**
 * @brief FilterWidget::FilterWidget
 * @param parent
 */
FilterWidget::FilterWidget(QWidget *parent) : QWidget(parent)
{
    // Data
    this->list_code = new QList<QString>();
    this->list_name = new QList<QString>();

    // UI
    this->setUIControl_basic();
}


/**
 * @brief FilterWidget::setUIControl_basic
 */
void FilterWidget::setUIControl_basic(){

    // Filter Icon Button
    QPushButton *btn_filter = new QPushButton();
    btn_filter->setContentsMargins(0,0,0,0);
    btn_filter->setIcon(QPixmap(":/images/ico_filter.png"));
    btn_filter->setIconSize(QSize(80,70));
    btn_filter->setStyleSheet("background-color:transparent;");
    btn_filter->setCursor(Qt::PointingHandCursor);
    btn_filter->setFixedSize(80,70);
    connect(btn_filter, &QPushButton::clicked, this, &FilterWidget::slot_btn_filterClicked);


    // layout of Filter View
    this->hBox_viewFiltered = new QHBoxLayout();
    this->hBox_viewFiltered->setSpacing(0);
    this->hBox_viewFiltered->setContentsMargins(0,0,0,0);


    QHBoxLayout * hBox_main = new QHBoxLayout();
    hBox_main->setSpacing(0);
    hBox_main->setContentsMargins(0,0,0,0);
    hBox_main->addWidget(btn_filter, 0, Qt::AlignLeft);
    hBox_main->addLayout(this->hBox_viewFiltered);


    this->setLayout(hBox_main);

    this->setContentsMargins(0,0,0,0);
    this->setFixedHeight(70);

}


/**
 * @brief [Slot] 버튼 클릭
 */
void FilterWidget::slot_btn_filterClicked(bool checked){
    Q_UNUSED(checked)
    emit this->signal_clickedFilter();
}


/**
 * @brief 필터 코드 추가 (중복체크 하지 않음.)
 * @param code
 * @param name
 */
void FilterWidget::add_filter(QString code, QString name){

    this->list_code->append(code);
    this->list_name->append(name);

    QLabel *tmp_label = new QLabel();
    tmp_label->setContentsMargins(20,6,20,6);
    tmp_label->setText(name);
//    tmp_label->setFixedHeight(30);
    tmp_label->setStyleSheet("font-size:16px; background-color:transparent; color:#ffffff; border:1px solid #707070; border-radius:15px;");

    this->hBox_viewFiltered->addWidget(tmp_label, 0, Qt::AlignLeft|Qt::AlignVCenter);
}


/**
 * @brief FilterWidget::clear_filter
 */
void FilterWidget::clear_filter(){
    this->list_code->clear();
    this->list_name->clear();

    GSCommon::clearLayout(this->hBox_viewFiltered);
}




