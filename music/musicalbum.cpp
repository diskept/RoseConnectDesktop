#include "musicalbum.h"
#include "widget/toastmsg.h"

#include <QThread>//cheon210608-test
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QPainter>
#include <QWheelEvent>
#include <QScrollBar>

#include <QCoreApplication>
#include <QIntValidator>//cheon210717-search
#include <common/gscommon.h>
#include <common/global.h>
#include <common/sqlitehelper.h>
#include <common/rosesettings.h>
#include <QJsonDocument>
#include <widget/flowlayout.h>
#include <common/networkhttp.h>
#include <QtWidgets/qabstractslider.h>

//binarycreator.exe -c config\config.xml -p packages Rose_Connect_setup_4.1.7.4.exe

const char* KEY_AFILTER = "filter";
const char* KEY_AIDX = "idx";
int PAGE_LMT = 100;//22040301
const int LEFT_WIDTH = 240;//158;//125+33
const int ALBUMWIDTH = 204;//186+18


/**
 * @brief MusicAlbum::MusicAlbum : 음악-앨범
 * @details 음악을 앨범으로 분류해 화면에 그린다.
 * @param parent QWidget
 */
MusicAlbum::MusicAlbum(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

MusicAlbum::~MusicAlbum(){
    delete settings;
    //this->deleteLater();
}

/**
 * @brief MusicAlbum::setInit : 초기 세팅
 */
void MusicAlbum::setInit(){
    global.LmtCnt_H = global.LmtCnt_H - 320;
    if(global.LmtCnt < 1460){
        global.LmtCnt = global.LmtCnt +180;
    }
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
void MusicAlbum::setUIControl(){//c220323
    // 상단 필터
    QPushButton *btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
    btn_filter->setFixedSize(80,50);
    btn_filter->setCursor(Qt::PointingHandCursor);

    this->hl_filter = new QHBoxLayout();
    this->hl_filter->setContentsMargins(0,0,0,0);
    this->hl_filter->setSpacing(0);
    this->hl_filter->setAlignment(Qt::AlignLeft);
    //this->hl_filter->addWidget(btn_filter);
    //this->hl_filter->setSpacing(10);


    QPushButton *btn_ser = new QPushButton(tr("Alphabet"),this);
    btn_ser->setFixedSize(140,50);
    QPixmap pixmap_btn_ser(":/images/icon_ser_w.png");
    btn_ser->setCursor(Qt::PointingHandCursor);
    btn_ser->setIcon(pixmap_btn_ser);
    btn_ser->setIconSize(pixmap_btn_ser.rect().size());
    btn_ser->setStyleSheet("color:#FFFFFF;font-size:15px;");


/*
    QPushButton *btn_ser = GSCommon::getUIBtnImg("btn_ch_ser",":/images/icon_ser_w.png");
    btn_ser->setFixedSize(180,50);
    btn_ser->setCursor(Qt::PointingHandCursor);
    btn_ser->setText(tr("First Letter"));
    btn_ser->setStyleSheet("color:#FFFFFF;font-size:15px;");
*/

    /*settings->setValue(rosesettings.SETTINGS_AFILTER_FILETYPE, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_AFILTER_BIT, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_AFILTER_SAMPLEING, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_AFILTER_ORDER, QVariant(0));
    settings->setValue(rosesettings.SETTINGS_AFILTER_STAR, QVariant(0));*/

    this->lb_file_type = new QLabel;
    this->lb_file_type->setContentsMargins(20,0,20,0);
    this->lb_file_type->setFixedHeight(30);
    this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
    this->lb_file_type->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() > 0){
        this->before_file_type = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt();
        this->lb_file_type->setText(fileTypeList.at(this->before_file_type));

        hl_filter->addWidget(this->lb_file_type);
        //hl_filter->addSpacing(10);

        this->flag_file_type = true;
    }

    this->lb_bit_depth = new QLabel;
    this->lb_bit_depth->setContentsMargins(20,0,20,0);
    this->lb_bit_depth->setFixedHeight(30);
    this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
    this->lb_bit_depth->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() > 0){
        this->before_bit_depth = settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt();
        this->lb_bit_depth->setText(bitDepthList.at(this->before_bit_depth));

        hl_filter->addWidget(this->lb_bit_depth);
        //hl_filter->addSpacing(10);

        this->flag_bit_depth = true;
    }

    this->lb_sample_rate = new QLabel;
    this->lb_sample_rate->setContentsMargins(20,0,20,0);
    this->lb_sample_rate->setFixedHeight(30);
    this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
    this->lb_sample_rate->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() > 0){
        this->before_sample_rate = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt();
        this->lb_sample_rate->setText(samplingList.at(this->before_sample_rate));

        hl_filter->addWidget(this->lb_sample_rate);
        //hl_filter->addSpacing(10);

        this->flag_sample_rate = true;
    }

    this->lb_star_point = new QLabel;
    this->lb_star_point->setContentsMargins(20,0,20,0);
    this->lb_star_point->setFixedHeight(30);
    this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
    this->lb_star_point->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
        this->before_star_point = settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt();
        this->lb_star_point->setText(starList.at(this->before_star_point));

        hl_filter->addWidget(this->lb_star_point);
        //hl_filter->addSpacing(10);

        this->flag_star_point = true;
    }

    this->lb_order = new QLabel;
    this->lb_order->setContentsMargins(20,0,20,0);
    this->lb_order->setFixedHeight(30);
    this->lb_order->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
    this->lb_order->setAlignment(Qt::AlignCenter);

    if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
        this->before_order = settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt();
        this->lb_order->setText(orderList.at(this->before_order));

        hl_filter->addWidget(this->lb_order);
        //hl_filter->addSpacing(10);

        this->flag_order = true;
    }



    widget_btnFilter = new QWidget();
    widget_btnFilter->setObjectName("widget_btnFilter");
    widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
    widget_btnFilter->setFixedHeight(50);
    widget_btnFilter->setLayout(hl_filter);
    //widget_btnFilter->hide();

    setUIlistBtnSer_boxControl();

    QHBoxLayout *hl_btn_Ser = new QHBoxLayout();//c220415
    hl_btn_Ser->addWidget(btn_ser,0, Qt::AlignCenter|Qt::AlignLeft);
    hl_btn_Ser->addWidget(btn_serClear,0, Qt::AlignVCenter|Qt::AlignLeft);
    hl_btn_Ser->addWidget(widget_total_ser,0, Qt::AlignVCenter|Qt::AlignLeft);

    QWidget *widget_total_ser2 = new QWidget();//c220415
    widget_total_ser2->setObjectName("widget_total_ser");//c220415
    widget_total_ser2->setStyleSheet("#widget_total_ser { background-color:#171717; }");//c220415
    widget_total_ser2->setLayout(hl_btn_Ser);//c220415



    QHBoxLayout *hl_btnFilter_Ser = new QHBoxLayout();//c220415
    hl_btnFilter_Ser->setSpacing(10);
    hl_btnFilter_Ser->setContentsMargins(30,0,20,0);
    hl_btnFilter_Ser->addWidget(btn_filter,0, Qt::AlignVCenter|Qt::AlignLeft);
    hl_btnFilter_Ser->addWidget(widget_btnFilter,0, Qt::AlignVCenter|Qt::AlignLeft);
    hl_btnFilter_Ser->addWidget(widget_total_ser2,1, Qt::AlignVCenter|Qt::AlignLeft);


    QWidget *widget_filter_ser = new QWidget();//cheon210527
    widget_filter_ser->setObjectName("widget_filter_ser");//cheon210527
    widget_filter_ser->setStyleSheet("#widget_filter_ser { background-color:#171717; }");//cheon210527
    widget_filter_ser->setLayout(hl_btnFilter_Ser);//cheon210527



    connect(btn_filter, SIGNAL(clicked()), SLOT(slot_clickedBtnFilter()));
    connect(btn_ser, SIGNAL(clicked()), SLOT(slot_clickedBtnSer()));



    albumtCnt = new QLabel;
    albumtCnt->setText(QString("Total : %1").arg(1));
    albumtCnt->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");

    albumPage = new QLabel;
    albumPage->setText(QString("Page : %1 ~ %2").arg(1).arg(1));
    albumPage->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");

    this->slider_playbar = new sectionbottomSlider(Qt::Horizontal);
    this->slider_playbar->setObjectName("slider_vol");
    this->slider_playbar->setCursor(Qt::PointingHandCursor);

    //this->slider_playbar->setSingleStep(1);     // 1초 = 1000    // arrow-key step-size
    //this->slider_playbar->setPageStep(1);                // mouse-wheel/page-key step-size
    this->slider_playbar->setFixedWidth(150);
    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background:#333333;width:4px; } QSlider::handle:horizontal { background:white;height:16px;border-radius:8px;margin:0 -6px; } QSlider::add-page:horizontal { background:#CCCCCC; } QSlider::sub-page:horizontal { background:#333333; } ");
    this->slider_playbar->setStyleSheet("QSlider::groove:horizontal {"
        "border-radius: 1px;"
        "height: 5px;"
        "margin: 0px;"
        "background-color: #555555;}"// rgb(52, 59, 172);}
    "QSlider::groove:horizontal:hover {"
        "background-color: rgb(55, 62, 76);"
    "}"
    "QSlider::handle:horizontal {"
        "background-color: #CCCCCC;"//rgb(85, 170, 255);
        "border: none;"
        "height: 16px;"
        "width: 16px;"
        "margin: -8px 0;"
        "border-radius: 8px;"
        "padding: -8px 0px;"
    "}"
    "QSlider::handle:horizontal:hover {"
        "background-color: rgb(155, 180, 255);"
    "}"
    "QSlider::handle:horizontal:pressed {"
        "background-color: rgb(65, 255, 195);"
    "}");
    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:4px;} QSlider::handle:horizontal {  background:white;width:15px;border-radius:8px;margin:0 -6py; } QSlider::add-page:horizontal { background:#aaaaaa; } QSlider::sub-page:horizontal { background:#B18658; } ");
    this->slider_playbar->setMaximum(1);
    this->slider_playbar->setMinimum(1);

    QLabel *le_page_search = new QLabel;//cheon210527
    le_page_search->setText(QString("goto Page"));//cheon210527
    le_page_search->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");//cheon210527
    le_page_search->setFixedSize(100,30);//cheon210527

    this->le_lisearch = new QLineEdit();//cheon210717-search
    //this->le_lisearch->setValidator( new QIntValidator(0, 1000, this) );//cheon210717-search
    this->le_lisearch->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
   // this->le_lisearch = new QTextEdit();//cheon210527
    this->le_lisearch->setObjectName("le_search");//cheon210527
    this->le_lisearch->setStyleSheet("#le_search { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:5px;}");//cheon210527
    this->le_lisearch->setMaxLength(5);//cheon210527
    this->le_lisearch->setFixedSize(100,30);//cheon210527
    this->le_lisearch->setTextMargins(1,1,1,1);
    this->le_lisearch->setText("1");

    QPushButton *btn_ok = new QPushButton;
    btn_ok->setText(tr("go"));//btn_ok->setText(tr("추가"));
    btn_ok->setCursor(Qt::PointingHandCursor);
    btn_ok->setStyleSheet("font-size:15px;font:bold; color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
    btn_ok->setFixedSize(70,30);

    QHBoxLayout *search_hBox = new QHBoxLayout();//cheon210527
    search_hBox->setSpacing(10);//cheon210527
    search_hBox->setContentsMargins(0,0,0,0);//cheon210527
   // search_hBox->setAlignment(Qt::AlignTop);//cheon210527
    search_hBox->addWidget(le_page_search,1,  Qt::AlignRight);//c220414
    search_hBox->addWidget(this->slider_playbar);
    search_hBox->addWidget(this->le_lisearch);//cheon210527
    search_hBox->addWidget(btn_ok);



    QWidget *widget_search = new QWidget();//cheon210527
    widget_search->setObjectName("widget_search");//cheon210527
    widget_search->setFixedHeight(30);//cheon210527
    widget_search->setFixedWidth(450);//cheon210527
    widget_search->setStyleSheet("#widget_search { background-color:#171717; }");//cheon210527
    widget_search->setLayout(search_hBox);//cheon210527

    QHBoxLayout *tmp2_hBox = new QHBoxLayout();//cheon210527
    tmp2_hBox->setSpacing(10);//cheon210527
    tmp2_hBox->setContentsMargins(30,0,20,0);//cheon210527
    tmp2_hBox->addWidget(widget_search,1, Qt::AlignVCenter);//cheon210527
    //tmp2_hBox->setSpacing(10);
    //tmp2_hBox->addWidget(btn_ok, 1, Qt::AlignVCenter | Qt::AlignLeft);

    QWidget *widget_search_last = new QWidget();//cheon210527
    widget_search_last->setObjectName("widget_search_last");//cheon210527
    widget_search_last->setStyleSheet("#widget_search_last { background-color:#171717; }");//cheon210527
    widget_search_last->setLayout(tmp2_hBox);//cheon210527

    //this->slider_playbar->disconnect();//5555
    //print_debug();
    this->slider_playbar->setValue(0);
    //connect(this->slider_playbar, SIGNAL(signal_sliderclick(int)), this, SLOT(slot_sliderClick(int)));//c1209

    //connect(this->slider_playbar, SIGNAL(sliderReleased()) ,this, SLOT(changedSliderBar()));
    //connect(this->slider_playbar, SIGNAL(sliderPressed()) ,this, SLOT(changedSliderBar()));

    //--start-----------------------------------------------------------------

    le_lisearch_str = new QLineEdit();//cheon210717-search
    //this->le_lisearch->setValidator( new QIntValidator(0, 1000, this) );//cheon210717-search
    //le_lisearch_str->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
    le_lisearch_str->setObjectName("le_lisearch_str");//cheon210527
    le_lisearch_str->setStyleSheet("#le_lisearch_str { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:5px;}");//cheon210527
    le_lisearch_str->setMaxLength(32);//cheon210527
    le_lisearch_str->setFixedSize(300,30);//cheon210527
    le_lisearch_str->setTextMargins(1,1,1,1);
    le_lisearch_str->setText("");

    QLabel *le_str_search = new QLabel;//cheon210527
    le_str_search->setText(QString("Album Search"));//cheon210527
    le_str_search->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");//cheon210527
    le_str_search->setFixedSize(100,30);//cheon210527

    QPushButton *btn_ok_str = new QPushButton;
    btn_ok_str->setText(tr("ok"));//btn_ok->setText(tr("추가"));
    btn_ok_str->setCursor(Qt::PointingHandCursor);
    btn_ok_str->setStyleSheet("font-size:15px;font:bold; color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
    btn_ok_str->setFixedSize(70,30);

    QHBoxLayout *search_str_hBox = new QHBoxLayout();//cheon210527
    search_str_hBox->setSpacing(0);//cheon210527
    search_str_hBox->setContentsMargins(0,0,0,0);//cheon210527
   // search_hBox->setAlignment(Qt::AlignTop);//cheon210527
    search_str_hBox->addWidget(le_str_search);//cheon210527
    search_str_hBox->setSpacing(10);//cheon210527
    search_str_hBox->addWidget(this->le_lisearch_str);//cheon210527
    search_str_hBox->addWidget(btn_ok_str);



    QWidget *widget_search_str = new QWidget();//cheon210527
    widget_search_str->setObjectName("search_str_hBox");//cheon210527
    widget_search_str->setStyleSheet("#search_str_hBox { background-color:#171717; }");//cheon210527
    widget_search_str->setLayout(search_str_hBox);//cheon210527
    widget_search_str->setFixedWidth(490);

    //-end------------------------------------------------------------------


    QHBoxLayout *tmp_hBox = new QHBoxLayout();
    tmp_hBox->setSpacing(0);
    tmp_hBox->setContentsMargins(40,0,20,0);
   // tmp_hBox->setAlignment(Qt::AlignTop);

    tmp_hBox->addWidget(albumtCnt,0, Qt::AlignVCenter);
    tmp_hBox->addWidget(albumPage,0, Qt::AlignVCenter);
    tmp_hBox->setSpacing(20);
    //tmp_hBox->addWidget(widget_btnSer, 0, Qt::AlignVCenter|Qt::AlignLeft );
    //tmp_hBox->addWidget(widget_search_last, 0, Qt::AlignVCenter|Qt::AlignLeft );//cheon210527
    tmp_hBox->addWidget(widget_search_str, 0, Qt::AlignVCenter|Qt::AlignLeft );//cheon210527

    QWidget *line_bottom_1 = new QWidget();
    line_bottom_1->setFixedHeight(1);
    line_bottom_1->setStyleSheet("background-color:#505050;");


    QWidget *widget_tt = new QWidget();
    widget_tt->setObjectName("widget_tt");
    widget_tt->setStyleSheet("#widget_tt { background-color:#171717; }");
    widget_tt->setFixedHeight(40);
    widget_tt->setLayout(tmp_hBox);

    flowLayout = new FlowLayout(this,0,18,20);

    // 노래 레이아웃
    this->widget_scrollArea = new QWidget();
    this->widget_scrollArea->setContentsMargins(33,0,18,20);//10);
    this->widget_scrollArea->setLayout(flowLayout);

    this->scrollArea = new QScrollArea();
    this->scrollArea->setObjectName("scrollArea");
    this->scrollArea->setWidget(widget_scrollArea);
    this->scrollArea->setWidgetResizable(true);
    this->scrollArea->setStyleSheet("#scrollArea { border:0px; }"); //background-color:#171717; }");
    this->scrollArea->verticalScrollBar()->
            setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                             "   width: 8px;"
                                             "   background: rgba (0,0,0,0%);"
                                             "   margin: 0px, 0px, 0px, 0px;"
                                             "   padding-top: 9px;"//set aside 9px for the arrows above and below
                                             "   padding-bottom: 9px;"
                                            "}"
                                            "QScrollBar :: handle: vertical"
                                            "{"
                                             "   width:8px;"
                                             "   background: rgba (0,0,0,25%);"
                                             "   border-radius: 4px;"//Both ends of the scroll bar become ellipses
                                             "   min-height: 20 px;"
                                           "}"
                                           " QScrollBar :: handle: vertical: hover"
                                           " {"
                                           "     width: 8px;"
                                           "     background: rgba (0,0,0,50%);"//When the mouse is placed on the scroll bar, the color becomes darker
                                           "     border-radius: 4px;"
                                           "     min-height: 20 px;"
                                           " }"));

            /*
            setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                            "    border: 2px solid #aaaaaa;"
                                            "    background:#777777;"
                                            "    width:15px;  "
                                            "    margin: 0px 0px 0px 0px;"
                                            "}"
                                            "QScrollBar::handle:vertical {"
                                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                            "    stop: 0 #999999, stop: 0.5 #171717, stop:1 #999999);"
                                            "    min-height: 50px;"
                                            "}"
                                            "QScrollBar::add-line:vertical {"
                                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                            "    stop: 0 #171717, stop: 0.5 #171717,  stop:1 #171717);"
                                            "    height: 0px;"
                                            "    subcontrol-position: bottom;"
                                            "    subcontrol-origin: margin;"
                                            "}"
                                            "QScrollBar::sub-line:vertical {"
                                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                            "    stop: 0  #171717, stop: 0.5 #171717,  stop:1 #171717);"
                                            "    height: 0 px;"
                                            "    subcontrol-position: top;"
                                            "    subcontrol-origin: margin;"
                                            "}"
                                            ));*/
    //this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    this->scrollArea->verticalScrollBar()->setSingleStep(92);
    //this->scrollArea->verticalScrollBar()->setPageStep(90*10);
/*
    QVBoxLayout *vl_total44= new QVBoxLayout();
    vl_total44->setContentsMargins(0,0,0,0);//c220414
    vl_total44->addWidget(this->scrollArea);

    line_bottom_33 = new QWidget();
    line_bottom_33->setLayout(vl_total44);
*/
    this->vl_total= new QVBoxLayout();
    this->vl_total->setContentsMargins(0,0,20,0);//c220414
    this->vl_total->setSpacing(0);
    this->vl_total->setAlignment(Qt::AlignTop);
    //this->vl_total->addWidget(widget_btnFilter);//c220415
    this->vl_total->addWidget(widget_filter_ser);//c220415


    this->vl_total->addWidget(widget_tt);
    this->vl_total->addWidget(line_bottom_1);
    this->vl_total->addSpacing(20);
    this->vl_total->addWidget(this->scrollArea);

    setLayout(this->vl_total);

    connect(this->scrollArea->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));
    connect(this->scrollArea->verticalScrollBar(), SIGNAL(sliderPressed()), this, SLOT(slot_sliderPressed()));
    //connect(this->scrollArea, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderChange()));

    // connect

    // FIlter 선택 UI 세팅
    setFIlterUIControl();
    total_db_cnt = getAlbumDataFromDBforTotalCount();
    connect(this->le_lisearch, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed()));//cheon210717-search
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedChange()));//cheon210717-search

    connect(this->le_lisearch_str, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed_str()));//cheon210717-search
    connect(btn_ok_str, SIGNAL(clicked()), this, SLOT(slot_clickedChange_str()));



    linker = Linker::getInstance();

    connect(this->slider_playbar, SIGNAL(valueChanged(int)) ,this, SLOT(slot_changedSliderBar(int)));
    connect(linker, SIGNAL(signal_completeDownloadDB()), this, SLOT(slot_returnPressed()));//c220402
    //connect(linker, SIGNAL(signal_window_resize()), this, SLOT(slot_sliderbarValue()));


}

