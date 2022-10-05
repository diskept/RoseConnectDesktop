#include "musiclist_group.h"

#include "common/global.h"
#include <common/gscommon.h>
#include <common/rosesettings.h>
#include <common/sqlitehelper.h>
#include "widget/framemusicgenre.h"
#include "widget/framemusicartist.h"
#include "widget/framemusiccomposer.h"
#include "widget/toastmsg.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QWheelEvent>
#include <QScrollBar>
#include <QIntValidator>//cheon210717-search

#include <widget/frameartist.h>
#include <widget/framefriendplaylist.h>
#include <widget/framemyplaylist.h>
#include <widget/framerecentadd.h>
#include <widget/framerecentalbum.h>

#include "widget/NoData_Widget.h"

#include <common/networkhttp.h>

const int HTTP_RECENT_ALBUM = 99;
const int HTTP_RECENT_PLAYLIST = 88;
const int HTTP_MY_PLYALIST = 77;
const int HTTP_FRIEND_PLAYLIST = 66;


MusicList_Group::MusicList_Group(GroupMode p_mode, QWidget *parent) : QWidget(parent)
{
    this->groupMode = p_mode;
    setInit();
    setUIControl();
}

MusicList_Group::MusicList_Group(QString p_strArg, GroupMode p_mode, QWidget *parent)
    : QWidget(parent)
    , strArg_1(p_strArg)
{
    this->groupMode = p_mode;
    setUIControl();
}

/**
 * @brief MusicList_Group::setInit : 초기 세팅
 */
void MusicList_Group::setInit(){
    linker = Linker::getInstance();

    settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);

    if(this->groupMode == GroupMode::VIEWALL_MYPLAYLIST){
        this->pageCNT = 0;
        this->myPlaylist_cnt = 0;
        connect(linker, SIGNAL(signal_myPlaylistChanged()), SLOT(slot_myPlaylistChanged()));
    }
}

/**
 * @brief MusicList_Group::setUIControl : UI 세팅
 */
void MusicList_Group::setUIControl(){

    this->fl_albums = new FlowLayout(this);
    this->fl_albums->setSpacingHV(18,20);

    this->widget_scrollArea = new QWidget();
    this->widget_scrollArea->setContentsMargins(23,0,5,50);
    this->widget_scrollArea->setLayout(fl_albums);

    this->scrollArea = new QScrollArea();
    this->scrollArea->setObjectName("scrollArea");
    this->scrollArea->setWidget(widget_scrollArea);
    this->scrollArea->setWidgetResizable(true);
    //this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->scrollArea->setStyleSheet("#scrollArea { border:0px; }");

    //---------------------------------------------------------------------------//cheon210601
        albumtCnt = new QLabel;
        albumtCnt->setText(QString("Total : %1").arg(1));
        albumtCnt->setStyleSheet("font-size:18px; color:#dddddd;");

        QLabel *le_page_search = new QLabel;//cheon210527
        le_page_search->setText(QString("goto Page"));//cheon210527
        le_page_search->setStyleSheet("font-size:18px; color:#dddddd;");//cheon210527
        le_page_search->setFixedSize(90,30);//cheon210527

        this->le_lisearch = new QLineEdit();//cheon210717-search
        this->le_lisearch->setValidator( new QIntValidator(1, 65535, this) );//cheon210717-search
        this->le_lisearch->setObjectName("le_search");//cheon210527
        this->le_lisearch->setStyleSheet("#le_search { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:15px;}");//cheon210527
        this->le_lisearch->setMaxLength(5);//cheon210527
        this->le_lisearch->setFixedSize(100,30);//cheon210527
        this->le_lisearch->setTextMargins(1,1,1,1);
        this->le_lisearch->setText("");
        /*
        this->le_lisearch = new CustomLineEdit();//cheon210527
       // this->le_lisearch = new QTextEdit();//cheon210527
        this->le_lisearch->setObjectName("le_search");//cheon210527
        this->le_lisearch->setStyleSheet("#le_search { background-color:white; border-radius:5px; font-size:15px; color:#333333; padding-left:15px;}");//cheon210527
        this->le_lisearch->setMaxLength(5);//cheon210527
        this->le_lisearch->setFixedSize(100,30);//cheon210527
        this->le_lisearch->setTextMargins(1,1,1,1);
        this->le_lisearch->setText("");
*/

        QHBoxLayout *search_hBox = new QHBoxLayout();//cheon210527
        search_hBox->setSpacing(0);//cheon210527
        search_hBox->setContentsMargins(0,0,0,0);//cheon210527
       // search_hBox->setAlignment(Qt::AlignTop);//cheon210527
        search_hBox->addWidget(le_page_search);//cheon210527
        search_hBox->setSpacing(0);//cheon210527
        search_hBox->addWidget(this->le_lisearch);//cheon210527

        QPushButton *btn_ok = new QPushButton;
        btn_ok->setText(tr("go"));//btn_ok->setText(tr("추가"));
        btn_ok->setCursor(Qt::PointingHandCursor);
        btn_ok->setStyleSheet("font-size:16px;color:#FFFFFF;background-color:#707070;border-color:#707070;border-radius:5px;");
        btn_ok->setFixedSize(70,30);

        QWidget *widget_search = new QWidget();//cheon210527
        widget_search->setObjectName("widget_search");//cheon210527
        //widget_search->setFixedHeight(30);//cheon210527
        //widget_search->setFixedWidth(200);//cheon210527
        widget_search->setStyleSheet("#widget_search { background-color:#171717; }");//cheon210527
        widget_search->setLayout(search_hBox);//cheon210527

        QHBoxLayout *tmp2_hBox = new QHBoxLayout();//cheon210527
        tmp2_hBox->setSpacing(0);//cheon210527
        tmp2_hBox->setContentsMargins(50,0,220,0);//cheon210527
        tmp2_hBox->addWidget(widget_search,1, Qt::AlignVCenter);//cheon210527
        tmp2_hBox->setSpacing(10);
        tmp2_hBox->addWidget(btn_ok, 1, Qt::AlignVCenter);
       // tmp2_hBox->addWidget(btn_ok, 1, Qt::AlignVCenter | Qt::AlignLeft);

    //    albumtCnt->setText(QString("Total : %1            Page : %2/%3").arg(total_db_cnt).arg(curr_page_cnt).arg(total_page_cnt));//cheon210601

        QWidget *widget_search_last = new QWidget();//cheon210527
        widget_search_last->setObjectName("widget_search_last");//cheon210527
        widget_search_last->setStyleSheet("#widget_search_last { background-color:#171717; }");//cheon210527
        widget_search_last->setLayout(tmp2_hBox);//cheon210527



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
        le_str_search->setText(QString("Artist Search"));//cheon210527
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
        tmp_hBox->setSpacing(0);
        tmp_hBox->setContentsMargins(50,0,20,0);
       // tmp_hBox->setAlignment(Qt::AlignTop);

        tmp_hBox->addWidget(albumtCnt,0, Qt::AlignVCenter);
        tmp_hBox->setSpacing(30);
        tmp_hBox->addWidget(widget_search_last, 1, Qt::AlignVCenter|Qt::AlignLeft );//cheon210527
        if(groupMode==GroupMode::ARTIST ){
            tmp_hBox->addWidget(widget_search_str, 1, Qt::AlignVCenter|Qt::AlignLeft );//cheon210527
        }




        QWidget *widget_tt = new QWidget();
        widget_tt->setObjectName("widget_btnFilter");
        widget_tt->setStyleSheet("#widget_btnFilter { background-color:#171717; }");

        widget_tt->setLayout(tmp_hBox);


        //----------------------------------------------------------------------

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);




    if(groupMode==GroupMode::KIND ){
        vl_total->addWidget(widget_tt);//cheon210601

        total_db_cnt_kind = getAlbumDataFromDBforGenreTotalCount();//cheon210601

        connect(this->le_lisearch, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed()));//cheon210717-search
        connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedChange()));//cheon210717-search



    }
    if(groupMode==GroupMode::ARTIST ){
        vl_total->addWidget(widget_tt);//cheon210601

        total_db_cnt_artist = getAlbumDataFromDBforArtistTotalCount();//cheon210601
        connect(this->le_lisearch, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed()));//cheon210717-search
        connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedChange()));//cheon210717-search

        connect(this->le_lisearch_str, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed_str()));//cheon210717-search
        connect(btn_ok_str, SIGNAL(clicked()), this, SLOT(slot_clickedChange_str()));

        workerTriggerartist = new QTimer();//cheon210608-test
        threadartist = new QThread();//cheon210608-test
       // workerTrigger1->setInterval(300);//cheon210608-test

        connect(workerTriggerartist, SIGNAL(timeout()), this, SLOT(OnTimerCallbackFunc2()));//cheon210608-test
        connect(threadartist, SIGNAL(finished()), workerTriggerartist, SLOT(deleteLater()));//cheon210608-test

        workerTriggerartist->start(400);//cheon210608-test
        workerTriggerartist->moveToThread(threadartist);//cheon210608-test
        threadartist->start();//cheon210608-test
        Etimerartist.start();//cheon210608-test
    }
    if(groupMode==GroupMode::COMPOSER ){
        vl_total->addWidget(widget_tt);//cheon210601

        total_db_cnt_composer = getAlbumDataFromDBforComposerTotalCount();//cheon210601
        connect(this->le_lisearch, SIGNAL(returnPressed()), this, SLOT(slot_returnPressed()));//cheon210717-search
        connect(btn_ok, SIGNAL(clicked()), this, SLOT(slot_clickedChange()));//cheon210717-search
    }
    vl_total->addWidget(this->scrollArea);
    this->setLayout(vl_total);

    albumtCnt->setText(QString("Total : %1            Page : %2/%3").arg(total_db_cnt_kind).arg(curr_page_cnt).arg(total_page_cnt));//cheon210601



}

