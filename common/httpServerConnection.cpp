#include "common/httpServerConnection.h"

#include "common/global.h"

#include "widget/toastmsg.h"

#include <QDebug>
#include <QTcpSocket>
#include <QRegularExpression>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QLabel>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QWidget>
#include <QTimer>



    QString Music = "MUSIC";
    QString VIDEO = "VIDEO";
    QString RADIO = "RADIO";
    QString ROSE_RADIO = "ROSE_RADIO";
    QString FM_TUNER = "FM_TUNER";
    QString YOUTUBE = "YOUTUBE";
    QString POD = "POD";
    QString SPOTIFY = "SPOTIFY";
    QString TIDAL = "TIDAL";
    QString TIDAL_VIDEO = "TIDAL_VIDEO";
    QString BUGS = "BUGS";
    QString BUGS_MV = "BUGS_MV";
    QString QOBUZ = "QOBUZ";                            ///< Added Jeon 31/12/2020
    QString APPLE = "APPLE_MUSIC";
    QString ROON = "ROON";
    QString DLNA = "DLNA";
    QString AIRPLAY = "AIRPLAY";
    QString BT = "BT";
    QString CD = "CD";                                  ///< CD재생시


httpServerConnection::httpServerConnection(QTcpSocket *socket, QWidget *parent) :
    QWidget(parent),
    m_socket(socket)
{

    this->showMessageDialog = new QDialog();

    linker = Linker::getInstance();//cheon210617-http
    connect(m_socket.get(), &QAbstractSocket::disconnected, this, &QObject::deleteLater);
    connect(m_socket.get(), &QIODevice::readyRead, this, &httpServerConnection::readyRead);
}


httpServerConnection::~httpServerConnection(){

    this->deleteLater();
}


void httpServerConnection::readyRead()
{
/*
    while(m_socket->canReadLine())
    {
        qDebug() << m_socket->readLine();
    }
    */

    while((m_state == ConnectionState::RequestLine || m_state == ConnectionState::RequestHeaders) &&
          m_socket->canReadLine())
    {
        auto line = m_socket->readLine();
        if(line.endsWith("\r\n"))
            line.chop(2);

        //qDebug() << "socket_test_01 : " << line;
        if(m_state == ConnectionState::RequestLine)
        {
            const auto parts = line.split(' ');
            if(parts.size() != 3)
            {
                qWarning() << "Request line dosen't consist of 3 parts:" << line;
                deleteLater();
                return;
            }

            m_request.method = parts.at(0);
            m_request.path = parts.at(1);
            m_request.protocol = parts.at(2);

          //  qDebug() <<   "socket_test_02 : " << m_request.method << m_request.path << m_request.protocol;

            m_state = ConnectionState::RequestHeaders;

            continue;
        }
        else if(m_state == ConnectionState::RequestHeaders)
        {
            if(line.isEmpty())
            {
                if(m_request.contentLength)
                {
                    m_state = ConnectionState::RequestBody;
               //     qDebug() << "---ConnectionState::RequestHeaders";
                    continue;
                }
                else
                {
                //    qWarning() << "Request line dosen't consist of request headers:" << line;
                    return;;
                }
            }

            static const QRegularExpression expr("^([^:]+): +(.*)$");
            const auto match = expr.match(line);
            if(!match.hasMatch())
            {
                qWarning() << "could not parse hader:" << line;
                deleteLater();
                return;
            }

            const auto name = match.captured(1);
            const auto value = match.captured(2);

            m_request.headers[name] = value;
          //  qDebug() << "---value" << value;

            if(name == "Content-Length")
            {
                bool ok;
                m_request.contentLength = value.toInt(&ok);
                if(!ok)
                {
                    qWarning() << "could not parse Content-Length:" << line;
                    deleteLater();
                    return;
                }
            }

            continue;
        }
        else if(m_state == ConnectionState::RequestBody){
            qDebug() << "---ConnectionState::RequestBody";

        }
    }


    if(m_state == ConnectionState::RequestBody)
    {
        //qDebug() << "---ConnectionState::RequestBody02";
        QString str = m_socket->read(m_request.contentLength - m_request.body.size());
        qDebug() << "---- str=" << str;

        m_request.body.append(str);
        Q_ASSERT(m_request.body.size() <= m_request.contentLength);
        if(m_request.body.size() ==  m_request.contentLength)
        {
            m_state = ConnectionState::Response;
            QByteArray br = str.toUtf8();

            QJsonDocument doc = QJsonDocument::fromJson(br);

            QJsonObject tmp_jsonobj = doc.object();
            setRequestBody(tmp_jsonobj);
         //   qDebug() << "---ConnectionState::RequestBody03";
            return;
        }
    }

}

