#include "widget/AbstractAddPlaylist_RHV.h"

#include "widget/VerticalScrollArea.h"
#include "widget/toastmsg.h"

#include "common/gscommon.h"
#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRegularExpression>
#include <QResizeEvent>
#include <QScroller>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>
#include "common/rosesettings.h"




const QString IMAGE_PATH_DEFAULT = ":/images/rosehome/thum_edit.png";

const QString ALBTAB_STEP_ADDED = "added";
const QString ALBTAB_STEP_EXISTING = "existing";

const int HTTP_NETWORK_PLAYLIST_FETCH = 0;


AbstractAddPlaylist_RHV::AbstractAddPlaylist_RHV(ContentsType contents_type, ContentsUIType contents_ui_type, QWidget *parent)
    : QWidget(parent)
    , contents_type(contents_type)
    , contents_ui_type(contents_ui_type)
{

    linker = Linker::getInstance();

    this->fileDownLoader = new FileDownloader(this);
    connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractAddPlaylist_RHV::slot_fileDownload_loadImage);

    this->tagTypeList << tr("#Classical") << tr("#Pop") << tr("#K-Pop") << tr("#Hip-Hop") << tr("#Jazz") << tr("#Dance") << tr("#R&B")
                           << tr("#Latin") << tr("#Blues") << tr("#Soul") << tr("#Country") << tr("#Indie") << tr("#Rock");

    this->flag_addTracks_qobuz = false;
    this->flag_addTracks_rose = false;
    this->flag_all_enable = false;
    this->flag_freind_enable = false;
    this->flag_private_enable = false;
    this->flag_stream_enable = false;
}


AbstractAddPlaylist_RHV::~AbstractAddPlaylist_RHV(){

    this->deleteLater();
}


void AbstractAddPlaylist_RHV::setData_fromJsonObject(const QJsonObject &jsonObj){
print_debug();
    str_added = tr("To be added");
    str_exsting = tr("Existing");

    if(jsonObj.contains("tracks")){
        this->added_array = QJsonArray();
        this->added_array = ProcJsonEasy::getJsonArray(jsonObj, "tracks");

        str_added += QString(" (%1)").arg(this->added_array.size());

        for(int i = 0; i < this->added_array.size(); i++){
            this->added_playlist[i] = new PlaylistTrackDetailInfo_RHV();
            //connect(this->added_playlist[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &AbstractAddPlaylist_RHV::slot_clickedItemTrack_inList);
            this->added_playlist[i]->setProperty("index", i);
        }
    }
    else{
        str_added += " (0)";
    }

    if(jsonObj.contains("playlistInfo")){
        this->playlist_info = QJsonObject();
        this->playlist_info = ProcJsonEasy::getJsonObject(jsonObj, "playlistInfo");

        this->track_count = ProcJsonEasy::getInt(this->playlist_info, "trackCount");
        this->playlist_id = ProcJsonEasy::getInt(this->playlist_info, "id");
        this->shared_type = ProcJsonEasy::getString(this->playlist_info, "share");

        str_exsting += QString(" (%1)").arg(this->track_count);

        this->create_count = 0;
        if(this->track_count > 60){
            this->create_count = 60;
        }
        else{
            this->create_count = this->track_count;
        }

        for(int i = 0; i < this->create_count; i++){
            this->existing_playlist[i] = new PlaylistTrackDetailInfo_RHV();
            //connect(this->existing_playlist[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &AbstractAddPlaylist_RHV::slot_clickedItemTrack_inList);
            this->existing_playlist[i]->setProperty("index", i);
        }

    }
    else{
        str_exsting += " (0)";
    }

    this->wg_menubar = new QWidget();
    this->wg_menubar->setFixedSize(1500, 50);

    QWidget *wg_line = new QWidget(this->wg_menubar);
    wg_line->setFixedSize(1500, 1);
    wg_line->setGeometry(0, 49, 0, 0);
    wg_line->setStyleSheet("background-color:#333333");

    QJsonArray *p_jsonArray_titlSub = new QJsonArray();
    QJsonObject sub1 { {"name", str_added}, {"code", ALBTAB_STEP_ADDED} };
    QJsonObject sub2 { {"name", str_exsting}, {"code", ALBTAB_STEP_EXISTING} };

    p_jsonArray_titlSub->push_back(sub1);
    p_jsonArray_titlSub->push_back(sub2);

    this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail, this->wg_menubar);
    this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

    connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));

    this->contentStep = "added";
    this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

    this->btn_withStream = new QPushButton(this->wg_menubar);
    this->btn_withStream->setStyleSheet("background-color:transparent;");
    this->btn_withStream->setCursor(Qt::PointingHandCursor);
    this->btn_withStream->setFixedSize(220, 50);//c220511
    this->btn_withStream->setGeometry(1030, 0, 0, 0);
    this->btn_withStream->hide();

    connect(this->btn_withStream, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnStream);



    this->btn_del = new QPushButton(this->wg_menubar);
    this->btn_del->setObjectName("btn_del");
    this->btn_del->setCursor(Qt::PointingHandCursor);
    this->btn_del->setFixedSize(270,50);            //45, 50
    this->btn_del->setGeometry(850, 0, 0, 0);
    this->btn_del->setStyleSheet("background-color:transparent;");
    //this->btn_del->hide();

    connect(this->btn_del, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clickedDelete);

    this->lb_del_icon = GSCommon::getUILabelImg(":/images/q_del_ico.png", this->btn_del);
    this->lb_del_icon->setFixedSize(50, 50);
    this->lb_del_icon->setGeometry(0, 0, 0, 0);

    this->lb_del = new QLabel(this->btn_del);
    this->lb_del->setFixedSize(200, 26);
    this->lb_del->setGeometry(50, 12, 0, 0);
    this->lb_del->setText(tr("Delete Tracks"));
    this->lb_del->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->lb_del->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    this->lb_listCheck_icon = GSCommon::getUILabelImg(":/images/rosehome/list_check_off.png", this->btn_withStream);
    this->lb_listCheck_icon->setFixedSize(50, 50);
    this->lb_listCheck_icon->setGeometry(0, 0, 0, 0);

    this->lb_listCheck = new QLabel(this->btn_withStream);
    this->lb_listCheck->setFixedSize(170, 26);//c220511
    this->lb_listCheck->setGeometry(50, 12, 0, 0);
    this->lb_listCheck->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->lb_listCheck->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    this->btn_selectList = new QPushButton(this->wg_menubar);
    this->btn_selectList->setStyleSheet("background-color:transparent;");
    this->btn_selectList->setCursor(Qt::PointingHandCursor);
    this->btn_selectList->setFixedSize(250, 50);
    this->btn_selectList->setGeometry(1250, 0, 0, 0);
    this->btn_selectList->hide();


    connect(this->btn_selectList, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnSelect);

    this->lb_selectList_icon = GSCommon::getUILabelImg(":/images/rosehome/playlist_ico.png", this->btn_selectList);
    this->lb_selectList_icon->setFixedSize(50, 50);
    this->lb_selectList_icon->setGeometry(0, 0, 0, 0);

    this->lb_selectList = new QLabel(this->btn_selectList);
    this->lb_selectList->setFixedSize(200, 26);
    this->lb_selectList->setGeometry(50, 12, 0, 0);
    this->lb_selectList->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
    this->lb_selectList->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    if(this->contents_ui_type == ContentsUIType::Creating){
        print_debug();
        this->setUIControl_Creating();
    }
    else if(this->contents_ui_type == ContentsUIType::Adding){
        print_debug();
        this->setUIControl_Adding();
    }
    else if(this->contents_ui_type == ContentsUIType::Editing){
        print_debug();
        this->setUIControl_Editing();
    }
}

void AbstractAddPlaylist_RHV::slot_clicked_btnConfirm(){

    if(this->le_title->text().replace(" ", "").isEmpty()){
        ToastMsg::show(this, "", tr("Please enter the title."));
    }
    else{

        QString type = "";

        if(contents_type == ContentsType::Qobuz){
            print_debug();
            if(this->flag_stream_enable == true){
                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                connect(procQobuz, &qobuz::ProcCommon::completeReq_addTracks_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_addTracks_qobuzPlaylist);
                procQobuz->request_qobuz_addTracks_myPlaylist(this->selected_playlist_id, this->selected_track_ids);
            }

            type = "QOBUZ";
        }
        else if(contents_type == ContentsType::Tidal){
            print_debug();
            if(this->flag_stream_enable == true){
                tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
                connect(procTidal, &tidal::ProcCommon::completeReq_addTracks_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_addTracks_tidalPlaylist);
                procTidal->request_tidal_addTracks_myPlaylist(this->selected_playlist_id, this->selected_track_ids);
            }

            type = "TIDAL";
        }

        if(contents_ui_type == ContentsUIType::Creating){
            print_debug();
            QString clientKey = QString("%1").arg(QDateTime::currentMSecsSinceEpoch());

            QJsonArray tags;
            QString str_tags = this->le_tag->text().replace("#", "").replace(" ", "");

            QStringList strList = str_tags.split(",");
            for(int i = 0 ; i < strList.count(); i++){
                tags.append(strList.at(i));
            }

            QString share = "PRIVATE";
            if(this->flag_all_enable == true){
                share = "ALL";
            }
            else if(this->flag_freind_enable == true){
                share = "FRIEND";
            }
            else if(this->flag_private_enable == true){
                share = "PRIVATE";
            }

            QJsonObject json;
            json.insert("clientKey", clientKey);
            json.insert("title", this->le_title->text());
            json.insert("comment", this->te_content->toPlainText());
            json.insert("type", type);
            json.insert("tags", tags);
            json.insert("share", share);
            //----------------------------------------
            for(int i = 0; i < this->added_array.size(); i++){
                print_debug();
                qDebug() << listWidget_add->item(i)->text().toInt();
                this->intList_editPositon.append(listWidget_add->item(i)->text().toInt());
            }
            //----------------------------------------

            roseHome::ProcCommon *procRose = new roseHome::ProcCommon(this);
            connect(procRose, &roseHome::ProcCommon::completeReq_create_playlist, this, &AbstractAddPlaylist_RHV::slot_create_rosePlaylist);
            procRose->request_rose_create_myPlaylist(json);
        }
        else if(contents_ui_type == ContentsUIType::Adding){
            print_debug();
            //----------------------------------------
            for(int i = 0; i < this->added_array.size(); i++){
                print_debug();
                qDebug() << listWidget_add->item(i)->text().toInt();
                this->intList_editPositon.append(listWidget_add->item(i)->text().toInt());
            }
            //----------------------------------------


            QJsonObject json;
            json.insert("id", this->playlist_id);
            this->slot_create_rosePlaylist(json);
        }
        else if(contents_ui_type == ContentsUIType::Editing){
            
        }
    }
}

void AbstractAddPlaylist_RHV::slot_clickedDelete(){//1126
    print_debug();

//existing_array
    if(contentStep == ALBTAB_STEP_ADDED){
        if(intlist_selectPositon.count() > 0){
            if(this->playList_delDialogConfirm()){
                print_debug();
                QJsonArray tmp_added_array;
                int added_cnt = this->added_array.size();
                for(int i = 0 ; i < added_cnt ; i++){
                    tmp_added_array.append(this->added_array.at(i));
                //    qDebug() << "this->added_playlist[i] = " << i << " " << this->added_playlist[i].id;
                }

                for(int i = intlist_selectPositon.size()-1 ; i >= 0 ; i--){
                    qDebug() << "intlist_selectPositon.at(i) = " << intlist_selectPositon.at(i);
                    delete this->listWidget_add->takeItem(intlist_selectPositon.at(i));
                    tmp_added_array.removeAt(intlist_selectPositon.at(i));
                }
                this->added_array = QJsonArray();
                for(int i = 0 ; i < tmp_added_array.size() ; i++){
                    print_debug();
                    qDebug() << "delete i = " << i;
                    this->added_array.append(tmp_added_array.at(i));
                }

                intlist_selectPositon.clear();
            }
        }else{
            emit linker->signal_checkQueue(15, "");
        }
    }
    if(contentStep == ALBTAB_STEP_EXISTING){
        if(intlist_selectPositon.count() > 0){
            if(this->playList_delDialogConfirm()){
                print_debug();
                QJsonArray tmp_added_array;
                int added_cnt = this->existing_array.size();
                for(int i = 0 ; i < added_cnt ; i++){
                    tmp_added_array.append(this->existing_array.at(i));
                }

                for(int i = intlist_selectPositon.size()-1 ; i >= 0 ; i--){
                    qDebug() << "intlist_selectPositon.at(i) = " << intlist_selectPositon.at(i);
                    delete this->listWidget_exist->takeItem(intlist_selectPositon.at(i));
                    tmp_added_array.removeAt(intlist_selectPositon.at(i));
                }
                this->existing_array = QJsonArray();
                for(int i = 0 ; i < tmp_added_array.size() ; i++){
                    print_debug();
                    qDebug() << "delete i = " << i;
                    this->existing_array.append(tmp_added_array.at(i));
                }

                intlist_selectPositon.clear();
            }
        }else{
            emit linker->signal_checkQueue(15, "");
        }
    }

}