/**
 * @brief SearchRemoteBar::slot_returnPressed
 * @param p_text
 */
void MusicList_Group::slot_returnPressed(){//cheon210717-search
    QString tmp_page =this->le_lisearch->text();
     qDebug() << "slot_returnPressed-p_text" << tmp_page;
     if(groupMode==GroupMode::KIND ){
         this->goto_getGenreDataFromDB(tmp_page.toInt());
     }
     if(groupMode==GroupMode::ARTIST ){
         this->goto_getArtistDataFromDB(tmp_page.toInt());
     }
     if(groupMode==GroupMode::COMPOSER ){
         this->goto_getComposerDataFromDB(tmp_page.toInt());
     }
   // this->goto_getGenreDataFromDB(tmp_page.toInt());
    this->le_lisearch->setText("");
    // go_page_text ="";
}

void MusicList_Group::slot_clickedChange(){//cheon210717-search
    QString tmp_page =this->le_lisearch->text();
     qDebug() << "slot_returnPressed-p_text" << tmp_page;
     if(groupMode==GroupMode::KIND ){
         this->goto_getGenreDataFromDB(tmp_page.toInt());
     }
     if(groupMode==GroupMode::ARTIST ){
         this->goto_getArtistDataFromDB(tmp_page.toInt());
     }
     if(groupMode==GroupMode::COMPOSER ){
         this->goto_getComposerDataFromDB(tmp_page.toInt());
     }
   // this->goto_getGenreDataFromDB(tmp_page.toInt());
    this->le_lisearch->setText("");
}

void MusicList_Group::slot_returnPressed_str(){//cheon210717-search
    QString tmp_page =this->le_lisearch_str->text();
    qDebug() << "slot_returnPressed_str-p_text" << tmp_page;
    if(groupMode==GroupMode::ARTIST ){
        strArg_1 = tmp_page;
        total_db_cnt_artist = getAlbumDataFromDBforArtistTotalCount();
        OffsetPoint = 0;
        GSCommon::clearLayout_new(fl_albums);
        curr_page_cnt = 0;
        getDataFromDB();
    }
}

void MusicList_Group::slot_clickedChange_str(){//cheon210717-search
    QString tmp_page =this->le_lisearch_str->text();
    qDebug() << "slot_clickedChange_str-p_text" << tmp_page;
    if(groupMode==GroupMode::ARTIST ){
        strArg_1 = tmp_page;
        total_db_cnt_artist = getAlbumDataFromDBforArtistTotalCount();
        OffsetPoint = 0;
        GSCommon::clearLayout_new(fl_albums);
        curr_page_cnt = 0;

        getDataFromDB();
    }
}

void MusicList_Group::goto_getGenreDataFromDB(int p_page){//cheon210527
    if(p_page == 1){
        //flag_page_first = true;
    }
    if( total_page_cnt < p_page || p_page <= 0 ) {
        ToastMsg::show(this, "", tr("There are no pages.."), 5000);//cheon210527
        return;
    }
    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;
    pre_lmt_cnt = LMT_CNT;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        if(flag_page_first == true){
            //OffsetPoint += LMT_CNT;
            flag_page_first = false;
        }else{
            OffsetPoint = (p_page - 1) * LMT_CNT;
        }
        //qDebug() << "OffsetPoint" << OffsetPoint;

        QString strQuery = "";
        strQuery = " SELECT M.genre_id, M.audio_id, G.name, count(0) AS number_of_tracks ";
        strQuery += " FROM audio_genres_map AS M INNER JOIN audio_genres AS G ON M.genre_id=G._id ";
        strQuery += " GROUP BY M.genre_id ";
        strQuery += " ORDER BY G.name ";
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

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
        if((total_db_cnt_kind % LMT_CNT) > 0){
            total_page_cnt += (total_db_cnt_kind / LMT_CNT) + 1;
        }
        else if((total_db_cnt_kind % LMT_CNT) == 0){
            total_page_cnt += (total_db_cnt_kind / LMT_CNT);
        }

        albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_kind).arg(curr_page_cnt).arg(total_page_cnt));

        scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);
            for(int i = 0 ; i < data.size(); i++){
                QJsonObject jsonData = data.at(i).toJsonObject();
                int tmp_audio_id = jsonData["audio_id"].toInt();

                /// album_art 조회
                strQuery  = " SELECT AA._data AS album_art FROM album_art AS AA ";
                strQuery += " INNER JOIN audio AS VA ON AA.album_id=VA.album_id ";
                strQuery += " WHERE VA._id=" + QString::number(tmp_audio_id);

                QVariantList dataAlbumArt;
                sqlite->exec(strQuery,dataAlbumArt);

                if(dataAlbumArt.size() > 0){
                    jsonData.insert("album_art", dataAlbumArt[0].toJsonObject()["album_art"].toString());
                }

                FrameMusicGenre *tmp_widget = new FrameMusicGenre;
                tmp_widget->setData(jsonData);
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }

        }
    }
    sqlite->close();
    delete sqlite;
}
void MusicList_Group::goto_getArtistDataFromDB(int p_page){//cheon210527
    if(p_page == 1){
        //flag_page_first = true;
    }
    if( total_page_cnt < p_page || p_page <= 0 ) {
        ToastMsg::show(this, "", tr("There are no pages.."), 5000);//cheon210527
        return;
    }
    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;
    pre_lmt_cnt = LMT_CNT;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        if(flag_page_first == true){
            //OffsetPoint += LMT_CNT;
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
        if((total_db_cnt_artist % LMT_CNT) > 0){
            total_page_cnt += (total_db_cnt_artist / LMT_CNT) + 1;
        }
        else if((total_db_cnt_artist % LMT_CNT) == 0){
            total_page_cnt += (total_db_cnt_artist / LMT_CNT);
        }

        albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_artist).arg(curr_page_cnt).arg(total_page_cnt));

        scrollArea->verticalScrollBar()->setValue(0);

        //qDebug() << "OffsetPoint" << OffsetPoint;

        QString strQuery = "";
        strQuery = " SELECT AI._id AS artist_id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";

        //strQuery += " GROUP BY AI._id ";
        //strQuery += " ORDER BY AI.artist ";

        if(!strArg_1.isEmpty()){

            print_debug();
            qDebug() << "strArg_1= " << strArg_1;
            strQuery += " WHERE AI.artist LIKE '%"+strArg_1+"%'";

            //strQuery += QString(" WHERE  AI.artist LIKE '%1%'").arg(strArg_1);

        }
        strQuery += " GROUP BY AI._id ";
        strQuery += " ORDER BY AI.artist ";


        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);




        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);
            for(int i = 0 ; i < data.size(); i++){
                FrameMusicArtist *tmp_widget = new FrameMusicArtist;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }

        }
    }
    sqlite->close();
    delete sqlite;
}
void MusicList_Group::goto_getComposerDataFromDB(int p_page){//cheon210527
    if(p_page == 1){
        //flag_page_first = true;
    }
    if( total_page_cnt < p_page || p_page <= 0 ) {
        ToastMsg::show(this, "", tr("There are no pages.."), 5000);//cheon210527
        return;
    }
    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;
    pre_lmt_cnt = LMT_CNT;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        if(flag_page_first == true){
            //OffsetPoint += LMT_CNT;
            flag_page_first = false;
        }else{
            OffsetPoint = (p_page - 1) * LMT_CNT;
        }
        //qDebug() << "OffsetPoint" << OffsetPoint;
        qDebug() << "OffsetPoint" << OffsetPoint;

        QString strQuery = "";
        strQuery = " SELECT composer ,(SELECT C.numsongs FROM composers AS C WHERE C.name=composer) AS number_of_tracks, album_art ";
        strQuery += " FROM composer_albums_map_noid ";
        strQuery += " WHERE composer!='<unknown>'";
        strQuery += " GROUP By composer ";
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

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
        if((total_db_cnt_composer % LMT_CNT) > 0){
            total_page_cnt += (total_db_cnt_composer / LMT_CNT) + 1;
        }
        else if((total_db_cnt_composer % LMT_CNT) == 0){
            total_page_cnt += (total_db_cnt_composer / LMT_CNT);
        }

        albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_composer).arg(curr_page_cnt).arg(total_page_cnt));

        scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);
            for(int i = 0 ; i < data.size(); i++){
                FrameMusicComposer *tmp_widget = new FrameMusicComposer;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }

        }
    }
    sqlite->close();
    delete sqlite;

}

