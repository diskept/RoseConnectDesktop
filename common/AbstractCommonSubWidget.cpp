#include "common/AbstractCommonSubWidget.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/rosesettings.h"
#include "common/networkhttp.h"//c220728
//#include "common/linker.h"

#include "tidal/PushBtn_withID.h"
#include "tidal/AbstractItem.h"

#include "widget/toastmsg.h"

#include <QSettings>
#include <QScroller>
#include <QScrollBar>
#include <QWheelEvent>
#include <QPixmapCache>
#include <QHBoxLayout>
#include <QMovie>
#include <QSplashScreen>
#include <QTimer>
#include <QThread>
#include <QDesktopServices>//c220816
#include <QGuiApplication>//c220816
#include <QClipboard>//c220816
#include <QLineEdit>//c220816
#include "roseHome/ProcCommon_forRosehome.h"//c220903_2

namespace common {

    const QString tmp_btnStyle      = "padding:10px;border:1px solid #707070;color:#CCCCCC;font-size:18px;";//cheon211008
    const QString tmp_btnStyleHover = "background-color:#B18658;color:#FFFFFF;";//cheon211008

    const int ITEM_LAYOUT_HEIGHT_PLAYLIST = 320;
    const int ITEM_LAYOUT_HEIGHT_ALBUM = 310;
    const int ITEM_LAYOUT_HEIGHT_ARTIST = 240;
    const int ITEM_LAYOUT_HEIGHT_VIDEO = 280;
    const int ITEM_LAYOUT_HEIGHT_PD_ALBUM = 320;


    const int IMAGE_SIZE_OF_TRACK = 60;

    int CNT = 0;

//c220816  start
    BtnLabel::BtnLabel(QString p_text, QString p_objName, QString p_imgPath_selected, QString p_imgPath_unSelected, QWidget *parent) : QPushButton(parent)
    {

        Q_UNUSED(p_objName);

        pixmap_selected = GSCommon::getUIPixmapImg_WH(p_imgPath_selected, 60, 60);
        pixmap_unSelected = GSCommon::getUIPixmapImg_WH(p_imgPath_unSelected, 60, 60);

        lb_image = new QLabel();
        lb_text = new QLabel(p_text);
        if(p_text==""){
            lb_text->setVisible(false);
        }
        QVBoxLayout *vl_total = new QVBoxLayout();
        vl_total->setContentsMargins(0,0,0,0);
        vl_total->setSpacing(0);
        vl_total->addWidget(lb_image, 0, Qt::AlignHCenter);
        vl_total->addWidget(lb_text, 0, Qt::AlignHCenter);
        this->setLayout(vl_total);
        this->setStyleSheet("border:0px;");

        // default
        this->setCursor(Qt::PointingHandCursor);
        this->setStyleSelected(false);
    }
    void BtnLabel::setText(QString p_text){

        this->lb_text->setText(p_text);
        if(p_text==""){
            lb_text->setVisible(false);
        }else{
            lb_text->setVisible(true);
        }
    }
    void BtnLabel::setImgPath(QString p_imgPath_selected, QString p_imgPath_unSelected){

        pixmap_selected = GSCommon::getUIPixmapImg(p_imgPath_selected);
        pixmap_unSelected = GSCommon::getUIPixmapImg(p_imgPath_unSelected);
    }

    void BtnLabel::setStyleSelected(bool p_flagSelected){
        if(p_flagSelected==true){
            lb_image->setPixmap(*pixmap_selected);
            lb_image->resize(pixmap_selected->width(), pixmap_selected->height());
            lb_text->setStyleSheet("color:#B18658;font-size:16px;");
        }else{
            lb_image->setPixmap(*pixmap_unSelected);
            lb_image->resize(pixmap_unSelected->width(), pixmap_unSelected->height());
            lb_text->setStyleSheet("color:#FFFFFF;font-size:16px;");
        }
    }

    void BtnLabel::setStyleText(QString a){
        lb_text->setStyleSheet(a);
    }
//c220816   end

    /**
     * @brief
     * @param parent
     */
    AbstractCommonSubWidget::AbstractCommonSubWidget(MainUIType p_mainUIType, QWidget *parent) : QWidget(parent)
    {
        linker = Linker::getInstance();
        this->flagNeedReload = false;
        this->curr_mainUIType = p_mainUIType;

        switch (curr_mainUIType) {
        case VerticalScroll:
            this->setUIControl_basic_verticalScroll();
            break;
        case VerticalScroll_filter:
        case VerticalScroll_viewAll:
        case VerticalScroll_rosetube:
        case VerticalScroll_roseradio:
            this->setUIControl_filter_verticalScroll();
            break;
        case VerticalScroll_rosefilter:
            this->setUIControl_rosefilter_verticalScroll();
            break;
        case NoneScroll_filter:
        default:
            this->setUIControl_basic_noneScroll();
        }
    }


    /**
     * @brief 소멸자 체크.
     */
    AbstractCommonSubWidget::~AbstractCommonSubWidget(){

        this->deleteLater();
    }


    /**
     * @brief AbstractCommonSubWidget::setActivePage
     */
    void AbstractCommonSubWidget::setActivePage(){

        this->scrollTop();
    }


    /**
     * @brief Scroll이 있는 경우에 대한 처리
     */
    void AbstractCommonSubWidget::scrollTop(){

        if(this->curr_mainUIType == AbstractCommonSubWidget::VerticalScroll || this->curr_mainUIType == AbstractCommonSubWidget::VerticalScroll_filter
                || this->curr_mainUIType == AbstractCommonSubWidget::VerticalScroll_rosefilter || this->curr_mainUIType == AbstractCommonSubWidget::VerticalScroll_viewAll
                || this->curr_mainUIType == AbstractCommonSubWidget::VerticalScroll_rosetube || this->curr_mainUIType == AbstractCommonSubWidget::VerticalScroll_roseradio){
            // scroll to top
            this->scrollArea_main->verticalScrollBar()->setValue(0);
        }
    }


    /**
         * @brief 스크롤이 없는 UI 로 구성
         */
    void AbstractCommonSubWidget::setUIControl_basic_noneScroll(){

        box_mainTitle = new QVBoxLayout();
        box_mainTitle->setSpacing(0);
        box_mainTitle->setContentsMargins(0,0,0,0);
        box_mainTitle->setAlignment(Qt::AlignTop);

        box_contents = new QVBoxLayout();
        box_contents->setSpacing(0);
        if(this->curr_mainUIType == AbstractCommonSubWidget::NoneScroll_filter){
            box_contents->setContentsMargins(0,0,0,0);
        }
        else{
            box_contents->setContentsMargins(33,10,0,0);
        }
        //        box_contents->setAlignment(Qt::AlignTop);

        QVBoxLayout *vBox_main = new QVBoxLayout();
        vBox_main->setSpacing(0);
        vBox_main->setContentsMargins(0,0,0,0);
        vBox_main->addLayout(box_mainTitle);

        vBox_main->addLayout(box_contents);
        vBox_main->setAlignment(Qt::AlignTop);

        setLayout(vBox_main);

        // 시작을 구분 라인 넣어줌
        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#333333;");
        widget_line->setContentsMargins(0,0,0,0);
        if(this->curr_mainUIType != AbstractCommonSubWidget::NoneScroll_filter){
            box_mainTitle->addWidget(widget_line);
        }
    }


