#include "folderwidgetgroup.h"

#include <common/gscommon.h>
#include <common/global.h>//c220609

#include <QDebug>
#include <QScroller>
#include <QScrollArea>

/**
 * @brief Folder_TopDirName::Folder_TopDirName 생성자
 * @param p_dirName
 * @param p_dirPath
 * @param parent
 */
Folder_TopDirName::Folder_TopDirName(QString p_dirName, QString p_dirPath, QWidget *parent)
    : QWidget(parent)
    , dirName(p_dirName)
    , dirPath(p_dirPath)
{
    this->setCursor(Qt::PointingHandCursor);

    QLabel *lb_dirName = new QLabel(p_dirName);
    lb_dirName->setProperty("dirName", p_dirName);
    lb_dirName->setProperty("dirPath", p_dirPath);
    QLabel *lb_icon_folder_gray = GSCommon::getUILabelImg(":/images/icon_folder_gray.png");
    //QLabel *lb_icon_fol_next_gold = GSCommon::getUILabelImg(":/images/icon_fol_next_gold.png");
    lb_dirName->setStyleSheet("font-size:16px;color:#FFFFFF;background-color:transparent");
    lb_icon_folder_gray->setStyleSheet("background-color:transparent");
    //lb_icon_fol_next_gold->setStyleSheet("background-color:transparent");

    QHBoxLayout *hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(0,0,0,0);
    hl_total->setSpacing(0);
    hl_total->addWidget(lb_icon_folder_gray);
    hl_total->addWidget(lb_dirName);
    //hl_total->addSpacing(15);
    //hl_total->addWidget(lb_icon_fol_next_gold);

    this->setLayout(hl_total);
}


void Folder_TopDirName::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);        // by sunnyfish
    emit signal_clicked(this->dirName, this->dirPath);
}





/**
 * @brief Folder_TopDirPath::Folder_TopDirPath 생성자
 * @param parent
 */
Folder_TopDirPath::Folder_TopDirPath(QWidget *parent) : QWidget(parent)
{
    this->list_folderDirName = QList<Folder_TopDirName*>();
    this->list_hlDirName = QList<QHBoxLayout*>();

    hl_total = new QHBoxLayout();
    hl_total->setContentsMargins(27,0,0,0);
    hl_total->setSpacing(0);
    hl_total->setAlignment(Qt::AlignLeft);

    widget_topTitle = new QWidget();
    widget_topTitle->setObjectName("widget_topTitle");
    widget_topTitle->setStyleSheet("#widget_topTitle { background-color:#171717; }");
    widget_topTitle->setFixedHeight(70);
    widget_topTitle->setLayout(hl_total);

    vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_topTitle);

    this->setLayout(vl_total);

    // 초기화
    //this->clearDirName();
}


/**
 * @brief Folder_TopDirPath::~Folder_TopDirPath : 소멸자
 */
Folder_TopDirPath::~Folder_TopDirPath(){
    qDeleteAll(list_folderDirName.begin(), list_folderDirName.end());
    qDeleteAll(list_hlDirName.begin(), list_hlDirName.end());
    this->list_folderDirName.clear();
    this->list_hlDirName.clear();
}


/**
 * @brief Folder_TopDirPath::addDirName DirName 추가
 * @param p_dirName
 * @param p_dirPath
 */
void Folder_TopDirPath::addDirName(QString p_dirName, QString p_dirPath){
//print_debug();
    Folder_TopDirName *folder_dirName = new Folder_TopDirName(p_dirName, p_dirPath);
    connect(folder_dirName, SIGNAL(signal_clicked(QString, QString)), this, SLOT(slot_clickedDirName(QString, QString)));
    this->list_folderDirName.append(folder_dirName);


    QLabel *lb_icon_fol_next_gold = GSCommon::getUILabelImg(":/images/icon_fol_next_gold.png");
    lb_icon_fol_next_gold->setStyleSheet("background-color:transparent");

    QHBoxLayout *hl_dirName = new QHBoxLayout();
    hl_dirName->setContentsMargins(0,0,0,0);
    hl_dirName->setSpacing(0);
    hl_dirName->addWidget(this->list_folderDirName.last());
    hl_dirName->addSpacing(15);
    hl_dirName->addWidget(lb_icon_fol_next_gold);

    this->list_hlDirName.append(hl_dirName);

    this->hl_total->addLayout(this->list_hlDirName.last());
}