void AbstractAddPlaylist_RHV::slot_clicked_btnSelect(){

    if(this->flag_stream_enable == true){

        if(contents_type == ContentsType::Qobuz){
            qobuz::Dialog_ChoosePlaylist_forQobuz *dialog_chooslPlaylist = new qobuz::Dialog_ChoosePlaylist_forQobuz(this);
            connect(dialog_chooslPlaylist, &qobuz::Dialog_ChoosePlaylist_forQobuz::signal_choosePlaylist_forQOBUZ, this, &AbstractAddPlaylist_RHV::slot_selected_playlist);

            int result = dialog_chooslPlaylist->exec();

            if(result == QDialog::Accepted){
                delete dialog_chooslPlaylist;
            }
        }
        else if(contents_type == ContentsType::Tidal){
            tidal::Dialog_ChoosePlaylist_forTidal *dialog_chooslPlaylist = new tidal::Dialog_ChoosePlaylist_forTidal(this);
            connect(dialog_chooslPlaylist, &tidal::Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forTIDAL, this, &AbstractAddPlaylist_RHV::slot_selected_playlist);

            int result = dialog_chooslPlaylist->exec();

            if(result == QDialog::Accepted){
                delete dialog_chooslPlaylist;
            }
        }
    }
}


void AbstractAddPlaylist_RHV::slot_selected_playlist(QString playlist_id_choose, QString playlist_title, QString playlist_description){

    if(playlist_id_choose != 0){
        this->selected_playlist_id = playlist_id_choose;
        this->selected_playlist_name = playlist_title;
        this->selected_playlist_description = playlist_description;

        if(!this->selected_playlist_name.isEmpty()){
            QLabel *tmp_name = new QLabel();
            tmp_name->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            tmp_name->setText(this->selected_playlist_name);

            if(tmp_name->geometry().width() > 190){
                this->lb_selectList->setText(GSCommon::getTextCutFromLabelWidth(this->selected_playlist_name, 180, this->lb_selectList->font()));
            }
            else{
                this->lb_selectList->setText(this->selected_playlist_name);
            }
        }
    }
    else{
        this->selected_playlist_name = playlist_title;
        this->selected_playlist_description = playlist_description;

        if(!this->selected_playlist_name.isEmpty()){
            QLabel *tmp_name = new QLabel();
            tmp_name->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            tmp_name->setText(this->selected_playlist_name);

            if(tmp_name->geometry().width() > 190){
                this->lb_selectList->setText(GSCommon::getTextCutFromLabelWidth(this->selected_playlist_name, 180, this->lb_selectList->font()));
            }
            else{
                this->lb_selectList->setText(this->selected_playlist_name);
            }

            if(contents_type == ContentsType::Qobuz){

                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                connect(procQobuz, &qobuz::ProcCommon::completeReq_create_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_create_qobuzPlaylist);
                procQobuz->request_qobuz_create_myPlaylist(this->selected_playlist_name, this->selected_playlist_description);
            }
            else if(contents_type == ContentsType::Tidal){

                tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
                connect(procTidal, &tidal::ProcCommon::completeReq_create_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_create_tidalPlaylist);
                procTidal->request_tidal_create_myPlaylist(global.user_forTidal.getUserId(), this->selected_playlist_name);
            }
        }
    }
}


bool AbstractAddPlaylist_RHV::playList_delDialogConfirm()
{
    DialogConfirm *dlg = new DialogConfirm(this);

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
    int left = 0;
    int top = 0;


    left = latestWidth - (dlg->width());
    top = (latestHeight/ 2) - (dlg->sizeHint().height() / 2);
    print_debug();
    qDebug() << "dlg->width() = " << dlg->width();
    qDebug() << "left = " << left;
    dlg->setGeometry(left-50, top, 0, 0);

    dlg->setFixedHeight(550);//c220521
    dlg->setTitle(tr("PlayList Delete Notice"));
    //dlg->setGeometry(this->width()-400,300,1000,200);
    //dlg->yellow_setStyleSheet();
    dlg->setText(tr("Are you sure you want to delete your selected playlist's tracks?"));


    int result = dlg->exec();

    if(result==QDialog::Accepted){
        return result;
    }
    return result;
    delete dlg;
}

bool AbstractAddPlaylist_RHV::playList_selectDialogConfirm()
{
    DialogConfirm *dlg = new DialogConfirm(this);

    QSettings *settings = new QSettings(rosesettings.ORGANIZATION_NAME, rosesettings.APPLICATION_NAME);
    int latestWidth = settings->value(rosesettings.SETTINGS_W, 0).toInt();
    int latestHeight = settings->value(rosesettings.SETTINGS_H, 800).toInt();
    int left = 0;
    int top = 0;


    left = latestWidth - (dlg->width());
    top = (latestHeight/ 2) - (dlg->sizeHint().height() / 2);
    print_debug();
    qDebug() << "dlg->width() = " << dlg->width();
    qDebug() << "left = " << left;
    dlg->setGeometry(left-50, top, 0, 0);


    dlg->setTitle(tr("PlayList select Notice"));
    //dlg->setGeometry(this->width()-400,300,1000,200);
    //dlg->yellow_setStyleSheet();
    dlg->setText(tr("Are you sure you want to creating/adding your selected playlist?"));


    int result = dlg->exec();

    if(result==QDialog::Accepted){
        return result;
    }
    return result;
    delete dlg;
}

void AbstractAddPlaylist_RHV::setActivePage(){

    if(this->added_array.size() > 0){

        if(this->added_array.size() < 4){
            int height = 210;

            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(height);
        }
        else{
            int height = 70 * this->added_array.size();

            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(height);
        }

        this->listWidget_add->setStyleSheet("#QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant
        global.isMoveModePlaylist = true;
        for(int i = 0; i < this->added_array.size(); i++){
            QJsonObject convertData = this->added_array.at(i).toObject();
            QJsonDocument doc(convertData);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " Dialog_Playlist_onRose::setActivePage--convertData=" << strJson;

            QJsonObject tmpData = QJsonObject();

            if(contents_type == ContentsType::Qobuz){

                this->selected_track_ids += QString("%1,").arg(ProcJsonEasy::getInt(convertData, "id"));

                QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");
                QJsonObject tmpImage = ProcJsonEasy::getJsonObject(tmpAlbum, "image");
                tmpData.insert("thumbnail", ProcJsonEasy::getString(tmpImage, "thumbnail"));

                if(i == 0){
                    this->setImage(ProcJsonEasy::getString(tmpImage, "large"));
                }

                QString artist = "";
                if(convertData.contains("artist")){
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                    QList<QVariant> list_artist = ProcJsonEasy::getJsonArray(tmpArtist, "list_artist_name").toVariantList();

                    for(int i = 0; i < list_artist.size(); i++){
                        if(i == 0){
                            artist = list_artist.at(i).toString();
                        }
                        else{
                            artist += "," + list_artist.at(i).toString();
                        }
                    }
                }
                else{
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(tmpAlbum, "artist");
                    artist = ProcJsonEasy::getString(tmpArtist, "name");
                }

                tmpData.insert("type", "QOBUZ");
                tmpData.insert("title", ProcJsonEasy::getString(convertData, "title"));
                tmpData.insert("artist", artist);
                tmpData.insert("duration", ProcJsonEasy::getInt(convertData, "duration"));
                tmpData.insert("hires", ProcJsonEasy::getBool(convertData, "hires"));
                tmpData.insert("maximum_sampling_rate", ProcJsonEasy::getDouble(convertData, "maximum_sampling_rate"));

                this->vtrack_id = ProcJsonEasy::getVariant(convertData, "id");

                this->added_playlist[i]->setDataTrackInfo_RoseListEdit(tmpData);
                //this->vl_Added->addWidget(this->added_playlist[i]);

                //-------listwidgetconvert------------------------------------------------


                QListWidgetItem *item = new QListWidgetItem();
                item->setBackground(QBrush("#333333"));
                item->setText("   "+ QString::number(i));
                //DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate.at(i);
                //item->setText("    "+ QString::number(i)+ " " + this->list_dataTrackForDelegate.at(i).title);
                item->setTextColor(QColor::fromRgb(33,33,33));
                item->setFont(QFont("Arial", 8));
                item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
    #if defined(Q_OS_WINDOWS)
                item->setSizeHint(QSize(item->sizeHint().width(), 70));
                //item->setSizeHint(QSize(90, 70));
    #endif
    #if defined(Q_OS_MAC)
                item->setSizeHint(QSize(90, 70));
    #endif

                this->listWidget_add->addItem(item);

                QWidget *line_bottom_1 = new QWidget();
                line_bottom_1->setFixedHeight(70);
                line_bottom_1->setStyleSheet("background-color:#707070;");


                //this->listWidget_add->setItemWidget(item,this->added_playlist[i]);
                this->setDataTrackInfo_RoseListEdit_Item(tmpData,i);
                this->listWidget_add->setItemWidget(item,widget_list_added.at(i));

                //--------------------------------------------------------

                QCoreApplication::processEvents();
            }
            else if(contents_type == ContentsType::Tidal){

                this->selected_track_ids += QString("%1,").arg(ProcJsonEasy::getInt(convertData, "id"));

                QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(convertData, "album");

                QString imagePath = ProcJsonEasy::getString(tmpAlbum, "cover");
                QString thumbnail = "";

                if(imagePath.isEmpty()){
                    thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(convertData, "imageId"), 750);

                }
                else{
                    thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(tmpAlbum, "cover"), 750);
                }

                if(i == 0){
                    this->setImage(thumbnail);
                }


                QString artist = "";
                if(convertData.contains("artist")){
                    QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(convertData, "artist");
                    artist = ProcJsonEasy::getString(tmpArtist, "name");
                }

                tmpData.insert("thumbnail", thumbnail);
                tmpData.insert("title", ProcJsonEasy::getString(convertData, "title"));
                tmpData.insert("artist", artist);
                tmpData.insert("duration", ProcJsonEasy::getInt(convertData, "duration"));
                tmpData.insert("hires", ProcJsonEasy::getBool(convertData, "audioQuality"));
                tmpData.insert("maximum_sampling_rate", 0);
                tmpData.insert("type", "TIDAL");

                this->vtrack_id = ProcJsonEasy::getVariant(convertData, "id");

                this->added_playlist[i]->setDataTrackInfo_RoseListEdit(tmpData);
                //this->vl_Added->addWidget(this->added_playlist[i]);

                //----listwidgetconvert--------------------------------------------------


                QListWidgetItem *item = new QListWidgetItem();
                item->setBackground(QBrush("#333333"));
                item->setText("   "+ QString::number(i));
                //DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate.at(i);
                //item->setText("    "+ QString::number(i)+ " " + this->list_dataTrackForDelegate.at(i).title);
                item->setTextColor(QColor::fromRgb(33,33,33));
                item->setFont(QFont("Arial", 8));
                item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
    #if defined(Q_OS_WINDOWS)
                item->setSizeHint(QSize(item->sizeHint().width(), 70));
                //item->setSizeHint(QSize(90, 70));
    #endif
    #if defined(Q_OS_MAC)
                item->setSizeHint(QSize(90, 70));
    #endif

                this->listWidget_add->addItem(item);

                QWidget *line_bottom_1 = new QWidget();
                line_bottom_1->setFixedHeight(70);
                line_bottom_1->setStyleSheet("background-color:#707070;");


                //this->listWidget_add->setItemWidget(item,this->added_playlist[i]);
                this->setDataTrackInfo_RoseListEdit_Item(tmpData,i);
                this->listWidget_add->setItemWidget(item,widget_list_added.at(i));
                //--------------------------------------------------------

                QCoreApplication::processEvents();
            }
        }
    }

    if(this->track_count > 0){
        // Playlsit 기본 정보를 가져와야 하는 경우임. (id만 전달받은 상태)
        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject tmp_json;
        network->request(HTTP_NETWORK_PLAYLIST_FETCH, QString("%1/member/playlist/%2?page=%3&size=20")
                         .arg(global.legacy_v1).arg(this->playlist_id).arg(this->pageCNT), tmp_json, false, true);

    }

    //this->menubar->show();
}


