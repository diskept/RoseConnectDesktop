#include "musiclist_one.h"
#include "proc_uploadthumbnail.h"
#include "common/global.h"
#include "widget/toastmsg.h"
#include <common/gscommon.h>
#include <common/rosesettings.h>
#include <common/sqlitehelper.h>
#include "widget/framewidget.h"
#include <QWheelEvent>
#include <QJsonObject>
#include <QTime>
#include <QPainter>
#include <QJsonArray>
#include <QScrollBar>
#include <common/networkhttp.h>
#include <widget/trackinfo.h>
#include <QDebug>
#include <QScrollArea>
#include "widget/flowlayout.h"


#include <QCoreApplication>//c211213
#include <QIntValidator>//cheon210717-search


const int HTTP_FAVORITE_GET = 59;
const int HTTP_FAVORITE_ADD = 58;
const int HTTP_FAVORITE_DELETE = 57;
const QString al_str[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

/**
 * @brief MusicList_One::MusicList_One : 음악-분류-전체
 * @param parent
 * @note 검색-트랙-View All 에서도 사용
 */
MusicList_One::MusicList_One(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief MusicList_One::setInit : 초기 세팅
 */
void MusicList_One::setInit(){

    linker = Linker::getInstance();
    list_audioInfo = QList<DataPopup*>();
    delegate = new FileDelegate(this);
    proc_addPlaylist = new Proc_AddTackInPlayList(this);

    settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

    connect(delegate, SIGNAL(signal_clickedBtn(int,int)), this, SLOT(slot_delegateClicked(int,int)));
    connect(proc_addPlaylist, SIGNAL(signal_playlistClicked(int)), this, SLOT(slot_playlistClicked(int)));
    connect(proc_addPlaylist, SIGNAL(signal_newPlaylistNameEdited(QString)), this, SLOT(slot_newPlaylistNameEdited(QString)));
}

/**
 * @brief MusicList_One::setUIControl : UI 세팅
 */
void MusicList_One::setUIControl(){

    //---------------------------------------------------------------------------//cheon210601
        albumtCnt = new QLabel;
        albumtCnt->setText(QString("Total : %1").arg(1));
        albumtCnt->setStyleSheet("font-size:18px; color:#dddddd;");
        albumtCnt->setFixedSize(400,30);


        this->le_lisearch = new QLineEdit();//cheon210717-search
        //this->le_lisearch->setValidator( new QIntValidator(0, 1000, this) );//cheon210717-search
        this->le_lisearch->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
        this->le_lisearch->setObjectName("le_search");//cheon210527
        this->le_lisearch->setStyleSheet("#le_search { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:15px;}");//cheon210527
        this->le_lisearch->setMaxLength(5);//cheon210527
        this->le_lisearch->setFixedSize(100,30);//cheon210527
        this->le_lisearch->setTextMargins(1,1,1,1);
        this->le_lisearch->setText("");

        QLabel *le_page_search = new QLabel;//cheon210527
        le_page_search->setText(QString("goto Page"));//cheon210527
        le_page_search->setStyleSheet("font-size:18px; color:#dddddd;");//cheon210527
        le_page_search->setFixedSize(90,30);//cheon210527

        QPushButton *btn_ok = new QPushButton;
        btn_ok->setText(tr("go"));//btn_ok->setText(tr("추가"));
        btn_ok->setCursor(Qt::PointingHandCursor);
        btn_ok->setStyleSheet("font-size:16px;color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
        btn_ok->setFixedSize(70,30);

        QHBoxLayout *search_hBox = new QHBoxLayout();//cheon210527
        search_hBox->setSpacing(0);//cheon210527
        search_hBox->setContentsMargins(0,0,0,0);//cheon210527
       // search_hBox->setAlignment(Qt::AlignTop);//cheon210527
        search_hBox->addWidget(le_page_search);//cheon210527
        search_hBox->setSpacing(10);//cheon210527
        search_hBox->addWidget(this->le_lisearch);//cheon210527
        search_hBox->addWidget(btn_ok);



        QWidget *widget_search = new QWidget();//cheon210527
        widget_search->setObjectName("widget_search");//cheon210527
        widget_search->setStyleSheet("#widget_search { background-color:#171717; }");//cheon210527
        widget_search->setLayout(search_hBox);//cheon210527

/*
        QHBoxLayout *tmp2_hBox = new QHBoxLayout();//cheon210527
        tmp2_hBox->setSpacing(0);//cheon210527
        tmp2_hBox->setContentsMargins(50,0,80,0);//cheon210717-search
        tmp2_hBox->addWidget(widget_search,1, Qt::AlignVCenter);//cheon210527
        tmp2_hBox->setSpacing(10);
        tmp2_hBox->addWidget(btn_ok, 1, Qt::AlignVCenter);
*/


       // tmp2_hBox->addWidget(btn_ok, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //    albumtCnt->setText(QString("Total : %1            Page : %2/%3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));//cheon210601

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
        le_str_search->setText(QString("Full-Song Search"));//cheon210527
        le_str_search->setStyleSheet("font-size:18px; color:#dddddd;");//cheon210527
        le_str_search->setFixedSize(150,30);//cheon210527

        QPushButton *btn_ok_str = new QPushButton;
        btn_ok_str->setText(tr("ok"));//btn_ok->setText(tr("추가"));
        btn_ok_str->setCursor(Qt::PointingHandCursor);
        btn_ok_str->setStyleSheet("font-size:16px;color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
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

        //-end------------------------------------------------------------------

        QHBoxLayout *tmp_hBox = new QHBoxLayout();
        tmp_hBox->setSpacing(50);
        tmp_hBox->setContentsMargins(30,0,0,0);
       // tmp_hBox->setAlignment(Qt::AlignTop);

        tmp_hBox->addWidget(albumtCnt,1, Qt::AlignVCenter|Qt::AlignLeft );
        //tmp_hBox->setSpacing(50);
        //tmp_hBox->addLayout(tmp2_hBox);//cheon210527
        //tmp_hBox->addLayout(tmp2_str_hBox);
        tmp_hBox->addWidget(widget_search, 1, Qt::AlignVCenter|Qt::AlignLeft );//cheon210527
        tmp_hBox->addWidget(widget_search_str, 1, Qt::AlignVCenter|Qt::AlignLeft );//cheon210527

        QWidget *widget_tt = new QWidget();
        widget_tt->setObjectName("widget_tt");
        widget_tt->setStyleSheet("#widget_tt { background-color:#171717; }");

        widget_tt->setLayout(tmp_hBox);


        //----------------------------------------------------------------------


    listWidget = new CustomListWidget(this);
    listWidget->setItemDelegate(delegate);
    listWidget->setCursor(Qt::PointingHandCursor);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setStyleSheet("border:none;");



    QVBoxLayout *boxContents = new QVBoxLayout;//c220402
    boxContents->setSpacing(0);
    boxContents->setContentsMargins(33,0,0,0);

    boxContents->addWidget(listWidget);

/*
    QHBoxLayout *hl_Alpa[26];
    for(int i = 0 ; i < 26; i++){
        hl_Alpa[i] = new QHBoxLayout;
        hl_Alpa[i]->setSpacing(0);
        hl_Alpa[i]->setContentsMargins(0,0,0,0);
        hl_Alpa[i]->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        //hl_Alpa[i]->addLayout(boxContents);
    }

    ClickableLabel *alpa[26];
    for(int i = 0 ; i < 26; i++){
        alpa[i] = new ClickableLabel();
        alpa[i]->setFixedSize(20,20);
        alpa[i]->setText(al_str[i]);
        alpa[i]->setProperty("sindex",i);
        //alpa[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        alpa[i]->setAlignment(Qt::AlignCenter);
        alpa[i]->setStyleSheet("text-align:center;font-size:18px; color:#dddddd;background-color:#B18658;border-radius:15px;");
        //hl_Alpa[i]->addWidget(alpa[i]);
    }
    */
/*
//widget_alpa2->setStyleSheet("#widget_alpa2 { background-color:#111111; }");
    FlowLayout *flowLayout = new FlowLayout(0, 30, 300);     // margin, hSpc, vSpc
    flowLayout->setSizeConstraint(QLayout::SetMinimumSize);


    ClickableLabel *alpa_a = new ClickableLabel();
    alpa_a->setText(QString("A"));
    alpa_a->setProperty("sindex",1);
    alpa_a->setStyleSheet("font-size:18px; color:#dddddd;");
    ClickableLabel *alpa_b = new ClickableLabel();
    alpa_b->setText(QString("B"));
    alpa_b->setProperty("sindex",2);
    alpa_b->setStyleSheet("font-size:18px; color:#dddddd;");

    QScrollArea* techScroll = new QScrollArea;
    techScroll->setLayout(flowLayout);
    //techScroll->setBackgroundRole(QPalette::Window);
    //techScroll->setFrameShadow(QFrame::Plain);
    techScroll->setFrameShape(QFrame::NoFrame);
    techScroll->setWidgetResizable(true);
    techScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    techScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    flowLayout->addWidget(widget_alpa2);

    QWidget *widget_alpa = new QWidget;
    widget_alpa->setLayout(flowLayout);
    techScroll->setWidget(widget_alpa);
*/
    /*
    QVBoxLayout *vl_alpa = new QVBoxLayout;
    vl_alpa->setSpacing(0);
    vl_alpa->setContentsMargins(0,0,0,0);
    for(int i = 0 ; i < 26; i++){
        //flowLayout->addWidget(alpa[i]);

        vl_alpa->addWidget(alpa[i]);
        //vl_alpa->addLayout(hl_Alpa[i]);
    }
    //vl_alpa->addWidget(alpa_a);
    //vl_alpa->addWidget(alpa_b);

    QWidget *widget_alpa2 = new QWidget;
    widget_alpa2->setLayout(vl_alpa);
*/

    QHBoxLayout *boxAlpa = new QHBoxLayout;
    boxAlpa->setSpacing(0);
    boxAlpa->setContentsMargins(0,0,10,0);
    boxAlpa->addLayout(boxContents);
    //boxAlpa->addWidget(widget_alpa2, 0, Qt::AlignLeft);



    QVBoxLayout *box = new QVBoxLayout;
    box->setSpacing(0);
    box->setContentsMargins(0,0,0,0);
    box->setAlignment(Qt::AlignTop);
    box->addWidget(widget_tt);
    box->addLayout(boxAlpa);


    setLayout(box);

    ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//cheontidal
    total_db_cnt = getAlbumDataFromDBforFullsongTotalCount();//cheon210601

    //if(this->gdialog_wait->isHidden() != true){
        this->ContentLoadingwaitingMsgHide();     //cheon Music
    //}

    connect(this->le_lisearch, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed()));//cheon210717-search
    connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedChange()));//cheon210717-search
    connect(this->le_lisearch_str, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed_str()));//cheon210717-search
    connect(btn_ok_str, SIGNAL(clicked()), this, SLOT(slot_clickedChange_str()));
    //cheon210717-search
    //connect(alpa_a, SIGNAL(signal_clicked()), this, SLOT(slot_clickedChange_alpa()));
    //connect(alpa_b, SIGNAL(signal_clicked()), this, SLOT(slot_clickedChange_alpa()));

    //for(int i = 0 ; i < 26; i++){
     //   connect(alpa[i], SIGNAL(signal_clicked()), this, SLOT(slot_clickedChange_alpa()));
    //}

/*
    workerTriggerone = new QTimer();//cheon210608-test
   threadone = new QThread();//cheon210608-test
    workerTriggerone->setInterval(300);//cheon210608-test

    connect(workerTriggerone, SIGNAL(timeout()), this, SLOT(OnTimerCallbackFunc()));//cheon210608-test
    connect(threadone, SIGNAL(finished()), workerTriggerone, SLOT(deleteLater()));//cheon210608-test

    workerTriggerone->start(400);//cheon210608-test
    workerTriggerone->moveToThread(threadone);//cheon210608-test
    threadone->start();//cheon210608-test
    Etimerone.start();//cheon210608-test
    */
}



