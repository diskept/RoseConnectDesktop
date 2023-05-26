#include "music/MusicAlbum.h"

#include "roseHome/ConvertData_rosehome.h"
#include "roseHome/ProcCommon_forRosehome.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"

#include "widget/NoData_Widget.h"
#include "widget/optionpopup.h"

#include <QResizeEvent>
#include <QScrollBar>


namespace music {

    const int SECTION_FOR_MORE_POPUP___albums = 0;

    const int HTTP_NETWORK_PLAY = 99;

    /**
     * @brief AlbumListAll::AlbumListAll
     * @param parent
     */
    MusicAlbum::MusicAlbum(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }


    /**
     * @brief 소멸자.
     */
    MusicAlbum::~MusicAlbum(){

        delete settings;
        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void MusicAlbum::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpPage = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(this->curr_page != tmpPage){
            this->flagNeedReload = true;

            this->curr_page = tmpPage;

            // init
            this->next_offset = 0;
            this->album_total_cnt = 0;
            this->album_draw_cnt = 0;

            this->list_album->clear();

            this->jsonArr_Album = QJsonArray();

            this->flag_flow_draw = false;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
        }
        else{
            print_debug();ContentLoadingwaitingMsgHide();   //j230328

            // flowlayout spacing change - by diskept j230317 start
            this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
            // flowlayout spacing change - by diskept j230317 finish
        }
    }