void MusicAlbum::slot_clickedBtnSer(){//c220415
        print_debug();
        btn_serClear->setVisible(!btn_serClear->isVisible());
        widget_total_ser->setVisible(!widget_total_ser->isVisible());

        if(!btn_serClear->isVisible()){
            QTimer::singleShot(500, this, SLOT(slot_clearButton_setStyleSheet()));
        }


}

void MusicAlbum::slot_clicked_search_eng_ch(){//c220415
    print_debug();

    for(int i = 0; i < engList.size(); i++){
        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

    }
    for(int i = 0; i < hanList.size(); i++){
        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

    }
    btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
    ClickableLabel *tmp_btn = dynamic_cast<ClickableLabel*>(sender());
    tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    int chIndedx = tmp_btn->property("search_chIndex").toInt();
    qDebug() << "engList.at(chIndedx) = " << engList.at(chIndedx);
    setDataFromDB_Alpa(engList.at(chIndedx));


}
void MusicAlbum::slot_clicked_search_han_ch(){//c220415
    print_debug();

    for(int i = 0; i < engList.size(); i++){
        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

    }
    for(int i = 0; i < hanList.size(); i++){
        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

    }
    btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
    ClickableLabel *tmp_btn = dynamic_cast<ClickableLabel*>(sender());
    tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    int chIndedx = tmp_btn->property("search_chIndex").toInt();
    qDebug() << "engList.at(chIndedx) = " << hanList.at(chIndedx);
    setDataFromDB_Alpa(hanList.at(chIndedx));


}