int MusicList_Group::getAlbumDataFromDBforGenreTotalCount(){//cheon210601
    int ret_cnt = 0;

    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

        strQuery = " SELECT M.genre_id, M.audio_id, G.name, count(0) AS number_of_tracks ";
        strQuery += " FROM audio_genres_map AS M INNER JOIN audio_genres AS G ON M.genre_id=G._id ";
        strQuery += " GROUP BY M.genre_id ";
        strQuery += " ORDER BY G.name ";


        QVariantList data;
        sqlite->exec(strQuery, data);
        ret_cnt = data.size();
    }
    sqlite->close();
    delete sqlite;
    return ret_cnt;
}
int MusicList_Group::getAlbumDataFromDBforArtistTotalCount(){//cheon210601
    int ret_cnt = 0;


    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

        strQuery = " SELECT AI._id AS artist_id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
        //strQuery += " GROUP BY AI._id ";
        //strQuery += " ORDER BY AI.artist ";

        if(!strArg_1.isEmpty()){

            print_debug();
            qDebug() << "strArg_1= " << strArg_1;
            strQuery += " WHERE AI.artist LIKE '%"+strArg_1+"%'";

            //strQuery += QString(" WHERE  AI.artist LIKE '%1%'").arg(strArg_1);
            strQuery += " GROUP BY AI._id ";
            strQuery += " ORDER BY AI.artist ";
        }

        QVariantList data;
        sqlite->exec(strQuery, data);
        ret_cnt = data.size();
        qDebug() << "ret_cnt= " << ret_cnt;
    }
    sqlite->close();
    delete sqlite;
    return ret_cnt;
}
int MusicList_Group::getAlbumDataFromDBforComposerTotalCount(){//cheon210601
    int ret_cnt = 0;
    // DB 쿼리
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

        strQuery = " SELECT composer ,(SELECT C.numsongs FROM composers AS C WHERE C.name=composer) AS number_of_tracks, album_art ";
        strQuery += " FROM composer_albums_map_noid ";
        strQuery += " WHERE composer!='<unknown>'";
        strQuery += " GROUP By composer ";


        QVariantList data;
        sqlite->exec(strQuery, data);
        ret_cnt = data.size();
    }
    sqlite->close();
    delete sqlite;
    return ret_cnt;
}
/**
 * @brief MusicList_Group::setDataGenreFromDB : 장르 데이터 세팅
 * @details 장르 데이터 세팅 FROM Rose DB
 */
void MusicList_Group::setDataGenreFromDB(){
    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;//--

    /*OffsetPoint += fl_albums->count();
    if(OffsetPoint == 0){
        curr_page_cnt = 1;
    }
    else{
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;
    }*/

    total_page_cnt = 0;
    if((total_db_cnt_kind % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_kind / LMT_CNT) + 1;
    }
    else if((total_db_cnt_kind % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_kind / LMT_CNT);
    }
    if(OffsetPoint > 0 && curr_page_cnt >= total_page_cnt ) return;//cheon210709-genre
    OffsetPoint += fl_albums->count();//cheon210709-genre
    if(OffsetPoint == 0){//cheon210709-genre
        curr_page_cnt = 1;//cheon210709-genre
    }//cheon210709-genre
    else{//cheon210709-genre
        if(total_db_cnt_kind == OffsetPoint) return;
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;//cheon210709-genre
    }//cheon210709-genre
    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_kind).arg(curr_page_cnt).arg(total_page_cnt));

qDebug() << "*OffsetPoint=" <<OffsetPoint ;
qDebug() << "*LMT_CNT=" <<LMT_CNT ;
qDebug() << "****flagReqMore=" <<flagReqMore ;

if(!flagReqMore){//cheon210607-test
    flagReqMore = true;//cheon210607-test
    //ToastMsg::show(this, "", tr("next."),50);
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";

        strQuery = " select  aiTable.genre_id as _id , genres_info.name, genres_info.numalbums, genres_info.numsongs, UPPER(genres_info.name) as head ,album_art._data as album_art  from  (select genres_info._id as genre_id, max( audio._id ) as audio_id, max(album_art.album_id) as album_id ,album_art._data  from audio left outer join album_art on ( audio.album_id = album_art.album_id ) join genres_info on (audio.genre_id = genres_info._id) group by genres_info._id)  as aiTable left outer join  album_art on ( aiTable.album_id = album_art.album_id  )  join genres_info on (aiTable.genre_id = genres_info._id) group by aiTable.genre_id  order by head  COLLATE NOCASE ";

        /*
        strQuery = " SELECT M.genre_id, M.audio_id, G.name, count(0) AS number_of_tracks ";
        strQuery += " FROM audio_genres_map AS M INNER JOIN audio_genres AS G ON M.genre_id=G._id ";
        strQuery += " GROUP BY M.genre_id ";
        strQuery += " ORDER BY G.name ";
        */
        //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);

        this->scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);   //cheon210526

            for(int i = 0 ; i < data.size(); i++){
                QJsonObject jsonData = data.at(i).toJsonObject();
                //--int tmp_audio_id = jsonData["audio_id"].toInt();
                //QJsonDocument doc(jsonData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " MusicList_Group::setDataGenreFromDB---jsonData_obj = " << strJson;

                /// album_art 조회
                //--strQuery  = " SELECT AA._data AS album_art FROM album_art AS AA ";
                //--strQuery += " INNER JOIN audio AS VA ON AA.album_id=VA.album_id ";
                //--strQuery += " WHERE VA._id=" + QString::number(tmp_audio_id);

                //--QVariantList dataAlbumArt;
                //--sqlite->exec(strQuery,dataAlbumArt);

                //--if(dataAlbumArt.size() > 0){
                //--    jsonData.insert("album_art", dataAlbumArt[0].toJsonObject()["album_art"].toString());
                //--}

                //QJsonDocument doc(jsonData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " MusicList_Group::setDataGenreFromDB---jsonData_obj = " << strJson;

                FrameMusicGenre *tmp_widget = new FrameMusicGenre;
                tmp_widget->setData(jsonData);
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }

    sqlite->close();
    delete sqlite;

    }
flagReqMore = false;

}

