#include "login/dialog_playlist_onRose.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/imagelabel.h"
#include "widget/toastmsg.h"
#include "widget/dialogconfirm.h"
#include "common/rosesettings.h"

#include <QWheelEvent>
#include <QSettings>


namespace Dialog {

    const int HTTP_MY_PLAYLIST = 0;
    const int HTTP_GET_PLAYLIST_DELETE = 11;

    int CNT = 0;

    Dialog_Playlist_onRose::Dialog_Playlist_onRose(Playlist_type playlistType, QJsonObject &jsonObj, QWidget *parent)
        : QDialog(parent)
        , playlist_type(playlistType)
    {
        this->jsonData_obj = new QJsonObject();
        this->jsonData_obj = &jsonObj;
        //QJsonDocument doc(*jsonData_obj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::Dialog_Playlist_onRose---jsonData_obj = " << strJson;

        this->jsonData_arr = new QJsonArray();

        this->list_playlist = new QList<roseHome::PlaylistItemData>();

        this->playlist_count = 0;
        this->total_playlist_count = 0;
        this->pageCNT = 0;
        this->playlist_beforeOffset = 0;
        this->playlist_currentOffset = 0;

        this->setInit();
        this->setUIControl();
    }


    void Dialog_Playlist_onRose::setInit(){

        this->setModal(true);
        this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setFixedSize(this->DLG_WIDTH, this->DLG_HEIGHT);
        this->setStyleSheet("background-color:transparent;");
    }


    void Dialog_Playlist_onRose::setUIControl(){

        this->widget_title = new QWidget();
        this->widget_title->setFixedSize(600, 70);
        this->widget_title->setObjectName("widget_title");
        this->widget_title->setStyleSheet("#widget_title {background-color:transparent;border-bottom:1px solid #404040;}");

        this->label_title = new QLabel(this->widget_title);
        this->label_title->setText(tr("Add to Playlist"));
        this->label_title->setStyleSheet("font:bold;color:#FFFFFF;font-size:22px;");
        this->label_title->setFixedHeight(70);

        int width = this->label_title->sizeHint().width();
        int left = (this->DLG_WIDTH - width) / 2;
        this->label_title->setGeometry(left, 0, width, 70);

        this->btn_close = GSCommon::getUIBtnImg("btn_close", ":images/icon_close_gray.png", this->widget_title);
        this->btn_close->setCursor(Qt::PointingHandCursor);
        this->btn_close->setFixedSize(QSize(70,70));
        this->btn_close->setGeometry(530, 0, 70, 70);


        this->dialog_creat = new QVBoxLayout();
        this->dialog_creat->setSpacing(0);
        this->dialog_creat->setContentsMargins(0,0,0,0);
        this->dialog_creat->setAlignment(Qt::AlignTop);


        this->dialog_playlist = new QVBoxLayout();
        this->dialog_playlist->setSpacing(0);
        this->dialog_playlist->setContentsMargins(0,0,0,0);
        this->dialog_playlist->setAlignment(Qt::AlignTop);

        this->widget_boby = new QWidget();
        this->widget_boby->setContentsMargins(0,0,0,0);
        this->widget_boby->setLayout(this->dialog_playlist);

        this->scrollArea = new QScrollArea();
        scrollArea->setWidget(this->widget_boby);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setStyleSheet("border:none;");


        this->dialog_total = new QVBoxLayout();
        this->dialog_total->setContentsMargins(0,0,0,0);
        this->dialog_total->setSpacing(0);
        this->dialog_total->setAlignment(Qt::AlignTop);
        this->dialog_total->addWidget(this->widget_title);
        this->dialog_total->addLayout(this->dialog_creat);
        this->dialog_total->addWidget(this->scrollArea);

        this->widget_total = new QWidget();
        this->widget_total->setContentsMargins(0,0,0,30);
        this->widget_total->setObjectName("widget_total");
        this->widget_total->setLayout(this->dialog_total);
        this->widget_total->setStyleSheet("#widget_total { background-color:#333333;border-radius:4px;border:1px solid #4D4D4D; }");

        this->dialog_layout = new QVBoxLayout();
        this->dialog_layout->setContentsMargins(0,0,0,0);
        this->dialog_layout->setSpacing(0);
        this->dialog_layout->addWidget(this->widget_total);

        this->setLayout(this->dialog_layout);

        this->setInit_playlist();
        // 커넥션
        connect(this->btn_close, SIGNAL(clicked()), this, SLOT(reject()));
    }