void MusicAlbum::slot_clearButton_setStyleSheet(){
    if(!strArg_1.isEmpty()){
        btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
        setDataFromDB_Alpa("");

    }else{
        btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
    }
}

void MusicAlbum::slot_clicked_search_eng_ch_clear(){//c220415
    print_debug();
    btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("transparent", "#B18658"));
//search_clearIndex
    for(int i = 0; i < engList.size(); i++){
        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

    }
    for(int i = 0; i < hanList.size(); i++){
        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

    }

     QTimer::singleShot(500, this, SLOT(slot_clearButton_setStyleSheet()));



}

void MusicAlbum::setUIlistBtnSer_boxControl(){//c220415
    hanList << "ㄱ"<<"ㄴ"<<"ㄷ"<<"ㄹ"<<"ㅁ"<<"ㅂ"<<"ㅅ"<<"ㅇ"<<"ㅈ"<<"ㅊ"<<"ㅋ"<<"ㅌ"<<"ㅍ"<<"ㅎ";
    engList << "A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T"<<"U"<<"V"<<"W"<<"X"<<"Y"<<"Z";

    btn_serClear = new ClickableLabel();
    btn_serClear->setContentsMargins(0,0,0,0);
    btn_serClear->setFixedSize(50,20);
    btn_serClear->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:12px;border:1px solid #707070;border-radius:20px;");
    btn_serClear->setAlignment(Qt::AlignLeft);
    btn_serClear->setText("Clear");
    btn_serClear->setCursor(Qt::PointingHandCursor);
    btn_serClear->setProperty("search_clearIndex", -1);

    this->hl_eng_ser = new QHBoxLayout();
    this->hl_eng_ser->setContentsMargins(0,0,0,0);
    this->hl_eng_ser->setSpacing(0);
    this->hl_eng_ser->setAlignment(Qt::AlignLeft);
    //this->hl_ser->addWidget(btn_ser);
    this->hl_eng_ser->setSpacing(10);

    this->hl_han_ser = new QHBoxLayout();
    this->hl_han_ser->setContentsMargins(0,0,0,0);
    this->hl_han_ser->setSpacing(0);
    this->hl_han_ser->setAlignment(Qt::AlignLeft);
    //this->hl_ser->addWidget(btn_ser);
    this->hl_han_ser->setSpacing(10);

    QWidget *widget_btnSer_eng = new QWidget();
    widget_btnSer_eng->setObjectName("widget_btnSer_eng");
    widget_btnSer_eng->setStyleSheet("#widget_btnSer_eng { background-color:#171717; }");
    widget_btnSer_eng->setFixedHeight(30);
    widget_btnSer_eng->setLayout(hl_eng_ser);

    QWidget *widget_btnSer_han = new QWidget();
    widget_btnSer_han->setObjectName("widget_btnSer_han");
    widget_btnSer_han->setStyleSheet("#widget_btnSer_han { background-color:#171717; }");
    widget_btnSer_han->setFixedHeight(30);
    widget_btnSer_han->setLayout(hl_han_ser);

    QVBoxLayout *vl_total_ser= new QVBoxLayout();
    vl_total_ser->setContentsMargins(0,0,200,0);//c220414
    vl_total_ser->setSpacing(0);
    //vl_total_ser->setAlignment(Qt::AlignTop);
    vl_total_ser->addWidget(widget_btnSer_eng);//c220415
    vl_total_ser->addWidget(widget_btnSer_han);//c220415

    widget_total_ser = new QWidget();//c220415
    widget_total_ser->setObjectName("widget_total_ser");//c220415
    widget_total_ser->setStyleSheet("#widget_total_ser { background-color:#171717; }");//c220415
    widget_total_ser->setLayout(vl_total_ser);//c220415




    for(int i = 0; i < engList.size(); i++){
        this->lb_eng_type[i] = new ClickableLabel();
        this->lb_eng_type[i]->setContentsMargins(20,0,20,0);
        this->lb_eng_type[i]->setFixedSize(20,20);
        this->lb_eng_type[i]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:12px;border:1px solid #707070;border-radius:20px;");
        this->lb_eng_type[i]->setAlignment(Qt::AlignCenter);
        this->lb_eng_type[i]->setText(engList.at(i));
        this->lb_eng_type[i]->setCursor(Qt::PointingHandCursor);

        //this->lb_han_type[i]->setStyleSheet("background-color:transparent;border-radius:15px;");
        //this->lb_han_type[i]->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);

        this->lb_eng_type[i]->setProperty("search_chIndex", i);
        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

        hl_eng_ser->addWidget(this->lb_eng_type[i]);
        hl_eng_ser->addSpacing(3);

        connect(this->lb_eng_type[i], SIGNAL(signal_clicked()), this, SLOT(slot_clicked_search_eng_ch()));//cheon210717-search

    }
    for(int i = 0; i < hanList.size(); i++){
        this->lb_han_type[i] = new ClickableLabel();
        this->lb_han_type[i]->setContentsMargins(20,0,20,0);
        this->lb_han_type[i]->setFixedSize(20,20);
        this->lb_han_type[i]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:12px;border:1px solid #707070;border-radius:20px;");
        this->lb_han_type[i]->setAlignment(Qt::AlignCenter);
        this->lb_han_type[i]->setText(hanList.at(i));
        this->lb_han_type[i]->setCursor(Qt::PointingHandCursor);

        //this->lb_han_type[i]->setStyleSheet("background-color:transparent;border-radius:15px;");
        //this->lb_han_type[i]->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);

        this->lb_han_type[i]->setProperty("search_chIndex", i);
        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

        hl_han_ser->addWidget(this->lb_han_type[i]);
        hl_han_ser->addSpacing(3);

        connect(this->lb_han_type[i], SIGNAL(signal_clicked()), this, SLOT(slot_clicked_search_han_ch()));//cheon210717-search

    }
    btn_serClear->hide();
    widget_total_ser->hide();
    connect(btn_serClear, SIGNAL(signal_clicked()), this, SLOT(slot_clicked_search_eng_ch_clear()));//cheon210717-search



}

void MusicAlbum::slot_changedSliderBar(int p_value){//c22040301

    //Q_UNUSED(p_value);
    // nothing
    //print_debug();
    qDebug() << "MusicAlbum::slot_changedSliderBar--p_value = " << p_value;
    curr_p_value = p_value;
    this->slider_playbar->setValue(curr_p_value);
    this->le_lisearch->setText(QString("%1").arg(curr_p_value));
}
/**
 * @brief SectionBottom::slot_timePlayValue
 */
void MusicAlbum::slot_sliderbarValue(){
    print_debug();
    //if(!blocking_flag)return;
    //blocking_flag = false;

    qDebug() << "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
    if(!global.device.getDeviceIP().isEmpty()){
        print_debug();
        //scrollArea->verticalScrollBar()->maximum()

        curr_LmtWidth = global.LmtCnt;
        qDebug() << "curr_LmtWidth= " << curr_LmtWidth;
        qDebug() << "pre_LmtWidth= " << pre_LmtWidth;
        qDebug() << "sliderPosition" << this->scrollArea->verticalScrollBar()->sliderPosition();

        if(pre_LmtWidth > 0 && abs(curr_LmtWidth - pre_LmtWidth) >= 5  && curr_page_cnt > 0 && curr_page_cnt <= total_page_cnt){
            print_debug();

            if(curr_page_cnt == 1){
                flag_page_first = true;
                currOffsetPoint = 0;
            }

            slot_sliderValue_flag = true;
            //0717 mtx.lock();
            getAlbumDataFromDB_view_only();
            //0717 mtx.unlock();


        }

        if(scrollArea->verticalScrollBar()->maximum()==this->scrollArea->verticalScrollBar()->sliderPosition()){
            print_debug();
        }else{
            print_debug();
        }
        pre_LmtWidth = global.LmtCnt;
    }
    qDebug() << "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww";
    //blocking_flag = true;

}

/**
 * @brief MusicAlbum::getAlbumDataFromDB : GET 앨범 데이터
 */