    /**
     * @brief 페이지 활성화를 처리하는 함수임.
     * HTTP API 호출한다.
     */
    void MusicAlbum::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_albums);

            this->album_total_cnt = this->getAlbumDataFromDBforTotalCount();
        }
    }



    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void MusicAlbum::setUIControl_albums(){

        this->settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

        btnStyle_normal = "QPushButton{ ";
        btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:16px;";
        btnStyle_normal += "border:1px solid #707070; border-radius:15px;}";
        btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:16px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:15px;}";

        this->orderTypeList << "A-Z"  << "Z-A" << tr("Artist");
        this->fileTypeList << tr("All") << tr("Lossless"); /// << "DSD64x" << "DSD128x" << "DSD256x" << "DSD512x" << "MQA";
        this->bitDepthList << tr("All") << "24bit↑";          /// << "16bit↑" << "24bit↑";
        this->samplingList << tr("All") << "88.2kHz↑" << "176.4kHz↑";  /// <<"44.1kHz↑" << "88.2kHz↑" << "176.4kHz↑" << "352.8kHz↑";

        this->setUIControl_button();
        this->setUIControl_filter();

        // get widget width, right margin - by diskept j230317 start
        GSCommon::clearLayout(this->box_contents);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] MusicAlbum::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;
        //  get widget width, right margin - by diskept j230317 finish

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }


    void MusicAlbum::setUIControl_button(){

        // 상단 필터
        this->widget_btnFilter = new QWidget();
        this->widget_btnFilter->setStyleSheet("background-color: transparent;");
        this->widget_btnFilter->setFixedSize(global.LmtCnt - 180, 75);

        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png", this->widget_btnFilter);
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);
        this->btn_filter_ico->setGeometry(3, 2, 0, 0);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));


        this->lb_total_count = new QLabel(this->widget_btnFilter);
        this->lb_total_count->setAlignment(Qt::AlignRight);
        this->lb_total_count->setStyleSheet("background-color:transparent; font-size:18px; font-weight:normal; color:#FFFFFF;");
        this->lb_total_count->setText(this->albumCount);
        this->lb_total_count->setGeometry(global.LmtCnt - this->lb_total_count->sizeHint().width() - 20, 27, this->lb_total_count->sizeHint().width(), 27);

        this->flow_select_filter = new FlowLayout(0, 0, 10, 5);
        this->flow_select_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_select_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_select_filter = new QWidget(this->widget_btnFilter);
        this->widget_select_filter->setStyleSheet("background:transparent;");
        this->widget_select_filter->setLayout(this->flow_select_filter);
        this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (70 + this->lb_total_count->sizeHint().width()), 53);

        this->box_filter->addWidget(this->widget_btnFilter);

        int orderType = this->settings->value(rosesettings.SETTINGS_AFILTER_ORDER, 0).toInt();
        if(orderType > 2){
            orderType = 0;
            this->settings->setValue(rosesettings.SETTINGS_AFILTER_ORDER, orderType);
        }

        int fileType = this->settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE, 0).toInt();
        if(fileType > 1){
            fileType = 0;
            this->settings->setValue(rosesettings.SETTINGS_AFILTER_FILETYPE, fileType);
        }

        int bitDepth = this->settings->value(rosesettings.SETTINGS_AFILTER_BIT, 0).toInt();
        if(bitDepth > 1){
            bitDepth = 0;
            this->settings->setValue(rosesettings.SETTINGS_AFILTER_BIT, bitDepth);
        }

        int sampling = this->settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING, 0).toInt();
        if(sampling > 1){
            sampling = 0;
            this->settings->setValue(rosesettings.SETTINGS_AFILTER_SAMPLEING, sampling);
        }

        this->lb_order_type  = new QLabel;
        this->lb_order_type->setFixedHeight(30);
        this->lb_order_type->setContentsMargins(20, 0, 20, 0);
        this->lb_order_type->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
        this->lb_order_type->setAlignment(Qt::AlignCenter);

        if(orderType >= 0){
            this->lb_order_type->setText(orderTypeList.at(orderType));

            this->flow_select_filter->addWidget(this->lb_order_type);

            this->flag_order_type = false;
        }
        this->before_order_type = orderType;

        this->lb_file_type = new QLabel;
        this->lb_file_type->setFixedHeight(30);
        this->lb_file_type->setContentsMargins(20, 0, 20, 0);
        this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
        this->lb_file_type->setAlignment(Qt::AlignCenter);

        if(fileType > 0){
            this->lb_file_type->setText(fileTypeList.at(fileType));

            this->flow_select_filter->addWidget(this->lb_file_type);

            this->flag_file_type = false;
        }
        this->before_file_type = fileType;

        this->lb_bit_depth = new QLabel;
        this->lb_bit_depth->setFixedHeight(30);
        this->lb_bit_depth->setContentsMargins(20, 0, 20, 0);
        this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
        this->lb_bit_depth->setAlignment(Qt::AlignCenter);

        if(bitDepth > 0){
            this->lb_bit_depth->setText(bitDepthList.at(bitDepth));

            this->flow_select_filter->addWidget(this->lb_bit_depth);

            this->flag_bit_depth = false;
        }
        this->before_bit_depth = bitDepth;

        this->lb_sample_rate = new QLabel;
        this->lb_sample_rate->setFixedHeight(30);
        this->lb_sample_rate->setContentsMargins(20, 0, 20, 0);
        this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
        this->lb_sample_rate->setAlignment(Qt::AlignCenter);

        if(sampling > 0){
            this->lb_sample_rate->setText(samplingList.at(sampling));

            this->flow_select_filter->addWidget(this->lb_sample_rate);

            this->flag_sample_rate = false;
        }
        this->before_sample_rate = sampling;
    }


    void MusicAlbum::setUIControl_filter(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(620, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_filter = new QPushButton(widget_header);
        btn_close_filter->setFixedSize(620, 50);
        btn_close_filter->setStyleSheet("background-color:transparent; border:0px;");
        btn_close_filter->setGeometry(0, 0, 0, 0);

        connect(btn_close_filter, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QLabel *lb_orderType = new QLabel(btn_close_filter);
        lb_orderType->setText("Order Type");
        lb_orderType->setFixedSize(125, 40);
        lb_orderType->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_orderType->setAlignment(Qt::AlignCenter);
        lb_orderType->setGeometry(20, 5, 0, 0);

        QLabel *lb_fileType = new QLabel(btn_close_filter);
        lb_fileType->setText("File Type");
        lb_fileType->setFixedSize(125, 40);
        lb_fileType->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_fileType->setAlignment(Qt::AlignCenter);
        lb_fileType->setGeometry(165, 5, 0, 0);

        QLabel *lb_bitDepth = new QLabel(btn_close_filter);
        lb_bitDepth->setText("Bit Depth");
        lb_bitDepth->setFixedSize(125, 40);
        lb_bitDepth->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_bitDepth->setAlignment(Qt::AlignCenter);
        lb_bitDepth->setGeometry(310, 5, 0, 0);

        QLabel *lb_sampling = new QLabel(btn_close_filter);
        lb_sampling->setText("Sampling Rate");
        lb_sampling->setFixedSize(125, 40);
        lb_sampling->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_sampling->setAlignment(Qt::AlignCenter);
        lb_sampling->setGeometry(465, 5, 0, 0);

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);


        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(620, 200);
        widget_body->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        for(int i = 0; i < this->orderTypeList.count(); i++){
            this->checkbox_order_type[i] = new QCheckBox(widget_body);
            this->checkbox_order_type[i]->setFixedSize(125, 40);
            this->checkbox_order_type[i]->setGeometry(20, (20 * (i+1)) + (40 * i), 0, 0);
            this->checkbox_order_type[i]->setProperty("type", "orderTypeList");
            this->checkbox_order_type[i]->setProperty("idx", i);
            this->checkbox_order_type[i]->setText(this->orderTypeList.at(i));
            this->checkbox_order_type[i]->setStyleSheet("QCheckBox {color:#FFFFFF; font-size:16px; background:transparent; border:0px;}"
                                                        "QCheckBox::indicator {width:50px; height:50px;}"
                                                        "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                        "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

            connect(this->checkbox_order_type[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_choose()));

        }

        for(int i = 0; i < this->fileTypeList.count(); i++){
            this->checkbox_file_type[i] = new QCheckBox(widget_body);
            this->checkbox_file_type[i]->setFixedSize(125, 40);
            this->checkbox_file_type[i]->setGeometry(165, (20 * (i+1)) + (40 * i), 0, 0);
            this->checkbox_file_type[i]->setProperty("type", "fileTypeList");
            this->checkbox_file_type[i]->setProperty("idx", i);
            this->checkbox_file_type[i]->setText(this->fileTypeList.at(i));
            this->checkbox_file_type[i]->setStyleSheet("QCheckBox {color:#FFFFFF; font-size:16px; background:transparent; border:0px;}"
                                                        "QCheckBox::indicator {width:50px; height:50px;}"
                                                        "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                        "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

            connect(this->checkbox_file_type[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_choose()));

        }

        for(int i = 0; i < this->bitDepthList.count(); i++){
            this->checkbox_bit_depth[i] = new QCheckBox(widget_body);
            this->checkbox_bit_depth[i]->setFixedSize(125, 40);
            this->checkbox_bit_depth[i]->setGeometry(310, (20 * (i+1)) + (40 * i), 0, 0);
            this->checkbox_bit_depth[i]->setProperty("type", "bitDepthList");
            this->checkbox_bit_depth[i]->setProperty("idx", i);
            this->checkbox_bit_depth[i]->setText(this->bitDepthList.at(i));
            this->checkbox_bit_depth[i]->setStyleSheet("QCheckBox {color:#FFFFFF; font-size:16px; background:transparent; border:0px;}"
                                                        "QCheckBox::indicator {width:50px; height:50px;}"
                                                        "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                        "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

            connect(this->checkbox_bit_depth[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_choose()));
        }

        for(int i = 0; i < this->samplingList.count(); i++){
            this->checkbox_sample_rate[i] = new QCheckBox(widget_body);
            this->checkbox_sample_rate[i]->setFixedSize(140, 40);
            this->checkbox_sample_rate[i]->setGeometry(455, (20 * (i+1)) + (40 * i), 0, 0);
            this->checkbox_sample_rate[i]->setProperty("type", "samplingList");
            this->checkbox_sample_rate[i]->setProperty("idx", i);
            this->checkbox_sample_rate[i]->setText(this->samplingList.at(i));
            this->checkbox_sample_rate[i]->setStyleSheet("QCheckBox {color:#FFFFFF; font-size:16px; background:transparent; border:0px;}"
                                                        "QCheckBox::indicator {width:50px; height:50px;}"
                                                        "QCheckBox::indicator:checked {image:url(:/images/playlist/list_check_on.png);}"
                                                        "QCheckBox::indicator:unchecked {image:url(:/images/playlist/list_check_off.png);}");

            connect(this->checkbox_sample_rate[i], SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_choose()));
        }

        QHBoxLayout *box_body = new QHBoxLayout;
        box_body->setContentsMargins(0, 0, 0, 0);
        box_body->setSpacing(0);
        box_body->addWidget(widget_body);


        QWidget *widget_tail = new QWidget();
        widget_tail->setFixedSize(620, 60);
        widget_tail->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_apply_filter = new QPushButton(widget_tail);
        btn_apply_filter->setStyleSheet("QPushButton{background:transparent;color:#FFFFFF;font-size:16px;border:1px solid #707070; border-radius:15px;} QPushButton:hover{background-color:#BE905F;}");
        btn_apply_filter->setText(tr("Apply"));
        btn_apply_filter->setFixedSize(130, 40);
        btn_apply_filter->setGeometry(165, 10, 0, 0);

        connect(btn_apply_filter, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_apply()));

        QPushButton *btn_cancel_filter = new QPushButton(widget_tail);
        btn_cancel_filter->setStyleSheet("QPushButton{background:transparent;color:#FFFFFF;font-size:16px;border:1px solid #707070; border-radius:15px;} QPushButton:hover{background-color:#BE905F;}");
        btn_cancel_filter->setText(tr("Cancel"));
        btn_cancel_filter->setFixedSize(130, 40);
        btn_cancel_filter->setGeometry(335, 10, 0, 0);

        connect(btn_cancel_filter, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_cancel()));

        QHBoxLayout *box_tail = new QHBoxLayout;
        box_tail->setContentsMargins(0, 0, 0, 0);
        box_tail->setSpacing(0);
        box_tail->addWidget(widget_tail);


        QVBoxLayout *box_total = new QVBoxLayout;
        box_total->setContentsMargins(0, 0, 0, 0);
        box_total->setSpacing(0);
        box_total->setAlignment(Qt::AlignTop);
        box_total->addLayout(box_header);
        box_total->addLayout(box_body);
        box_total->addLayout(box_tail);

        this->widget_filter = new QWidget(this);
        this->widget_filter->setObjectName("widget_filter");
        this->widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
        this->widget_filter->setFixedSize(620, 310);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setFixedSize(620, 310);
        btn_total->setGeometry(0, 0, 0, 0);
        btn_total->setLayout(box_total);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void MusicAlbum::initAll(){

        this->list_album->clear();

        this->album_total_cnt = 0;
        this->album_draw_cnt = 0;

        this->flag_album_draw = false;

        this->jsonArr_Album = QJsonArray();

        GSCommon::clearLayout(this->flowLayout_albums);

        this->album_total_cnt = this->getAlbumDataFromDBforTotalCount();

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void MusicAlbum::proc_wheelEvent_to_getMoreData(){

        // wheel evnet change - by diskept j230317 start
        if((this->album_total_cnt > this->list_album->count()) && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumData();
        }
        else if((this->album_total_cnt > this->album_draw_cnt) && this->flag_album_draw == false && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
        // wheel evnet change - by diskept j230317 start
    }


    void MusicAlbum::request_more_albumData(){

        // next_offset
        if(this->list_album->count() == 0){
            this->next_offset = 0;
        }

        int limit_count = (this->album_total_cnt - this->list_album->count()) > 500 ? 500 : (this->album_total_cnt - this->list_album->count());

        // DB 쿼리
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        QList<roseHome::AlbumItemData> tmp_list_album;
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";

            strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
            strQuery += " FROM album_info AS AI ";
            strQuery += " LEFT OUTER JOIN album_art AS AA ";
            strQuery += " ON AI._id=AA.album_id ";

            QString setWhere = "";
            QString setOrder = "";

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

            /*if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
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
            }*/

            if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 0){
                setOrder += " ORDER BY AI.album ASC";
            }
            else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
                setOrder += " ORDER BY AI.album DESC";
            }
            else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
                setOrder += " ORDER BY AI.artist ";
            }

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }

            if(!setOrder.isEmpty()){
                strQuery += setOrder;
            }

            strQuery += QString(" LIMIT %1 ").arg(limit_count) + QString("OFFSET %1 ").arg(this->next_offset);

            this->next_offset += limit_count;

            //qDebug() << "[Debug] MusicAlbum::request_more_albumData " << strQuery;

            QVariantList data;
            sqlite->exec(strQuery, data);

            if(data.size() > 0){
                for(int i = 0 ; i < data.size(); i++){
                    this->jsonArr_Album.append(data.at(i).toJsonObject());

                    QMap<QString, QVariant> map = data.at(i).toMap();

                    QString tmpMime = map["mime_type"].toString();
                    int tmpbitDepth = map["bitdepth"].toInt();
                    int tmpsampleRate = map["samplerate"].toInt();

                    if(!tmpMime.isEmpty()){
                        QString tmp_data = tmpMime;
                        QStringList splitToken = tmp_data.split("/");

                        if(splitToken.size() > 1){
                            QString tmp_type = splitToken.at(1);
                            if(tmp_type == "x-wav" || tmp_type == "wav"){
                                tmpMime = "WAV";
                            }
                            else if (tmp_type == "wv") {
                                tmpMime = "WV";
                            }
                            else if(tmp_type == "flac"){
                                tmpMime = "FLAC";
                            }
                            else if(tmp_type == "dff" || tmp_type == "dsf"){
                                tmpMime = "DSD";
                            }
                            else if(tmp_type == "mp4"){
                                tmpMime = "ALAC";
                            }
                            else if(tmp_type == "mpeg"){
                                tmpMime = "MP3";
                            }
                            else if(tmp_type == "amr"){
                                tmpMime = "AMR";
                            }
                            else if(tmp_type == "amr-wb"){
                                tmpMime = "AWB";
                            }
                            else if(tmp_type == "x-ms-wma"){
                                tmpMime = "WMA";
                            }
                            else if(tmp_type == "ape"){
                                tmpMime = "APE";
                            }
                            else if(tmp_type == "ogg"){
                                tmpMime = "OGG";
                            }
                            else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                                tmpMime = "AAC";
                            }
                            else if(tmp_type == "x-matroska"){
                                tmpMime = "MKA";
                            }
                            else if(tmp_type == "midi"){
                                tmpMime = "MIDI";
                            }
                            else if(tmp_type == "sp-midi"){
                                tmpMime = "SMF";
                            }
                            else if(tmp_type == "imelody"){
                                tmpMime = "IMY";
                            }
                            else if(tmp_type == "x-mpegurl"){
                                tmpMime = "M3U8";
                            }
                            else if(tmp_type == "x-scpls"){
                                tmpMime = "PLS";
                            }
                            else if(tmp_type == "aiff"){
                                tmpMime = "AIFF";
                            }
                        }
                    }

                    QString mimeType = "";
                    if(tmpMime == "MP3" || tmpMime == "AMR" || tmpMime == "AWB" || tmpMime == "WMA" || tmpMime == "APE" || tmpMime == "OGG" || tmpMime == "AAC" || tmpMime == "MKA"
                            || tmpMime == "MIDI" || tmpMime == "SMF" || tmpMime == "IMY" || tmpMime == "M3U8" || tmpMime == "PLS" || tmpMime == "AIFF" || tmpMime == "WV"){

                        mimeType = tmpMime;
                    }
                    else if(tmpMime == "DSD"){
                        if(tmpsampleRate == 88200 || tmpsampleRate == 2822400){
                            mimeType = tmpMime + " 64";
                        }
                        else if(tmpsampleRate == 176400 || tmpsampleRate == 5644800){
                            mimeType = tmpMime + " 128";
                        }
                        else if(tmpsampleRate == 352800 || tmpsampleRate == 11289600){
                            mimeType = tmpMime + " 256";
                        }
                        else if(tmpsampleRate == 705600 || tmpsampleRate == 22579200){
                            mimeType = tmpMime + " 512";
                        }
                        else if(tmpsampleRate == 1411200 || tmpsampleRate == 45158400){
                            mimeType = tmpMime + " 1024";
                        }
                        else{
                            mimeType = tmpMime;
                        }
                    }
                    else if(tmpMime == "WAV" || tmpMime == "FLAC" || tmpMime == "ALAC"){
                        if(tmpbitDepth == 0 || tmpbitDepth == 16){
                            mimeType = tmpMime;
                        }
                        else if(tmpbitDepth > 16){
                            mimeType = tmpMime + QString(" %1").arg(tmpbitDepth);
                        }
                    }

                    QString tmpImg = map["album_art"].toString();
                    if(!tmpImg.isEmpty()){
                        tmpImg = "http://" + global.device.getDeviceIP() + ":" + global.port_img + map["album_art"].toString();
                    }

                    roseHome::AlbumItemData data_output;
                    data_output.id = map["album_id"].toInt();
                    data_output.trackCount = map["num_of_song"].toInt();
                    data_output.totalCount = data.size();

                    data_output.type = "MUSIC";
                    data_output.title = map["album"].toString();
                    data_output.artist = map["artist"].toString();
                    data_output.playTime = map["album_art"].toString();
                    data_output.thumbnail = tmpImg;
                    data_output.mime = mimeType;

                    tmp_list_album.append(data_output);
                }
            }
        }

        sqlite->close();
        delete sqlite;

        this->slot_applyResult_albums(tmp_list_album, QJsonArray(), true);
    }


    void MusicAlbum::request_more_albumDraw(){

        // widget draw change - by diskept j230317 start
        this->flag_album_draw = true;

        int f_width = this->flowLayout_albums->geometry().width();

        if(this->flowLayout_albums->geometry().width() <= 0){
            f_width = this->width() - (80 + 63) - 10;
        }

        int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
        int f_mod = this->album_draw_cnt % f_wg_cnt;

        if(f_mod == 0){
            this->album_widget_cnt = f_wg_cnt * 30;
        }
        else{
            this->album_widget_cnt = f_wg_cnt * 30 + (f_wg_cnt - f_mod);
        }

        //qDebug() << "[Debug] MusicAlbum::request_more_albumDraw " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        this->albumCount = tr("Albums") + " " + QString(" %1").number(this->album_draw_cnt) + " / " + QString("%1").number(this->album_total_cnt);
        this->lb_total_count->setText(this->albumCount);
        this->lb_total_count->setGeometry(this->widget_btnFilter->geometry().width() - this->lb_total_count->sizeHint().width() - 20, 27, this->lb_total_count->sizeHint().width(), 27);
        this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (70 + this->lb_total_count->geometry().width()), 53);

        //qDebug() << "[Debug] MusicAlbum::request_more_albumDraw " << start_index << max_cnt << this->album_draw_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            //QCoreApplication::processEvents();
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicAlbum::slot_clickedItemAlbum);
        }

        ContentLoadingwaitingMsgHide();

        this->flag_album_draw = false;
        // widget draw change - by diskept j230317 finish
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

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }

            QVariantList data;
            sqlite->exec(strQuery, data);
            ret_cnt = data.size();
        }
        sqlite->close();
        delete sqlite;

        return ret_cnt;
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] API Response 에 대해서, 화면에 UI Drawing
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] [Override]
     * @param list_data
     */
    void MusicAlbum::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);
        Q_UNUSED(flag_lastPage);

        //qDebug() << "[Debug] MusicAlbum::slot_applyResult_albums " << list_data.count();

        if(list_data.size() > 0){

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0)
            {
                // widget draw change - by diskept j230317 start
                int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 30;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 30 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] MusicAlbum::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);

                if(max_cnt > list_data.count()){
                    max_cnt = list_data.count();
                }
                this->album_draw_cnt += max_cnt;

                //qDebug() << "[Debug] MusicAlbum::slot_applyResult_albums " << this->album_draw_cnt << this->list_album->count();

                this->albumCount = tr("Albums") + " " + QString("%1").number(this->album_draw_cnt) + " / " + QString("%1").number(this->album_total_cnt);
                this->lb_total_count->setText(this->albumCount);
                this->lb_total_count->setGeometry(this->widget_btnFilter->geometry().width() - this->lb_total_count->sizeHint().width() - 20, 27, this->lb_total_count->sizeHint().width(), 27);
                this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (70 + this->lb_total_count->geometry().width()), 53);

                //qDebug() << "[Debug] MusicAlbum::slot_applyResult_albums " << start_index << this->album_draw_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    //QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicAlbum::slot_clickedItemAlbum);
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                }
                // widget draw change - by diskept j230317 finish

                ContentLoadingwaitingMsgHide();

                this->flag_flow_draw = true;
                this->flag_album_draw = false;
            }
            else{
                this->request_more_albumDraw();
            }
        }
        else{
            ContentLoadingwaitingMsgHide();

            // noData widget change - by diskept j230317 start
            int f_width = this->flowLayout_albums->geometry().width();

            if(this->flowLayout_albums->geometry().width() <= 0){
                f_width = this->width() - (80 + 63) - 10;
            }

            NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
            noData_widget->setFixedSize(f_width, 500);
            // noData widget change - by diskept j230317 finish

            this->flowLayout_albums->addWidget(noData_widget);
        }
    }


    void MusicAlbum::slot_clickBtn_Filter(){

        int idx = -1;

        for(int i = 0; i < this->orderTypeList.count(); i++){
            this->checkbox_order_type[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_AFILTER_ORDER, 0).toInt();
        this->checkbox_order_type[idx]->setCheckState(Qt::CheckState::Checked);

        for(int i = 0; i < this->fileTypeList.count(); i++){
            this->checkbox_file_type[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE, 0).toInt();
        this->checkbox_file_type[idx]->setCheckState(Qt::CheckState::Checked);

        for(int i = 0; i < this->bitDepthList.count(); i++){
            this->checkbox_bit_depth[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_AFILTER_BIT, 0).toInt();
        this->checkbox_bit_depth[idx]->setCheckState(Qt::CheckState::Checked);

        for(int i = 0; i < this->samplingList.count(); i++){
            this->checkbox_sample_rate[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING, 0).toInt();
        this->checkbox_sample_rate[idx]->setCheckState(Qt::CheckState::Checked);

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void MusicAlbum::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void MusicAlbum::slot_clickBtn_Filter_choose(){

        QString type = sender()->property("type").toString();
        int idx = sender()->property("idx").toInt();

        if(type == "orderTypeList") {

            for(int i = 0; i < this->orderTypeList.count(); i++){
                this->checkbox_order_type[i]->setCheckState(Qt::CheckState::Unchecked);
            }

            this->checkbox_order_type[idx]->setCheckState(Qt::CheckState::Checked);
        }

        if(type == "fileTypeList"){

            for(int i = 0; i < this->fileTypeList.count(); i++){
                this->checkbox_file_type[i]->setCheckState(Qt::CheckState::Unchecked);
            }

            this->checkbox_file_type[idx]->setCheckState(Qt::CheckState::Checked);
        }

        if(type == "bitDepthList"){

            for(int i = 0; i < this->bitDepthList.count(); i++){
                this->checkbox_bit_depth[i]->setCheckState(Qt::CheckState::Unchecked);
            }

            this->checkbox_bit_depth[idx]->setCheckState(Qt::CheckState::Checked);
        }

        if(type == "samplingList"){

            for(int i = 0; i < this->samplingList.count(); i++){
                this->checkbox_sample_rate[i]->setCheckState(Qt::CheckState::Unchecked);
            }

            this->checkbox_sample_rate[idx]->setCheckState(Qt::CheckState::Checked);
        }
    }


    void MusicAlbum::slot_clickBtn_Filter_apply(){

        int orderType = -1;
        int fileType = -1;
        int bitDepth = -1;
        int sampling = -1;

        for(int i = 0; i < this->orderTypeList.count(); i++){
            if(this->checkbox_order_type[i]->checkState() == Qt::CheckState::Checked){
                orderType = i;
                break;
            }
        }

        if(orderType > 2){
            orderType = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_AFILTER_ORDER, orderType);

        for(int i = 0; i < this->fileTypeList.count(); i++){
            if(this->checkbox_file_type[i]->checkState() == Qt::CheckState::Checked){
                fileType = i;
                break;
            }
        }

        if(fileType > 1){
            fileType = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_AFILTER_FILETYPE, fileType);

        for(int i = 0; i < this->bitDepthList.count(); i++){
            if(this->checkbox_bit_depth[i]->checkState() == Qt::CheckState::Checked){
                bitDepth = i;
                break;
            }
        }

        if(bitDepth > 1){
            bitDepth = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_AFILTER_BIT, bitDepth);

        for(int i = 0; i < this->samplingList.count(); i++){
            if(this->checkbox_sample_rate[i]->checkState() == Qt::CheckState::Checked){
                sampling = i;
                break;
            }
        }

        if(sampling > 1){
            sampling = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_AFILTER_SAMPLEING, sampling);

        this->widget_filter->setVisible(false);

        //qDebug() << "[Debug]" << __FUNCTION__ << __LINE__ << this->before_order_type << orderType << this->before_file_type << fileType << this->before_bit_depth << bitDepth << this->before_sample_rate << sampling;

        if(this->before_order_type != orderType || this->before_file_type != fileType || this->before_bit_depth != bitDepth || this->before_sample_rate != sampling){

            this->before_order_type = orderType;
            this->before_file_type = fileType;
            this->before_bit_depth = bitDepth;
            this->before_sample_rate = sampling;

            GSCommon::clearLayout(this->box_filter);
            this->setUIControl_button();

            this->initAll();
            this->setActivePage();
            this->scrollArea_main->verticalScrollBar()->setValue(0);

            this->request_more_albumData();
        }
    }


    void MusicAlbum::slot_clickBtn_Filter_cancel(){

        this->widget_filter->setVisible(false);
    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // [Slot] Item 의 custom 클릭 siganl에 대한 처리  (AbstractItem 상속받은 자식 클래스들에 대해)
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief [slot] override - ItemAlbum 위짓의 clicked 이벤트를 처리하는 슬롯함수 재정의
     * @param clickMode
     */
    void MusicAlbum::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

        int index = ((tidal::AbstractItem*)sender())->index();
        int section = ((tidal::AbstractItem*)sender())->section();

        // ClickMode 별로 처리
        roseHome::AlbumItemData data_album = this->list_album->at(index);

        if(clickMode == tidal::AbstractItem::ClickMode::MoreBtn){
            // OptMorePopup 띄우기
            this->makeObj_optMorePopup(OptMorePopup::Rosehome_Album, roseHome::ConvertData::getConvertOptHeaderData(data_album), index, section, true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::PlayBtn){

            QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);
            // Album 바로 재생
            QJsonObject tmp_json;
            tmp_json.insert("musicPlayType", 15);
            tmp_json.insert("music", jsonArr_tracks);
            tmp_json.insert("currentPosition", 0);
            tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

            /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
            QJsonDocument doc(tmp_json);
            QString strJson(doc.toJson(QJsonDocument::Compact));
            qDebug() << "jsonObj = " << strJson;*/

            if(global.Queue_track_count != 0) {
                emit linker->signal_checkQueue(27, "");

                return;
            }
            emit linker->signal_queuelist_mouse_trigger_menu_flag();
            global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_NETWORK_PLAY
                             , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song")
                             , tmp_json
                             , true
                             , true);
        }
        else if(clickMode == tidal::AbstractItem::ClickMode::AllBox){

            // Album Detail 페이지 진입
            QJsonObject jsonData;
            jsonData.insert(KEY_OP_TITLE_MAIN, data_album.title);
            jsonData.insert(KEY_OP_TITLE_SUB, data_album.artist);
            jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Album);
            jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
            jsonData.insert(KEY_OP_MIME, data_album.mime);
            jsonData.insert(KEY_OP_ARTIST, data_album.artist);
            jsonData.insert(KEY_OP_cntStar, 0);
            jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));
            jsonData.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);
            jsonData.insert("isAlbum", true);

            jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ALBUMINFO);

            emit linker->signal_clickedMovePage(jsonData);
        }
    }


    QJsonArray MusicAlbum::getDataForPlayMusic(const int album_id){

        QJsonArray jsonArray;

        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery += " SELECT A.album, A.album_key, A.artist_key, A.artist_id, A.album_id, A._id AS id, A._display_name AS orderName, A._data AS data, A.title, A.artist, A.duration, A.bookmark, A.track, A.mime_type, A.samplerate, A.bitdepth, ART._data AS album_art ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS ART ON A.album_id=ART.album_id ";
            strQuery += " WHERE A.album_id=%1 ORDER BY A.bookmark ASC, A.track ASC, orderName ASC ";

            QVariantList dataDB;
            sqlite->exec(strQuery.arg(album_id), dataDB);
            if(dataDB.size() > 0){
                for(int i = 0 ; i < dataDB.size(); i++){
                    DataPopup *tmp_dataPopup = new DataPopup(this);
                    tmp_dataPopup->setJsonData(dataDB.at(i).toJsonObject());

                    jsonArray.append(tmp_dataPopup->getJsonData());
                }
            }
        }

        sqlite->close();
        delete sqlite;

        return jsonArray;
    }


    /**
     * @brief OptMorePopup 의 custom signal에 대한 처리 [Overriding]
     * @param clickMode
     * @param index
     * @param section
     */
    void MusicAlbum::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

        Q_UNUSED(clickMode);
        Q_UNUSED(index);

        if(section == SECTION_FOR_MORE_POPUP___albums){
            //this->proc_clicked_optMorePopup_fromAlbum(this->list_album, index, clickMode);

            roseHome::AlbumItemData data_album = this->list_album->at(index);

            if(clickMode == OptMorePopup::ClickMode::Play_RightNow
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty
                    || clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext
            )
            {
                int playType = 0;

                if(clickMode == OptMorePopup::ClickMode::Play_RightNow){
                    playType = 15;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Last){
                    playType = 13;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_Empty){
                    playType = 12;
                }
                else if(clickMode == OptMorePopup::ClickMode::SubMenu_QueueAdd_CurrNext){
                    playType = 16;
                }
                else{
                    playType = 15;
                }

                // Album 바로 재생
                QJsonArray jsonArr_tracks = this->getDataForPlayMusic(data_album.id);

                // Album 바로 재생
                QJsonObject tmp_json;
                tmp_json.insert("musicPlayType", playType);
                tmp_json.insert("music", jsonArr_tracks);
                tmp_json.insert("currentPosition", 0);
                tmp_json.insert("roseToken", global.device.getDeviceRoseToken());

                /*qDebug() << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song");
                QJsonDocument doc(tmp_json);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                qDebug() << "jsonObj = " << strJson;*/

                if(global.Queue_track_count != 0) {
                    print_debug();emit linker->signal_checkQueue(27, "");

                    return;
                }
                print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
                global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

                NetworkHttp *network = new NetworkHttp;
                connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                network->request(0, QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("music_song"), tmp_json, true, true);
            }
            else if(clickMode == OptMorePopup::ClickMode::Go_Artist){
                // artist Detail 페이지 진입
                QJsonObject jsonData;
                jsonData.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Etc_ArtistDetailTrack);
                jsonData.insert(KEY_OP_albumImg, data_album.thumbnail);
                jsonData.insert(KEY_OP_DATA, this->getDataForPlayMusic(data_album.id));

                jsonData.insert(KEY_PAGE_CODE, PAGECODE_OP_ARTISTINFO);

                emit this->linker->signal_clickedMovePage(jsonData);
            }
        }
    }


    void MusicAlbum::resizeEvent(QResizeEvent *event){//c230304

        Q_UNUSED(event);

        //qDebug() << "[Debug] MusicAlbum::resizeEvent " << this->width();

        this->widget_btnFilter->setFixedSize(this->width() - (80 + 63) - 10, 75);
        this->lb_total_count->setGeometry(this->widget_btnFilter->geometry().width() - this->lb_total_count->sizeHint().width() - 20, 27, this->lb_total_count->sizeHint().width(), 27);
        this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (70 + this->lb_total_count->sizeHint().width()), 53);

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        if(this->flag_flow_draw == false){
            this->flag_flow_draw = true;

            this->request_more_albumData();
        }
    }

}