    void Dialog_Playlist_onRose::setInit_playlist(){

        GSCommon::clearLayout(this->dialog_playlist);
        this->appendPlayListUI(-1, tr("Create a New Playlist"), "", -1);//c02
    }


    void Dialog_Playlist_onRose::appendPlayListUI(int p_playlistNo, QString p_name, QString p_thumbPath, int p_numContents){

        QPushButton *btn_playlist = new QPushButton();
        btn_playlist->setCursor(Qt::PointingHandCursor);
        btn_playlist->setObjectName("wg_item");
        btn_playlist->setFixedSize(500, 80);
        btn_playlist->setProperty("playlistno", p_playlistNo);
        btn_playlist->setStyleSheet("#wg_item {border:none;border-bottom:1px solid #404040;margin-left:10px;margin-right:10px;} #wg_item:hover {border:none;border-bottom:1px solid #2A2A2A; background-color:#4D4D4D;margin-left:0px;margin-right:0px;}");

        ImageLabel *thumbnail = new ImageLabel(btn_playlist);
        thumbnail->setType(ImageLabel::Type::IconSmallRect);
        thumbnail->setFixedSize(60, 60);

        /*QLabel *lable_topListBG = GSCommon::getUILabelImg(":/images/playlist_bg_s.png", widget_playlist);
        lable_topListBG->resize(60, 10);
        lable_topListBG->setGeometry(10, 5, 60, 10);
        lable_topListBG->setVisible(false);*/

        if(!p_thumbPath.isEmpty()){
            //lable_topListBG->setVisible(true);
            thumbnail->setImageUrl(p_thumbPath);
            thumbnail->setGeometry(10, 10, 60, 60);
        }else if(p_playlistNo < 0){
            //lable_topListBG->setVisible(true);
            thumbnail->setImageResource(":/images/rosehome/playlist_add_ico.png");
            thumbnail->setGeometry(10, 10, 60, 60);
        }else{
            //lable_topListBG->setVisible(true);
            thumbnail->setImageResource(":/images/rosehome/playlist_def.png");
            thumbnail->setGeometry(10, 10, 60, 60);
        }

        QLabel *label_name = new QLabel(btn_playlist);
        label_name->setText(GSCommon::getTextCutFromLabelWidth(p_name, 440, label_name->font()));
        if(label_name->text().contains("…")){
            label_name->setToolTip(p_name);
            label_name->setToolTipDuration(2000);
        }
        label_name->setStyleSheet("color:#FFFFFF;font-size:16px;");

        QLabel *label_count = new QLabel(btn_playlist);
        if(p_numContents  <= 1){
            label_count->setText(QString("%1 ").arg(p_numContents) + tr("Song"));
        }
        else{
            label_count->setText(QString("%1 ").arg(p_numContents) + tr("Songs"));
        }

        label_count->setStyleSheet("color:#777777;font-size:16px;");

        if(p_playlistNo < 0){
            label_name->setGeometry(90, 31, 440, 18);
            label_count->hide();
        }
        else{
            label_name->setGeometry(90, 18, 440, 18);
            label_count->setGeometry(90, 45, 440, 18);
        }

        this->btn_playlistdel = new QPushButton();
        this->btn_playlistdel->setObjectName("btn_playlistdel");
        this->btn_playlistdel->setCursor(Qt::PointingHandCursor);
        this->btn_playlistdel->setProperty("playlistno", p_playlistNo);
        this->btn_playlistdel->setFixedSize(70,50);            //45, 50
        this->btn_playlistdel->setGeometry(550, 0, 0, 0);
        //this->btn_playlistdel->setStyleSheet("background-color:transparent;");
        this->btn_playlistdel->setStyleSheet("#btn_playlistdel {border:none;border-bottom:1px solid #404040;margin-left:10px;margin-right:10px;} #btn_playlistdel:hover {border:none;border-bottom:1px solid #2A2A2A; background-color:#4D4D4D;margin-left:0px;margin-right:0px;}");

        //this->btn_del->hide();

        connect(this->btn_playlistdel, &QPushButton::clicked, this, &Dialog_Playlist_onRose::slot_clickedPlaylistDelete);

        this->lb_playlistdel_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", btn_playlistdel);
        this->lb_playlistdel_icon->setFixedSize(50, 50);
        this->lb_playlistdel_icon->setGeometry(0, 0, 0, 0);

        QHBoxLayout *hl_lb_del_image = new QHBoxLayout();
        hl_lb_del_image->setContentsMargins(0, 0, 0, 0);
        hl_lb_del_image->setSpacing(0);
        hl_lb_del_image->addWidget(btn_playlist);
        //hl_lb_del_image->addWidget(this->lb_playlistdel_icon);
        hl_lb_del_image->addWidget(this->btn_playlistdel);

        if(p_playlistNo < 0){
            this->dialog_creat->addWidget(btn_playlist);
        }
        else{
            //this->dialog_playlist->addWidget(btn_playlist);
            this->dialog_playlist->addLayout(hl_lb_del_image);
        }

        // 커넥트
        connect(btn_playlist, SIGNAL(clicked()), this, SLOT(slot_clicked_playlist()));
    }
/*
QWidget* RoseHomeFriend::getUIScrollViewWidget(QWidget *p_widget){

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setObjectName("scrollArea_in");
    scrollArea->setWidget(p_widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("#scrollArea_in { border:0px; }");

    QVBoxLayout *vl_scroll = new QVBoxLayout();
    vl_scroll->setContentsMargins(0,0,0,0);
    vl_scroll->setSpacing(0);
    vl_scroll->addWidget(scrollArea);
    QWidget *widget_scrollArea = new QWidget();
    widget_scrollArea->setLayout(vl_scroll);

    return widget_scrollArea;
}
*/
    bool Dialog_Playlist_onRose::playList_delDialogConfirm()
    {
        DialogConfirm *dlg = new DialogConfirm(this);

        QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
        int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
        int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
        int left = global.left_mainwindow + ((latestWidth - dlg->sizeHint().width()) / 2);
        int top = global.top_mainwindow + ((latestHeight - dlg->sizeHint().height()) / 2);


        /*left = latestWidth - (dlg->width());
        top = (latestHeight/ 2) - (dlg->sizeHint().height() / 3);
        print_debug();
        qDebug() << "dlg->width() = " << dlg->width();
        qDebug() << "left = " << left;*/
        dlg->setGeometry(left, top, 0, 0);

        dlg->setFixedHeight(560);//c220521
        dlg->setTitle(tr("PlayList track Delete Notice"));
        //dlg->setGeometry(this->width()-400,300,1000,200);
        //dlg->yellow_setStyleSheet();
        dlg->setText(tr("Are you sure you want to delete your selected playlist's track?"));


        int result = dlg->exec();

        if(result==QDialog::Accepted){
            return result;
        }
        return result;
        delete dlg;
    }