void MusicList_Group::setDataGenreFromDB_pre(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

   // OffsetPoint += fl_albums->count();
    if(OffsetPoint >= LMT_CNT){
        if(OffsetPoint%LMT_CNT !=0){
            OffsetPoint -= (OffsetPoint%LMT_CNT);
        }
        else OffsetPoint -= LMT_CNT;
    }
    else{
        OffsetPoint = 0;
        return;
    }
    if(OffsetPoint == 0){
        curr_page_cnt = 1;
    }
    else{
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;
    }

    total_page_cnt = 0;
    if((total_db_cnt_kind % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_kind / LMT_CNT) + 1;
    }
    else if((total_db_cnt_kind % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_kind / LMT_CNT);
    }

    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_kind).arg(curr_page_cnt).arg(total_page_cnt));

    if(!flagReqMore){//cheon210607-test
               flagReqMore = true;//cheon210607-test
    //ToastMsg::show(this, "", tr("pre."),50);
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT M.genre_id, M.audio_id, G.name, count(0) AS number_of_tracks ";
        strQuery += " FROM audio_genres_map AS M INNER JOIN audio_genres AS G ON M.genre_id=G._id ";
        strQuery += " GROUP BY M.genre_id ";
        strQuery += " ORDER BY G.name ";
        //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);//cheon210526
       // if(OffsetPoint >= fl_albums->count()){//cheon210526
        //    OffsetPoint -= fl_albums->count();//cheon210526
        //    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_kind).arg((OffsetPoint+1)/LMT_CNT+1).arg(total_db_cnt/(LMT_CNT)));
       // }//cheon210526
        QVariantList data;
        sqlite->exec(strQuery, data);


        this->scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);//cheon210526

            for(int i = 0 ; i < data.size(); i++){
                QJsonObject jsonData = data.at(i).toJsonObject();
                int tmp_audio_id = jsonData["audio_id"].toInt();

                /// album_art 조회
                strQuery  = " SELECT AA._data AS album_art FROM album_art AS AA ";
                strQuery += " INNER JOIN audio AS VA ON AA.album_id=VA.album_id ";

                strQuery += " WHERE VA._id=" + QString::number(tmp_audio_id);

                QVariantList dataAlbumArt;
                sqlite->exec(strQuery,dataAlbumArt);

                if(dataAlbumArt.size() > 0){
                    jsonData.insert("album_art", dataAlbumArt[0].toJsonObject()["album_art"].toString());
                }

                FrameMusicGenre *tmp_widget = new FrameMusicGenre;
                tmp_widget->setData(jsonData);
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }

    sqlite->close();
    delete sqlite;

    }
    flagReqMore = false;

}

void MusicList_Group::setDataArtistFromDB(){
    print_debug();
    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

    total_page_cnt = 0;
    if((total_db_cnt_artist % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_artist / LMT_CNT) + 1;
    }
    else if((total_db_cnt_artist % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_artist / LMT_CNT);
    }
    if(OffsetPoint > 0 && curr_page_cnt >= total_page_cnt ) return;//cheon210709-genre
    OffsetPoint += fl_albums->count();//cheon210709-genre
    qDebug() << "pre_OffsetPoint" <<OffsetPoint ;
    qDebug() << "pre_flagReqMore" <<flagReqMore ;
    if(OffsetPoint == 0){//cheon210709-genre
        curr_page_cnt = 1;//cheon210709-genre
    }//cheon210709-genre
    else{//cheon210709-genre
        if(total_db_cnt_artist == OffsetPoint) return;
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;//cheon210709-genre
    }//cheon210709-genre
    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_artist).arg(curr_page_cnt).arg(total_page_cnt));

    qDebug() << "OffsetPoint" <<OffsetPoint ;
    qDebug() << "flagReqMore" <<flagReqMore ;
    if(!flagReqMore){//cheon210607-test
        flagReqMore = true;//cheon210607-test
        // ToastMsg::show(this, "", tr("next."),50);
        SqliteHelper *sqlite = new SqliteHelper(this);
        QSqlError err = sqlite->addConnectionRose();
        if(err.type() == QSqlError::NoError){
            QString strQuery = "";
            strQuery = " SELECT AI._id AS artist_id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
            strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
            strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";

            print_debug();
            qDebug() << "strArg_1= " << strArg_1;
            if(!strArg_1.isEmpty()){

                print_debug();
                qDebug() << "strArg_1= " << strArg_1;
                strQuery += " WHERE AI.artist LIKE '%"+strArg_1+"%'";

                //strQuery += QString(" WHERE  AI.artist LIKE '%1%'").arg(strArg_1);
                strQuery += " GROUP BY AI._id ";
                strQuery += " ORDER BY AI.artist ";
            }else{
                strQuery += " GROUP BY AI._id ";
                strQuery += " ORDER BY AI.artist ";
            }
            //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
            strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

            QVariantList data;
            sqlite->exec(strQuery, data);

            this->scrollArea->verticalScrollBar()->setValue(0);
            qDebug() << "data.size()= " << data.size() ;
            if(data.size() > 0){
                GSCommon::clearLayout_new(fl_albums);   //cheon210526

                for(int i = 0 ; i < data.size(); i++){
                    FrameMusicArtist *tmp_widget = new FrameMusicArtist;
                    //QJsonDocument doc(data.at(i).toJsonObject());  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " MusicList_Group::setDataArtistFromDB---jsonData_obj = " << strJson;

                    tmp_widget->setData(data.at(i).toJsonObject());
                    tmp_widget->setHover();
                    this->fl_albums->addWidget(tmp_widget);
                }
            }
        }

        sqlite->close();
        delete sqlite;

    }
    flagReqMore = false;
}

void MusicList_Group::setDataArtistFromDB_pre(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

   // OffsetPoint += fl_albums->count();
    if(OffsetPoint >= LMT_CNT){
        if(OffsetPoint%LMT_CNT !=0){
            OffsetPoint -= (OffsetPoint%LMT_CNT);
        }
        else OffsetPoint -= LMT_CNT;
    }
    else{
        OffsetPoint = 0;
        return;
    }
    if(OffsetPoint == 0){
        curr_page_cnt = 1;
    }
    else{
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;
    }

    total_page_cnt = 0;
    if((total_db_cnt_artist % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_artist / LMT_CNT) + 1;
    }
    else if((total_db_cnt_artist % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_artist / LMT_CNT);
    }

    if(curr_page_cnt > total_page_cnt ) return;
        albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_artist).arg(curr_page_cnt).arg(total_page_cnt));

    if(!flagReqMore){//cheon210607-test
               flagReqMore = true;//cheon210607-test
    //ToastMsg::show(this, "", tr("pre."),50);
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT AI._id AS artist_id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";


        if(!strArg_1.isEmpty()){

            print_debug();
            qDebug() << "strArg_1= " << strArg_1;
            strQuery += " WHERE AI.artist LIKE '%"+strArg_1+"%'";

            //strQuery += QString(" WHERE  AI.artist LIKE '%1%'").arg(strArg_1);
            strQuery += " GROUP BY AI._id ";
            strQuery += " ORDER BY AI.artist ";
        }else{
            strQuery += " GROUP BY AI._id ";
            strQuery += " ORDER BY AI.artist ";
        }

        //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);//cheon210526
       // if(OffsetPoint >= fl_albums->count()){//cheon210526
        //    OffsetPoint -= fl_albums->count();//cheon210526
        //    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_artist).arg((OffsetPoint+1)/LMT_CNT+1).arg(total_db_cnt/(LMT_CNT)));
       // }//cheon210526
        QVariantList data;
        sqlite->exec(strQuery, data);


        this->scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);//cheon210526

            for(int i = 0 ; i < data.size(); i++){
                FrameMusicArtist *tmp_widget = new FrameMusicArtist;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }

    sqlite->close();
    delete sqlite;
    flagReqMore = false;
  }
    flagReqMore = false;
}


void MusicList_Group::setDataComposerFromDB(){
    print_debug();
    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

    //OffsetPoint += fl_albums->count();
    if(OffsetPoint == 0){
        curr_page_cnt = 1;
    }
    else{
        if(total_db_cnt_composer == OffsetPoint) return;
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;
    }
/*
    total_page_cnt = 0;
    if((total_db_cnt_composer % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_composer / LMT_CNT) + 1;
    }
    else if((total_db_cnt_composer % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_composer / LMT_CNT);
    }
    if(curr_page_cnt > total_page_cnt ) return;
        albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_composer).arg(curr_page_cnt).arg(total_page_cnt));
*/
    total_page_cnt = 0;
    if((total_db_cnt_composer % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_composer / LMT_CNT) + 1;
    }
    else if((total_db_cnt_composer % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_composer / LMT_CNT);
    }
    if(OffsetPoint > 0 && curr_page_cnt >= total_page_cnt ) return;//cheon210709-genre
    OffsetPoint += fl_albums->count();//cheon210709-genre
    if(OffsetPoint == 0){//cheon210709-genre
        curr_page_cnt = 1;//cheon210709-genre
    }//cheon210709-genre
    else{//cheon210709-genre
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;//cheon210709-genre
    }//cheon210709-genre
        albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_composer).arg(curr_page_cnt).arg(total_page_cnt));