//#include "musicalbum.h"
//#include "widget/toastmsg.h"

//#include <QThread>//cheon210608-test
//#include <QLabel>
//#include <QHBoxLayout>
//#include <QPushButton>
//#include <QJsonObject>
//#include <QPainter>
//#include <QWheelEvent>
//#include <QScrollBar>

//#include <QCoreApplication>
//#include <QIntValidator>//cheon210717-search
//#include <common/gscommon.h>
//#include <common/global.h>
//#include <common/sqlitehelper.h>
//#include <common/rosesettings.h>
//#include <QJsonDocument>
//#include <widget/flowlayout.h>
//#include <common/networkhttp.h>
//#include <QtWidgets/qabstractslider.h>

////binarycreator.exe -c config\config.xml -p packages Rose_Connect_setup_4.1.7.4.exe

//const char* KEY_AFILTER = "filter";
//const char* KEY_AIDX = "idx";
//int PAGE_LMT = 100;//22040301
//const int LEFT_WIDTH = 240;//158;//125+33
//const int ALBUMWIDTH = 204;//186+18


///**
// * @brief MusicAlbum::MusicAlbum : 음악-앨범
// * @details 음악을 앨범으로 분류해 화면에 그린다.
// * @param parent QWidget
// */
//MusicAlbum::MusicAlbum(QWidget *parent) : QWidget(parent)
//{
//    setInit();
//    setUIControl();
//}

//MusicAlbum::~MusicAlbum(){
//    delete settings;
//    this->deleteLater();
//}

///**
// * @brief MusicAlbum::setInit : 초기 세팅
// */
//void MusicAlbum::setInit(){
//    global.LmtCnt_H = global.LmtCnt_H - 320;
//    if(global.LmtCnt < 1460){
//       // global.LmtCnt = global.LmtCnt +180;
//    }
//    settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

//    btnStyle_normal = "QPushButton{ ";
//    btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:16px;";
//    btnStyle_normal += "border:1px solid #707070; border-radius:15px;}";
//    btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

//    btnStyle_selected = "QPushButton{ ";
//    btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:16px;";
//    btnStyle_selected += "border:1px solid #BE905F; border-radius:15px;}";

//    fileTypeList << tr("All") << tr("Lossless"); /// << "DSD64x" << "DSD128x" << "DSD256x" << "DSD512x" << "MQA";
//    bitDepthList << tr("All") << "24bit↑";          /// << "16bit↑" << "24bit↑";
//    samplingList << tr("All") << "88.2kHz↑" << "176.4kHz↑";  /// <<"44.1kHz↑" << "88.2kHz↑" << "176.4kHz↑" << "352.8kHz↑";
//    starList << tr("All") << "★★★★★↑" << "★★★★↑" << "★★★↑" << "★★↑" << "★↑" << tr("NONE");
//    orderList << tr("NONE") <<"A-Z" << tr("Artist") << tr("Favorite");  /// << tr("Recently") << tr("Artist,Release date") << tr("Year(Ascending)") << tr("Year(Descending)");
//      /*
//    fileTypeList << tr("전체") << tr("무손실PCM") << "DSD64x" << "DSD128x" << "DSD256x" << "DSD512x" << "MQA";
//    bitDepthList << tr("전체") << "16bit↑" << "24bit↑";
//    samplingList << tr("전체") << "44.1kHz↑" << "88.2kHz↑" << "176.4kHz↑" << "352.8kHz↑";
//    orderList << tr("전체") << tr("최근순") << tr("선호도순") << "A-Z" << tr("아티스트순") << tr("아티스트,발매일") << tr("년도(오름차순)") << tr("년도(내림차순)");
//    starList << tr("전체") << "★★★★★↑" << "★★★★↑" << "★★★↑" << "★★↑" << "★↑";
//    */

//}
///**
// * @brief MusicCategory::setUIControl : UI 세팅
// */
//void MusicAlbum::setUIControl(){//c220323
//    // 상단 필터
//    QPushButton *btn_filter = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png");
//    btn_filter->setFixedSize(80,50);
//    btn_filter->setCursor(Qt::PointingHandCursor);

//    this->hl_filter = new QHBoxLayout();
//    this->hl_filter->setContentsMargins(0,0,0,0);
//    this->hl_filter->setSpacing(0);
//    this->hl_filter->setAlignment(Qt::AlignLeft);
//    //this->hl_filter->addWidget(btn_filter);
//    //this->hl_filter->setSpacing(10);


//    QPushButton *btn_ser = new QPushButton(tr("Alphabet"),this);
//    btn_ser->setFixedSize(140,50);
//    QPixmap pixmap_btn_ser(":/images/icon_ser_w.png");
//    btn_ser->setCursor(Qt::PointingHandCursor);
//    btn_ser->setIcon(pixmap_btn_ser);
//    btn_ser->setIconSize(pixmap_btn_ser.rect().size());
//    btn_ser->setStyleSheet("color:#FFFFFF;font-size:15px;");


///*
//    QPushButton *btn_ser = GSCommon::getUIBtnImg("btn_ch_ser",":/images/icon_ser_w.png");
//    btn_ser->setFixedSize(180,50);
//    btn_ser->setCursor(Qt::PointingHandCursor);
//    btn_ser->setText(tr("First Letter"));
//    btn_ser->setStyleSheet("color:#FFFFFF;font-size:15px;");
//*/

//    /*settings->setValue(rosesettings.SETTINGS_AFILTER_FILETYPE, QVariant(0));
//    settings->setValue(rosesettings.SETTINGS_AFILTER_BIT, QVariant(0));
//    settings->setValue(rosesettings.SETTINGS_AFILTER_SAMPLEING, QVariant(0));
//    settings->setValue(rosesettings.SETTINGS_AFILTER_ORDER, QVariant(0));
//    settings->setValue(rosesettings.SETTINGS_AFILTER_STAR, QVariant(0));*/

//    this->lb_file_type = new QLabel;
//    this->lb_file_type->setContentsMargins(20,0,20,0);
//    this->lb_file_type->setFixedHeight(30);
//    this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
//    this->lb_file_type->setAlignment(Qt::AlignCenter);

//    if(settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() > 0){
//        this->before_file_type = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt();
//        this->lb_file_type->setText(fileTypeList.at(this->before_file_type));

//        hl_filter->addWidget(this->lb_file_type);
//        //hl_filter->addSpacing(10);

//        this->flag_file_type = true;
//    }

//    this->lb_bit_depth = new QLabel;
//    this->lb_bit_depth->setContentsMargins(20,0,20,0);
//    this->lb_bit_depth->setFixedHeight(30);
//    this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
//    this->lb_bit_depth->setAlignment(Qt::AlignCenter);

//    if(settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() > 0){
//        this->before_bit_depth = settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt();
//        this->lb_bit_depth->setText(bitDepthList.at(this->before_bit_depth));

//        hl_filter->addWidget(this->lb_bit_depth);
//        //hl_filter->addSpacing(10);

//        this->flag_bit_depth = true;
//    }

//    this->lb_sample_rate = new QLabel;
//    this->lb_sample_rate->setContentsMargins(20,0,20,0);
//    this->lb_sample_rate->setFixedHeight(30);
//    this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
//    this->lb_sample_rate->setAlignment(Qt::AlignCenter);

//    if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() > 0){
//        this->before_sample_rate = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt();
//        this->lb_sample_rate->setText(samplingList.at(this->before_sample_rate));

//        hl_filter->addWidget(this->lb_sample_rate);
//        //hl_filter->addSpacing(10);

//        this->flag_sample_rate = true;
//    }

//    this->lb_star_point = new QLabel;
//    this->lb_star_point->setContentsMargins(20,0,20,0);
//    this->lb_star_point->setFixedHeight(30);
//    this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
//    this->lb_star_point->setAlignment(Qt::AlignCenter);

//    if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
//        this->before_star_point = settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt();
//        this->lb_star_point->setText(starList.at(this->before_star_point));

//        hl_filter->addWidget(this->lb_star_point);
//        //hl_filter->addSpacing(10);

//        this->flag_star_point = true;
//    }

//    this->lb_order = new QLabel;
//    this->lb_order->setContentsMargins(20,0,20,0);
//    this->lb_order->setFixedHeight(30);
//    this->lb_order->setStyleSheet("color:#FFFFFF;font-size:15px;border:1px solid #707070;border-radius:15px;");
//    this->lb_order->setAlignment(Qt::AlignCenter);

//    if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
//        this->before_order = settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt();
//        this->lb_order->setText(orderList.at(this->before_order));

//        hl_filter->addWidget(this->lb_order);
//        //hl_filter->addSpacing(10);

//        this->flag_order = true;
//    }



//    widget_btnFilter = new QWidget();
//    widget_btnFilter->setObjectName("widget_btnFilter");
//    widget_btnFilter->setStyleSheet("#widget_btnFilter { background-color:#171717; }");
//    widget_btnFilter->setFixedHeight(50);
//    widget_btnFilter->setLayout(hl_filter);
//    //widget_btnFilter->hide();

//    setUIlistBtnSer_boxControl();

//    QHBoxLayout *hl_btn_Ser = new QHBoxLayout();//c220415
//    hl_btn_Ser->addWidget(btn_ser,0, Qt::AlignCenter|Qt::AlignLeft);
//    hl_btn_Ser->addWidget(btn_serClear,0, Qt::AlignVCenter|Qt::AlignLeft);
//    hl_btn_Ser->addWidget(widget_total_ser,0, Qt::AlignVCenter|Qt::AlignLeft);

//    QWidget *widget_total_ser2 = new QWidget();//c220415
//    widget_total_ser2->setObjectName("widget_total_ser");//c220415
//    widget_total_ser2->setStyleSheet("#widget_total_ser { background-color:#171717; }");//c220415
//    widget_total_ser2->setLayout(hl_btn_Ser);//c220415


//    albumtCnt = new QLabel;//c230304
//    albumtCnt->setText(QString("Albums %1").arg(1));//c230304
//    albumtCnt->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");


//    QHBoxLayout *hl_btnFilter_Ser = new QHBoxLayout();//c220415
//    hl_btnFilter_Ser->setSpacing(10);
//    hl_btnFilter_Ser->setContentsMargins(30,0,20,0);
//    hl_btnFilter_Ser->addWidget(btn_filter,0, Qt::AlignVCenter|Qt::AlignLeft);
//    hl_btnFilter_Ser->addWidget(widget_btnFilter,0, Qt::AlignVCenter|Qt::AlignLeft);
//    //hl_btnFilter_Ser->addWidget(widget_total_ser2,1, Qt::AlignVCenter|Qt::AlignLeft);//c230304
//    hl_btnFilter_Ser->addWidget(albumtCnt,1, Qt::AlignVCenter|Qt::AlignRight);//c230304


//    QWidget *widget_filter_ser = new QWidget();//cheon210527
//    widget_filter_ser->setObjectName("widget_filter_ser");//cheon210527
//    widget_filter_ser->setStyleSheet("#widget_filter_ser { background-color:#171717; }");//cheon210527
//    widget_filter_ser->setLayout(hl_btnFilter_Ser);//cheon210527



//    connect(btn_filter, SIGNAL(clicked()), SLOT(slot_clickedBtnFilter()));
//    connect(btn_ser, SIGNAL(clicked()), SLOT(slot_clickedBtnSer()));


///*//c230304
//    albumtCnt = new QLabel;
//    albumtCnt->setText(QString("Total : %1").arg(1));
//    albumtCnt->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");
//*/
//    albumPage = new QLabel;
//    albumPage->setText(QString("Page : %1 ~ %2").arg(1).arg(1));
//    albumPage->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");

//    this->slider_playbar = new sectionbottomSlider(Qt::Horizontal);
//    this->slider_playbar->setObjectName("slider_vol");
//    this->slider_playbar->setCursor(Qt::PointingHandCursor);

//    //this->slider_playbar->setSingleStep(1);     // 1초 = 1000    // arrow-key step-size
//    //this->slider_playbar->setPageStep(1);                // mouse-wheel/page-key step-size
//    this->slider_playbar->setFixedWidth(150);
//    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background:#333333;width:4px; } QSlider::handle:horizontal { background:white;height:16px;border-radius:8px;margin:0 -6px; } QSlider::add-page:horizontal { background:#CCCCCC; } QSlider::sub-page:horizontal { background:#333333; } ");
//    this->slider_playbar->setStyleSheet("QSlider::groove:horizontal {"
//        "border-radius: 1px;"
//        "height: 5px;"
//        "margin: 0px;"
//        "background-color: #555555;}"// rgb(52, 59, 172);}
//    "QSlider::groove:horizontal:hover {"
//        "background-color: rgb(55, 62, 76);"
//    "}"
//    "QSlider::handle:horizontal {"
//        "background-color: #CCCCCC;"//rgb(85, 170, 255);
//        "border: none;"
//        "height: 16px;"
//        "width: 16px;"
//        "margin: -8px 0;"
//        "border-radius: 8px;"
//        "padding: -8px 0px;"
//    "}"
//    "QSlider::handle:horizontal:hover {"
//        "background-color: rgb(155, 180, 255);"
//    "}"
//    "QSlider::handle:horizontal:pressed {"
//        "background-color: rgb(65, 255, 195);"
//    "}");
//    //this->slider_playbar->setStyleSheet("QSlider::groove:horizontal { background:#333333;height:4px;} QSlider::handle:horizontal {  background:white;width:15px;border-radius:8px;margin:0 -6py; } QSlider::add-page:horizontal { background:#aaaaaa; } QSlider::sub-page:horizontal { background:#B18658; } ");
//    this->slider_playbar->setMaximum(1);
//    this->slider_playbar->setMinimum(1);

//    QLabel *le_page_search = new QLabel;//cheon210527
//    le_page_search->setText(QString("goto Page"));//cheon210527
//    le_page_search->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");//cheon210527
//    le_page_search->setFixedSize(100,30);//cheon210527

//    this->le_lisearch = new QLineEdit();//cheon210717-search
//    //this->le_lisearch->setValidator( new QIntValidator(0, 1000, this) );//cheon210717-search
//    this->le_lisearch->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
//   // this->le_lisearch = new QTextEdit();//cheon210527
//    this->le_lisearch->setObjectName("le_search");//cheon210527
//    this->le_lisearch->setStyleSheet("#le_search { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:5px;}");//cheon210527
//    this->le_lisearch->setMaxLength(5);//cheon210527
//    this->le_lisearch->setFixedSize(100,30);//cheon210527
//    this->le_lisearch->setTextMargins(1,1,1,1);
//    this->le_lisearch->setText("1");

//    QPushButton *btn_ok = new QPushButton;
//    btn_ok->setText(tr("go"));//btn_ok->setText(tr("추가"));
//    btn_ok->setCursor(Qt::PointingHandCursor);
//    btn_ok->setStyleSheet("font-size:15px;font:bold; color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
//    btn_ok->setFixedSize(70,30);

//    QHBoxLayout *search_hBox = new QHBoxLayout();//cheon210527
//    search_hBox->setSpacing(10);//cheon210527
//    search_hBox->setContentsMargins(0,0,0,0);//cheon210527
//   // search_hBox->setAlignment(Qt::AlignTop);//cheon210527
//    search_hBox->addWidget(le_page_search,1,  Qt::AlignRight);//c220414
//    search_hBox->addWidget(this->slider_playbar);
//    search_hBox->addWidget(this->le_lisearch);//cheon210527
//    search_hBox->addWidget(btn_ok);



//    QWidget *widget_search = new QWidget();//cheon210527
//    widget_search->setObjectName("widget_search");//cheon210527
//    widget_search->setFixedHeight(30);//cheon210527
//    widget_search->setFixedWidth(450);//cheon210527
//    widget_search->setStyleSheet("#widget_search { background-color:#171717; }");//cheon210527
//    widget_search->setLayout(search_hBox);//cheon210527

//    QHBoxLayout *tmp2_hBox = new QHBoxLayout();//cheon210527
//    tmp2_hBox->setSpacing(10);//cheon210527
//    tmp2_hBox->setContentsMargins(30,0,20,0);//cheon210527
//    tmp2_hBox->addWidget(widget_search,1, Qt::AlignVCenter);//cheon210527
//    //tmp2_hBox->setSpacing(10);
//    //tmp2_hBox->addWidget(btn_ok, 1, Qt::AlignVCenter | Qt::AlignLeft);

//    QWidget *widget_search_last = new QWidget();//cheon210527
//    widget_search_last->setObjectName("widget_search_last");//cheon210527
//    widget_search_last->setStyleSheet("#widget_search_last { background-color:#171717; }");//cheon210527
//    widget_search_last->setLayout(tmp2_hBox);//cheon210527

//    //this->slider_playbar->disconnect();//5555
//    //print_debug();
//    this->slider_playbar->setValue(0);
//    //connect(this->slider_playbar, SIGNAL(signal_sliderclick(int)), this, SLOT(slot_sliderClick(int)));//c1209

//    //connect(this->slider_playbar, SIGNAL(sliderReleased()) ,this, SLOT(changedSliderBar()));
//    //connect(this->slider_playbar, SIGNAL(sliderPressed()) ,this, SLOT(changedSliderBar()));

//    //--start-----------------------------------------------------------------

//    le_lisearch_str = new QLineEdit();//cheon210717-search
//    //this->le_lisearch->setValidator( new QIntValidator(0, 1000, this) );//cheon210717-search
//    //le_lisearch_str->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
//    le_lisearch_str->setObjectName("le_lisearch_str");//cheon210527
//    le_lisearch_str->setStyleSheet("#le_lisearch_str { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:5px;}");//cheon210527
//    le_lisearch_str->setMaxLength(32);//cheon210527
//    le_lisearch_str->setFixedSize(300,30);//cheon210527
//    le_lisearch_str->setTextMargins(1,1,1,1);
//    le_lisearch_str->setText("");