void MusicList_One::ContentLoadingwaitingMsgShow(QString msg)//cheontidal
{
    //----------------------------------------------------
    //global.dialog_wait->hide(); //cheontidal
    //global.dialog_wait->deleteLater();
    //qDebug() << "===================================";
    QLabel *lb_msg = new QLabel();
    lb_msg->setText(msg);
    lb_msg->setAlignment(Qt::AlignCenter);

    QHBoxLayout *hl_msgBox = new QHBoxLayout();
    hl_msgBox->setContentsMargins(0,0,0,0);
    hl_msgBox->setSpacing(0);
    hl_msgBox->setContentsMargins(35,20,35,15);
    hl_msgBox->addStretch(1);
    hl_msgBox->addWidget(lb_msg);
    hl_msgBox->addStretch(1);
    QWidget *widget_msgBox = new QWidget();
    widget_msgBox->setStyleSheet("background-color:#B18658;color:#FFFFFF;font-size:20px;border-radius:5px;");
    widget_msgBox->setLayout(hl_msgBox);

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_msgBox, 0, Qt::AlignCenter);

    gdialog_wait = new QDialog(this) ;
    gdialog_wait->setModal(true);
    gdialog_wait->setWindowFlags( Qt::Tool | Qt::FramelessWindowHint );
    gdialog_wait->setAttribute(Qt::WA_TranslucentBackground);

    gdialog_wait->setStyleSheet("background-color:#55FF0000;border-radius:20px;");
    gdialog_wait->setGeometry(this->width()/2,400,1000,50);//c211213
    gdialog_wait->setFixedSize(500,100);
    gdialog_wait->setLayout(vl_total);
    gdialog_wait->show();
}

void MusicList_One::ContentLoadingwaitingMsgHide()//cheontidal
{
    gdialog_wait->hide(); //cheontidal
    //global.dialog_wait->deleteLater();
}

void MusicList_One::slot_returnPressed(){//cheon210717-search
    QString tmp_page =this->le_lisearch->text();
    qDebug() << "slot_returnPressed-p_text" << tmp_page;

    this->goto_getFullsongDataFromDB(tmp_page.toInt());
    this->le_lisearch->setText("");
}

void MusicList_One::slot_clickedChange(){//cheon210717-search
    QString tmp_page =this->le_lisearch->text();
    qDebug() << "slot_clickedChange-p_text" << tmp_page;

    this->goto_getFullsongDataFromDB(tmp_page.toInt());
    this->le_lisearch->setText("");
}

void MusicList_One::slot_returnPressed_str(){//cheon210717-search
    QString tmp_page =this->le_lisearch_str->text();
    qDebug() << "slot_returnPressed_str-p_text" << tmp_page;

    setDataFromDB_Alpa(tmp_page);
}

void MusicList_One::slot_clickedChange_str(){//cheon210717-search
    QString tmp_page =this->le_lisearch_str->text();
    qDebug() << "slot_clickedChange_str-p_text" << tmp_page;
    setDataFromDB_Alpa(tmp_page);
}


void MusicList_One::setDataFromDB_Alpa(const QString &p_arg){

    this->setContentsMargins(0,10,0,0);

    strArg_1 = p_arg;
    listWidget->verticalScrollBar()->setValue(0);
    list_audioInfo.clear();
    listWidget->clear();
    total_db_cnt = getAlbumDataFromDBforFullsongTotalCount();
    OffsetPoint = 0;

    getTrackDataFromDB();
}


void MusicList_One::slot_clickedChange_alpa(){//cheon210717-search
    ClickableLabel *lb_alpa = qobject_cast<ClickableLabel*>(sender());
    int sindex = lb_alpa->property("sindex").toInt();
    print_debug();
    qDebug() << "sindex" << sindex;
    setDataFromDB_Alpa(al_str[sindex]);
/*
    if(sindex == 1){

        setDataFromDB_Alpa("A");

    }else if(sindex == 2){
        setDataFromDB_Alpa("B");
    }
    else{
        //strArg_1 = "";
        setDataFromDB_Alpa("");
    }
*/
    //this->goto_getFullsongDataFromDB(tmp_page.toInt());
    //this->le_lisearch->setText("");
}