void MusicAlbum::getAlbumDataFromDB_pre(){//c220323

    int cnt = 0;
    LMT_CNT = 0;

    cnt = 1;
        LMT_CNT = cnt * PAGE_LMT;
    //}

    this->data = QVariantList();

    pre_lmt_cnt = LMT_CNT;
    if(OffsetPoint_Pre >= LMT_CNT){
        //currOffsetPoint = OffsetPoint;
        if(slot_sliderValue_flag){
            if(mouse_wheel_flag){
                if(OffsetPoint_Pre%LMT_CNT !=0){
                    OffsetPoint_Pre -= (OffsetPoint_Pre%LMT_CNT);
                }
                else OffsetPoint_Pre -= LMT_CNT;
            }
        }
        else {
            if(OffsetPoint_Pre%LMT_CNT !=0){
                OffsetPoint_Pre -= (OffsetPoint_Pre%LMT_CNT);
            }
            else OffsetPoint_Pre -= LMT_CNT;
        }


    }
    else{
        OffsetPoint_Pre = 0;


    }

    print_debug();
    qDebug() << "OffsetPoint_Pre = " << OffsetPoint_Pre;
    qDebug() << "flagReqMore = " << flagReqMore;


    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){

        QString strQuery = "";
        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        //strQuery += " ORDER BY AI.date_added desc ";
        //strQuery += " ORDER BY AI.album asc ";
        QString setWhere = "";
        QString setOrder = "";


        if(!strArg_1.isEmpty()){
            print_debug();
            qDebug() << "strArg_1 = " << strArg_1;

            //strQuery += "(";
            strQuery += searchcharactorSqlWhere();
            //strQuery += ")";

            setWhere = getSetWhereFilter();
            if(!setWhere.isEmpty()){
                strQuery += " AND ";
                strQuery += setWhere;
            }

            strQuery += " ORDER BY AI.album ";
        }
        else{



            setWhere = getSetWhereFilter();

            if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
                    setOrder += " ORDER BY AI.album ";
                }
                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
                    setOrder += " ORDER BY AI.artist ";
                }
                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3){
                    setOrder += " ORDER BY AI.favorite ";
                }
            }

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }
            if(!setOrder.isEmpty()){
                strQuery += setOrder;
            }
            //strQuery += " ORDER BY AI.album ";
        }


        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint_Pre);

        //this->data = QVariantList();
        sqlite->exec(strQuery, this->data);


        total_page_cnt = 0;
        if((total_db_cnt % LMT_CNT) > 0){
            total_page_cnt = (total_db_cnt / LMT_CNT) + 1;
        }
        else if((total_db_cnt % LMT_CNT) == 0){
            total_page_cnt = (total_db_cnt / LMT_CNT);
        }
        //OffsetPoint += flowLayout->count();
        //OffsetPoint += LMT_CNT;

        qDebug() << "data.size() = " << this->data.size();

        sqlite->close();
        delete sqlite;
    }


}
void MusicAlbum::getAlbumDataFromDB(){//c220323
    print_debug();
    qDebug() << "this->scrollArea->width()= " << this->scrollArea->verticalScrollBar()->height();
    qDebug() << "strArg_1= " << strArg_1;
    qDebug() << "global.LmtCnt= " << global.LmtCnt;

    int cnt = 0;
    LMT_CNT = 0;

    cnt = 1;
        cnt_size = cnt;


        LMT_CNT = cnt * PAGE_LMT;
    //}

    this->data = QVariantList();

    pre_lmt_cnt = LMT_CNT;
    //if(!flagReqMore){//cheon210607-test
    //    flagReqMore = true;//cheon210607-test
    // DB 쿼리

    qDebug() << "global.LmtCnt= " << global.LmtCnt;
    qDebug() << "global.LmtCnt_H= " << global.LmtCnt_H;
    qDebug() << "LMT_CNT= " << LMT_CNT;
    //qDebug() << "this->scrollArea->width= " << this->scrollArea->width();
    //qDebug() << "this->scrollArea->height= " << this->scrollArea->height();
    qDebug() << "cnt= " << cnt;
    //qDebug() << "dum= " << dum;

    if(flag_page_first == true){
        OffsetPoint = 0;
        flag_page_first = false;
    }
    else{
        //currOffsetPoint = OffsetPoint;


    }
    total_page_cnt = 0;
    if((total_db_cnt % LMT_CNT) > 0){
        total_page_cnt = (total_db_cnt / LMT_CNT) + 1;
    }
    else if((total_db_cnt % LMT_CNT) == 0){
        total_page_cnt = (total_db_cnt / LMT_CNT);
    }


    if(total_db_cnt == OffsetPoint) return;

    qDebug() << "********************curr_page_cnt= " << curr_page_cnt;
    //albumtCnt->setText(QString("Total List Count : %1                            Page : %2 / %3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));



        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";

            strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
            strQuery += " FROM album_info AS AI ";
            strQuery += " LEFT OUTER JOIN album_art AS AA ";
            strQuery += " ON AI._id=AA.album_id ";

            QString setWhere = "";
            QString setOrder = "";
            if(!strArg_1.isEmpty()){
                print_debug();
                qDebug() << "strArg_1 = " << strArg_1;

                //strQuery += "(";
                strQuery += searchcharactorSqlWhere();
                //strQuery += ")";

                setWhere = getSetWhereFilter();
                if(!setWhere.isEmpty()){
                    strQuery += " AND ";
                    strQuery += setWhere;
                }

                strQuery += " ORDER BY AI.album ";
            }
            else{

                setWhere = getSetWhereFilter();

                if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
                    if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
                        setOrder += " ORDER BY AI.album ";
                    }
                    else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
                        setOrder += " ORDER BY AI.artist ";
                    }
                    else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3){
                        setOrder += " ORDER BY AI.favorite ";
                    }
                }

                if(!setWhere.isEmpty()){
                    strQuery += " WHERE";
                    strQuery += setWhere;
                }
                if(!setOrder.isEmpty()){
                    strQuery += setOrder;
                }
                //strQuery += " ORDER BY AI.album ";

            }

            strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

            qDebug() << strQuery;

            //QVariantList data;

            sqlite->exec(strQuery, this->data);

            OffsetPoint += this->data.size();
            //OffsetPoint += flowLayout->count();
            //OffsetPoint += LMT_CNT;

            qDebug() << "data.size() = " << this->data.size();

            qDebug() << "in getAlbumDataFromDB-------------OffsetPoint = " << OffsetPoint;
            qDebug() << "in getAlbumDataFromDB-------------curr_page_cnt = " << curr_page_cnt;

            print_debug();
            //Etimer.();
            sqlite->close();
             print_debug();
            delete sqlite;
             print_debug();

        }//if(err.type() == QSqlError::NoError)

}

void MusicAlbum::getAlbumDataFromDB_pre_view(){//c220323
    print_debug();
    qDebug() << "layout->count() = " << flowLayout->count();
    int nn = (global.LmtCnt-33-15-20)/(186+18);
    int mod_nn = (global.LmtCnt-33-15-20)%(186+18);
    int ff = mod_nn/186;
    nn = nn + ff;

    qDebug() << "global.LmtCnt = " << global.LmtCnt;
    qDebug() << "mod_nn = " << mod_nn;
    qDebug() << "ff = " << ff;
    qDebug() << "nn = " << nn;
    //int scrol_cnt = PAGE_LMT/nn;
    PAGE_LMT = nn * 10;
    qDebug() << "1-OffsetPoint_Pre = " << OffsetPoint_Pre;
    getAlbumDataFromDB_pre();
    qDebug() << "2-OffsetPoint_Pre = " << OffsetPoint_Pre;
    if(this->data.size() > 0){
        print_debug();
        //scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    }else{
        print_debug();
        //scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->minimum()+10);
    }
    //if( curr_page_cnt < 1) return;
    if(this->data.size() > 0){

        if(flowLayout->count() >= PAGE_LMT*20){

            OffsetPoint -= this->data.size();
            qDebug() << "#layout->count() = " << flowLayout->count();
            GSCommon::clearLayout_album_onePage(flowLayout, flowLayout->count()-this->data.size(), flowLayout->count());
        }
        //currSliderPosition =  this->scrollArea->verticalScrollBar()->sliderPosition();

        albumtCnt->setText(QString("Total Count : %1").arg(total_db_cnt));
        //albumPage->setText(QString("Album : %2 ~ %3").arg(OffsetPoint_Pre+1).arg(OffsetPoint));
        albumPage->setText(QString("Album : %2 ~ %3").arg(1).arg(OffsetPoint));
        this->slider_playbar->setMaximum(total_page_cnt);
        //GSCommon::clearLayout(flowLayout);

        print_debug();
        for(int i = 0 ; i < this->data.size(); i++){
            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

            QJsonObject jsonData = this->data.at(i).toJsonObject();
            jsonData.insert("section", "album");
            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

            //wg_album->setData(datAI.at(i).toJsonObject());
            wg_album->setData(jsonData);
            wg_album->setHover();
            if(i == 0 || i == this->data.size()-1){
                //wg_album_tmp = new FrameMusicAlbum();
                //wg_album_tmp = wg_album;
                //wg_album->setBackground_label_base();//c220718

            }

            flowLayout->insertWidget(i,wg_album);
        }
        //this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->maximum()/2+150)/92)*92);


    }else{
        if(total_db_cnt==0){//c220511
            print_debug();
            GSCommon::clearLayout(flowLayout);
            QLabel *albumNone = new QLabel;//c220402
            albumNone->setText(QString(tr("\n\n\n                    There is no album.")));
            albumNone->setStyleSheet("font-size:25px; color:#dddddd;");
            flowLayout->addWidget(albumNone);
        }
    }
    this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->minimum()+2700)/92)*92);

}

