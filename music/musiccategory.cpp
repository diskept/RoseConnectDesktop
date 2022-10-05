#include "musiccategory.h"

#include <common/gscommon.h>
#include <common/rosesettings.h>
#include <QDebug>
#include <QString>
#include <string.h>

const char* KEY_FILTER = "filter";
const char* KEY_IDX = "idx";

const QString SORT_STEP_AUDIO = "audio";
const QString SORT_STEP_KIND = "kind";
const QString SORT_STEP_ARTIST = "artist";
const QString SORT_STEP_COMPOSER = "composer";

const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";
const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";

/**
 * @brief MusicCategory::MusicCategory 음악 > 분류
 * @param parent
 */
MusicCategory::MusicCategory(QWidget *parent) : AbstractPage(parent)
{

    setInit();
    setUIControl();
}

/**
 * @brief MusicCategory::~MusicCategory : 소멸자
 */
MusicCategory::~MusicCategory(){

    delete settings;
    this->deleteLater();
}

/**
 * @brief MusicCategory::setInit : 초기 세팅
 */
void MusicCategory::setInit(){
    settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

    btnStyle_normal = "QPushButton{ ";
    btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:16px;";
    btnStyle_normal += "border:1px solid #707070; border-radius:15px;}";
    btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

    btnStyle_selected = "QPushButton{ ";
    btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:16px;";
    btnStyle_selected += "border:1px solid #BE905F; border-radius:15px;}";

    fileTypeList << tr("All") << tr("Lossless"); /// << "DSD64x" << "DSD128x" << "DSD256x" << "DSD512x" << "MQA";
    bitDepthList << tr("All") << "24bit↑";          /// << "16bit↑" << "24bit↑";
    samplingList << tr("All") << "88.2kHz↑" << "176.4kHz↑";  /// <<"44.1kHz↑" << "88.2kHz↑" << "176.4kHz↑" << "352.8kHz↑";    
    starList << tr("All") << "★★★★★↑" << "★★★★↑" << "★★★↑" << "★★↑" << "★↑" << tr("NONE");
    orderList << tr("NONE") <<"A-Z" << tr("Artist") << tr("Favorite");  /// << tr("Recently") << tr("Artist,Release date") << tr("Year(Ascending)") << tr("Year(Descending)");
    /*
     *
    fileTypeList << tr("전체") << tr("무손실PCM") << "DSD64x" << "DSD128x" << "DSD256x" << "DSD512x" << "MQA";
    bitDepthList << tr("전체") << "16bit↑" << "24bit↑";
    samplingList << tr("전체") << "44.1kHz↑" << "88.2kHz↑" << "176.4kHz↑" << "352.8kHz↑";
    orderList << tr("전체") << tr("최근순") << tr("선호도순") << "A-Z" << tr("아티스트순") << tr("아티스트,발매일") << tr("년도(오름차순)") << tr("년도(내림차순)");
    starList << tr("전체") << "★★★★★↑" << "★★★★↑" << "★★★↑" << "★★↑" << "★↑";
    */
}

/**
 * @brief MusicCategory::setUIControl : UI 세팅
 */