    /**
     * @brief Vertical 스크롤이 있는 UI로 구성
     */
    void AbstractCommonSubWidget::setUIControl_basic_verticalScroll(){

        box_contents = new QVBoxLayout();
        box_contents->setSpacing(0);
        box_contents->setContentsMargins(0,0,0,0);
        box_contents->setAlignment(Qt::AlignTop);

        QWidget *widget_forBox = new QWidget();
        widget_forBox->setLayout(box_contents);
        widget_forBox->setContentsMargins(33,10,0,0);

        this->scrollArea_main = new VerticalScrollArea();
        this->scrollArea_main->setWidget(widget_forBox);
        this->scrollArea_main->setWidgetResizable(true);
        this->scrollArea_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->scrollArea_main->setStyleSheet("border:0px;");
        this->scrollArea_main->setAlignment(Qt::AlignTop);

        //QVBoxLayout *boxMain_scrollOut = new QVBoxLayout();
        //boxMain_scrollOut->setSpacing(0);
        //boxMain_scrollOut->setContentsMargins(0,0,0,0);
        //boxMain_scrollOut->addWidget(this->scrollArea_main);

        //boxMain_scrollOut->setSizeConstraint(QLayout::SetNoConstraint); // MyPlalist Hide, Show를 위함

        //----------------------------------------------------------------------

        box_mainTitle = new QVBoxLayout();
        box_mainTitle->setSpacing(0);
        box_mainTitle->setContentsMargins(0,0,0,0);
        box_mainTitle->setAlignment(Qt::AlignTop);


        //QWidget *tmp_widgetHeader = new QWidget();
        //this->setLayout(box_mainTitle);
        //QWidget *tmp_widgetBody = new QWidget();
        //this->setLayout(boxMain_scrollOut);

        QVBoxLayout *vBox_main = new QVBoxLayout();
        vBox_main->setSpacing(0);
        vBox_main->setContentsMargins(0,0,0,0);
        vBox_main->addLayout(box_mainTitle);


        //vBox_main->addLayout(boxMain_scrollOut);
        vBox_main->addWidget(this->scrollArea_main);
        //vBox_main->addWidget(tmp_widgetHeader);
        //vBox_main->addWidget(tmp_widgetBody);

        setLayout(vBox_main);


        // 시작을 구분 라인 넣어줌
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief Fiter & Vertical 스크롤이 있는 UI로 구성
     */
    /*void AbstractCommonSubWidget::setUIControl_filter_verticalScroll(){

        this->box_filter = new QHBoxLayout();
        this->box_filter->setContentsMargins(0,0,0,0);
        this->box_filter->setSpacing(0);
        this->box_filter->setAlignment(Qt::AlignLeft);

        QWidget *widget_filter = new QWidget();
        widget_filter->setContentsMargins(0,0,0,0);
        widget_filter->setStyleSheet("background-color:#171717;");
        widget_filter->setLayout(this->box_filter);

        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#333333;");
        widget_line->setContentsMargins(0,0,0,0);

        QVBoxLayout *topBox_filter = new QVBoxLayout();
        topBox_filter->setSpacing(0);
        topBox_filter->setContentsMargins(0,0,0,0);
        topBox_filter->addWidget(widget_filter);
        topBox_filter->addWidget(widget_line);

        this->box_contents = new QVBoxLayout();
        this->box_contents->setSpacing(0);
        this->box_contents->setContentsMargins(0,0,0,0);
        this->box_contents->setAlignment(Qt::AlignTop);

        QWidget *widget_forBox = new QWidget();
        widget_forBox->setLayout(box_contents);
        if(this->curr_mainUIType == VerticalScroll_filter){
            widget_forBox->setContentsMargins(80,10,80,0);
        }
        if(this->curr_mainUIType == VerticalScroll_viewAll){
            widget_forBox->setContentsMargins(80,10,70,0);
        }

        this->scrollArea_main = new VerticalScrollArea();
        this->scrollArea_main->setWidget(widget_forBox);
        this->scrollArea_main->setWidgetResizable(true);
        this->scrollArea_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setStyleSheet("border:0px;");
        this->scrollArea_main->setAlignment(Qt::AlignTop);

        //----------------------------------------------------------------------

        this->box_mainTitle = new QVBoxLayout();
        this->box_mainTitle->setSpacing(0);
        this->box_mainTitle->setContentsMargins(0,0,0,0);
        this->box_mainTitle->setAlignment(Qt::AlignTop);



        QVBoxLayout *vBox_main = new QVBoxLayout();
        vBox_main->setSpacing(0);
        vBox_main->setContentsMargins(0,0,0,0);
        vBox_main->addLayout(this->box_mainTitle);
        vBox_main->addLayout(topBox_filter);
        vBox_main->addWidget(this->scrollArea_main);

        this->setLayout(vBox_main);
        this->setStyleSheet("background-color:#212121;");


        // 시작을 구분 라인 넣어줌
        this->addUIControl_dividedLine_onMain();
    }*/


    /**
         * @brief Rose Fiter & Vertical 스크롤이 있는 UI로 구성
         */
    /*void AbstractCommonSubWidget::setUIControl_rosefilter_verticalScroll(){

        this->box_filter = new QHBoxLayout();
        this->box_filter->setContentsMargins(0,0,0,0);
        this->box_filter->setSpacing(0);
        this->box_filter->setAlignment(Qt::AlignLeft);

        QWidget *widget_filter = new QWidget();
        widget_filter->setContentsMargins(0,0,0,0);
        widget_filter->setStyleSheet("background-color:#212121;");
        widget_filter->setLayout(this->box_filter);

        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#333333;");
        widget_line->setContentsMargins(0,0,0,0);

        QVBoxLayout *topBox_filter = new QVBoxLayout();
        topBox_filter->setSpacing(0);
        topBox_filter->setContentsMargins(80,0,80,0);
        topBox_filter->addWidget(widget_filter);
        topBox_filter->addWidget(widget_line);

        this->box_contents = new QVBoxLayout();
        this->box_contents->setSpacing(0);
        this->box_contents->setContentsMargins(0,0,0,0);
        this->box_contents->setAlignment(Qt::AlignTop);

        QWidget *widget_forBox = new QWidget();
        widget_forBox->setLayout(box_contents);
        widget_forBox->setContentsMargins(80,0,70,0);

        this->scrollArea_main = new VerticalScrollArea();
        this->scrollArea_main->setWidget(widget_forBox);
        this->scrollArea_main->setWidgetResizable(true);
        this->scrollArea_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setStyleSheet("border:0px;");
        this->scrollArea_main->setAlignment(Qt::AlignTop);

        //----------------------------------------------------------------------

        this->box_mainTitle = new QVBoxLayout();
        this->box_mainTitle->setSpacing(0);
        this->box_mainTitle->setContentsMargins(0,0,0,0);
        this->box_mainTitle->setAlignment(Qt::AlignTop);



        QVBoxLayout *vBox_main = new QVBoxLayout();
        vBox_main->setSpacing(0);
        vBox_main->setContentsMargins(0,0,0,0);
        vBox_main->addLayout(this->box_mainTitle);
        vBox_main->addLayout(topBox_filter);
        vBox_main->addSpacing(10);
        vBox_main->addWidget(this->scrollArea_main);

        this->setLayout(vBox_main);
        this->setStyleSheet("background-color:#212121;");


        // 시작을 구분 라인 넣어줌
        this->addUIControl_dividedLine_onMain();
    }*/


    void AbstractCommonSubWidget::setUIControl_filter_verticalScroll(){

        this->box_filter = new QHBoxLayout();
        this->box_filter->setContentsMargins(0,0,0,0);
        this->box_filter->setSpacing(0);
        this->box_filter->setAlignment(Qt::AlignLeft);

        QWidget *widget_filter = new QWidget();
        widget_filter->setContentsMargins(0,0,0,0);
        if(this->curr_mainUIType == VerticalScroll_roseradio){
            widget_filter->setStyleSheet("background-color:#212121;");
        }
        else{
            widget_filter->setStyleSheet("background-color:#171717;");
        }

        widget_filter->setLayout(this->box_filter);

        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#333333;");
        widget_line->setContentsMargins(0,0,0,0);

        QVBoxLayout *topBox_filter = new QVBoxLayout();
        topBox_filter->setSpacing(0);
        if(this->curr_mainUIType == VerticalScroll_roseradio){
            topBox_filter->setContentsMargins(80, 0, 80 ,0);
        }
        else{
            topBox_filter->setContentsMargins(0,0,0,0);
        }
        topBox_filter->addWidget(widget_filter);
        topBox_filter->addWidget(widget_line);

        this->box_contents = new QVBoxLayout();
        this->box_contents->setSpacing(0);
        this->box_contents->setContentsMargins(0,0,0,0);
        this->box_contents->setAlignment(Qt::AlignTop);

        QWidget *widget_forBox = new QWidget();
        widget_forBox->setLayout(box_contents);
        if(this->curr_mainUIType == VerticalScroll_filter){
            widget_forBox->setContentsMargins(90, 10, 90, 0);
        }
        else if(this->curr_mainUIType == VerticalScroll_roseradio){
            widget_forBox->setContentsMargins(80, 10, 60, 0);
        }
        else if(this->curr_mainUIType == VerticalScroll_viewAll){
            widget_forBox->setContentsMargins(80, 10, 60, 0);
        }
        else if(this->curr_mainUIType == VerticalScroll_rosetube){
            widget_forBox->setContentsMargins(0, 0, 0, 0);
        }

        this->scrollArea_main = new VerticalScrollArea();
        this->scrollArea_main->setWidget(widget_forBox);
        this->scrollArea_main->setWidgetResizable(true);
        this->scrollArea_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setStyleSheet("border:0px;");
        this->scrollArea_main->setAlignment(Qt::AlignTop);
        this->scrollArea_main->verticalScrollBar()->
                setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                                "    border: 2px solid #aaaaaa;border-radius:5px;"
                                                "    background:#777777;"
                                                "    width:15px;  "
                                                "    margin: 0px 0px 0px 0px;"
                                                "}"
                                                "QScrollBar::handle:vertical {"
                                                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                "    stop: 0 #999999, stop: 0.1 #555555, stop: 0.9 #555555, stop:1 #999999);"
                                                "    min-height: 50px;"
                                                "}"
                                                "QScrollBar::add-page:vertical {"
                                                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                "    stop: 0 #aaaaaa, stop: 0.5 #aaaaaa,  stop:1 #aaaaaa);"
                                                "    height: 30px;"
                                                "    subcontrol-position: bottom;"
                                                "    subcontrol-origin: margin;"
                                                "}"
                                                "QScrollBar::sub-page:vertical {"
                                                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                "    stop: 0  ##9facaa, stop: 0.5 ##9facaa,  stop:1 ##9facaa);"
                                                "    height: 30 px;"
                                                "    subcontrol-position: top;"
                                                "    subcontrol-origin: margin;"
                                                "}"
                                                ));
        //----------------------------------------------------------------------

        this->box_mainTitle = new QVBoxLayout();
        this->box_mainTitle->setSpacing(0);
        this->box_mainTitle->setContentsMargins(0,0,0,0);
        this->box_mainTitle->setAlignment(Qt::AlignTop);



        QVBoxLayout *vBox_main = new QVBoxLayout();
        vBox_main->setSpacing(0);
        vBox_main->setContentsMargins(0,0,0,0);
        vBox_main->addLayout(this->box_mainTitle);
        vBox_main->addLayout(topBox_filter);
        vBox_main->addWidget(this->scrollArea_main);

        this->setLayout(vBox_main);
        this->setStyleSheet("background-color:#212121;");