void AbstractAddPlaylist_RHV::setUIControl_Creating(){

    widget_list_added.clear();
    widget_list_exist.clear();
    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    //this->fileDownLoader = new FileDownloader(this);
    //connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractAddPlaylist_RHV::slot_fileDownload_loadImage);

    this->wg_info = new QWidget();
    this->wg_info->setFixedSize(1500, 315);
    this->wg_info->setGeometry(0, 0, 0, 0);
    this->wg_info->setContentsMargins(0, 20, 0, 50);

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->lb_image = new QLabel(this->wg_info);
    this->lb_image->setStyleSheet("background-color:#333");
    this->lb_image->setFixedSize(315, 315);
    this->lb_image->setGeometry(0, 0, 0, 0);
print_debug();
    this->setImage("");
print_debug();

    this->wg_info_title = new QWidget(this->wg_info);
    this->wg_info_title->setStyleSheet("background-color:#333");
    this->wg_info_title->setFixedSize(1125, 50);
    this->wg_info_title->setGeometry(375, 10, 0, 0);

    this->le_title = new QLineEdit(this->wg_info_title);
    this->le_title->setProperty("type", "title");
    this->le_title->setMaxLength(51);
    this->le_title->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_title->setPlaceholderText(tr("Playlist Title"));
    this->le_title->setGeometry(22, 12, 800, 26);

    connect(this->le_title, &QLineEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_title_text);

    this->lb_title = new QLabel(this->wg_info_title);
    this->lb_title->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_title->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_title->setText("0/50 " + tr("Charaters"));
    this->lb_title->setGeometry(900, 12, 200, 26);


    this->wg_info_content = new QWidget(this->wg_info);
    this->wg_info_content->setStyleSheet("background-color:#333");
    this->wg_info_content->setFixedSize(1125, 77);
    this->wg_info_content->setGeometry(375, 75, 0, 0);

    this->te_content = new QTextEdit(this->wg_info_content);
    this->te_content->setProperty("type", "contents");
    this->te_content->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;line-height:2pt");
    this->te_content->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->te_content->setPlaceholderText(tr("Please enter\nPlaylist introduction."));
    this->te_content->setGeometry(22, 9, 900, 52);

    connect(this->te_content, &QTextEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_content_text);

    this->lb_content = new QLabel(this->wg_info_content);
    this->lb_content->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_content->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_content->setText("0/500 " + tr("Charaters"));
    this->lb_content->setGeometry(900, 30, 200, 26);


    int pub_left = 0;
    int pub_width = 0;

    this->lb_public_set = new QLabel(wg_info);
    this->lb_public_set->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_set->setText(tr("Public"));

    pub_width = this->lb_public_set->sizeHint().width();
    this->lb_public_set->setGeometry(375, 174, pub_width, 26);

    this->btn_all = GSCommon::getUIBtnImg("btnAll", ":/images/icon_che_on.png", wg_info);
    this->btn_all->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_set->geometry().left() + lb_public_set->geometry().width() + 25;
    this->btn_all->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_all, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_all = new QLabel(wg_info);
    this->lb_public_all->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_all->setText(tr("ALL"));

    pub_left = this->btn_all->geometry().left() + this->btn_all->geometry().width();
    pub_width = this->lb_public_all->sizeHint().width();
    this->lb_public_all->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_freind = GSCommon::getUIBtnImg("btnFreind", ":/images/icon_che_off.png", wg_info);
    this->btn_freind->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_all->geometry().left() + lb_public_all->geometry().width() + 50;
    this->btn_freind->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_freind, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_freind = new QLabel(wg_info);
    this->lb_public_freind->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_freind->setText(tr("FRIEND"));

    pub_left = this->btn_freind->geometry().left() + this->btn_freind->geometry().width();
    pub_width = this->lb_public_freind->sizeHint().width();
    this->lb_public_freind->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_private = GSCommon::getUIBtnImg("btnPrivate", ":/images/icon_che_off.png", wg_info);
    this->btn_private->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_freind->geometry().left() + lb_public_freind->geometry().width() + 50;
    this->btn_private->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_private, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_private = new QLabel(wg_info);
    this->lb_public_private->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_private->setText(tr("PRIVATE"));

    pub_left = this->btn_private->geometry().left() + this->btn_private->geometry().width();
    pub_width = this->lb_public_private->sizeHint().width();
    this->lb_public_private->setGeometry(pub_left, 174, pub_width, 26);

    this->flag_all_enable = true;
    this->flag_freind_enable = false;
    this->flag_private_enable = false;


    this->wg_info_tag = new QWidget(this->wg_info);
    this->wg_info_tag->setStyleSheet("background-color:#333");
    this->wg_info_tag->setFixedSize(1125, 50);
    this->wg_info_tag->setGeometry(375, 220, 0, 0);

    this->le_tag = new QLineEdit(this->wg_info_tag);
    this->le_tag->setProperty("type", "tag");
    this->le_tag->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_tag->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_tag->setPlaceholderText(tr("#Please enter your tags."));
    this->le_tag->setGeometry(22, 12, 1000, 26);

    pub_left = 0;
    pub_width = 0;
    for(int i = 0; i < this->tagTypeList.size(); i++){
        this->btn_tag[i] = new QPushButton(wg_info);
        this->btn_tag[i]->setProperty("idx", i);
        this->btn_tag[i]->setFixedHeight(30);
        this->btn_tag[i]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
        this->btn_tag[i]->setCursor(Qt::PointingHandCursor);
        this->btn_tag[i]->setText(this->tagTypeList[i].replace("&", "&&"));

        if(pub_left == 0 && pub_width == 0){
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(375, 280, pub_width, 30);
        }
        else{
            pub_left = this->btn_tag[i - 1]->geometry().left() + this->btn_tag[i - 1]->geometry().width() + 10;
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(pub_left, 280, pub_width, 30);
        }

        connect(this->btn_tag[i], &QPushButton::pressed, this, &AbstractAddPlaylist_RHV::slot_btnTags_pressed);
        connect(this->btn_tag[i], &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnTags_clicked);
    }

    if(contents_type == ContentsType::Qobuz || contents_type == ContentsType::Tidal){

        if(contents_type == ContentsType::Qobuz){
            this->lb_listCheck->setText(tr("Save also to QOBUZ"));
        }
        else if(contents_type == ContentsType::Tidal){
            this->lb_listCheck->setText(tr("Save also to TIDAL"));
        }

        this->lb_selectList->setText(tr("Select Playlist"));
        this->lb_del->setText(tr("Delete Tracks"));

        this->btn_withStream->show();
        this->lb_del->show();
        this->lb_del->setEnabled(true);
        this->btn_selectList->show();
        this->btn_selectList->setEnabled(false);
    }

    this->listWidget_add = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_add->setCursor(Qt::PointingHandCursor);

    this->listWidget_add->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->listWidget_add->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_add->setStyleSheet("background-color:#333333;");


    this->vl_Added = new QVBoxLayout();
    this->vl_Added->setContentsMargins(5, 0, 5, 0);
    this->vl_Added->setSpacing(0);
    this->vl_Added->setAlignment(Qt::AlignTop);
    this->vl_Added->addWidget(this->listWidget_add);


    this->wg_Added = new QWidget();
    this->wg_Added->setLayout(this->vl_Added);

    //linker = Linker::getInstance();
    //connect(listWidget, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));
    //connect(linker, SIGNAL(signal_dragDrop()), this, SLOT(slot_dragAndDropLoop()));

    this->listWidget_exist = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_exist->setCursor(Qt::PointingHandCursor);

    this->listWidget_exist->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->listWidget_exist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_exist->setStyleSheet("background-color:#333333;");


    this->vl_Existing = new QVBoxLayout();
    this->vl_Existing->setContentsMargins(5, 0, 5, 0);
    this->vl_Existing->setSpacing(0);
    this->vl_Existing->setAlignment(Qt::AlignTop);
    this->vl_Existing->addWidget(this->listWidget_exist);

    this->wg_Existing = new QWidget();
    this->wg_Existing->setLayout(this->vl_Existing);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->wg_Added);
    this->stackedwidget->addWidget(this->wg_Existing);
    this->stackedwidget->setFixedHeight(210);

    this->wg_button = new QWidget();
    this->wg_button->setFixedSize(1500, 120);

    this->btn_confirm = new QPushButton(this->wg_button);
    this->btn_confirm->setText(tr("Confirm"));
    this->btn_confirm->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                     "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
    this->btn_confirm->setCursor(Qt::PointingHandCursor);
    this->btn_confirm->setFixedSize(140, 40);
    this->btn_confirm->setGeometry(590, 30, 0, 0);

    connect(this->btn_confirm, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnConfirm);

    this->btn_cancel = new QPushButton(this->wg_button);
    this->btn_cancel->setText(tr("Cancel"));
    this->btn_cancel->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                     "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(760, 30, 0, 0);

    connect(this->btn_cancel, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnCancel);

    this->vl_addPlaylist = new QVBoxLayout();
    this->vl_addPlaylist->setAlignment(Qt::AlignTop);
    this->vl_addPlaylist->addWidget(this->wg_info);
    this->vl_addPlaylist->addSpacing(45);
    this->vl_addPlaylist->addWidget(this->wg_menubar);
    this->vl_addPlaylist->addWidget(this->stackedwidget);
    this->vl_addPlaylist->addSpacing(30);
    this->vl_addPlaylist->addWidget(this->wg_button);

    this->setLayout(this->vl_addPlaylist);

    connect(this->listWidget_add, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));
    connect(linker, SIGNAL(signal_dragDrop_playlist()), this, SLOT(slot_dragAndDropLoop()));

    print_debug();

}

void AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item(const QJsonObject &trackInfo, const int &p_index){

    print_debug();
    //QJsonDocument doc(trackInfo);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item---" << strJson;

    //this->fileDownLoader = new FileDownloader(this);
    //connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractAddPlaylist_RHV::slot_fileDownload_loadImage);

    this->widget_info_main_item = new QWidget();
    this->widget_info_main_item->setFixedSize(1500, 70);
    this->widget_info_main_item->setObjectName("trackInfo");


    //this->paint_image_item(*this->pixmap_albumImg_default);
    //this->lb_image
    //QVBoxLayout *vl_lb_image = new QVBoxLayout();
    //vl_lb_image->setContentsMargins(5, 0, 5, 0);
    //vl_lb_image->setSpacing(0);
    //vl_lb_image->addWidget(this->lb_image);

    //hl_listBoxlayout->addWidget(checkbox_Selected.at(p_index));
    checkbox_Selected.append(new QCheckBox(this->widget_info_main_item));
    checkbox_Selected.at(p_index)->setGeometry(5, 15, 60, 60);
    checkbox_Selected.at(p_index)->setProperty("check_index", p_index);
    checkbox_Selected.at(p_index)->setStyleSheet(
                "#checkbox_Selected { background-color:transparent;spacing:0px;}"
                "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                );
    //checkbox_Selected->setCursor(Qt::PointingHandCursor);
    checkbox_Selected.at(p_index)->setFixedSize(40,33);
    connect(checkbox_Selected.at(p_index), SIGNAL(stateChanged(int)), this, SLOT(slot_clickedSelected(int)));


    this->pixmap_albumImg_default_track = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default_track = this->pixmap_albumImg_default_track->scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    /*

    this->label_track_thumbnail_item.append(new QLabel(this->widget_info_main_item));
    //this->label_track_thumbnail_item.at(p_index) = new QLabel(this->widget_info_main_item);
    this->label_track_thumbnail_item.at(p_index)->setScaledContents(true);
    this->label_track_thumbnail_item.at(p_index)->setGeometry(60+5, 5, 60, 60);
    this->label_track_thumbnail_item.at(p_index)->setStyleSheet("background-color:transparent;");
    //this->label_track_thumbnail_item.at(p_index)->setPixmap(*this->pixmap_albumImg_default_track);
    //this->paint_image_item(album_PixmapBig);
*/
    //this->added_playlist[p_index]->label_track_thumbnail;
    QVBoxLayout *vl_lb_image = new QVBoxLayout();
    vl_lb_image->setContentsMargins(0, 0, 0, 0);
    vl_lb_image->setSpacing(0);
    vl_lb_image->addWidget(this->added_playlist[p_index]->getThumnailLabel());
    //vl_lb_image->addWidget(this->added_playlist[p_index]->label_track_thumbnail);//private-->public change

    this->label_track_thumbnail_item = new QLabel(this->widget_info_main_item);
    this->label_track_thumbnail_item->setScaledContents(true);
    this->label_track_thumbnail_item->setGeometry(60+5, 5, 60, 60);
    this->label_track_thumbnail_item->setStyleSheet("background-color:transparent;");
    this->label_track_thumbnail_item->setLayout(vl_lb_image);



    //this->label_track_thumbnail_item.at(p_index)->setPixmap(*this->pixmap_albumImg_default_track);
    //this->paint_image_item(album_PixmapBig);

    this->label_track_type = new QLabel(this->widget_info_main_item);
    this->label_track_type->setGeometry(60+78, 15, 30, 30);
    this->label_track_type->setStyleSheet("background-color:transparent;");
    //this->label_track_type->hide();

    this->label_title = new QLabel(this->widget_info_main_item);
    this->label_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_title->setGeometry(60+120, 10, 720, 25);

    this->label_artist = new QLabel(this->widget_info_main_item);
    this->label_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    this->label_artist->setGeometry(60+120, 34, 720, 25);

    this->label_track_resolution = new QLabel(this->widget_info_main_item);
    this->label_track_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    this->label_track_resolution->setGeometry(60+480, 34, 360, 25);
    this->label_track_resolution->hide();

    this->label_track_hires = new QLabel(this->widget_info_main_item);
    this->label_track_hires->hide();

    this->label_album = new QLabel(this->widget_info_main_item);
    this->label_album->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_album->setWordWrap(true);
    this->label_album->setGeometry(60+937, 14, 220, 40);

    this->label_duration = new QLabel(this->widget_info_main_item);
    this->label_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_duration->setGeometry(60+1231, 24, 65, 20);
/*
    this->btn_play_list_add = GSCommon::getUIBtnImg("btn_play_list_add", ":/images/play_list_add_icon.png", this->widget_info_main);
    this->btn_play_list_add->setGeometry(1350, 10, 50, 50);
    this->btn_play_list_add->setCursor(Qt::PointingHandCursor);
    this->btn_play_list_add->setProperty("type", "playlist");

    this->btn_list_fav = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, this->widget_info_main);
    this->btn_list_fav->setGeometry(1400, 6, 50, 58);
    this->btn_list_fav->setCursor(Qt::PointingHandCursor);
    this->btn_list_fav->setProperty("type", "fav");

    this->btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", this->widget_info_main);
    this->btn_menu->setGeometry(1450, 6, 50, 58);
    this->btn_menu->setCursor(Qt::PointingHandCursor);
    this->btn_menu->setProperty("type", "more");
*/
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->addWidget(this->widget_info_main_item);

    widget_list_added.append(new QWidget());
    widget_list_added.at(p_index)->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { background-color:#333333; } ");
    widget_list_added.at(p_index)->setFixedHeight(70);
    widget_list_added.at(p_index)->setLayout(vl_total);
    widget_list_added.at(p_index)->setCursor(Qt::PointingHandCursor);
    widget_list_added.at(p_index)->setProperty("type", "playtrack");

 //-----------------------------------------------------------------------------

    QString type = ProcJsonEasy::getString(trackInfo, "type");
    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    QString artist = ProcJsonEasy::getString(trackInfo, "artist");
    int duration = ProcJsonEasy::getInt(trackInfo, "duration");
    bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
    double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

   // this->btn_play_list_add->hide();
   // this->btn_list_fav->hide();
   // this->btn_menu->hide();

    //this->playlist_type = type;

    QString default_path = "";
    QString type_path = "";
    QString hires_path = "";
    if(type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
        hires_path = ":/images/tidal/mqa_ico.png";
    }
    else if(type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
        hires_path = ":/images/qobuz/hires_ico.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(default_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    //this->setImage(thumbnail);

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
    this->label_track_type->setPixmap(*pixmap_icon);
    this->label_track_type->setGeometry(60+68, 9, 49, 43);
    this->label_track_type->setStyleSheet("background-color:transparent;");
    this->label_track_type->show();

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->label_title->font()));
    }
    else{
        this->label_title->setText(title);
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    this->label_artist->setText(artist);

    if(hires == true){

        int hires_width = 0;
        int hires_height = 0;
        if(this->playlist_type == "TIDAL"){
            hires_width = 36;
            hires_height = 13;
        }
        else if(this->playlist_type == "QOBUZ"){
            hires_width = 30;
            hires_height = 30;
        }

        //this->label_track_hires = new QLabel(this->widget_info_main);

        QPixmap pixmap(hires_path);
        pixmap = pixmap.scaled(hires_width, hires_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPixmap pixmap_painter = QPixmap(QSize(hires_width, hires_height));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QBrush brush = QBrush(pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, hires_width, hires_height, 2, Qt::RelativeSize);

        this->label_track_hires->setPixmap(pixmap_painter);

        int left = this->label_title->geometry().left() + this->label_title->geometry().width() + 20;
        int top = (70 / 2) - (this->label_track_hires->sizeHint().height() / 2);

        this->label_track_hires->setGeometry(left, top, hires_width, hires_height);
    }

    if(sampling > 0){
        //this->label_track_resolution = new QLabel(this->widget_info_main);
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        this->label_track_resolution->setText(QString("%1 kHz").arg(sampling));
        this->label_track_resolution->setAlignment(Qt::AlignCenter);

        int width = this->label_track_resolution->sizeHint().width() + 10;
        int left = 0;
        if(artist_width > 720){
            left = this->label_artist->geometry().left() + this->label_artist->geometry().width() + 15;
        }
        else{
            left = this->label_artist->geometry().left() + artist_width + 15;
        }

        this->label_track_resolution->setGeometry(left, 40, width, 16);
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(title);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width <= 210){
        this->label_album->setText(title);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = title;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){
            for(i = 0; i < splitToken.count(); i++){
                tmp_album_set += splitToken.at(i) + " ";
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() > 210){
                    tmp_album_title->setText("");
                    tmp_album_set = "";
                    break;
                }
            }

            for(int j = i; j < splitToken.count(); j++){
                if(j + 1 == splitToken.count()){
                    tmp_album_set += splitToken.at(j);
                }
                else{
                    if(!splitToken.at(j).isEmpty()){
                        tmp_album_set += splitToken.at(j) + " ";
                    }
                }

                tmp_album_title->setText(tmp_album_set);
            }

            album_title_width = tmp_album_title->sizeHint().width();
            if(album_title_width <= 210){
                this->label_album->setText(title);
            }
            else{
                this->label_album->setText(GSCommon::getTextCutFromLabelWidth(title, (210 * 2) - 40, this->label_album->font()));
            }
        }
    }

    if(duration == 0){
        this->label_duration->setText("");
    }
    else if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }

    this->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { } ");
    //this->setCursor(Qt::ArrowCursor);//c0208
}

void AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item_exist(const QJsonObject &trackInfo, const int &p_index){
    print_debug();
    //QJsonDocument doc(trackInfo);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << " AbstractAddPlaylist_RHV::setDataTrackInfo_RoseListEdit_Item_exist---" << strJson;
    //this->fileDownLoader = new FileDownloader(this);
    //connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractAddPlaylist_RHV::slot_fileDownload_loadImage);

    this->widget_info_main_item = new QWidget();
    this->widget_info_main_item->setFixedSize(1500, 70);
    this->widget_info_main_item->setObjectName("trackInfo");


    //this->paint_image_item(*this->pixmap_albumImg_default);
    //this->lb_image


    //hl_listBoxlayout->addWidget(checkbox_Selected.at(p_index));
    checkbox_Selected_Exist.append(new QCheckBox(this->widget_info_main_item));
    checkbox_Selected_Exist.at(p_index)->setGeometry(5, 15, 60, 60);
    checkbox_Selected_Exist.at(p_index)->setProperty("check_index", p_index);
    checkbox_Selected_Exist.at(p_index)->setStyleSheet(
                "#checkbox_Selected { background-color:transparent;spacing:0px;}"
                "QCheckBox::indicator:unchecked { image: url(:images/playlist/list_check_off.png);}"
                "QCheckBox::indicator:checked { image: url(:images/playlist/list_check_on.png);}"
                );
    //checkbox_Selected->setCursor(Qt::PointingHandCursor);
    checkbox_Selected_Exist.at(p_index)->setFixedSize(40,33);
    connect(checkbox_Selected_Exist.at(p_index), SIGNAL(stateChanged(int)), this, SLOT(slot_clickedSelected_exist(int)));


    QVBoxLayout *vl_lb_image = new QVBoxLayout();
    vl_lb_image->setContentsMargins(0, 0, 0, 0);
    vl_lb_image->setSpacing(0);
    vl_lb_image->addWidget(this->existing_playlist[p_index]->getThumnailLabel());
    //vl_lb_image->addWidget(this->existing_playlist[p_index]->label_track_thumbnail);//private-->public change

    this->label_track_thumbnail_item = new QLabel(this->widget_info_main_item);
    this->label_track_thumbnail_item->setScaledContents(true);
    this->label_track_thumbnail_item->setGeometry(60+5, 5, 60, 60);
    this->label_track_thumbnail_item->setStyleSheet("background-color:transparent;");
    this->label_track_thumbnail_item->setLayout(vl_lb_image);


    this->label_track_type = new QLabel(this->widget_info_main_item);
    this->label_track_type->setGeometry(60+78, 15, 30, 30);
    this->label_track_type->setStyleSheet("background-color:transparent;");
    //this->label_track_type->hide();

    this->label_title = new QLabel(this->widget_info_main_item);
    this->label_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    this->label_title->setGeometry(60+120, 10, 720, 25);

    this->label_artist = new QLabel(this->widget_info_main_item);
    this->label_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    this->label_artist->setGeometry(60+120, 34, 720, 25);

    this->label_track_resolution = new QLabel(this->widget_info_main_item);
    this->label_track_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
    this->label_track_resolution->setGeometry(60+480, 34, 360, 25);
    this->label_track_resolution->hide();

    this->label_track_hires = new QLabel(this->widget_info_main_item);
    this->label_track_hires->hide();

    this->label_album = new QLabel(this->widget_info_main_item);
    this->label_album->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_album->setWordWrap(true);
    this->label_album->setGeometry(60+937, 14, 220, 40);

    this->label_duration = new QLabel(this->widget_info_main_item);
    this->label_duration->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    this->label_duration->setGeometry(60+1231, 24, 65, 20);
    /*
this->btn_play_list_add = GSCommon::getUIBtnImg("btn_play_list_add", ":/images/play_list_add_icon.png", this->widget_info_main);
this->btn_play_list_add->setGeometry(1350, 10, 50, 50);
this->btn_play_list_add->setCursor(Qt::PointingHandCursor);
this->btn_play_list_add->setProperty("type", "playlist");

this->btn_list_fav = GSCommon::getUIBtnImg("btn_list_fav", ICON_PATH___favorite_off, this->widget_info_main);
this->btn_list_fav->setGeometry(1400, 6, 50, 58);
this->btn_list_fav->setCursor(Qt::PointingHandCursor);
this->btn_list_fav->setProperty("type", "fav");

this->btn_menu = GSCommon::getUIBtnImg("btn_menu", ":/images/icon_menu.png", this->widget_info_main);
this->btn_menu->setGeometry(1450, 6, 50, 58);
this->btn_menu->setCursor(Qt::PointingHandCursor);
this->btn_menu->setProperty("type", "more");
*/
    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->addWidget(this->widget_info_main_item);
    widget_list_exist.append(new QWidget());
    widget_list_exist.at(p_index)->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { background-color:#333333; } ");
    widget_list_exist.at(p_index)->setFixedHeight(70);
    widget_list_exist.at(p_index)->setLayout(vl_total);
    widget_list_exist.at(p_index)->setCursor(Qt::PointingHandCursor);
    widget_list_exist.at(p_index)->setProperty("type", "playtrack");

    //-----------------------------------------------------------------------------

    QString type = ProcJsonEasy::getString(trackInfo, "type");
    QString thumbnail = ProcJsonEasy::getString(trackInfo, "thumbnail");
    QString title = ProcJsonEasy::getString(trackInfo, "title");
    QString artist = ProcJsonEasy::getString(trackInfo, "artist");
    int duration = ProcJsonEasy::getInt(trackInfo, "duration");
    bool hires = ProcJsonEasy::getBool(trackInfo, "hires");
    double sampling = ProcJsonEasy::getDouble(trackInfo, "maximum_sampling_rate");

    // this->btn_play_list_add->hide();
    // this->btn_list_fav->hide();
    // this->btn_menu->hide();

    this->playlist_type = type;
qDebug() << "type = " << type;
    QString default_path = "";
    QString type_path = "";
    QString hires_path = "";
    if(this->playlist_type == "MUSIC"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }
    else if(this->playlist_type == "VIDEO"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_video_s.png";
    }
    else if(this->playlist_type == "YOUTUBE"){
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_tube_s.png";
    }
    else if(this->playlist_type == "TIDAL"){
        default_path = ":/images/tidal/tidal_def_400.png";
        type_path = ":/images/rosehome/home_tidal_s.png";
        hires_path = ":/images/tidal/mqa_ico.png";
    }
    else if(this->playlist_type == "BUGS"){
        default_path = ":/images/bugs/bugs_def_430x2.png";
        type_path = ":/images/rosehome/home_bugs_s.png";
    }
    else if(this->playlist_type == "QOBUZ"){
        default_path = ":/images/qobuz/qobuz_default_400.png";
        type_path = ":/images/rosehome/home_qobuz_s.png";
        hires_path = ":/images/qobuz/hires_ico.png";
    }
    else{
        default_path = ":/images/def_mus_300.png";
        type_path = ":/images/rosehome/home_music_s.png";
    }

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(default_path);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    //this->setImage(thumbnail);

    QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(type_path);
    this->label_track_type->setPixmap(*pixmap_icon);
    this->label_track_type->setGeometry(60+68, 9, 49, 43);
    this->label_track_type->setStyleSheet("background-color:transparent;");
    this->label_track_type->show();

    QLabel *tmp_title = new QLabel();
    tmp_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:16px;font-weight:normal;");
    tmp_title->setText(title);

    int title_width = 0;
    title_width = tmp_title->sizeHint().width();

    if(title_width > 720){
        this->label_title->setText(GSCommon::getTextCutFromLabelWidth(title, 720, this->label_title->font()));
    }
    else{
        this->label_title->setText(title);
    }

    QLabel *tmp_artist = new QLabel();
    tmp_artist->setStyleSheet("background-color:transparent;color:#999999;font-size:16px;font-weight:300;");
    tmp_artist->setText(artist);

    int artist_width = 0;
    artist_width = tmp_artist->sizeHint().width();

    this->label_artist->setText(artist);

    if(hires == true){

        int hires_width = 0;
        int hires_height = 0;
        if(this->playlist_type == "TIDAL"){
            hires_width = 36;
            hires_height = 13;
        }
        else if(this->playlist_type == "QOBUZ"){
            hires_width = 30;
            hires_height = 30;
        }

        //this->label_track_hires = new QLabel(this->widget_info_main);

        QPixmap pixmap(hires_path);
        pixmap = pixmap.scaled(hires_width, hires_height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QPixmap pixmap_painter = QPixmap(QSize(hires_width, hires_height));
        pixmap_painter.fill(Qt::transparent);

        QPainter painter (&pixmap_painter);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QBrush brush = QBrush(pixmap);
        painter.setBrush(brush);
        painter.drawRoundedRect(0, 0, hires_width, hires_height, 2, Qt::RelativeSize);

        this->label_track_hires->setPixmap(pixmap_painter);

        int left = this->label_title->geometry().left() + this->label_title->geometry().width() + 20;
        int top = (70 / 2) - (this->label_track_hires->sizeHint().height() / 2);

        this->label_track_hires->setGeometry(left, top, hires_width, hires_height);
    }

    if(sampling > 0){
        //this->label_track_resolution = new QLabel(this->widget_info_main);
        this->label_track_resolution->setStyleSheet("background-color:transparent;color:#FFF;font-size:12px;font-weight:300;border:1px solid #FFF;border-radius:8px;");
        this->label_track_resolution->setText(QString("%1 kHz").arg(sampling));
        this->label_track_resolution->setAlignment(Qt::AlignCenter);

        int width = this->label_track_resolution->sizeHint().width() + 10;
        int left = 0;
        if(artist_width > 720){
            left = this->label_artist->geometry().left() + this->label_artist->geometry().width() + 15;
        }
        else{
            left = this->label_artist->geometry().left() + artist_width + 15;
        }

        this->label_track_resolution->setGeometry(left, 40, width, 16);
    }

    QLabel *tmp_album_title = new QLabel();
    tmp_album_title->setStyleSheet("background-color:transparent;color:#FFFFFF;font-size:14px;font-weight:300;");
    tmp_album_title->setText(title);

    int album_title_width = 0;
    album_title_width = tmp_album_title->sizeHint().width();

    if(album_title_width <= 210){
        this->label_album->setText(title);
    }
    else{
        QString tmp_split = "";
        QStringList splitToken;
        QString tmp_album_set = "";

        tmp_split = title;
        splitToken = tmp_split.split(" ");

        tmp_album_title->setText("");
        int i = 0;
        if(splitToken.size() > 1){
            for(i = 0; i < splitToken.count(); i++){
                tmp_album_set += splitToken.at(i) + " ";
                tmp_album_title->setText(tmp_album_set);

                if(tmp_album_title->sizeHint().width() > 210){
                    tmp_album_title->setText("");
                    tmp_album_set = "";
                    break;
                }
            }

            for(int j = i; j < splitToken.count(); j++){
                if(j + 1 == splitToken.count()){
                    tmp_album_set += splitToken.at(j);
                }
                else{
                    if(!splitToken.at(j).isEmpty()){
                        tmp_album_set += splitToken.at(j) + " ";
                    }
                }

                tmp_album_title->setText(tmp_album_set);
            }

            album_title_width = tmp_album_title->sizeHint().width();
            if(album_title_width <= 210){
                this->label_album->setText(title);
            }
            else{
                this->label_album->setText(GSCommon::getTextCutFromLabelWidth(title, (210 * 2) - 40, this->label_album->font()));
            }
        }
    }

    if(duration == 0){
        this->label_duration->setText("");
    }
    else if(duration >= 3600){
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("hh:mm:ss"));
    }
    else{
        this->label_duration->setText(QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
    }

    this->setStyleSheet("#trackInfo { border-bottom:1px solid #333333; } #trackInfo:hover { } ");
    //this->setCursor(Qt::ArrowCursor);//c0208
}

void AbstractAddPlaylist_RHV::slot_clickedSelected(int ss){//1126
    Q_UNUSED(ss);
    print_debug();
    //qDebug() << "ss = " << ss;
    QCheckBox *tmp_btn = dynamic_cast<QCheckBox *>(sender());
    //tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    int check_index = tmp_btn->property("check_index").toInt();
    qDebug() << "check_index = " << check_index;

    //if(isEditMode == true && !isMoveMode && !isPlayMode ){
        if(checkbox_Selected.at(check_index)->isChecked()){

            print_debug();
            intlist_selectPositon.append(check_index);
            widget_list_added.at(check_index)->setStyleSheet(widget_list_added.at(check_index)->styleSheet().replace("#333333", "#222222"));

        }else{
            bool ff = false;
            for (int i = 0; i < intlist_selectPositon.size(); ++i){

                int vv = intlist_selectPositon.at(i);
                if(vv == check_index){
                    intlist_selectPositon.removeAt(i);
                    ff = true;
                }
            }
            if(ff){
                widget_list_added.at(check_index)->setStyleSheet(widget_list_added.at(check_index)->styleSheet().replace("#222222", "#333333"));

            }
        }
        //lb_title_del->setText(QString(tr("%1 songs selected")).arg(intlist_selectPositon.count()));

    //}


}
void AbstractAddPlaylist_RHV::slot_clickedSelected_exist(int ss){//1126
    Q_UNUSED(ss);
    print_debug();
    //qDebug() << "ss = " << ss;
    QCheckBox *tmp_btn = dynamic_cast<QCheckBox *>(sender());
    //tmp_btn->setStyleSheet(tmp_btn->styleSheet().replace("transparent", "#B18658"));
    int check_index = tmp_btn->property("check_index").toInt();
    qDebug() << "check_index = " << check_index;

    //if(isEditMode == true && !isMoveMode && !isPlayMode ){
        if(checkbox_Selected_Exist.at(check_index)->isChecked()){

            print_debug();
            intlist_selectPositon.append(check_index);
            widget_list_exist.at(check_index)->setStyleSheet(widget_list_exist.at(check_index)->styleSheet().replace("#333333", "#222222"));

        }else{
            bool ff = false;
            for (int i = 0; i < intlist_selectPositon.size(); ++i){

                int vv = intlist_selectPositon.at(i);
                if(vv == check_index){
                    intlist_selectPositon.removeAt(i);
                    ff = true;
                }
            }
            if(ff){
                widget_list_exist.at(check_index)->setStyleSheet(widget_list_exist.at(check_index)->styleSheet().replace("#222222", "#333333"));

            }
        }
        //lb_title_del->setText(QString(tr("%1 songs selected")).arg(intlist_selectPositon.count()));

    //}


}

void AbstractAddPlaylist_RHV::slot_fileDownload_loadImage(){

    QImage image;
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(315, 315));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(315, 315, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);


        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, 315, 315, 2, Qt::RelativeSize);

        painter.setClipPath(path);
        /*if(tmp_pixmap.width() > 315){
            int start_width = (tmp_pixmap.width() - 315) / 2;
            painter.drawPixmap((0 - start_width), 0, tmp_pixmap);
        }
        else{
            painter.drawPixmap(0, 0, tmp_pixmap);
        }*/


        int leftValue = 0;
        int topValue = 0;

        if(tmp_pixmap.width() > 315){
            leftValue = (315 - tmp_pixmap.width()) / 2;
        }

        if(tmp_pixmap.height() > 315){
            topValue = (315 - tmp_pixmap.height()) / 2;
        }

        painter.drawPixmap(leftValue, topValue, tmp_pixmap);
        painter.end();

        this->lb_image->setPixmap(pixmapIMG);
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }

    this->fileDownLoader->deleteLater();
}