void MusicCategory::setUIControl(){

    // 상단 필터
    this->btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
    this->btn_filter->setFixedSize(80,70);
    this->btn_filter->setCursor(Qt::PointingHandCursor);
    this->btn_filter->hide();

    this->hl_filter = new QHBoxLayout();
    this->hl_filter->setContentsMargins(0,0,0,0);
    this->hl_filter->setSpacing(0);
    this->hl_filter->setAlignment(Qt::AlignLeft);
    this->hl_filter->addWidget(btn_filter);
    this->hl_filter->setSpacing(10);

    /*settings->setValue(rosesettings.SETTINGS_FILTER_FILETYPE, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_FILTER_BIT, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_FILTER_SAMPLEING, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_FILTER_ORDER, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_FILTER_STAR, QVariant(0));*/

    this->lb_file_type = new QLabel;
    this->lb_file_type->setContentsMargins(20,0,20,0);
    this->lb_file_type->setFixedHeight(30);
    this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
    this->lb_file_type->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
        int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt();
        this->lb_file_type->setText(fileTypeList.at(tmp_idx));

        hl_filter->addWidget(this->lb_file_type);
        //hl_filter->addSpacing(10);

        this->flag_file_type = true;
    }

    this->lb_bit_depth = new QLabel;
    this->lb_bit_depth->setContentsMargins(20,0,20,0);
    this->lb_bit_depth->setFixedHeight(30);
    this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
    this->lb_bit_depth->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
        int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt();
        this->lb_bit_depth->setText(bitDepthList.at(tmp_idx));

        hl_filter->addWidget(this->lb_bit_depth);
        //hl_filter->addSpacing(10);

        this->flag_bit_depth = true;
    }

    this->lb_sample_rate = new QLabel;
    this->lb_sample_rate->setContentsMargins(20,0,20,0);
    this->lb_sample_rate->setFixedHeight(30);
    this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
    this->lb_sample_rate->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
        int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt();
        this->lb_sample_rate->setText(samplingList.at(tmp_idx));

        hl_filter->addWidget(this->lb_sample_rate);
        //hl_filter->addSpacing(10);

        this->flag_sample_rate = true;
    }

    this->lb_star_point = new QLabel;
    this->lb_star_point->setContentsMargins(20,0,20,0);
    this->lb_star_point->setFixedHeight(30);
    this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
    this->lb_star_point->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
        int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt();
        this->lb_star_point->setText(starList.at(tmp_idx));

        hl_filter->addWidget(this->lb_star_point);
        //hl_filter->addSpacing(10);

        this->flag_star_point = true;
    }

    this->lb_order = new QLabel;
    this->lb_order->setContentsMargins(20,0,20,0);
    this->lb_order->setFixedHeight(30);
    this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
    this->lb_order->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() > 0){
        int tmp_idxOrder = settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt();
        this->lb_order->setText(orderList.at(tmp_idxOrder));

        hl_filter->addWidget(this->lb_order);
        //hl_filter->addSpacing(10);

        this->flag_order = true;
    }

    QWidget *widget_btnFilter = new QWidget();
    widget_btnFilter->setObjectName("widget_btnFilter");
    widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
    widget_btnFilter->setFixedHeight(70);
    widget_btnFilter->setLayout(hl_filter);
    //widget_btnFilter->hide();

    // 상단 step 버튼
    this->btn_sort_audio= new QPushButton(tr("Full Song"));
    //this->btn_sort_audio= new QPushButton(tr("전체 곡"));
    this->btn_sort_audio->setObjectName("btn_sort_audio");
    this->btn_sort_audio->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_AUDIO);
    this->btn_sort_kind= new QPushButton(tr("Genre"));
    //this->btn_sort_kind= new QPushButton(tr("장르"));
    this->btn_sort_kind->setObjectName("btn_sort_kind");
    this->btn_sort_kind->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_KIND);
    this->btn_sort_artist = new QPushButton(tr("Artist"));
    //this->btn_sort_artist = new QPushButton(tr("아티스트"));
    this->btn_sort_artist->setObjectName("btn_sort_artist");
    this->btn_sort_artist->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_ARTIST);
    this->btn_sort_composer = new QPushButton(tr("Composer"));
    //this->btn_sort_composer = new QPushButton(tr("작곡가"));
    this->btn_sort_composer->setObjectName("btn_sort_composer");
    this->btn_sort_composer->setProperty(KEY_CONTENT_STEP.toStdString().c_str(), SORT_STEP_COMPOSER);
    this->btn_sort_audio->setFixedSize(125,40);
    this->btn_sort_kind->setFixedSize(125,40);
    this->btn_sort_artist->setFixedSize(125,40);
    this->btn_sort_composer->setFixedSize(125,40);
    this->btn_sort_audio->setCursor(Qt::PointingHandCursor);
    this->btn_sort_kind->setCursor(Qt::PointingHandCursor);
    this->btn_sort_artist->setCursor(Qt::PointingHandCursor);
    this->btn_sort_composer->setCursor(Qt::PointingHandCursor);

    this->hl_filterAndBtn = new QHBoxLayout;
    this->hl_filterAndBtn->setContentsMargins(0,0,0,0);
    this->hl_filterAndBtn->setSpacing(0);
    this->hl_filterAndBtn->addWidget(widget_btnFilter);
    this->hl_filterAndBtn->addStretch(1);
    this->hl_filterAndBtn->addWidget(this->btn_sort_audio);