        // 시작을 구분 라인 넣어줌
        this->addUIControl_dividedLine_onMain();
    }


    /**
             * @brief Rose Fiter & Vertical 스크롤이 있는 UI로 구성
             */
    void AbstractCommonSubWidget::setUIControl_rosefilter_verticalScroll(){

        this->box_filter = new QHBoxLayout();
        this->box_filter->setContentsMargins(0,0,0,0);
        this->box_filter->setSpacing(0);
        this->box_filter->setAlignment(Qt::AlignLeft);

        QWidget *widget_filter = new QWidget();
        widget_filter->setContentsMargins(0,0,0,0);
        widget_filter->setStyleSheet("background-color:#212121;");
        widget_filter->setLayout(this->box_filter);

        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#333333;");
        widget_line->setContentsMargins(0,0,0,0);

        QVBoxLayout *topBox_filter = new QVBoxLayout();
        topBox_filter->setSpacing(0);
        topBox_filter->setContentsMargins(80,0,80,0);
        topBox_filter->addWidget(widget_filter);
        topBox_filter->addWidget(widget_line);

        this->box_contents = new QVBoxLayout();
        this->box_contents->setSpacing(0);
        this->box_contents->setContentsMargins(0,0,0,0);
        this->box_contents->setAlignment(Qt::AlignTop);

        QWidget *widget_forBox = new QWidget();
        widget_forBox->setLayout(box_contents);
        widget_forBox->setContentsMargins(75,0,75,0);

        this->scrollArea_main = new VerticalScrollArea();
        this->scrollArea_main->setWidget(widget_forBox);
        this->scrollArea_main->setWidgetResizable(true);
        this->scrollArea_main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->scrollArea_main->setStyleSheet("border:0px;");
        this->scrollArea_main->setAlignment(Qt::AlignTop);
        this->scrollArea_main->verticalScrollBar()->
                setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
                                                        "    border: 2px solid #aaaaaa;"
                                                        "    background:#777777;"
                                                        "    width:15px;  "
                                                        "    margin: 0px 0px 0px 0px;"
                                                        "}"
                                                        "QScrollBar::handle:vertical {"
                                                        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                        "    stop: 0 #999999, stop: 0.5 #171717, stop:1 #999999);"
                                                        "    min-height: 0px;"
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
                                                        ));
        //----------------------------------------------------------------------

        this->box_mainTitle = new QVBoxLayout();
        this->box_mainTitle->setSpacing(0);
        this->box_mainTitle->setContentsMargins(0,0,0,0);
        this->box_mainTitle->setAlignment(Qt::AlignTop);



        QVBoxLayout *vBox_main = new QVBoxLayout();
        vBox_main->setSpacing(0);
        vBox_main->setContentsMargins(0,0,10,0);
        vBox_main->addLayout(this->box_mainTitle);
        vBox_main->addLayout(topBox_filter);
        vBox_main->addSpacing(10);
        vBox_main->addWidget(this->scrollArea_main);

        this->setLayout(vBox_main);
        this->setStyleSheet("background-color:#212121;");


        // 시작을 구분 라인 넣어줌
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief VerticalScrollArea 객체 생성해서 반환
     * @return
     */
    VerticalScrollArea* AbstractCommonSubWidget::get_UIControl_VerticalScrollArea(){
        VerticalScrollArea *tmp_scrollArea = new VerticalScrollArea();
        tmp_scrollArea->setWidgetResizable(true);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setAlignment(Qt::AlignTop);

        return tmp_scrollArea;
    }


    /**
     * @brief 메인 타이틀 라벨을 생성해서 메인 레이아웃에 추가한다.
     * @param mainTitle
     */
    void AbstractCommonSubWidget::addUIControl_mainTitle(const QString mainTitle){

        QLabel *label_mainTitle = new QLabel();
        label_mainTitle->setText(mainTitle);
        label_mainTitle->setStyleSheet("font-size:26px; color:#FFFFFF; background-color:#171717;");//
        label_mainTitle->setContentsMargins(33,20,33,19);
        box_mainTitle->addWidget(label_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();
    }


    /**
     * @brief 메인 타이틀 라벨을 생성해서 메인 레이아웃에 추가하고, 생성한 라벨을 반환한다.//transparent #171717
     * @param mainTitle
     */
    QLabel* AbstractCommonSubWidget::get_addUIControl_mainTitle(const QString mainTitle){

        QLabel *label_mainTitle = new QLabel();
        label_mainTitle->setText(mainTitle);
        label_mainTitle->setStyleSheet("font-size:26px; color:#FFFFFF; background-color:#171717;");
        label_mainTitle->setContentsMargins(33,20,33,19);
        box_mainTitle->addWidget(label_mainTitle);

        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        return label_mainTitle;
    }


    /**
         * @brief 메인 타이틀 라벨을 생성해서 메인 레이아웃에 추가하고, 생성한 라벨을 반환한다.
         * @param mainTitle
         */

    QLabel* AbstractCommonSubWidget::get_addUIControl_mainTitle(const QString mainTitle, QPushButton *a, QPushButton *b, QPushButton *c, QPushButton *d, QPushButton *e, QPushButton *f){//cheon211008-serach


        QLabel *label_mainTitle = new QLabel();
        label_mainTitle->setText(mainTitle);
        label_mainTitle->setStyleSheet("font-size:26px; color:#FFFFFF; background-color:#171717;");
        label_mainTitle->setContentsMargins(33,20,33,19);

        QHBoxLayout *box_mainTitle_submenu = new QHBoxLayout();
        box_mainTitle_submenu->setSpacing(0);
        box_mainTitle_submenu->setContentsMargins(0,0,75,0);
        box_mainTitle_submenu->setAlignment(Qt::AlignTop);
        box_mainTitle_submenu->addWidget(label_mainTitle);
        box_mainTitle_submenu->addWidget(a);
        box_mainTitle_submenu->addWidget(b);
        box_mainTitle_submenu->addWidget(c);
        box_mainTitle_submenu->addWidget(d);
        box_mainTitle_submenu->addWidget(e);
        box_mainTitle_submenu->addWidget(f);
        //box_mainTitle->addLayout(box_mainTitle_submenu);

        QWidget *widget_mainTitle_submenu = new QWidget();
        widget_mainTitle_submenu->setStyleSheet("background-color:#171717;");
        widget_mainTitle_submenu->setLayout(box_mainTitle_submenu);
        widget_mainTitle_submenu->setFixedHeight(70);

        box_mainTitle->addWidget(widget_mainTitle_submenu);

        /*
        // 상단 필터
        QPushButton *btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
        //QPushButton *btn_filter = new BtnLabel(tr("Filter"), "btn_filter", ":/images/ico_filter.png", ":/images/ico_filter.png");
        btn_filter->setFixedSize(50,70);
        //btn_filter->setStyleSheet("font-size:26px; color:#FFFFFF; background-color:#171717;");
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

        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#171717;");
        widget_line->setContentsMargins(0,0,0,0);

        QWidget *widget_btnFilter = new QWidget();
        widget_btnFilter->setObjectName("widget_btnFilter");
        widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
        widget_btnFilter->setFixedHeight(70);
        widget_btnFilter->setLayout(hl_filter);
        box_mainTitle->addWidget(widget_btnFilter);
        box_mainTitle->addWidget(widget_line);
        */
        /*
        //---------------------------------------------------
        a->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
        b->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
        c->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
        d->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
        e->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
        */
        //a->setCursor(Qt::PointingHandCursor);

        connect(a, SIGNAL(clicked()), this, SLOT(slot_clickedMusic()));
        connect(b, SIGNAL(clicked()), this, SLOT(slot_clickedRoseRadio()));
        connect(c, SIGNAL(clicked()), this, SLOT(slot_clickedRoseTube()));
        connect(d, SIGNAL(clicked()), this, SLOT(slot_clickedTidal()));
        connect(e, SIGNAL(clicked()), this, SLOT(slot_clickedBugs()));
        connect(f, SIGNAL(clicked()), this, SLOT(slot_clickedQobuz()));

        /*
        connect(a, SIGNAL(clicked()), this, SLOT(slot_singleShot_clickedMusic()));
        connect(b, SIGNAL(clicked()), this, SLOT(slot_singleShot_clickedRoseRadio()));
        connect(c, SIGNAL(clicked()), this, SLOT(slot_singleShot_clickedRoseTube()));
        connect(d, SIGNAL(clicked()), this, SLOT(slot_singleShot_clickedTidal()));
        connect(e, SIGNAL(clicked()), this, SLOT(slot_singleShot_clickedBugs()));
        connect(f, SIGNAL(clicked()), this, SLOT(slot_singleShot_clickedQobuz()));
        //---------------------------------------------------
*/
        // 하단에 구분 라인
        this->addUIControl_dividedLine_onMain();

        return label_mainTitle;
    }

    void AbstractCommonSubWidget::slot_singleShot_clickMusic()
    {
        QTimer::singleShot(300, this, SLOT(slot_clickedMusic()));
    }

    void AbstractCommonSubWidget::slot_singleShot_clickRoseTube()
    {
        QTimer::singleShot(300, this, SLOT(slot_clickedRoseTube()));
    }
    void AbstractCommonSubWidget::slot_singleShot_clickTidal()
    {
        QTimer::singleShot(300, this, SLOT(slot_clickedTidal()));
    }
    void AbstractCommonSubWidget::slot_singleShot_clickQobuz()
    {
        QTimer::singleShot(300, this, SLOT(slot_clickedQobuz()));
    }
    void AbstractCommonSubWidget::slot_singleShot_clickRoseRadio()
    {
        QTimer::singleShot(300, this, SLOT(slot_clickedRoseRadio()));
    }
    void AbstractCommonSubWidget::slot_singleShot_clickBugs()
    {
        QTimer::singleShot(300, this, SLOT(signal_clickedMovePageBugsSearch()));
    }

    void AbstractCommonSubWidget::slot_clickedMusic(){//cheon211008-serach
        print_debug();

        print_debug();
        if(global.abs_ani_dialog_wait->isHidden() == true){
            QPushButton *tmp_sender = dynamic_cast<QPushButton*>(sender());
            qDebug() << "tmp_sender->objectName()=" << tmp_sender->objectName();
            if(this->pre_btn == tmp_sender->objectName()) {
                // tmp_sender->deleteLater();
                return;
            }
            else{
                this->pre_btn = tmp_sender->objectName();
                //tmp_sender->deleteLater();
                global.searchMenuFlag = true;
                emit linker->signal_clickedMovePageMusicSearch();
                QThread::msleep(300);
            }

            //this->pre_btn = tmp_sender->objectName();

        }


    }


    void AbstractCommonSubWidget::slot_clickedRoseTube(){//cheon211008-serach
        print_debug();

        print_debug();
        if(global.abs_ani_dialog_wait->isHidden() == true){
            QPushButton *tmp_sender = dynamic_cast<QPushButton*>(sender());
            qDebug() << "tmp_sender->objectName()=" << tmp_sender->objectName();
            if(this->pre_btn == tmp_sender->objectName()) {
                //  tmp_sender->deleteLater();
                return;
            }
            else{
                this->pre_btn = tmp_sender->objectName();
                // tmp_sender->deleteLater();
                global.searchMenuFlag = true;
                emit linker->signal_clickedMovePageRoseTubeSearch();
                QThread::msleep(300);
            }

            //this->pre_btn = tmp_sender->objectName();
        }



    }


    void AbstractCommonSubWidget::slot_clickedRoseRadio(){//cheon211008-serach
        print_debug();

        print_debug();
        if(global.abs_ani_dialog_wait->isHidden() == true){
            QPushButton *tmp_sender = dynamic_cast<QPushButton*>(sender());
            qDebug() << "tmp_sender->objectName()=" << tmp_sender->objectName();
            if(this->pre_btn == tmp_sender->objectName()) {
                // tmp_sender->deleteLater();
                return;
            }
            else{
                this->pre_btn = tmp_sender->objectName();
                // tmp_sender->deleteLater();
                global.searchMenuFlag = true;
                emit linker->signal_clickedMovePageRoseRadioSearch();
                QThread::msleep(300);
                //this->pre_btn = tmp_sender->objectName();

            }

        }


    }


    void AbstractCommonSubWidget::slot_clickedTidal(){//cheon211008-serach
        print_debug();

        print_debug();
        if(global.abs_ani_dialog_wait->isHidden() == true){
            QPushButton *tmp_sender = dynamic_cast<QPushButton*>(sender());
            qDebug() << "tmp_sender->objectName()=" << tmp_sender->objectName();
            if(this->pre_btn == tmp_sender->objectName()) {
                // tmp_sender->deleteLater();
                return;
            }
            else{
                this->pre_btn = tmp_sender->objectName();
                // tmp_sender->deleteLater();
                global.searchMenuFlag = true;
                emit linker->signal_clickedMovePageTidalSearch();
                QThread::msleep(300);
                //this->pre_btn = tmp_sender->objectName();
            }
        }


        //global.btn_Tidal->setStyleSheet(QString("#%1 { %2%3 } #%1:hover { %2%3%4 }").arg("btn_Music").arg(tmp_btnStyle + tmp_btnStyleHover).arg("border-top-left-radius:20px;border-bottom-left-radius:20px;").arg(tmp_btnStyleHover));
        //global.btn_Tidal->setStyleSheet(QString("#%1 { %2 } #%1:hover { %2%3 }").arg("btn_Tidal").arg(tmp_btnStyle + tmp_btnStyleHover).arg(tmp_btnStyleHover));

    }


    void AbstractCommonSubWidget::slot_clickedBugs(){//cheon211008-serach
        print_debug();

        print_debug();
        if(global.abs_ani_dialog_wait->isHidden() == true){
            QPushButton *tmp_sender = dynamic_cast<QPushButton*>(sender());
            qDebug() << "tmp_sender->objectName()=" << tmp_sender->objectName();
            if(this->pre_btn == tmp_sender->objectName()) {
              //  tmp_sender->deleteLater();
                return;
            }
            else{
                this->pre_btn = tmp_sender->objectName();
               // tmp_sender->deleteLater();
                global.searchMenuFlag = true;
                emit linker->signal_clickedMovePageBugsSearch();
                QThread::msleep(300);
                //this->pre_btn = tmp_sender->objectName();
            }

        }



    }


    void AbstractCommonSubWidget::slot_clickedQobuz(){//cheon211008-serach
        print_debug();

        print_debug();
        if(global.abs_ani_dialog_wait->isHidden() == true){
            QPushButton *tmp_sender = dynamic_cast<QPushButton*>(sender());
            qDebug() << "tmp_sender->objectName()=" << tmp_sender->objectName();
            if(this->pre_btn == tmp_sender->objectName()) {
              //  tmp_sender->deleteLater();
                return;
            }
            else{
                this->pre_btn = tmp_sender->objectName();
           // tmp_sender->deleteLater();
            global.searchMenuFlag = true;
            emit linker->signal_clickedMovePageQobuzSearch();
            QThread::msleep(300);

            }

        }


    }


    /**
     * @brief 상단의 mainTitle 부분의 Layout에 구분 라인 추가
     */
    void AbstractCommonSubWidget::addUIControl_dividedLine_onMain(){

        // 하단에 구분 라인
        QWidget *widget_line = new QWidget();
        widget_line->setFixedHeight(1);
        widget_line->setStyleSheet("background-color:#333333;");
        widget_line->setContentsMargins(0,0,0,0);
        box_mainTitle->addWidget(widget_line);

    }


    /**
     * @brief Playlist 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forPlaylist(){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,10,10,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        //tmp_scrollArea->setFixedHeight(500);
        tmp_scrollArea->setFixedHeight(ITEM_LAYOUT_HEIGHT_PLAYLIST);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        // Apply Main Layout with spacing
        box_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return hBox_playlist;
    }


    /**
     * @brief Album 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forAlbum(){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,10,10,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(ITEM_LAYOUT_HEIGHT_ALBUM);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        box_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return hBox_album;
    }


    /**
     * @brief Music 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forMusic(){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_music = new QHBoxLayout();
        hBox_music->setSpacing(0);
        hBox_music->setContentsMargins(0,0,0,0);
        hBox_music->setAlignment(Qt::AlignTop);
        hBox_music->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_music);
        widget_content->setContentsMargins(0,10,10,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(ITEM_LAYOUT_HEIGHT_ALBUM);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        box_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return hBox_music;
    }


    /**
     * @brief Track 전용으로, 5개 Row 형태의 세로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QVBoxLayout* AbstractCommonSubWidget::get_addUIControl_vBoxLayout_forTrack(){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QVBoxLayout *vBox_track = new QVBoxLayout();
        vBox_track->setSpacing(0);
        vBox_track->setContentsMargins(0,0,0,0);
        vBox_track->setAlignment(Qt::AlignTop);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(vBox_track);
        widget_content->setContentsMargins(0,10,20,0);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        box_contents->addWidget(widget_content, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return vBox_track;
    }


    /**
     * @brief Artist 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forArtist(){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_album = new QHBoxLayout();
        hBox_album->setSpacing(0);
        hBox_album->setContentsMargins(0,0,0,0);
        hBox_album->setAlignment(Qt::AlignTop);
        hBox_album->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(hBox_album);
        widget_content->setContentsMargins(0,10,10,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(ITEM_LAYOUT_HEIGHT_ARTIST);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        box_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return hBox_album;
    }


    /**
     * @brief Video 전용으로, 1 Row 형태의 가로스크롤 배치하는 Layout 구조 생성 후 반환
     * @return
     */
    QHBoxLayout* AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forVideo(){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QHBoxLayout *hBox_video = new QHBoxLayout();
        hBox_video->setSpacing(0);
        hBox_video->setContentsMargins(0,0,0,0);
        hBox_video->setAlignment(Qt::AlignTop);
        hBox_video->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(hBox_video);
        widget_content->setContentsMargins(0,10,10,0);

        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_content);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(ITEM_LAYOUT_HEIGHT_VIDEO);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        box_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return hBox_video;
    }


    /**
     * @brief AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forPD_Album
     * @return
     */
    QHBoxLayout* AbstractCommonSubWidget::get_addUIControl_hBoxLayout_forPD_Album(){

        //----------------------------------------------------------------------------------------------------
        QHBoxLayout *hBox_playlist = new QHBoxLayout();
        hBox_playlist->setSpacing(0);
        hBox_playlist->setContentsMargins(0,0,0,0);
        hBox_playlist->setSizeConstraint(QLayout::SetFixedSize);

        QWidget *widget_contentSub = new QWidget();
        widget_contentSub->setLayout(hBox_playlist);
        widget_contentSub->setContentsMargins(0,10,10,0);


        QScrollArea *tmp_scrollArea = new QScrollArea();
        tmp_scrollArea->setWidget(widget_contentSub);
        tmp_scrollArea->setWidgetResizable(false);
        tmp_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tmp_scrollArea->setStyleSheet("border:0px;");
        tmp_scrollArea->setContentsMargins(0,0,0,0);
        tmp_scrollArea->setFixedHeight(ITEM_LAYOUT_HEIGHT_PD_ALBUM);

        QScroller::grabGesture(tmp_scrollArea, QScroller::LeftMouseButtonGesture);
        //----------------------------------------------------------------------------------------------------

        // Apply Main Layout with spacing
        box_contents->addWidget(tmp_scrollArea, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return hBox_playlist;
    }


    /**
     * @brief 일반적인 FlowLayout을 생성하여 반환한다.
     * @param hSpace - 아이템들의 가로 간격
     * @param vSpace - 아이템들의 세로 간격
     * @return
     */
    FlowLayout* AbstractCommonSubWidget::get_addUIControl_flowLayout(int hSpace, int vSpace){

        //----------------------------------------------------------------------------------------------------  BODY : START
        FlowLayout *flowLayout = new FlowLayout(0, hSpace, vSpace);     // margin, hSpc, vSpc
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(flowLayout);
        //        widget_content->setContentsMargins(0,20,33,0);
        widget_content->setContentsMargins(0,20,5,0);
        widget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        widget_content->setStyleSheet("background-color:transparent;");
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        box_contents->addWidget(widget_content, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return flowLayout;
    }


    /**
     * @brief AbstractCommonSubWidget::get_UIControl_flowLayout
     * @param hSpace
     * @param vSpace
     * @return
     */
    FlowLayout* AbstractCommonSubWidget::get_UIControl_flowLayout(int hSpace, int vSpace){

        //----------------------------------------------------------------------------------------------------  BODY : START
        FlowLayout *flowLayout = new FlowLayout(0, hSpace, vSpace);     // margin, hSpc, vSpc
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget();
        widget_content->setLayout(flowLayout);
        //        widget_content->setContentsMargins(0,20,33,0);
        widget_content->setContentsMargins(0,20,5,0);
        widget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // return
        return flowLayout;
    }


    /**
     * @brief AbstractCommonSubWidget::get_addUIControl_flowLayout___inStackedWidget
     * @param hSpace
     * @param vSpace
     * @param parent_stackedWidgetAR
     * @return
     */
    FlowLayout* AbstractCommonSubWidget::get_addUIControl_flowLayout___inStackedWidget(int hSpace, int vSpace, StackedWidget_AR *parent_stackedWidgetAR){

        //----------------------------------------------------------------------------------------------------  BODY : START
        FlowLayout *flowLayout = new FlowLayout(0, hSpace, vSpace);     // margin, hSpc, vSpc
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(flowLayout);
        //        widget_content->setContentsMargins(0,20,33,0);
        widget_content->setContentsMargins(0,20,5,0);
        widget_content->setObjectName("tmp_w");
        widget_content->setStyleSheet("#tmp_w { background-color:#881188;} ");
        widget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Add Widget in StackedWidget_AR
        parent_stackedWidgetAR->addWidget(widget_content);

        // return
        return flowLayout;
    }


    /**
     * @brief get_addUIControl_flowLayout___inWidget
     * @param hSpace
     * @param vSpace
     * @param parent_stackedWidget
     * @return
     */
    FlowLayout* AbstractCommonSubWidget::get_addUIControl_flowLayout___inStackedWidget(int hSpace, int vSpace, QStackedWidget *parent_stackedWidget){

        //----------------------------------------------------------------------------------------------------  BODY : START
        FlowLayout *flowLayout = new FlowLayout(0, hSpace, vSpace);     // margin, hSpc, vSpc
        flowLayout->setSizeConstraint(QLayout::SetMinimumSize);

        QWidget *widget_content = new QWidget;
        widget_content->setLayout(flowLayout);
        //        widget_content->setContentsMargins(0,20,33,0);
        widget_content->setContentsMargins(0,20,5,0);
        widget_content->setObjectName("tmp_w");
        widget_content->setStyleSheet("#tmp_w { background-color:#881188;} ");
        widget_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Add Widget in StackedWidget
        parent_stackedWidget->addWidget(widget_content);

        // return
        return flowLayout;
    }


    /**
     * @brief QListWidget 생성하여 반환
     * @return
     */
    QListWidget* AbstractCommonSubWidget::get_addUIControl_ListWidget(tidal::AbstractTidalTrackDelegate *delegate){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QListWidget *listWidget = new QListWidget();
        listWidget->setContentsMargins(0,0,20,0);
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        this->box_contents->addWidget(listWidget, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return listWidget;
    }


    /**
     * @brief AbstractCommonSubWidget::get_addUIControl_ListWidget_forEdit
     * @param delegate
     * @return
     */
    QListWidget* AbstractCommonSubWidget::get_addUIControl_ListWidget_forEdit(AbstractEditListDelegate *delegate){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QListWidget *listWidget = new QListWidget();
        listWidget->setContentsMargins(0,0,20,0);
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");
        //----------------------------------------------------------------------------------------------------  BODY : END

        // Apply Main Layout with spacing
        this->box_contents->addWidget(listWidget, 0, Qt::AlignTop);
        this->addSpacing_forNewPart();

        // return
        return listWidget;
    }


    /**
     * @brief AbstractCommonSubWidget::get_UIControl_ListWidget
     * @param delegate
     * @return
     */
    QListWidget* AbstractCommonSubWidget::get_UIControl_ListWidget(tidal::AbstractTidalTrackDelegate *delegate){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QListWidget *listWidget = new QListWidget();
        listWidget->setContentsMargins(0,0,20,0);
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");
        //----------------------------------------------------------------------------------------------------  BODY : END

        // return
        return listWidget;
    }



    /**
     * @brief AbstractCommonSubWidget::get_addUIControl_ListWidget___inStackedWidget
     * @param delegate
     * @param parent_stackedWidgetAR
     * @return
     */
    QListWidget* AbstractCommonSubWidget::get_addUIControl_ListWidget___inStackedWidget(tidal::AbstractTidalTrackDelegate *delegate, StackedWidget_AR *parent_stackedWidgetAR){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QListWidget *listWidget = new QListWidget();
        listWidget->setContentsMargins(0,0,20,0);
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");
        //----------------------------------------------------------------------------------------------------  BODY : END

        listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        // Add Widget in StackedWidget_AR
        parent_stackedWidgetAR->addWidget(listWidget);

        // return
        return listWidget;
    }


    /**
     * @brief get_addUIControl_ListWidget___inWidget
     * @param delegate
     * @param parent_stackedWidget
     * @return
     */
    QListWidget* AbstractCommonSubWidget::get_addUIControl_ListWidget___inStackedWidget(tidal::AbstractTidalTrackDelegate *delegate, QStackedWidget *parent_stackedWidget){

        //----------------------------------------------------------------------------------------------------  BODY : START
        QListWidget *listWidget = new QListWidget();
        listWidget->setContentsMargins(0,0,20,0);
        listWidget->setItemDelegate(delegate);
        listWidget->setCursor(Qt::PointingHandCursor);
        listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setStyleSheet("border:none;");
        //----------------------------------------------------------------------------------------------------  BODY : END

        listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        // Add Widget in StackedWidget
        parent_stackedWidget->addWidget(listWidget);

        // return
        return listWidget;
    }


    /**
     * @brief 서브타이틀과 우측에 버튼이 있는 구성으로 위젯을 만들어서 메인 레이아웃에 추가한다.
     * @details 사이드 버튼을 만들고 싶지 않은 경우에는 btnText="" 로 넣어준다. 사이드 버튼에 대한 슬롯을 specific 하게 지정해주고 싶을 때 slot_btnClick에 지정할 수 있다.
     * @param subTitle
     * @param btnText
     * @param slot_btnClick
     */
    void AbstractCommonSubWidget::addUIControl_subTitle_withSideBtn(const QString subTitle, const QString btnText, const char *slot_btnClick){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0,5,33,5);
        widget_box_subTitle->setLayout(tmp_hBox);

        box_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);



        // subTitle label
        QLabel *tmp_label_subTitle = new QLabel();
        tmp_label_subTitle->setText(subTitle);
        tmp_label_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF;");
        tmp_hBox->addWidget(tmp_label_subTitle,0, Qt::AlignVCenter);


        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            QPushButton *tmp_btnView_all = new QPushButton();
            tmp_btnView_all->setText("View All");
            tmp_btnView_all->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            tmp_btnView_all->setCursor(Qt::PointingHandCursor);
            connect(tmp_btnView_all, SIGNAL(clicked()), this, slot_btnClick);

            tmp_hBox->addWidget(tmp_btnView_all,1, Qt::AlignVCenter | Qt::AlignRight);
        }

    }


    void AbstractCommonSubWidget::addUIControl_subTitle_common(const QString subTitle){//cheon211015

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0,5,33,5);
        widget_box_subTitle->setLayout(tmp_hBox);

        box_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);



        // subTitle label
        QLabel *tmp_label_subTitle = new QLabel();
        tmp_label_subTitle->setText(subTitle);
        tmp_label_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF;");
        tmp_hBox->addWidget(tmp_label_subTitle,0, Qt::AlignVCenter);
    }


    /**
     * @brief 서브타이틀과 우측에 버튼이 있는 구성으로 위젯을 만들어서 메인 레이아웃에 추가한다.
     * @details 사이드 버튼을 만들고 싶지 않은 경우에는 btnText="" 로 넣어준다.  사이드 버튼에 대한 슬롯은 자동으로 @see slot_clickBtn_subTitle_side 와 연결되어 있다.
     * @param subTitle
     * @param btnText
     * @param btnId
     */
    void AbstractCommonSubWidget::addUIControl_subTitle_withSideBtn_common(const QString subTitle, const QString btnText, const int btnId){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0,5,33,5);
        widget_box_subTitle->setLayout(tmp_hBox);

        box_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);

        // subTitle label
        QLabel *tmp_label_subTitle = new QLabel();
        tmp_label_subTitle->setText(subTitle);
        tmp_label_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF;");
        tmp_hBox->addWidget(tmp_label_subTitle,0, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            tidal::PushBtn_withID *tmp_btnView_all = new tidal::PushBtn_withID(btnId);
            tmp_btnView_all->setText(btnText);
            tmp_btnView_all->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            tmp_btnView_all->setCursor(Qt::PointingHandCursor);
            connect(tmp_btnView_all, &tidal::PushBtn_withID::btnClicked, this, &AbstractCommonSubWidget::slot_clickBtn_subTitle_side);

            tmp_hBox->addWidget(tmp_btnView_all,1, Qt::AlignVCenter | Qt::AlignRight);
        }
    }


    void AbstractCommonSubWidget::addUIControl_subTitle_withSideBtn_common_cnt(const QString subTitle, const QString btnText, const int btnId, QLabel *lbCnt){

        // box_contents 에 담을 widget, layout 생성.  box_contents에 먼저 담는다.
        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(0,0,0,0);
        tmp_hBox->setAlignment(Qt::AlignTop);

        QWidget *widget_box_subTitle = new QWidget();
        widget_box_subTitle->setContentsMargins(0,5,33,5);
        widget_box_subTitle->setLayout(tmp_hBox);

        box_contents->addWidget(widget_box_subTitle, 0, Qt::AlignTop);

        // lbCnt->setText(QString("%1").arg(global.listCnt1));
        lbCnt->setStyleSheet("font-size:22px; color:#FFFFFF;");

        // subTitle label
        QLabel *tmp_label_subTitle = new QLabel();
        tmp_label_subTitle->setText(subTitle);
        tmp_label_subTitle->setStyleSheet("font-size:22px; color:#FFFFFF;");
        tmp_hBox->addWidget(tmp_label_subTitle,0, Qt::AlignVCenter);
        tmp_hBox->setSpacing(10);
        tmp_hBox->addWidget(lbCnt,1, Qt::AlignVCenter);

        // side button - it works when the button text is not empty
        if(btnText.isEmpty() == false){
            tidal::PushBtn_withID *tmp_btnView_all = new tidal::PushBtn_withID(btnId);
            tmp_btnView_all->setText(btnText);
            tmp_btnView_all->setStyleSheet("QPushButton{background:transparent; color:#CCCCCC; font-size:20px;} QPushButton:hover{color:#B18658;}");
            tmp_btnView_all->setCursor(Qt::PointingHandCursor);
            connect(tmp_btnView_all, &tidal::PushBtn_withID::btnClicked, this, &AbstractCommonSubWidget::slot_clickBtn_subTitle_side);

            tmp_hBox->addWidget(tmp_btnView_all,2, Qt::AlignVCenter | Qt::AlignRight);
        }
    }


    /**
     * @brief 화면에서 컨텐츠들 간의 구분을 위한 기본 스페이스를 적용. (디자인에선 30 으로 통일되어 있음)
     */
    void AbstractCommonSubWidget::addSpacing_forNewPart(){

        this->box_contents->addSpacing(30);
    }


    /**
     * @brief 타이달 > 탐색에서 장르선택, 무드선택 옵션 버튼에 대한 스타일. 공통 사용. (다른 메뉴에서도 보임)
     */
    QString AbstractCommonSubWidget::getStyle_pushButton_optionChoose(){

        QString btn_style = ""
                    ".QPushButton{ "
                        "padding-left:37px; padding-right:37px; padding-top:10px; padding-bottom:10px;"
                        "background:transparent;color:#FFFFFF;font-size:16px;"
                        "border:2px solid #494949; border-radius:20px;"
                   "}"
                   ".QPushButton:hover {background-color:#BE905F;}";
        return btn_style;
    }


    /**
     * @brief 레이아웃에다가 개발에 필요한 내용 적어둠. UI에 표현
     * @param layout
     */
    void AbstractCommonSubWidget::addUIControl_needDevCheckMsg(QLayout *layout, const DevCheckMsg p_devCheckMsg){

        QLabel *tmp_label_unknown_part = new QLabel();
        tmp_label_unknown_part->setFixedHeight(100);
        tmp_label_unknown_part->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

        switch (p_devCheckMsg) {
        case WaitRes_fromCITECH :
            tmp_label_unknown_part->setText("어떤 API를 사용해야하는지, API 필요함");
            tmp_label_unknown_part->setStyleSheet("font-size:14px; color:#ffff66; background-color:#333333; border-radius:10px;");
            break;

        case NoExist_API :
            tmp_label_unknown_part->setText("현재 해당 API가 없는 상태임");
            tmp_label_unknown_part->setStyleSheet("font-size:14px; color:#66ff66; background-color:#333333; border-radius:10px;");
            break;

        case API_doesNot_Work :
            tmp_label_unknown_part->setText("API가 현재 동작하지 않음. API 개선 중인듯?");
            tmp_label_unknown_part->setStyleSheet("font-size:14px; color:#ff0000; background-color:#333333; border-radius:10px;");
            break;

        case NeededCheckAPI :
            tmp_label_unknown_part->setText("API 체크 필요함");
            tmp_label_unknown_part->setStyleSheet("font-size:14px; color:#ffff66; background-color:#333333; border-radius:10px;");
            break;

        case NeedDev :
            tmp_label_unknown_part->setText("개발 해야됨");
            tmp_label_unknown_part->setStyleSheet("font-size:14px; color:#333333; background-color:#eeeeee; border-radius:10px;");
            break;

        case Unknown :
            tmp_label_unknown_part->setText("Unknown Part");
            tmp_label_unknown_part->setStyleSheet("font-size:14px; color:#ff0000; background-color:#eeeeee; border-radius:10px;");
            break;

        }

        layout->addWidget(tmp_label_unknown_part);
    }


    /**
     * @brief 토스트 메시지를 화면에 뿌린다.
     * @param message
     */
    void AbstractCommonSubWidget::showToastMsg(const QString message){

        ToastMsg::show(this, "", message);
    }


    /**
         * @brief AbstractCommonSubWidget::wheelEvent
         * @param event
         */
    void AbstractCommonSubWidget::wheelEvent(QWheelEvent* event){

        event->accept();
        // 더 가져오기 실행

        // -------------------------------------------------
        //  Win, MSVC 환경에서 pixelDelta 항상 0,0
        // -------------------------------------------------
    /*
        if(event->pixelDelta().y() < 0){
            // Widget의 아래쪽으로 경계에서 Wheel 될때. - for Geting More Data
            this->proc_wheelEvent_to_getMoreData();
        }
        else if(event->pixelDelta().y() > 0){
            // Widget의 위쪽으로 경계에서 Wheel 될때. - for Refreshing (Reloading)
            this->proc_wheelEvent_to_refresh();
        }
        */

    /*
       if(event->delta() < 0){
            // Widget의 아래쪽으로 경계에서 Wheel 될때. - for Geting More Data
            this->proc_wheelEvent_to_getMoreData();
        }
        else if(event->delta() > 0){
            // Widget의 위쪽으로 경계에서 Wheel 될때. - for Refreshing (Reloading)
            this->proc_wheelEvent_to_refresh();
        }*/

#if defined(Q_OS_WINDOWS)//cheon210703-list
        CNT++;//cheon210703-list
        if(CNT > 10000) CNT = 1;//cheon210703-list
        qDebug() << "event->angleDelta()" << event->angleDelta();//cheon210703-list
        if(event->angleDelta().ry() <= -120 && CNT%1 == 0){//cheon210703-list
            // Widget의 아래쪽으로 경계에서 Wheel 될때. - for Geting More Data
            //qDebug() << "event->angleDelta()" << event->angleDelta();

            this->proc_wheelEvent_to_getMoreData();//cheon210703-list

        }
        else if(event->angleDelta().ry() >= 120 && CNT%1 == 0){//cheon210703-list
            // Widget의 위쪽으로 경계에서 Wheel 될때. - for Refreshing (Reloading)
            //qDebug() << "event->angleDelta()" << event->angleDelta();

            this->proc_wheelEvent_to_refresh();//cheon210703-list
        }
#elif defined(Q_OS_MAC)//cheon210703-list
        if(event->angleDelta().ry() <= -120 ){//cheon210703-list
            this->proc_wheelEvent_to_getMoreData();//cheon210703-list
        }
        else if(event->angleDelta().ry() >= 120){//cheon210703-list
            this->proc_wheelEvent_to_refresh();//cheon210703-list
        }
#endif//cheon210703-list
    }


    /**
     * @brief signal_clickedMovePage 시그널에 대한 Slot 연결
     * @param receiver - signal 받을 객체
     * @param member - receiver의 slot 함수.  @see signal_clickedMovePage 시그널 참고.
     */
    void AbstractCommonSubWidget::setConnectSiganl_toMovePage(const QObject *receiver, const char *member){

        connect(this, SIGNAL(signal_clickedMovePage(const QJsonObject&)), receiver, member);
    }


    /**
     * @brief signal_shouldGoBackward 시그널에 대한 Slot 연결
     * @param receiver
     * @param member
     */
    void AbstractCommonSubWidget::setConnectSiganl_toShouldGoBackward(const QObject *receiver, const char *member){

        connect(this, SIGNAL(signal_shouldGoBackward(const QJsonObject&)), receiver, member);
    }


    /**
         * @brief HTTP 요청이 있는 경우, 요놈 재정의해서 사용할 것
         * @param p_id
         * @param p_jsonObj
         */
    void AbstractCommonSubWidget::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){
        Q_UNUSED(p_id);
        Q_UNUSED(p_jsonObj);
    }


    /**
     * @brief subTitle의 우측에 버튼이 있는 경우, 재정의해서 사용할 것
     * @see addUIControl_subTitle_withSideBtn(const QString, const QString, const int)
     */
    void AbstractCommonSubWidget::slot_clickBtn_subTitle_side(const int btnId){
        Q_UNUSED(btnId);
    }


    /**
     * @brief OptMorePopup을 생성하여 화면에 띄운다. signal - slot도 자동으로 연결한다.
     * @param menuMode
     * @param data_header
     * @param index
     * @param section
     */
    void AbstractCommonSubWidget::makeObj_optMorePopup(const OptMorePopup::PopupMenuMode menuMode, const OptMorePopup::HeaderData &data_header, const int index, const int section, const bool flagForceHide_favorite){//c220903_2
        /*
        data_header.main_title = data.title;
        data_header.sub_title = data.ownerName;
        data_header.imageUrl = data.thumbnail;
        data_header.data_pk = QString("%1").arg(data.id);
        */

        print_debug();
        qDebug() << "data_header.title=" << data_header.main_title;
        qDebug() << "data_header.clientKey=" << data_header.clientKey;
        qDebug() << "data_header.id=" << data_header.data_pk;
        qDebug() << "data_header.imageUrl=" << data_header.imageUrl;    //j220905 share link
        qDebug() << "data_header.type=" << data_header.type;            //j220906 share link
        qDebug() << "data_header.isShare=" << data_header.isShare;      //j220906 share link
        qDebug() << "data_header.isRose=" << data_header.isRose;        //j220905 share link

        int sw = menuMode/10;
        qDebug() << "menuMode=" << menuMode;

        QString param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

        switch(sw)
        {
        case 0://none
            print_debug();
            break;
        case 2://Music
            print_debug();
            break;
        case 4://RoseTube
            print_debug();
            if(menuMode == 99){//album
                param = "ROSE_TUBE/ALBUM/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 42){//playlist
                param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 43){//track
                param = "ROSE_TOTAL/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 41){//video
                //j220906 share link
                if(data_header.data_pk.contains("https://youtu.be/")){
                    param = "ROSE_TUBE/TRACK/" + QString("%1").arg(data_header.data_pk);
                }
                else{
                    param = "ROSE_TOTAL/TRACK/" + QString("%1").arg(data_header.data_pk);
                }
                //j220906 share link

            }else if(menuMode == 44){//caching
                param = "ROSE_TUBE/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else{
            }
            break;
        case 7://Tidal
             print_debug();
            if(menuMode == 71){//album
                param = "TIDAL/ALBUM/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 72){//track
                param = "TIDAL/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 73){//playlist
                param = "TIDAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 74){//Playlist_Created
                param = "TIDAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 75){//video
                param = "TIDAL/VIDEO/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 76){//artist
                param = "TIDAL/ARTIST/" + QString("%1").arg(data_header.data_pk);

            }else{
            }
            break;
        case 8://Bugs
            print_debug();
            if(menuMode == 81){//track
                param = "BUGS/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 82){//Track_withVideo
                param = "BUGS/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 83 ){//album
                param = "BUGS/ALBUM/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 84 ){//pd_album
                param = "BUGS/musicpd/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 85){//my_album
                param = "BUGS/ALBUM/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 86){//video
                param = "BUGS/mv/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 87){//artist
                param = "BUGS/ARTIST/" + QString("%1").arg(data_header.data_pk);

            }else{
            }
            break;
        case 9://Qobuz
             print_debug();
            if(menuMode == 91){//album
                param = "QOBUZ/ALBUM/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 92){//track
                param = "QOBUZ/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 93){//playlist
                param = "QOBUZ/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 94){//Playlist_Created
                param = "QOBUZ/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 95){//video
                param = "QOBUZ/VIDEO/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 96){//artist
                param = "QOBUZ/ARTIST/" + QString("%1").arg(data_header.data_pk);   //j220907 share link

            }else{
            }
            break;
        case 10://RoseHome
            print_debug();
            if(menuMode == 101){//album
                param = QString("%1/ALBUM/").arg(data_header.type) + QString("%1").arg(data_header.clientKey);

            }else if(menuMode == 102){//playlist
                param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 103){//Playlist_Created
                param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 104){//track
                param = "ROSE_TOTAL/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 105){//video
                param = "ROSE_TOTAL/VIDEO/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 106){//artist
                param = "ROSE_TOTAL/ARTIST/" + QString("%1").arg(data_header.data_pk);

            }else{
            }
            break;
        case 11://Apple
             print_debug();
            if(menuMode == 111){//album
                param = "APPLE_MUSIC/ALBUM/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 112){//playlist
                param = "APPLE_MUSIC/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 113){//Playlist_Created
                param = "APPLE_MUSIC/PLAYLIST/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 114){//track
                param = "APPLE_MUSIC/TRACK/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 115){//video
                param = "APPLE_MUSIC/VIDEO/" + QString("%1").arg(data_header.data_pk);

            }else if(menuMode == 116){//artist
                param = "APPLE_MUSIC/ARTIST/" + QString("%1").arg(data_header.data_pk);

            }else{
            }
            break;
        default:
            break;
        }

        //QString param = "ROSE_TOTAL/PLAYLIST/" + QString("%1").arg(data_header.data_pk);
        roseHome::ProcCommon *proc_link = new roseHome::ProcCommon(this);
        connect(proc_link, &roseHome::ProcCommon::completeReq_share_link, this, &AbstractCommonSubWidget::slot_applyResult_getShareLink);
        proc_link->request_rose_get_shareLink(data_header.imageUrl  , data_header.sub_title , data_header.main_title, param);

        // OptMorePopup 을 띄운다.
        print_debug();
        qDebug() << "OptMorePopup::PopupMenuMode menuMode=" << menuMode;

        OptMorePopup *optPopup = new OptMorePopup(this);
        connect(optPopup, &OptMorePopup::signal_menuClicked, this, &AbstractCommonSubWidget::slot_optMorePopup_menuClicked);
        optPopup->setMenuMode(menuMode, data_header, index, section, flagForceHide_favorite);
        optPopup->showPopup();

    }



    void AbstractCommonSubWidget::slot_applyResult_getShareLink(const QString &link){//c220826_2
        print_debug();
        qDebug() << "link= " << link;
        this->shareLink = link;
    }
    /**
     * @brief Layout에서 itemWidget을 찾아서 hide 처리한다. (index, section 정보로 찾음)
     * @param p_layout
     * @param index
     * @param section
     */
    void AbstractCommonSubWidget::hide_itemWidget_fromLayout(QLayout *p_layout, int index, int section){

        for(int i=0 ; i<p_layout->count() ; i++){
            QLayoutItem *tmp_layoutItem = p_layout->itemAt(i);
            tidal::AbstractItem *tmp_item = qobject_cast<tidal::AbstractItem*>(tmp_layoutItem->widget());
            if(tmp_item){
                if(tmp_item->section()==section && tmp_item->index()==index){
                    // hide 처리함
                    tmp_item->hide();
                    break;
                }
            }
        }
    }


    /**
     * @brief Layout에서 itemWidget을 찾아서 delete 처리한다. (index, section 정보로 찾음)
     *      꼭 필요한 경우에만 사용한다. hide 사용을 권고함. (list 변수와의 index 동기화가 필요하기 때문임)
     * @param p_layout
     * @param index
     * @param section
     */
    void AbstractCommonSubWidget::remove_itemWidget_fromLayout(QLayout *p_layout, int index, int section){

        for(int i=0 ; i<p_layout->count() ; i++){
            QLayoutItem *tmp_layoutItem = p_layout->itemAt(i);
            tidal::AbstractItem *tmp_item = qobject_cast<tidal::AbstractItem*>(tmp_layoutItem->widget());
            if(tmp_item){
                if(tmp_item->section()==section && tmp_item->index()==index){
                    // remove 처리함
                    tmp_layoutItem = p_layout->takeAt(i);
                    delete tmp_layoutItem->widget();
                    delete tmp_layoutItem;
                    break;

                }
            }
        }
    }


    /**
     * @brief [Slot] OptMorePopup에서 메뉴 클릭에 대한 처리
     * @param clickMode
     * @param index
     * @param section
     */
    void AbstractCommonSubWidget::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){
        Q_UNUSED(clickMode);
        Q_UNUSED(index);
        Q_UNUSED(section);

    }



    /**
     * @brief AbstracctTidalTrckDelegate 클래스 군에 대해서 Track 기본 이미지를 위한 QPixmap 반환
     * @return
     */
    QPixmap AbstractCommonSubWidget::getDefaultImage_forTrack(){

        return QPixmap(":/images/def_mus_60.png");
        //return QPixmap("");
    }


    /**
     * @brief index 유효성을 체크한다. 유효하지 않은 경우에는 -1을 반환한다.
     * @param length_ofData
     * @param index
     * @return
     */
    int AbstractCommonSubWidget::checkValid_index(int length_ofData, int index){

        if(index<0 || index>=length_ofData){
            index = -1;
        }
        return index;
    }


    /**
     * @brief createList_itemEditDelegate_applyingWithData_showImage
     * @param list_data
     * @param p_listWidget
     * @param start_index
     * @param flagAdjustHeight
     */
    void AbstractCommonSubWidget::createList_itemEditDelegate_applyingWithData_showImage(const QList<editDelegate::ItemEditData>& list_data, QListWidget *p_listWidget, const int start_index, bool flagAdjustHeight){

        // Track 기본 이미지
        QPixmap pixmap_default(":images/def_mus_60.png");

        for(int i=0 ; i<list_data.size() ; i++){
            QListWidgetItem *tmp_item = new QListWidgetItem();
            tmp_item->setData(Qt::UserRole, AbstractEditListDelegate::getObjectJson_itemEditData(list_data.at(i)));

            QPixmap pixmap;
            QString tmp_imgPath = list_data.at(i).album_image;
            if(tmp_imgPath.isEmpty()){
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image
            }
            else{
                // download (no-cache)
                tmp_item->setData(Qt::DecorationRole, pixmap_default);  // using Default image (download 실패 케이스 대응을 위한 기본값 우선 세팅)

                FileDownloader *fileDownloader = new FileDownloader(this);
                connect(fileDownloader, &FileDownloader::downloaded, this, &AbstractCommonSubWidget::slot_editDelegate_thumbnailDownloaded_fotListWidget);
                fileDownloader->setProperty("index", i + start_index);
                fileDownloader->setImageURL(tmp_imgPath);
            }

            p_listWidget->addItem(tmp_item);
        }


        if(list_data.length() > 0){
            p_listWidget->setVisible(true);
            // listWidget height 조절
            if(flagAdjustHeight){
                p_listWidget->setFixedHeight(70 * (start_index + list_data.length()));         // 고정값 설정
            }
        }
        else{
            p_listWidget->setVisible(false);
        }
    }


    /**
     * @brief ListWidget과 관련 - Image 다운로드가 완료되면 ListWidget에 보이도록 하기 위한 처리 (DecorationRole 로 설정)
     * @param p_listWidget
     * @param p_list_track
     */
    void AbstractCommonSubWidget::applyImageData_itemEditDelegate_withFileDownload(FileDownloader *fileDownload_sender, QListWidget *p_listWidget, QList<editDelegate::ItemEditData>* p_list_item){

        int tmp_index = fileDownload_sender->property("index").toInt();
        if(tmp_index>=0 && tmp_index<p_list_item->count()){
            QImage image;
            bool flagLoaded = image.loadFromData(fileDownload_sender->downloadedData());

            if(flagLoaded){
                QPixmap tmp_pixmap;
                tmp_pixmap = tmp_pixmap.fromImage(image);
                tmp_pixmap = tmp_pixmap.scaled(IMAGE_SIZE_OF_TRACK, IMAGE_SIZE_OF_TRACK, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p_listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
                QPixmapCache::insert(p_list_item->at(tmp_index).album_image, tmp_pixmap);
            }
        }
        fileDownload_sender->deleteLater();
    }


    void AbstractCommonSubWidget::ContentLoadingwaitingMsgShow(QString msg){    //cheontidal
        print_debug();
        Q_UNUSED(msg);
        if(!global.window_activate_flag) return;//c220804
        if(global.powerDialogShowFlag) return;//c220804
        print_debug();
        //----------------------------------------------------
        //abs_ani_dialog_wait = new QDialog(this) ;//c220616
        QLabel *lb_Movie = new QLabel();
        //lb_Movie->setVisible(false);
        lb_Movie->setStyleSheet("background-color:transparent;");
        //QMovie *mov = new QMovie("C:/Users/doulz/Rose_Connect-21-12-21-20/images/Spinner-4.2s-200px.gif");
        //abs_ani_mov = new QMovie(":/images/loading.gif");//":/images/Spinner-4.2s-200px.gif"
        QMovie *abs_ani_mov = new QMovie(":/images/Spinner-4.2s-200px.gif");//":/images/Spinner-4.2s-200px.gif"//c220616

        lb_Movie->setMovie(abs_ani_mov);
        //Movie->setGeometry(20, 20, 392, 225);
        abs_ani_mov->setScaledSize(QSize(120,120));
        abs_ani_mov->setBackgroundColor("transparent");

        QHBoxLayout *hl_msgBox = new QHBoxLayout();
        hl_msgBox->setContentsMargins(0,0,0,0);
        hl_msgBox->setSpacing(0);
        hl_msgBox->setContentsMargins(35,20,35,15);
        //hl_msgBox->addStretch(1);
        //hl_msgBox->addWidget(lb_msg);
        hl_msgBox->addWidget(lb_Movie);
        //hl_msgBox->addStretch(1);

        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
        int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
        int left = 0;
        int top = 0;

        if(global.abs_ani_dialog_wait->isHidden() != true){//c220616
            //ContentLoadingwaitingMsgHide();
            return;
        }

        print_debug();
        if(global.abs_ani_dialog_wait->layout()==0){//c220616
            global.abs_ani_dialog_wait->setLayout(hl_msgBox);
            global.abs_ani_dialog_wait->setModal(true);
            global.abs_ani_dialog_wait->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
            global.abs_ani_dialog_wait->setAttribute(Qt::WA_TranslucentBackground);

            //ani_dialog_wait->setFixedSize(300,300);

            //if(global.abs_ani_dialog_wait->layout()==0){//c220616
              //  global.abs_ani_dialog_wait->setLayout(hl_msgBox);
            //}

            left = global.left_mainwindow+global.width_mainwindow/2- (global.abs_ani_dialog_wait->sizeHint().width() / 2);//c220804
            top = global.top_mainwindow+global.height_mainwindow/2- (global.abs_ani_dialog_wait->sizeHint().height() / 2);//c220804
            //left = (latestWidth / 2) - (global.abs_ani_dialog_wait->sizeHint().width() / 2);//c220804
            //top = (latestHeight/ 2) - (global.abs_ani_dialog_wait->sizeHint().height() / 2);//c220804
            global.abs_ani_dialog_wait->move(left, top);//c220804

            //global.abs_ani_dialog_wait->setGeometry(left, top, 0, 0);
            //return;
        }

        //ani_dialog_wait->setGeometry(this->width()-400,300,1000,200);
        global.abs_ani_dialog_wait->show();
        //ani_dialog_wait->raise();

        abs_ani_mov->start();

        QTimer::singleShot(5000, this, [=](){ContentLoadingwaitingMsgHide();});//c220928_1
        print_debug();
    }


    void AbstractCommonSubWidget::ContentLoadingwaitingMsgHide(){

        print_debug();
        if(global.abs_ani_dialog_wait == nullptr) return;//c221004_3
        qDebug() << "abs_ani_dialog_wait->isHidden() = " << global.abs_ani_dialog_wait->isHidden();

        if(global.abs_ani_dialog_wait->isHidden() != true){
            print_debug();
            //abs_ani_mov->stop();
            global.abs_ani_dialog_wait->hide(); //cheontidal
            print_debug();
        }

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
    }

    void AbstractCommonSubWidget::setUIShare(){//c220823

        int dialogWidth = 400;
        dialog_box_share = new QDialog();
        dialog_box_share->setModal(true);
        dialog_box_share->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        dialog_box_share->setAttribute(Qt::WA_TranslucentBackground);
        //dialog_box_share->setFixedSize(250, 350);//200
        //dialog_box_share->setStyleSheet("background-color:transparent;");
        dialog_box_share->setStyleSheet("background-color:#444444;");


        BtnLabel *btn_facebook = new BtnLabel(tr("FaceBook"), "btn_facebook", ":/images/facebook_logo.png", ":/images/facebook_logo.png");
        //QPushButton *btn_facebook = GSCommon::getUIBtnImg("FaceBook", ":images/facebook_logo.png");
                //new BtnLabel(tr("FaceBook"), "btn_facebook", ":/images/facebook_logo.png", ":/images/facebook_logo.png");
        btn_facebook->setFixedSize(80,80);
        // btn_facebook->setProperty("internalMode", 64);
        btn_facebook->setProperty("btnNo", 11);
        btn_facebook->setCursor(Qt::PointingHandCursor);


        QLabel *lb_title = new QLabel;
        lb_title->setText(tr("Share"));//lb_title->setText(tr("클라우드 추가"));
        lb_title->setStyleSheet("color:#FFFFFF;font-size:22px;");
        lb_title->setAlignment(Qt::AlignLeft);
        lb_title->setFixedSize(dialogWidth-80,30);

        QPushButton *btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png");
        btn_close->setCursor(Qt::PointingHandCursor);
        btn_close->setFixedSize(QSize(20,20));
        //btn_close->setParent(lb_title);
        //btn_close->setGeometry(250-70, 0, 70, 70);
        btn_close->show();


        QLineEdit *lb_title_link = new QLineEdit;
        lb_title_link->setStyleSheet("background-color:white;font-size:14px; color:#333333; ");
        lb_title_link->setMaxLength(200);
        lb_title_link->setFixedSize(dialogWidth-120,30);
        lb_title_link->setPlaceholderText(tr(""));

        /*
        //lb_title_link->setTextInteractionFlags(Qt::TextSelectableByMouse);
        lb_title_link->setText(tr("https://"));//lb_title->setText(tr("클라우드 추가"));

        lb_title_link->setStyleSheet("Text-align:Left; color:#cccccc;font-size:18px;background-color:#FFFFFF;");
        lb_title_link->setAlignment(Qt::AlignCenter);
        lb_title_link->setFixedSize(400,30);
 */


        QString btnStyle = "QPushButton{font-size:14px; color:#8E8E8E; padding-left:0px; background-color:transparent; border:1px solid #8E8E8E; border-radius:10px;}";
        btnStyle += "QPushButton:hover{font-size:14px; color:#777777; padding-left:0px; background-color:#777777; border:1px solid #777777; border-radius:10px;}";


        QPushButton *btn_title_link_copy = new QPushButton;
        btn_title_link_copy->setText(tr("Copy"));
        btn_title_link_copy->setCursor(Qt::PointingHandCursor);
        btn_title_link_copy->setStyleSheet("font-size:16px;color:#FFFFFF;background-color:#333333;border-radius:15px;");
        btn_title_link_copy->setFixedSize(60,30);


        QHBoxLayout *hl_btn_link_copy = new QHBoxLayout();
        hl_btn_link_copy->setContentsMargins(0,0,0,0);
        hl_btn_link_copy->setSpacing(30);
        hl_btn_link_copy->addWidget(lb_title_link, 0, Qt::AlignLeft);
        hl_btn_link_copy->addWidget(btn_title_link_copy, 0, Qt::AlignRight);
        //-----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------------
        QHBoxLayout *hl_btn_title = new QHBoxLayout();
        hl_btn_title->setContentsMargins(0,0,0,0);
        hl_btn_title->setSpacing(130);
        hl_btn_title->addWidget(lb_title, 0, Qt::AlignLeft);
        hl_btn_title->addWidget(btn_close, 0, Qt::AlignRight);

        QWidget *widget_title = new QWidget();
        widget_title->setFixedHeight(50);
        widget_title->setLayout(hl_btn_title);

        BtnLabel *btn_twiter = new BtnLabel(tr("Twiter"), "btn_twiter", ":/images/twiter_logo.png", ":/images/twiter_logo.png");
        //QPushButton *btn_twiter = GSCommon::getUIBtnImg("Twiter", ":images/twiter_logo.png");
                //new BtnLabel(tr("Twiter"), "btn_twiter", ":/images/twiter_logo.png", ":/images/twiter_logo.png");
        btn_twiter->setFixedSize(80,80);
        // btn_twiter->setProperty("internalMode", 4);
        btn_twiter->setProperty("btnNo", 12);
        btn_twiter->setCursor(Qt::PointingHandCursor);


        QVBoxLayout *vl_btn_twiter = new QVBoxLayout();
        vl_btn_twiter->setContentsMargins(0,0,0,0);
        vl_btn_twiter->setSpacing(0);

        vl_btn_twiter->addWidget(btn_twiter);
        //----------------------------------------------------------------------------------------
        QHBoxLayout *hl_btn_share = new QHBoxLayout();
        hl_btn_share->setContentsMargins(0,0,20,0);
        hl_btn_share->setSpacing(0);
        hl_btn_share->addWidget(btn_facebook);//, 0, Qt::AlignLeft);
        hl_btn_share->addWidget(btn_twiter);//, 0, Qt::AlignLeft);
        //-----------------------------------------------------------------------------------
        QWidget *line_bottom_1 = new QWidget();
        line_bottom_1->setFixedHeight(1);
        line_bottom_1->setStyleSheet("background-color:#707070;");

        QLabel *msg_box = new QLabel();
        msg_box->setFixedHeight(50);
        msg_box->setText("This function is being prepared\n for service!!!");
        msg_box->setStyleSheet("font-size:20px;color:#ffffff; background-color:transparent;");

        QVBoxLayout *vl_btn_share = new QVBoxLayout();
        vl_btn_share->setContentsMargins(20,0,20,0);
        vl_btn_share->addSpacing(5);
        vl_btn_share->setAlignment(Qt::AlignTop);
        vl_btn_share->addWidget(widget_title, 0, Qt::AlignLeft);
        vl_btn_share->addWidget(line_bottom_1);
        vl_btn_share->addSpacing(15);
        vl_btn_share->addWidget(msg_box);
        vl_btn_share->addLayout(hl_btn_share);
        vl_btn_share->addSpacing(15);
        vl_btn_share->addLayout(hl_btn_link_copy);

        msg_box->hide();


        QWidget *widget_box_btn_share = new QWidget();
        widget_box_btn_share->setObjectName("widget_box_internal_out");
        widget_box_btn_share->setFixedSize(dialogWidth, 260);//200
        // widget_box_btn_share->setStyleSheet("#widget_box_btn_share { border:1px solid #707070; }");
        widget_box_btn_share->setLayout(vl_btn_share);
        //---------------------------------------------------------------------------


        QVBoxLayout *vl_total = new QVBoxLayout();
        vl_total->setContentsMargins(30,10,0,0);
        vl_total->setSpacing(30);

        vl_total->addWidget(widget_box_btn_share, 0, Qt::AlignHCenter);

        dialog_box_share->setLayout(vl_total);


        QString tmpStr = this->shareLink;
        QStringList tmpLink = tmpStr.split("?");
        //this->shareLink = tmpLink.at(0);

       // lb_title_link->setText(tmpStr);
         lb_title_link->setText(tmpLink.at(0));
        lb_title_link->setCursorPosition(0);

        //dialog_box_share->set;
        dialog_box_share->show();

        if(this->shareLink.size() <= 0){

            print_debug();
            btn_facebook->hide();
            btn_twiter->hide();
            lb_title_link->hide();
            btn_title_link_copy->hide();
            msg_box->show();
            //dialog_box_share->hide();
            //global.window_activate_flag = true;
            //ToastMsg::show(this, "", tr("There is no response for the requested content from the server."), 3000);
            //emit linker->signal_checkQueue(this,34, "");
        }else{
            //dialog_box_share->show();
        }

        connect(btn_close, SIGNAL(clicked()), dialog_box_share, SLOT(close()));
        connect(btn_facebook, SIGNAL(clicked()), this, SLOT(slot_faceBookShare()));
        connect(btn_twiter, SIGNAL(clicked()), this, SLOT(slot_twiterShare()));
        connect(btn_title_link_copy, SIGNAL(clicked()), this, SLOT(slot_title_link_copy()));

    }

    void AbstractCommonSubWidget::slot_title_link_copy(){//c220818
        print_debug();
        QClipboard *clipboard = QGuiApplication::clipboard();
        QString tmpStr = this->shareLink;
        QStringList tmpLink = tmpStr.split("?");

        clipboard->setText(tmpLink.at(0));
        dialog_box_share->hide();//c220826_2
    }

    void AbstractCommonSubWidget::slot_faceBookShare(){//c220818
        print_debug();

        QString link = QString("http://www.facebook.com/sharer/sharer.php?u=%1").arg(this->shareLink);
        QDesktopServices::openUrl(QUrl(link));
        dialog_box_share->hide();//c220820

    }

    void AbstractCommonSubWidget::slot_twiterShare(){//c220818
        print_debug();
        qDebug() << "this->shareLink=" << this->shareLink;
        //QString link = QString("https://twitter.com/intent/tweet?text=TEXT&url=%1").arg(this->shareLink);
        //QString link = QString("https://twitter.com/intent/tweet?text=%1&url=%2").arg(this->shareLink.split("https://").last()).arg(this->shareLink);
        QString link = QString("https://twitter.com/intent/tweet?text=%1&url=%2").arg(this->shareLink).arg(this->shareLink);

        //QString link = QString("https://twitter.com/intent/tweet?text=%1").arg(this->shareLink);
        QDesktopServices::openUrl(QUrl(link));
        dialog_box_share->hide();//c220820

    }



};