qDebug() << "OffsetPoint" <<OffsetPoint ;
qDebug() << "flagReqMore" <<flagReqMore ;

  if(!flagReqMore){//cheon210607-test
         flagReqMore = true;//cheon210607-test
    //ToastMsg::show(this, "", tr("next."),50);
    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT composer ,(SELECT C.numsongs FROM composers AS C WHERE C.name=composer) AS number_of_tracks, album_art ";
        strQuery += " FROM composer_albums_map_noid ";
        strQuery += " WHERE composer!='<unknown>'";
        strQuery += " GROUP By composer ";
        //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);

        this->scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);   //cheon210526

            for(int i = 0 ; i < data.size(); i++){
                FrameMusicComposer *tmp_widget = new FrameMusicComposer;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }

    sqlite->close();
    delete sqlite;

  }
  flagReqMore = false;
}

void MusicList_Group::setDataComposerFromDB_pre(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

   // OffsetPoint += fl_albums->count();
    if(OffsetPoint >= LMT_CNT){
        if(OffsetPoint%LMT_CNT !=0){
            OffsetPoint -= (OffsetPoint%LMT_CNT);
        }
        else OffsetPoint -= LMT_CNT;
    }
    else{
        OffsetPoint = 0;
        return;
    }
    if(OffsetPoint == 0){
        curr_page_cnt = 1;
    }
    else{
        curr_page_cnt = OffsetPoint / LMT_CNT + 1;
    }

    total_page_cnt = 0;
    if((total_db_cnt_composer % LMT_CNT) > 0){
        total_page_cnt += (total_db_cnt_composer / LMT_CNT) + 1;
    }
    else if((total_db_cnt_composer % LMT_CNT) == 0){
        total_page_cnt += (total_db_cnt_composer / LMT_CNT);
    }

    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_composer).arg(curr_page_cnt).arg(total_page_cnt));

  if(!flagReqMore){//cheon210607-test
           flagReqMore = true;//cheon210607-test
    //ToastMsg::show(this, "", tr("pre."),50);

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT composer ,(SELECT C.numsongs FROM composers AS C WHERE C.name=composer) AS number_of_tracks, album_art ";
        strQuery += " FROM composer_albums_map_noid ";
        strQuery += " WHERE composer!='<unknown>'";
        strQuery += " GROUP By composer ";
        //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);//cheon210526
       // if(OffsetPoint >= fl_albums->count()){//cheon210526
        //    OffsetPoint -= fl_albums->count();//cheon210526
        //    albumtCnt->setText(QString("Total : %1                            Page : %2/%3").arg(total_db_cnt_composer).arg((OffsetPoint+1)/LMT_CNT+1).arg(total_db_cnt/(LMT_CNT)));
       // }//cheon210526
        QVariantList data;
        sqlite->exec(strQuery, data);


        this->scrollArea->verticalScrollBar()->setValue(0);

        if(data.size() > 0){
            GSCommon::clearLayout_new(fl_albums);//cheon210526

            for(int i = 0 ; i < data.size(); i++){
                FrameMusicComposer *tmp_widget = new FrameMusicComposer;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }

    sqlite->close();
    delete sqlite;

  }
  flagReqMore = false;
}

/**
 * @brief MusicList_Group::setDataArtistFromDB : 아티스트 데이터 세팅
 * @details 아티스트 데이터 세팅 FROM Rose DB
 */
/*
void MusicList_Group::setDataArtistFromDB(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 198;          //aritst width + H Spacing -> 180 + 18
    LMT_CNT = cnt * 10;

    OffsetPoint += fl_albums->count();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        //strQuery = " SELECT AI._id AS artist_id, AI.artist , AI.number_of_tracks ";
        strQuery = " SELECT AI._id AS artist_id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
        strQuery += " GROUP BY AI._id ";
        strQuery += " ORDER BY AI.artist ";
        //strQuery += " LIMIT 100 OFFSET " + QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                FrameMusicArtist *tmp_widget = new FrameMusicArtist;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}
*/
/**
 * @brief MusicList_Group::setDataArtistFromDB : 작곡가 데이터 세팅
 * @details 작곡가 데이터 세팅 FROM Rose DB
 * <unknown> 작곡가 제외
 */
/*
void MusicList_Group::setDataComposerFromDB(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //composer width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

    OffsetPoint += fl_albums->count();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){
        QString strQuery = "";
        strQuery = " SELECT composer ,(SELECT C.numsongs FROM composers AS C WHERE C.name=composer) AS number_of_tracks, album_art ";
        strQuery += " FROM composer_albums_map_noid ";
        strQuery += " WHERE composer!='<unknown>'";
        strQuery += " GROUP By composer ";
        //strQuery += " LIMIT 100 OFFSET "+QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                FrameMusicComposer *tmp_widget = new FrameMusicComposer;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}
*/


/**
 * @brief MusicList_Group::setDataViewAllPlayAlbumFromAPI [View All] 최근 재생 앨범 API 호출
 * @note 음악 > 홈 화면의 API와 동일하다
 */
void MusicList_Group::setDataViewAllPlayAlbumFromAPI(){
    /*if(!global.device.getDeviceIP().isEmpty()){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        json.insert("name", "recent_rose");

        network->request(HTTP_RECENT_ALBUM, QString("http://%1:%2/music.playlist.get")
                         .arg(global.device.getDeviceIP()).arg(global.port), json, true);
    }*/


    if((this->recent_album_count < this->recent_total_album_count) && (this->recent_album_beforeOffset == this->recent_album_currentOffset)){
        this->recent_pageCNT++;
        this->recent_album_currentOffset = this->recent_pageCNT;

        QJsonObject json;
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_RECENT_ALBUM, QString("%1/member/album/recent?type=MUSIC&excludeMediaType=&page=%2&size=20").arg(global.legacy_v1).arg(this->recent_pageCNT), json, false, true);
    }
}


/**
 * @brief MusicList_Group::setDataViewAllPlayAlbumFromAPI [View All] 최근 재생 앨범 API 호출
 * @note 음악 > 홈 화면의 API와 동일하다
 */
void MusicList_Group::setDataViewAllPlayPlaylistFromAPI(){

    if((this->recent_album_count < this->recent_total_album_count) && (this->recent_album_beforeOffset == this->recent_album_currentOffset)){
        this->recent_pageCNT++;
        this->recent_album_currentOffset = this->recent_pageCNT;

        QJsonObject json;
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_RECENT_PLAYLIST, QString("%1/member/playlist/recent?type=MUSIC&excludeMediaType=&page=%2&size=20").arg(global.legacy_v1).arg(this->recent_pageCNT), json, false, true);
    }
}
/**
 * @brief MusicList_Group::setDataViewAllMyPlayListFromAPI [View All] My 플레이리스트 API 호출
 * @note 음악 > 홈 화면의 API와 동일하다
 */
void MusicList_Group::setDataViewAllMyPlayListFromAPI(){
    if(!global.device.getDeviceIP().isEmpty()){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        /*json.insert("local_ip", global.device.getDeviceIP());
        json.insert("mac_address", global.device.getDeviceID());
        network->request(HTTP_MY_PLYALIST, QString("%1/playlist?page=0&size=50")
                         .arg(global.legacy_v3)
                         , json, true,true);*/

        network->request(HTTP_MY_PLYALIST, QString("%1/member/playlist/member/%2?sortType=PLAYLIST_RECENT&mediaType=MUSIC&page=%3&size=20")
                         .arg(global.legacy_v1).arg(global.user.getSub()).arg(this->pageCNT), json, false, true);
    }
}
/**
 * @brief MusicList_Group::setDataViewAllFriendPlayListFromAPI [View All] 친구 플레이리스트 API 호출
 * @note 음악 > 홈 화면의 API와 동일하다
 */
void MusicList_Group::setDataViewAllFriendPlayListFromAPI(){
    if(!global.device.getDeviceIP().isEmpty()){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject json;
        /*json.insert("local_ip", global.device.getDeviceIP());
        json.insert("mac_address", global.device.getDeviceID());
        json.insert("permission","1");
        network->request(HTTP_FRIEND_PLAYLIST, QString("%1/playlist/friend?page=0&size=50")
                         .arg(global.legacy_v3)
                         , json, true,true);*/

        network->request(HTTP_FRIEND_PLAYLIST, QString("%1/member/playlist/userpick/friend/MUSIC?sortType=PLAYLIST_RECENT&page=0&size=20")
                         .arg(global.legacy_v1).arg(global.user.getSub()), json, false, true);
    }
}
/**
 * @brief MusicList_Group::setResultOfRecentAlbum : 최근재생앨범 요청 결과 처리
 * @param p_jsonObject QJsonObject
 * @note 음악>홈 동일함수
 */