void MusicAlbum::getAlbumDataFromDB_view(){//c22040301
    print_debug();
    qDebug() << "layout->count() = " << flowLayout->count();
    currOffsetPoint = OffsetPoint;
    if( curr_page_cnt > total_page_cnt) {
        print_debug();
        return;
    }
    //qDebug() << line_bottom_33->width();
    int nn = (global.LmtCnt-33-15-20)/(186+18);
    int mod_nn = (global.LmtCnt-33-15-20)%(186+18);
    int ff = mod_nn/186;
    nn = nn + ff;

    qDebug() << "global.LmtCnt = " << global.LmtCnt;
    qDebug() << "mod_nn = " << mod_nn;
    qDebug() << "ff = " << ff;
    qDebug() << "nn = " << nn;
    //int scrol_cnt = PAGE_LMT/nn;
    PAGE_LMT = nn * 10;
    qDebug() << "1-OffsetPoint = " << OffsetPoint;
    getAlbumDataFromDB();
    qDebug() << "2-OffsetPoint = " << OffsetPoint;

    if(this->data.size() > 0){
        albumtCnt->setText(QString("Total Count : %1").arg(total_db_cnt));
        //albumPage->setText(QString("Album : %2 ~ %3").arg(OffsetPoint_Pre+1).arg(OffsetPoint));
        albumPage->setText(QString("Album : %2 ~ %3").arg(1).arg(OffsetPoint));
        this->slider_playbar->setMaximum(total_page_cnt);

        print_debug();
        qDebug() << "curr_page_cnt = " << curr_page_cnt;
        qDebug() << "flowLayout->count()/LMT_CNT = " << flowLayout->count()/LMT_CNT;

        qDebug() << "after delete---layout->count() = " << flowLayout->count();
        qDebug() << "after delete---this->data.size() = " << this->data.size();
        for(int i = 0 ; i < this->data.size(); i++){
            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

            QJsonObject jsonData = this->data.at(i).toJsonObject();
            jsonData.insert("section", "album");
            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

            //wg_album->setData(datAI.at(i).toJsonObject());
            wg_album->setData(jsonData);
            wg_album->setHover();
            if(i == 0 || i == this->data.size()-1){
                //wg_album->setBackground_label_base();

            }
            flowLayout->addWidget(wg_album);
            //qDebug() << "wg_albumwidth =" << wg_album->width();
            //qDebug() << "this->scrollAreawidth = " << this->scrollArea->width();

        }
        print_debug();
        qDebug() << this->scrollArea->maximumViewportSize();
        qDebug() << "flowLayout->count() = " << flowLayout->count();
        qDebug() << "data->count() = " << this->data.size();
        qDebug() << "currSliderPosition = " << this->scrollArea->verticalScrollBar()->sliderPosition();
        qDebug() << "maximum = " << scrollArea->verticalScrollBar()->maximum();
        currSliderPosition =  this->scrollArea->verticalScrollBar()->sliderPosition();
        if(this->data.size() < PAGE_LMT && flowLayout->count() < PAGE_LMT){
            //this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->minimum()+92)/92)*92);
        }else{
            if(flowLayout->count() < PAGE_LMT*20){
               // this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->maximum())/92)*92);
            }
        }

        qDebug() << "***currSliderPosition = " << this->scrollArea->verticalScrollBar()->sliderPosition();
        if(flowLayout->count() >= PAGE_LMT*20){
            print_debug();
            OffsetPoint_Pre += PAGE_LMT;
            //GSCommon::clearLayout_album_onePage(flowLayout, 0, PAGE_LMT);
            //this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->maximum()-270*10)/92)*92);

        }



    }else{
        if(total_db_cnt == 0){//c220511
            print_debug();
            GSCommon::clearLayout(flowLayout);
            QLabel *albumNone = new QLabel;
            albumNone->setText(QString(tr("\n\n\n                    There is no album.")));
            albumNone->setStyleSheet("font-size:25px; color:#dddddd;");
            flowLayout->addWidget(albumNone);
        }

    }
    if(this->data.size() > 0){
    }else{
    }
    //getAlbumDataFromDB();


}


void MusicAlbum::slot_sliderbarValue_check(){//c22040501

 //0717 mtx.lock();
    if(!global.device.getDeviceIP().isEmpty()){
        //print_debug();

        //curr_LmtWidth = global.LmtCnt;
        currSliderPosition =  this->scrollArea->verticalScrollBar()->sliderPosition();

        //-qDebug() << "sliderPosition=" << currSliderPosition;
        //-qDebug() << "pre---curr_page_cnt=" << curr_page_cnt;
        //-qDebug() << "data.size()=" << data.size();
        //qDebug() << "scrollArea->verticalScrollBar()->maximum()=" << scrollArea->verticalScrollBar()->maximum();
        //-qDebug() << "scrollArea->verticalScrollBar()->minimum()=" << scrollArea->verticalScrollBar()->minimum();


        //if(data.size() >= PAGE_LMT && ((curr_page_cnt == 1 && scrollArea->verticalScrollBar()->maximum()==currSliderPosition) || (scrollArea->verticalScrollBar()->maximum())==currSliderPosition)){

        if((scrollArea->verticalScrollBar()->maximum()==currSliderPosition)){//c220511

                getAlbumDataFromDB_view();

        }else if(OffsetPoint_Pre > 0 && scrollArea->verticalScrollBar()->minimum()==currSliderPosition) {//c220511

           // getAlbumDataFromDB_pre_view();

        }else {
            //qDebug() << "scrollArea->verticalScrollBar()->maximum()=" << scrollArea->verticalScrollBar()->maximum();
            //print_debug();
        }

    }
    //0717 mtx.unlock();

}


void MusicAlbum::slot_sliderPressed(){
    print_debug();
    qDebug() << "pressed----------------------------------------------";
    qDebug() << "this->scrollArea->verticalScrollBar()->sliderPosition()=" << this->scrollArea->verticalScrollBar()->sliderPosition();
}
void MusicAlbum::slot_sliderReleased(){
    print_debug();
    qDebug() << "Released----------------------------------------------";
    qDebug() << "this->scrollArea->verticalScrollBar()->sliderPosition()=" << this->scrollArea->verticalScrollBar()->sliderPosition();
}
/**
 * @brief SearchRemoteBar::slot_returnPressed
 * @param p_text
 */
void MusicAlbum::slot_returnPressed(){//22040301
    print_debug();
    QString tmp_page =this->le_lisearch->text();
    //qDebug() << "slot_returnPressed-p_text" << tmp_page;

    this->goto_setDataFromDB(tmp_page.toInt());
    this->le_lisearch->setText(QString("%1").arg(curr_p_value));
}

void MusicAlbum::slot_clickedChange(){//22040301
    QString tmp_page =this->le_lisearch->text();
    //qDebug() << "slot_clickedChange-p_text" << tmp_page;

    this->goto_setDataFromDB(tmp_page.toInt());
    this->le_lisearch->setText(QString("%1").arg(curr_p_value));
}

void MusicAlbum::slot_returnPressed_str(){//22040301
    QString tmp_page =this->le_lisearch_str->text();
    qDebug() << "slot_returnPressed_str-p_text" << tmp_page;

    setDataFromDB_Alpa(tmp_page);
    this->le_lisearch_str->setText(QString("%1").arg(tmp_page));
}

void MusicAlbum::slot_clickedChange_str(){//22040301
    QString tmp_page =this->le_lisearch_str->text();
    qDebug() << "slot_clickedChange_str-p_text" << tmp_page;
    setDataFromDB_Alpa(tmp_page);
    this->le_lisearch_str->setText(QString("%1").arg(tmp_page));
}

void MusicAlbum::setDataFromDB_Alpa(const QString &p_arg){//c220323

    this->setContentsMargins(0,10,0,0);

    strArg_1 = p_arg;
    print_debug();
    qDebug() << "strArg_1 = " << strArg_1;
    slot_sliderbarValue2_flag = false;
    total_db_cnt = getAlbumDataFromDBforTotalCount();
    flag_page_first = true;
    OffsetPoint = 0;
    curr_page_cnt = 0;

    GSCommon::clearLayout(flowLayout);//c220416
    getAlbumDataFromDB_view();
}

/**
 * @brief MusicAlbum::setFIlterUIControl : Filter 선택 UI 세팅
 */