/**
 * @brief Folder_TopDirPath::setRootDirName : root DirName , path 세팅
 * @param p_dirName
 * @param p_dirPath
 */
void Folder_TopDirPath::setRootDirName(QString p_dirName, QString p_dirPath){
    clearDirName();

    addDirName(p_dirName, p_dirPath);
}


/**
 * @brief Folder_TopDirPath::clearDirName 초기화
 */
void Folder_TopDirPath::clearDirName(){

    // 초기화
    for(int i=(this->list_folderDirName.count()-1); i>=0; i--){
        GSCommon::clearLayout(this->list_hlDirName.at(i));
        this->hl_total->removeItem(this->list_hlDirName.at(i));
    }
    this->list_folderDirName.clear();
    this->list_hlDirName.clear();
    GSCommon::clearLayout(this->hl_total);
    //this->addDirName(tr("Explore"), FOLDER_USB_DIR_CODE_INIT);
    //this->addDirName(tr("탐색"), FOLDER_USB_DIR_CODE_INIT);
}


/**
 * @brief Folder_TopDirPath::slot_clickedDirName [SLOT] 디렉토리명 클릭시
 */
void Folder_TopDirPath::slot_clickedDirName(QString p_dirName, QString p_dirPath){

    // 클릭한 객체부터 끝까지 모두 제거
    int currIndex = this->list_folderDirName.indexOf(dynamic_cast<Folder_TopDirName*>(sender()));

    for(int i=(this->list_folderDirName.size()-1); i>=currIndex; i--){
        GSCommon::clearLayout(this->list_hlDirName.at(i));
        this->hl_total->removeItem(this->list_hlDirName.at(i));
        this->list_hlDirName.removeAt(i);
        this->list_folderDirName.removeAt(i);
    }

    // 해당 path 로 Dir경로 변경해라
    emit signal_changedDirPath(p_dirName, p_dirPath);
}





/**
 * @brief Folder_ClickableRow::Folder_ClickableRow 생성자
 * @param p_iconPath
 * @param p_text
 * @param parent
 */
Folder_ClickableRow::Folder_ClickableRow(QString p_iconPath, QString p_text, QWidget *parent) : QWidget(parent)
{
    this->setCursor(Qt::PointingHandCursor);

    QLabel *lb_icon = GSCommon::getUILabelImg(p_iconPath);
    QLabel *lb_text = new QLabel();
    lb_text->setText(p_text);
    lb_icon->setStyleSheet("background-color:transparent;");
    lb_text->setStyleSheet("font-size:16px;color:#E5E5E4;background-color:transparent;");
    QHBoxLayout *hl_row = new QHBoxLayout();
    hl_row->setContentsMargins(0,0,0,0);
    hl_row->setSpacing(0);
    hl_row->setAlignment(Qt::AlignLeft);
    hl_row->addWidget(lb_icon);
    hl_row->addSpacing(15);
    hl_row->addWidget(lb_text);

    QWidget *widget_row = new QWidget();
    widget_row->setObjectName("widget_row");
    widget_row->setStyleSheet("#widget_row { border-bottom:2px solid #282828; } #widget_row:hover { background-color:#333333; } ");
    widget_row->setLayout(hl_row);
    QVBoxLayout *vl_row = new QVBoxLayout();
    vl_row->setContentsMargins(33,0,38,0);
    vl_row->setSpacing(0);
    vl_row->addWidget(widget_row);

    this->setLayout(vl_row);
}


void Folder_ClickableRow::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);    // by sunnyfish
    emit signal_clicked();
}




/**
 * @brief FolderNetwork_ClickableRow::FolderNetwork_ClickableRow 생성자
 * @param p_iconPath
 * @param p_text
 * @param parent
 */