    void Dialog_Playlist_onRose::slot_clickedPlaylistDelete(){//c02
        print_debug();
        int tmp_playlistNo = sender()->property("playlistno").toInt();
        qDebug() << "tmp_playlistNo = " << tmp_playlistNo;
        QJsonObject json;
        QString playlistID = QString("%1").arg(tmp_playlistNo);//this->lb_userid->text();

        if(this->playList_delDialogConfirm()){
            roseHome::PlaylistItemData tmp_data2;
            //for(int i = 0 ; i < this->list_playlist->size(); i++){
             //   tmp_data2 = this->list_playlist->at(i);
                //qDebug() << "tmp_data2.id = " << tmp_data2.id;
                //qDebug() << "tmp_data2.trackId = " << tmp_data2.trackId;
            //}


            roseHome::PlaylistItemData tmp_data;
            tmp_data = this->list_playlist->at(tmp_playlistNo);


            //tmp_data.title = ProcJsonEasy::getString(tmp_json, "title");
            //json.insert("playlistId", tmp_data.id);
            //json.insert("playlistId", playlistID);

            QUrlQuery params;


            //json.insert("page", 0);
            //json.insert("size", 20);
            //params.addQueryItem("playlistId", QString("%1").arg(tmp_data.id));
            //params.addQueryItem("size", "20");
            //----------------------------------------------------------
            QJsonDocument doc(json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::setResult_of_fetch--json-" << strJson;

            NetworkHttp *network = new NetworkHttp();
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            //network->request(HTTP_GET_PLAYLIST_DELETE
            //                 , QString("%1/member/playlist?playlistId=%2").arg(global.legacy_dev_v1).arg(tmp_data.id)
            //                 , json
            //                 , true
            //                 , true);

            network->requestDelete(HTTP_GET_PLAYLIST_DELETE, QString("%1/member/playlist?playlistId=%2").arg(global.legacy_v1).arg(tmp_data.id), json, true);
        }



    }

   /*
    void Dialog_Playlist_onRose::slot_clickedPlaylistDelete(){//c02
        print_debug();
        int tmp_playlistNo = sender()->property("playlistno").toInt();
        qDebug() << "tmp_playlistNo = " << tmp_playlistNo;
        QJsonObject json;
        QString playlistID = QString("%1").arg(tmp_playlistNo);//this->lb_userid->text();



        roseHome::PlaylistItemData tmp_data;
        tmp_data = this->list_playlist->at(tmp_playlistNo);

        QJsonObject json_tracks_data;
        json_tracks_data.insert("macAddress", tmp_data.macAddress);

        QJsonArray jsonarry_tracks;
        QJsonObject json_tracks;
        json_tracks.insert("id", tmp_data.trackId);
        json_tracks.insert("type", tmp_data.type);
        json_tracks.insert("favorite", tmp_data.favorite);
        //json_tracks.insert("playUrl", tmp_data.thumbup);
        json_tracks.insert("clientKey", tmp_data.clientKey);
        json_tracks.insert("data", json_tracks_data);
        //

        jsonarry_tracks.append(json_tracks);

        json.insert("id", tmp_data.id);
        json.insert("favoriteTrack", tmp_data.favoriteTrack);
        json.insert("favorite", tmp_data.favorite);
        json.insert("thumbup", tmp_data.thumbup);
        json.insert("tracks", jsonarry_tracks);
        json.insert("isRose", tmp_data.isRose);

        //----------------------------------------------------------
        QJsonDocument doc(json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::setResult_of_fetch--json-" << strJson;

        NetworkHttp *network = new NetworkHttp();
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_GET_PLAYLIST_DELETE
                         , QString("%1/member/playlist/track").arg(global.legacy_v1)
                         , json
                         , true
                         , true);
        //network->requestDelete(HTTP_GET_PLAYLIST_DELETE, QString("%1/member/playlis/track").arg(global.legacy_v1), json, true);
    }
    */
    void Dialog_Playlist_onRose::request_playlist_fetch(){

        // clear
        GSCommon::clearLayout(this->dialog_playlist);
        this->pageCNT = 0;
        qDebug() << "this->pageCNT= " << this->pageCNT;
        QJsonObject json;

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        network->request(HTTP_MY_PLAYLIST
                         , QString("%1/member/playlist/member/%2?sortType=PLAYLIST_RECENT&mediaType=&page=%3&size=20").arg(global.legacy_v1).arg(global.user.getSub()).arg(this->pageCNT)
                         , json
                         , false
                         , true);

    }


    void Dialog_Playlist_onRose::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
        print_debug();
        //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::slot_responseHttp---" << strJson;

        if(p_id == HTTP_MY_PLAYLIST){
            this->setResult_of_fetch(p_jsonObject);
        }
        if(p_id == HTTP_GET_PLAYLIST_DELETE){
            print_debug();
            const QString jsonKey_flagOk = "flagOk";

            if(p_jsonObject.contains(jsonKey_flagOk) && p_jsonObject[jsonKey_flagOk].toBool()){
                QDialog::reject();
                ToastMsg::show(this, "", tr("The selected playlist has been deleted."));

            }else{
                ToastMsg::show(this, "", tr("Failed to delete the selected playlist."));
            }

            QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::slot_responseHttp---" << strJson;

        }
    }