void MusicList_One::goto_getFullsongDataFromDB(int p_page){//cheon210527
print_debug();
    listWidget->verticalScrollBar()->setValue(0);
    list_audioInfo.clear();
    listWidget->clear();

    //ContentLoadingwaitingMsgShow(tr("Content is being loaded. Please wait."));//cheontidal
    //qDebug() << "-----goto_getFullsongDataFromDB--001";
    total_db_cnt = getAlbumDataFromDBforFullsongTotalCount();//cheon210601
    //qDebug() << "-----goto_getFullsongDataFromDB--002";
    //ContentLoadingwaitingMsgHide();//cheontidal
    //qDebug() << "333-01"<< p_page<< total_db_cnt;
    int cnt = 0, LMT_CNT = 100;
    total_page_cnt = 0;
    if((total_db_cnt % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt / LMT_CNT) + 1;
    }
    else if((total_db_cnt % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt / LMT_CNT);
    }
    if( total_page_cnt < p_page || p_page <= 0 ) {
        //qDebug() << "333-02";
        ToastMsg::show(this, "", tr("There are no pages.."), 5000);//cheon210527
        return;
    }


    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        if(flag_page_first == true){
            OffsetPoint += LMT_CNT;
            flag_page_first = false;
        }else{
            print_debug();
            OffsetPoint = (p_page - 1) * LMT_CNT;
        }
        //qDebug() << "goto_getFullsongDataFromDB--OffsetPoint" << OffsetPoint;

        QString strQuery = "";
        strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth, A.songrate";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";

        if(!strArg_1.isEmpty()){
            print_debug();
            qDebug() << "strArg_1 = " << strArg_1;

            strQuery += " WHERE artist LIKE '%" + strArg_1 + "%' OR title LIKE '%" + strArg_1+ "%'";

            strQuery += " ORDER BY A.title ";
        }
        else{
            strQuery += " ORDER BY A.title ";
            QString setWhere = "";
            QString setOrder = "";

            if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                 setWhere += " ((A.mime_type == 'audio/ape') || (A.mime_type == 'audio/flac') || (A.mime_type == 'audio/dff') || (A.mime_type == 'audio/dsf') || (A.mime_type == 'audio/wav') || (A.mime_type == 'audio/x-wav')) ";
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                /*if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                    setWhere += " AND (A.bitdepth >= 24) ";
                }
                else{
                    setWhere += " (A.bitdepth >= 24) ";
                }*/
                if(setWhere.isEmpty()){
                    setWhere += " (A.bitdepth >= 24) ";
                }
                else{
                    setWhere += " AND (A.bitdepth >= 24) ";
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.samplerate >= 88200) ";
                    }
                    else{
                        setWhere += " AND (A.samplerate >= 88200) ";
                    }
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.samplerate >= 176400) ";
                    }
                    else{
                        setWhere += " AND (A.samplerate >= 176400) ";
                    }
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.songrate == 0) ";
                    }
                    else{
                        setWhere += " AND (A.songrate == 0) ";
                    }
                }
                else{
                    switch (settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt()) {
                        case 0:
                        case 6:
                            break;
                        case 1:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 5) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 5) ";
                            }
                            break;
                        case 2:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 4) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 4) ";
                            }
                            break;
                        case 3:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 3) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 3) ";
                            }
                            break;
                        case 4:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 2) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 2) ";
                            }
                            break;
                        case 5:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 1) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 1) ";
                            }
                            break;
                    }
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 1){
                    setOrder += " ORDER BY A.title ";
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 2){
                    setOrder += " ORDER BY A.artist ";
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 3){
                    setOrder += " ORDER BY A.favorite ";
                }
            }

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }
            if(!setOrder.isEmpty()){
                strQuery += setOrder;
            }
        }

        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);
qDebug() << "strQuery= " << strQuery;
        QVariantList data;
        sqlite->exec(strQuery, data);


        if(OffsetPoint == 0){
            curr_page_cnt = 1;
        }
        else{
            curr_page_cnt = OffsetPoint / LMT_CNT + 1;
        }
        before_album_page = curr_page_cnt;

        total_page_cnt = 0;
        if((total_db_cnt % LMT_CNT) > 0){
            total_page_cnt += (total_db_cnt / LMT_CNT) + 1;
        }
        else if((total_db_cnt % LMT_CNT) == 0){
            total_page_cnt += (total_db_cnt / LMT_CNT);
        }

        albumtCnt->setText(QString("Total : %1                    Page : %2/%3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));

      //  scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            //qDebug() << QString("Total : %1                            Page : %2/%3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt);
/*
            while (listWidget->count()>0){
                 qDebug()<< listWidget->count();
                QListWidgetItem* item = listWidget->takeItem(0);
               // listWidget->removeItemWidget(item);
                delete item; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
           }
*/
            listWidget->clear();   //cheon210526
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject tmp_data = data.at(i).toJsonObject();

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_type = "";

                QString tmp_mime = "";
                int tmp_sample = 0;
                int tmp_bit = 0;
                QString snd_mime = "";

                tmp_sample = tmp_data["samplerate"].toInt();
                tmp_bit = tmp_data["bitdepth"].toInt();

                tmp_split = tmp_data["mime_type"].toString();
                splitToken = tmp_split.split("/");

                if(splitToken.size() > 1){
                    tmp_type = splitToken.at(1);

                    if(tmp_type == "x-wav" || tmp_type == "wav"){
                        tmp_mime = "WAV";
                    }
                    else if (tmp_type == "wv") {
                        tmp_mime = "WV";
                    }
                    else if(tmp_type == "flac"){
                        tmp_mime = "FLAC";
                    }
                    else if(tmp_type == "dff" || tmp_type == "dsf"){
                        tmp_mime = "DSD";
                    }
                    else if(tmp_type == "mp4"){
                        tmp_mime = "ALAC";
                    }
                    else if(tmp_type == "mpeg"){
                        tmp_mime = "MP3";
                    }
                    else if(tmp_type == "amr"){
                        tmp_mime = "AMR";
                    }
                    else if(tmp_type == "amr-wb"){
                        tmp_mime = "AWB";
                    }
                    else if(tmp_type == "x-ms-wma"){
                        tmp_mime = "WMA";
                    }
                    else if(tmp_type == "ape"){
                        tmp_mime = "APE";
                    }
                    else if(tmp_type == "ogg"){
                        tmp_mime = "OGG";
                    }
                    else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                        tmp_mime = "AAC";
                    }
                    else if(tmp_type == "x-matroska"){
                        tmp_mime = "MKA";
                    }
                    else if(tmp_type == "midi"){
                        tmp_mime = "MIDI";
                    }
                    else if(tmp_type == "sp-midi"){
                        tmp_mime = "SMF";
                    }
                    else if(tmp_type == "imelody"){
                        tmp_mime = "IMY";
                    }
                    else if(tmp_type == "x-mpegurl"){
                        tmp_mime = "M3U8";
                    }
                    else if(tmp_type == "x-scpls"){
                        tmp_mime = "PLS";
                    }
                    else if(tmp_type == "aiff"){
                        tmp_mime = "AIFF";
                    }
                }

                if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                        || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                    snd_mime = tmp_mime;
                }
                else if(tmp_mime == "DSD"){
                    if(tmp_sample == 88200 || tmp_sample == 2822400){
                        snd_mime = tmp_mime + " 64";
                    }
                    else if(tmp_sample == 176400 || tmp_sample == 5644800){
                        snd_mime = tmp_mime + " 128";
                    }
                    else if(tmp_sample == 352800 || tmp_sample == 11289600){
                        snd_mime = tmp_mime + " 256";
                    }
                    else if(tmp_sample == 705600 || tmp_sample == 22579200){
                        snd_mime = tmp_mime + " 512";
                    }
                    else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                        snd_mime = tmp_mime + " 1024";
                    }
                    else{
                        snd_mime = tmp_mime;
                    }
                }
                else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                    if(tmp_bit == 0 || tmp_bit == 16){
                        snd_mime = tmp_mime;
                    }
                    else if(tmp_bit > 16){
                        snd_mime = tmp_mime + QString(" %1").arg(tmp_bit);
                    }
                }
                tmp_data.insert("album_mime", snd_mime);

                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(tmp_data);
                this->list_audioInfo.append(tmp_dataPopup);

                QString tmp_albumImg = "";
                if(tmp_data.contains("album_art")){
                    tmp_albumImg = tmp_data["album_art"].toString();
                }

                QString tmp_strDuration="";
                int tmp_duration = tmp_data["duration"].toInt();
                if(tmp_duration >=0){
                    QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                    tmp_strDuration = tmp_time.toString("mm:ss");
                }

                QJsonObject json;
                json.insert("index", i);
                //json.insert("index", i);
                json.insert("fileName", tmp_dataPopup->getTitle());
                json.insert("duration", tmp_strDuration);
                json.insert("author", tmp_dataPopup->getArtist());
                json.insert("albumTitle", tmp_dataPopup->getAlbum());
                json.insert("title", tmp_dataPopup->getTitle());
                json.insert("isFile", true);
                json.insert("album_art", tmp_albumImg);
                json.insert("album_mime", snd_mime);

                QListWidgetItem *item = new QListWidgetItem;
                item->setData(Qt::UserRole, json);
                item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
                listWidget->addItem(item);

                FileDownloader *fileDownloader = new FileDownloader(this);
                fileDownloader->setProperty("index", i);
                //fileDownloader->setProperty("index", i);
                fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
                connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));
                //qDebug() << "-----goto_getFullsongDataFromDB--003";
                requestGetTrackFavorites(i);
                //qDebug() << "-----goto_getFullsongDataFromDB--004";
                // requestGetTrackFavorites(i);

            }
            OffsetPoint += LMT_CNT;
            //qDebug() << "-----goto_getFullsongDataFromDB--004-OffsetPoint" << OffsetPoint;

        }
    }
    sqlite->close();
    delete sqlite;
}