void httpServerConnection::setRequestBody(QJsonObject p_jsonObject){//c211213l

    print_debug();
    QJsonDocument doc(p_jsonObject);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug() << "httpServerConnection::setRequestBody-------------------" << strJson;

    QJsonObject tmp_Json;

    if(p_jsonObject.contains("dataObj")){

        if(p_jsonObject.contains("messageType")){
            tmp_Json = p_jsonObject["dataObj"].toObject();
            if(p_jsonObject["messageType"].toString() == "show_toast_noti"){
                print_debug();
                qDebug() << "message = " << tmp_Json["message"].toString();
                //ToastMsg::show(this, "", tmp_Json["message"].toString());
                if(global.show_flag == false){
                    //print_debug();
                    global.show_flag =  true;
                    if("The volume can be adjusted when the pre-out level is turned off."== tmp_Json["message"].toString()){
                         ToastMsg::show(this,"", tmp_Json["message"].toString(), 2000, -60, -1);//c220930_2
                    }else{
                         ToastMsg::show(this,"", tmp_Json["message"].toString(), 2000, -60);//c220930_2
                    }

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    if("The volume can be adjusted when the pre-out level is turned off."== tmp_Json["message"].toString()){
                         ToastMsg::show(this,"", tmp_Json["message"].toString(), 2000, 0, -1);//c220930_2
                    }else{
                         ToastMsg::show(this,"", tmp_Json["message"].toString(), 2000, 0);//c220930_2
                    }
                }
            }
        }
        if(global.device.CurrPlayType =="CD"){//c220711
            print_debug();
            global.Queue_track_count = 0;
            emit linker->signal_httpservermsg(p_jsonObject);
        }


    }else{//cheon211203

        print_debug();
        if(global.device.getDeviceIP().isEmpty())  {//c220525
            print_debug();
            return;
        }
        qDebug() << "global.device.getMusicPlayType() = " << global.device.getMusicPlayType();
        if(p_jsonObject.contains("messageType")){
            print_debug();
            qDebug() << "global.indexCurrPlay[global.now_Section]= " << global.indexCurrPlay[global.now_Section];//c220625
            qDebug() << "global.total_queue_list_count= " << global.total_queue_list_count;
            qDebug() << "global.Queue_track_count= " << global.Queue_track_count;
            qDebug() << "global.queue_playFlag= " << global.queue_playFlag;
            qDebug() << "global.queue_position_startFlag= " << global.queue_position_startFlag;
            qDebug() << "global.queuelist_mouse_trigger_menu_flag= " << global.queuelist_mouse_trigger_menu_flag;
            QString messageType = p_jsonObject["messageType"].toString();
            qDebug() << "messageType=" << messageType;


            if(global.queueTimerFlag &&!global.queuelist_mouse_trigger_menu_flag){
                print_debug();
                ToastMsg::show(this,"", tr("Added to device queue. Please wait. Downloading Queuelist."), 3000, 0);
                //emit linker->signal_queuelist_mouse_trigger_menu_flag();
                return;
            }else{


                print_debug();
            }

            if(messageType == "music_start"  ){////c220716
//global.onOffModeFlag
                /*//c220716
                if(!global.queueTimerFlag){
                    print_debug();
                    if(global.onOffModeFlag && global.indexCurrPlay[global.now_Section] == global.total_queue_list_count-1 && global.Queue_track_count == 0  && !global.queue_recommPlayFlag && !global.queue_queuePlayFlag ){//c220625
                        print_debug();
                        global.autoRecommCount++;
                    }
                    global.autoRecommCount += global.Queue_track_count;
                    global.Queue_track_count = 0;
                }
                print_debug();
                //if((global.onOffModeFlag && (global.queue_playFlag || global.queue_recommPlayFlag)) && global.queueTimerFlag && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){
                if(global.queueTimerFlag && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){

                    emit linker->signal_queuelist_musicStart();//c220704
                    //global.queue_music_startFlag = true;
                    //--------------------------------------
                }
                */
                if(global.show_flag == false){//c221001_1
                    //print_debug();
                    global.show_flag =  true;
                     ToastMsg::show(this,"", tr("music start"), 2000, -60);//c220930_2

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    ToastMsg::show(this,"", tr("music start"), 2000, 0);//c220930_2
                }

            }
            //else if(global.device.getMusicPlayType() != 16 &&  messageType == "queue_play" && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){
            else if(global.Queue_track_count > 0 && (global.device.getMusicPlayType() == 19 || global.device.getMusicPlayType() == 16 || global.device.getMusicPlayType() == 13 || global.device.getMusicPlayType() == 12) &&  messageType == "queue_play" && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){//c220826_2
                    print_debug();
                    if(!global.queueTimerFlag){
                        global.Queue_track_count = 0;
                    }
                    if(global.queueTimerFlag){
                        emit linker->signal_queuelist_musicStart();
                    }



            }else if( messageType == "queue_recent_track_add" && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){
                print_debug();
                if(global.queueTimerFlag){
                    //emit linker->signal_queuelist_reload();//cheon211206
                    //emit linker->signal_queuelist_musicStart();//c220826_2
                    global.queue_recent_track_addFlag = true;
                }
                if(global.show_flag == false){//c221001_1
                    //print_debug();
                    global.show_flag =  true;
                     ToastMsg::show(this,"", tr("A queue was recently added."), 2000, -60);//c220930_2

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    ToastMsg::show(this,"", tr("A queue was recently added."), 2000, 0);//c220930_2
                }
            }else if( messageType == "recommand_data_change" && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){
                print_debug();
                if(global.queueTimerFlag){
                    emit linker->signal_recommand_data_change();//c220826_2
                    //emit linker->signal_queuelist_reload();//cheon211206
                    //global.queue_position_startFlag = false;
                }
                if(global.show_flag == false){//c221001_1
                    //print_debug();
                    global.show_flag =  true;
                     ToastMsg::show(this,"", tr("Recommand queue list  changed."), 2000, -60);//c220930_2

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    ToastMsg::show(this,"", tr("Recommand queue list  changed."), 2000, 0);//c220930_2
                }


            }else if(messageType == "current_play_state_refresh" && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){
                print_debug();
                if(!global.queueTimerFlag){
                    //global.Queue_track_count = 0;
                }
                if(global.queueTimerFlag){
                    //emit linker->signal_queue_recent_track_add();//c220418
                    //global.queue_recent_track_addFlag = true;
                    //----emit linker->signal_queuelist_musicStart();//c220411
                    global.queue_music_startFlag = true;
                }


            }
            else if( messageType == "queue_play" && (global.device.getMusicPlayType() != 19 || global.device.getMusicPlayType() != 16 || global.device.getMusicPlayType() != 13 || global.device.getMusicPlayType() != 12) &&  messageType == "queue_play" && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){//c220826_2
                print_debug();
                /*//c220716
                qDebug() << "global.desktopPlayerMode=" << global.desktopPlayerMode;
                qDebug() << "global.desktopPlayerMode=" << global.desktopPlayerMode;
                if(global.queueTimerFlag){
                     emit linker->signal_queuelist_musicStart();//c220627
                }
                if(global.desktopPlayerMode){
                    emit linker->signal_deviceStart();//c220609
                }
                */
                if(!global.queueTimerFlag){//c220826_2
                    print_debug();
                    //if(global.onOffModeFlag && global.indexCurrPlay[global.now_Section] == global.total_queue_list_count-1 && global.Queue_track_count == 0  && !global.queue_recommPlayFlag && !global.queue_queuePlayFlag ){//c220625
                    //    print_debug();
                    //    global.autoRecommCount++;
                    //}
                    //global.autoRecommCount += global.Queue_track_count;
                    global.Queue_track_count = 0;
                }
                print_debug();
                //if((global.onOffModeFlag && (global.queue_playFlag || global.queue_recommPlayFlag)) && global.queueTimerFlag && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){
                if(global.queueTimerFlag && global.device.CurrPlayType != POD && global.device.CurrPlayType != VIDEO && global.device.CurrPlayType != RADIO && global.device.CurrPlayType != ROSE_RADIO){

                   // emit linker->signal_queuelist_musicStart();//c220826_2
                    //global.queue_music_startFlag = true;
                    //--------------------------------------
                }
                if(global.show_flag == false){//c221001_1
                    //print_debug();
                    global.show_flag =  true;
                     ToastMsg::show(this,"", tr("It is played from the queue list."), 2000, -60);//c220930_2

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    ToastMsg::show(this,"", tr("It is played from the queue list."), 2000, 0);//c220930_2
                }

            }

            if(messageType == "play_state_change"){
                print_debug();
                //if(global.queueTimerFlag){//c220521
                //emit linker->signal_checkQueue(23, "Play state changed");//c221001_1
                if(global.show_flag == false){//c221001_1
                    //print_debug();
                    global.show_flag =  true;
                     ToastMsg::show(this,"", tr("Play state changed."), 2000, -60);//c220930_2

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    ToastMsg::show(this,"", tr("Play state changed."), 2000, 0);//c220930_2
                }
                linker->signal_reload_setting_clicked("inputOutput");//c221020_1
                //emit linker->signal_device_connect_check_error();
                //emit linker->signal_queuelist_reload();//cheon211206
                //global.queue_position_startFlag = true;
                //}

            }
            if(messageType == "state_check"){
                print_debug();
                //if(global.queueTimerFlag){//c220521
                emit linker->signal_checkQueue(28, "");//c220624
                //emit linker->signal_checknetwork();//c220715
                    //emit linker->signal_device_connect_check_error();//c220526
                    //emit linker->signal_queuelist_reload();//cheon211206
                    //global.queue_position_startFlag = true;
                //}
            }
            if(messageType == "music_play_type_change"){//c220407
                print_debug();
                //if(global.queueTimerFlag){

                    //emit linker->signal_httpservermsg_music_play_type_change();//c220407
                    //emit linker->signal_queuelist_reload();//cheon211206
                    //global.queue_position_startFlag = true;
                //}
                emit linker->signal_httpservermsg(p_jsonObject);
            }

            if(messageType == "login_state_change" || messageType == "user_login_noti" || messageType == "user_logout_noti"){
                print_debug();
               emit linker->signal_httpservermsg(p_jsonObject);
                if(global.show_flag == false){//c221001_1
                    //print_debug();
                    global.show_flag =  true;
                     ToastMsg::show(this,"", tr("Check your login status."), 2000, -60);//c220930_2

                }else{
                    //print_debug();
                    global.show_flag =  false;
                    ToastMsg::show(this,"", tr("Check your login status."), 2000, 0);//c220930_2
                }
            }
        }
    }
}