/*
void AbstractAddPlaylist_RHV::slot_fileDownload_loadImage(){
    print_debug();
    QImage image;
    //bool flagLoad = this->pixmap_albumImg->loadFromData(this->fileDownLoader->downloadedData());
    bool flagLoad = image.loadFromData(this->fileDownLoader->downloadedData());

    if(flagLoad){
        QPixmap pixmapIMG = QPixmap(QSize(60, 60));
        pixmapIMG.fill(Qt::transparent);

        QPixmap tmp_pixmap;
        tmp_pixmap = tmp_pixmap.fromImage(image);
        tmp_pixmap = tmp_pixmap.scaled(60, 60, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        QPainter painter (&pixmapIMG);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        QPainterPath path = QPainterPath();
        path.addRoundedRect(0, 0, 60, 60, 2, Qt::RelativeSize);
        painter.setClipPath(path);
        if(tmp_pixmap.width() > 60){
            int start_width = (tmp_pixmap.width() - 60) / 2;
            painter.drawPixmap((0 - start_width), 0, tmp_pixmap);
        }
        else{
            painter.drawPixmap(0, 0, tmp_pixmap);
        }
        painter.end();

        this->label_track_thumbnail_item->setPixmap(pixmapIMG);
    }
    else{
        this->paint_image_item(*this->pixmap_albumImg_default);
    }
}
*/


void AbstractAddPlaylist_RHV::setImage_item(QString imagePath){
    // 이미지 경로 설정
    print_debug();
    if(imagePath.isEmpty() == false){
         this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_image_item(*this->pixmap_albumImg_default);
    }

}

void AbstractAddPlaylist_RHV::paint_image_item(QPixmap &pixmap){
    // 빈 Pixmap
    print_debug();
    QPixmap pixmap_painter = QPixmap(QSize(60, 60));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QBrush brush = QBrush(pixmap);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, 60, 60, 2, Qt::RelativeSize);
    this->label_track_thumbnail_item->setPixmap(pixmap_painter);

    /*
    //QPixmap tmp_pixmap = tmp_pixmap.fromImage(image,  Qt::AutoColor);

    pixmap = pixmap.scaled(60,60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QPixmap pixmap_painter = QPixmap(QSize(60, 60));
    pixmap_painter.fill(Qt::transparent);


    if(contentStep == ALBTAB_STEP_ADDED){
        for(int i = 0 ; i < this->added_array.size(); i++){
           // this->label_track_thumbnail_item.at(i)->setPixmap(pixmap);
        }
    }
    if(contentStep == ALBTAB_STEP_EXISTING){
        for(int i = 0 ; i < this->existing_array.size(); i++){
            //this->label_track_thumbnail_item.at(i)->setPixmap(pixmap);
        }
    }
    */


}

void AbstractAddPlaylist_RHV::slot_dragAndDropLoop(){
    print_debug();
    this->drop_cnt_flag++;
    qDebug() << "this->drop_cnt_flag = " << this->drop_cnt_flag;

}
void AbstractAddPlaylist_RHV::slot_listwidgetItemPressed(QListWidgetItem *item){  //cheon210916-total
    print_debug();
    qDebug() << "slot_listwidgetItemPressed ";

    int tmp_index = this->listWidget_add->row(item);
    qDebug() << "tmp_index=" << tmp_index;


}
void AbstractAddPlaylist_RHV::setUIControl_Adding(){

    // 파일다운로드 객체 생성 및 커넥트
    this->pixmap_albumImg = new QPixmap();
    //this->fileDownLoader = new FileDownloader(this);
    //connect(this->fileDownLoader, &FileDownloader::downloaded, this, &AbstractAddPlaylist_RHV::slot_fileDownload_loadImage);

    this->wg_info = new QWidget();
    this->wg_info->setFixedSize(1500, 315);
    this->wg_info->setGeometry(0, 0, 0, 0);
    this->wg_info->setContentsMargins(0, 20, 0, 50);

    this->pixmap_albumImg_default = GSCommon::getUIPixmapImg(IMAGE_PATH_DEFAULT);
    *this->pixmap_albumImg_default = this->pixmap_albumImg_default->scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    this->lb_image = new QLabel(this->wg_info);
    this->lb_image->setStyleSheet("background-color:#333");
    this->lb_image->setFixedSize(315, 315);
    this->lb_image->setGeometry(0, 0, 0, 0);
print_debug();
    this->setImage("");

    this->wg_info_title = new QWidget(this->wg_info);
    this->wg_info_title->setStyleSheet("background-color:#333");
    this->wg_info_title->setFixedSize(1125, 50);
    this->wg_info_title->setGeometry(375, 10, 0, 0);

    QString tmp_title = ProcJsonEasy::getString(this->playlist_info, "title");

    this->le_title = new QLineEdit(this->wg_info_title);
    this->le_title->setProperty("type", "title");
    this->le_title->setMaxLength(51);
    this->le_title->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_title->setText(tmp_title);
    this->le_title->setGeometry(22, 12, 800, 26);

    connect(this->le_title, &QLineEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_title_text);

    this->lb_title = new QLabel(this->wg_info_title);
    this->lb_title->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_title->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    this->lb_title->setText(QString("%1/50 ").arg(tmp_title.size()) + tr("Charaters"));
    this->lb_title->setGeometry(900, 12, 200, 26);


    this->wg_info_content = new QWidget(this->wg_info);
    this->wg_info_content->setStyleSheet("background-color:#333");
    this->wg_info_content->setFixedSize(1125, 77);
    this->wg_info_content->setGeometry(375, 75, 0, 0);

    QString tmp_content = ProcJsonEasy::getString(this->playlist_info, "comment");

    this->te_content = new QTextEdit(this->wg_info_content);
    this->te_content->setProperty("type", "contents");
    this->te_content->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;line-height:2pt");
    this->te_content->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->te_content->setText(tmp_content);
    this->te_content->setGeometry(22, 9, 900, 52);

    connect(this->te_content, &QTextEdit::textChanged, this, &AbstractAddPlaylist_RHV::setChange_content_text);

    this->lb_content = new QLabel(this->wg_info_content);
    this->lb_content->setStyleSheet("background-color:transparent;color:#777;font-size:16px;font-weight:Normal;");
    this->lb_content->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    this->lb_content->setText(QString("%1/500 ").arg(tmp_content.size()) + tr("Charaters"));
    this->lb_content->setGeometry(900, 30, 200, 26);


    int pub_left = 0;
    int pub_width = 0;

    this->lb_public_set = new QLabel(wg_info);
    this->lb_public_set->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_set->setText(tr("Public"));

    pub_width = this->lb_public_set->sizeHint().width();
    this->lb_public_set->setGeometry(375, 174, pub_width, 26);

    this->btn_all = GSCommon::getUIBtnImg("btnAll", ":/images/icon_che_off.png", wg_info);
    this->btn_all->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_set->geometry().left() + lb_public_set->geometry().width() + 25;
    this->btn_all->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_all, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_all = new QLabel(wg_info);
    this->lb_public_all->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_all->setText(tr("ALL"));

    pub_left = this->btn_all->geometry().left() + this->btn_all->geometry().width();
    pub_width = this->lb_public_all->sizeHint().width();
    this->lb_public_all->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_freind = GSCommon::getUIBtnImg("btnFreind", ":/images/icon_che_off.png", wg_info);
    this->btn_freind->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_all->geometry().left() + lb_public_all->geometry().width() + 50;
    this->btn_freind->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_freind, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_freind = new QLabel(wg_info);
    this->lb_public_freind->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_freind->setText(tr("FRIEND"));

    pub_left = this->btn_freind->geometry().left() + this->btn_freind->geometry().width();
    pub_width = this->lb_public_freind->sizeHint().width();
    this->lb_public_freind->setGeometry(pub_left, 174, pub_width, 26);

    this->btn_private = GSCommon::getUIBtnImg("btnPrivate", ":/images/icon_che_off.png", wg_info);
    this->btn_private->setCursor(Qt::PointingHandCursor);

    pub_left = this->lb_public_freind->geometry().left() + lb_public_freind->geometry().width() + 50;
    this->btn_private->setGeometry(pub_left, 167, 40, 40);

    connect(this->btn_private, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnPublic_clicked);

    this->lb_public_private = new QLabel(wg_info);
    this->lb_public_private->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->lb_public_private->setText(tr("PRIVATE"));

    pub_left = this->btn_private->geometry().left() + this->btn_private->geometry().width();
    pub_width = this->lb_public_private->sizeHint().width();
    this->lb_public_private->setGeometry(pub_left, 174, pub_width, 26);

    if(this->shared_type == "ALL"){
        this->flag_all_enable = true;
        this->flag_freind_enable = false;
        this->flag_private_enable = false;

        this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else if(this->shared_type == "FRIEND"){
        this->flag_all_enable = false;
        this->flag_freind_enable = true;
        this->flag_private_enable = false;

        this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else{
        this->flag_all_enable = false;
        this->flag_freind_enable = false;
        this->flag_private_enable = true;

        this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }

    this->wg_info_tag = new QWidget(this->wg_info);
    this->wg_info_tag->setStyleSheet("background-color:#333");
    this->wg_info_tag->setFixedSize(1125, 50);
    this->wg_info_tag->setGeometry(375, 220, 0, 0);

    this->le_tag = new QLineEdit(this->wg_info_tag);
    this->le_tag->setProperty("type", "tag");
    this->le_tag->setStyleSheet("background-color:transparent;color:#FFF;font-size:20px;font-weight:Normal;");
    this->le_tag->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->le_tag->setGeometry(22, 12, 1000, 26);

    QList<QVariant> list_tags = ProcJsonEasy::getJsonArray(this->playlist_info, "tags").toVariantList();

    QString tags = "";
    for(int i = 0; i < list_tags.size(); i++){
        if(i == 0){
            tags = "#" + list_tags.at(i).toString();
        }
        else{
            tags += ",#" + list_tags.at(i).toString();
        }
    }

    this->le_tag->setText(tags);

    pub_left = 0;
    pub_width = 0;
    for(int i = 0; i < this->tagTypeList.size(); i++){
        this->btn_tag[i] = new QPushButton(wg_info);
        this->btn_tag[i]->setProperty("idx", i);
        this->btn_tag[i]->setFixedHeight(30);
        this->btn_tag[i]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
        this->btn_tag[i]->setCursor(Qt::PointingHandCursor);
        this->btn_tag[i]->setText(this->tagTypeList[i].replace("&", "&&"));

        if(pub_left == 0 && pub_width == 0){
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(375, 280, pub_width, 30);
        }
        else{
            pub_left = this->btn_tag[i - 1]->geometry().left() + this->btn_tag[i - 1]->geometry().width() + 10;
            pub_width = this->btn_tag[i]->sizeHint().width() + 20;
            this->btn_tag[i]->setGeometry(pub_left, 280, pub_width, 30);
        }

        connect(this->btn_tag[i], &QPushButton::pressed, this, &AbstractAddPlaylist_RHV::slot_btnTags_pressed);
        connect(this->btn_tag[i], &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_btnTags_clicked);
    }


    if(contents_type == ContentsType::Qobuz || contents_type == ContentsType::Tidal){

        if(contents_type == ContentsType::Qobuz){
            this->lb_listCheck->setText(tr("Save also to QOBUZ"));
        }
        else if(contents_type == ContentsType::Tidal){
            this->lb_listCheck->setText(tr("Save also to TIDAL"));
        }

        this->lb_selectList->setText(tr("Select Playlist"));
        this->lb_del->setText(tr("Delete Tracks"));

        this->btn_withStream->show();
        this->btn_del->show();
        this->btn_del->setEnabled(true);//c0217
        this->btn_selectList->show();
        this->btn_selectList->setEnabled(false);
    }

    this->listWidget_add = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_add->setCursor(Qt::PointingHandCursor);

    this->listWidget_add->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->listWidget_add->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_add->setStyleSheet("background-color:#333333;");


    this->vl_Added = new QVBoxLayout();
    this->vl_Added->setContentsMargins(5, 0, 5, 0);
    this->vl_Added->setSpacing(0);
    this->vl_Added->setAlignment(Qt::AlignTop);
    this->vl_Added->addWidget(this->listWidget_add);

    this->wg_Added = new QWidget();
    this->wg_Added->setLayout(this->vl_Added);

    connect(this->listWidget_add, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(slot_listwidgetItemPressed(QListWidgetItem *)));
    connect(this->linker, SIGNAL(signal_dragDrop_playlist()), this, SLOT(slot_dragAndDropLoop()));


/*
    this->vl_Added = new QVBoxLayout();
    this->vl_Added->setContentsMargins(5, 0, 5, 0);
    this->vl_Added->setSpacing(0);
    this->vl_Added->setAlignment(Qt::AlignTop);

    this->wg_Added = new QWidget();
    this->wg_Added->setLayout(this->vl_Added);
*/
    this->listWidget_exist = new ListWidget_Rosehome_Playlist(this);
    this->listWidget_exist->setCursor(Qt::PointingHandCursor);

    this->listWidget_exist->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->listWidget_exist->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->listWidget_exist->setStyleSheet("background-color:#333333;");


    this->vl_Existing = new QVBoxLayout();
    this->vl_Existing->setContentsMargins(5, 0, 5, 0);
    this->vl_Existing->setSpacing(0);
    this->vl_Existing->setAlignment(Qt::AlignTop);
    //this->vl_Existing->addWidget(this->listWidget_exist);

    this->wg_Existing = new QWidget();
    this->wg_Existing->setLayout(this->vl_Existing);

    this->stackedwidget = new QStackedWidget();
    this->stackedwidget->addWidget(this->wg_Added);
    this->stackedwidget->addWidget(this->wg_Existing);
    this->stackedwidget->setFixedHeight(210);


    this->wg_button = new QWidget();
    this->wg_button->setFixedSize(1500, 120);

    this->btn_confirm = new QPushButton(this->wg_button);
    this->btn_confirm->setText(tr("Confirm"));
    this->btn_confirm->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                     "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
    this->btn_confirm->setCursor(Qt::PointingHandCursor);
    this->btn_confirm->setFixedSize(140, 40);
    this->btn_confirm->setGeometry(590, 30, 0, 0);

    connect(this->btn_confirm, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnConfirm);

    this->btn_cancel = new QPushButton(this->wg_button);
    this->btn_cancel->setText(tr("Cancel"));
    this->btn_cancel->setStyleSheet("QPushButton{background:#212121; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #777777;border-radius:20px;} "
                                     "QPushButton:hover{background:#B18658; font-size:18px;font-weight:500;color:#FFFFFF; border:1px solid #B18658; border-radius:20px;}");
    this->btn_cancel->setCursor(Qt::PointingHandCursor);
    this->btn_cancel->setFixedSize(140, 40);
    this->btn_cancel->setGeometry(760, 30, 0, 0);

    connect(this->btn_cancel, &QPushButton::clicked, this, &AbstractAddPlaylist_RHV::slot_clicked_btnCancel);

    this->vl_addPlaylist = new QVBoxLayout();
    this->vl_addPlaylist->setAlignment(Qt::AlignTop);
    this->vl_addPlaylist->addWidget(this->wg_info);
    this->vl_addPlaylist->addSpacing(50);
    this->vl_addPlaylist->addWidget(this->wg_menubar);
    this->vl_addPlaylist->addWidget(this->stackedwidget);
    this->vl_addPlaylist->addSpacing(30);
    this->vl_addPlaylist->addWidget(this->wg_button);

    this->setLayout(this->vl_addPlaylist);
}


void AbstractAddPlaylist_RHV::setUIControl_Editing(){

}


void AbstractAddPlaylist_RHV::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonData){

    if(p_id == HTTP_NETWORK_PLAYLIST_FETCH){

        const QString jsonKey_flagOk = "flagOk";

        if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
            this->listWidget_exist->setStyleSheet("#QListWidget::item {border-bottom: 1px solid #707070;background-color:#222222; }");//transperant

            if(p_jsonData.contains("playlist")){
                QJsonObject playlist_obj = ProcJsonEasy::getJsonObject(p_jsonData, "playlist");

                if(playlist_obj.contains("tracks")){
                    QJsonArray track_Arr = ProcJsonEasy::getJsonArray(playlist_obj, "tracks");

                    if(track_Arr.size() > 0){
                        //this->btn_del->hide();
                        //this->btn_del->setEnabled(false);//c0217
                        for(int i = 0; i < track_Arr.size(); i++){
                            QJsonObject track_obj = track_Arr.at(i).toObject();

                            QJsonObject sndData;
                            sndData.insert("type", ProcJsonEasy::getString(track_obj, "type"));
                            sndData.insert("title", ProcJsonEasy::getString(track_obj, "title"));
                            sndData.insert("thumbnail", ProcJsonEasy::getString(track_obj, "thumbnailUrl"));

                            QJsonObject tmpData = ProcJsonEasy::getJsonObject(track_obj, "data");
                            sndData.insert("duration", ProcJsonEasy::getInt(tmpData, "duration"));
                            sndData.insert("maximum_sampling_rate", ProcJsonEasy::getDouble(tmpData, "maximum_sampling_rate"));

                            if(tmpData.contains("audioQuality")){
                                QString hires = ProcJsonEasy::getString(tmpData, "audioQuality");
                                if(hires == "HI_RES"){
                                    sndData.insert("hires", true);
                                }
                                else{
                                    sndData.insert("hires", false);
                                }
                            }
                            else if(tmpData.contains("hires")){
                                 sndData.insert("hires", ProcJsonEasy::getBool(tmpData, "hires"));
                            }

                            QJsonObject tmpAlbum = ProcJsonEasy::getJsonObject(tmpData, "album");
                            if(tmpAlbum.contains("artist")){
                                QJsonObject tmpArtist = ProcJsonEasy::getJsonObject(tmpAlbum, "artist");
                                sndData.insert("artist", ProcJsonEasy::getString(tmpArtist, "name"));
                            }
                            else if(tmpData.contains("composer")){
                                QJsonObject tmpComposer = ProcJsonEasy::getJsonObject(tmpData, "composer");
                                sndData.insert("artist", ProcJsonEasy::getString(tmpComposer, "name"));
                            }

                            this->existing_array.append(tmpData);

                            int idx = this->pageCNT * 20;
                            this->existing_playlist[idx + i]->setDataTrackInfo_RoseListEdit(sndData);
                            this->vl_Existing->addWidget(this->existing_playlist[idx + i]);

                            //-------listwidgetconvert------------------------------------------------


                            QListWidgetItem *item = new QListWidgetItem();
                            item->setBackground(QBrush("#333333"));
                            item->setText("   "+ QString::number(idx + i));
                            //DataTrackforDelegate tmp_dataTrack = this->list_dataTrackForDelegate.at(i);
                            //item->setText("    "+ QString::number(i)+ " " + this->list_dataTrackForDelegate.at(i).title);
                            item->setTextColor(QColor::fromRgb(33,33,33));
                            item->setFont(QFont("Arial", 8));
                            item->setFlags(Qt::ItemIsEnabled| Qt::ItemIsSelectable| Qt::ItemIsDragEnabled);
                #if defined(Q_OS_WINDOWS)
                            item->setSizeHint(QSize(item->sizeHint().width(), 70));
                            //item->setSizeHint(QSize(90, 70));
                #endif
                #if defined(Q_OS_MAC)
                            item->setSizeHint(QSize(90, 70));
                #endif

                            this->listWidget_exist->addItem(item);

                            QWidget *line_bottom_1 = new QWidget();
                            line_bottom_1->setFixedHeight(70);
                            line_bottom_1->setStyleSheet("background-color:#707070;");


                            //this->listWidget_exist->setItemWidget(item,this->existing_playlist[idx + i]);
                            //this->setDataTrackInfo_RoseListEdit_Item_exist(sndData, idx + i);
                            //this->listWidget_exist->setItemWidget(item,widget_list_exist.at(idx + i));

                            //--------------------------------------------------------

                            QCoreApplication::processEvents();
                        }
                    }
                }
            }
        }

        if(this->track_count > this->existing_array.size()){

            if(this->track_count > this->create_count){
                int idx = this->create_count;
                int maxCount = 0;
                if(this->track_count - idx > 20){
                    this->create_count += 20;
                    maxCount = this->create_count;
                }
                else{
                    this->create_count += (this->track_count - this->create_count);
                    maxCount = this->create_count;
                }

                for(int i = idx; i < maxCount; i++){
                    QCoreApplication::processEvents();

                    this->existing_playlist[i] = new PlaylistTrackDetailInfo_RHV();
                    //connect(this->existing_playlist[i], &PlaylistTrackDetailInfo_RHV::clicked, this, &AbstractAddPlaylist_RHV::slot_clickedItemTrack_inList);
                    this->existing_playlist[i]->setProperty("index", i);
                }
            }

            this->pageCNT++;

            NetworkHttp *network = new NetworkHttp(this);
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

            QJsonObject tmp_json;
            network->request(HTTP_NETWORK_PLAYLIST_FETCH, QString("%1/member/playlist/%2?page=%3&size=20")
                             .arg(global.legacy_v1).arg(this->playlist_id).arg(this->pageCNT), tmp_json, false, true);
        }
    }
}