#if defined(Q_OS_MAC)
    this->hl_filterAndBtn->setSpacing(10);
#endif
    this->hl_filterAndBtn->addWidget(this->btn_sort_kind);
#if defined(Q_OS_MAC)
    this->hl_filterAndBtn->setSpacing(10);
#endif
    this->hl_filterAndBtn->addWidget(this->btn_sort_artist);
#if defined(Q_OS_MAC)
    this->hl_filterAndBtn->setSpacing(10);
#endif
    this->hl_filterAndBtn->addWidget(this->btn_sort_composer);
    this->hl_filterAndBtn->addSpacing(70);

    QWidget *widget_btnCategory = new QWidget();
    widget_btnCategory->setObjectName("widget_btnCategory");
    widget_btnCategory->setStyleSheet("#widget_btnCategory { background-color:#171717; }");
    widget_btnCategory->setFixedHeight(70);
    widget_btnCategory->setLayout(this->hl_filterAndBtn);

    // 4개 위젯 생성
    this->musicList_audio = new MusicList_One();
    this->musicList_kind = new MusicList_Group(MusicList_Group::GroupMode::KIND);
    this->musicList_artist = new MusicList_Group(MusicList_Group::GroupMode::ARTIST);
    this->musicList_composer = new MusicList_Group(MusicList_Group::GroupMode::COMPOSER);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->musicList_audio);
    this->stackedwidget->addWidget(this->musicList_kind);
    this->stackedwidget->addWidget(this->musicList_artist);
    this->stackedwidget->addWidget(this->musicList_composer);

    this->vl_total= new QVBoxLayout();
    this->vl_total->setContentsMargins(0,0,0,0);
    this->vl_total->setSpacing(0);
    this->vl_total->setAlignment(Qt::AlignTop);
    this->vl_total->addWidget(widget_btnCategory);
    this->vl_total->addSpacing(0);
    this->vl_total->addWidget(this->stackedwidget);

    setLayout(this->vl_total);

    // 커넥션
    connect(this->btn_sort_audio, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
    connect(this->btn_sort_kind, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
    connect(this->btn_sort_artist, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
    connect(this->btn_sort_composer, SIGNAL(clicked()), this, SLOT(slot_changedSubTabUI()));
    connect(btn_filter, SIGNAL(clicked()), SLOT(slot_clickedBtnFilter()));

    setFIlterUIControl();
}

/**
 * @brief MusicCategory::setFIlterUIControl : Filter 선택 UI 세팅
 */
void MusicCategory::setFIlterUIControl(){

    QLabel *lb_fileType = createFilterLabel("File Type");
    QLabel *lb_bitDepth = createFilterLabel("Bit Depth");
    QLabel *lb_sampling = createFilterLabel("Sampling Rate");    
    QLabel *lb_star = createFilterLabel(tr("Star Rating"));
    //QLabel *lb_star = createFilterLabel(tr("별점"));
    QLabel *lb_order = createFilterLabel(tr("Sort order"));
    //QLabel *lb_order = createFilterLabel(tr("정렬순서"));


    QHBoxLayout *box_header = new QHBoxLayout;
    box_header->setContentsMargins(0,0,0,0);
    box_header->setSpacing(0);
    box_header->addWidget(lb_fileType,1);
    box_header->addWidget(lb_bitDepth,1);
    box_header->addWidget(lb_sampling,1);    
    box_header->addWidget(lb_star,1);
    box_header->addWidget(lb_order,1);

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setLineWidth(1);
    line->setStyleSheet("background-color:#4D4D4D;border:none;");

    btn_file_type[0] = createFilterBtn("FileType", 0, tr("All"), settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() == 0 ? true:false);
    btn_file_type[1] = createFilterBtn("FileType", 1, tr("Lossless"), settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() == 1 ? true:false);

    QVBoxLayout *box_fileType = new QVBoxLayout;
    box_fileType->setContentsMargins(0,0,20,0);
    box_fileType->setSpacing(20);
    box_fileType->setAlignment(Qt::AlignTop);
    box_fileType->addWidget(btn_file_type[0]);
    box_fileType->addWidget(btn_file_type[1]);
    //box_fileType->addWidget(createFilterBtn("FileType", 0, tr("All"),true));
    //box_fileType->addWidget(createFilterBtn(tr("전체"),false));
    //box_fileType->addWidget(createFilterBtn("FileType", 1, tr("Lossless"),false));
    //box_fileType->addWidget(createFilterBtn(tr("무손실"),true));

    btn_bit_depth[0] = createFilterBtn("BitDepth", 0, tr("All"), settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() == 0 ? true:false);
    btn_bit_depth[1] = createFilterBtn("BitDepth", 1, "24bit↑", settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() == 1 ? true:false);

    QVBoxLayout *box_bitDepth = new QVBoxLayout;
    box_bitDepth->setContentsMargins(0,0,20,0);
    box_bitDepth->setSpacing(20);
    box_bitDepth->setAlignment(Qt::AlignTop);
    box_bitDepth->addWidget(btn_bit_depth[0]);
    box_bitDepth->addWidget(btn_bit_depth[1]);
    //box_bitDepth->addWidget(createFilterBtn("BitDepth", 0, tr("All"),true));
    //box_bitDepth->addWidget(createFilterBtn(tr("전체"),false));
    //box_bitDepth->addWidget(createFilterBtn("BitDepth", 1, "24bit↑",false));
    //box_bitDepth->addWidget(createFilterBtn("24bit↑",true));

    btn_sample_rate[0] = createFilterBtn("Sampling", 0, tr("All"), settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 0 ? true:false);
    btn_sample_rate[1] = createFilterBtn("Sampling", 1, "88.2kHz↑", settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1 ? true:false);
    btn_sample_rate[2] = createFilterBtn("Sampling", 2, "176.4kHz↑", settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2 ? true:false);

    QVBoxLayout *box_sampling = new QVBoxLayout;
    box_sampling->setContentsMargins(0,0,20,0);
    box_sampling->setSpacing(20);
    box_sampling->setAlignment(Qt::AlignTop);
    box_sampling->addWidget(btn_sample_rate[0]);
    box_sampling->addWidget(btn_sample_rate[1]);
    box_sampling->addWidget(btn_sample_rate[2]);
    //box_sampling->addWidget(createFilterBtn("Sampling", 0, tr("All"),true));
    //box_sampling->addWidget(createFilterBtn(tr("전체"),false));
    //box_sampling->addWidget(createFilterBtn("Sampling", 1, "88.2kHz↑",false));
    //box_sampling->addWidget(createFilterBtn("Sampling", 2, "176.4kHz↑",false));

    btn_star_point[0] = createFilterBtn("Star", 0, tr("All"), settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 0 ? true:false);
    btn_star_point[1] = createFilterBtn("Star", 1, "★★★★★↑", settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 1 ? true:false);
    btn_star_point[2] = createFilterBtn("Star", 2, "★★★★↑", settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 2 ? true:false);
    btn_star_point[3] = createFilterBtn("Star", 3, "★★★↑", settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 3 ? true:false);
    btn_star_point[4] = createFilterBtn("Star", 4, "★★↑", settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 4 ? true:false);
    btn_star_point[5] = createFilterBtn("Star", 5, "★↑", settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 5 ? true:false);
    btn_star_point[6] = createFilterBtn("Star", 6, tr("NONE"), settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6 ? true:false);

    QVBoxLayout *box_star = new QVBoxLayout;
    box_star->setContentsMargins(0,0,20,0);
    box_star->setSpacing(20);
    box_star->setAlignment(Qt::AlignTop);
    box_star->addWidget(btn_star_point[0]);
    //box_star->addWidget(btn_star_point[1]);
    //box_star->addWidget(btn_star_point[2]);
    //box_star->addWidget(btn_star_point[3]);
    //box_star->addWidget(btn_star_point[4]);
    //box_star->addWidget(btn_star_point[5]);
    //box_star->addWidget(btn_star_point[6]);
    //box_star->addWidget(createFilterBtn("Star", 0, tr("All"),true));
    //box_star->addWidget(createFilterBtn("Star", 1, "★★★★★↑",false));
    //box_star->addWidget(createFilterBtn("Star", 2, "★★★★↑",false));
    //box_star->addWidget(createFilterBtn("Star", 3, "★★★↑",false));
    //box_star->addWidget(createFilterBtn("Star", 4, "★★↑",false));
    //box_star->addWidget(createFilterBtn("Star", 5, "★↑",false));
    //box_star->addWidget(createFilterBtn("Star", 6, tr("NONE"),false));

    btn_order[0] = createFilterBtn("OrderBy", 0, tr("NONE"), settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 0 ? true:false);
    btn_order[1] = createFilterBtn("OrderBy", 1, tr("A-Z"), settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 1 ? true:false);
    btn_order[2] = createFilterBtn("OrderBy", 2, tr("Artist"), settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 2 ? true:false);
    btn_order[3] = createFilterBtn("OrderBy", 3, tr("Favorite"), settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 3 ? true:false);

    QVBoxLayout *box_order = new QVBoxLayout;
    box_order->setContentsMargins(0,0,0,0);
    box_order->setSpacing(20);
    box_order->setAlignment(Qt::AlignTop);
    box_order->addWidget(btn_order[0]);
    box_order->addWidget(btn_order[1]);
    box_order->addWidget(btn_order[2]);
    box_order->addWidget(btn_order[3]);
    //box_order->addWidget(createFilterBtn(tr("Recently order"),false));
    //box_order->addWidget(createFilterBtn(tr("최근순"),false));    
    //box_order->addWidget(createFilterBtn("OrderBy", 0, tr("A-Z"),true));
    //box_order->addWidget(createFilterBtn(tr("이름순"),false));
    //box_order->addWidget(createFilterBtn("OrderBy", 1, tr("Artist"),false));
    //box_order->addWidget(createFilterBtn(tr("아티스트순"),false));
    //box_order->addWidget(createFilterBtn("OrderBy", 2, tr("Favorite"),false));
    //box_order->addWidget(createFilterBtn(tr("선호도순"),true));

    QHBoxLayout *box_body = new QHBoxLayout;
    box_body->setContentsMargins(20,20,20,20);
    box_body->setSpacing(0);
    box_body->addLayout(box_fileType);
    box_body->addLayout(box_bitDepth);
    box_body->addLayout(box_sampling);    
    box_body->addLayout(box_star);
    box_body->addLayout(box_order);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setContentsMargins(0,0,0,0);
    box_total->setSpacing(0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addLayout(box_header);
    box_total->addWidget(line);
    box_total->addLayout(box_body);

    widget_filter = new QWidget(this);
    widget_filter->setObjectName("widget_filter");
    widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
    widget_filter->setFixedSize(700,360);
    widget_filter->move(70,10);
    widget_filter->setContentsMargins(1,1,1,1);
    widget_filter->setLayout(box_total);

    widget_filter->hide();
}

/**
 * @brief MusicCategory::createFilterLabel : Filter 선택 UI의 필터 라벨 반환
 * @param p_label QString 라벨명
 * @return QLabel*
 */
QLabel* MusicCategory::createFilterLabel(const QString &p_label){
    QLabel *lb_filter = new QLabel;
    lb_filter->setText(p_label);
    lb_filter->setFixedHeight(48);
    lb_filter->setStyleSheet("color:#CCCCCC;font-size:14px;background-color:transparent;");
    lb_filter->setAlignment(Qt::AlignCenter);

    return lb_filter;
}

/**
 * @brief MusicCategory::createFilterBtn : Filter 선택 UI의 필터 버튼 반환
 * @param p_text QString 버튼명
 * @param flagSelected bool if true selected 상태 지정
 * @return QPushButton*
 */
QPushButton* MusicCategory::createFilterBtn(const QString p_filter, const int idx, const QString &p_text, bool flagSelected){

    QPushButton *btn = new QPushButton;
    btn->setText(p_text);
    btn->setFixedSize(QSize(110,30));
    btn->setStyleSheet(flagSelected ? btnStyle_selected : btnStyle_normal);
    btn->setProperty(KEY_FILTER, p_filter);
    btn->setProperty(KEY_IDX, idx);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, SIGNAL(clicked()), this, SLOT(slot_changedBtnFilter()));

    return btn;
}

/**
 * @brief MusicCategory::setUISortType 해당 유형에 맞는 StackWidget 변경
 * @param p_type
 */
void MusicCategory::setUISortType(QString p_type){

    this->contentStep = p_type;

    if(this->flag_file_type == true){
        this->lb_file_type->setText("");
        this->lb_file_type->setStyleSheet("border:0px solid #707070;border-radius:15px;");
        this->flag_file_type = false;

        hl_filter->removeWidget(this->lb_file_type);
    }
    if(this->flag_bit_depth == true){
        this->lb_bit_depth->setText("");
        this->lb_bit_depth->setStyleSheet("border:0px solid #707070;border-radius:15px;");
        this->flag_bit_depth = false;

        hl_filter->removeWidget(this->lb_bit_depth);
    }
    if(this->flag_sample_rate == true){
        this->lb_sample_rate->setText("");
        this->lb_sample_rate->setStyleSheet("border:0px solid #707070;border-radius:15px;");
        this->flag_sample_rate = false;

        hl_filter->removeWidget(this->lb_sample_rate);
    }
    if(this->flag_star_point == true){
        this->lb_star_point->setText("");
        this->lb_star_point->setStyleSheet("border:0px solid #707070;border-radius:15px;");
        this->flag_star_point = false;

        hl_filter->removeWidget(this->lb_star_point);
    }
    if(this->flag_order == true){
        this->lb_order->setText("");
        this->lb_order->setStyleSheet("border:0px solid #707070;border-radius:15px;");
        this->flag_order = false;

        hl_filter->removeWidget(this->lb_order);
    }

    if(p_type==SORT_STEP_AUDIO){
        if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt();
            this->lb_file_type->setText(fileTypeList.at(tmp_idx));
            this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_file_type = true;

            hl_filter->addWidget(this->lb_file_type);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt();
            this->lb_bit_depth->setText(bitDepthList.at(tmp_idx));
            this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_bit_depth = true;

            hl_filter->addWidget(this->lb_bit_depth);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt();
            this->lb_sample_rate->setText(samplingList.at(tmp_idx));
            this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_sample_rate = true;

            hl_filter->addWidget(this->lb_sample_rate);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt();
            this->lb_star_point->setText(starList.at(tmp_idx));
            this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_star_point = true;

            hl_filter->addWidget(this->lb_star_point);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() > 0){
            int tmp_idxOrder = settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt();
            this->lb_order->setText(orderList.at(tmp_idxOrder));
            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_order = true;

            hl_filter->addWidget(this->lb_order);
            //hl_filter->addSpacing(10);
        }
    }

    // 버튼 스타일 초기화
    this->btn_filter->hide();
    this->btn_sort_audio->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_sort_audio").arg(tmp_btnStyle).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
    this->btn_sort_kind->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_sort_kind").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
    this->btn_sort_artist->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_sort_artist").arg(tmp_btnStyle).arg(tmp_btnStyleHover));
    this->btn_sort_composer->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_sort_composer").arg(tmp_btnStyle).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));

    if(p_type==SORT_STEP_AUDIO){
        this->btn_filter->show();
        this->btn_sort_audio->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_sort_audio").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
        this->musicList_audio->setDataFromDB();
        this->stackedwidget->setCurrentWidget(this->musicList_audio);
    }else if(p_type==SORT_STEP_KIND){
        this->btn_sort_kind->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_sort_kind").arg(tmp_btnStyle + tmp_btnStyleHover).arg(tmp_btnStyleHover));
        this->musicList_kind->setDataFromDB();
        this->stackedwidget->setCurrentWidget(this->musicList_kind);
    }else if(p_type==SORT_STEP_ARTIST){
        this->btn_sort_artist->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_sort_artist").arg(tmp_btnStyle + tmp_btnStyleHover).arg(tmp_btnStyleHover));
        this->musicList_artist->setDataFromDB();
        this->stackedwidget->setCurrentWidget(this->musicList_artist);
    }else if(p_type==SORT_STEP_COMPOSER){
        this->btn_sort_composer->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_sort_composer").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-right-radius:20px;border-bottom-right-radius:20px;").arg(tmp_btnStyleHover));
        this->musicList_composer->setDataFromDB();
        this->stackedwidget->setCurrentWidget(this->musicList_composer);
    }
}