void MusicAlbum::setFIlterUIControl(){

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

    btn_file_type[0] = createFilterBtn("FileType", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() == 0 ? true:false);
    btn_file_type[1] = createFilterBtn("FileType", 1, tr("Lossless"), settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() == 1 ? true:false);

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

    btn_bit_depth[0] = createFilterBtn("BitDepth", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() == 0 ? true:false);
    btn_bit_depth[1] = createFilterBtn("BitDepth", 1, "24bit↑", settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() == 1 ? true:false);

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

    btn_sample_rate[0] = createFilterBtn("Sampling", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 0 ? true:false);
    btn_sample_rate[1] = createFilterBtn("Sampling", 1, "88.2kHz↑", settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 1 ? true:false);
    btn_sample_rate[2] = createFilterBtn("Sampling", 2, "176.4kHz↑", settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 2 ? true:false);

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

    settings->setValue(rosesettings.SETTINGS_AFILTER_STAR, 0);

    btn_star_point[0] = createFilterBtn("Star", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 0 ? true:false);
    btn_star_point[1] = createFilterBtn("Star", 1, "★★★★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 1 ? true:false);
    btn_star_point[2] = createFilterBtn("Star", 2, "★★★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 2 ? true:false);
    btn_star_point[3] = createFilterBtn("Star", 3, "★★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 3 ? true:false);
    btn_star_point[4] = createFilterBtn("Star", 4, "★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 4 ? true:false);
    btn_star_point[5] = createFilterBtn("Star", 5, "★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 5 ? true:false);
    btn_star_point[6] = createFilterBtn("Star", 6, tr("NONE"), settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 6 ? true:false);

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

    btn_order[0] = createFilterBtn("OrderBy", 0, tr("NONE"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 0 ? true:false);
    btn_order[1] = createFilterBtn("OrderBy", 1, tr("A-Z"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1 ? true:false);
    btn_order[2] = createFilterBtn("OrderBy", 2, tr("Artist"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2 ? true:false);
    btn_order[3] = createFilterBtn("OrderBy", 3, tr("Favorite"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3 ? true:false);

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

    QWidget *btn_Apply = new QPushButton(tr("Apply"));
    btn_Apply->setObjectName("btn_Apply");
    btn_Apply->setCursor(Qt::PointingHandCursor);
    btn_Apply->setFixedSize(70,26);            //45, 50
    btn_Apply->setStyleSheet("#btn_Apply {font-weight:200; font-size:14px; color:#FFFFFF;  background-color:transparent; border:1px solid #FFFFFF;border-radius:13px;  } ");

    QWidget *btn_Cancel = new QPushButton(tr("Cancel"));
    btn_Cancel->setObjectName("btn_Cancel");
    btn_Cancel->setCursor(Qt::PointingHandCursor);
    btn_Cancel->setFixedSize(70,26);            //45, 50
    btn_Cancel->setStyleSheet("#btn_Cancel {font-weight:200; font-size:14px; color:#FFFFFF;  background-color:transparent; border:1px solid #FFFFFF;border-radius:13px;  } ");

    connect(btn_Apply, SIGNAL(clicked()), this, SLOT(slot_filter_apply()));
    //connect(btn_Cancel, SIGNAL(clicked()), this, SLOT(slot_filter_cancel()));

    QHBoxLayout *hl_box_body_btn = new QHBoxLayout;
    hl_box_body_btn->setContentsMargins(250,20,250,20);
    hl_box_body_btn->setSpacing(0);
    hl_box_body_btn->addWidget(btn_Apply);
    //hl_box_body_btn->addWidget(btn_Cancel);

    QVBoxLayout *box_total = new QVBoxLayout;
    box_total->setContentsMargins(0,0,0,0);
    box_total->setSpacing(0);
    box_total->setAlignment(Qt::AlignTop);
    box_total->addLayout(box_header);
    box_total->addWidget(line);
    box_total->addLayout(box_body);
    box_total->addWidget(line);
    box_total->addLayout(hl_box_body_btn);

    widget_filter = new QWidget(this);
    widget_filter->setObjectName("widget_filter");
    widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
    widget_filter->setFixedSize(700,360);
    widget_filter->move(90,10);
    widget_filter->setContentsMargins(1,1,1,1);
    widget_filter->setLayout(box_total);

    widget_filter->hide();
}

void MusicAlbum::setFIlterUIControl_cancel(){

}



void MusicAlbum::slot_filter_apply(){
    print_debug();

    slot_clickedBtnFilter();
     widget_filter->hide();

}

void MusicAlbum::slot_filter_cancel(){
    print_debug();
}

/**
 * @brief MusicAlbum::createFilterLabel : Filter 선택 UI의 필터 라벨 반환
 * @param p_label QString 라벨명
 * @return QLabel*
 */
QLabel* MusicAlbum::createFilterLabel(const QString &p_label){
    QLabel *lb_filter = new QLabel;
    lb_filter->setText(p_label);
    lb_filter->setFixedHeight(48);
    lb_filter->setStyleSheet("color:#CCCCCC;font-size:14px;background-color:transparent;");
    lb_filter->setAlignment(Qt::AlignCenter);

    return lb_filter;
}

/**
 * @brief MusicAlbum::createFilterBtn : Filter 선택 UI의 필터 버튼 반환
 * @param p_text QString 버튼명
 * @param flagSelected bool if true selected 상태 지정
 * @return QPushButton*
 */
/*QPushButton* MusicAlbum::createFilterBtn(const QString &p_text, bool flagSelected){

    QPushButton *btn = new QPushButton;
    btn->setText(p_text);
    btn->setFixedSize(QSize(110,30));
    btn->setStyleSheet(flagSelected ? btnStyle_selected : btnStyle_normal);
    btn->setCursor(Qt::PointingHandCursor);

    return btn;
}*/
QPushButton* MusicAlbum::createFilterBtn(const QString p_filter, const int idx, const QString &p_text, bool flagSelected){

    QPushButton *btn = new QPushButton;
    btn->setText(p_text);
    btn->setFixedSize(QSize(110,30));
    btn->setStyleSheet(flagSelected ? btnStyle_selected : btnStyle_normal);
    btn->setProperty(KEY_AFILTER, p_filter);
    btn->setProperty(KEY_AIDX, idx);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, SIGNAL(clicked()), this, SLOT(slot_changedBtnFilter()));

    return btn;
}

/**
 * @brief MusicAlbum::setDataFromDB DB에서 가져와서 전체 데이터 세팅
 */
void MusicAlbum::setDataFromDB(){//c220323
    // 초기화
    strArg_1 = "";
    total_db_cnt = getAlbumDataFromDBforTotalCount();
    slot_sliderbarValue2_flag = false;
    GSCommon::clearLayout(flowLayout);
    flag_page_first = true;
    OffsetPoint = 0;
    curr_page_cnt = 0;

    print_debug();
    qDebug() << "this->scrollArea->verticalScrollBar()->singleStep=" << this->scrollArea->verticalScrollBar()->singleStep();
    getAlbumDataFromDB_view();

    this->le_lisearch_str->setText(QString("%1").arg(""));
/*
    this->timer = new QTimer(this);
    this->timer->setInterval(200); // 1초
    connect(this->timer, SIGNAL(timeout()), SLOT(slot_sliderbarValue_check()));
    this->timer->start();
    */
}

void MusicAlbum::goto_setDataFromDB(int p_page){//c220323
    // 초기화
    //if(slot_sliderbarValue2_flag == true) return;
//slot_sliderbarValue2_flag = true;
    goto_getAlbumDataFromDB(p_page);
    goto_getAlbumDataFromDB_view();
    //slot_sliderbarValue2_flag = false;
}

/**
 * @brief MusicAlbum::getAlbumDataFromDB : GET 앨범 데이터
 */
void MusicAlbum::goto_getAlbumDataFromDB(int p_page){//cheon210527
    //this->scrollArea->verticalScrollBar()->setSingleStep(180);
    if( total_page_cnt < p_page || p_page <= 0 ) {
        ToastMsg::show(this, "", tr("There are no pages.."), 5000);//cheon210527
        return;
    }
    int cnt = 0;
    LMT_CNT = 0;
    int hwindow_width = global.LmtCnt-33-18-30;// - LEFT_WIDTH;
    cnt = hwindow_width/ (ALBUMWIDTH);

    int cnt_mode = hwindow_width % (ALBUMWIDTH);
    qDebug() << "cnt = " << cnt;
    qDebug() << "cnt_mode = " << cnt_mode;
    int tmp = cnt_mode/(ALBUMWIDTH-18);
    cnt = cnt + tmp;
    /*
    if(global.LmtCnt_H > 1030){
        LMT_CNT = cnt * global.LmtCnt_H/70;
        this->scrollArea->verticalScrollBar()->setSingleStep(270);//90*3);
    }else if(global.LmtCnt_H < 580) {
        this->scrollArea->verticalScrollBar()->setSingleStep(270);//90);
        LMT_CNT = cnt * PAGE_LMT;
    }else{
        this->scrollArea->verticalScrollBar()->setSingleStep(270);//90*2);
        */
    cnt = 1;
        LMT_CNT = cnt * PAGE_LMT;
    //}
    pre_lmt_cnt = LMT_CNT;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        if(flag_page_first == true){
            OffsetPoint += LMT_CNT;
            flag_page_first = false;
        }else{
            OffsetPoint = (p_page - 1) * LMT_CNT;
        }

        if(OffsetPoint == 0){
            curr_page_cnt = 1;
        }
        else{
            curr_page_cnt = OffsetPoint / LMT_CNT + 1;
        }
        before_album_page = curr_page_cnt;

        total_page_cnt = 0;
        if((total_db_cnt % LMT_CNT) > 0){
            total_page_cnt = (total_db_cnt / LMT_CNT) + 1;
        }
        else if((total_db_cnt % LMT_CNT) == 0){
            total_page_cnt = (total_db_cnt / LMT_CNT);
        }

        print_debug();
        qDebug() << "OffsetPoint =" << OffsetPoint;
        //albumtCnt->setText(QString("Total Count : %1                            Page : %2 / %3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));
        //albumtCnt->setText(QString("Total List Count : %1                            Page : %2 / %3").arg(total_db_cnt, (OffsetPoint+1)/LMT_CNT+1, total_page_cnt));

        //qDebug() << "OffsetPoint" << OffsetPoint;
        QString strQuery = "";
        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        //strQuery += " ORDER BY AI.date_added desc ";
        //strQuery += " ORDER BY AI.album asc ";
        //strQuery += " LIMIT 100 OFFSET " + QString::number(flowLayout->count());
        //strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(flowLayout->count());

        QString setWhere = "";
        QString setOrder = "";


        if(!strArg_1.isEmpty()){
            print_debug();
            qDebug() << "strArg_1 = " << strArg_1;

            //strQuery += "(";
            strQuery += searchcharactorSqlWhere();
            //strQuery += ")";

            setWhere = getSetWhereFilter();
            if(!setWhere.isEmpty()){
                strQuery += " AND ";
                strQuery += setWhere;
            }

            strQuery += " ORDER BY AI.album ";
        }
        else{


            setWhere = getSetWhereFilter();

            if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
                    setOrder += " ORDER BY AI.album ";
                }
                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
                    setOrder += " ORDER BY AI.artist ";
                }
                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3){
                    setOrder += " ORDER BY AI.favorite ";
                }
            }

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }
            if(!setOrder.isEmpty()){
                strQuery += setOrder;
            }
            //strQuery += " ORDER BY AI.album ";

        }
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        //QVariantList data;
        this->data = QVariantList();
        sqlite->exec(strQuery, this->data);




        if(this->data.size() > 0){
        }else{

        }
        /*
        //scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout(flowLayout);
            for(int i = 0 ; i < data.size(); i++){
                FrameMusicAlbum *wg_album = new FrameMusicAlbum();

                QJsonObject jsonData = data.at(i).toJsonObject();
                jsonData.insert("section", "album");

                //wg_album->setData(datAI.at(i).toJsonObject());
                wg_album->setData(jsonData);
                wg_album->setHover();
                flowLayout->addWidget(wg_album);
            }
        }
        */
        sqlite->close();

    }

    delete sqlite;
}



QString MusicAlbum::searchcharactorSqlWhere(){
    QString strQuery;
    QString strArg_space = " ";
    if(strArg_1.size() > 1){
        /*
        if(strArg_1 == "etc"){
            print_debug();qDebug() << "etc";
            //strQuery += " WHERE (AI.artist < '가' and AI.artist > '힣' OR AI.album < '가' and AI.album > '힣') and (UPPER(AI.artist) < 'A' and UPPER(AI.artist) > 'Z' OR UPPER(AI.album) < 'A' and UPPER(AI.album) > 'Z')";
            //strQuery += " WHERE (AI.album < '가' and AI.album > '힣') and UPPER(AI.album) < 'A' and UPPER(AI.album) > 'Z'";
            strQuery += " WHERE AI.album >= '가' and AI.album < '힣'";


        }
        else {
            strQuery += " WHERE AI.artist LIKE '%" + strArg_1 + "%' OR AI.album LIKE '%" + strArg_1+ "%'";
        }
        */
        strQuery += " WHERE (AI.artist LIKE '%" + strArg_1 + "%' OR AI.album LIKE '%" + strArg_1+ "%')";

    }else{
        if(strArg_1 == "ㄱ"){
            strQuery += " WHERE (AI.artist >= '가' and AI.artist < '나' OR AI.album >= '가' and AI.album < '나')";

        }else if(strArg_1 == "ㄴ"){
            strQuery += " WHERE (AI.artist >= '나' and AI.artist < '다' OR AI.album >= '가' and AI.album < '다')";

        }else if(strArg_1 == "ㄷ"){
            strQuery += " WHERE (AI.artist >= '다' and AI.artist < '라' OR AI.album >= '다' and AI.album < '라')";

        }else if(strArg_1 == "ㄹ"){
            strQuery += " WHERE (AI.artist >= '라' and AI.artist < '마' OR AI.album >= '라' and AI.album < '마')";

        }else if(strArg_1 == "ㅁ"){
            strQuery += " WHERE (AI.artist >= '마' and AI.artist < '바' OR AI.album >= '마' and AI.album < '바')";

        }else if(strArg_1 == "ㅂ"){
            strQuery += " WHERE (AI.artist >= '바' and AI.artist < '사' OR AI.album >= '바' and AI.album < '사')";

        }else if(strArg_1 == "ㅅ"){
            strQuery += " WHERE (AI.artist >= '사' and AI.artist < '아' OR AI.album >= '사' and AI.album < '아'";

        }else if(strArg_1 == "ㅇ"){
            strQuery += " WHERE (AI.artist >= '아' and AI.artist < '자' OR AI.album >= '아' and AI.album < '자')";

        }else if(strArg_1 == "ㅈ"){
            strQuery += " WHERE (AI.artist >= '자' and AI.artist < '차' OR AI.album >= '자' and AI.album < '차')";

        }else if(strArg_1 == "ㅊ"){
            strQuery += " WHERE (AI.artist >= '차' and AI.artist < '카' OR AI.album >= '차' and AI.album < '카')";

        }else if(strArg_1 == "ㅋ"){
            strQuery += " WHERE AI.artist >= '카' and AI.artist < '타' OR AI.album >= '카' and AI.album < '타')";

        }else if(strArg_1 == "ㅌ"){
            strQuery += " WHERE (AI.artist >= '타' and AI.artist < '파' OR AI.album >= '타' and AI.album < '파')";

        }else if(strArg_1 == "ㅍ"){
            strQuery += " WHERE (AI.artist >= '파' and AI.artist < '하' OR AI.album >= '파' and AI.album < '하')";

        }else if(strArg_1 == "ㅎ"){
            strQuery += " WHERE (AI.artist >= '하' and AI.artist < '힣' OR AI.album >= '하' and AI.album < '힣')";

        }else {
            print_debug();
            //strQuery += " WHERE UPPER(replace(AI.artist, '-','')) LIKE '"+ strArg_1 + "%' OR UPPER(replace(AI.album, '-','')) LIKE '" + strArg_1+ "%'";
            strQuery += " WHERE (UPPER(substr(AI.album, instr(AI.album, ' ')+1,2)) LIKE '"+ strArg_1 + "%' OR UPPER(AI.artist) LIKE '"+ strArg_1 + "%' OR (UPPER(AI.album) LIKE '" + strArg_1+ "%' AND UPPER(substr(AI.album, 2,2)) NOT LIKE '" + strArg_space+ "%'))";

        }
    }
    return strQuery;
}

/**
 * @brief MusicAlbum::getAlbumDataFromDB : GET 앨범 데이터
 */


//strQuery += " WHERE UPPER(AI.artist) LIKE '"+ strArg_1 + "%' OR UPPER(AI.album) LIKE '" + strArg_1+ "%'";
//strQuery += " WHERE UPPER(replace(AI.artist, '-','')) LIKE '"+ strArg_1 + "%' OR UPPER(replace(AI.album, '-','')) LIKE '" + strArg_1+ "%'";
//SUBSTR('ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ',  ROUND((UNICODE(SUBSTR(data1,1,1)) - 44032 ) / 588) + 1 , 1)

//strQuery += " WHERE UPPER(AI.artist) LIKE '"+ SUBSTR('ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ',  ROUND((UNICODE(SUBSTR(data1,1,1)) - 44032 ) / 588) + 1 , 1) + "%' OR UPPER(AI.album) LIKE '" + SUBSTR('ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ',  ROUND((UNICODE(SUBSTR(data1,1,1)) - 44032 ) / 588) + 1 , 1)+ "%'";
//0xAC00
//strQuery += " WHERE (AI.artist RLIKE '^(ㄱ|ㄲ)' OR ( AI.artist >= '가' AND AI.artist < '나' ))";
//uint16_t cc = 0xAC00;
//strArg_1 = "가";




void MusicAlbum::goto_getAlbumDataFromDB_view(){//c220323
    print_debug();
    qDebug() << "pre_mouse_drag_flag = " << pre_mouse_drag_flag;
    int nn = (global.LmtCnt-33-15-20)/(186+18);
    int mod_nn = (global.LmtCnt-33-15-20)%(186+18);
    int ff = mod_nn/186;
    nn = nn + ff;

    qDebug() << "global.LmtCnt = " << global.LmtCnt;
    qDebug() << "mod_nn = " << mod_nn;
    qDebug() << "ff = " << ff;
    qDebug() << "nn = " << nn;
    //int scrol_cnt = PAGE_LMT/nn;
    PAGE_LMT = nn * 10;
    if(pre_mouse_drag_flag == 1){
        print_debug();
        getAlbumDataFromDB();
    }
    pre_mouse_drag_flag = 0;

    if(this->data.size() > 0){

        albumtCnt->setText(QString("Total Count : %1").arg(total_db_cnt));
        albumPage->setText(QString("Page : %2 / %3").arg(curr_page_cnt).arg(total_page_cnt));
        this->slider_playbar->setMaximum(total_page_cnt);
        GSCommon::clearLayout(flowLayout);

        for(int i = 0 ; i < this->data.size(); i++){
            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

            QJsonObject jsonData = this->data.at(i).toJsonObject();
            jsonData.insert("section", "album");
            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

            //wg_album->setData(datAI.at(i).toJsonObject());
            wg_album->setData(jsonData);
            wg_album->setHover();
            flowLayout->addWidget(wg_album);
        }
    }else{
        print_debug();
        GSCommon::clearLayout(flowLayout);
        QLabel *albumNone = new QLabel;//c220402
        albumNone->setText(QString(tr("\n\n\n                    There is no album.")));
        albumNone->setStyleSheet("font-size:25px; color:#dddddd;");
        flowLayout->addWidget(albumNone);

    }
    if(this->data.size() > 0){
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum()*0);
    }else{
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    }
    //getAlbumDataFromDB();

}


void MusicAlbum::getAlbumDataFromDB_view_only(){//c220323
    print_debug();
    qDebug() << "pre_mouse_drag_flag = " << pre_mouse_drag_flag;
    qDebug() << "OffsetPoint = " << OffsetPoint;


    getAlbumDataFromDB();

    currOffsetPoint = OffsetPoint-(cnt_size*PAGE_LMT);
    OffsetPoint = currOffsetPoint;
    if(OffsetPoint<0) OffsetPoint = 0;

    if(this->data.size() > 0){
        albumtCnt->setText(QString("Total Count : %1").arg(total_db_cnt));
        albumPage->setText(QString("Page : %2 / %3").arg(curr_page_cnt).arg(total_page_cnt));
        this->slider_playbar->setMaximum(total_page_cnt);
        GSCommon::clearLayout(flowLayout);

        for(int i = 0 ; i < this->data.size(); i++){
            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

            QJsonObject jsonData = this->data.at(i).toJsonObject();
            jsonData.insert("section", "album");
            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

            //wg_album->setData(datAI.at(i).toJsonObject());
            wg_album->setData(jsonData);
            wg_album->setHover();
            flowLayout->addWidget(wg_album);
            QCoreApplication::processEvents();
        }
    }
    if(this->data.size() > 0){
    }else{
    }


}




QString MusicAlbum::getSetWhereFilter(){
    QString setWhere;

    if(settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() > 0){
        setWhere += " ((AI.mime_type == 'audio/ape') || (AI.mime_type == 'audio/flac') || (AI.mime_type == 'audio/dff') || (AI.mime_type == 'audio/dsf') || (AI.mime_type == 'audio/wav') || (AI.mime_type == 'audio/x-wav')) ";
    }
    if(settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() > 0){
        if(setWhere.isEmpty()){
            setWhere += " (AI.bitdepth >= 24) ";
        }
        else{
            setWhere += " AND (AI.bitdepth >= 24) ";
        }
    }
    if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() > 0){
        if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 1){
            if(setWhere.isEmpty()){
                setWhere += " (AI.samplerate >= 88200) ";
            }
            else{
                setWhere += " AND (AI.samplerate >= 88200) ";
            }
        }
        else if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 2){
            if(setWhere.isEmpty()){
                setWhere += " (AI.samplerate >= 176400) ";
            }
            else{
                setWhere += " AND (AI.samplerate >= 176400) ";
            }
        }
    }
    if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
        if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 6){
            if(setWhere.isEmpty()){
                setWhere += " (AI.songrate == 0) ";
            }
            else{
                setWhere += " AND (AI.songrate == 0) ";
            }
        }
        else{
            switch (settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt()) {
            case 0:
            case 6:
                break;
            case 1:
                if(setWhere.isEmpty()){
                    setWhere += " (AI.songrate == 5) ";
                }
                else{
                    setWhere += " AND (AI.songrate == 5) ";
                }
                break;
            case 2:
                if(setWhere.isEmpty()){
                    setWhere += " (AI.songrate == 4) ";
                }
                else{
                    setWhere += " AND (AI.songrate == 4) ";
                }
                break;
            case 3:
                if(setWhere.isEmpty()){
                    setWhere += " (AI.songrate == 3) ";
                }
                else{
                    setWhere += " AND (AI.songrate == 3) ";
                }
                break;
            case 4:
                if(setWhere.isEmpty()){
                    setWhere += " (AI.songrate == 2) ";
                }
                else{
                    setWhere += " AND (AI.songrate == 2) ";
                }
                break;
            case 5:
                if(setWhere.isEmpty()){
                    setWhere += " (AI.songrate == 1) ";
                }
                else{
                    setWhere += " AND (AI.songrate == 1) ";
                }
                break;
            }
        }
    }
    return setWhere;
}
int MusicAlbum::getAlbumDataFromDBforTotalCount(){
    int ret_cnt = 0;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";

        QString setWhere = "";

        QString setOrder = "";
        if(!strArg_1.isEmpty()){
            print_debug();
            qDebug() << "strArg_1 = " << strArg_1;

            //strQuery += "(";
            strQuery += searchcharactorSqlWhere();
            //strQuery += ")";

            setWhere = getSetWhereFilter();
            if(!setWhere.isEmpty()){
                strQuery += " AND ";
                strQuery += setWhere;
            }

            //strQuery += " ORDER BY AI.album ";
        }
        else{

            setWhere = getSetWhereFilter();
            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }
        }