    void Dialog_Playlist_onRose::setResult_of_fetch(const QJsonObject &p_jsonObject){
        print_debug();
        //QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::setResult_of_fetch---" << strJson;

        if(ProcJsonEasy::get_flagOk(p_jsonObject) == true){

            this->total_playlist_count = ProcJsonEasy::getInt(p_jsonObject, "totalCount");
            QJsonArray jsonArrSub = ProcJsonEasy::getJsonArray(p_jsonObject, "playlists");
            ProcJsonEasy::mergeJsonArray(*this->jsonData_arr, jsonArrSub);

            if(jsonArrSub.size() > 0){
                for(int i = 0; i < jsonArrSub.count(); i++){
                    QJsonObject tmp_json = jsonArrSub.at(i).toObject();

                    roseHome::PlaylistItemData tmp_data;

                    tmp_data.id = ProcJsonEasy::getInt(tmp_json, "id");
                    tmp_data.ownerId =ProcJsonEasy::getInt(tmp_json, "ownerId");
                    tmp_data.sort = ProcJsonEasy::getInt(tmp_json, "sort");
                    tmp_data.star = ProcJsonEasy::getInt(tmp_json, "star");
                    tmp_data.thumbupCount = ProcJsonEasy::getInt(tmp_json, "thumbupCount");
                    tmp_data.trackCount = ProcJsonEasy::getInt(tmp_json, "trackCount");

                    tmp_data.clientKey = ProcJsonEasy::getString(tmp_json, "clientKey");
                    tmp_data.comment = ProcJsonEasy::getString(tmp_json, "comment");
                    tmp_data.ownerName = ProcJsonEasy::getString(tmp_json, "ownerName");
                    tmp_data.registDateTime = ProcJsonEasy::getString(tmp_json, "registDateTime");
                    tmp_data.share = ProcJsonEasy::getString(tmp_json, "share");
                    tmp_data.thumbnail = ProcJsonEasy::getString(tmp_json, "thumbnail");
                    tmp_data.title = ProcJsonEasy::getString(tmp_json, "title");
                    tmp_data.type = ProcJsonEasy::getString(tmp_json, "type");
                    tmp_data.playTime = ProcJsonEasy::getString(tmp_json, "playTime");

                    tmp_data.favorite = ProcJsonEasy::getBool(tmp_json, "favorite");
                    tmp_data.favoriteTrack = ProcJsonEasy::getBool(tmp_json, "favoriteTrack");
                    tmp_data.isRose = ProcJsonEasy::getBool(tmp_json, "isRose");
                    tmp_data.thumbup = ProcJsonEasy::getBool(tmp_json, "thumbup");

                    QJsonObject data_json = ProcJsonEasy::getJsonObject(tmp_json, "data");

                    tmp_data.trackId = ProcJsonEasy::getInt(data_json, "trackId");
                    tmp_data.macAddress = ProcJsonEasy::getString(data_json, "macAddress");

                    QJsonArray tags_jsonArr = ProcJsonEasy::getJsonArray(tmp_json, "tags");

                    for(int i = 0; i < tags_jsonArr.count(); i++){
                        tmp_data.tags.append(tags_jsonArr.at(i).toString());
                    }

                    tmp_data.totalCount = ProcJsonEasy::getInt(p_jsonObject, "totalCount");

                    this->list_playlist->append(tmp_data);

                    this->appendPlayListUI((pageCNT * 20) + i, tmp_data.title, tmp_data.thumbnail, tmp_data.trackCount);
                }
            }

            this->playlist_count = this->list_playlist->count();
            this->playlist_beforeOffset = this->playlist_currentOffset;
        }
    }