/**
 * @brief MusicCategory::changedSubTab [SLOT][override]
 * @param p_data
 */
void MusicCategory::slot_changedSubTabUI(const QJsonObject &p_data){
    Q_UNUSED(p_data);
}
/**
 * @brief MusicCategory::changedSubTab [SLOT]
 */
void MusicCategory::slot_changedSubTabUI(){

    QString tmp_contentStep = this->sender()->property(KEY_CONTENT_STEP.toStdString().c_str()).toString();
    this->setUISortType(tmp_contentStep);

    QJsonObject tmp_data = this->getDataJson();
    tmp_data[KEY_CONTENT_STEP] = tmp_contentStep;

    emit clickedSubTab(tmp_data);
}

/**
 * @brief MusicCategory::setData [override]
 * @param p_data
 */
void MusicCategory::setDataABSPage(QJsonObject p_data){

    // 공통 데이터 세팅
    AbstractPage::setDataABSPage(p_data);


    // step 있는경우
    if(this->contentStep==""){
        this->contentStep = SORT_STEP_AUDIO;
        this->setUISortType(SORT_STEP_AUDIO);
    }else{
        this->setUISortType(this->contentStep);
    }
}


/**
 * @brief MusicCategory::getDataJson [override]
 * @return
 */
QJsonObject MusicCategory::getDataJson(){

    // 공통 데이터 반환
    QJsonObject tmp_data = AbstractPage::getDataJson();

    return tmp_data;
}