int MusicList_One::getAlbumDataFromDBforFullsongTotalCount(){//cheon210601
    print_debug();

    int ret_cnt = 0;
    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

       // strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth ";
        strQuery += " SELECT A._id AS id,  AI._data AS album_art, A.album_id, A._data AS data ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";

        QString setWhere = "";
        if(!strArg_1.isEmpty()){
            print_debug();
            qDebug() << "strArg_1= " << strArg_1;
            strQuery += " WHERE artist LIKE '%"+strArg_1+"%' OR title LIKE '%"+strArg_1+"%'";

            //strQuery += QString(" artist LIKE '%1%' OR title LIKE '%1%'").arg(strArg_1);

        }else{
            if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                setWhere += " ((A.mime_type == 'audio/ape') || (A.mime_type == 'audio/flac') || (A.mime_type == 'audio/dff') || (A.mime_type == 'audio/dsf') || (A.mime_type == 'audio/wav') || (A.mime_type == 'audio/x-wav')) ";
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                /*if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                setWhere += " AND (A.bitdepth >= 24) ";
            }
            else{
                setWhere += " (A.bitdepth >= 24) ";
            }*/
                if(setWhere.isEmpty()){
                    setWhere += " (A.bitdepth >= 24) ";
                }
                else{
                    setWhere += " AND (A.bitdepth >= 24) ";
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.samplerate >= 88200) ";
                    }
                    else{
                        setWhere += " AND (A.samplerate >= 88200) ";
                    }
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.samplerate >= 176400) ";
                    }
                    else{
                        setWhere += " AND (A.samplerate >= 176400) ";
                    }
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.songrate == 0) ";
                    }
                    else{
                        setWhere += " AND (A.songrate == 0) ";
                    }
                }
                else{
                    switch (settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt()) {
                    case 0:
                    case 6:
                        break;
                    case 1:
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 5) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 5) ";
                        }
                        break;
                    case 2:
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 4) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 4) ";
                        }
                        break;
                    case 3:
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 3) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 3) ";
                        }
                        break;
                    case 4:
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 2) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 2) ";
                        }
                        break;
                    case 5:
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 1) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 1) ";
                        }
                        break;
                    }
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
        if(ret_cnt == 0){//c220402
            ToastMsg::show(this, "", tr("There is no song."), 2000);
            // emit linker->signal_checkQueue(25, "");
        }else{
            ToastMsg::show(this, "", tr("Content is being loaded. Please wait."), 2000);
        }
    }
    sqlite->close();
    delete sqlite;
    //qDebug() << "getAlbumDataFromDBforFullsongTotalCount = "<< ret_cnt;
    return ret_cnt;
}

/**
 * @brief MusicList_One::setDataFromDB DB에서 가져와서 데이터 세팅
 * @param p_arg
 */
void MusicList_One::setDataFromDB(const QString &p_arg){

    this->setContentsMargins(0,10,0,0);

    strArg_1 = p_arg;
    listWidget->verticalScrollBar()->setValue(0);
    list_audioInfo.clear();
    listWidget->clear();
    //flag_page_first = true;
    print_debug();
    //goto_getFullsongDataFromDB(1);
    getTrackDataFromDB();
}

/**
 * @brief MusicList_One::setDataFromDB DB에서 가져와서 전체 데이터 세팅
 */
void MusicList_One::setDataFromDB(){

    // 초기화
    strArg_1 = "";
    listWidget->verticalScrollBar()->setValue(0);
    list_audioInfo.clear();
    listWidget->clear();
    //flag_page_first = true;
    OffsetPoint = 0;//c220511
    flagReqMore = false;//c220511
    print_debug();
    //goto_getFullsongDataFromDB(1);
    getTrackDataFromDB();
}


/**
 * @brief MusicList_One::getJsonArr_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index if -1, 앨범 전체
 * @return QJsonArray
 */
QJsonArray MusicList_One::getJsonArr_TrackData(const int &p_index){
    QJsonArray jsonArray;

    if(p_index<0){
        for(int i= 0 ; i < list_audioInfo.size(); i++){
            jsonArray.append(getJsonObject_TrackData(i));
        }
    }else{
        jsonArray.append(getJsonObject_TrackData(p_index));
    }

    return jsonArray;
}

/**
 * @brief MusicList_One::getJsonObject_TrackDataForAddInPlaylist : 플레이리스트 추가를 위한 Track 정보 반환
 * @param p_index
 * @return QJsonObject
 */
QJsonObject MusicList_One::getJsonObject_TrackData(const int &p_index){

    QJsonObject jsonTrack;

    if(p_index >=0 && p_index < list_audioInfo.size()){
        DataPopup *dataPopup = list_audioInfo.at(p_index);

        // 앨범
        jsonTrack.insert("album", dataPopup->getAlbum());

        // 아티스트
        QJsonObject jsonArtist;
        jsonArtist.insert("name", dataPopup->getArtist());
        QJsonArray jsonArrArtists;
        jsonArrArtists.append(jsonArtist);
        jsonTrack.insert("artists", jsonArrArtists);

        // 클라이언트 아이디 = 오디오 아이디
        jsonTrack.insert("client_id", dataPopup->getId());

        // 썸네일
        QJsonObject jsonThumb;
        jsonThumb.insert("url", dataPopup->getAlbum_art());

        QJsonArray jsonArrThumbs;
        jsonArrThumbs.append(jsonThumb);
        jsonTrack.insert("thumbnails", jsonArrThumbs);

        // 타이틀
        jsonTrack.insert("title", dataPopup->getTitle());

        // 재생 URL
        jsonTrack.insert("url", dataPopup->getData());

        jsonTrack.insert("isFavorite", false);
        jsonTrack.insert("isSelected", false);
        jsonTrack.insert("mac_address", global.device.getDeviceID());
        jsonTrack.insert("type","music");
    }


    return jsonTrack;
}

/**
 * @brief MusicList_One::setTrackFavoritesUI : 트랙 즐겨찾기 정보 세팅
 * @param p_index
 * @param p_flagFavor
 */
void MusicList_One::setTrackFavoritesUI(const int &p_index, const bool &p_flagFavor){

    if(p_index < listWidget->count()){
        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        map["favorites"] = p_flagFavor;
        item->setData(Qt::UserRole, map);
    }
}

/**
 * @brief MusicList_One::changeTrackFavoritesUI : 트랙 즐겨찾기 정보 변경
 * @param p_index
 */
void MusicList_One::changeTrackFavoritesUI(const int &p_index){

    QListWidgetItem *item = listWidget->item(p_index);
    QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
    map["favorites"] = !map["favorites"].toBool();
    item->setData(Qt::UserRole, map);
}

/**
 * @brief MusicList_One::requestGetTrackFavorites : 트랙 하트 정보 요청
 * @param p_index
 */
void MusicList_One::requestGetTrackFavorites(const int &p_index){

    if(!global.user.getAccess_token().isEmpty()){
        //qDebug() << "-----requestGetTrackFavorites--001";
        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        //qDebug() << "-----requestGetTrackFavorites--001-1";

        //qDebug() << "-----requestGetTrackFavorites--001-2" << p_index << list_audioInfo.at(p_index)->getData();
        tmp_json.insert("url",list_audioInfo.at(p_index)->getData());
        //qDebug() << "-----requestGetTrackFavorites--002";
        //qDebug() << "-----requestGetTrackFavorites--001-1"<< global.device.getDeviceID();
        tmp_json.insert("mac_address",global.device.getDeviceID());
        NetworkHttp *network = new NetworkHttp;
        network->setProperty("index", p_index);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_FAVORITE_GET, QString("%1%2").arg(global.legacy_v3_api).arg(global.api_track_favorite_get), tmp_json, true, true);
        //qDebug() << "-----requestGetTrackFavorites--003";
    }
}

/**
 * @brief MusicList_One::requestSetTrackFavorites : 음악-트랙 즐겨찾기 세팅
 * @param p_index
 */