FolderNetwork_ClickableRow::FolderNetwork_ClickableRow(QString p_iconPath, QString p_text, QWidget *parent) : QWidget(parent)
{
    this->setCursor(Qt::PointingHandCursor);


    //lb_icon = GSCommon::getUILabelImg(p_iconPath);
    lb_icon = new QLabel;
    lb_icon->setPixmap(*GSCommon::getUIPixmapImg(p_iconPath));
    QLabel *lb_text = new QLabel();
    lb_text->setText(p_text);
    lb_icon->setStyleSheet("background-color:transparent;");
    lb_text->setStyleSheet("font-size:16px;color:#E5E5E4;background-color:transparent;");

    QPushButton *btn_del = GSCommon::getUIBtnImg("btn_remove", ":/images/fol_del_ico.png");
    btn_del->setFixedSize(QSize(40,40));
    btn_del->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_row = new QHBoxLayout();
    hl_row->setContentsMargins(0,0,0,0);
    hl_row->setSpacing(0);
    hl_row->setAlignment(Qt::AlignLeft);
    hl_row->addWidget(lb_icon);
    hl_row->addSpacing(15);
    hl_row->addWidget(lb_text);
    hl_row->addStretch(1);
    hl_row->addWidget(btn_del);

    QWidget *widget_row = new QWidget();
    widget_row->setObjectName("widget_row");
    widget_row->setStyleSheet("#widget_row { border-bottom:2px solid #282828; } #widget_row:hover { background-color:#333333; } ");
    widget_row->setLayout(hl_row);

    QVBoxLayout *vl_row = new QVBoxLayout();
    vl_row->setContentsMargins(33,0,38,0);
    vl_row->setSpacing(0);
    vl_row->addWidget(widget_row);

    this->setLayout(vl_row);

    connect(btn_del, SIGNAL(clicked()), SIGNAL(signal_clickedRemove()));
}


void FolderNetwork_ClickableRow::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);        // by sunnyfish
    emit signal_clicked();
}


void FolderNetwork_ClickableRow::setImage(QString p_iconPath){
    lb_icon->setPixmap(*GSCommon::getUIPixmapImg(p_iconPath));
}





/**
 * @brief FolderCloud_ClickableRow::FolderCloud_ClickableRow 생성자
 * @param p_iconPath
 * @param p_text
 * @param parent
 */
FolderCloud_ClickableRow::FolderCloud_ClickableRow(QString p_iconPath, QString p_name, QString p_path, QWidget *parent) : QWidget(parent)
{
    this->setCursor(Qt::PointingHandCursor);


    //lb_icon = GSCommon::getUILabelImg(p_iconPath);
    lb_icon = new QLabel;
    lb_icon->setPixmap(*GSCommon::getUIPixmapImg(p_iconPath));
    QLabel *lb_text = new QLabel();
    lb_text->setText(p_name);
    lb_icon->setStyleSheet("background-color:transparent;");
    lb_text->setStyleSheet("font-size:16px;color:#E5E5E4;background-color:transparent;");

    QPushButton *btn_del = GSCommon::getUIBtnImg("btn_remove", ":/images/fol_del_ico.png");
    btn_del->setFixedSize(QSize(40,40));
    btn_del->setCursor(Qt::PointingHandCursor);

    QPushButton *btn_next = GSCommon::getUIBtnImg("btn_next", ":/images/popup_more_ico.png");
    btn_next->setFixedSize(QSize(40,40));
    btn_next->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hl_row = new QHBoxLayout();
    hl_row->setContentsMargins(0,0,0,0);
    hl_row->setSpacing(0);
    hl_row->setAlignment(Qt::AlignLeft);
    hl_row->addWidget(lb_icon);
    hl_row->addSpacing(15);
    hl_row->addWidget(lb_text);
    hl_row->addStretch(1);
    if(p_path.isEmpty()){
        hl_row->addWidget(btn_del);
    }
    else{
        hl_row->addWidget(btn_next);
    }

    QWidget *widget_row = new QWidget();
    widget_row->setObjectName("widget_row");
    widget_row->setStyleSheet("#widget_row { border-bottom:2px solid #282828; } #widget_row:hover { background-color:#333333; } ");
    widget_row->setLayout(hl_row);

    QVBoxLayout *vl_row = new QVBoxLayout();
    vl_row->setContentsMargins(33,0,38,0);
    vl_row->setSpacing(0);
    vl_row->addWidget(widget_row);

    this->setLayout(vl_row);

    connect(btn_del, SIGNAL(clicked()), SIGNAL(signal_clickedRemove()));
    connect(btn_next, SIGNAL(clicked()), SIGNAL(signal_clicked()));
}


void FolderCloud_ClickableRow::mousePressEvent(QMouseEvent *event){
    Q_UNUSED(event);        // by sunnyfish
    emit signal_clicked();
}


void FolderCloud_ClickableRow::setImage(QString p_iconPath){
    lb_icon->setPixmap(*GSCommon::getUIPixmapImg(p_iconPath));
}