/**
 * @brief MusicCategory::slot_clickedBtnFilter:[슬롯] 필터 버튼 클릭
 */
void MusicCategory::slot_clickedBtnFilter(){

    if(this->contentStep==SORT_STEP_AUDIO){

        widget_filter->setVisible(!widget_filter->isVisible());

        if(widget_filter->isVisible() == false){
            if(this->flag_file_type == true){
                this->lb_file_type->setText("");
                this->lb_file_type->setStyleSheet("border:0px solid #707070;border-radius:15px;");
                this->flag_file_type = false;

                hl_filter->removeWidget(this->lb_file_type);
            }
            if(this->flag_bit_depth == true){
                this->lb_bit_depth->setText("");
                this->lb_bit_depth->setStyleSheet("border:0px solid #707070;border-radius:15px;");
                this->flag_bit_depth = false;

                hl_filter->removeWidget(this->lb_bit_depth);
            }
            if(this->flag_sample_rate == true){
                this->lb_sample_rate->setText("");
                this->lb_sample_rate->setStyleSheet("border:0px solid #707070;border-radius:15px;");
                this->flag_sample_rate = false;

                hl_filter->removeWidget(this->lb_sample_rate);
            }
            if(this->flag_star_point == true){
                this->lb_star_point->setText("");
                this->lb_star_point->setStyleSheet("border:0px solid #707070;border-radius:15px;");
                this->flag_star_point = false;

                hl_filter->removeWidget(this->lb_star_point);
            }
            if(this->flag_order == true){
                this->lb_order->setText("");
                this->lb_order->setStyleSheet("border:0px solid #707070;border-radius:15px;");
                this->flag_order = false;

                hl_filter->removeWidget(this->lb_order);
            }

            if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt();
                this->lb_file_type->setText(fileTypeList.at(tmp_idx));
                this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
                this->flag_file_type = true;

                hl_filter->addWidget(this->lb_file_type);
                //hl_filter->addSpacing(10);
            }

            if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt();
                this->lb_bit_depth->setText(bitDepthList.at(tmp_idx));
                this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
                this->flag_bit_depth = true;

                hl_filter->addWidget(this->lb_bit_depth);
                //hl_filter->addSpacing(10);
            }

            if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt();
                this->lb_sample_rate->setText(samplingList.at(tmp_idx));
                this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
                this->flag_sample_rate = true;

                hl_filter->addWidget(this->lb_sample_rate);
                //hl_filter->addSpacing(10);
            }

            if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                int tmp_idx = settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt();
                this->lb_star_point->setText(starList.at(tmp_idx));
                this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
                this->flag_star_point = true;

                hl_filter->addWidget(this->lb_star_point);
                //hl_filter->addSpacing(10);
            }

            if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() > 0){
                int tmp_idxOrder = settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt();
                this->lb_order->setText(orderList.at(tmp_idxOrder));
                this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
                this->flag_order = true;

                hl_filter->addWidget(this->lb_order);
                //hl_filter->addSpacing(10);
            }
        }

        this->btn_sort_audio->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_sort_audio").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
        this->musicList_audio->setDataFromDB();
        this->stackedwidget->setCurrentWidget(this->musicList_audio);
    }else if(this->contentStep==SORT_STEP_KIND || this->contentStep==SORT_STEP_ARTIST || this->contentStep==SORT_STEP_COMPOSER){
        return;
    }
}