void MusicList_One::requestSetTrackFavorites(const int &p_index){
    if(!global.user.getAccess_token().isEmpty()){
        //qDebug() << "-----requestSetTrackFavorites--001";
        QListWidgetItem *item = listWidget->item(p_index);
        QMap<QString,QVariant> map = item->data(Qt::UserRole).toMap();
        bool tmp_isFavor = map["favorites"].toBool();
        //qDebug() << "-----requestSetTrackFavorites--002";
        QJsonArray tmp_jsonArrTrack;
        QJsonObject tmp_jsonTrack = getJsonObject_TrackData(p_index);
        tmp_jsonTrack.insert("mac_address", global.device.getDeviceID());
        tmp_jsonTrack.insert("type", "music");
        tmp_jsonTrack.insert("url",list_audioInfo.at(p_index)->getData());
        tmp_jsonTrack.insert("isFavorite", tmp_isFavor);
        tmp_jsonArrTrack.append(tmp_jsonTrack);
        //qDebug() << "-----requestSetTrackFavorites--003";
        QJsonObject tmp_json;
        tmp_json.insert("local_ip",global.device.getDeviceIP());
        tmp_json.insert("mac_address",global.device.getDeviceID());
        tmp_json.insert("url",list_audioInfo.at(p_index)->getData());
        tmp_json.insert("favorite", tmp_isFavor);
        tmp_json.insert("tracks", tmp_jsonArrTrack);
        //qDebug() << "-----requestSetTrackFavorites--004";
        if(tmp_isFavor){
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_ADD,  QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_add), tmp_json, true, true);

            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(list_audioInfo.at(p_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, list_audioInfo.at(p_index)->getData());
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
            //qDebug() << "-----requestSetTrackFavorites--005";
        }else{
            //qDebug() << "-----requestSetTrackFavorites--006";
            NetworkHttp *network = new NetworkHttp;
            network->setProperty("index", p_index);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_FAVORITE_DELETE, QString("%1%2").arg(global.legacy_v3).arg(global.api_track_favorite_delete), tmp_json, true, true);
            //qDebug() << "-----requestSetTrackFavorites--007";
        }


    }
}

/**
 * @brief MusicList_One::getJsonObject_OptionPopup : 옵션팝업에 사용할 데이터 반환
 * @param p_index
 * @return
 */
QJsonObject MusicList_One::getJsonObject_OptionPopup(const int &p_index){
    DataPopup *data = list_audioInfo.at(p_index);

    QJsonObject tmp_data = data->getJsonData();
    QJsonArray tmp_array;
    tmp_array.append(tmp_data);

    QJsonObject tmp_jsonObject;
    tmp_jsonObject.insert(KEY_OP_TITLE_MAIN, data->getTitle());
    tmp_jsonObject.insert(KEY_OP_TITLE_SUB, data->getArtist());
    tmp_jsonObject.insert(KEY_MAIN_CODE, GSCommon::MainMenuCode::Music);       // 강제로 넣어주는 경우 : AbstractMainContain 페이지가 아닌 다른 페이지에서 옵션팝업을 띄우는 경우
    tmp_jsonObject.insert(KEY_OP_TYPE, OptionPopup::TypeMenu::Music_Category_Track);
    tmp_jsonObject.insert(KEY_OP_albumImg, data->getAlbum_art());
    tmp_jsonObject.insert(KEY_OP_cntStar, 0);
    tmp_jsonObject.insert(KEY_OP_DATA, tmp_array);
    tmp_jsonObject.insert("isAlbum", false);

    return tmp_jsonObject;
}

/**
 * @brief MusicList_One::requestPlayMusic : 음악 재생
 * @param p_index
 */
void MusicList_One::requestPlayMusic(const int &p_index){
    emit linker->signal_clickedHoverItem(HOVER_CODE_PLAY, getJsonObject_OptionPopup(p_index));
}

/**
 * @brief MusicList_One::requestShowOptionPopup : 옵션팝업 Show
 * @param p_index
 */
void MusicList_One::requestShowOptionPopup(const int &p_index){
    emit linker->signal_clickedHoverItem(HOVER_CODE_MORE, getJsonObject_OptionPopup(p_index));
}


/**
 * @brief MusicList_One::setResultOfGetFavorite : 트랙 하트정보 요청결과 처리
 * @param p_jsonData QJsonObject response
 */
void MusicList_One::setResultOfGetFavorite(const int &p_idx, const QJsonObject &p_jsonData){

    const QString jsonKey_favorite = "favorite";

    if(p_jsonData.contains(jsonKey_favorite)){
        bool tmp_favorite = p_jsonData[jsonKey_favorite].toBool();
        setTrackFavoritesUI(p_idx, tmp_favorite);
    }
}

/**
 * @brief MusicList_One::setResultOfSetFavorite : 트랙 하트정보 SET 요청결과 처리
 * @param p_index int index
 * @param p_jsonData QJsonObject response
 */
void MusicList_One::setResultOfSetFavorite(const int &p_index, const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";

    bool tmp_flagOk = false;

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            tmp_flagOk = true;
        }
    }

    if(!tmp_flagOk){
        changeTrackFavoritesUI(p_index);
        ToastMsg::show(this,"",tr("You cannot change your favorites."));
        //ToastMsg::show(this,"",tr("즐겨찾기를 변경할 수 없습니다."));
    }
}

/**
 * @brief MusicList_One::slot_thumbnailDownloaded : [슬롯] 썸네일 다운로드 완료
 */
void MusicList_One::slot_thumbnailDownloaded(){

    FileDownloader *fileDownloader = qobject_cast<FileDownloader*>(sender());
    int tmp_index = fileDownloader->property("index").toInt();

    if(tmp_index < listWidget->count()){
        QImage image;
        bool flagLoaded = image.loadFromData(fileDownloader->downloadedData());

        if(flagLoaded){
            QPixmap tmp_pixmap;
            tmp_pixmap = tmp_pixmap.fromImage(image);
            tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

            listWidget->item(tmp_index)->setData(Qt::DecorationRole, tmp_pixmap);
        }
    }

    fileDownloader->deleteLater();
}

/**
 * @brief MusicList_One::slot_delegateClicked
 */
void MusicList_One::slot_delegateClicked(const int &p_index, const int &p_btnType){

    if(p_btnType == FileDelegate::BtnType::etc){
        requestPlayMusic(p_index);
    }
    else if(p_btnType == FileDelegate::BtnType::playListAdd){
        // 플레이리스트 담기
        proc_addPlaylist->setProperty("index", p_index);
        proc_addPlaylist->showDlgOfPlayList();
    }
    else if(p_btnType == FileDelegate::BtnType::trackHeart){
        // 하트 클릭
        changeTrackFavoritesUI(p_index);
        requestSetTrackFavorites(p_index);
    }
    else if(p_btnType == FileDelegate::BtnType::more){
        // 더보기 클릭
        requestShowOptionPopup(p_index);
    }

}



/**
 * @brief MusicList_One::slot_playlistClicked : [슬롯] 플레이리스트 선택 다이얼로그-플레이리스트 클릭
 * @param p_playlistno
 */
void MusicList_One::slot_playlistClicked(const int &p_playlistno){

    int tmp_index = sender()->property("index").toInt();

    if(p_playlistno >0){
        proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
        proc_addPlaylist->requestAddPlayListTrack(p_playlistno);

        if(tmp_index >=0 && tmp_index < list_audioInfo.size()){
            QString imgPath = QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(list_audioInfo.at(tmp_index)->getAlbum_art());
            Proc_UploadThumbnail *proc_uploadThumb = new Proc_UploadThumbnail(this);
            proc_uploadThumb->uploadThumnail(imgPath, list_audioInfo.at(tmp_index)->getData());
            connect(proc_uploadThumb, SIGNAL(signal_uploadedThumbnail()), proc_uploadThumb, SLOT(deleteLater()));
        }

    }else{
        proc_addPlaylist->showDlgOfNewPlayList();
    }
}

/**
 * @brief MusicList_One::slot_newPlaylistNameEdited : [슬롯] 신규 플레이리스트 명 입력 완료
 * @param p_name
 * @note 신규 플레이리스트 생성
 */
void MusicList_One::slot_newPlaylistNameEdited(QString p_name){

    int tmp_index = sender()->property("index").toInt();

    proc_addPlaylist->setTrackJsonArray(getJsonArr_TrackData(tmp_index));
    proc_addPlaylist->requestNewPlaylistTrack(p_name);

}

/**
 * @brief MusicList_One::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicList_One::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_FAVORITE_GET){
        setResultOfGetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_FAVORITE_DELETE){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }
    else if(p_id == HTTP_FAVORITE_ADD){
        setResultOfSetFavorite(sender()->property("index").toInt(), p_jsonObject);
    }

    sender()->deleteLater();
}

/**
 * @brief MusicList_One::wheelEvent : 휠 이벤트
 * @param event
 */

