#include "cdplay/CDplay_Home.h"

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"

#include "widget/dialogconfirm.h"
#include "widget/NoData_Widget.h"
#include "widget/VerticalScrollArea.h"

#include <QDebug>
#include <QScroller>
#include <QScrollBar>


namespace CDplay {

    const int HTTP_SET_QUEUE = 0;

    const int HTTP_CD_LIST_GET = 1;
    const int HTTP_CD_PLAY = 2;
    const int HTTP_CD_STOP = 3;
    const int HTTP_CD_EJECT = 4;


    const QString ALBTAB_STEP_TRACK = "track";
    const QString ALBTAB_STEP_CREDIT = "credit";


    CDplayHome::CDplayHome(QWidget *parent) : CDplay::AbstractCDPlaySubWidget(VerticalScroll_filter, parent) {

        this->linker = Linker::getInstance();

        this->setUIControl_topButton(parent);
        this->flagNeedReload = false;

        if(global.enable_section_left == true){
            global.enable_section_left = false;
        }
    }


    CDplayHome::~CDplayHome(){

        this->deleteLater();
    }


    /**
     * @brief CDplayHome::show_topBtnControl : public show, refresh, eject UI
     */
    void CDplayHome::show_topBtnControl(){

        this->widget_topContrl->show();
    }


    /**
     * @brief CDplayHome::hide_topBtnFavorite : public hide, refresh, eject UI
     */
    void CDplayHome::hide_topBtnControl(){

        this->widget_topContrl->hide();
    }


    /**
     * @brief CDplayHome::setUIControl_topButton : refresh, eject UI 세팅
     */
    void CDplayHome::setUIControl_topButton(QWidget *parent){

        this->widget_topContrl = new QWidget(parent);
        this->widget_topContrl->setFixedSize(460, 70);
        this->widget_topContrl->setGeometry(1190, 0, 0, 0);
        this->widget_topContrl->setObjectName("widget_topContrl");
        this->widget_topContrl->setStyleSheet("#widget_topContrl { background-color:#171717; } ");

        /*QPushButton *btn_stop = new QPushButton(this->widget_topContrl);
        btn_stop->setObjectName("btn_stop");
        btn_stop->setProperty("type", "stop");
        btn_stop->setCursor(Qt::PointingHandCursor);
        btn_stop->setFixedSize(140, 40);
        btn_stop->setGeometry(0, 15, 0, 0);
        btn_stop->setStyleSheet("#btn_stop{background-color:#333333; border-radius:20px;} #btn_stop:hover{background-color:#B18658; border-radius:20px;}");

        QLabel *lb_stop_img = GSCommon::getUILabelImg(":/images/icon_close_gray.png", btn_stop);
        lb_stop_img->setFixedSize(70, 70);
        lb_stop_img->setGeometry(-5, -15, 0, 0);
        lb_stop_img->setStyleSheet("background-color:transparent; border-radius:0px;");

        QLabel *lb_stop_name = new QLabel(btn_stop);
        lb_stop_name->setFixedSize(75, 20);
        lb_stop_name->setGeometry(50, 10, 0, 0);
        lb_stop_name->setAlignment(Qt::AlignCenter);
        lb_stop_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:18px; color:#FFFFFF;");
        lb_stop_name->setText(tr("Stop"));*/


        QPushButton *btn_refresh = new QPushButton(this->widget_topContrl);
        btn_refresh->setObjectName("btn_refresh");
        btn_refresh->setProperty("type", "refresh");
        btn_refresh->setCursor(Qt::PointingHandCursor);
        btn_refresh->setFixedSize(140, 40);
        btn_refresh->setGeometry(160, 15, 0, 0);
        btn_refresh->setStyleSheet("#btn_refresh{background-color:#333333; border-radius:20px;} #btn_refresh:hover{background-color:#B18658; border-radius:20px;}");

        QLabel *lb_refresh_img = GSCommon::getUILabelImg(":/images/cd/icon_refresh.png", btn_refresh);
        lb_refresh_img->setFixedSize(50, 50);
        lb_refresh_img->setGeometry(0, -5, 0, 0);
        lb_refresh_img->setStyleSheet("background-color:transparent; border-radius:0px;");

        QLabel *lb_refresh_name = new QLabel(btn_refresh);
        lb_refresh_name->setFixedSize(75, 20);
        lb_refresh_name->setGeometry(50, 10, 0, 0);
        lb_refresh_name->setAlignment(Qt::AlignCenter);
        lb_refresh_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:18px; color:#FFFFFF;");
        lb_refresh_name->setText(tr("Refresh"));


        QPushButton *btn_eject = new QPushButton(this->widget_topContrl);
        btn_eject->setObjectName("btn_eject");
        btn_eject->setProperty("type", "eject");
        btn_eject->setCursor(Qt::PointingHandCursor);
        btn_eject->setFixedSize(130, 40);
        btn_eject->setGeometry(320, 15, 0, 0);
        btn_eject->setStyleSheet("#btn_eject{background-color:#333333; border-radius:20px;} #btn_eject:hover{background-color:#B18658; border-radius:20px;}");

        QLabel *lb_eject_img = GSCommon::getUILabelImg(":/images/cd/cd_out_icon.png", btn_eject);
        lb_eject_img->setFixedSize(50, 70);
        lb_eject_img->setGeometry(5, -15, 0, 0);
        lb_eject_img->setStyleSheet("background-color:transparent; border-radius:0px;");

        QLabel *lb_eject_name = new QLabel(btn_eject);
        lb_eject_name->setFixedSize(55, 20);
        lb_eject_name->setGeometry(55, 10, 0, 0);
        lb_eject_name->setAlignment(Qt::AlignCenter);
        lb_eject_name->setStyleSheet("background-color:transparent; border-radius:0px; font-size:18px; color:#FFFFFF;");
        lb_eject_name->setText(tr("Eject"));

        //connect(btn_stop, SIGNAL(clicked()), SLOT(slot_clicked_topmenuItem()));
        connect(btn_refresh, SIGNAL(clicked()), SLOT(slot_clicked_topmenuItem()));
        connect(btn_eject, SIGNAL(clicked()), SLOT(slot_clicked_topmenuItem()));
    }