    void Dialog_Playlist_onRose::slot_clicked_playlist(){
        int tmp_playlistNo = sender()->property("playlistno").toInt();

        this->hide();

        QJsonObject playlistInfo = QJsonObject();
        playlistInfo = *this->jsonData_obj;

        if(tmp_playlistNo >= 0){
            playlistInfo.insert("playlistInfo", this->jsonData_arr->at(tmp_playlistNo).toObject());
        }

        emit signal_clicked_playlist(tmp_playlistNo, playlistInfo);

        this->accept();
    }


    void Dialog_Playlist_onRose::wheelEvent(QWheelEvent* event){
        event->accept();
        // 더 가져오기 실행

#if defined(Q_OS_WINDOWS)
        CNT++;
        if(CNT > 10000) CNT = 1;
        if(event->angleDelta().ry() <= -120 && CNT%2 == 0){
            // Widget의 아래쪽으로 경계에서 Wheel 될때. - for Geting More Data
            this->request_more_playlistData();

        }
#elif defined(Q_OS_MAC)//cheon210703-list
        if(event->angleDelta().ry() <= -120 ){
            this->request_more_playlistData();
        }
#endif
    }


    void Dialog_Playlist_onRose::request_more_playlistData(){
print_debug();
        if((this->playlist_count < this->total_playlist_count) && (this->playlist_beforeOffset == this->playlist_currentOffset)){
            this->pageCNT++;
            this->playlist_currentOffset = this->pageCNT;

            QJsonObject json;

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_MY_PLAYLIST
                             , QString("%1/member/playlist/member/%2?sortType=PLAYLIST_RECENT&mediaType=&page=%3&size=20").arg(global.legacy_v1).arg(global.user.getSub()).arg(this->pageCNT)
                             , json
                             , false
                             , true);
        }
    }
}