//    QLabel *le_str_search = new QLabel;//cheon210527
//    le_str_search->setText(QString("Album Search"));//cheon210527
//    le_str_search->setStyleSheet("font-size:15px; font:bold; color:#dddddd;");//cheon210527
//    le_str_search->setFixedSize(100,30);//cheon210527

//    QPushButton *btn_ok_str = new QPushButton;
//    btn_ok_str->setText(tr("ok"));//btn_ok->setText(tr("추가"));
//    btn_ok_str->setCursor(Qt::PointingHandCursor);
//    btn_ok_str->setStyleSheet("font-size:15px;font:bold; color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
//    btn_ok_str->setFixedSize(70,30);

//    QHBoxLayout *search_str_hBox = new QHBoxLayout();//cheon210527
//    search_str_hBox->setSpacing(0);//cheon210527
//    search_str_hBox->setContentsMargins(0,0,0,0);//cheon210527
//   // search_hBox->setAlignment(Qt::AlignTop);//cheon210527
//    search_str_hBox->addWidget(le_str_search);//cheon210527
//    search_str_hBox->setSpacing(10);//cheon210527
//    search_str_hBox->addWidget(this->le_lisearch_str);//cheon210527
//    search_str_hBox->addWidget(btn_ok_str);



//    QWidget *widget_search_str = new QWidget();//cheon210527
//    widget_search_str->setObjectName("search_str_hBox");//cheon210527
//    widget_search_str->setStyleSheet("#search_str_hBox { background-color:#171717; }");//cheon210527
//    widget_search_str->setLayout(search_str_hBox);//cheon210527
//    widget_search_str->setFixedWidth(490);

//    //-end------------------------------------------------------------------


//    QHBoxLayout *tmp_hBox = new QHBoxLayout();
//    tmp_hBox->setSpacing(0);
//    tmp_hBox->setContentsMargins(40,0,20,0);
//   // tmp_hBox->setAlignment(Qt::AlignTop);

//    //tmp_hBox->addWidget(albumtCnt,0, Qt::AlignVCenter);//c230304
//    //tmp_hBox->addWidget(albumPage,0, Qt::AlignVCenter);//c230304
//    //tmp_hBox->setSpacing(20);//c230304
//    //tmp_hBox->addWidget(widget_btnSer, 0, Qt::AlignVCenter|Qt::AlignLeft );//c230304
//    //tmp_hBox->addWidget(widget_search_last, 0, Qt::AlignVCenter|Qt::AlignLeft );//c230304
//    //tmp_hBox->addWidget(albumtCnt, 0, Qt::AlignVCenter|Qt::AlignRight);//c230304

//    QWidget *line_bottom_1 = new QWidget();
//    line_bottom_1->setFixedHeight(1);
//    line_bottom_1->setStyleSheet("background-color:#505050;");


//    QWidget *widget_tt = new QWidget();
//    widget_tt->setObjectName("widget_tt");
//    widget_tt->setStyleSheet("#widget_tt { background-color:#171717; }");
//    widget_tt->setFixedHeight(40);
//    widget_tt->setLayout(tmp_hBox);

//    flowLayout = new FlowLayout(this,0,18,20);

//    // 노래 레이아웃
//    this->widget_scrollArea = new QWidget();
//    this->widget_scrollArea->setContentsMargins(33,0,18,20);//10);

//    this->widget_scrollArea->setLayout(flowLayout);

//    this->scrollArea = new QScrollArea();
//    this->scrollArea->setObjectName("scrollArea");
//    this->scrollArea->setWidget(widget_scrollArea);
//    this->scrollArea->setWidgetResizable(true);
//    this->scrollArea->setStyleSheet("#scrollArea { border:0px; }"); //background-color:#171717; }");
//    this->scrollArea->verticalScrollBar()->setCursor(Qt::PointingHandCursor);
//    this->scrollArea->verticalScrollBar()->
//            setStyleSheet("QScrollBar:vertical {border: none; background-color: transparent; width: 6px; margin: 12px 0px 12px 0px; }"
//                                      "QScrollBar::handle:vertical {background-color: #b18658; min-height: 60px; border-radius: 3px; }"
//                                      "QScrollBar::add-line:vertical {height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }"
//                                      "QScrollBar::sub-line:vertical {height: 0 px; subcontrol-position: top; subcontrol-origin: margin; }"
//                                      "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none; }");
//            /*setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
//                                             "   width: 8px;"
//                                             "   background: rgba (0,0,0,0%);"
//                                             "   margin: 0px, 0px, 0px, 0px;"
//                                             "   padding-top: 9px;"//set aside 9px for the arrows above and below
//                                             "   padding-bottom: 9px;"
//                                            "}"
//                                            "QScrollBar :: handle: vertical"
//                                            "{"
//                                             "   width:8px;"
//                                             "   background: rgba (0,0,0,25%);"
//                                             "   border-radius: 4px;"//Both ends of the scroll bar become ellipses
//                                             "   min-height: 20 px;"
//                                           "}"
//                                           " QScrollBar :: handle: vertical: hover"
//                                           " {"
//                                           "     width: 8px;"
//                                           "     background: rgba (0,0,0,50%);"//When the mouse is placed on the scroll bar, the color becomes darker
//                                           "     border-radius: 4px;"
//                                           "     min-height: 20 px;"
//                                           " }"));*/

//            /*
//            setStyleSheet(QString::fromUtf8("QScrollBar:vertical {"
//                                            "    border: 2px solid #aaaaaa;"
//                                            "    background:#777777;"
//                                            "    width:15px;  "
//                                            "    margin: 0px 0px 0px 0px;"
//                                            "}"
//                                            "QScrollBar::handle:vertical {"
//                                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
//                                            "    stop: 0 #999999, stop: 0.5 #171717, stop:1 #999999);"
//                                            "    min-height: 50px;"
//                                            "}"
//                                            "QScrollBar::add-line:vertical {"
//                                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
//                                            "    stop: 0 #171717, stop: 0.5 #171717,  stop:1 #171717);"
//                                            "    height: 0px;"
//                                            "    subcontrol-position: bottom;"
//                                            "    subcontrol-origin: margin;"
//                                            "}"
//                                            "QScrollBar::sub-line:vertical {"
//                                            "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
//                                            "    stop: 0  #171717, stop: 0.5 #171717,  stop:1 #171717);"
//                                            "    height: 0 px;"
//                                            "    subcontrol-position: top;"
//                                            "    subcontrol-origin: margin;"
//                                            "}"
//                                            ));*/
//    //this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

//    this->scrollArea->verticalScrollBar()->setSingleStep(92);

//    QVBoxLayout *box_scroll = new QVBoxLayout();
//    box_scroll->setSpacing(0);
//    box_scroll->setContentsMargins(33, 0, 10, 20);
//    box_scroll->setAlignment(Qt::AlignTop);
//    box_scroll->addWidget(this->scrollArea);


//    //this->scrollArea->verticalScrollBar()->setPageStep(90*10);
///*
//    QVBoxLayout *vl_total44= new QVBoxLayout();
//    vl_total44->setContentsMargins(0,0,0,0);//c220414
//    vl_total44->addWidget(this->scrollArea);

//    line_bottom_33 = new QWidget();
//    line_bottom_33->setLayout(vl_total44);
//*/
//    this->vl_total= new QVBoxLayout();
//    this->vl_total->setContentsMargins(0,0,0,0);//c220414
//    this->vl_total->setSpacing(0);
//    this->vl_total->setAlignment(Qt::AlignTop );
//    //this->vl_total->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
//    //this->vl_total->addWidget(widget_btnFilter);//c220415
//    this->vl_total->addWidget(widget_filter_ser);//c220415


//    //this->vl_total->addWidget(widget_tt);//c230304
//    this->vl_total->addWidget(line_bottom_1);
//    this->vl_total->addSpacing(20);
//    //this->vl_total->addWidget(this->scrollArea);box_scroll
//    this->vl_total->addLayout(box_scroll);

//    setLayout(this->vl_total);

//    connect(this->scrollArea->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));
//    connect(this->scrollArea->verticalScrollBar(), SIGNAL(sliderPressed()), this, SLOT(slot_sliderPressed()));
//    //connect(this->scrollArea, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderChange()));

//    // connect

//    // FIlter 선택 UI 세팅
//    setFIlterUIControl();
//    total_db_cnt = getAlbumDataFromDBforTotalCount();
//    connect(this->le_lisearch, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed()));//cheon210717-search
//    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedChange()));//cheon210717-search

//    connect(this->le_lisearch_str, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed_str()));//cheon210717-search
//    connect(btn_ok_str, SIGNAL(clicked()), this, SLOT(slot_clickedChange_str()));



//    linker = Linker::getInstance();

//    connect(this->slider_playbar, SIGNAL(valueChanged(int)) ,this, SLOT(slot_changedSliderBar(int)));
//    connect(linker, SIGNAL(signal_completeDownloadDB()), this, SLOT(slot_returnPressed()));//c220402
//    //connect(linker, SIGNAL(signal_window_resize()), this, SLOT(slot_sliderbarValue()));


//}

//void MusicAlbum::slot_clickedBtnSer(){//c220415
//        print_debug();
//        btn_serClear->setVisible(!btn_serClear->isVisible());
//        widget_total_ser->setVisible(!widget_total_ser->isVisible());

//        if(!btn_serClear->isVisible()){
//            QTimer::singleShot(500, this, SLOT(slot_clearButton_setStyleSheet()));
//        }


//}

//void MusicAlbum::slot_clicked_search_eng_ch(){//c220415
//    print_debug();

//    for(int i = 0; i < engList.size(); i++){
//        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

//    }
//    for(int i = 0; i < hanList.size(); i++){
//        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

//    }
//    btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
//    ClickableLabel *tmp_btn = dynamic_cast<ClickableLabel*>(sender());
//    tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
//    int chIndedx = tmp_btn->property("search_chIndex").toInt();
//    qDebug() << "engList.at(chIndedx) = " << engList.at(chIndedx);
//    setDataFromDB_Alpa(engList.at(chIndedx));


//}
//void MusicAlbum::slot_clicked_search_han_ch(){//c220415
//    print_debug();

//    for(int i = 0; i < engList.size(); i++){
//        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

//    }
//    for(int i = 0; i < hanList.size(); i++){
//        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

//    }
//    btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
//    ClickableLabel *tmp_btn = dynamic_cast<ClickableLabel*>(sender());
//    tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
//    int chIndedx = tmp_btn->property("search_chIndex").toInt();
//    qDebug() << "engList.at(chIndedx) = " << hanList.at(chIndedx);
//    setDataFromDB_Alpa(hanList.at(chIndedx));


//}

//void MusicAlbum::slot_clearButton_setStyleSheet(){
//    if(!strArg_1.isEmpty()){
//        btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
//        setDataFromDB_Alpa("");

//    }else{
//        btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("#B18658", "transparent"));
//    }
//}

//void MusicAlbum::slot_clicked_search_eng_ch_clear(){//c220415
//    print_debug();
//    btn_serClear->setStyleSheet(btn_serClear->styleSheet().replace("transparent", "#B18658"));
////search_clearIndex
//    for(int i = 0; i < engList.size(); i++){
//        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

//    }
//    for(int i = 0; i < hanList.size(); i++){
//        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

//    }

//     QTimer::singleShot(500, this, SLOT(slot_clearButton_setStyleSheet()));



//}

//void MusicAlbum::setUIlistBtnSer_boxControl(){//c220415
//    hanList << "ㄱ"<<"ㄴ"<<"ㄷ"<<"ㄹ"<<"ㅁ"<<"ㅂ"<<"ㅅ"<<"ㅇ"<<"ㅈ"<<"ㅊ"<<"ㅋ"<<"ㅌ"<<"ㅍ"<<"ㅎ";
//    engList << "A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T"<<"U"<<"V"<<"W"<<"X"<<"Y"<<"Z";

//    btn_serClear = new ClickableLabel();
//    btn_serClear->setContentsMargins(0,0,0,0);
//    btn_serClear->setFixedSize(50,20);
//    btn_serClear->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:12px;border:1px solid #707070;border-radius:20px;");
//    btn_serClear->setAlignment(Qt::AlignLeft);
//    btn_serClear->setText("Clear");
//    btn_serClear->setCursor(Qt::PointingHandCursor);
//    btn_serClear->setProperty("search_clearIndex", -1);

//    this->hl_eng_ser = new QHBoxLayout();
//    this->hl_eng_ser->setContentsMargins(0,0,0,0);
//    this->hl_eng_ser->setSpacing(0);
//    this->hl_eng_ser->setAlignment(Qt::AlignLeft);
//    //this->hl_ser->addWidget(btn_ser);
//    this->hl_eng_ser->setSpacing(10);

//    this->hl_han_ser = new QHBoxLayout();
//    this->hl_han_ser->setContentsMargins(0,0,0,0);
//    this->hl_han_ser->setSpacing(0);
//    this->hl_han_ser->setAlignment(Qt::AlignLeft);
//    //this->hl_ser->addWidget(btn_ser);
//    this->hl_han_ser->setSpacing(10);

//    QWidget *widget_btnSer_eng = new QWidget();
//    widget_btnSer_eng->setObjectName("widget_btnSer_eng");
//    widget_btnSer_eng->setStyleSheet("#widget_btnSer_eng { background-color:#171717; }");
//    widget_btnSer_eng->setFixedHeight(30);
//    widget_btnSer_eng->setLayout(hl_eng_ser);

//    QWidget *widget_btnSer_han = new QWidget();
//    widget_btnSer_han->setObjectName("widget_btnSer_han");
//    widget_btnSer_han->setStyleSheet("#widget_btnSer_han { background-color:#171717; }");
//    widget_btnSer_han->setFixedHeight(30);
//    widget_btnSer_han->setLayout(hl_han_ser);

//    QVBoxLayout *vl_total_ser= new QVBoxLayout();
//    vl_total_ser->setContentsMargins(0,0,200,0);//c220414
//    vl_total_ser->setSpacing(0);
//    //vl_total_ser->setAlignment(Qt::AlignTop);
//    vl_total_ser->addWidget(widget_btnSer_eng);//c220415
//    vl_total_ser->addWidget(widget_btnSer_han);//c220415

//    widget_total_ser = new QWidget();//c220415
//    widget_total_ser->setObjectName("widget_total_ser");//c220415
//    widget_total_ser->setStyleSheet("#widget_total_ser { background-color:#171717; }");//c220415
//    widget_total_ser->setLayout(vl_total_ser);//c220415




//    for(int i = 0; i < engList.size(); i++){
//        this->lb_eng_type[i] = new ClickableLabel();
//        this->lb_eng_type[i]->setContentsMargins(20,0,20,0);
//        this->lb_eng_type[i]->setFixedSize(20,20);
//        this->lb_eng_type[i]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:12px;border:1px solid #707070;border-radius:20px;");
//        this->lb_eng_type[i]->setAlignment(Qt::AlignCenter);
//        this->lb_eng_type[i]->setText(engList.at(i));
//        this->lb_eng_type[i]->setCursor(Qt::PointingHandCursor);

//        //this->lb_han_type[i]->setStyleSheet("background-color:transparent;border-radius:15px;");
//        //this->lb_han_type[i]->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);

//        this->lb_eng_type[i]->setProperty("search_chIndex", i);
//        this->lb_eng_type[i]->setStyleSheet(this->lb_eng_type[i]->styleSheet().replace("#B18658", "transparent"));

//        hl_eng_ser->addWidget(this->lb_eng_type[i]);
//        hl_eng_ser->addSpacing(3);

//        connect(this->lb_eng_type[i], SIGNAL(signal_clicked()), this, SLOT(slot_clicked_search_eng_ch()));//cheon210717-search

//    }
//    for(int i = 0; i < hanList.size(); i++){
//        this->lb_han_type[i] = new ClickableLabel();
//        this->lb_han_type[i]->setContentsMargins(20,0,20,0);
//        this->lb_han_type[i]->setFixedSize(20,20);
//        this->lb_han_type[i]->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:12px;border:1px solid #707070;border-radius:20px;");
//        this->lb_han_type[i]->setAlignment(Qt::AlignCenter);
//        this->lb_han_type[i]->setText(hanList.at(i));
//        this->lb_han_type[i]->setCursor(Qt::PointingHandCursor);

//        //this->lb_han_type[i]->setStyleSheet("background-color:transparent;border-radius:15px;");
//        //this->lb_han_type[i]->setFixedSize(Q_MAIN_ICON_W,Q_MAIN_ICON_H);

//        this->lb_han_type[i]->setProperty("search_chIndex", i);
//        this->lb_han_type[i]->setStyleSheet(this->lb_han_type[i]->styleSheet().replace("#B18658", "transparent"));

//        hl_han_ser->addWidget(this->lb_han_type[i]);
//        hl_han_ser->addSpacing(3);

//        connect(this->lb_han_type[i], SIGNAL(signal_clicked()), this, SLOT(slot_clicked_search_han_ch()));//cheon210717-search

//    }
//    btn_serClear->hide();
//    widget_total_ser->hide();
//    connect(btn_serClear, SIGNAL(signal_clicked()), this, SLOT(slot_clicked_search_eng_ch_clear()));//cheon210717-search



//}

//void MusicAlbum::slot_changedSliderBar(int p_value){//c22040301

//    //Q_UNUSED(p_value);
//    // nothing
//    //print_debug();
//    qDebug() << "MusicAlbum::slot_changedSliderBar--p_value = " << p_value;
//    curr_p_value = p_value;
//    this->slider_playbar->setValue(curr_p_value);
//    this->le_lisearch->setText(QString("%1").arg(curr_p_value));
//}
///**
// * @brief SectionBottom::slot_timePlayValue
// */
//void MusicAlbum::slot_sliderbarValue(){
//    print_debug();
//    //if(!blocking_flag)return;
//    //blocking_flag = false;