/*
void MusicList_One::wheelEvent(QWheelEvent *event){

    // 더 가져오기 실행
    if(event->delta() < 0){
        getTrackDataFromDB();
    }
}
*/

void MusicList_One::OnTimerCallbackFunc()//cheon210608-test
{
    /*
    int nMilliseconds = Etimerone.elapsed();
    if(nMilliseconds > 100000) Etimerone.restart();
    //QTime time = QTime::currentTime();
    //QString time_text = time.toString("hh : mm : ss");
    qDebug()<< "OnTimerCallbackFunc-nMilliseconds" << nMilliseconds;
    qDebug()<< "OnTimerCallbackFunc-NreloadFlag :" << NreloadFlag;
    //qDebug()<< "OnTimerCallbackFunc-NreloadFlag :" << NreloadFlag;
    //qDebug()<< "OnTimerCallbackFunc-PreloadFlag :" << PreloadFlag;

    if(NreloadFlag){//cheon210704-mouse
        //flagReqMore = false;//cheon210607-test
        if(nMilliseconds > 2000){//mouse_time_skippageterm)
            qDebug()<< "OnTimerCallbackFunc-nMilliseconds > 2000 " ;
            flagReqMore = false;
            NreloadFlag = false;
             getTrackDataFromDB();//cheon210608-test
        }
        else NreloadFlag = true;

    }
    if(PreloadFlag){//cheon210704-mouse
        flagReqMore = false;//cheon210607-test

        getTrackDataFromDB_pre();//cheon210608-test
        PreloadFlag = false;

    }
*/
}


void MusicList_One::getTrackDataFromDB_pre(){

    int LMT_CNT = 0;

    LMT_CNT = 100;
    if(OffsetPoint >= LMT_CNT){
        print_debug();
        qDebug() << "0--OffsetPoint%LMT_CNT=" << OffsetPoint%LMT_CNT;
        if(OffsetPoint%LMT_CNT !=0){
            OffsetPoint -= (OffsetPoint%LMT_CNT);
        }
        else OffsetPoint -= LMT_CNT;
    }
    else{
        OffsetPoint = 0;
        print_debug();
        return;
    }
    listWidget->verticalScrollBar()->setValue(0);
    list_audioInfo.clear();
    listWidget->clear();
    qDebug() << "0--currpage=" << curr_page_cnt;
    qDebug() << "0--OffsetPoint=" << OffsetPoint;


    if(OffsetPoint == 0){
        curr_page_cnt = 1;
    }
    else{
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;
    }

    total_page_cnt = 0;
    if((total_db_cnt % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt / LMT_CNT) + 1;
    }
    else if((total_db_cnt % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt / LMT_CNT);
    }
    if(curr_page_cnt > total_page_cnt ) return;//cheon210601


    albumtCnt->setText(QString("Total : %1                    Page : %2/%3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));


if(!flagReqMore ){//cheon210608-moti

    flagReqMore = true;//cheon210607-test
   // ToastMsg::show(this, "", tr("next."),1000);//cheon210608-moti
    //------------------------------------------------------------
    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    //qDebug() << "002---------------------------------------------------------------------------------------";
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        //strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data ";
        strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth ";
        strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";

        if(!strArg_1.isEmpty()){

            strQuery += " WHERE artist LIKE '%" + strArg_1 + "%' OR title LIKE '%" + strArg_1+ "%'";

            strQuery += " ORDER BY A.title ";
        }
        else{
            QString setWhere = "";
            QString setOrder = "";

            if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                 setWhere += " ((A.mime_type == 'audio/ape') || (A.mime_type == 'audio/flac') || (A.mime_type == 'audio/dff') || (A.mime_type == 'audio/dsf') || (A.mime_type == 'audio/wav') || (A.mime_type == 'audio/x-wav')) ";
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                /*if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                    setWhere += " AND (A.bitdepth >= 24) ";
                }
                else{
                    setWhere += " (A.bitdepth >= 24) ";
                }*/
                if(setWhere.isEmpty()){
                    setWhere += " (A.bitdepth >= 24) ";
                }
                else{
                    setWhere += " AND (A.bitdepth >= 24) ";
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.samplerate >= 88200) ";
                    }
                    else{
                        setWhere += " AND (A.samplerate >= 88200) ";
                    }
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.samplerate >= 176400) ";
                    }
                    else{
                        setWhere += " AND (A.samplerate >= 176400) ";
                    }
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6){
                    if(setWhere.isEmpty()){
                        setWhere += " (A.songrate == 0) ";
                    }
                    else{
                        setWhere += " AND (A.songrate == 0) ";
                    }
                }
                else{
                    switch (settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt()) {
                        case 0:
                            break;
                        case 1:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 5) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 5) ";
                            }
                            break;
                        case 2:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 4) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 4) ";
                            }
                            break;
                        case 3:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 3) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 3) ";
                            }
                            break;
                        case 4:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 2) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 2) ";
                            }
                            break;
                        case 5:
                            if(setWhere.isEmpty()){
                                setWhere += " (A.songrate == 1) ";
                            }
                            else{
                                setWhere += " AND (A.songrate == 1) ";
                            }
                            break;

                        case 6:
                            break;
                    }
                }
            }
            if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() > 0){
                if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 1){
                    setOrder += " ORDER BY A.title ";
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 2){
                    setOrder += " ORDER BY A.artist ";
                }
                else if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 3){
                    setOrder += " ORDER BY A.favorite ";
                }
            }

            if(!setWhere.isEmpty()){
                strQuery += " WHERE";
                strQuery += setWhere;
            }
            if(!setOrder.isEmpty()){
                strQuery += setOrder;
            }
        }

        //strQuery += " LIMIT 50 OFFSET "+ QString::number(listWidget->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);

        if(data.size() > 0){

           listWidget->clear();   //cheon210526

            for(int i = 0 ; i < data.size(); i++){

                QJsonObject tmp_data = data.at(i).toJsonObject();

                QString tmp_split = "";
                QStringList splitToken;
                QString tmp_type = "";

                QString tmp_mime = "";
                int tmp_sample = 0;
                int tmp_bit = 0;
                QString snd_mime = "";

                tmp_sample = tmp_data["samplerate"].toInt();
                tmp_bit = tmp_data["bitdepth"].toInt();

                tmp_split = tmp_data["mime_type"].toString();
                splitToken = tmp_split.split("/");

                if(splitToken.size() > 1){
                    tmp_type = splitToken.at(1);

                    if(tmp_type == "x-wav" || tmp_type == "wav"){
                        tmp_mime = "WAV";
                    }
                    else if (tmp_type == "wv") {
                        tmp_mime = "WV";
                    }
                    else if(tmp_type == "flac"){
                        tmp_mime = "FLAC";
                    }
                    else if(tmp_type == "dff" || tmp_type == "dsf"){
                        tmp_mime = "DSD";
                    }
                    else if(tmp_type == "mp4"){
                        tmp_mime = "ALAC";
                    }
                    else if(tmp_type == "mpeg"){
                        tmp_mime = "MP3";
                    }
                    else if(tmp_type == "amr"){
                        tmp_mime = "AMR";
                    }
                    else if(tmp_type == "amr-wb"){
                        tmp_mime = "AWB";
                    }
                    else if(tmp_type == "x-ms-wma"){
                        tmp_mime = "WMA";
                    }
                    else if(tmp_type == "ape"){
                        tmp_mime = "APE";
                    }
                    else if(tmp_type == "ogg"){
                        tmp_mime = "OGG";
                    }
                    else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                        tmp_mime = "AAC";
                    }
                    else if(tmp_type == "x-matroska"){
                        tmp_mime = "MKA";
                    }
                    else if(tmp_type == "midi"){
                        tmp_mime = "MIDI";
                    }
                    else if(tmp_type == "sp-midi"){
                        tmp_mime = "SMF";
                    }
                    else if(tmp_type == "imelody"){
                        tmp_mime = "IMY";
                    }
                    else if(tmp_type == "x-mpegurl"){
                        tmp_mime = "M3U8";
                    }
                    else if(tmp_type == "x-scpls"){
                        tmp_mime = "PLS";
                    }
                    else if(tmp_type == "aiff"){
                        tmp_mime = "AIFF";
                    }
                }

                if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                        || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                    snd_mime = tmp_mime;
                }
                else if(tmp_mime == "DSD"){
                    if(tmp_sample == 88200 || tmp_sample == 2822400){
                        snd_mime = tmp_mime + " 64";
                    }
                    else if(tmp_sample == 176400 || tmp_sample == 5644800){
                        snd_mime = tmp_mime + " 128";
                    }
                    else if(tmp_sample == 352800 || tmp_sample == 11289600){
                        snd_mime = tmp_mime + " 256";
                    }
                    else if(tmp_sample == 705600 || tmp_sample == 22579200){
                        snd_mime = tmp_mime + " 512";
                    }
                    else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                        snd_mime = tmp_mime + " 1024";
                    }
                    else{
                        snd_mime = tmp_mime;
                    }
                }
                else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                    if(tmp_bit == 0 || tmp_bit == 16){
                        snd_mime = tmp_mime;
                    }
                    else if(tmp_bit > 16){
                        snd_mime = tmp_mime + QString(" %1").arg(tmp_bit);
                    }
                }
                tmp_data.insert("album_mime", snd_mime);

                DataPopup *tmp_dataPopup = new DataPopup();
                tmp_dataPopup->setJsonData(tmp_data);
                this->list_audioInfo.append(tmp_dataPopup);

                QString tmp_albumImg = "";
                if(tmp_data.contains("album_art")){
                    tmp_albumImg = tmp_data["album_art"].toString();
                }

                QString tmp_strDuration="";
                int tmp_duration = tmp_data["duration"].toInt();
                if(tmp_duration >=0){
                    QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                    tmp_strDuration = tmp_time.toString("mm:ss");
                }

                QJsonObject json;
                json.insert("index", i);
                json.insert("fileName", tmp_dataPopup->getTitle());
                json.insert("duration", tmp_strDuration);
                json.insert("author", tmp_dataPopup->getArtist());
                json.insert("albumTitle", tmp_dataPopup->getAlbum());
                json.insert("title", tmp_dataPopup->getTitle());
                json.insert("isFile", true);
                json.insert("album_art", tmp_albumImg);
                json.insert("album_mime", snd_mime);

                QListWidgetItem *item = new QListWidgetItem;
                item->setData(Qt::UserRole, json);
                item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
                listWidget->addItem(item);

                FileDownloader *fileDownloader = new FileDownloader(this);
                fileDownloader->setProperty("index", i);
                fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
                connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                requestGetTrackFavorites(i);

            }

        }
            //OffsetPoint += data.size();

    }

    sqlite->close();
    delete sqlite;
    flagReqMore = false;
    }



}