void httpServerConnection::msgshow(const QString &p_msgText, int duration){

    QLabel *lb_msg = new QLabel();
    lb_msg->setText(p_msgText);
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


    /*QDialog *dialog = new QDialog();
    dialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");

    dialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    dialog->setAttribute(Qt::WA_TranslucentBackground);
    dialog->setLayout(vl_total);

   // int duration = MSG_DURATION;
    QPropertyAnimation* anim_hide = new QPropertyAnimation(dialog, "windowOpacity");
    anim_hide->setStartValue(1.0);
    anim_hide->setEndValue(0.3);
    anim_hide->setDuration(duration);
    anim_hide->start(QAbstractAnimation::DeleteWhenStopped);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    dialog->move(dialog->pos().x(), (global.height_mainwindow - dialog->height() - 100));

    QTimer::singleShot(duration, dialog, SLOT(hide()));*/

    this->showMessageDialog->setStyleSheet("background-color:#55FF0000;border-radius:20px;");

    this->showMessageDialog->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    this->showMessageDialog->setAttribute(Qt::WA_TranslucentBackground);
    this->showMessageDialog->setLayout(vl_total);

   // int duration = MSG_DURATION;
    QPropertyAnimation* anim_hide = new QPropertyAnimation(this->showMessageDialog, "windowOpacity");
    anim_hide->setStartValue(1.0);
    anim_hide->setEndValue(0.3);
    anim_hide->setDuration(duration);
    anim_hide->start(QAbstractAnimation::DeleteWhenStopped);

    this->showMessageDialog->show();
    this->showMessageDialog->raise();
    this->showMessageDialog->activateWindow();
    //this->showMessageDialog->move(this->showMessageDialog->pos().x(), (global.height_mainwindow - this->showMessageDialog->height() - 100));
    this->showMessageDialog->move(((global.width_mainwindow - this->showMessageDialog->width()) / 2), (global.height_mainwindow - this->showMessageDialog->height() - 100));

    if(global.show_flag == false){
        global.show_flag = true;
    }

    QTimer::singleShot(1900, this, SLOT(msghide()));
}


void httpServerConnection::msghide(){

    if(this->showMessageDialog->isHidden() == false){
        global.show_flag = false;
        this->showMessageDialog->hide();
    }
}