//    qDebug() << "tttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt";
//    if(!global.device.getDeviceIP().isEmpty()){
//        print_debug();
//        //scrollArea->verticalScrollBar()->maximum()

//        curr_LmtWidth = global.LmtCnt;
//        qDebug() << "curr_LmtWidth= " << curr_LmtWidth;
//        qDebug() << "pre_LmtWidth= " << pre_LmtWidth;
//        qDebug() << "sliderPosition" << this->scrollArea->verticalScrollBar()->sliderPosition();

//        if(pre_LmtWidth > 0 && abs(curr_LmtWidth - pre_LmtWidth) >= 5  && curr_page_cnt > 0 && curr_page_cnt <= total_page_cnt){
//            print_debug();

//            if(curr_page_cnt == 1){
//                flag_page_first = true;
//                currOffsetPoint = 0;
//            }

//            slot_sliderValue_flag = true;
//            //0717 mtx.lock();
//            getAlbumDataFromDB_view_only();
//            //0717 mtx.unlock();


//        }

//        if(scrollArea->verticalScrollBar()->maximum()==this->scrollArea->verticalScrollBar()->sliderPosition()){
//            print_debug();
//        }else{
//            print_debug();
//        }
//        pre_LmtWidth = global.LmtCnt;
//    }
//    qDebug() << "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww";
//    //blocking_flag = true;

//}

///**
// * @brief MusicAlbum::getAlbumDataFromDB : GET 앨범 데이터
// */
//void MusicAlbum::getAlbumDataFromDB_pre(){//c220323

//    int cnt = 0;
//    LMT_CNT = 0;

//    cnt = 1;
//        LMT_CNT = cnt * PAGE_LMT;
//    //}

//    this->data = QVariantList();

//    pre_lmt_cnt = LMT_CNT;
//    if(OffsetPoint_Pre >= LMT_CNT){
//        //currOffsetPoint = OffsetPoint;
//        if(slot_sliderValue_flag){
//            if(mouse_wheel_flag){
//                if(OffsetPoint_Pre%LMT_CNT !=0){
//                    OffsetPoint_Pre -= (OffsetPoint_Pre%LMT_CNT);
//                }
//                else OffsetPoint_Pre -= LMT_CNT;
//            }
//        }
//        else {
//            if(OffsetPoint_Pre%LMT_CNT !=0){
//                OffsetPoint_Pre -= (OffsetPoint_Pre%LMT_CNT);
//            }
//            else OffsetPoint_Pre -= LMT_CNT;
//        }


//    }
//    else{
//        OffsetPoint_Pre = 0;


//    }

//    print_debug();
//    qDebug() << "OffsetPoint_Pre = " << OffsetPoint_Pre;
//    qDebug() << "flagReqMore = " << flagReqMore;


//    // DB 쿼리
//    SqliteHelper *sqlite = new SqliteHelper(this);
//    QSqlError err = sqlite->addConnectionRose();
//    if(err.type() == QSqlError::NoError){

//        QString strQuery = "";
//        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
//        strQuery += " FROM album_info AS AI ";
//        strQuery += " LEFT OUTER JOIN album_art AS AA ";
//        strQuery += " ON AI._id=AA.album_id ";
//        //strQuery += " ORDER BY AI.date_added desc ";
//        //strQuery += " ORDER BY AI.album asc ";
//        QString setWhere = "";
//        QString setOrder = "";


//        if(!strArg_1.isEmpty()){
//            print_debug();
//            qDebug() << "strArg_1 = " << strArg_1;

//            //strQuery += "(";
//            strQuery += searchcharactorSqlWhere();
//            //strQuery += ")";

//            setWhere = getSetWhereFilter();
//            if(!setWhere.isEmpty()){
//                strQuery += " AND ";
//                strQuery += setWhere;
//            }

//            strQuery += " ORDER BY AI.album ";
//        }
//        else{



//            setWhere = getSetWhereFilter();

//            if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
//                if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
//                    setOrder += " ORDER BY AI.album ";
//                }
//                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
//                    setOrder += " ORDER BY AI.artist ";
//                }
//                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3){
//                    setOrder += " ORDER BY AI.favorite ";
//                }
//            }

//            if(!setWhere.isEmpty()){
//                strQuery += " WHERE";
//                strQuery += setWhere;
//            }
//            if(!setOrder.isEmpty()){
//                strQuery += setOrder;
//            }
//            //strQuery += " ORDER BY AI.album ";
//        }


//        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint_Pre);

//        //this->data = QVariantList();
//        sqlite->exec(strQuery, this->data);


//        total_page_cnt = 0;
//        if((total_db_cnt % LMT_CNT) > 0){
//            total_page_cnt = (total_db_cnt / LMT_CNT) + 1;
//        }
//        else if((total_db_cnt % LMT_CNT) == 0){
//            total_page_cnt = (total_db_cnt / LMT_CNT);
//        }
//        //OffsetPoint += flowLayout->count();
//        //OffsetPoint += LMT_CNT;

//        qDebug() << "data.size() = " << this->data.size();

//        sqlite->close();
//        delete sqlite;
//    }


//}
//void MusicAlbum::getAlbumDataFromDB(){//c220323
//    print_debug();
//    qDebug() << "this->scrollArea->width()= " << this->scrollArea->verticalScrollBar()->height();
//    qDebug() << "strArg_1= " << strArg_1;
//    qDebug() << "global.LmtCnt= " << global.LmtCnt;

//    int cnt = 0;
//    LMT_CNT = 0;

//    cnt = 1;
//        cnt_size = cnt;


//        LMT_CNT = cnt * PAGE_LMT;
//    //}

//    this->data = QVariantList();

//    pre_lmt_cnt = LMT_CNT;
//    //if(!flagReqMore){//cheon210607-test
//    //    flagReqMore = true;//cheon210607-test
//    // DB 쿼리

//    qDebug() << "global.LmtCnt= " << global.LmtCnt;
//    qDebug() << "global.LmtCnt_H= " << global.LmtCnt_H;
//    qDebug() << "LMT_CNT= " << LMT_CNT;
//    //qDebug() << "this->scrollArea->width= " << this->scrollArea->width();
//    //qDebug() << "this->scrollArea->height= " << this->scrollArea->height();
//    qDebug() << "cnt= " << cnt;
//    //qDebug() << "dum= " << dum;

//    if(flag_page_first == true){
//        OffsetPoint = 0;
//        flag_page_first = false;
//    }
//    else{
//        //currOffsetPoint = OffsetPoint;


//    }
//    total_page_cnt = 0;
//    if((total_db_cnt % LMT_CNT) > 0){
//        total_page_cnt = (total_db_cnt / LMT_CNT) + 1;
//    }
//    else if((total_db_cnt % LMT_CNT) == 0){
//        total_page_cnt = (total_db_cnt / LMT_CNT);
//    }


//    if(total_db_cnt == OffsetPoint) return;

//    qDebug() << "********************curr_page_cnt= " << curr_page_cnt;
//    //albumtCnt->setText(QString("Total List Count : %1                            Page : %2 / %3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));



//        SqliteHelper *sqlite = new SqliteHelper(this);
//        QSqlError err = sqlite->addConnectionRose();
//        if(err.type() == QSqlError::NoError){
//            QString strQuery = "";

//            strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
//            strQuery += " FROM album_info AS AI ";
//            strQuery += " LEFT OUTER JOIN album_art AS AA ";
//            strQuery += " ON AI._id=AA.album_id ";

//            QString setWhere = "";
//            QString setOrder = "";
//            if(!strArg_1.isEmpty()){
//                print_debug();
//                qDebug() << "strArg_1 = " << strArg_1;

//                //strQuery += "(";
//                strQuery += searchcharactorSqlWhere();
//                //strQuery += ")";

//                setWhere = getSetWhereFilter();
//                if(!setWhere.isEmpty()){
//                    strQuery += " AND ";
//                    strQuery += setWhere;
//                }

//                strQuery += " ORDER BY AI.album ";
//            }
//            else{
//                print_debug();
//                setWhere = getSetWhereFilter();
//                qDebug() << "setWhere=" << setWhere;

//                if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
//                    if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
//                        setOrder += " ORDER BY AI.album ";
//                    }
//                    else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
//                        setOrder += " ORDER BY AI.artist ";
//                    }
//                    else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3){
//                        setOrder += " ORDER BY AI.favorite ";
//                    }
//                    else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 4){//c221109
//                        setOrder = " ORDER BY RANDOM() ";
//                    }
//                }

//                if(!setWhere.isEmpty()){
//                    strQuery += " WHERE";
//                    strQuery += setWhere;
//                }
//                if(!setOrder.isEmpty()){
//                    strQuery += setOrder;
//                }
//                //strQuery += " ORDER BY AI.album ";

//            }

//            strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

//            qDebug() << strQuery;

//            //QVariantList data;

//            sqlite->exec(strQuery, this->data);

//            OffsetPoint += this->data.size();
//            //OffsetPoint += flowLayout->count();
//            //OffsetPoint += LMT_CNT;

//            qDebug() << "data.size() = " << this->data.size();

//            qDebug() << "in getAlbumDataFromDB-------------OffsetPoint = " << OffsetPoint;
//            qDebug() << "in getAlbumDataFromDB-------------curr_page_cnt = " << curr_page_cnt;

//            print_debug();
//            //Etimer.();
//            sqlite->close();
//             print_debug();
//            delete sqlite;
//             print_debug();

//        }//if(err.type() == QSqlError::NoError)

//}

//void MusicAlbum::getAlbumDataFromDB_pre_view(){//c220323
//    print_debug();
//    qDebug() << "layout->count() = " << flowLayout->count();
//    double nnn = (global.LmtCnt-33-15-20)/(186+18);
//    int nn = nnn;
//    int mod_nn = (global.LmtCnt-33-15-20)%(186+18);
//    double fff = mod_nn/186;
//    int ff = fff;
//    nn = nn + ff;

//    qDebug() << "global.LmtCnt = " << global.LmtCnt;
//    qDebug() << "mod_nn = " << mod_nn;
//    qDebug() << "ff = " << ff;
//    qDebug() << "nn = " << nn;
//    //int scrol_cnt = PAGE_LMT/nn;
//    PAGE_LMT = nn * 10;
//    qDebug() << "1-OffsetPoint_Pre = " << OffsetPoint_Pre;
//    getAlbumDataFromDB_pre();
//    qDebug() << "2-OffsetPoint_Pre = " << OffsetPoint_Pre;
//    if(this->data.size() > 0){
//        print_debug();
//        //scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
//    }else{
//        print_debug();
//        //scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->minimum()+10);
//    }
//    //if( curr_page_cnt < 1) return;
//    if(this->data.size() > 0){

//        if(flowLayout->count() >= PAGE_LMT*20){

//            OffsetPoint -= this->data.size();
//            qDebug() << "#layout->count() = " << flowLayout->count();
//            GSCommon::clearLayout_album_onePage(flowLayout, flowLayout->count()-this->data.size(), flowLayout->count());
//        }
//        //currSliderPosition =  this->scrollArea->verticalScrollBar()->sliderPosition();

//        albumtCnt->setText(QString("Albums %1").arg(total_db_cnt));//c230304
//        //albumPage->setText(QString("Album : %2 ~ %3").arg(OffsetPoint_Pre+1).arg(OffsetPoint));
//        albumPage->setText(QString("Album : %2 ~ %3").arg(1).arg(OffsetPoint));
//        this->slider_playbar->setMaximum(total_page_cnt);
//        //GSCommon::clearLayout(flowLayout);

//        print_debug();
//        for(int i = 0 ; i < this->data.size(); i++){
//            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

//            QJsonObject jsonData = this->data.at(i).toJsonObject();
//            jsonData.insert("section", "album");
//            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

//            //wg_album->setData(datAI.at(i).toJsonObject());
//            wg_album->setData(jsonData);
//            wg_album->setHover();
//            if(i == 0 || i == this->data.size()-1){
//                //wg_album_tmp = new FrameMusicAlbum();
//                //wg_album_tmp = wg_album;
//                //wg_album->setBackground_label_base();//c220718

//            }

//            flowLayout->insertWidget(i,wg_album);
//        }
//        //this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->maximum()/2+150)/92)*92);


//    }else{
//        if(total_db_cnt==0){//c220511
//            print_debug();
//            GSCommon::clearLayout(flowLayout);
//            QLabel *albumNone = new QLabel;//c220402
//            albumNone->setText(QString(tr("\n\n\n                    There is no album.")));
//            albumNone->setStyleSheet("font-size:25px; color:#dddddd;");
//            flowLayout->addWidget(albumNone);
//        }
//    }
//    this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->minimum()+2700)/92)*92);

//}

//void MusicAlbum::getAlbumDataFromDB_view(){//c221109
//    print_debug();
//    qDebug() << "layout->count() = " << flowLayout->count();
//    currOffsetPoint = OffsetPoint;
//    if( curr_page_cnt > total_page_cnt) {
//        print_debug();
//        return;
//    }
//    //qDebug() << line_bottom_33->width();
//    double nnn = (global.LmtCnt-33-15-20)/(186+18);
//    int nn = nnn;
//    int mod_nn = (global.LmtCnt-33-15-20)%(186+18);
//    double fff = mod_nn/186;
//    int ff = fff;
//    nn = nn + ff;

//    qDebug() << "global.LmtCnt = " << global.LmtCnt;
//    qDebug() << "mod_nn = " << mod_nn;
//    qDebug() << "ff = " << ff;
//    qDebug() << "nn = " << nn;
//    //int scrol_cnt = PAGE_LMT/nn;
//    if(total_db_cnt > 1000){
//        for(int i = 0 ; nn*i <= 1000 ; i++){
//            PAGE_LMT = nn * i;
//        }

//    }else{
//        PAGE_LMT = nn * 10;
//    }


//    qDebug() << "1-OffsetPoint = " << OffsetPoint;
//    getAlbumDataFromDB();
//    qDebug() << "2-OffsetPoint = " << OffsetPoint;

//    if(this->data.size() > 0){
//        albumtCnt->setText(QString("Albums %1").arg(total_db_cnt));//c230304
//        //albumPage->setText(QString("Album : %2 ~ %3").arg(OffsetPoint_Pre+1).arg(OffsetPoint));
//        albumPage->setText(QString("Album : %2 ~ %3").arg(1).arg(OffsetPoint));
//        this->slider_playbar->setMaximum(total_page_cnt);

//        print_debug();
//        qDebug() << "curr_page_cnt = " << curr_page_cnt;
//        qDebug() << "flowLayout->count()/LMT_CNT = " << flowLayout->count()/LMT_CNT;

//        qDebug() << "after delete---layout->count() = " << flowLayout->count();
//        qDebug() << "after delete---this->data.size() = " << this->data.size();
//        //c230210 start
//#if defined(Q_OS_WINDOWS)
//        for(int i = 0 ; i < this->data.size(); i++){
//            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

//            QJsonObject jsonData = this->data.at(i).toJsonObject();
//            jsonData.insert("section", "album");
//            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

//            //wg_album->setData(datAI.at(i).toJsonObject());
//            wg_album->setData(jsonData);
//            wg_album->setHover();
//            if(i == 0 || i == this->data.size()-1){
//                //wg_album->setBackground_label_base();

//            }
//            flowLayout->addWidget(wg_album);
//            //qDebug() << "wg_albumwidth =" << wg_album->width();
//            //qDebug() << "this->scrollAreawidth = " << this->scrollArea->width();

//            QCoreApplication::processEvents();
//        }
//#endif

//#if defined(Q_OS_MAC)
//        FrameMusicAlbum *wg_album[this->data.size()];
//        for(int i = 0 ; i < this->data.size(); i++){
//            wg_album[i] = new FrameMusicAlbum();

//        }
//        for(int i = 0 ; i < this->data.size(); i++){
//            QJsonObject jsonData = this->data.at(i).toJsonObject();
//            jsonData.insert("section", "album");
//            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

//            //wg_album->setData(datAI.at(i).toJsonObject());
//            wg_album[i]->setData(jsonData);
//            wg_album[i]->setHover();
//            if(i == 0 || i == this->data.size()-1){
//                //wg_album->setBackground_label_base();

//            }
//            flowLayout->addWidget(wg_album[i]);
//            //qDebug() << "wg_albumwidth =" << wg_album->width();
//            //qDebug() << "this->scrollAreawidth = " << this->scrollArea->width();

//        }
//#endif
//        //c230210 end
//        print_debug();
//        qDebug() << this->scrollArea->maximumViewportSize();
//        qDebug() << "flowLayout->count() = " << flowLayout->count();
//        qDebug() << "data->count() = " << this->data.size();
//        qDebug() << "currSliderPosition = " << this->scrollArea->verticalScrollBar()->sliderPosition();
//        qDebug() << "maximum = " << scrollArea->verticalScrollBar()->maximum();
//        currSliderPosition =  this->scrollArea->verticalScrollBar()->sliderPosition();
//        if(this->data.size() < PAGE_LMT && flowLayout->count() < PAGE_LMT){
//            //this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->minimum()+92)/92)*92);
//        }else{
//            if(flowLayout->count() < PAGE_LMT*20){
//               // this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->maximum())/92)*92);
//            }
//        }

//        qDebug() << "***currSliderPosition = " << this->scrollArea->verticalScrollBar()->sliderPosition();
//        if(flowLayout->count() >= PAGE_LMT*20){
//            print_debug();
//            //OffsetPoint_Pre += PAGE_LMT;
//            //GSCommon::clearLayout_album_onePage(flowLayout, 0, PAGE_LMT);
//            //this->scrollArea->verticalScrollBar()->setValue(((scrollArea->verticalScrollBar()->maximum()-270*10)/92)*92);

//        }



//    }else{
//        if(total_db_cnt == 0){//c220511
//            print_debug();
//            GSCommon::clearLayout(flowLayout);
//            QLabel *albumNone = new QLabel;
//            albumNone->setText(QString(tr("\n\n\n                    There is no album.")));
//            albumNone->setStyleSheet("font-size:25px; color:#dddddd;");
//            flowLayout->addWidget(albumNone);
//        }