void AbstractAddPlaylist_RHV::paint_imageBig(QPixmap &pixmap){
    // 빈 Pixmap
    //print_debug();
    QPixmap pixmap_painter = QPixmap(QSize(315, 315));
    pixmap_painter.fill(Qt::transparent);

    QPainter painter (&pixmap_painter);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path = QPainterPath();
    path.addRoundedRect(0, 0, 315, 315, 2, Qt::RelativeSize);

    painter.setClipPath(path);
    painter.drawPixmap(116, 116, pixmap);
    painter.end();

    this->lb_image->setPixmap(pixmap_painter);
}


void AbstractAddPlaylist_RHV::setImage(QString imagePath){
    // 이미지 경로 설정
    print_debug();
    qDebug() << "imagePath = " << imagePath;
    if(imagePath.isEmpty() == false){
         this->fileDownLoader->setImageURL(QUrl(imagePath));
    }
    else{
        this->paint_imageBig(*this->pixmap_albumImg_default);
    }
}


void AbstractAddPlaylist_RHV::setChange_title_text(const QString &title){

    QString tmp_title = title;

    if(title.length() > 50){
        tmp_title = tmp_title.chopped(1);
        this->le_title->setText(tmp_title);

        ToastMsg::show(this, "", tr("The title can be up to 50 characters in length."));
    }

    QString tmpText = QString("%1/50" + tr("Charaters")).arg(tmp_title.length());
    this->lb_title->setText(tmpText);
}


void AbstractAddPlaylist_RHV::setChange_content_text(){

    QString tmp_title = this->te_content->toPlainText();

    if(this->te_content->toPlainText().size() > 500){

        tmp_title = tmp_title.chopped(1);
        this->te_content->setText(tmp_title);

        ToastMsg::show(this, "", tr("The comment can be up to 500 characters."));
    }

    QString tmpText = QString("%1/500" + tr("Charaters")).arg(tmp_title.length());
    this->lb_content->setText(tmpText);
}