void MusicList_Group::setResultOfRecentAlbum(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_recentList = "recentList";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){

        if(p_jsonObject.contains(jsonKey_recentList)){
            this->recent_total_album_count = ProcJsonEasy::getInt(p_jsonObject, "totalCount");
            QJsonArray jsonRecentList = ProcJsonEasy::getJsonArray(p_jsonObject, jsonKey_recentList);

            if(this->recent_total_album_count > 0){
                for(int i = 0; i < jsonRecentList.size(); i++){
                    QJsonObject tmpObj = jsonRecentList.at(i).toObject();
                    QJsonObject recentObj = ProcJsonEasy::getJsonObject(tmpObj, "album");

                    FrameRecentAlbum *tmp_widget = new FrameRecentAlbum(true, this);
                    tmp_widget->setData(recentObj);
                    tmp_widget->setHover();
                    this->fl_albums->addWidget(tmp_widget);
                }
            }
            else{
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Album_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->fl_albums->addWidget(noData_widget);
            }
        }

        this->recent_album_beforeOffset = this->recent_album_currentOffset;
    }
}


void MusicList_Group::setResultOfRecentPlaylist(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_recentList = "recentList";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){

        if(p_jsonObject.contains(jsonKey_recentList)){
            this->recent_total_album_count = ProcJsonEasy::getInt(p_jsonObject, "totalCount");
            QJsonArray jsonRecentList = ProcJsonEasy::getJsonArray(p_jsonObject, jsonKey_recentList);

            if(this->recent_total_album_count > 0){
                for(int i = 0; i < jsonRecentList.size(); i++){
                    QJsonObject tmpObj = jsonRecentList.at(i).toObject();
                    QJsonObject recentObj = ProcJsonEasy::getJsonObject(tmpObj, "playlist");

                    //FrameRecentAlbum *tmp_widget = new FrameRecentAlbum(true, this);
                    FrameMyPlayList *tmp_widget = new FrameMyPlayList(true,this);
                    tmp_widget->setData(recentObj);
                    tmp_widget->setHover();
                    this->fl_albums->addWidget(tmp_widget);
                }
            }
            else{
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->fl_albums->addWidget(noData_widget);
            }
        }

        this->recent_album_beforeOffset = this->recent_album_currentOffset;
    }
}


/**
 * @brief MusicList_Group::setDataViewAllAddedAlbumFromDB [View All] 최근 추가 된 앨범
 * @note 음악 > 홈 화면의 최근 추가 된 앨범 쿼리와 동일하다
 */
void MusicList_Group::setDataViewAllAddedAlbumFromDB(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

    OffsetPoint += fl_albums->count();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){

        QString strQuery = "";
        /*strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AA._data AS album_art  ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        strQuery += " ORDER BY AI.date_added desc ";
        strQuery += " LIMIT 30";*/

        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        strQuery += " ORDER BY AI.date_added desc ";
        //strQuery += " LIMIT 100";
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);


        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){


                QMap<QString, QVariant> map = data.at(i).toMap();

                QJsonObject jsonData = data.at(i).toJsonObject();
                if(map.contains("album")){
                    jsonData.insert("title", map["album"].toString());  // key 추가
                }
                if(map.contains("path")){
                    jsonData.insert("data", map["path"].toString());    // key 변경
                }
                if(map.contains("audio_id")){
                    jsonData.insert("id", map["audio_id"].toString());          // key 변경
                }
                jsonData.insert("section", "recent_add");


                FrameRecentAdd *tmp_widget = new FrameRecentAdd(true, this);
                tmp_widget->setData(jsonData);
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

/**
 * @brief MusicList_Group::setDataViewAllSearchAlbumFromDB [View All] 검색 앨범
 */
void MusicList_Group::setDataViewAllSearchAlbumFromDB(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 204;          //album width + H Spacing -> 186 + 18
    LMT_CNT = cnt * 10;

    OffsetPoint += fl_albums->count();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){

        QString strQuery = "";
        strQuery += " SELECT AI._id AS album_id, AI.album, AI.artist, AI.numsongs AS num_of_song, AI.mime_type, AI.samplerate, AI.bitdepth, AA._data AS album_art  ";
        strQuery += " FROM album_info AS AI ";
        strQuery += " LEFT OUTER JOIN album_art AS AA ";
        strQuery += " ON AI._id=AA.album_id ";
        strQuery += " WHERE AI.album LIKE '%"+strArg_1+"%' OR AI.artist LIKE '%"+strArg_1+"%'";
        strQuery += " ORDER BY AI.album ";
        //strQuery += " LIMIT 100";
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);


        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){


                QMap<QString, QVariant> map = data.at(i).toMap();

                QJsonObject jsonData = data.at(i).toJsonObject();
                if(map.contains("album")){
                    jsonData.insert("title", map["album"].toString());  // key 추가
                }
                if(map.contains("path")){
                    jsonData.insert("data", map["path"].toString());    // key 변경
                }
                if(map.contains("audio_id")){
                    jsonData.insert("id", map["audio_id"].toString());  // key 변경
                }
                jsonData.insert("section", "search_album");

                FrameRecentAdd *tmp_widget = new FrameRecentAdd(true, this);
                tmp_widget->setData(jsonData);
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

/**
 * @brief MusicList_Group::setDataViewAllSearchAlbumFromDB [View All] 검색 앨범
 */
void MusicList_Group::setDataViewAllArtistFromDB(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 198;          //artist width + H Spacing -> 180 + 18
    LMT_CNT = cnt * 10;

    OffsetPoint += fl_albums->count();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){

        QString strQuery = "";
        //strQuery = " SELECT AI._id , AI.artist , AI.number_of_tracks, ";
        strQuery = " SELECT AI._id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
        strQuery += " GROUP BY AI._id ";
        strQuery += " ORDER BY AI.artist ";
        //strQuery += " LIMIT 100 OFFSET "+ QString::number(fl_albums->count());
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                FrameArtist *tmp_widget = new FrameArtist;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

/**
 * @brief MusicList_Group::setResultOfMyPlayList : MY 플레이리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject
 * @note 음악>홈 동일함수
 */
void MusicList_Group::setResultOfMyPlayList(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_totalcount = "totalcount";
    const QString jsonKey_totalCount = "totalCount";
    const QString jsonKey_size = "size";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            int tmp_cntPlayList = p_jsonObject[jsonKey_totalcount].toInt(0);

            if(tmp_cntPlayList){
                if(p_jsonObject.contains(jsonKey_playLists)){
                    QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();

                    for(int i = 0; i < jsonArr.size(); i++){
                        FrameMyPlayList *tmp_widget = new FrameMyPlayList(true,this);
                        tmp_widget->setData(jsonArr.at(i).toObject());
                        tmp_widget->setHover();
                        this->fl_albums->addWidget(tmp_widget);
                    }
                    //stackMyPlayList->setCurrentIndex(2);
                }
            }
            else{
                //lb_emptyMyPlayList->setText(tr("There is no My Playlist."));
                //lb_emptyMyPlayList->setText(tr("My 플레이 리스트가 없습니다."));
            }
        }
        else if(p_jsonObject.contains(jsonKey_totalCount)){

            int tmp_cntPlayList = p_jsonObject[jsonKey_totalCount].toInt(0);

            if(tmp_cntPlayList > 0){
                if(p_jsonObject.contains(jsonKey_playLists)){
                    QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();
                    this->myPlaylist_cnt += jsonArr.size();

                    for(int i = 0; i < jsonArr.size(); i++){
                        FrameMyPlayList *tmp_widget = new FrameMyPlayList(true,this);
                        tmp_widget->setData(jsonArr.at(i).toObject());
                        tmp_widget->setHover();
                        this->fl_albums->addWidget(tmp_widget);
                    }

                    if(this->myPlaylist_cnt < p_jsonObject[jsonKey_totalCount].toInt(0)){
                        this->pageCNT++;
                        this->setDataViewAllMyPlayListFromAPI();
                    }
                }
            }else{
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::Playlist_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->fl_albums->addWidget(noData_widget);
            }
        }
    }
    /*
    if(!flagOk){
        lb_emptyMyPlayList->setText(tr("My playlist information cannot be checked."));
        //lb_emptyMyPlayList->setText(tr("My 플레이 리스트 정보를 확인할 수 없습니다."));
    }
    */
}