//    }
//    if(this->data.size() > 0){
//    }else{
//    }
//    //getAlbumDataFromDB();


//}


//void MusicAlbum::slot_sliderbarValue_check(){//c22040501

// //0717 mtx.lock();
//    if(!global.device.getDeviceIP().isEmpty()){
//        //print_debug();

//        //curr_LmtWidth = global.LmtCnt;
//        currSliderPosition =  this->scrollArea->verticalScrollBar()->sliderPosition();

//        //-qDebug() << "sliderPosition=" << currSliderPosition;
//        //-qDebug() << "pre---curr_page_cnt=" << curr_page_cnt;
//        //-qDebug() << "data.size()=" << data.size();
//        //qDebug() << "scrollArea->verticalScrollBar()->maximum()=" << scrollArea->verticalScrollBar()->maximum();
//        //-qDebug() << "scrollArea->verticalScrollBar()->minimum()=" << scrollArea->verticalScrollBar()->minimum();


//        //if(data.size() >= PAGE_LMT && ((curr_page_cnt == 1 && scrollArea->verticalScrollBar()->maximum()==currSliderPosition) || (scrollArea->verticalScrollBar()->maximum())==currSliderPosition)){

//        if((scrollArea->verticalScrollBar()->maximum()==currSliderPosition)){//c220511

//                getAlbumDataFromDB_view();

//        }else if(OffsetPoint_Pre > 0 && scrollArea->verticalScrollBar()->minimum()==currSliderPosition) {//c220511

//           // getAlbumDataFromDB_pre_view();

//        }else {
//            //qDebug() << "scrollArea->verticalScrollBar()->maximum()=" << scrollArea->verticalScrollBar()->maximum();
//            //print_debug();
//        }

//    }
//    //0717 mtx.unlock();

//}


//void MusicAlbum::slot_sliderPressed(){
//    print_debug();
//    qDebug() << "pressed----------------------------------------------";
//    qDebug() << "this->scrollArea->verticalScrollBar()->sliderPosition()=" << this->scrollArea->verticalScrollBar()->sliderPosition();
//}
//void MusicAlbum::slot_sliderReleased(){
//    print_debug();
//    qDebug() << "Released----------------------------------------------";
//    qDebug() << "this->scrollArea->verticalScrollBar()->sliderPosition()=" << this->scrollArea->verticalScrollBar()->sliderPosition();
//}
///**
// * @brief SearchRemoteBar::slot_returnPressed
// * @param p_text
// */
//void MusicAlbum::slot_returnPressed(){//22040301
//    print_debug();
//    QString tmp_page =this->le_lisearch->text();
//    //qDebug() << "slot_returnPressed-p_text" << tmp_page;

//    this->goto_setDataFromDB(tmp_page.toInt());
//    this->le_lisearch->setText(QString("%1").arg(curr_p_value));
//}

//void MusicAlbum::slot_clickedChange(){//22040301
//    QString tmp_page =this->le_lisearch->text();
//    //qDebug() << "slot_clickedChange-p_text" << tmp_page;

//    this->goto_setDataFromDB(tmp_page.toInt());
//    this->le_lisearch->setText(QString("%1").arg(curr_p_value));
//}

//void MusicAlbum::slot_returnPressed_str(){//22040301
//    QString tmp_page =this->le_lisearch_str->text();
//    qDebug() << "slot_returnPressed_str-p_text" << tmp_page;

//    setDataFromDB_Alpa(tmp_page);
//    this->le_lisearch_str->setText(QString("%1").arg(tmp_page));
//}

//void MusicAlbum::slot_clickedChange_str(){//22040301
//    QString tmp_page =this->le_lisearch_str->text();
//    qDebug() << "slot_clickedChange_str-p_text" << tmp_page;
//    setDataFromDB_Alpa(tmp_page);
//    this->le_lisearch_str->setText(QString("%1").arg(tmp_page));
//}

//void MusicAlbum::setDataFromDB_Alpa(const QString &p_arg){//c220323

//    this->setContentsMargins(0,10,0,0);

//    strArg_1 = p_arg;
//    print_debug();
//    qDebug() << "strArg_1 = " << strArg_1;
//    slot_sliderbarValue2_flag = false;
//    total_db_cnt = getAlbumDataFromDBforTotalCount();
//    flag_page_first = true;
//    OffsetPoint = 0;
//    curr_page_cnt = 0;

//    GSCommon::clearLayout(flowLayout);//c220416
//    getAlbumDataFromDB_view();
//}

///**
// * @brief MusicAlbum::setFIlterUIControl : Filter 선택 UI 세팅
// */
//void MusicAlbum::setFIlterUIControl(){

//    QLabel *lb_fileType = createFilterLabel("File Type");
//    QLabel *lb_bitDepth = createFilterLabel("Bit Depth");
//    QLabel *lb_sampling = createFilterLabel("Sampling Rate");
//    QLabel *lb_star = createFilterLabel(tr("Star Rating"));
//    //QLabel *lb_star = createFilterLabel(tr("별점"));
//    QLabel *lb_order = createFilterLabel(tr("Sort order"));
//    //QLabel *lb_order = createFilterLabel(tr("정렬순서"));


//    QHBoxLayout *box_header = new QHBoxLayout;
//    box_header->setContentsMargins(0,0,0,0);
//    box_header->setSpacing(0);
//    box_header->addWidget(lb_fileType,1);
//    box_header->addWidget(lb_bitDepth,1);
//    box_header->addWidget(lb_sampling,1);
//    box_header->addWidget(lb_star,1);
//    box_header->addWidget(lb_order,1);

//    QFrame *line = new QFrame;
//    line->setFrameShape(QFrame::HLine);
//    line->setFixedHeight(1);
//    line->setLineWidth(1);
//    line->setStyleSheet("background-color:#4D4D4D;border:none;");

//    btn_file_type[0] = createFilterBtn("FileType", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() == 0 ? true:false);
//    btn_file_type[1] = createFilterBtn("FileType", 1, tr("Lossless"), settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() == 1 ? true:false);

//    QVBoxLayout *box_fileType = new QVBoxLayout;
//    box_fileType->setContentsMargins(0,0,20,0);
//    box_fileType->setSpacing(20);
//    box_fileType->setAlignment(Qt::AlignTop);
//    box_fileType->addWidget(btn_file_type[0]);
//    box_fileType->addWidget(btn_file_type[1]);
//    //box_fileType->addWidget(createFilterBtn("FileType", 0, tr("All"),true));
//    //box_fileType->addWidget(createFilterBtn(tr("전체"),false));
//    //box_fileType->addWidget(createFilterBtn("FileType", 1, tr("Lossless"),false));
//    //box_fileType->addWidget(createFilterBtn(tr("무손실"),true));

//    btn_bit_depth[0] = createFilterBtn("BitDepth", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() == 0 ? true:false);
//    btn_bit_depth[1] = createFilterBtn("BitDepth", 1, "24bit↑", settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() == 1 ? true:false);

//    QVBoxLayout *box_bitDepth = new QVBoxLayout;
//    box_bitDepth->setContentsMargins(0,0,20,0);
//    box_bitDepth->setSpacing(20);
//    box_bitDepth->setAlignment(Qt::AlignTop);
//    box_bitDepth->addWidget(btn_bit_depth[0]);
//    box_bitDepth->addWidget(btn_bit_depth[1]);
//    //box_bitDepth->addWidget(createFilterBtn("BitDepth", 0, tr("All"),true));
//    //box_bitDepth->addWidget(createFilterBtn(tr("전체"),false));
//    //box_bitDepth->addWidget(createFilterBtn("BitDepth", 1, "24bit↑",false));
//    //box_bitDepth->addWidget(createFilterBtn("24bit↑",true));

//    btn_sample_rate[0] = createFilterBtn("Sampling", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 0 ? true:false);
//    btn_sample_rate[1] = createFilterBtn("Sampling", 1, "88.2kHz↑", settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 1 ? true:false);
//    btn_sample_rate[2] = createFilterBtn("Sampling", 2, "176.4kHz↑", settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 2 ? true:false);

//    QVBoxLayout *box_sampling = new QVBoxLayout;
//    box_sampling->setContentsMargins(0,0,20,0);
//    box_sampling->setSpacing(20);
//    box_sampling->setAlignment(Qt::AlignTop);
//    box_sampling->addWidget(btn_sample_rate[0]);
//    box_sampling->addWidget(btn_sample_rate[1]);
//    box_sampling->addWidget(btn_sample_rate[2]);
//    //box_sampling->addWidget(createFilterBtn("Sampling", 0, tr("All"),true));
//    //box_sampling->addWidget(createFilterBtn(tr("전체"),false));
//    //box_sampling->addWidget(createFilterBtn("Sampling", 1, "88.2kHz↑",false));
//    //box_sampling->addWidget(createFilterBtn("Sampling", 2, "176.4kHz↑",false));

//    settings->setValue(rosesettings.SETTINGS_AFILTER_STAR, 0);

//    btn_star_point[0] = createFilterBtn("Star", 0, tr("All"), settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 0 ? true:false);
//    btn_star_point[1] = createFilterBtn("Star", 1, "★★★★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 1 ? true:false);
//    btn_star_point[2] = createFilterBtn("Star", 2, "★★★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 2 ? true:false);
//    btn_star_point[3] = createFilterBtn("Star", 3, "★★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 3 ? true:false);
//    btn_star_point[4] = createFilterBtn("Star", 4, "★★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 4 ? true:false);
//    btn_star_point[5] = createFilterBtn("Star", 5, "★↑", settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 5 ? true:false);
//    btn_star_point[6] = createFilterBtn("Star", 6, tr("NONE"), settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 6 ? true:false);

//    QVBoxLayout *box_star = new QVBoxLayout;
//    box_star->setContentsMargins(0,0,20,0);
//    box_star->setSpacing(20);
//    box_star->setAlignment(Qt::AlignTop);
//    box_star->addWidget(btn_star_point[0]);
//    //box_star->addWidget(btn_star_point[1]);
//    //box_star->addWidget(btn_star_point[2]);
//    //box_star->addWidget(btn_star_point[3]);
//    //box_star->addWidget(btn_star_point[4]);
//    //box_star->addWidget(btn_star_point[5]);
//    //box_star->addWidget(btn_star_point[6]);
//    //box_star->addWidget(createFilterBtn("Star", 0, tr("All"),true));
//    //box_star->addWidget(createFilterBtn("Star", 1, "★★★★★↑",false));
//    //box_star->addWidget(createFilterBtn("Star", 2, "★★★★↑",false));
//    //box_star->addWidget(createFilterBtn("Star", 3, "★★★↑",false));
//    //box_star->addWidget(createFilterBtn("Star", 4, "★★↑",false));
//    //box_star->addWidget(createFilterBtn("Star", 5, "★↑",false));
//    //box_star->addWidget(createFilterBtn("Star", 6, tr("NONE"),false));

//    btn_order[0] = createFilterBtn("OrderBy", 0, tr("NONE"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 0 ? true:false);
//    btn_order[1] = createFilterBtn("OrderBy", 1, tr("A-Z"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1 ? true:false);
//    btn_order[2] = createFilterBtn("OrderBy", 2, tr("Artist"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2 ? true:false);
//    btn_order[3] = createFilterBtn("OrderBy", 3, tr("Favorite"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3 ? true:false);
//    btn_order[4] = createFilterBtn("OrderBy", 4, tr("Random"), settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 4 ? true:false);//c221109

//    QVBoxLayout *box_order = new QVBoxLayout;
//    box_order->setContentsMargins(0,0,0,0);
//    box_order->setSpacing(20);
//    box_order->setAlignment(Qt::AlignTop);
//    box_order->addWidget(btn_order[0]);
//    box_order->addWidget(btn_order[1]);
//    box_order->addWidget(btn_order[2]);
//    box_order->addWidget(btn_order[3]);
//    //box_order->addWidget(btn_order[4]);//c221109
//    //box_order->addWidget(createFilterBtn(tr("Recently order"),false));
//    //box_order->addWidget(createFilterBtn(tr("최근순"),false));
//    //box_order->addWidget(createFilterBtn("OrderBy", 0, tr("A-Z"),true));
//    //box_order->addWidget(createFilterBtn(tr("이름순"),false));
//    //box_order->addWidget(createFilterBtn("OrderBy", 1, tr("Artist"),false));
//    //box_order->addWidget(createFilterBtn(tr("아티스트순"),false));
//    //box_order->addWidget(createFilterBtn("OrderBy", 2, tr("Favorite"),false));
//    //box_order->addWidget(createFilterBtn(tr("선호도순"),true));

//    QHBoxLayout *box_body = new QHBoxLayout;
//    box_body->setContentsMargins(20,20,20,20);
//    box_body->setSpacing(0);
//    box_body->addLayout(box_fileType);
//    box_body->addLayout(box_bitDepth);
//    box_body->addLayout(box_sampling);
//    box_body->addLayout(box_star);
//    box_body->addLayout(box_order);

//    QWidget *btn_Apply = new QPushButton(tr("Apply"));
//    btn_Apply->setObjectName("btn_Apply");
//    btn_Apply->setCursor(Qt::PointingHandCursor);
//    btn_Apply->setFixedSize(70,26);            //45, 50
//    btn_Apply->setStyleSheet("#btn_Apply {font-weight:200; font-size:14px; color:#FFFFFF;  background-color:transparent; border:1px solid #FFFFFF;border-radius:13px;  } ");

//    QWidget *btn_Cancel = new QPushButton(tr("Cancel"));
//    btn_Cancel->setObjectName("btn_Cancel");
//    btn_Cancel->setCursor(Qt::PointingHandCursor);
//    btn_Cancel->setFixedSize(70,26);            //45, 50
//    btn_Cancel->setStyleSheet("#btn_Cancel {font-weight:200; font-size:14px; color:#FFFFFF;  background-color:transparent; border:1px solid #FFFFFF;border-radius:13px;  } ");

//    connect(btn_Apply, SIGNAL(clicked()), this, SLOT(slot_filter_apply()));
//    //connect(btn_Cancel, SIGNAL(clicked()), this, SLOT(slot_filter_cancel()));

//    QHBoxLayout *hl_box_body_btn = new QHBoxLayout;
//    hl_box_body_btn->setContentsMargins(250,20,250,20);
//    hl_box_body_btn->setSpacing(0);
//    hl_box_body_btn->addWidget(btn_Apply);
//    //hl_box_body_btn->addWidget(btn_Cancel);

//    QVBoxLayout *box_total = new QVBoxLayout;
//    box_total->setContentsMargins(0,0,0,0);
//    box_total->setSpacing(0);
//    box_total->setAlignment(Qt::AlignTop);
//    box_total->addLayout(box_header);
//    box_total->addWidget(line);
//    box_total->addLayout(box_body);
//    box_total->addWidget(line);
//    box_total->addLayout(hl_box_body_btn);

//    widget_filter = new QWidget(this);
//    widget_filter->setObjectName("widget_filter");
//    widget_filter->setStyleSheet("#widget_filter { background-color:#333333;border:1px solid #4D4D4D; border-radius:3px; }");
//    widget_filter->setFixedSize(700,360);
//    widget_filter->move(90,10);
//    widget_filter->setContentsMargins(1,1,1,1);
//    widget_filter->setLayout(box_total);

//    widget_filter->hide();
//}

//void MusicAlbum::setFIlterUIControl_cancel(){

//}



//void MusicAlbum::slot_filter_apply(){
//    print_debug();

//    slot_clickedBtnFilter();
//     widget_filter->hide();

//}

//void MusicAlbum::slot_filter_cancel(){
//    print_debug();
//}

///**
// * @brief MusicAlbum::createFilterLabel : Filter 선택 UI의 필터 라벨 반환
// * @param p_label QString 라벨명
// * @return QLabel*
// */
//QLabel* MusicAlbum::createFilterLabel(const QString &p_label){
//    QLabel *lb_filter = new QLabel;
//    lb_filter->setText(p_label);
//    lb_filter->setFixedHeight(48);
//    lb_filter->setStyleSheet("color:#CCCCCC;font-size:14px;background-color:transparent;");
//    lb_filter->setAlignment(Qt::AlignCenter);

//    return lb_filter;
//}

///**
// * @brief MusicAlbum::createFilterBtn : Filter 선택 UI의 필터 버튼 반환
// * @param p_text QString 버튼명
// * @param flagSelected bool if true selected 상태 지정
// * @return QPushButton*
// */
///*QPushButton* MusicAlbum::createFilterBtn(const QString &p_text, bool flagSelected){

//    QPushButton *btn = new QPushButton;
//    btn->setText(p_text);
//    btn->setFixedSize(QSize(110,30));
//    btn->setStyleSheet(flagSelected ? btnStyle_selected : btnStyle_normal);
//    btn->setCursor(Qt::PointingHandCursor);

//    return btn;
//}*/
//QPushButton* MusicAlbum::createFilterBtn(const QString p_filter, const int idx, const QString &p_text, bool flagSelected){

//    QPushButton *btn = new QPushButton;
//    btn->setText(p_text);
//    btn->setFixedSize(QSize(110,30));
//    btn->setStyleSheet(flagSelected ? btnStyle_selected : btnStyle_normal);
//    btn->setProperty(KEY_AFILTER, p_filter);
//    btn->setProperty(KEY_AIDX, idx);
//    btn->setCursor(Qt::PointingHandCursor);
//    connect(btn, SIGNAL(clicked()), this, SLOT(slot_changedBtnFilter()));

//    return btn;
//}

///**
// * @brief MusicAlbum::setDataFromDB DB에서 가져와서 전체 데이터 세팅
// */
//void MusicAlbum::setDataFromDB(){//c220323
//    // 초기화
//    strArg_1 = "";
//    total_db_cnt = getAlbumDataFromDBforTotalCount();
//    slot_sliderbarValue2_flag = false;
//    GSCommon::clearLayout(flowLayout);
//    flag_page_first = true;
//    OffsetPoint = 0;
//    curr_page_cnt = 0;