/*
        if(!strArg_1.isEmpty()){
            print_debug();
            qDebug() << "strArg_1 = " << strArg_1;
            strQuery += searchcharactorSqlWhere();

        }
        else{
            setWhere = getSetWhereFilter();
        }

        if(!setWhere.isEmpty()){
            strQuery += " WHERE";
            strQuery += setWhere;
        }

        */
        //strQuery += " ORDER BY AI.date_added desc ";
        //strQuery += " ORDER BY AI.album asc ";

        QVariantList data;
        sqlite->exec(strQuery, data);
        ret_cnt = data.size();
    }
    sqlite->close();
    delete sqlite;
    return ret_cnt;
}

/**
 * @brief MusicAlbum::slot_clickedBtnFilter:[슬롯] 필터 버튼 클릭
 */
void MusicAlbum::slot_clickedBtnFilter(){
    //if(slot_clickedBtnFilter_filter_flag) return;
    //widget_filter->setVisible(!widget_filter->isVisible());

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

        if(settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt();
            this->lb_file_type->setText(fileTypeList.at(tmp_idx));
            this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_file_type = true;

            hl_filter->addWidget(this->lb_file_type);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt();
            this->lb_bit_depth->setText(bitDepthList.at(tmp_idx));
            this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_bit_depth = true;

            hl_filter->addWidget(this->lb_bit_depth);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt();
            this->lb_sample_rate->setText(samplingList.at(tmp_idx));
            this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_sample_rate = true;

            hl_filter->addWidget(this->lb_sample_rate);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt();
            this->lb_star_point->setText(starList.at(tmp_idx));
            this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_star_point = true;

            hl_filter->addWidget(this->lb_star_point);
            //hl_filter->addSpacing(10);
        }

        if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
            int tmp_idxOrder = settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt();
            this->lb_order->setText(orderList.at(tmp_idxOrder));
            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
            this->flag_order = true;

            hl_filter->addWidget(this->lb_order);
            //hl_filter->addSpacing(10);
        }
        print_debug();
        qDebug() << "this->flag_change_filter = " << this->flag_change_filter;
        if(this->flag_change_filter == true){
            this->flag_change_filter = false;


        }

        setDataFromDB();
    }

}