/**
 * @brief MusicList_Group::setResultOfFrienPlayList : 친구 플레이리스트 요청 결과 처리
 * @param p_jsonObject QJsonObject
 * @note 음악>홈 동일함수
 */
void MusicList_Group::setResultOfFrienPlayList(const QJsonObject &p_jsonObject){

    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_playLists = "playlists";
    const QString jsonKey_totalcount = "totalcount";
    const QString jsonKey_totalCount = "totalCount";

    if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool(false)){
        if(p_jsonObject.contains(jsonKey_totalcount)){

            int tmp_cntPlayList = p_jsonObject[jsonKey_totalcount].toInt(0);

            if(tmp_cntPlayList){
                if(p_jsonObject.contains(jsonKey_playLists)){
                    QJsonArray jsonArr = p_jsonObject[jsonKey_playLists].toArray();

                    for(int i = 0; i < jsonArr.size(); i++){
                        FrameFriendPlayList *tmp_widget = new FrameFriendPlayList(true,this);
                        tmp_widget->setData(jsonArr.at(i).toObject());
                        tmp_widget->setHover();
                        this->fl_albums->addWidget(tmp_widget);
                    }
                    //stackFriendPlayList->setCurrentIndex(2);
                }
            }else{
                //lb_emptyFriendPlayList->setText(tr("There is no friend playlist."));
                //lb_emptyFriendPlayList->setText(tr("친구 플레이 리스트가 없습니다."));
            }
        }
        else if(p_jsonObject.contains(jsonKey_totalCount)){
            int tmp_cntPlayList = p_jsonObject[jsonKey_totalCount].toInt(0);

            if(tmp_cntPlayList){
                if(p_jsonObject.contains("userPicks")){
                    QJsonArray jsonArr_userpicks = p_jsonObject["userPicks"].toArray();

                    for(int idx = 0; idx < jsonArr_userpicks.size(); idx++){
                        QJsonObject jsonUserPicks = jsonArr_userpicks.at(idx).toObject();

                        if(jsonUserPicks.contains(jsonKey_playLists)){
                            QJsonArray jsonArr = jsonUserPicks[jsonKey_playLists].toArray();

                            for(int sub_idx = 0; sub_idx < jsonArr.size(); sub_idx++){
                                FrameFriendPlayList *tmp_widget = new FrameFriendPlayList(true,this);
                                tmp_widget->setData(jsonArr.at(sub_idx).toObject());
                                tmp_widget->setHover();
                                this->fl_albums->addWidget(tmp_widget);
                            }
                        }
                    }
                }
            }else{
                //lb_emptyFriendPlayList->setText(tr("There is no friend playlist."));
                //lb_emptyFriendPlayList->setText(tr("친구 플레이 리스트가 없습니다."));
            }
        }
    }
    /*
    if(!flagOk){
        lb_emptyFriendPlayList->setText(tr("I can't check my friend's playlist information."));
        //lb_emptyFriendPlayList->setText(tr("친구 플레이 리스트 정보를 확인할 수 없습니다."));
    }
    */
}

/**
 * @brief MusicList_Group::setDataViewAllSearchArtistFromDB [View All] 추천 아티스트 데이터 세팅
 * @note 음악 > 홈 화면의 추천 아티스트 쿼리와 동일하다
 */
void MusicList_Group::setDataViewAllSearchArtistFromDB(){

    int cnt = 0, LMT_CNT = 0;
    int hwindow_width = 0;  //global.LmtCnt - 100;

    if(this->width() < 940){
        if(global.LmtCnt > 1200){
            hwindow_width = global.LmtCnt - 240;
        }
        else{
            hwindow_width = global.LmtCnt - 80;
        }
    }
    else{
        hwindow_width = this->width();
    }

    cnt = hwindow_width / 198;          //album width + H Spacing -> 180 + 18
    LMT_CNT = cnt * 10;

    OffsetPoint += fl_albums->count();

    SqliteHelper *sqlite = new SqliteHelper(this);
    QSqlError err = sqlite->addConnectionRose();
    if(err.type() == QSqlError::NoError){

        QString strQuery = "";
        //strQuery = " SELECT AI._id , AI.artist , AI.number_of_tracks, ";
        strQuery = " SELECT AI._id, AI.artist, AI.number_of_albums, AI.number_of_tracks, ";
        strQuery += " (SELECT _data FROM album_art WHERE album_id=M.album_id) AS album_art";
        strQuery += " FROM artist_info AS AI INNER JOIN artists_albums_map AS M ON AI._id=M.artist_id ";
        strQuery += " WHERE AI.artist LIKE '%"+strArg_1+"%'";
        strQuery += " GROUP BY AI._id ";
        //strQuery += " LIMIT 100";
        strQuery += QString(" LIMIT %1 OFFSET ").arg(LMT_CNT) + QString::number(OffsetPoint);

        QVariantList data;
        sqlite->exec(strQuery, data);
        if(data.size() > 0){
            for(int i = 0 ; i < data.size(); i++){
                FrameArtist *tmp_widget = new FrameArtist;
                tmp_widget->setData(data.at(i).toJsonObject());
                tmp_widget->setHover();
                this->fl_albums->addWidget(tmp_widget);
            }
        }
    }
    sqlite->close();
    delete sqlite;
}

/**
 * @brief MusicList_Group::slot_responseHttp : Http 요청 결과 처리 함수
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void MusicList_Group::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    if(p_id == HTTP_RECENT_ALBUM){
        setResultOfRecentAlbum(p_jsonObject);
    }
    else if(p_id == HTTP_RECENT_PLAYLIST){
        setResultOfRecentPlaylist(p_jsonObject);
    }
    else if(p_id == HTTP_MY_PLYALIST){
        setResultOfMyPlayList(p_jsonObject);
    }
    else if(p_id == HTTP_FRIEND_PLAYLIST){
        setResultOfFrienPlayList(p_jsonObject);
    }
    sender()->deleteLater();
}

/**
 * @brief MusicList_Group::setDataFromDB DB에서 가져와서 전체 데이터 세팅
 * @param p_arg
 */
void MusicList_Group::setDataFromDB(const QString &p_arg){
    strArg_1 = p_arg;

    setDataFromDB();
}

/**
 * @brief MusicList_Group::setDataFromDB DB에서 가져와서 전체 데이터 세팅
 */
void MusicList_Group::setDataFromDB(){
    print_debug();
    OffsetPoint = 0;//c220511
    GSCommon::clearLayout(this->fl_albums);
    scrollArea->verticalScrollBar()->setValue(0);

    this->fl_albums->setSpacingHV(18,20);

    if(this->groupMode == GroupMode::VIEWALL_PLAYALBUM){
        QJsonObject json;
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_RECENT_ALBUM, QString("%1/member/album/recent?type=MUSIC&excludeMediaType=&page=0&size=20").arg(global.legacy_v1), json, false, true);
    }
    else if(this->groupMode == GroupMode::VIEWALL_PLAYPLAYLIST){
        QJsonObject json;
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_RECENT_PLAYLIST, QString("%1/member/playlist/recent?type=MUSIC&excludeMediaType=&page=0&size=20").arg(global.legacy_v1), json, false, true);
    }
    else{
        getDataFromDB();
    }
}

/**
 * @brief MusicList_Group::getDataFromDB : setDataFromDB 에서 분리
 */
void MusicList_Group::getDataFromDB(){
    print_debug();
    qDebug() << "groupMode = " << groupMode;
    if(groupMode==GroupMode::KIND){
        setDataGenreFromDB();
    }else if(groupMode==GroupMode::ARTIST){
        print_debug();
        setDataArtistFromDB();
    }else if(groupMode==GroupMode::COMPOSER){
        setDataComposerFromDB();
    }else if(groupMode==GroupMode::VIEWALL_PLAYALBUM){
        setDataViewAllPlayAlbumFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_PLAYPLAYLIST){
        setDataViewAllPlayAlbumFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_ADDEDALBUM){
        setDataViewAllAddedAlbumFromDB();
    }else if(groupMode==GroupMode::VIEWALL_MYPLAYLIST){
        this->fl_albums->setSpacingHV(18,40);
        this->pageCNT = 0;
        this->myPlaylist_cnt = 0;
        setDataViewAllMyPlayListFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_FRIENDPLAYLIST){
        this->fl_albums->setSpacingHV(18,40);
        setDataViewAllFriendPlayListFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_RECOMMENDARTIST){
        this->fl_albums->setSpacingHV(0,5);
        setDataViewAllArtistFromDB();
    }else if(groupMode==GroupMode::VIEWALL_SEARCHALBUM){
        setDataViewAllSearchAlbumFromDB();
    }else if(groupMode==GroupMode::VIEWALL_SEARCHARTIST){
        setDataViewAllSearchArtistFromDB();
    }
    qDebug()<< "MusicList_Group::getDataFromDB----flagReqMore = " << flagReqMore;
}