    /**
     * @brief RoseHome::setActivePage
     */
    void CDplayHome::setActivePage(){

        if(this->flagNeedReload == false){

            if(global.user.isValid() == true){

                // 항상 부모클래스의 함수 먼저 호출
                AbstractCDPlaySubWidget::setActivePage();
                this->flagNeedReload = true;

                if(this->flag_draw == true){
                    this->flag_draw = false;
                    this->widget_cd_contents->hide();
                    this->box_contents->removeWidget(this->widget_cd_contents);
                }

                GSCommon::clearLayout(this->box_contents);
                this->box_contents->setAlignment(Qt::AlignTop);

                this->vbox_cd_contents = new QVBoxLayout();
                this->vbox_cd_contents->setSpacing(0);
                this->vbox_cd_contents->setContentsMargins(0, 0, 0, 0);
                this->vbox_cd_contents->setAlignment(Qt::AlignTop);

                this->widget_cd_contents = new QWidget();
                this->widget_cd_contents->setStyleSheet("background:#212121; border:0px;");
                this->widget_cd_contents->setLayout(this->vbox_cd_contents);


                this->box_contents->addWidget(this->widget_cd_contents);
                this->scrollArea_main->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                QJsonArray *p_jsonArray_titlSub = new QJsonArray();
                QJsonObject sub1 { {"name", tr("Track")}, {"code", ALBTAB_STEP_TRACK} };
                //QJsonObject sub2 { {"name", tr("Credit")}, {"code", ALBTAB_STEP_CREDIT} };

                p_jsonArray_titlSub->push_back(sub1);
                //p_jsonArray_titlSub->push_back(sub2);

                this->menubar = new TopMenuBar(TopMenuBar::ShowMenuStyle::AlbumDetail);
                this->menubar->setDataTopMenuBar(QJsonObject(), p_jsonArray_titlSub);

                connect(this->menubar, SIGNAL(changedSubMenu(QJsonObject)), this, SLOT(slot_changedSubTabUI(QJsonObject)));

                this->contentStep = "track";
                this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

                this->album_detail_info = new AbstractImageDetailContents_RHV(AbstractImageDetailContents_RHV::CDPlay_album);
                connect(this->album_detail_info, &AbstractImageDetailContents_RHV::signal_clicked, this, &CDplayHome::slot_detailInfo_btnClicked);

                this->album_detail_info->initView();

                // 앨범의 트랙정보 UI
                this->vl_tracks = new QVBoxLayout();
                this->vl_tracks->setContentsMargins(0,0,0,0);
                this->vl_tracks->setSpacing(0);
                this->vl_tracks->setAlignment(Qt::AlignTop);

                this->widget_tab_tracks = new QWidget();
                this->widget_tab_tracks->setLayout(this->vl_tracks);

                this->stackedwidget = new QStackedWidget();
                this->stackedwidget->addWidget(this->widget_tab_tracks);

                this->vbox_trackList = new QVBoxLayout();
                this->vbox_trackList->setSpacing(0);
                this->vbox_trackList->setContentsMargins(0, 0, 0, 0);
                this->vbox_trackList->setAlignment(Qt::AlignTop);
                this->vbox_trackList->addWidget(this->menubar);
                this->vbox_trackList->addWidget(this->stackedwidget);

                this->setUIControl_requestCD();
            }
        }
    }