void MusicCategory::slot_changedBtnFilter(){

    int i = 0;

    this->selected_optFilter = sender()->property(KEY_FILTER);
    this->selected_optIdx = sender()->property(KEY_IDX);

    if(this->selected_optFilter == "FileType"){
        if(this->selected_optIdx == 0){
            this->btn_file_type[0]->setStyleSheet(btnStyle_selected);
            this->btn_file_type[1]->setStyleSheet(btnStyle_normal);
        }
        else if(this->selected_optIdx == 1){
            this->btn_file_type[0]->setStyleSheet(btnStyle_normal);
            this->btn_file_type[1]->setStyleSheet(btnStyle_selected);
        }

        settings->setValue(rosesettings.SETTINGS_FILTER_FILETYPE, this->selected_optIdx);
    }
    else if(this->selected_optFilter == "BitDepth"){
        if(this->selected_optIdx == 0){
            this->btn_bit_depth[0]->setStyleSheet(btnStyle_selected);
            this->btn_bit_depth[1]->setStyleSheet(btnStyle_normal);
        }
        else if(this->selected_optIdx == 1){
            this->btn_bit_depth[0]->setStyleSheet(btnStyle_normal);
            this->btn_bit_depth[1]->setStyleSheet(btnStyle_selected);
        }

        settings->setValue(rosesettings.SETTINGS_FILTER_BIT, this->selected_optIdx);
    }
    else if(this->selected_optFilter == "Sampling"){
        for(i = 0; i < 3; i++){
            if(i == this->selected_optIdx){
                this->btn_sample_rate[i]->setStyleSheet(btnStyle_selected);
            }
            else{
                this->btn_sample_rate[i]->setStyleSheet(btnStyle_normal);
            }
        }

        settings->setValue(rosesettings.SETTINGS_FILTER_SAMPLEING, this->selected_optIdx);
    }
    else if(this->selected_optFilter == "Star"){
        for(i = 0; i < 6; i++){
            if(i == this->selected_optIdx){
                this->btn_star_point[i]->setStyleSheet(btnStyle_selected);
            }
            else{
                this->btn_star_point[i]->setStyleSheet(btnStyle_normal);
            }
        }

        settings->setValue(rosesettings.SETTINGS_FILTER_STAR, this->selected_optIdx);
    }
    else if(this->selected_optFilter == "OrderBy"){
        for(i = 0; i < 4; i++){
            if(i == this->selected_optIdx){
                this->btn_order[i]->setStyleSheet(btnStyle_selected);
            }
            else{
                this->btn_order[i]->setStyleSheet(btnStyle_normal);
            }

            settings->setValue(rosesettings.SETTINGS_FILTER_ORDER, this->selected_optIdx);
        }
    }

}