void MusicList_Group::getDataFromDB_pre(){
    if(groupMode==GroupMode::KIND){
        setDataGenreFromDB_pre();
    }else if(groupMode==GroupMode::ARTIST){
        setDataArtistFromDB_pre();
    }else if(groupMode==GroupMode::COMPOSER){
        setDataComposerFromDB_pre();
    }else if(groupMode==GroupMode::VIEWALL_PLAYALBUM){
       // setDataViewAllPlayAlbumFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_ADDEDALBUM){
      //  setDataViewAllAddedAlbumFromDB();
    }else if(groupMode==GroupMode::VIEWALL_MYPLAYLIST){
      //  this->fl_albums->setSpacingHV(18,40);
      //  setDataViewAllMyPlayListFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_FRIENDPLAYLIST){
      //  this->fl_albums->setSpacingHV(18,40);
      //  setDataViewAllFriendPlayListFromAPI();
    }else if(groupMode==GroupMode::VIEWALL_RECOMMENDARTIST){
      //  this->fl_albums->setSpacingHV(0,5);
       // setDataViewAllArtistFromDB();
    }else if(groupMode==GroupMode::VIEWALL_SEARCHALBUM){
       // setDataViewAllSearchAlbumFromDB();
    }else if(groupMode==GroupMode::VIEWALL_SEARCHARTIST){
      //  setDataViewAllSearchArtistFromDB();
    }
}



void MusicList_Group::OnTimerCallbackFunc2()//cheon210608-test
{
  if(groupMode==GroupMode::ARTIST){
      int nMilliseconds = Etimerartist.elapsed();
      if(nMilliseconds > 100000) Etimerartist.restart();
      //QTime time = QTime::currentTime();
      //QString time_text = time.toString("hh : mm : ss");
      //qDebug()<< "OnTimerCallbackFunc-nMilliseconds" << nMilliseconds;
      //qDebug()<< "OnTimerCallbackFunc-NreloadFlag :" << NreloadFlag;
      //qDebug()<< "OnTimerCallbackFunc-PreloadFlag :" << PreloadFlag;

      if(NreloadFlag){//cheon210704-mouse
          //flagReqMore = false;//cheon210607-test
          if(nMilliseconds > 2000){//mouse_time_skippageterm)
              flagReqMore = false;
               getDataFromDB();//cheon210608-test
          }
          NreloadFlag = false;

      }
      if(PreloadFlag){//cheon210704-mouse
          flagReqMore = false;//cheon210607-test

          getDataFromDB_pre();//cheon210608-test
          PreloadFlag = false;

      }

  }

}


/**
 * @brief MusicList_Group::wheelEvent : 휠 이벤트
 * @param event
 */
void MusicList_Group::wheelEvent(QWheelEvent *event){

    // 더 가져오기 실행

    //qDebug() << this->scrollArea->verticalScrollBar()->sliderPosition();
    /*scroll_pos_currunt = this->scrollArea->verticalScrollBar()->sliderPosition();

    if(event->delta() < -60 && (scroll_pos_currunt > scroll_pos_before)){
        if(groupMode==GroupMode::KIND
                || groupMode==GroupMode::ARTIST
                || groupMode==GroupMode::COMPOSER
                || groupMode==GroupMode::VIEWALL_RECOMMENDARTIST){
            getDataFromDB();
        }
    }

    scroll_pos_before = scroll_pos_currunt;*/


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
      if(groupMode==GroupMode::ARTIST){
        int nMilliseconds = Etimerartist.elapsed();
        //qDebug() << "nMilliseconds = " << nMilliseconds;
        //qDebug() << "flagReqMore = " << flagReqMore;
        if(flagReqMore == true){
            if(nMilliseconds< global.mouse_time_term){//cheon210704-mouse
                //flagReqMore = true;//cheon210607-test
                if(!threadartist->isRunning()){
                    workerTriggerartist = new QTimer();//cheon210608-test
                    threadartist = new QThread();//cheon210608-test
                   // workerTrigger1->setInterval(300);//cheon210608-test
                    connect(workerTriggerartist, SIGNAL(timeout()), this, SLOT(OnTimerCallbackFunc()));//cheon210608-test
                    connect(threadartist, SIGNAL(finished()), workerTriggerartist, SLOT(deleteLater()));//cheon210608-test
                    workerTriggerartist->start(400);//cheon210608-test
                    workerTriggerartist->moveToThread(threadartist);//cheon210608-test
                    //qDebug()  << "++++++++++++++++++++++++";
                    threadartist->start();//cheon210608-test
                }
                NreloadFlag = true;
                PreloadFlag = false;
                return;

            }else {
                flagReqMore = false;//cheon210607-test
                NreloadFlag = false;
                PreloadFlag = false;
            }

        }
        getDataFromDB();//cheon210608-test
        Etimerartist.restart();//cheon210608-test
      // ToastMsg::show(this, "", tr("next."));
      // ToastMsg::show(this, "", tr("next."),50);
      }
      if(groupMode==GroupMode::KIND
                || groupMode==GroupMode::COMPOSER
                || groupMode==GroupMode::VIEWALL_RECOMMENDARTIST){
            getDataFromDB();
      }
      if(groupMode==GroupMode::VIEWALL_PLAYALBUM){
          getDataFromDB();
      }

    }


#if defined(Q_OS_WINDOWS)//cheon210709-genre
    //qDebug() << "event->angleDelta()" << event->angleDelta();//cheon210709-genre
    if(event->angleDelta().ry() >= 120 && CNT%2 == 0){//cheon210709-genre
#endif        //cheon210709-genre
#if defined(Q_OS_MAC)//cheon210709-genre
    if(event->angleDelta().ry() >= 120 ){//cheon210709-genre
#endif     //cheon210709-genre
      if(groupMode==GroupMode::ARTIST){
        int nMilliseconds = Etimerartist.elapsed();
        qDebug() << "CNT = " << CNT;
        //qDebug() << "nMilliseconds = " << nMilliseconds;
        //qDebug() << "flagReqMore = " << flagReqMore;
        if(flagReqMore == true){
            if(nMilliseconds< global.mouse_time_term){//cheon210704-mouse
                //flagReqMore = true;//cheon210607-test
                if(!threadartist->isRunning()){
                    workerTriggerartist = new QTimer();//cheon210608-test
                    threadartist = new QThread();//cheon210608-test
                   // workerTrigger1->setInterval(300);//cheon210608-test
                    connect(workerTriggerartist, SIGNAL(timeout()), this, SLOT(OnTimerCallbackFunc()));//cheon210608-test
                    connect(threadartist, SIGNAL(finished()), workerTriggerartist, SLOT(deleteLater()));//cheon210608-test
                    workerTriggerartist->start(400);//cheon210608-test
                    workerTriggerartist->moveToThread(threadartist);//cheon210608-test
                    //qDebug()  << "++++++++++++++++++++++++";
                    threadartist->start();//cheon210608-test
                }
                NreloadFlag = false;
                PreloadFlag = true;
                return;

            }else {
                flagReqMore = false;//cheon210607-test
                NreloadFlag = false;
                PreloadFlag = false;
            }
        }
        getDataFromDB_pre();//cheon210608-test

        Etimerartist.restart();//cheon210608-test
       // ToastMsg::show(this, "", tr("pre."));
       // ToastMsg::show(this, "", tr("pre."),50);
      }
      if(groupMode==GroupMode::KIND
                || groupMode==GroupMode::COMPOSER
                || groupMode==GroupMode::VIEWALL_RECOMMENDARTIST){
            getDataFromDB_pre();
            //qDebug() << "0033----------------------------";
      }

    }
    //qDebug() << "event->angleDelta()" << event->angleDelta();



}

/**
 * @brief MusicList_Group::slot_myPlaylistChanged : [슬롯] My 플레이리스트 변경
 */
void MusicList_Group::slot_myPlaylistChanged(){
    setDataFromDB();
}