    void CDplayHome::setUIControl_requestCD(){//c220721

        ContentLoadingwaitingMsgShow("");

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject tmp_json;
        QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1").arg(global.port) + QString("/%1").arg("cd.list.get");
        network->request(HTTP_CD_LIST_GET, url, tmp_json, true, false);
    }


    void CDplayHome::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){
print_debug();
qDebug() << "p_jsonObj=" << p_jsonObj;
        if(p_id == HTTP_SET_QUEUE){

           if(p_jsonObj.contains("code")){
                //qDebug() << ProcJsonEasy::getString(p_jsonObj, "code");
           }
        }
        else if(p_id == HTTP_CD_LIST_GET){

            if(p_jsonObj.contains("data")){

                QJsonObject data = ProcJsonEasy::getJsonObject(p_jsonObj, "data");
                this->album_detail_info->setData_fromCDplayData(data);

                this->vbox_cd_contents->addWidget(this->album_detail_info);

                this->jsonArray_to_play = ProcJsonEasy::getJsonArray(data, "mArr");

                int maxCnt = this->jsonArray_to_play.count();
                if(maxCnt > 0){
                    for(int i = 0; i < maxCnt; i++){
                        this->album_track_info[i] = new AlbumTrackDetailInfo_RHV();
                        connect(this->album_track_info[i], &AlbumTrackDetailInfo_RHV::clicked, this, &CDplayHome::slot_clickedItemTrack_inList);

                        this->album_track_info[i]->setProperty("index", i);
                        this->album_track_info[i]->setProperty("type", "playtrack");
                        this->album_track_info[i]->setDataTrackInfo_CDplay(this->jsonArray_to_play.at(i).toObject());

                        this->vl_tracks->addWidget(this->album_track_info[i]);
                    }

                    this->vbox_cd_contents->addLayout(this->vbox_trackList);
                    this->vbox_cd_contents->addSpacing(30);
                }
                else{
                    NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::CDplay_NoData);
                    noData_widget->setFixedSize(1500, 300);

                    this->vbox_cd_contents->addWidget(noData_widget);
                    this->vbox_cd_contents->addSpacing(30);
                }

                this->menubar->setSelectedSubMenuNoSignal(this->contentStep);

                QJsonObject p_data;
                p_data.insert("pageCode", "track");
                slot_changedSubTabUI(p_data);
            }
            else{
                NoData_Widget *noData_widget = new NoData_Widget(NoData_Widget::NoData_Message::CDplay_NoData);
                noData_widget->setFixedSize(1500, 500);

                this->vbox_cd_contents->addWidget(noData_widget);
            }

            this->widget_cd_contents->show();

            this->flag_draw = true;

            this->slot_hide_msg();
        }
        else if(p_id == HTTP_CD_PLAY){
print_debug();
            if(p_jsonObj.contains("code")){//cheon210812-iso
                QString tmp_code = p_jsonObj["code"].toString();
                bool tmp_codeOk = p_jsonObj["flagOk"].toBool();

                if(tmp_code =="G0000" && tmp_codeOk){
                    //emit signal_isoplay(false);
                    //global.isoflag = false;
                    //ToastMsg::show(this, "", tr("play--Select a song again to play CD PLAY"),3000);//cheon210812-iso
                }
                // else global.isoflag = true;
            }
        }
        else if(p_id == HTTP_CD_STOP){
print_debug();
            if(p_jsonObj.contains("code")){//cheon210812-iso
                print_debug();
                QString tmp_code = p_jsonObj["code"].toString();
                bool tmp_codeOk = p_jsonObj["flagOk"].toBool();
                if(tmp_code =="CDPLAY" && tmp_codeOk){
                    print_debug();
                    //qDebug() << "tmp_code-CDPLAYING---";
                    //emit signal_isoplay(true);
                   // global.isoflag = true;
                }
                else if(tmp_code =="G0000" && tmp_codeOk){
                    print_debug();
                            qDebug() << "CDplayHome::slot_responseHttp_stop----tmp_code ==G0000&& tmp_codeOk";
                            //emit signal_isoplay(false);
                           // QTimer::singleShot(2000, this, SLOT(slot_cd_play()));
                           // global.isoflag = false;
                        //   ToastMsg::show(this, "", tr("CD PLAY has stoped. \nPlaese Select a song again to play CD PLAY."),3000);//cheon210812-iso

                }
            }
        }
        else if(p_id == HTTP_CD_EJECT){

            QThread::msleep(5000);

            this->flagNeedReload = false;
            this->setActivePage();
        }

        sender()->deleteLater();
    }


    void CDplayHome::slot_hide_msg(){

        ContentLoadingwaitingMsgHide();
    }


    void CDplayHome::changedOnlyTabUI_notSendSignal(QString p_step){

        this->contentStep = p_step;

        // Tab 타이틀 변경
        this->menubar->setSelectedSubMenuNoSignal(contentStep);

        // DB 가져와서 데이터 세팅
        if(contentStep == ALBTAB_STEP_TRACK){
            this->stackedwidget->setCurrentIndex(0);
            this->stackedwidget->setFixedHeight(this->vl_tracks->sizeHint().height());
        }
        else if(contentStep == ALBTAB_STEP_CREDIT){
            //this->stackedwidget->setCurrentIndex(1);
            //this->stackedwidget->setFixedHeight(this->vl_credit->sizeHint().height());
        }
    }


    void CDplayHome::slot_changedSubTabUI(const QJsonObject &p_data){

        QString tmp_step = p_data[KEY_PAGE_CODE].toString();

        // 페이지 변경 및 데이터 세팅
        this->changedOnlyTabUI_notSendSignal(tmp_step);

    }


    void CDplayHome::slot_clicked_topmenuItem(){

        QString type = sender()->property("type").toString();

        if(global.device.CurrPlayType =="CD" && type == "stop"){

            DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
            dlgConfirmMSG->setTitle(tr("Notice"));
            dlgConfirmMSG->setFixedSize(350, 400);
            dlgConfirmMSG->setAlignment(Qt::AlignCenter);
            dlgConfirmMSG->setTextHeight(150);
            dlgConfirmMSG->setText(tr("Do you want to stop playing?"));

            int result = dlgConfirmMSG->exec();

            if(result == QDialog::Accepted){

                QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1").arg(global.port) + QString("/%1").arg("cd.play.stop");
                QJsonObject tmp_json;

                NetworkHttp *stop = new NetworkHttp;
                connect(stop, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                stop->request(HTTP_CD_STOP, url, tmp_json, true, true);
            }

            delete dlgConfirmMSG;
        }
        else if(type == "refresh"){

            this->flagNeedReload = false;
            this->setActivePage();
        }
        else if(type == "eject"){

            DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
            dlgConfirmMSG->setTitle(tr("Notice"));
            dlgConfirmMSG->setFixedSize(350, 400);
            dlgConfirmMSG->setAlignment(Qt::AlignCenter);
            dlgConfirmMSG->setTextHeight(150);
            dlgConfirmMSG->setText(tr("Do you want to stop playing and eject the CD?"));

            int result = dlgConfirmMSG->exec();

            if(result == QDialog::Accepted){

                QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1").arg(global.port) + QString("/%1").arg("cd.out");
                QJsonObject tmp_json;

                NetworkHttp *eject = new NetworkHttp;
                connect(eject, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                eject->request(HTTP_CD_EJECT, url, tmp_json, false, true);
            }

            delete dlgConfirmMSG;
        }
    }


    /**
     * @brief [Slot] slot_detailInfo_btnClicked signal에 대하여 처리
     * @param clickMode
     */
    void CDplayHome::slot_detailInfo_btnClicked(const AbstractImageDetailContents_RHV::BtnClickMode clickMode){

        if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAdd || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx2 ||
                clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toAddx3 || clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Favorite_toDelete){

        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::More){

        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::Info){

        }
        else if(clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayAll){

            QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1").arg(global.port) + QString("/%1").arg("cd.play");

            QJsonObject tmp_json;
            tmp_json.insert("position", 1);

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_CD_PLAY, url, tmp_json, true, true);
        }
        else if(global.device.CurrPlayType =="CD" && clickMode == AbstractImageDetailContents_RHV::BtnClickMode::PlayShuffle){

            DialogConfirm *dlgConfirmMSG = new DialogConfirm(this);
            dlgConfirmMSG->setTitle(tr("Notice"));
            dlgConfirmMSG->setFixedSize(350, 400);
            dlgConfirmMSG->setAlignment(Qt::AlignCenter);
            dlgConfirmMSG->setTextHeight(150);
            dlgConfirmMSG->setText(tr("Do you want to stop playing?"));

            int result = dlgConfirmMSG->exec();

            if(result == QDialog::Accepted){

                QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1").arg(global.port) + QString("/%1").arg("cd.play.stop");
                QJsonObject tmp_json;

                NetworkHttp *stop = new NetworkHttp;
                connect(stop, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
                stop->request(HTTP_CD_STOP, url, tmp_json, true, true);
            }

            delete dlgConfirmMSG;
        }
    }


    /**
     * @brief [Slot] slot_clickedItemTrack_inList signal에 대하여 처리
     * @param clickMode
     */
    void CDplayHome::slot_clickedItemTrack_inList(const int idx, const AlbumTrackDetailInfo_RHV::ClickMode clickMode){

        // ClickMode 별로 처리
        if(clickMode == AlbumTrackDetailInfo_RHV::ClickMode::AllBox){

            QString url = QString("http://%1").arg(global.device.getDeviceIP()) + QString(":%1").arg(global.port) + QString("/%1").arg("cd.play");

            QJsonObject tmp_json;
            tmp_json.insert("position", idx + 1);

            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
            network->request(HTTP_CD_PLAY, url, tmp_json, true, true);

            //QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"CDplayHome::playMusic---" << strJson;
        }
    }
}