void AbstractAddPlaylist_RHV::slot_btnPublic_clicked(){

    QString obj_name = sender()->objectName();

    this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_on.png", "icon_che_off.png"));
    this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_on.png", "icon_che_off.png"));
    this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_on.png", "icon_che_off.png"));

    this->flag_all_enable = false;
    this->flag_freind_enable = false;
    this->flag_private_enable = false;

    if(obj_name == "btnAll"){
        this->flag_all_enable = true;
        this->btn_all->setStyleSheet(this->btn_all->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else if(obj_name == "btnFreind"){
        this->flag_freind_enable = true;
        this->btn_freind->setStyleSheet(this->btn_freind->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }
    else if(obj_name == "btnPrivate"){
        this->flag_private_enable = true;
        this->btn_private->setStyleSheet(this->btn_private->styleSheet().replace("icon_che_off.png", "icon_che_on.png"));
    }

}

void AbstractAddPlaylist_RHV::slot_btnTags_pressed(){
    int idx = sender()->property("idx").toInt();

    this->btn_tag[idx]->setStyleSheet("background-color:#BE905F;color:#FFF;font-size:16px;font-weight:300;border:1px solid #BE905F;border-radius:15px;");

    QString tag_find = this->le_tag->text();
    if(!tag_find.contains(this->tagTypeList[idx])){
        if(tag_find.isEmpty()){
            tag_find += this->tagTypeList[idx];
            this->le_tag->setText(tag_find);
        }
        else{
            tag_find = tag_find + ", " + this->tagTypeList[idx];
            this->le_tag->setText(tag_find);
        }
    }
}


void AbstractAddPlaylist_RHV::slot_btnTags_clicked(){
    int idx = sender()->property("idx").toInt();

    this->btn_tag[idx]->setStyleSheet("background-color:transparent;color:#FFF;font-size:16px;font-weight:300;border:1px solid #707070;border-radius:15px;");
}


void AbstractAddPlaylist_RHV::slot_changedSubTabUI(const QJsonObject &p_data){

    QString tmp_step = p_data[KEY_PAGE_CODE].toString();

    this->contentStep = tmp_step;

    // Tab 타이틀 변경
    this->menubar->setSelectedSubMenuNoSignal(contentStep);

    int height = 0;
    if(contentStep == ALBTAB_STEP_ADDED){
        this->btn_del->show();
        this->btn_del->setEnabled(true);//c0217
        if(this->added_array.size() < 4){
            height = 210;
        }
        else{
            height = 70 * this->added_array.size();
        }

        this->stackedwidget->setCurrentIndex(0);
        this->stackedwidget->setFixedHeight(height);
    }
    else if(contentStep == ALBTAB_STEP_EXISTING){
        this->btn_del->hide();
        //this->btn_del->setEnabled(false);//c0217
        if(this->track_count < 4){
            height = 210;
        }
        else{
            height = 70 * this->track_count;
        }

        this->stackedwidget->setCurrentIndex(1);
        this->stackedwidget->setFixedHeight(height);
    }
}


void AbstractAddPlaylist_RHV::slot_clicked_btnStream(){

    if(this->flag_stream_enable == false){
        this->flag_stream_enable = true;

        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_on.png");
        this->lb_listCheck_icon->setPixmap(*pixmap_icon);
        this->lb_listCheck_icon->resize(pixmap_icon->width(), pixmap_icon->height());

        //this->btn_del->setEnabled(this->flag_stream_enable);
        this->btn_selectList->setEnabled(this->flag_stream_enable);
    }
    else if(this->flag_stream_enable == true){
        this->flag_stream_enable = false;

        QPixmap *pixmap_icon = GSCommon::getUIPixmapImg(":/images/rosehome/list_check_off.png");
        this->lb_listCheck_icon->setPixmap(*pixmap_icon);
        this->lb_listCheck_icon->resize(pixmap_icon->width(), pixmap_icon->height());

        //this->btn_del->setEnabled(this->flag_stream_enable);
        this->btn_selectList->setEnabled(this->flag_stream_enable);
    }

}

/*
void AbstractAddPlaylist_RHV::slot_clicked_btnSelect(){

    if(this->flag_stream_enable == true){

        if(contents_type == ContentsType::Qobuz){
            qobuz::Dialog_ChoosePlaylist_forQobuz *dialog_chooslPlaylist = new qobuz::Dialog_ChoosePlaylist_forQobuz(this);
            connect(dialog_chooslPlaylist, &qobuz::Dialog_ChoosePlaylist_forQobuz::signal_choosePlaylist_forQOBUZ, this, &AbstractAddPlaylist_RHV::slot_selected_playlist);

            int result = dialog_chooslPlaylist->exec();

            if(result == QDialog::Accepted){
                delete dialog_chooslPlaylist;
            }
        }
        else if(contents_type == ContentsType::Tidal){
            tidal::Dialog_ChoosePlaylist_forTidal *dialog_chooslPlaylist = new tidal::Dialog_ChoosePlaylist_forTidal(this);
            connect(dialog_chooslPlaylist, &tidal::Dialog_ChoosePlaylist_forTidal::signal_choosePlaylist_forTIDAL, this, &AbstractAddPlaylist_RHV::slot_selected_playlist);

            int result = dialog_chooslPlaylist->exec();

            if(result == QDialog::Accepted){
                delete dialog_chooslPlaylist;
            }
        }
    }
}


void AbstractAddPlaylist_RHV::slot_selected_playlist(QString playlist_id_choose, QString playlist_title, QString playlist_description){

    if(playlist_id_choose != 0){
        this->selected_playlist_id = playlist_id_choose;
        this->selected_playlist_name = playlist_title;
        this->selected_playlist_description = playlist_description;

        if(!this->selected_playlist_name.isEmpty()){
            QLabel *tmp_name = new QLabel();
            tmp_name->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            tmp_name->setText(this->selected_playlist_name);

            if(tmp_name->geometry().width() > 190){
                this->lb_selectList->setText(GSCommon::getTextCutFromLabelWidth(this->selected_playlist_name, 180, this->lb_selectList->font()));
            }
            else{
                this->lb_selectList->setText(this->selected_playlist_name);
            }
        }
    }
    else{
        this->selected_playlist_name = playlist_title;
        this->selected_playlist_description = playlist_description;

        if(!this->selected_playlist_name.isEmpty()){
            QLabel *tmp_name = new QLabel();
            tmp_name->setStyleSheet("background-color:transparent;font-size:16px;color:#FFFFFF;");
            tmp_name->setText(this->selected_playlist_name);

            if(tmp_name->geometry().width() > 190){
                this->lb_selectList->setText(GSCommon::getTextCutFromLabelWidth(this->selected_playlist_name, 180, this->lb_selectList->font()));
            }
            else{
                this->lb_selectList->setText(this->selected_playlist_name);
            }

            if(contents_type == ContentsType::Qobuz){

                qobuz::ProcCommon *procQobuz = new qobuz::ProcCommon(this);
                connect(procQobuz, &qobuz::ProcCommon::completeReq_create_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_create_qobuzPlaylist);
                procQobuz->request_qobuz_create_myPlaylist(this->selected_playlist_name, this->selected_playlist_description);
            }
            else if(contents_type == ContentsType::Tidal){

                tidal::ProcCommon *procTidal = new tidal::ProcCommon(this);
                connect(procTidal, &tidal::ProcCommon::completeReq_create_myPlaylist, this, &AbstractAddPlaylist_RHV::slot_create_tidalPlaylist);
                procTidal->request_tidal_create_myPlaylist(global.user_forTidal.getUserId(), this->selected_playlist_name);
            }
        }
    }
}

*/

void AbstractAddPlaylist_RHV::slot_create_qobuzPlaylist(const int playlist_id){

    if(playlist_id > 0){
        this->selected_playlist_id = QString("%1").number(playlist_id);
    }
}


void AbstractAddPlaylist_RHV::slot_create_tidalPlaylist(const QString uuid){

    if(!uuid.isEmpty()){
        this->selected_playlist_id = uuid;
    }
}


void AbstractAddPlaylist_RHV::slot_addTracks_qobuzPlaylist(const int tracks_count){

    if(tracks_count > 0){
        this->flag_addTracks_qobuz = true;
    }

    if(this->flag_addTracks_rose == true){
        emit linker->signal_addPlaylistFinish();
    }
}


void AbstractAddPlaylist_RHV::slot_addTracks_tidalPlaylist(const bool flag){

    if(flag == true){
        this->flag_addTracks_tidal = true;
    }

    if(this->flag_addTracks_rose == true){
        emit linker->signal_addPlaylistFinish();
    }
}


void AbstractAddPlaylist_RHV::slot_create_rosePlaylist(const QJsonObject &p_jsonData){

    int playlist_id = 0;

    if(p_jsonData.contains("id")){
        playlist_id = ProcJsonEasy::getInt(p_jsonData, "id");


        QJsonArray tracks;
        for(int i = 0; i < this->added_array.size(); i++){
            //QJsonObject tmp_data = this->added_array.at(i).toObject();
            print_debug();
            qDebug() << "intList_editPositon.at(i) = " << intList_editPositon.at(i);
            QJsonObject tmp_data = this->added_array.at(intList_editPositon.at(i)).toObject();


            QString clientKey = "";
            QString type = "";
            QString thumbnail = "";
            QString title = "";
            int duration = 0;

            QJsonObject data = QJsonObject();

            if(contents_type == ContentsType::Qobuz){

                clientKey = QString("%1").arg(ProcJsonEasy::getInt(tmp_data, "id"));
                title = ProcJsonEasy::getString(tmp_data, "title");
                qDebug() << "title = " << title;
                type = "QOBUZ";
                duration = ProcJsonEasy::getInt(tmp_data, "duration");

                QJsonObject tmp_album = ProcJsonEasy::getJsonObject(tmp_data, "album");
                QJsonObject tmp_image = ProcJsonEasy::getJsonObject(tmp_album, "image");
                thumbnail = ProcJsonEasy::getString(tmp_image, "large");

                QJsonObject artist;
                QJsonObject tmp_artist = ProcJsonEasy::getJsonObject(tmp_data, "artist");
                QJsonArray tmpArr = ProcJsonEasy::getJsonArray(tmp_artist, "list_artist_albums_count");
                artist.insert("albums_count", tmpArr.at(0).toInt());
                tmpArr = ProcJsonEasy::getJsonArray(tmp_artist, "list_artist_id");
                artist.insert("id", tmpArr.at(0).toInt());
                tmpArr = ProcJsonEasy::getJsonArray(tmp_artist, "list_artist_name");
                artist.insert("name", tmpArr.at(0).toString());


                tmp_data.remove("artist");

                data = tmp_data;
                data.insert("artist", artist);
            }
            else if(contents_type == ContentsType::Tidal){
                clientKey = QString("%1").arg(ProcJsonEasy::getInt(tmp_data, "id"));
                title = ProcJsonEasy::getString(tmp_data, "title");
                qDebug() << "title = " << title;
                type = "TIDAL";
                duration = ProcJsonEasy::getInt(tmp_data, "duration");

                QJsonObject tmp_album = ProcJsonEasy::getJsonObject(tmp_data, "album");
                thumbnail = tidal::ProcCommon::convertImagePath(ProcJsonEasy::getString(tmp_album, "cover"), 320);

                data = tmp_data;
            }
            else{

            }

            QJsonObject tmp_track = QJsonObject();
            tmp_track.insert("clientKey", clientKey);
            tmp_track.insert("data", data);
            tmp_track.insert("duration", duration);
            tmp_track.insert("favorite", false);
            tmp_track.insert("ownerId", 0);
            tmp_track.insert("playUrl", clientKey);
            tmp_track.insert("sort", 0);
            tmp_track.insert("star", 0);
            tmp_track.insert("thumbnailUrl", thumbnail);
            tmp_track.insert("title", title);
            tmp_track.insert("type", type);


            tracks.append(tmp_track);
        }

        QJsonObject json;
        json.insert("favorite", false);
        json.insert("favoriteTrack", false);
        json.insert("id", playlist_id);
        json.insert("isRose", true);
        json.insert("ownerId", 0);
        json.insert("star", 0);
        json.insert("thumbup", false);
        json.insert("thumbupCount", 0);
        json.insert("trackCount", 0);
        json.insert("tracks", tracks);

        roseHome::ProcCommon *procRose = new roseHome::ProcCommon(this);
        connect(procRose, &roseHome::ProcCommon::completeReq_addTrack_playlist, this, &AbstractAddPlaylist_RHV::slot_addTracks_rosePlaylist);
        procRose->request_rose_addTrack_myPlaylist(json);
    }
    intList_editPositon.clear();
}


void AbstractAddPlaylist_RHV::slot_addTracks_rosePlaylist(const QJsonObject &p_jsonData){

    if(p_jsonData.contains("code") && (p_jsonData["code"].toString() == "200")){
        print_debug();
        this->flag_addTracks_rose = true;
    }

    if(this->flag_addTracks_tidal == true){
        print_debug();
        emit linker->signal_addPlaylistFinish();
    }
    else if(this->flag_addTracks_qobuz == true){
        print_debug();
        emit linker->signal_addPlaylistFinish();
    }
    else if(this->flag_stream_enable == false){
        print_debug();
        emit linker->signal_addPlaylistFinish();
    }
}





void AbstractAddPlaylist_RHV::slot_clicked_btnCancel(){
    emit linker->signal_addPlaylistFinish();
}