/**
 * @brief MusicList_One::getTrackDataFromDB : 트랙 데이터 가져오기
 */
void MusicList_One::getTrackDataFromDB(){

print_debug();
        int LMT_CNT = 0;

        LMT_CNT = 100;

        qDebug() << "0-OffsetPoint" << OffsetPoint;


        if(OffsetPoint == 0){
            curr_page_cnt = 1;
        }
        else{
            print_debug();
            //qDebug() << "next_OffsetPoint = " << next_OffsetPoint;
            qDebug() << "OffsetPoint = " << OffsetPoint;
            if(total_db_cnt == OffsetPoint) return;
            //next_OffsetPoint = OffsetPoint;
            curr_page_cnt = OffsetPoint / LMT_CNT + 1;
        }
        qDebug() << "1-currpage" << curr_page_cnt;
        total_page_cnt = 0;
        if((total_db_cnt % LMT_CNT) > 0){
            total_page_cnt += (total_db_cnt / LMT_CNT) + 1;
        }
        else if((total_db_cnt % LMT_CNT) == 0){
            total_page_cnt += (total_db_cnt / LMT_CNT);
        }
        qDebug() << "total_page_cnt = " << total_page_cnt;
        qDebug() << "OffsetPoint = " << OffsetPoint;
        if(curr_page_cnt > total_page_cnt ) return;//cheon210601
        listWidget->verticalScrollBar()->setValue(0);
        list_audioInfo.clear();
        listWidget->clear();

        albumtCnt->setText(QString("Total : %1                    Page : %2/%3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));

 if(!flagReqMore ){//cheon210608-moti
        flagReqMore = true;//cheon210607-test
       // ToastMsg::show(this, "", tr("next."),1000);//cheon210608-moti
        //------------------------------------------------------------
        // DB 쿼리
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();

        if(err.type() == QSqlError::NoError){
            print_debug();
            QString strQuery = "";
            //strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data ";
            strQuery += " SELECT A._id AS id, A.title, A.artist, A.album, A.duration, AI._data AS album_art, A.album_id, A._data AS data, A.mime_type, A.samplerate, A.bitdepth ";
            strQuery += " FROM audio AS A LEFT JOIN album_art AS AI ON A.album_id=AI.album_id ";

            if(!strArg_1.isEmpty()){

                print_debug();
                qDebug() << "strArg_1= " << strArg_1;
                strQuery += " WHERE artist LIKE '%"+strArg_1+"%' OR title LIKE '%"+strArg_1+"%'";

                //strQuery += QString(" WHERE artist LIKE '%1%' OR title LIKE '%1%'").arg(strArg_1);

                strQuery += " ORDER BY A.title ";
            }
            else{
                QString setWhere = "";
                QString setOrder = "";

                if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                     setWhere += " ((A.mime_type == 'audio/ape') || (A.mime_type == 'audio/flac') || (A.mime_type == 'audio/dff') || (A.mime_type == 'audio/dsf') || (A.mime_type == 'audio/wav') || (A.mime_type == 'audio/x-wav')) ";
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_BIT,0).toInt() > 0){
                    /*if(settings->value(rosesettings.SETTINGS_FILTER_FILETYPE,0).toInt() > 0){
                        setWhere += " AND (A.bitdepth >= 24) ";
                    }
                    else{
                        setWhere += " (A.bitdepth >= 24) ";
                    }*/
                    if(setWhere.isEmpty()){
                        setWhere += " (A.bitdepth >= 24) ";
                    }
                    else{
                        setWhere += " AND (A.bitdepth >= 24) ";
                    }
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() > 0){
                    if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 1){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.samplerate >= 88200) ";
                        }
                        else{
                            setWhere += " AND (A.samplerate >= 88200) ";
                        }
                    }
                    else if(settings->value(rosesettings.SETTINGS_FILTER_SAMPLEING,0).toInt() == 2){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.samplerate >= 176400) ";
                        }
                        else{
                            setWhere += " AND (A.samplerate >= 176400) ";
                        }
                    }
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() > 0){
                    if(settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt() == 6){
                        if(setWhere.isEmpty()){
                            setWhere += " (A.songrate == 0) ";
                        }
                        else{
                            setWhere += " AND (A.songrate == 0) ";
                        }
                    }
                    else{
                        switch (settings->value(rosesettings.SETTINGS_FILTER_STAR,0).toInt()) {
                            case 0:
                            case 6:
                                break;
                            case 1:
                                if(setWhere.isEmpty()){
                                    setWhere += " (A.songrate == 5) ";
                                }
                                else{
                                    setWhere += " AND (A.songrate == 5) ";
                                }
                                break;
                            case 2:
                                if(setWhere.isEmpty()){
                                    setWhere += " (A.songrate == 4) ";
                                }
                                else{
                                    setWhere += " AND (A.songrate == 4) ";
                                }
                                break;
                            case 3:
                                if(setWhere.isEmpty()){
                                    setWhere += " (A.songrate == 3) ";
                                }
                                else{
                                    setWhere += " AND (A.songrate == 3) ";
                                }
                                break;
                            case 4:
                                if(setWhere.isEmpty()){
                                    setWhere += " (A.songrate == 2) ";
                                }
                                else{
                                    setWhere += " AND (A.songrate == 2) ";
                                }
                                break;
                            case 5:
                                if(setWhere.isEmpty()){
                                    setWhere += " (A.songrate == 1) ";
                                }
                                else{
                                    setWhere += " AND (A.songrate == 1) ";
                                }
                                break;
                        }
                    }
                }
                if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() > 0){
                    if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 1){
                        setOrder += " ORDER BY A.title ";
                    }
                    else if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 2){
                        print_debug();
                        setOrder += " ORDER BY A.artist ";
                    }
                    else if(settings->value(rosesettings.SETTINGS_FILTER_ORDER,0).toInt() == 3){
                        setOrder += " ORDER BY A.favorite ";
                    }
                }else{//c220511
                    setOrder += " ORDER BY A.title ";
                }

                if(!setWhere.isEmpty()){
                    strQuery += " WHERE";
                    strQuery += setWhere;
                }
                if(!setOrder.isEmpty()){
                    strQuery += setOrder;
                }
            }

           // strQuery += " LIMIT 50 OFFSET "+ QString::number(listWidget->count());
            strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);
            print_debug();
            qDebug() << "strQuery = " << strQuery;
            QVariantList data;
            sqlite->exec(strQuery, data);


            if(data.size() > 0){
                print_debug();

               listWidget->clear();   //cheon210526

                for(int i = 0 ; i < data.size(); i++){

                    QJsonObject tmp_data = data.at(i).toJsonObject();

                    QString tmp_split = "";
                    QStringList splitToken;
                    QString tmp_type = "";

                    QString tmp_mime = "";
                    int tmp_sample = 0;
                    int tmp_bit = 0;
                    QString snd_mime = "";

                    tmp_sample = tmp_data["samplerate"].toInt();
                    tmp_bit = tmp_data["bitdepth"].toInt();

                    tmp_split = tmp_data["mime_type"].toString();
                    splitToken = tmp_split.split("/");

                    if(splitToken.size() > 1){
                        tmp_type = splitToken.at(1);

                        if(tmp_type == "x-wav" || tmp_type == "wav"){
                            tmp_mime = "WAV";
                        }
                        else if (tmp_type == "wv") {
                            tmp_mime = "WV";
                        }
                        else if(tmp_type == "flac"){
                            tmp_mime = "FLAC";
                        }
                        else if(tmp_type == "dff" || tmp_type == "dsf"){
                            tmp_mime = "DSD";
                        }
                        else if(tmp_type == "mp4"){
                            tmp_mime = "ALAC";
                        }
                        else if(tmp_type == "mpeg"){
                            tmp_mime = "MP3";
                        }
                        else if(tmp_type == "amr"){
                            tmp_mime = "AMR";
                        }
                        else if(tmp_type == "amr-wb"){
                            tmp_mime = "AWB";
                        }
                        else if(tmp_type == "x-ms-wma"){
                            tmp_mime = "WMA";
                        }
                        else if(tmp_type == "ape"){
                            tmp_mime = "APE";
                        }
                        else if(tmp_type == "ogg"){
                            tmp_mime = "OGG";
                        }
                        else if(tmp_type == "aac" || tmp_type == "aac-adts"){
                            tmp_mime = "AAC";
                        }
                        else if(tmp_type == "x-matroska"){
                            tmp_mime = "MKA";
                        }
                        else if(tmp_type == "midi"){
                            tmp_mime = "MIDI";
                        }
                        else if(tmp_type == "sp-midi"){
                            tmp_mime = "SMF";
                        }
                        else if(tmp_type == "imelody"){
                            tmp_mime = "IMY";
                        }
                        else if(tmp_type == "x-mpegurl"){
                            tmp_mime = "M3U8";
                        }
                        else if(tmp_type == "x-scpls"){
                            tmp_mime = "PLS";
                        }
                        else if(tmp_type == "aiff"){
                            tmp_mime = "AIFF";
                        }
                    }

                    if(tmp_mime == "MP3" || tmp_mime == "AMR" || tmp_mime == "AWB" || tmp_mime == "WMA" || tmp_mime == "APE" || tmp_mime == "OGG" || tmp_mime == "AAC" || tmp_mime == "MKA"
                            || tmp_mime == "MIDI" || tmp_mime == "SMF" || tmp_mime == "IMY" || tmp_mime == "M3U8" || tmp_mime == "PLS" || tmp_mime == "AIFF" || tmp_mime == "WV"){

                        snd_mime = tmp_mime;
                    }
                    else if(tmp_mime == "DSD"){
                        if(tmp_sample == 88200 || tmp_sample == 2822400){
                            snd_mime = tmp_mime + " 64";
                        }
                        else if(tmp_sample == 176400 || tmp_sample == 5644800){
                            snd_mime = tmp_mime + " 128";
                        }
                        else if(tmp_sample == 352800 || tmp_sample == 11289600){
                            snd_mime = tmp_mime + " 256";
                        }
                        else if(tmp_sample == 705600 || tmp_sample == 22579200){
                            snd_mime = tmp_mime + " 512";
                        }
                        else if(tmp_sample == 1411200 || tmp_sample == 45158400){
                            snd_mime = tmp_mime + " 1024";
                        }
                        else{
                            snd_mime = tmp_mime;
                        }
                    }
                    else if(tmp_mime == "WAV" || tmp_mime == "FLAC" || tmp_mime == "ALAC"){
                        if(tmp_bit == 0 || tmp_bit == 16){
                            snd_mime = tmp_mime;
                        }
                        else if(tmp_bit > 16){
                            snd_mime = tmp_mime + QString(" %1").arg(tmp_bit);
                        }
                    }
                    tmp_data.insert("album_mime", snd_mime);

                    DataPopup *tmp_dataPopup = new DataPopup();
                    tmp_dataPopup->setJsonData(tmp_data);
/*
                    QString tmp_titleUTF8 = tmp_dataPopup->getTitle().toUtf8();
                    qDebug() << "tmp_titleUTF8=" << tmp_titleUTF8;
                    qDebug() << "tmp_dataPopup->getArtist().size=" << tmp_dataPopup->getArtist().size();
                    QString tmp_ArtistUTF8 = tmp_dataPopup->getArtist().toUtf8();
                    qDebug() << "tmp_ArtistUTF8=" << tmp_ArtistUTF8;
                    qDebug() << "tmp_ArtistUTF8_size=" << tmp_ArtistUTF8.size();
QChar tt = 'a';
                    if(tmp_ArtistUTF8[0]== QString::fromUtf8("ㅇ")){
                        print_debug();
                        qDebug() << "tmp_titleUTF8=" << tmp_titleUTF8;
                    }
*/
                    this->list_audioInfo.append(tmp_dataPopup);

                    QString tmp_albumImg = "";
                    if(tmp_data.contains("album_art")){
                        tmp_albumImg = tmp_data["album_art"].toString();
                    }

                    QString tmp_strDuration="";
                    int tmp_duration = tmp_data["duration"].toInt();
                    if(tmp_duration >=0){
                        QTime tmp_time = QTime::fromMSecsSinceStartOfDay(tmp_duration); // QTime("00:03:27.700")
                        tmp_strDuration = tmp_time.toString("mm:ss");
                    }

                    QJsonObject json;
                    json.insert("index", i);
                    json.insert("fileName", tmp_dataPopup->getTitle());
                    json.insert("duration", tmp_strDuration);
                    json.insert("author", tmp_dataPopup->getArtist());
                    json.insert("albumTitle", tmp_dataPopup->getAlbum());
                    json.insert("title", tmp_dataPopup->getTitle());
                    json.insert("isFile", true);
                    json.insert("album_art", tmp_albumImg);
                    json.insert("album_mime", snd_mime);

                    QListWidgetItem *item = new QListWidgetItem;
                    item->setData(Qt::UserRole, json);
                    item->setData(Qt::DecorationRole, QPixmap(":images/def_mus_60.png"));
                    listWidget->addItem(item);

                    FileDownloader *fileDownloader = new FileDownloader(this);
                    fileDownloader->setProperty("index", i);
                    fileDownloader->setImageURL( QString("http://%1:%2%3").arg(global.device.getDeviceIP()).arg(global.port_img).arg(tmp_albumImg));
                    connect(fileDownloader, SIGNAL(downloaded()), this, SLOT(slot_thumbnailDownloaded()));

                    requestGetTrackFavorites(i);

                }

            }
                OffsetPoint += data.size();
        }else{
            print_debug();
        }

        sqlite->close();
        delete sqlite;
        //c220511
    }
    flagReqMore = false;//c220511

}



void MusicList_One::wheelEvent(QWheelEvent *event){

#if defined(Q_OS_WINDOWS)
    CNT++;
    if(CNT > 10000) CNT = 1;
    //qDebug() << "event->angleDelta()" << event->angleDelta();
    if(event->angleDelta().ry() <= -120 && CNT%2 == 0){//cheon210607-test
#endif

#if defined(Q_OS_MAC)
  //  timer.start();//cheon210608-test
    if(event->angleDelta().ry() <= -120 ){//cheon210607-test
#endif

        curr_page_next++;
        getTrackDataFromDB();//cheon210608-test

       ToastMsg::show(this, "", tr("next."), 500);
    }
#if defined(Q_OS_WINDOWS)

    //qDebug() << "event->angleDelta()" << event->angleDelta();
    if(event->angleDelta().ry() >= 120 && CNT%2 == 0){//cheon210607-test
#endif

#if defined(Q_OS_MAC)

    if(event->angleDelta().ry() >= 120 ){//cheon210607-test
#endif

        getTrackDataFromDB_pre();//cheon210608-test
        ToastMsg::show(this, "", tr("pre."), 500);
    }

    //qDebug() << "event->angleDelta()" << event->angleDelta();

}