void MusicAlbum::slot_changedBtnFilter(){

    int i = 0;

    this->selected_optFilter = sender()->property(KEY_AFILTER);
    this->selected_optIdx = sender()->property(KEY_AIDX);

    if(this->selected_optFilter == "FileType"){
        if(this->selected_optIdx == 0){
            this->btn_file_type[0]->setStyleSheet(btnStyle_selected);
            this->btn_file_type[1]->setStyleSheet(btnStyle_normal);
        }
        else if(this->selected_optIdx == 1){
            this->btn_file_type[0]->setStyleSheet(btnStyle_normal);
            this->btn_file_type[1]->setStyleSheet(btnStyle_selected);
        }

        settings->setValue(rosesettings.SETTINGS_AFILTER_FILETYPE, this->selected_optIdx);
        if(this->before_file_type != settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE).toInt()){
            this->before_file_type = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE).toInt();
            this->flag_change_filter = true;
        }
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

        settings->setValue(rosesettings.SETTINGS_AFILTER_BIT, this->selected_optIdx);
        if(this->before_bit_depth != settings->value(rosesettings.SETTINGS_AFILTER_BIT).toInt()){
            this->before_bit_depth = settings->value(rosesettings.SETTINGS_AFILTER_BIT).toInt();
            this->flag_change_filter = true;
        }
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

        settings->setValue(rosesettings.SETTINGS_AFILTER_SAMPLEING, this->selected_optIdx);
        if(this->before_sample_rate != settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING).toInt()){
            this->before_sample_rate = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING).toInt();
            this->flag_change_filter = true;
        }
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

        settings->setValue(rosesettings.SETTINGS_AFILTER_STAR, this->selected_optIdx);
        if(this->before_star_point != settings->value(rosesettings.SETTINGS_AFILTER_STAR).toInt()){
            this->before_star_point = settings->value(rosesettings.SETTINGS_AFILTER_STAR).toInt();
            this->flag_change_filter = true;
        }
    }
    else if(this->selected_optFilter == "OrderBy"){
        for(i = 0; i < 4; i++){
            if(i == this->selected_optIdx){
                this->btn_order[i]->setStyleSheet(btnStyle_selected);
            }
            else{
                this->btn_order[i]->setStyleSheet(btnStyle_normal);
            }

            settings->setValue(rosesettings.SETTINGS_AFILTER_ORDER, this->selected_optIdx);
            if(this->before_order != settings->value(rosesettings.SETTINGS_AFILTER_ORDER).toInt()){
                this->before_order = settings->value(rosesettings.SETTINGS_AFILTER_ORDER).toInt();
                this->flag_change_filter = true;
            }
        }
    }

}

void MusicAlbum::run_flagcheck(){
    forever{
        // 여기서 오래걸리는 작업을 수행




        if ( QThread::currentThread()->isInterruptionRequested() ) {
            return;
        }
    }
}


void MusicAlbum::OnTimerCallbackFunc()//cheon210608-test
{
    int nMilliseconds = Etimer1.elapsed();
    if(nMilliseconds > 100000) Etimer1.restart();
    //QTime time = QTime::currentTime();
    //QString time_text = time.toString("hh : mm : ss");
    //qDebug()<< "OnTimerCallbackFunc-nMilliseconds" << nMilliseconds;
    //qDebug()<< "OnTimerCallbackFunc-NreloadFlag :" << NreloadFlag;
    //qDebug()<< "OnTimerCallbackFunc-PreloadFlag :" << PreloadFlag;

    if(NreloadFlag){//cheon210704-mouse
        //flagReqMore = false;//cheon210607-test
        if(nMilliseconds > 2000){//mouse_time_skippageterm)
            flagReqMore = false;
             getAlbumDataFromDB();//cheon210608-test
        }
        NreloadFlag = false;

    }
    if(PreloadFlag){//cheon210704-mouse
        flagReqMore = false;//cheon210607-test

        getAlbumDataFromDB_pre();//cheon210608-test
        PreloadFlag = false;

    }

}


//scrollArea->verticalScrollBar()->maximum()
/**
 * @brief MusicAlbum::wheelEvent : 휠 이벤트
 * @param event
 */
void MusicAlbum::wheelEvent(QWheelEvent *event){//c220323

    //print_debug();

    CNT++;
    if(CNT > 100000) CNT = 1;
    //qDebug() << "event->angleDelta()" << event->angleDelta();
    //qDebug() << scrollArea->verticalScrollBar()->maximum();
    //qDebug() << this->scrollArea->verticalScrollBar()->sliderPosition();

    if(CNT%1 == 0 && event->angleDelta().ry() <= -120 && (this->scrollArea->verticalScrollBar()->sliderPosition() == scrollArea->verticalScrollBar()->maximum())){//cheon210607-test

        //mtx.lock();
        //if(curr_page_cnt > 0 && curr_page_cnt <= total_page_cnt){
            getAlbumDataFromDB_view();


           //this->scrollArea->ensureWidgetVisible(flowLayout->itemAt(flowLayout->count()/2)->widget());
        //}


        //mtx.unlock();

    }
    if(CNT%1 == 0 && event->angleDelta().ry() >= 120 ){
        //
        //mtx.lock();
        //CNT = 1;
        if(curr_page_cnt > 0){
           // print_debug();
            //getAlbumDataFromDB_pre_view();


        }
        //mtx.unlock();
    }

}

/*
#if defined(Q_OS_WINDOWS)
//if(CNT > 4) CNT = 1;

if(event->angleDelta().ry() <= -120 && this->scrollArea->verticalScrollBar()->sliderPosition() == scrollArea->verticalScrollBar()->maximum()){//cheon210607-test

#endif

#if defined(Q_OS_MAC)
//  timer.start();//cheon210608-test
//if(CNT > 2) CNT = 1;
if(event->angleDelta().ry() <= -120 && CNT%2 == 0){//cheon210607-test
#endif


   mouse_wheel_flag = true;
   //getAlbumDataFromDB();
   if(curr_page_cnt > 0 && curr_page_cnt <= total_page_cnt){
       if(slot_sliderValue_flag && mouse_wheel_flag){
           print_debug();
           getAlbumDataFromDB();
           mouse_wheel_flag = false;
       }
       slot_sliderValue_flag = false;
       getAlbumDataFromDB_view();
   }
  // Etimer1.restart();//cheon210608-test
 // ToastMsg::show(this, "", tr("next."));
 // ToastMsg::show(this, "", tr("next."),50);
   //CNT = 1;
}


#if defined(Q_OS_WINDOWS)//cheon210709-genre
if(CNT > 4) CNT = 1;
//qDebug() << "event->angleDelta()" << event->angleDelta();//cheon210709-genre
if(event->angleDelta().ry() >= 120 && CNT%4 == 0){//cheon210709-genre
#endif        //cheon210709-genre
#if defined(Q_OS_MAC)//cheon210709-genre
   if(CNT > 2) CNT = 1;
if(event->angleDelta().ry() >= 120  && CNT%2 == 0 ){//cheon210709-genre
#endif     //cheon210709-genre
//qDebug() << "event->angleDelta()" << event->angleDelta();


   //getAlbumDataFromDB_pre();//cheon210608-test

   if(curr_page_cnt > 0){
       qDebug() << "curr_page_cnt=" << curr_page_cnt;
       qDebug() << "next_page_cnt=" << next_page_cnt;
       if(curr_page_cnt != next_page_cnt){
           next_page_cnt = curr_page_cnt;
           if(slot_sliderValue_flag && mouse_wheel_flag){
               print_debug();
               getAlbumDataFromDB_pre();
               mouse_wheel_flag = false;
           }
           slot_sliderValue_flag = false;
           getAlbumDataFromDB_pre_view();
       }


   }

   //Etimer1.restart();//cheon210608-test
  // ToastMsg::show(this, "", tr("pre."));
  // ToastMsg::show(this, "", tr("pre."),50);
   //CNT = 1;
}

//qDebug() << "event->angleDelta()" << event->angleDelta();
*/
