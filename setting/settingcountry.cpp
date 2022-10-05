#include "settingcountry.h"

#include <common/gscommon.h>
#include "common/rosesettings.h"

#include <QScrollArea>
#include <QSettings>
#include <QComboBox>
#include <QDebug>

/**
 * @brief SettingCountry::SettingCountry 생성자
 * @param parent
 */
SettingCountry::SettingCountry(QWidget *parent) : QWidget(parent)
{
    this->setUIControl();
}

/**
 * @brief SettingCountry::setUIControl 기본 UI 세팅
 */
void SettingCountry::setUIControl()
{
    // 전체
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->setAlignment(Qt::AlignTop);
    vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Country Channel Option")));
    //vl_total->addWidget(GSCommon::getUIControlTitleForSetting(tr("Rose Podcast Channel")));
    vl_total->addWidget(this->getUIControlOption(tr("Rose Podcast Channel")));
    //vl_total->addWidget(this->getUIControlOption(tr("로즈팟캐스트 채널")));

    // 스크롤
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setContentsMargins(0,0,0,0);
    widget_scrollArea->setObjectName("widget_scrollArea");
    widget_scrollArea->setLayout(vl_total);
    widget_scrollArea->setStyleSheet("#widget_scrollArea { border-left:1px solid #303030; }");

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea");
    scrollArea->setWidget(widget_scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);

    this->setLayout(vl_scroll);
}

/**
 * @brief SettingCountry::getUIControlOption 콤보박스 포함된 ROW 생성한다
 * @param p_title
 * @return
 */
QWidget* SettingCountry::getUIControlOption(QString p_title){

    QLabel *lb_title = new QLabel(p_title);
    lb_title->setStyleSheet("color:#E5E5E4;font-size:18px;");

    this->list_keyValue.append(2);
    this->list_keyValue.append(3);
    this->list_keyValue.append(11);
    this->list_keyValue.append(18);
    this->list_keyValue.append(45);
    this->list_keyValue.append(100);

    comboBox_pod = new QComboBox();
    comboBox_pod->setContentsMargins(0,0,0,0);
    comboBox_pod->setObjectName("comboBox_pod");
    comboBox_pod->setFixedWidth(340);
    comboBox_pod->addItem(tr("KOREA"), 2);
    comboBox_pod->addItem(tr("USA"), 3);
    comboBox_pod->addItem(tr("FRANCE"), 11);
    comboBox_pod->addItem(tr("ITALY"), 18);
    comboBox_pod->addItem(tr("CZECH"), 45);
    comboBox_pod->addItem(tr("SLOVAKIA"), 100);
    comboBox_pod->setStyleSheet("QComboBox { font-size:18px;color:#FFFFFF; background-color:#333333; padding:5px;  } "
                                "QComboBox QListView { font-size:18px;color:#FFFFFF; background-color:#333333; } "
                                "QComboBox QAbstractItemView { selection-color:#FFFFFF; selection-background-color:#B18658; border: 1px solid #B18658;   } "
                                //"QAbstractItemView QAbstractItemView::item {  padding-left:5px; } "
                                //"QComboBox QAbstractItemView:selected { color:red;selection-background-color:red } "
                                //"QComboBox QAbstractItemView:hover { color:red;selection-background-color:red } "
                                );
    /*
    "QComboBox::item { font-size:18px;color:red; background-color:#333333; }"

    " QListView::item { color:red; } "
    " QListView::item:selected { border: 1px solid #B18658;background-color:#B18658;  }"
    " QListView::item:hover { background-color:#B18658; } "
    "QListView::item:selected:active { background-color:#B18658; } "

    "#comboBox_pod:item { color:#FFFFFF;height:35px; } "
    "#comboBox_pod:item:selected { background-color:#B18658; } "
    "#comboBox_pod:item::hover { background-color:#B18658; } "
    */

    QHBoxLayout *hl_lb = new QHBoxLayout();
    hl_lb->setContentsMargins(33,15,47,15);
    hl_lb->setSpacing(0);
    hl_lb->addWidget(lb_title);
    hl_lb->addWidget(comboBox_pod);

    QWidget *widget_total = new QWidget();
    widget_total->setObjectName("widget_total");
    widget_total->setStyleSheet("#widget_total { border-bottom:1px solid #303030;border-left:1px solid #303030; } ");
    widget_total->setLayout(hl_lb);

    // 커넥션
    //connect(comboBox, &QComboBox::currentIndexChanged, this, &SettingCountry::slot_changedIndexPod);
    connect(comboBox_pod, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changedIndexPod(int)));


    return widget_total;
}


/**
 * @brief SettingCountry::getSettingInfo 설정정보 가져와서 세팅
 */
void SettingCountry::getSettingInfo(){

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int tmp_podcast = settings->value(rosesettings.SETTINGS_PODCAST_COUNTRY).toInt();

    // 콤보박스 현재 index 세팅
    int tmp_index = 0;
    for(int i=0; i<this->list_keyValue.count(); i++){
        if(this->list_keyValue.at(i) == tmp_podcast){
            tmp_index = i;
            break;
        }
    }
    this->comboBox_pod->setCurrentIndex(tmp_index);
}


/**
 * @brief SettingCountry::slot_changedIndexPod [SLOT] 팟캐스트 콤보박스 index 변경시
 * @param index
 */
void SettingCountry::slot_changedIndexPod(int index){

    Q_UNUSED(index);
    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    settings->setValue(rosesettings.SETTINGS_PODCAST_COUNTRY, this->comboBox_pod->currentData());
}