//    print_debug();
//    qDebug() << "this->scrollArea->verticalScrollBar()->singleStep=" << this->scrollArea->verticalScrollBar()->singleStep();
//    getAlbumDataFromDB_view();

//    this->le_lisearch_str->setText(QString("%1").arg(""));
///*
//    this->timer = new QTimer(this);
//    this->timer->setInterval(200); // 1초
//    connect(this->timer, SIGNAL(timeout()), SLOT(slot_sliderbarValue_check()));
//    this->timer->start();
//    */
//}

//void MusicAlbum::goto_setDataFromDB(int p_page){//c220323
//    // 초기화
//    //if(slot_sliderbarValue2_flag == true) return;
////slot_sliderbarValue2_flag = true;
//    goto_getAlbumDataFromDB(p_page);
//    goto_getAlbumDataFromDB_view();
//    //slot_sliderbarValue2_flag = false;
//}

///**
// * @brief MusicAlbum::getAlbumDataFromDB : GET 앨범 데이터
// */
//void MusicAlbum::goto_getAlbumDataFromDB(int p_page){//cheon210527
//    //this->scrollArea->verticalScrollBar()->setSingleStep(180);
//    if( total_page_cnt < p_page || p_page <= 0 ) {
//        ToastMsg::show(this, "", tr("There are no pages.."), 5000);//cheon210527
//        return;
//    }
//    int cnt = 0;
//    LMT_CNT = 0;
//    int hwindow_width = global.LmtCnt-33-18-30;// - LEFT_WIDTH;
//    cnt = hwindow_width/ (ALBUMWIDTH);

//    int cnt_mode = hwindow_width % (ALBUMWIDTH);
//    qDebug() << "cnt = " << cnt;
//    qDebug() << "cnt_mode = " << cnt_mode;
//    int tmp = cnt_mode/(ALBUMWIDTH-18);
//    cnt = cnt + tmp;
//    /*
//    if(global.LmtCnt_H > 1030){
//        LMT_CNT = cnt * global.LmtCnt_H/70;
//        this->scrollArea->verticalScrollBar()->setSingleStep(270);//90*3);
//    }else if(global.LmtCnt_H < 580) {
//        this->scrollArea->verticalScrollBar()->setSingleStep(270);//90);
//        LMT_CNT = cnt * PAGE_LMT;
//    }else{
//        this->scrollArea->verticalScrollBar()->setSingleStep(270);//90*2);
//        */
//    cnt = 1;
//        LMT_CNT = cnt * PAGE_LMT;
//    //}
//    pre_lmt_cnt = LMT_CNT;

//    // DB 쿼리
//    SqliteHelper *sqlite = new SqliteHelper(this);
//    QSqlError err = sqlite->addConnectionRose();
//    if(err.type() == QSqlError::NoError){
//        if(flag_page_first == true){
//            OffsetPoint += LMT_CNT;
//            flag_page_first = false;
//        }else{
//            OffsetPoint = (p_page - 1) * LMT_CNT;
//        }

//        if(OffsetPoint == 0){
//            curr_page_cnt = 1;
//        }
//        else{
//            curr_page_cnt = OffsetPoint / LMT_CNT + 1;
//        }
//        before_album_page = curr_page_cnt;

//        total_page_cnt = 0;
//        if((total_db_cnt % LMT_CNT) > 0){
//            total_page_cnt = (total_db_cnt / LMT_CNT) + 1;
//        }
//        else if((total_db_cnt % LMT_CNT) == 0){
//            total_page_cnt = (total_db_cnt / LMT_CNT);
//        }

//        print_debug();
//        qDebug() << "OffsetPoint =" << OffsetPoint;
//        //albumtCnt->setText(QString("Total Count : %1                            Page : %2 / %3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));
//        //albumtCnt->setText(QString("Total List Count : %1                            Page : %2 / %3").arg(total_db_cnt, (OffsetPoint+1)/LMT_CNT+1, total_page_cnt));

//        //qDebug() << "OffsetPoint" << OffsetPoint;
//        QString strQuery = "";
//        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
//        strQuery += " FROM album_info AS AI ";
//        strQuery += " LEFT OUTER JOIN album_art AS AA ";
//        strQuery += " ON AI._id=AA.album_id ";
//        //strQuery += " ORDER BY AI.date_added desc ";
//        //strQuery += " ORDER BY AI.album asc ";
//        //strQuery += " LIMIT 100 OFFSET " + QString::number(flowLayout->count());
//        //strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(flowLayout->count());

//        QString setWhere = "";
//        QString setOrder = "";


//        if(!strArg_1.isEmpty()){
//            print_debug();
//            qDebug() << "strArg_1 = " << strArg_1;

//            //strQuery += "(";
//            strQuery += searchcharactorSqlWhere();
//            //strQuery += ")";

//            setWhere = getSetWhereFilter();
//            if(!setWhere.isEmpty()){
//                strQuery += " AND ";
//                strQuery += setWhere;
//            }

//            strQuery += " ORDER BY AI.album ";
//        }
//        else{


//            setWhere = getSetWhereFilter();

//            if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
//                if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 1){
//                    setOrder += " ORDER BY AI.album ";
//                }
//                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 2){
//                    setOrder += " ORDER BY AI.artist ";
//                }
//                else if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() == 3){
//                    setOrder += " ORDER BY AI.favorite ";
//                }
//            }

//            if(!setWhere.isEmpty()){
//                strQuery += " WHERE";
//                strQuery += setWhere;
//            }
//            if(!setOrder.isEmpty()){
//                strQuery += setOrder;
//            }
//            //strQuery += " ORDER BY AI.album ";

//        }
//        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

//        //QVariantList data;
//        this->data = QVariantList();
//        sqlite->exec(strQuery, this->data);




//        if(this->data.size() > 0){
//        }else{

//        }
//        /*
//        //scrollArea->verticalScrollBar()->setValue(0);

//        if(data.size() > 0){
//            GSCommon::clearLayout(flowLayout);
//            for(int i = 0 ; i < data.size(); i++){
//                FrameMusicAlbum *wg_album = new FrameMusicAlbum();

//                QJsonObject jsonData = data.at(i).toJsonObject();
//                jsonData.insert("section", "album");

//                //wg_album->setData(datAI.at(i).toJsonObject());
//                wg_album->setData(jsonData);
//                wg_album->setHover();
//                flowLayout->addWidget(wg_album);
//            }
//        }
//        */
//        sqlite->close();

//    }

//    delete sqlite;
//}



//QString MusicAlbum::searchcharactorSqlWhere(){
//    QString strQuery;
//    QString strArg_space = " ";
//    if(strArg_1.size() > 1){
//        /*
//        if(strArg_1 == "etc"){
//            print_debug();qDebug() << "etc";
//            //strQuery += " WHERE (AI.artist < '가' and AI.artist > '힣' OR AI.album < '가' and AI.album > '힣') and (UPPER(AI.artist) < 'A' and UPPER(AI.artist) > 'Z' OR UPPER(AI.album) < 'A' and UPPER(AI.album) > 'Z')";
//            //strQuery += " WHERE (AI.album < '가' and AI.album > '힣') and UPPER(AI.album) < 'A' and UPPER(AI.album) > 'Z'";
//            strQuery += " WHERE AI.album >= '가' and AI.album < '힣'";


//        }
//        else {
//            strQuery += " WHERE AI.artist LIKE '%" + strArg_1 + "%' OR AI.album LIKE '%" + strArg_1+ "%'";
//        }
//        */
//        strQuery += " WHERE (AI.artist LIKE '%" + strArg_1 + "%' OR AI.album LIKE '%" + strArg_1+ "%')";

//    }else{
//        if(strArg_1 == "ㄱ"){
//            strQuery += " WHERE (AI.artist >= '가' and AI.artist < '나' OR AI.album >= '가' and AI.album < '나')";

//        }else if(strArg_1 == "ㄴ"){
//            strQuery += " WHERE (AI.artist >= '나' and AI.artist < '다' OR AI.album >= '가' and AI.album < '다')";

//        }else if(strArg_1 == "ㄷ"){
//            strQuery += " WHERE (AI.artist >= '다' and AI.artist < '라' OR AI.album >= '다' and AI.album < '라')";

//        }else if(strArg_1 == "ㄹ"){
//            strQuery += " WHERE (AI.artist >= '라' and AI.artist < '마' OR AI.album >= '라' and AI.album < '마')";

//        }else if(strArg_1 == "ㅁ"){
//            strQuery += " WHERE (AI.artist >= '마' and AI.artist < '바' OR AI.album >= '마' and AI.album < '바')";

//        }else if(strArg_1 == "ㅂ"){
//            strQuery += " WHERE (AI.artist >= '바' and AI.artist < '사' OR AI.album >= '바' and AI.album < '사')";

//        }else if(strArg_1 == "ㅅ"){
//            strQuery += " WHERE (AI.artist >= '사' and AI.artist < '아' OR AI.album >= '사' and AI.album < '아'";

//        }else if(strArg_1 == "ㅇ"){
//            strQuery += " WHERE (AI.artist >= '아' and AI.artist < '자' OR AI.album >= '아' and AI.album < '자')";

//        }else if(strArg_1 == "ㅈ"){
//            strQuery += " WHERE (AI.artist >= '자' and AI.artist < '차' OR AI.album >= '자' and AI.album < '차')";

//        }else if(strArg_1 == "ㅊ"){
//            strQuery += " WHERE (AI.artist >= '차' and AI.artist < '카' OR AI.album >= '차' and AI.album < '카')";

//        }else if(strArg_1 == "ㅋ"){
//            strQuery += " WHERE AI.artist >= '카' and AI.artist < '타' OR AI.album >= '카' and AI.album < '타')";

//        }else if(strArg_1 == "ㅌ"){
//            strQuery += " WHERE (AI.artist >= '타' and AI.artist < '파' OR AI.album >= '타' and AI.album < '파')";

//        }else if(strArg_1 == "ㅍ"){
//            strQuery += " WHERE (AI.artist >= '파' and AI.artist < '하' OR AI.album >= '파' and AI.album < '하')";

//        }else if(strArg_1 == "ㅎ"){
//            strQuery += " WHERE (AI.artist >= '하' and AI.artist < '힣' OR AI.album >= '하' and AI.album < '힣')";

//        }else {
//            print_debug();
//            qDebug() << "strArg_1=" << strArg_1;
//            //strQuery += " WHERE UPPER(replace(AI.artist, '-','')) LIKE '"+ strArg_1 + "%' OR UPPER(replace(AI.album, '-','')) LIKE '" + strArg_1+ "%'";
//            strQuery += " WHERE (UPPER(substr(AI.album, instr(AI.album, ' ')+1,2)) LIKE '"+ strArg_1 + "%' OR UPPER(AI.artist) LIKE '"+ strArg_1 + "%' OR (UPPER(AI.album) LIKE '" + strArg_1+ "%' AND UPPER(substr(AI.album, 2,2)) NOT LIKE '" + strArg_space+ "%'))";

//        }
//    }
//    return strQuery;
//}

///**
// * @brief MusicAlbum::getAlbumDataFromDB : GET 앨범 데이터
// */


////strQuery += " WHERE UPPER(AI.artist) LIKE '"+ strArg_1 + "%' OR UPPER(AI.album) LIKE '" + strArg_1+ "%'";
////strQuery += " WHERE UPPER(replace(AI.artist, '-','')) LIKE '"+ strArg_1 + "%' OR UPPER(replace(AI.album, '-','')) LIKE '" + strArg_1+ "%'";
////SUBSTR('ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ',  ROUND((UNICODE(SUBSTR(data1,1,1)) - 44032 ) / 588) + 1 , 1)

////strQuery += " WHERE UPPER(AI.artist) LIKE '"+ SUBSTR('ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ',  ROUND((UNICODE(SUBSTR(data1,1,1)) - 44032 ) / 588) + 1 , 1) + "%' OR UPPER(AI.album) LIKE '" + SUBSTR('ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ',  ROUND((UNICODE(SUBSTR(data1,1,1)) - 44032 ) / 588) + 1 , 1)+ "%'";
////0xAC00
////strQuery += " WHERE (AI.artist RLIKE '^(ㄱ|ㄲ)' OR ( AI.artist >= '가' AND AI.artist < '나' ))";
////uint16_t cc = 0xAC00;
////strArg_1 = "가";




//void MusicAlbum::goto_getAlbumDataFromDB_view(){//c220323
//    print_debug();
//    qDebug() << "pre_mouse_drag_flag = " << pre_mouse_drag_flag;
//    double nnn = (global.LmtCnt-33-15-20)/(186+18);
//    int nn = nnn;
//    int mod_nn = (global.LmtCnt-33-15-20)%(186+18);
//    double fff = mod_nn/186;
//    int ff = fff;
//    nn = nn + ff;

//    qDebug() << "global.LmtCnt = " << global.LmtCnt;
//    qDebug() << "mod_nn = " << mod_nn;
//    qDebug() << "ff = " << ff;
//    qDebug() << "nn = " << nn;
//    //int scrol_cnt = PAGE_LMT/nn;
//    PAGE_LMT = nn * 10;
//    if(pre_mouse_drag_flag == 1){
//        print_debug();
//        getAlbumDataFromDB();
//    }
//    pre_mouse_drag_flag = 0;

//    if(this->data.size() > 0){

//        albumtCnt->setText(QString("Albums %1").arg(total_db_cnt));//c230304
//        albumPage->setText(QString("Page : %2 / %3").arg(curr_page_cnt).arg(total_page_cnt));
//        this->slider_playbar->setMaximum(total_page_cnt);
//        GSCommon::clearLayout(flowLayout);

//        for(int i = 0 ; i < this->data.size(); i++){
//            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

//            QJsonObject jsonData = this->data.at(i).toJsonObject();
//            jsonData.insert("section", "album");
//            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

//            //wg_album->setData(datAI.at(i).toJsonObject());
//            wg_album->setData(jsonData);
//            wg_album->setHover();
//            flowLayout->addWidget(wg_album);
//        }
//    }else{
//        print_debug();
//        GSCommon::clearLayout(flowLayout);
//        QLabel *albumNone = new QLabel;//c220402
//        albumNone->setText(QString(tr("\n\n\n                    There is no album.")));
//        albumNone->setStyleSheet("font-size:25px; color:#dddddd;");
//        flowLayout->addWidget(albumNone);

//    }
//    if(this->data.size() > 0){
//        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum()*0);
//    }else{
//        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
//    }
//    //getAlbumDataFromDB();

//}


//void MusicAlbum::getAlbumDataFromDB_view_only(){//c220323
//    print_debug();
//    qDebug() << "pre_mouse_drag_flag = " << pre_mouse_drag_flag;
//    qDebug() << "OffsetPoint = " << OffsetPoint;


//    getAlbumDataFromDB();

//    currOffsetPoint = OffsetPoint-(cnt_size*PAGE_LMT);
//    OffsetPoint = currOffsetPoint;
//    if(OffsetPoint<0) OffsetPoint = 0;

//    if(this->data.size() > 0){
//        albumtCnt->setText(QString("Albums %1").arg(total_db_cnt));//c230304
//        albumPage->setText(QString("Page : %2 / %3").arg(curr_page_cnt).arg(total_page_cnt));
//        this->slider_playbar->setMaximum(total_page_cnt);
//        GSCommon::clearLayout(flowLayout);

//        for(int i = 0 ; i < this->data.size(); i++){
//            FrameMusicAlbum *wg_album = new FrameMusicAlbum();

//            QJsonObject jsonData = this->data.at(i).toJsonObject();
//            jsonData.insert("section", "album");
//            //QJsonDocument doc1(jsonData);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << " MusicAlbum::getAlbumDataFromDB---jsonData_obj = " << strJson;

//            //wg_album->setData(datAI.at(i).toJsonObject());
//            wg_album->setData(jsonData);
//            wg_album->setHover();
//            flowLayout->addWidget(wg_album);
//            QCoreApplication::processEvents();
//        }
//    }
//    if(this->data.size() > 0){
//    }else{
//    }


//}




//QString MusicAlbum::getSetWhereFilter(){
//    QString setWhere;

//    if(settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() > 0){
//        setWhere += " ((AI.mime_type == 'audio/ape') || (AI.mime_type == 'audio/flac') || (AI.mime_type == 'audio/dff') || (AI.mime_type == 'audio/dsf') || (AI.mime_type == 'audio/wav') || (AI.mime_type == 'audio/x-wav')) ";
//    }
//    if(settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() > 0){
//        if(setWhere.isEmpty()){
//            setWhere += " (AI.bitdepth >= 24) ";
//        }
//        else{
//            setWhere += " AND (AI.bitdepth >= 24) ";
//        }
//    }
//    if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() > 0){
//        if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 1){
//            if(setWhere.isEmpty()){
//                setWhere += " (AI.samplerate >= 88200) ";
//            }
//            else{
//                setWhere += " AND (AI.samplerate >= 88200) ";
//            }
//        }
//        else if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() == 2){
//            if(setWhere.isEmpty()){
//                setWhere += " (AI.samplerate >= 176400) ";
//            }
//            else{
//                setWhere += " AND (AI.samplerate >= 176400) ";
//            }
//        }
//    }
//    if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
//        if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() == 6){
//            if(setWhere.isEmpty()){
//                setWhere += " (AI.songrate == 0) ";
//            }
//            else{
//                setWhere += " AND (AI.songrate == 0) ";
//            }
//        }
//        else{
//            switch (settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt()) {
//            case 0:
//            case 6:
//                break;
//            case 1:
//                if(setWhere.isEmpty()){
//                    setWhere += " (AI.songrate == 5) ";
//                }
//                else{
//                    setWhere += " AND (AI.songrate == 5) ";
//                }
//                break;
//            case 2:
//                if(setWhere.isEmpty()){
//                    setWhere += " (AI.songrate == 4) ";
//                }
//                else{
//                    setWhere += " AND (AI.songrate == 4) ";
//                }
//                break;
//            case 3:
//                if(setWhere.isEmpty()){
//                    setWhere += " (AI.songrate == 3) ";
//                }
//                else{
//                    setWhere += " AND (AI.songrate == 3) ";
//                }
//                break;
//            case 4:
//                if(setWhere.isEmpty()){
//                    setWhere += " (AI.songrate == 2) ";
//                }
//                else{
//                    setWhere += " AND (AI.songrate == 2) ";
//                }
//                break;
//            case 5:
//                if(setWhere.isEmpty()){
//                    setWhere += " (AI.songrate == 1) ";
//                }
//                else{
//                    setWhere += " AND (AI.songrate == 1) ";
//                }
//                break;
//            }
//        }
//    }
//    return setWhere;
//}
//int MusicAlbum::getAlbumDataFromDBforTotalCount(){
//    int ret_cnt = 0;

