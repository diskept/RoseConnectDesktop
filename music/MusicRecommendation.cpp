#include "music/MusicRecommendation.h"

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
    MusicRecommend::MusicRecommend(QWidget *parent) : AbstractRoseHomeSubWidget(MainUIType::VerticalScroll_roseviewAll, parent) {

        this->linker = Linker::getInstance();

        // Data
        this->list_album = new QList<roseHome::AlbumItemData>();

        // 기본 UI 세팅
        this->setUIControl_albums();
    }


    /**
     * @brief 소멸자.
     */
    MusicRecommend::~MusicRecommend(){

        delete settings;
        this->deleteLater();
    }


    /**
     * @brief 전달받은 JSON 데이터로, 기본 데이터를 세팅한다.
     * @param jsonObj
     */
    void MusicRecommend::setJsonObject_forData(const QJsonObject &jsonObj){

        QString tmpPage = ProcJsonEasy::getString(jsonObj, "pageCode");
        this->flagNeedReload = false;

        if(this->curr_page != tmpPage){
            this->flagNeedReload = true;

            this->curr_page = tmpPage;

            // init
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
    void MusicRecommend::setActivePage(){

        if(this->flagNeedReload){

            // 항상 부모클래스의 함수 먼저 호출
            AbstractRoseHomeSubWidget::setActivePage();

            // init
            GSCommon::clearLayout(this->flowLayout_albums);
        }
    }


    int MusicRecommend::getAlbumDataFromDBforTotalCount(){

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
    // MARK : Create UI and Set
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief 기본 UI 구성함
     */
    void MusicRecommend::setUIControl_albums(){

        this->settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

        btnStyle_normal = "QPushButton{ ";
        btnStyle_normal += "background:transparent;color:#FFFFFF;font-size:16px;";
        btnStyle_normal += "border:1px solid #707070; border-radius:15px;}";
        btnStyle_normal += "QPushButton:hover{background-color:#BE905F;}";

        this->btnStyle_selected = "QPushButton{ ";
        this->btnStyle_selected += "background:#BE905F;color:#FFFFFF;font-size:16px;";
        this->btnStyle_selected += "border:1px solid #BE905F; border-radius:15px;}";

        this->fileTypeList << tr("All") << tr("Lossless"); /// << "DSD64x" << "DSD128x" << "DSD256x" << "DSD512x" << "MQA";
        this->bitDepthList << tr("All") << "24bit↑";          /// << "16bit↑" << "24bit↑";
        this->samplingList << tr("All") << "88.2kHz↑" << "176.4kHz↑";  /// <<"44.1kHz↑" << "88.2kHz↑" << "176.4kHz↑" << "352.8kHz↑";

        this->setUIControl_button();
        this->setUIControl_filter();

        // get widget width, right margin - by diskept j230317 start
        GSCommon::clearLayout(this->box_contents);
        this->box_contents->setAlignment(Qt::AlignTop);
        this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        roseHome::ItemAlbum_rosehome *listAlbum = new roseHome::ItemAlbum_rosehome(0, 0, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);

        this->album_widget_width = listAlbum->get_fixedWidth();
        this->album_widget_margin = listAlbum->get_rightMargin();

        //qDebug() << "[Debug] MusicRecommend::setUIControl_albums " << listAlbum->get_fixedWidth() << listAlbum->get_rightMargin();

        delete listAlbum;
        //  get widget width, right margin - by diskept j230317 finish

        // layout for items
        this->flowLayout_albums = this->get_addUIControl_flowLayout(0, 20);
    }


    void MusicRecommend::setUIControl_button(){

        // 상단 필터
        this->widget_btnFilter = new QWidget();
        this->widget_btnFilter->setStyleSheet("background-color: transparent;");
        this->widget_btnFilter->setFixedSize(global.LmtCnt - 180, 75);

        this->btn_filter_ico = GSCommon::getUIBtnImg("btn_filter",":/images/ico_filter.png", this->widget_btnFilter);
        this->btn_filter_ico->setFixedSize(80, 70);
        this->btn_filter_ico->setCursor(Qt::PointingHandCursor);
        this->btn_filter_ico->setGeometry(3, 2, 0, 0);

        connect(this->btn_filter_ico, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter()));


        this->btn_refresh = new QPushButton(this->widget_btnFilter);
        this->btn_refresh->setObjectName("btn_refresh");
        this->btn_refresh->setStyleSheet("#btn_refresh{background-color:transparent;} #btn_refresh:hover{background-color:transparent;}");
        this->btn_refresh->setCursor(Qt::PointingHandCursor);
        this->btn_refresh->setGeometry(global.LmtCnt - 320, 17, 140, 40);

        connect(this->btn_refresh, SIGNAL(clicked()), SLOT(slot_clickBtn_Refresh()));

        QLabel *lb_refresh_img = GSCommon::getUILabelImg(":/images/cd/icon_refresh.png", this->btn_refresh);
        lb_refresh_img->setStyleSheet("background-color:transparent;");
        lb_refresh_img->setFixedSize(50, 50);
        lb_refresh_img->setGeometry(0, 0, 0, 0);

        QLabel *lb_refresh_name = new QLabel(this->btn_refresh);
        lb_refresh_name->setStyleSheet("background-color:transparent; font-size:18px; font-weight:normal; color:#CCCCCC;");
        lb_refresh_name->setText(tr("Refresh"));
        lb_refresh_name->setGeometry(50, 10, 75, 27);

        this->lb_recomm_count = new QLabel(this->widget_btnFilter);
        this->lb_recomm_count->setAlignment(Qt::AlignRight);
        this->lb_recomm_count->setStyleSheet("background-color:transparent; font-size:18px; font-weight:normal; color:#FFFFFF;");
        this->lb_recomm_count->setText(this->rocommCount);
        this->lb_recomm_count->setGeometry(global.LmtCnt - (340 + this->lb_recomm_count->sizeHint().width()), 27, this->lb_recomm_count->sizeHint().width(), 27);

        this->flow_select_filter = new FlowLayout(0, 0, 10, 5);
        this->flow_select_filter->setContentsMargins(0, 0, 0, 0);
        this->flow_select_filter->setSizeConstraint(QLayout::SetMinimumSize);

        this->widget_select_filter = new QWidget(this->widget_btnFilter);
        this->widget_select_filter->setStyleSheet("background:transparent;");
        this->widget_select_filter->setLayout(this->flow_select_filter);
        this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (280 + this->lb_recomm_count->sizeHint().width()), 53);

        this->box_filter->addWidget(this->widget_btnFilter);

        int fileType = this->settings->value(rosesettings.SETTINGS_RFILTER_FILETYPE, 0).toInt();
        if(fileType > 1){
            fileType = 0;
            this->settings->setValue(rosesettings.SETTINGS_RFILTER_FILETYPE, fileType);
        }

        int bitDepth = this->settings->value(rosesettings.SETTINGS_RFILTER_BIT, 0).toInt();
        if(bitDepth > 1){
            bitDepth = 0;
            this->settings->setValue(rosesettings.SETTINGS_RFILTER_BIT, bitDepth);
        }

        int sampling = this->settings->value(rosesettings.SETTINGS_RFILTER_SAMPLING, 0).toInt();
        if(sampling > 2){
            sampling = 0;
            this->settings->setValue(rosesettings.SETTINGS_RFILTER_SAMPLING, sampling);
        }

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


    void MusicRecommend::setUIControl_filter(){

        QWidget *widget_header = new QWidget();
        widget_header->setFixedSize(480, 50);
        widget_header->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_close_filter = new QPushButton(widget_header);
        btn_close_filter->setFixedSize(480, 50);
        btn_close_filter->setStyleSheet("background-color:transparent; border:0px;");
        btn_close_filter->setGeometry(0, 0, 0, 0);

        connect(btn_close_filter, SIGNAL(clicked()), SLOT(slot_clickBtn_Filter_close()));

        QLabel *lb_fileType = new QLabel(btn_close_filter);
        lb_fileType->setText("File Type");
        lb_fileType->setFixedSize(130, 40);
        lb_fileType->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_fileType->setAlignment(Qt::AlignCenter);
        lb_fileType->setGeometry(20, 5, 0, 0);

        QLabel *lb_bitDepth = new QLabel(btn_close_filter);
        lb_bitDepth->setText("Bit Depth");
        lb_bitDepth->setFixedSize(130, 40);
        lb_bitDepth->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_bitDepth->setAlignment(Qt::AlignCenter);
        lb_bitDepth->setGeometry(170, 5, 0, 0);

        QLabel *lb_sampling = new QLabel(btn_close_filter);
        lb_sampling->setText("Sampling Rate");
        lb_sampling->setFixedSize(140, 40);
        lb_sampling->setStyleSheet("color:#CCCCCC; font-size:14px; background-color:transparent; border:0px;");
        lb_sampling->setAlignment(Qt::AlignCenter);
        lb_sampling->setGeometry(320, 5, 0, 0);

        QHBoxLayout *box_header = new QHBoxLayout;
        box_header->setContentsMargins(0, 0, 0, 0);
        box_header->setSpacing(0);
        box_header->addWidget(widget_header);


        QWidget *widget_body = new QWidget();
        widget_body->setFixedSize(480, 200);
        widget_body->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        for(int i = 0; i < this->fileTypeList.count(); i++){
            this->checkbox_file_type[i] = new QCheckBox(widget_body);
            this->checkbox_file_type[i]->setFixedSize(130, 40);
            this->checkbox_file_type[i]->setGeometry(20, (20 * (i+1)) + (40 * i), 0, 0);
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
            this->checkbox_bit_depth[i]->setFixedSize(130, 40);
            this->checkbox_bit_depth[i]->setGeometry(170, (20 * (i+1)) + (40 * i), 0, 0);
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
            this->checkbox_sample_rate[i]->setGeometry(315, (20 * (i+1)) + (40 * i), 0, 0);
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
        widget_tail->setFixedSize(480, 60);
        widget_tail->setStyleSheet("background:transparent; border:1px solid #4D4D4D;");

        QPushButton *btn_apply_filter = new QPushButton(widget_tail);
        btn_apply_filter->setStyleSheet("QPushButton{background:transparent;color:#FFFFFF;font-size:16px;border:1px solid #707070; border-radius:15px;} QPushButton:hover{background-color:#BE905F;}");
        btn_apply_filter->setText(tr("Apply"));
        btn_apply_filter->setFixedSize(130, 40);
        btn_apply_filter->setGeometry(90, 10, 0, 0);

        connect(btn_apply_filter, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_apply()));

        QPushButton *btn_cancel_filter = new QPushButton(widget_tail);
        btn_cancel_filter->setStyleSheet("QPushButton{background:transparent;color:#FFFFFF;font-size:16px;border:1px solid #707070; border-radius:15px;} QPushButton:hover{background-color:#BE905F;}");
        btn_cancel_filter->setText(tr("Cancel"));
        btn_cancel_filter->setFixedSize(130, 40);
        btn_cancel_filter->setGeometry(260, 10, 0, 0);

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
        this->widget_filter->setFixedSize(480, 310);
        this->widget_filter->move(70, 10);
        this->widget_filter->setContentsMargins(1, 1, 1, 1);

        QPushButton *btn_total = new QPushButton(this->widget_filter);
        btn_total->setStyleSheet("background:transparent; border:0px");
        btn_total->setCursor(Qt::PointingHandCursor);
        btn_total->setFixedSize(480, 310);
        btn_total->setGeometry(0, 0, 0, 0);
        btn_total->setLayout(box_total);

        connect(btn_total, SIGNAL(clicked()), this, SLOT(slot_clickBtn_Filter_close()));

        this->widget_filter->hide();
    }


    /**
     * @brief 데이터 , UI 초기화
     */
    void MusicRecommend::initAll(){

        this->list_album->clear();

        this->album_total_cnt = 0;
        this->album_draw_cnt = 0;

        this->flag_album_draw = false;

        this->jsonArr_Album = QJsonArray();

        print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));
    }


    /**
     * @brief 스크롤링에 대해서, get more data 처리
     */
    void MusicRecommend::proc_wheelEvent_to_getMoreData(){

        if((this->album_total_cnt > this->album_draw_cnt) && (this->flag_album_draw == false)
                && (this->scrollArea_main->verticalScrollBar()->value() == this->scrollArea_main->verticalScrollBar()->maximum())){

            this->flag_album_draw = true;

            print_debug();ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));

            this->request_more_albumDraw();
        }
    }


    void MusicRecommend::request_more_albumData(){

        int fileType = this->settings->value(rosesettings.SETTINGS_RFILTER_FILETYPE, 0).toInt();
        int bitDepth = this->settings->value(rosesettings.SETTINGS_RFILTER_BIT, 0).toInt();
        int sampling = this->settings->value(rosesettings.SETTINGS_RFILTER_SAMPLING, 0).toInt();

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

            if(fileType > 0){
                setWhere += " ((AI.mime_type == 'audio/ape') || (AI.mime_type == 'audio/flac') || (AI.mime_type == 'audio/dff') || (AI.mime_type == 'audio/dsf') || (AI.mime_type == 'audio/wav') || (AI.mime_type == 'audio/x-wav')) ";
            }
            if(bitDepth > 0){
                if(setWhere.isEmpty()){
                    setWhere += " (AI.bitdepth >= 24) ";
                }
                else{
                    setWhere += " AND (AI.bitdepth >= 24) ";
                }
            }
            if(sampling > 0){
                if(sampling == 1){
                    if(setWhere.isEmpty()){
                        setWhere += " (AI.samplerate >= 88200) ";
                    }
                    else{
                        setWhere += " AND (AI.samplerate >= 88200) ";
                    }
                }
                else if(sampling == 2){
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

            QString setOrder = "";

            setOrder = " ORDER BY RANDOM() ";
            strQuery += setOrder;

            strQuery += QString(" LIMIT 300 ");

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

        this->rocommCount = tr("Recommendation") + " (" + QString("%1").number(tmp_list_album.count()) + ") / " + QString("%1").number(this->getAlbumDataFromDBforTotalCount());
        this->lb_recomm_count->setText(this->rocommCount);
        this->lb_recomm_count->setGeometry(this->widget_btnFilter->geometry().width() - (140 + this->lb_recomm_count->sizeHint().width()), 27, this->lb_recomm_count->sizeHint().width(), 27);
        this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (280 + this->lb_recomm_count->sizeHint().width()), 53);

        this->slot_applyResult_albums(tmp_list_album, QJsonArray(), true);
    }


    void MusicRecommend::request_more_albumDraw(){

        // j220913 list count check
        int width_cnt = global.LmtCnt / 218;
        int mod = this->album_draw_cnt % width_cnt;

        int album_widget_cnt = 0;
        if(mod == 0){
            album_widget_cnt = width_cnt * 10;
        }
        else{
            album_widget_cnt = width_cnt * 10 + (width_cnt - mod);
        }
        // j220913 list count check

        int start_index = this->album_draw_cnt;
        int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > album_widget_cnt ) ? album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
        this->album_draw_cnt += max_cnt;

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
            connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicRecommend::slot_clickedItemAlbum);
        }

        for(int i = start_index; i < this->album_draw_cnt; i++){
            this->listAlbum_all[i]->setData(this->list_album->at(i));
            this->flowLayout_albums->addWidget(this->listAlbum_all[i]);

            QCoreApplication::processEvents();
        }

        ContentLoadingwaitingMsgHide();
        this->flag_album_draw = false;
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
    void MusicRecommend::slot_applyResult_albums(const QList<roseHome::AlbumItemData> &list_data, const QJsonArray &jsonArr_dataToPlay, const bool flag_lastPage){

        Q_UNUSED(jsonArr_dataToPlay);

        //qDebug() << "[Debug] MusicRecommend::slot_applyResult_albums " << list_data.count();

        if(list_data.size() > 0){
            this->flag_lastPage_album = flag_lastPage;
            this->flagReqMore_album = false;

            if(this->list_album->size() == 0){
                this->album_total_cnt = list_data.at(0).totalCount;
            }

            int start_index = this->list_album->size();
            this->list_album->append(list_data);

            if(start_index == 0){
                // widget draw change - by diskept j230317 start
                /*int f_width = this->flowLayout_albums->geometry().width();

                if(this->flowLayout_albums->geometry().width() <= 0){
                    f_width = this->width() - (80 + 63) - 10;
                }

                int f_wg_cnt = f_width / (this->album_widget_width + this->album_widget_margin);
                int f_mod = this->album_draw_cnt % f_wg_cnt;

                if(f_mod == 0){
                    this->album_widget_cnt = f_wg_cnt * 10;
                }
                else{
                    this->album_widget_cnt = f_wg_cnt * 10 + (f_wg_cnt - f_mod);
                }

                //qDebug() << "[Debug] MusicRecommend::slot_applyResult_albums " << f_width << f_wg_cnt << f_mod << this->album_widget_cnt;

                int start_index = this->album_draw_cnt;
                int max_cnt = ((this->album_total_cnt - this->album_draw_cnt) > this->album_widget_cnt ) ? this->album_widget_cnt : (this->album_total_cnt - this->album_draw_cnt);
                this->album_draw_cnt += max_cnt;*/

                //qDebug() << "[Debug] MusicRecommend::slot_applyResult_albums " << start_index << max_cnt;

                int max_cnt = this->list_album->count();
                this->album_draw_cnt = max_cnt;

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i] = new roseHome::ItemAlbum_rosehome(i, SECTION_FOR_MORE_POPUP___albums, tidal::AbstractItem::ImageSizeMode::Square_200x200, true);
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    this->listAlbum_all[i]->setData(this->list_album->at(i));
                    QCoreApplication::processEvents();
                }

                for(int i = start_index; i < this->album_draw_cnt; i++){
                    connect(this->listAlbum_all[i], &roseHome::ItemAlbum_rosehome::signal_clicked, this, &MusicRecommend::slot_clickedItemAlbum);
                    this->flowLayout_albums->addWidget(this->listAlbum_all[i]);
                }
                // widget draw change - by diskept j230317 finish

                this->flag_flow_draw = true;
            }

            ContentLoadingwaitingMsgHide();
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


    void MusicRecommend::slot_clickBtn_Refresh(){

        this->flagNeedReload = true;

        this->initAll();
        this->setActivePage();

        this->request_more_albumData();
    }


    void MusicRecommend::slot_clickBtn_Filter(){

        int idx = -1;

        for(int i = 0; i < this->fileTypeList.count(); i++){
            this->checkbox_file_type[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_RFILTER_FILETYPE, 0).toInt();
        this->checkbox_file_type[idx]->setCheckState(Qt::CheckState::Checked);

        for(int i = 0; i < this->bitDepthList.count(); i++){
            this->checkbox_bit_depth[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_RFILTER_BIT, 0).toInt();
        this->checkbox_bit_depth[idx]->setCheckState(Qt::CheckState::Checked);

        for(int i = 0; i < this->samplingList.count(); i++){
            this->checkbox_sample_rate[i]->setCheckState(Qt::CheckState::Unchecked);
        }

        idx = settings->value(rosesettings.SETTINGS_RFILTER_SAMPLING, 0).toInt();
        this->checkbox_sample_rate[idx]->setCheckState(Qt::CheckState::Checked);

        this->widget_filter->setVisible(!widget_filter->isVisible());
    }


    void MusicRecommend::slot_clickBtn_Filter_close(){

        this->widget_filter->setVisible(false);
    }


    void MusicRecommend::slot_clickBtn_Filter_choose(){

        QString type = sender()->property("type").toString();
        int idx = sender()->property("idx").toInt();

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


    void MusicRecommend::slot_clickBtn_Filter_apply(){

        int fileType = -1;
        int bitDepth = -1;
        int sampling = -1;

        for(int i = 0; i < this->fileTypeList.count(); i++){
            if(this->checkbox_file_type[i]->checkState() == Qt::CheckState::Checked){
                fileType = i;
                break;
            }
        }

        if(fileType > 1){
            fileType = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_RFILTER_FILETYPE, fileType);

        for(int i = 0; i < this->bitDepthList.count(); i++){
            if(this->checkbox_bit_depth[i]->checkState() == Qt::CheckState::Checked){
                bitDepth = i;
                break;
            }
        }

        if(bitDepth > 1){
            bitDepth = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_RFILTER_BIT, bitDepth);

        for(int i = 0; i < this->samplingList.count(); i++){
            if(this->checkbox_sample_rate[i]->checkState() == Qt::CheckState::Checked){
                sampling = i;
                break;
            }
        }

        if(sampling > 2){
            sampling = 0;
        }
        this->settings->setValue(rosesettings.SETTINGS_RFILTER_SAMPLING, sampling);

        this->widget_filter->setVisible(false);

        if(this->before_file_type != fileType || this->before_bit_depth != bitDepth || this->before_sample_rate != sampling){

            this->before_file_type = fileType;
            this->before_bit_depth = bitDepth;
            this->before_sample_rate = sampling;

            GSCommon::clearLayout(this->box_filter);
            this->setUIControl_button();

            this->initAll();
            this->setActivePage();

            this->request_more_albumData();
        }
    }


    void MusicRecommend::slot_clickBtn_Filter_cancel(){

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
    void MusicRecommend::slot_clickedItemAlbum(const tidal::AbstractItem::ClickMode clickMode){

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


    QJsonArray MusicRecommend::getDataForPlayMusic(const int album_id){

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
    void MusicRecommend::slot_optMorePopup_menuClicked(const OptMorePopup::ClickMode clickMode, const int index, const int section){

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


    void MusicRecommend::resizeEvent(QResizeEvent *event){//c230304

        Q_UNUSED(event);

        //qDebug() << "[Debug] MusicRecommend::resizeEvent " << this->width();

        this->widget_btnFilter->setFixedSize(this->width() - (80 + 63) - 10, 75);
        this->btn_refresh->setGeometry(this->widget_btnFilter->geometry().width() - 140, 17, 140, 40);
        this->lb_recomm_count->setGeometry(this->widget_btnFilter->geometry().width() - (140 + this->lb_recomm_count->sizeHint().width()), 27, this->lb_recomm_count->sizeHint().width(), 27);
        this->widget_select_filter->setGeometry(90, 22, this->widget_btnFilter->geometry().width() - (280 + this->lb_recomm_count->sizeHint().width()), 53);

        // flowlayout spacing change - by diskept j230317 start
        this->setFlowLayoutResize(this, this->flowLayout_albums, this->album_widget_width, this->album_widget_margin);
        // flowlayout spacing change - by diskept j230317 finish

        if(this->flag_flow_draw == false){
            this->flag_flow_draw = true;

            this->request_more_albumData();
        }
    }
}