//    // DB 쿼리
//    SqliteHelper *sqlite = new SqliteHelper(this);
//    QSqlError err = sqlite->addConnectionRose();
//    if(err.type() == QSqlError::NoError){
//        QString strQuery = "";

//        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
//        strQuery += " FROM album_info AS AI ";
//        strQuery += " LEFT OUTER JOIN album_art AS AA ";
//        strQuery += " ON AI._id=AA.album_id ";

//        QString setWhere = "";

//        QString setOrder = "";
//        if(!strArg_1.isEmpty()){
//            print_debug();
//            qDebug() << "strArg_1 = " << strArg_1;

//            //strQuery += "(";
//            strQuery += searchcharactorSqlWhere();
//            //strQuery += ")";

//            setWhere = getSetWhereFilter();
//            if(!setWhere.isEmpty()){
//                strQuery += " AND ";
//                strQuery += setWhere;
//            }

//            //strQuery += " ORDER BY AI.album ";
//        }
//        else{

//            setWhere = getSetWhereFilter();
//            if(!setWhere.isEmpty()){
//                strQuery += " WHERE";
//                strQuery += setWhere;
//            }
//        }


///*
//        if(!strArg_1.isEmpty()){
//            print_debug();
//            qDebug() << "strArg_1 = " << strArg_1;
//            strQuery += searchcharactorSqlWhere();

//        }
//        else{
//            setWhere = getSetWhereFilter();
//        }

//        if(!setWhere.isEmpty()){
//            strQuery += " WHERE";
//            strQuery += setWhere;
//        }

//        */
//        //strQuery += " ORDER BY AI.date_added desc ";
//        //strQuery += " ORDER BY AI.album asc ";

//        QVariantList data;
//        sqlite->exec(strQuery, data);
//        ret_cnt = data.size();
//    }
//    sqlite->close();
//    delete sqlite;
//    return ret_cnt;
//}

///**
// * @brief MusicAlbum::slot_clickedBtnFilter:[슬롯] 필터 버튼 클릭
// */
//void MusicAlbum::slot_clickedBtnFilter(){
//    //if(slot_clickedBtnFilter_filter_flag) return;
//    //widget_filter->setVisible(!widget_filter->isVisible());

//    widget_filter->setVisible(!widget_filter->isVisible());

//    if(widget_filter->isVisible() == false){
//        if(this->flag_file_type == true){
//            this->lb_file_type->setText("");
//            this->lb_file_type->setStyleSheet("border:0px solid #707070;border-radius:15px;");
//            this->flag_file_type = false;

//            hl_filter->removeWidget(this->lb_file_type);
//        }
//        if(this->flag_bit_depth == true){
//            this->lb_bit_depth->setText("");
//            this->lb_bit_depth->setStyleSheet("border:0px solid #707070;border-radius:15px;");
//            this->flag_bit_depth = false;

//            hl_filter->removeWidget(this->lb_bit_depth);
//        }
//        if(this->flag_sample_rate == true){
//            this->lb_sample_rate->setText("");
//            this->lb_sample_rate->setStyleSheet("border:0px solid #707070;border-radius:15px;");
//            this->flag_sample_rate = false;

//            hl_filter->removeWidget(this->lb_sample_rate);
//        }
//        if(this->flag_star_point == true){
//            this->lb_star_point->setText("");
//            this->lb_star_point->setStyleSheet("border:0px solid #707070;border-radius:15px;");
//            this->flag_star_point = false;

//            hl_filter->removeWidget(this->lb_star_point);
//        }
//        if(this->flag_order == true){
//            this->lb_order->setText("");
//            this->lb_order->setStyleSheet("border:0px solid #707070;border-radius:15px;");
//            this->flag_order = false;

//            hl_filter->removeWidget(this->lb_order);
//        }

//        if(settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt() > 0){
//            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE,0).toInt();
//            this->lb_file_type->setText(fileTypeList.at(tmp_idx));
//            this->lb_file_type->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
//            this->flag_file_type = true;

//            hl_filter->addWidget(this->lb_file_type);
//            //hl_filter->addSpacing(10);
//        }

//        if(settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt() > 0){
//            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_BIT,0).toInt();
//            this->lb_bit_depth->setText(bitDepthList.at(tmp_idx));
//            this->lb_bit_depth->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
//            this->flag_bit_depth = true;

//            hl_filter->addWidget(this->lb_bit_depth);
//            //hl_filter->addSpacing(10);
//        }

//        if(settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt() > 0){
//            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING,0).toInt();
//            this->lb_sample_rate->setText(samplingList.at(tmp_idx));
//            this->lb_sample_rate->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
//            this->flag_sample_rate = true;

//            hl_filter->addWidget(this->lb_sample_rate);
//            //hl_filter->addSpacing(10);
//        }

//        if(settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt() > 0){
//            int tmp_idx = settings->value(rosesettings.SETTINGS_AFILTER_STAR,0).toInt();
//            this->lb_star_point->setText(starList.at(tmp_idx));
//            this->lb_star_point->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
//            this->flag_star_point = true;

//            hl_filter->addWidget(this->lb_star_point);
//            //hl_filter->addSpacing(10);
//        }

//        if(settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt() > 0){
//            int tmp_idxOrder = settings->value(rosesettings.SETTINGS_AFILTER_ORDER,0).toInt();
//            this->lb_order->setText(orderList.at(tmp_idxOrder));
//            this->lb_order->setStyleSheet("color:#FFFFFF;font-size:16px;border:1px solid #707070;border-radius:15px;");
//            this->flag_order = true;

//            hl_filter->addWidget(this->lb_order);
//            //hl_filter->addSpacing(10);
//        }
//        print_debug();
//        qDebug() << "this->flag_change_filter = " << this->flag_change_filter;
//        if(this->flag_change_filter == true){
//            this->flag_change_filter = false;


//        }

//        setDataFromDB();
//    }

//}



//void MusicAlbum::slot_changedBtnFilter(){

//    int i = 0;

//    this->selected_optFilter = sender()->property(KEY_AFILTER);
//    this->selected_optIdx = sender()->property(KEY_AIDX);

//    if(this->selected_optFilter == "FileType"){
//        if(this->selected_optIdx == 0){
//            this->btn_file_type[0]->setStyleSheet(btnStyle_selected);
//            this->btn_file_type[1]->setStyleSheet(btnStyle_normal);
//        }
//        else if(this->selected_optIdx == 1){
//            this->btn_file_type[0]->setStyleSheet(btnStyle_normal);
//            this->btn_file_type[1]->setStyleSheet(btnStyle_selected);
//        }

//        settings->setValue(rosesettings.SETTINGS_AFILTER_FILETYPE, this->selected_optIdx);
//        if(this->before_file_type != settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE).toInt()){
//            this->before_file_type = settings->value(rosesettings.SETTINGS_AFILTER_FILETYPE).toInt();
//            this->flag_change_filter = true;
//        }
//    }
//    else if(this->selected_optFilter == "BitDepth"){
//        if(this->selected_optIdx == 0){
//            this->btn_bit_depth[0]->setStyleSheet(btnStyle_selected);
//            this->btn_bit_depth[1]->setStyleSheet(btnStyle_normal);
//        }
//        else if(this->selected_optIdx == 1){
//            this->btn_bit_depth[0]->setStyleSheet(btnStyle_normal);
//            this->btn_bit_depth[1]->setStyleSheet(btnStyle_selected);
//        }

//        settings->setValue(rosesettings.SETTINGS_AFILTER_BIT, this->selected_optIdx);
//        if(this->before_bit_depth != settings->value(rosesettings.SETTINGS_AFILTER_BIT).toInt()){
//            this->before_bit_depth = settings->value(rosesettings.SETTINGS_AFILTER_BIT).toInt();
//            this->flag_change_filter = true;
//        }
//    }
//    else if(this->selected_optFilter == "Sampling"){
//        for(i = 0; i < 3; i++){
//            if(i == this->selected_optIdx){
//                this->btn_sample_rate[i]->setStyleSheet(btnStyle_selected);
//            }
//            else{
//                this->btn_sample_rate[i]->setStyleSheet(btnStyle_normal);
//            }
//        }

//        settings->setValue(rosesettings.SETTINGS_AFILTER_SAMPLEING, this->selected_optIdx);
//        if(this->before_sample_rate != settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING).toInt()){
//            this->before_sample_rate = settings->value(rosesettings.SETTINGS_AFILTER_SAMPLEING).toInt();
//            this->flag_change_filter = true;
//        }
//    }
//    else if(this->selected_optFilter == "Star"){
//        for(i = 0; i < 6; i++){
//            if(i == this->selected_optIdx){
//                this->btn_star_point[i]->setStyleSheet(btnStyle_selected);
//            }
//            else{
//                this->btn_star_point[i]->setStyleSheet(btnStyle_normal);
//            }
//        }

//        settings->setValue(rosesettings.SETTINGS_AFILTER_STAR, this->selected_optIdx);
//        if(this->before_star_point != settings->value(rosesettings.SETTINGS_AFILTER_STAR).toInt()){
//            this->before_star_point = settings->value(rosesettings.SETTINGS_AFILTER_STAR).toInt();
//            this->flag_change_filter = true;
//        }
//    }
//    else if(this->selected_optFilter == "OrderBy"){
//        for(i = 0; i < 4; i++){//c221109
//            if(i == this->selected_optIdx){
//                this->btn_order[i]->setStyleSheet(btnStyle_selected);
//            }
//            else{
//                this->btn_order[i]->setStyleSheet(btnStyle_normal);
//            }

//            settings->setValue(rosesettings.SETTINGS_AFILTER_ORDER, this->selected_optIdx);
//            if(this->before_order != settings->value(rosesettings.SETTINGS_AFILTER_ORDER).toInt()){
//                this->before_order = settings->value(rosesettings.SETTINGS_AFILTER_ORDER).toInt();
//                this->flag_change_filter = true;
//            }
//        }
//    }

//}

//void MusicAlbum::run_flagcheck(){
//    forever{
//        // 여기서 오래걸리는 작업을 수행




//        if ( QThread::currentThread()->isInterruptionRequested() ) {
//            return;
//        }
//    }
//}


//void MusicAlbum::OnTimerCallbackFunc()//cheon210608-test
//{
//    int nMilliseconds = Etimer1.elapsed();
//    if(nMilliseconds > 100000) Etimer1.restart();
//    //QTime time = QTime::currentTime();
//    //QString time_text = time.toString("hh : mm : ss");
//    //qDebug()<< "OnTimerCallbackFunc-nMilliseconds" << nMilliseconds;
//    //qDebug()<< "OnTimerCallbackFunc-NreloadFlag :" << NreloadFlag;
//    //qDebug()<< "OnTimerCallbackFunc-PreloadFlag :" << PreloadFlag;

//    if(NreloadFlag){//cheon210704-mouse
//        //flagReqMore = false;//cheon210607-test
//        if(nMilliseconds > 2000){//mouse_time_skippageterm)
//            flagReqMore = false;
//             getAlbumDataFromDB();//cheon210608-test
//        }
//        NreloadFlag = false;

//    }
//    if(PreloadFlag){//cheon210704-mouse
//        flagReqMore = false;//cheon210607-test

//        getAlbumDataFromDB_pre();//cheon210608-test
//        PreloadFlag = false;

//    }

//}


////scrollArea->verticalScrollBar()->maximum()
///**
// * @brief MusicAlbum::wheelEvent : 휠 이벤트
// * @param event
// */
//void MusicAlbum::wheelEvent(QWheelEvent *event){//c220323

//    //print_debug();

//    CNT++;
//    if(CNT > 100000) CNT = 1;
//    //qDebug() << "event->angleDelta()" << event->angleDelta();
//    //qDebug() << scrollArea->verticalScrollBar()->maximum();
//    //qDebug() << this->scrollArea->verticalScrollBar()->sliderPosition();

//    if(CNT%1 == 0 && event->angleDelta().ry() <= -120 && (this->scrollArea->verticalScrollBar()->sliderPosition() == scrollArea->verticalScrollBar()->maximum())){//cheon210607-test

//        //mtx.lock();
//        //if(curr_page_cnt > 0 && curr_page_cnt <= total_page_cnt){
//            getAlbumDataFromDB_view();


//           //this->scrollArea->ensureWidgetVisible(flowLayout->itemAt(flowLayout->count()/2)->widget());
//        //}


//        //mtx.unlock();

//    }
//    if(CNT%1 == 0 && event->angleDelta().ry() >= 120 ){
//        //
//        //mtx.lock();
//        //CNT = 1;
//        if(curr_page_cnt > 0){
//           // print_debug();
//            //getAlbumDataFromDB_pre_view();


//        }
//        //mtx.unlock();
//    }

//}
///*
//void MusicAlbum::resizeEvent(QResizeEvent *event){//c230222

//    QWidget::resizeEvent(event);
//    print_debug();
//    double nnn = (global.LmtCnt-33-15-20)/(186+18+term_i);
//    int nn = nnn;
//    int mod_nn = (global.LmtCnt-33-15-20)%(186+18+term_i);



//    while(1){

//        if(mod_nn > 18){
//            //nnn = (global.LmtCnt-33-15-20)/(186+(18+i));
//            //nn = nnn;
//            mod_nn = (global.LmtCnt-33-15-20)%(186+(18+(term_i++)));
//            if(term_i > 10) { term_i = 0; break;}
//        }else{
//            break;
//        }

//    }
//    double fff = mod_nn/186;
//    int ff = fff;
//    nnn = nn + ff;
//    qDebug() << "term_i=" << term_i;
//    qDebug() << "mod_nn=" << mod_nn;
//    qDebug() << "(mod_nn+33)/nn=" << (mod_nn)/nn;
//    flowLayout->setSpacingHV(18+term_i,20);
//}
//*/
//void MusicAlbum::resizeEvent(QResizeEvent *event){//c230304

//    QWidget::resizeEvent(event);

//    int w = flowLayout->sizeHint().width();
//    int l = 33, r = 15, scrollbarW = 20;

//    int mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w+18);
//    //qDebug() << "mod_nn=" << mod_nn;
//    int i = 0;
//    while(1){

//        mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w+18+i);
//        if(mod_nn > 20){

//            mod_nn = (global.LmtCnt-l-r-scrollbarW)%(w+(18+(i++)));
//        }else{
//            break;
//        }
//    }

//    flowLayout->setSpacingHV(18+i,20);
//    //QCoreApplication::processEvents();
//}
///*
//#if defined(Q_OS_WINDOWS)
////if(CNT > 4) CNT = 1;

//if(event->angleDelta().ry() <= -120 && this->scrollArea->verticalScrollBar()->sliderPosition() == scrollArea->verticalScrollBar()->maximum()){//cheon210607-test

//#endif

//#if defined(Q_OS_MAC)
////  timer.start();//cheon210608-test
////if(CNT > 2) CNT = 1;
//if(event->angleDelta().ry() <= -120 && CNT%2 == 0){//cheon210607-test
//#endif


//   mouse_wheel_flag = true;
//   //getAlbumDataFromDB();
//   if(curr_page_cnt > 0 && curr_page_cnt <= total_page_cnt){
//       if(slot_sliderValue_flag && mouse_wheel_flag){
//           print_debug();
//           getAlbumDataFromDB();
//           mouse_wheel_flag = false;
//       }
//       slot_sliderValue_flag = false;
//       getAlbumDataFromDB_view();
//   }
//  // Etimer1.restart();//cheon210608-test
// // ToastMsg::show(this, "", tr("next."));
// // ToastMsg::show(this, "", tr("next."),50);
//   //CNT = 1;
//}


//#if defined(Q_OS_WINDOWS)//cheon210709-genre
//if(CNT > 4) CNT = 1;
////qDebug() << "event->angleDelta()" << event->angleDelta();//cheon210709-genre
//if(event->angleDelta().ry() >= 120 && CNT%4 == 0){//cheon210709-genre
//#endif        //cheon210709-genre
//#if defined(Q_OS_MAC)//cheon210709-genre
//   if(CNT > 2) CNT = 1;
//if(event->angleDelta().ry() >= 120  && CNT%2 == 0 ){//cheon210709-genre
//#endif     //cheon210709-genre
////qDebug() << "event->angleDelta()" << event->angleDelta();


//   //getAlbumDataFromDB_pre();//cheon210608-test

//   if(curr_page_cnt > 0){
//       qDebug() << "curr_page_cnt=" << curr_page_cnt;
//       qDebug() << "next_page_cnt=" << next_page_cnt;
//       if(curr_page_cnt != next_page_cnt){
//           next_page_cnt = curr_page_cnt;
//           if(slot_sliderValue_flag && mouse_wheel_flag){
//               print_debug();
//               getAlbumDataFromDB_pre();
//               mouse_wheel_flag = false;
//           }
//           slot_sliderValue_flag = false;
//           getAlbumDataFromDB_pre_view();
//       }


//   }

//   //Etimer1.restart();//cheon210608-test
//  // ToastMsg::show(this, "", tr("pre."));
//  // ToastMsg::show(this, "", tr("pre."),50);
//   //CNT = 1;
//}

////qDebug() << "event->angleDelta()" << event->angleDelta();
//*/
