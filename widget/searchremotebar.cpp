#include "widget/searchremotebar.h"
//#include "home/abstractmaincontent.h"
#include "widget/toastmsg.h"//c220710

#include "common/global.h"
#include "common/gscommon.h"
#include "common/networkhttp.h"
#include "common/sqlitehelper.h"
#include "common/commonGetRedirectUrl.h"    //j220903 twitter

#include <QJsonArray>


const int MAX_LENGTH_SEARCH = 200;//c220818
const int SEARCHBAR_W = 577;
//const int SEARCHBAR_W = 77;
const int SEARCHBAR_H = 69;
const int LINEEDIT_MIN_WIDTH = 77;


const int HTTP_NETWORK_SUGGESTION = 3;

/**
 * @brief SearchRemoteBar::SearchRemoteBar : 공통 상단 - 검색 및 리모컨
 * @param parent
 */
SearchRemoteBar::SearchRemoteBar(QWidget *parent) : QWidget(parent)
{
    setInit();
    setUIControl();
}


SearchRemoteBar::~SearchRemoteBar(){

    this->deleteLater();
}


void SearchRemoteBar::setInitUIRemoteBtn(){

    if(this->flagSelectedRemote==true){
        this->flagSelectedRemote = false;
        QString tmp_style = this->btn_remote->styleSheet();
        tmp_style.replace("/images/icon_remote_gold.png", "/images/icon_remote.png");
        this->btn_remote->setStyleSheet(tmp_style);
    }
}


void SearchRemoteBar::setSelectedRemoteBtnStyle(){

        this->flagSelectedRemote = true;
        QString tmp_style = this->btn_remote->styleSheet();
        tmp_style.replace("/images/icon_remote.png", "/images/icon_remote_gold.png");
        this->btn_remote->setStyleSheet(tmp_style);
}


/**
 * @brief SearchRemoteBar::setInit : 초기 세팅
 */
void SearchRemoteBar::setInit(){

    linker = Linker::getInstance();
    pixmapDelete.load(":/images/icon_close_gray.png");//c220715
}

/**
 * @brief SearchRemoteBar::setUIControl : UI 세팅
 */
void SearchRemoteBar::setUIControl(){

    // -------------------------------------------------
    // 검색 입력 및 리모컨 버튼
    // -------------------------------------------------
    this->le_search = new CustomLineEdit();
    this->le_search->setObjectName("le_search");
    this->le_search->setStyleSheet("#le_search { background-color:white; border-radius:20px; font-size:15px; color:#333333; padding-left:35px;}");
    this->le_search->setMaxLength(MAX_LENGTH_SEARCH);
    this->le_search->setFixedSize(SEARCHBAR_W-LINEEDIT_MIN_WIDTH,40);
    this->le_search->setFocus(Qt::FocusReason::ActiveWindowFocusReason);//c220425

    this->btn_remote = GSCommon::getUIBtnImg("btn_remote", ":/images/icon_remote.png");
    this->btn_remote->setFixedSize(80,70);
    this->btn_remote->setCursor(Qt::PointingHandCursor);

    QLabel *lb_search  = GSCommon::getUILabelImg(":/images/icon_ser.png", this->le_search);
    lb_search->setStyleSheet("background-color:rgba(0,0,0,0);margin-bottom:30px;");
    lb_search->show();

    btn_del = new ClickableLabel(this->le_search);
    btn_del->setImageResource(":/images/icon_close_gray.png");
    btn_del->setStyleSheet("background-color:#B18658;border-radius:15px;");
    btn_del->setFixedSize(40,40);
    btn_del->setCursor(Qt::PointingHandCursor);
    btn_del->setGeometry(le_search->size().width()-60, 15, 40, 40);
    btn_del->setStyleSheet("background-color:rgba(0,0,0,0);margin-bottom:30px;");
    btn_del->hide();

    QHBoxLayout *hlayout_etc = new QHBoxLayout();
    hlayout_etc->setContentsMargins(0,0,0,0);
    hlayout_etc->setSpacing(0);
    hlayout_etc->addWidget(this->le_search);
    hlayout_etc->addWidget(this->btn_remote);

    QWidget *widget_search = new QWidget();
    widget_search->setObjectName("searchRemoteBar");
    widget_search->setLayout(hlayout_etc);
    widget_search->setStyleSheet("background-color:transparent;");

    QVBoxLayout *vl_total = new QVBoxLayout();
    vl_total->setContentsMargins(0,0,0,0);
    vl_total->setSpacing(0);
    vl_total->addWidget(widget_search);

    this->setLayout(vl_total);
    this->setFixedSize(SEARCHBAR_W,SEARCHBAR_H);

    connect(this->btn_del, SIGNAL(signal_clicked()), this, SLOT(clickedRemoteBtn_del()));
    connect(this->btn_remote, SIGNAL(clicked()), this, SLOT(clickedRemoteBtn()));
    connect(this->le_search, SIGNAL(signal_textChanged(QString)), this, SLOT(slot_textChanged(QString)));
    connect(this->le_search, SIGNAL(signal_clickedEnter(QString)), this, SLOT(slot_returnPressed(QString)));
    connect(this->le_search, SIGNAL(signal_clickedDown(QString)), this, SLOT(slot_downPressed(QString)));
    connect(this->le_search, SIGNAL(signal_clickedUp(QString)), this, SLOT(slot_upPressed(QString)));

    connect(this->le_search, SIGNAL(signal_dragDrop_search()), this, SLOT(slot_dragDrop_search_singleshot()));//c220726
    connect(this->le_search, SIGNAL(signal_dragDrop_clear()), this, SLOT(slot_dragDrop_clear_singleshot()));//c220726

    connect(linker, SIGNAL(signal_sugestionbarOld_sel(QString)), this, SLOT(slot_sugestionbarOld_sel(QString)));
    connect(linker, SIGNAL(signal_searchCopy(QString)), this, SLOT(slot_searchCopy(QString)));//c220904_1
    connect(linker, SIGNAL(signal_searchBar_clearfocus()), this, SLOT(slot_searchBar_clearfocus()));//c230220


}


void SearchRemoteBar::slot_searchBar_clearfocus(){//c230220
    print_debug();
    le_search->clearFocus();
}


void SearchRemoteBar::slot_dragDrop_search_singleshot(){//c220726
    print_debug();
    //le_search->displayText();
    //this->slot_get_dragDropText();
    QTimer::singleShot(1000, this, SLOT(slot_get_dragDropText()));

}
void SearchRemoteBar::slot_dragDrop_clear_singleshot(){//c220726
    print_debug();
    //le_search->displayText();
    qDebug() << "Drag-and-drop playback is not supported except for YouTube content.";
    ToastMsg::show(this,"", tr("Drag-and-drop playback is not supported except for YouTube content."), 2000, 0);


    QTimer::singleShot(1000, this, SLOT(slot_clear_dragDropText()));

}
void SearchRemoteBar::slot_clear_dragDropText(){//c220726
    print_debug();
    //220726-tmp le_search->clear();
}
//#include <QXmlQuery>
QString SearchRemoteBar::getUrlResponse(QString tmp_str){//c220818

    QUrl url(QString("%1").arg(tmp_str));
    QNetworkRequest networkRequest(url);
    QNetworkAccessManager manager;
    QEventLoop event;
    QObject::connect(&manager, &QNetworkAccessManager::finished, &event, &QEventLoop::quit);
    auto* const pResponse = manager.get(networkRequest);
    event.exec();
    pResponse->deleteLater();

    //qDebug() << "pResponse->readAll()---" << pResponse->readAll();
    return pResponse->readAll();
}

void SearchRemoteBar::slot_redirectUrl(const QString url){  //j220903 twitter

    if(this->redirectUrl_before != url){
        this->redirectUrl_before = url;
        this->le_search->setText(url);
        this->slot_get_dragDropText();
    }
    else if(this->redirectUrl_before == url){
        this->redirectUrl_before = "";
    }
}


void SearchRemoteBar::slot_get_dragDropText(){//c220930
    print_debug();

    //QString videoID = jsonTrackData["playurl"].toString().split("?v=").last();
    //le_search->displayText();
    qDebug() << "le_search->text()=" << this->le_search->text();
    if(this->le_search->text().size()<2){
        return;
    }

    // Drag and Drop Error fixed 10/17/2022 by diskept
    QString strInput = this->le_search->text();
    QString strRelplace = "";
    QString tmp_str = "";

    if(strInput.contains("\n")){
        QStringList removeNewline = strInput.split("\n");
        strRelplace = removeNewline.at(0);
    }
    else{
        strRelplace = strInput;
    }

    if(strRelplace.contains("\t")){
        QStringList removeTab = strRelplace.split("\t");
        tmp_str = removeTab.at(0);
    }
    else{
         tmp_str = strRelplace;
    }

    tmp_str.replace(" ", "");

    global.dragDrop_pathStr = tmp_str;
    QString tmp_Path4 = tmp_str.split("&").at(0);
    qDebug() << "tmp_Path4=" << tmp_Path4;
    QString tmp_Path1 = tmp_str.split("//").first();
    QString tmp_Path2 = tmp_str.split("//").last();
    QString tmp_Path3 = tmp_str.split("/").last();

    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int, QJsonObject)), SLOT(slot_responseHttp(int, QJsonObject)));

    QJsonObject json;

    //220726-tmp le_search->clear();
    if(tmp_str.contains("https://t.")){ //j220903 twitter

        this->redirectUrl_before = tmp_str;

        CommonGetRedirectUrl *redirectUrl = new CommonGetRedirectUrl("ROSE", tmp_str);
        connect(redirectUrl, &CommonGetRedirectUrl::signal_redirectUrl, this, &SearchRemoteBar::slot_redirectUrl);
    }
    else if(tmp_Path1.contains("http")&& tmp_Path2.size() > 0){
        qDebug() << "tmp_str.split(?v=).last()=" << tmp_str.split("?v=").last();
        if((tmp_str.contains("?v=")&& tmp_str.split("?v=").last().size()>0) || ( tmp_str.contains("/shorts/")&& tmp_str.split("/shorts/").last().size()>0)){
            print_debug();
            if(tmp_str.contains("start_radio")){
                print_debug();
                QStringList tmpPlaylist = tmp_str.split("&");
                qDebug() << "---" << QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmpPlaylist[1]);
                network->request(5678
                                 , QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmpPlaylist[1])
                        , json
                        , false
                        , true);
            }
            else if((tmp_str.contains("&")&& tmp_str.split("&").at(1).size()>0)){
                print_debug();
                //qDebug() << QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmp_str.split("&").at(1));
                //qDebug() << "network->request=" << QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_Path4);
                qDebug() << "network->request=" << QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmp_str.split("&").at(1));
                network->request(5678
                                 // , QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=https://www.youtube.com/playlist?%1").arg(tmp_str.split("&").at(1))
                                 , QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_Path4)
                                 , json
                                 , false
                                 , true);
            }else if(tmp_str.contains("?v=")&& tmp_str.split("?v=").last().size()>0){
                global.search_text = tmp_str.split("?v=").last();
                qDebug() << "global.search_text" << global.search_text;
                //this->slot_returnPressed(tmp_Path3);
                print_debug();
                //emit linker->signal_checkQueue(11, "");
                //slot_overrideSigalSearch(true);
                emit linker->signal_clickedMovePageRoseTubeSearch();//c230228
                print_debug();
                //this->le_search_back->clear();
            }else{
                qDebug() << "network->request=" << QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_str);
                network->request(5678
                                 //, QString("https://api.roseaudio.kr/newpipe/v1/playlist?playlistUrl=%1").arg(tmp_str)
                                 , QString("https://api.roseaudio.kr/newpipe/v1/get?playurl=%1").arg(tmp_str)
                                 , json
                                 , false
                                 , true);
            }
        }
        else if(tmp_Path2.contains("youtube.com/c/") || tmp_Path2.contains("youtube.com/user/")){
            print_debug();
            if(tmp_str.split("/").count() > 1 && tmp_Path3.size()>0){
                print_debug();

                global.search_text = tmp_Path3;
                qDebug() << "tmp_Path3=" << tmp_Path3;
                //this->slot_returnPressed(tmp_Path3);
                print_debug();
                //emit linker->signal_checkQueue(11, "");
                //slot_overrideSigalSearch(true);
                //emit linker->signal_clickedMovePageRoseTubeSearchCall();
                print_debug();
                this->le_search->clear();

            }else{
                //slot_overrideSigalSearch(false);
                qDebug() << "Drag-and-drop playback is not supported except for YouTube content.";
                // ToastMsg::show(this,"", tr("Drag-and-drop playback is not supported except for YouTube content."), 2000, 0);

            }
        }
        else if(tmp_Path2.contains("link.roseaudio.kr")){//c220820
            //https://dev.link.roseaudio.kr/connect/AIR5Cz4leB?param=QOBUZ/ALBUM/hie6erjj8564a
            print_debug();
            qDebug() << "---" << tmp_str.split("?param=").first();
            //----------------------------
            QString get_str;

            if(!tmp_str.split("https://").first().isEmpty()) {//c220820
                print_debug();
                if(tmp_str.split("link.roseaudio.kr/connect/").size()>=2){
                    QString se_str = tmp_str.split("link.roseaudio.kr/connect/").at(1).split(")").at(0);
                    qDebug() << "se_str--" << se_str;
                    se_str = QString("https://link.roseaudio.kr/connect/%1").arg(se_str);
                    qDebug() << "se_str--" << se_str;
                    get_str = getUrlResponse(se_str);
                    qDebug() << "get_str--" << get_str;
                }
                //get_str = getUrlResponse( tmp_str.split("?param=").first());//tmp_str);
                //ToastMsg::show(this, "", tr("The input stream format does not match."));
                get_str = get_str.toUtf8().replace("\\", "");
                global.search_text = tmp_str.split("https://").first().split(" (").first();//c220824_4
                qDebug() << "global.search_text--" << global.search_text;
                //slot_overrideSigalSearch(true);
                if(get_str.contains("TIDAL/")){//c220820
                    print_debug();
                    emit linker->signal_clickedMovePageTidalSearch();
                }else if(get_str.contains("BUGS/")){
                    print_debug();
                    emit linker->signal_clickedMovePageBugsSearch();
                }else if(get_str.contains("QOBUZ/")){
                    print_debug();
                    emit linker->signal_clickedMovePageQobuzSearch();
                }else if(get_str.contains("VIDEO/")){
                    print_debug();
                    emit linker->signal_clickedMovePageVideoSearch();
                }else if(get_str.contains("ROSERADIO/")){
                    print_debug();
                    emit linker->signal_clickedMovePageRoseRadioSearch();
                }else if(get_str.contains("APPLEMUSIC/")){
                    print_debug();
                    //emit linker->signal_clickedMovePageAppleMusicSearch();
                }else{

                }
            }
            else{
                print_debug();
                QString ty = tmp_str.split("?param=").first();
                qDebug() << "ty==" << ty;
                get_str = getUrlResponse( ty);//tmp_str);
                //qDebug() << "pResponse->readAll()---" << get_str.toUtf8();
                get_str = get_str.toUtf8().replace("\\", "");
                qDebug() << "pResponse->readAll()---" << get_str;
                if(get_str.split("<title>").size()<=0) {
                    ToastMsg::show(this, "", tr("The input stream format does not match."));
                    return;
                }
                qDebug() << "tmp_str.split(<title>).at(1)=" << get_str.split("<title>").at(1);
                qDebug() << "tmp_str.split(<title>).at(0)=" <<  get_str.split("<title>").at(1).split("</title>").at(0);
                QString track_title = get_str.split("<title>").at(1).split("</title>").at(0);

                //-------------------------------
                global.search_text = get_str.split("?param=").at(1).split("\";").at(0);

                //slot_overrideSigalSearch(true);
                //slot_returnPressed(tmp_Path3);
                if(get_str.contains("param=")){
                    print_debug();
                    if(get_str.contains("/TRACK/")){
                        print_debug();
                        //slot_overrideSigalSearch(false);

                        // emit linker->signal_clickedMovePageSearch_share(tmp_str);

                        //------------------------------------------------------------

                        //slot_overrideSigalSearch(true);
                        //slot_returnPressed(tmp_Path3);
                        if(get_str.contains("MSUIC")){
                            emit linker->signal_clickedMovePageMusicSearch();
                        }
                        else if(get_str.contains("ROSE_TOTAL")){
                            print_debug();
                            //emit linker->signal_clickedMovePageSearch_share("ROSE_TOTAL/TRACK/"+global.search_text);
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                        }
                        else if(get_str.contains("ROSE_TUBE")){
                            print_debug();
                            if(!get_str.contains("ROSE_TUBE/TRACK/") && get_str.contains("https://i.ytimg.com")){//c220828_3
                                QString temp = get_str.split("https://i.ytimg.com/").at(1).split("/").at(1);
                                global.search_text = temp;
                                qDebug() <<"temp="<<temp;
                                emit linker->signal_clickedMovePageRoseTubeSearch();
                                //emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                            }
                            else if(get_str.contains("ROSE_TUBE/TRACK/")){//c220828_3
                                QString track_url_id = get_str.split("ROSE_TUBE/TRACK/").at(1).split("\";").at(0);
                                qDebug() << "track_url_id---" << track_url_id;
                                global.search_text = "ROSE_TUBE/TRACK/" + track_url_id;
                                //emit linker->signal_clickedMovePageRoseTubeSearch();
                                emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                            }
                            else{
                                print_debug();
                                //emit linker->signal_clickedMovePageSearch_share("ROSE_TOTAL/TRACK/"+global.search_text);
                                emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220826_1
                            }
                        }
                        else if(get_str.contains("TIDAL")){//c220820
                            print_debug();

                            //QString track_url_id = get_str.split("TIDAL/TRACK/").at(1).split("\";").at(0);
                            //qDebug() << "track_url_id---" << track_url_id;
                            //global.search_text = track_url_id;
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                        }
                        else if(get_str.contains("BUGS")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageBugsSearch();
                        }
                        else if(get_str.contains("QOBUZ")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageQobuzSearch();
                        }
                        else if(get_str.contains("VIDEO")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageVideoSearch();
                        }
                        else if(get_str.contains("ROSERADIO")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageRoseRadioSearch();
                        }
                        else if(get_str.contains("APPLEMUSIC")){
                            print_debug();
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                            //emit linker->signal_clickedMovePageAppleMusicSearch();
                        }
                        else{
                            emit linker->signal_checkQueue(34, "");//c220824_4
                        }
                    }
                    else if(get_str.contains("/VIDEO/")){
                        if(get_str.contains("TIDAL")){//c220820
                            print_debug();

                            //QString track_url_id = get_str.split("TIDAL/TRACK/").at(1).split("\";").at(0);
                            //qDebug() << "track_url_id---" << track_url_id;
                            //global.search_text = track_url_id;
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                        }
                    }
                    else if(get_str.contains("/mv/")){
                        if(get_str.contains("BUGS")){//c220820
                            print_debug();

                            //QString track_url_id = get_str.split("TIDAL/TRACK/").at(1).split("\";").at(0);
                            //qDebug() << "track_url_id---" << track_url_id;
                            //global.search_text = track_url_id;
                            emit linker->signal_clickedMovePageSearch_share(global.search_text);//c220824_4
                        }
                    }
                    else{
                        //slot_overrideSigalSearch(false);
                        emit linker->signal_clickedMovePageSearch_share(global.search_text);
                    }
                }

                //slot_returnPressed(tmp_Path3);
                this->le_search->clear();
            }

            print_debug();
            this->le_search->clear();
        }
        else{

            print_debug();
            qDebug() << "tmp_str=" << tmp_str;
            QString get_str = getUrlResponse(tmp_str);
            //qDebug() << "pResponse->readAll()---" << get_str.toUtf8();
            get_str = get_str.toUtf8().replace("\\", "");
            qDebug() << "pResponse->readAll()---" << get_str;

            print_debug();
            if(tmp_str.split("/").count() > 1 && tmp_Path3.size()>0){
                print_debug();
                global.search_text = tmp_Path3;
                //emit linker->signal_clickedMovePageRoseTubeSearch();
                slot_returnPressed(tmp_Path3);
                le_search->clear();

            }
            else{
                qDebug() << "Drag-and-drop playback is not supported except for YouTube content.";
                ToastMsg::show(this,"", tr("Drag-and-drop playback is not supported except for YouTube content."), 2000, 0);
            }
        }
    }
    else {
        print_debug();
        global.dragDrop_pathStr = "";
        qDebug()<< "tmp_str= " << tmp_str;
        if(tmp_str.size()>=2){
            global.search_text = tmp_str;
            slot_returnPressed(tmp_str);
            this->le_search->clear();
        }else{
            ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
            //slot_overrideSigalSearch(false);
        }
    }
    print_debug();
    qDebug() << "global.dragDrop_pathStr=" << global.dragDrop_pathStr;
}

QJsonObject ObjectFromString(const QString& in)//c220726
{
    QJsonValue val(in);
    return val.toObject();
}

/**
 * @brief SearchRemoteBar::slot_responseHttp : [슬롯] Http 요청 결과 처리
 * @param p_id int 요청 아이디
 * @param p_jsonObject QJsonObject response
 */
void SearchRemoteBar::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){

    print_debug();
    QJsonDocument doc(p_jsonObject);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"SearchRemoteBar::slot_responseHttp---" << strJson;

    if(p_id==HTTP_NETWORK_SUGGESTION){

        if(!le_search->text().isEmpty()){
            print_debug();
            QJsonArray jsonArrTotal = p_jsonObject["array"].toArray();
            QJsonArray jsonArrSuggestions = jsonArrTotal.at(1).toArray();

            if(le_search->hasFocus()){
                emit linker->signal_searchSuggestionsChanged(p_jsonObject);
            }
        }
        else{
            print_debug();
            if(le_search->hasFocus()){
                emit linker->signal_searchSuggestionsChanged(QJsonObject());
            }
        }
    }
    else if(p_id == 0){//c220726
        qDebug() << "test";
        print_debug();
        le_search->clear();
        bool p_flagShuffle = false;
        QJsonArray jsonArrYoutube;
        //QJsonObject tmp_json;
        QJsonObject jsonTrackData;
        jsonTrackData.insert("playurl",global.dragDrop_pathStr);
        qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;

        //---//c220802
        QString videoID;
        QString tmp_Path4 = global.dragDrop_pathStr.split("&").at(0);
        if((global.dragDrop_pathStr.contains("&")&& global.dragDrop_pathStr.split("&").at(1).size()>0)){
            videoID = tmp_Path4.split("?v=").last();
        }else{
            videoID = jsonTrackData["playurl"].toString().split("?v=").last();
        }
        //---------------------
        qDebug()<<"videoID="<<videoID;
        QJsonObject jsonYoutube;
        jsonYoutube.insert("channelId", p_jsonObject["channelId"].toString());
        jsonYoutube.insert("channelName", p_jsonObject["uploaderName"].toString());
        jsonYoutube.insert("duration", QString("%1").arg(p_jsonObject["duration"].toInt()));
        jsonYoutube.insert("id", videoID);
        jsonYoutube.insert("isFavorite", p_jsonObject["favorites"].toString()=="1" ? true : false);
        jsonYoutube.insert("isLiveStream",p_jsonObject["streamType"].toString()=="LIVE_STREAM" ? true : false);
        jsonYoutube.insert("thumbnailUrl", p_jsonObject["thumbnailUrl"].toString());
        jsonYoutube.insert("thumbsUpPercentage",0);
        jsonYoutube.insert("title", p_jsonObject["name"].toString());
        jsonYoutube.insert("viewsCount", QString("%1").arg(p_jsonObject["viewCount"].toInt()));

        jsonArrYoutube.append(jsonYoutube);
        QJsonObject jsonParam;
        jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
        jsonParam.insert("shuffle", p_flagShuffle ? 1 : 0);
        jsonParam.insert("youtubePlayType", 15);
        jsonParam.insert("youtube", jsonArrYoutube);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArrYoutube.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        network->request(3344, QString("http://%1:%2/youtubePlay.playlist.add")
                         .arg(global.device.getDeviceIP()).arg(global.port)
                         , jsonParam, true);


    }


    sender()->deleteLater();
}

QJsonObject SearchRemoteBar::make_requestPlayYouTubeJson(const QJsonObject &jsonTrackData){//c220726
    print_debug();
    QJsonObject tmp_json;
    QJsonObject tmp_urljson;
    tmp_urljson.insert("playurl",global.dragDrop_pathStr);
    qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;

    QString videoID = tmp_urljson["playurl"].toString().split("?v=").last();
    qDebug()<<"videoID="<<videoID;
    QJsonObject jsonYoutube;
    //jsonYoutube.insert("channelId", jsonTrackData["channelId"].toString());
    //jsonYoutube.insert("channelName", jsonTrackData["category"].toString());
    //jsonYoutube.insert("duration", QString("%1").arg(jsonTrackData["duration"].toInt()));
    jsonYoutube.insert("id", videoID);
    //jsonYoutube.insert("isFavorite", jsonTrackData["favorites"].toString()=="1" ? true : false);
    //jsonYoutube.insert("isLiveStream",false);
    //jsonYoutube.insert("thumbnailUrl", jsonTrackData["thumbnailUrl"].toString());
    //jsonYoutube.insert("thumbsUpPercentage",0);
    //tmp_json = jsonTrackData["description"].toObject();
    print_debug();
    QJsonDocument doc(tmp_json);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() <<"SearchRemoteBar::slot_responseHttp---" << strJson;

    //jsonYoutube.insert("title", tmp_json["content"].toObject());
    //jsonYoutube.insert("viewsCount", QString("%1").arg(jsonTrackData["viewCount"].toInt()));


    return jsonYoutube;


}

void SearchRemoteBar::requestPlayAll(bool p_flagShuffle){//c220726
    print_debug();
    QJsonArray jsonArrYoutube;
    QJsonObject tmp_json;
    QJsonObject jsonTrackData;
    jsonTrackData.insert("playurl",global.dragDrop_pathStr);
    qDebug()<<"global.dragDrop_pathStr="<<global.dragDrop_pathStr;

    //---//c220802
    QString videoID;
    QString tmp_Path4 = global.dragDrop_pathStr.split("&").at(0);
    if((global.dragDrop_pathStr.contains("&")&& global.dragDrop_pathStr.split("&").at(1).size()>0)){
        videoID = tmp_Path4.split("?v=").last();
    }else{
        videoID = jsonTrackData["playurl"].toString().split("?v=").last();
    }
    //---------------------
    qDebug()<<"videoID="<<videoID;
        QJsonObject jsonYoutube;
        jsonYoutube.insert("channelId", jsonTrackData["channelId"].toString());
        jsonYoutube.insert("channelName", jsonTrackData["channelTitle"].toString());
        jsonYoutube.insert("duration", jsonTrackData["duration_convert"].toString());
        jsonYoutube.insert("id", videoID);
        jsonYoutube.insert("isFavorite", jsonTrackData["favorites"].toString()=="1" ? true : false);
        jsonYoutube.insert("isLiveStream",false);
        jsonYoutube.insert("thumbnailUrl", jsonTrackData["thumbnails"].toString());
        jsonYoutube.insert("thumbsUpPercentage",0);
        jsonYoutube.insert("title", jsonTrackData["title"].toString());
        jsonYoutube.insert("viewsCount", jsonTrackData["viewCount"].toString());

        jsonArrYoutube.append(jsonYoutube);
   // }
        QJsonObject jsonParam;
        jsonParam.insert("roseToken", global.device.getDeviceRoseToken());
        jsonParam.insert("shuffle", p_flagShuffle ? 1 : 0);
        jsonParam.insert("youtubePlayType", 15);
        jsonParam.insert("youtube", jsonArrYoutube);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArrYoutube.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        network->request(3344, QString("http://%1:%2/youtubePlay.playlist.add")
                         .arg(global.device.getDeviceIP()).arg(global.port)
                         , jsonParam, true);

}

void SearchRemoteBar::resizeEvent(QResizeEvent* ) {//c220726

    if(btn_del != nullptr){//c230322
        btn_del->setGeometry(le_search->size().width()-60, 15, 40, 40);//c220719
    }

    print_debug();
}

void SearchRemoteBar::setsearchtext(QString a){//c220726

    print_debug();
    qDebug() << "setsearchtext---a = " << a;
    le_search->setText(a);
    le_search->clearFocus();//c220722
    if(a.size()>1){//c220719
        if(btn_del != nullptr){//
            btn_del->show();
        }
    }
    else{
        if(btn_del != nullptr){//c230423
            btn_del->hide();
        }
    }
}


void SearchRemoteBar::slot_searchCopy(QString a){//c220904_1

    print_debug();
    qDebug() << "setsearchtext---a = " << a;
    le_search->setText(a);
    le_search->setFocus();//c220722
    if(a.size()>1){//c220719
        if(btn_del != nullptr){//
            btn_del->show();
        }
    }
    else{
        if(btn_del != nullptr){//
            btn_del->hide();
        }
    }
}

void SearchRemoteBar::slot_sugestionbarOld_sel(const QString p_text){//c220726
    print_debug();
    qDebug() << "p_text=" << p_text;
    le_search->setText(p_text);//c22715
    //le_search->clear();//c22715
    btn_del->show();
    le_search->clearFocus();//c220628
    if(p_text.size()< 2) {
        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        return;
    }
    emit linker->signal_search(p_text);//c220722

}


/**
 * @brief SearchRemoteBar::resetFixedWidth 사이즈 Width 재세팅
 * @param p_width
 */
void SearchRemoteBar::resetFixedWidth(int p_width){

    if(p_width>SEARCHBAR_W){
        p_width = SEARCHBAR_W;
    }
    this->le_search->setFixedWidth(p_width-LINEEDIT_MIN_WIDTH);
    this->setFixedWidth(p_width);
}


/**
 * @brief SearchRemoteBar::clickedRemoteBtn [SLOT] 리모콘 아이콘 클릭시
 */

void SearchRemoteBar::clickedRemoteBtn(){

    QString tmp_style = this->btn_remote->styleSheet();
    if(this->flagSelectedRemote==true){
        this->flagSelectedRemote = false;
        tmp_style.replace(":/images/icon_remote_gold.png", ":/images/icon_remote.png");
    }else{
        this->flagSelectedRemote = true;
        tmp_style.replace(":/images/icon_remote.png", ":/images/icon_remote_gold.png");
    }
    this->btn_remote->setStyleSheet(tmp_style);
    emit clickedRemote(this->flagSelectedRemote);
}


/**
 * @brief SearchRemoteBar::slot_returnPressed
 * @param p_text
 */
void SearchRemoteBar::slot_returnPressed(const QString p_text){
    print_debug();
    qDebug() << "p_text=" << p_text;

     btn_del->show();
    //le_search->setText("");//c220620
    //le_search->clear();
    //le_search->clearFocus();
    if(p_text.size()< 2) {

        ToastMsg::show(this, "", tr("Please enter at least 2 characters for the search term."));
        return;
    }

    if(p_text.contains("link.roseaudio.kr") || p_text.contains("youtube.com") || p_text.contains("https://t.")){    //j220903 twitter
        slot_get_dragDropText();
    }else{

        emit linker->signal_search(p_text);
        emit linker->signal_sugestionbarOld_sel(p_text);
    }

    //le_search->setText(p_text);//c22715

}

void SearchRemoteBar::slot_downPressed(const QString p_text){//c220904_1
    print_debug();
    qDebug() << "p_text=" << p_text;

    emit linker->signal_sugestionbarDown(p_text);


}

void SearchRemoteBar::slot_upPressed(const QString p_text){//c220904_1
    print_debug();
    qDebug() << "p_text=" << p_text;

    emit linker->signal_sugestionbarUp(p_text);


}

//void SearchRemoteBar::slot_searchBarClear(){//c220628
//    le_search->clear();//c220624
//}

bool SearchRemoteBar::IsKorean(QChar ch)
{

    qDebug() << "ch = " << ch;
    //( 한글자 || 자음 , 모음 )

    if ((0x21 <= ch && ch <= 0x2F) ||(0xAC00 <= ch && ch <= 0xD7A3) || (0x3131 <= ch && ch <= 0x318E)){
        return true;
    }
    else{
        return false;
    }
}

void SearchRemoteBar::clickedRemoteBtn_del(){
    print_debug();
    emit linker->signal_searchBarFocusChanged(false);//c220804
    le_search->clearFocus();//c220804
    le_search->setText("");
    global.search_text = "";
    btn_del->hide();
}

/**
 * @brief SearchRemoteBar::slot_textChanged : [슬롯] 검색 텍스트 변경
 * @param p_text
 */
void SearchRemoteBar::slot_textChanged(QString p_text){//c220818

    /*
query (q): the keyword that you need to search about its autocomplete. For example loreal.
language (hl): autocomplete results language. For example en, de, tr. For more language codes you can visit Google supported language page.
country (gl): autocomplete results country. For example US, DE, TR. For more country codes you can visit Google supported country code page.
context (ds): which type of google autosuggestion source you need. For Web as source use web, YouTube as source use yt, Products as source use sh, and News as source use n.
*/
qDebug() << "SearchRemoteBar::slot_textChanged()";
qDebug() << "p_text = " << p_text;
    if(!p_text.trimmed().isEmpty()){

        NetworkHttp *network = new NetworkHttp(this);
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QUrlQuery jsonData;
        //jsonData.addQueryItem("client","firefox"); // jsonp
        jsonData.addQueryItem("client","chrome"); // jsonp
        //jsonData.addQueryItem("client","youtube"); // jsonp
        jsonData.addQueryItem("q", p_text);
        jsonData.addQueryItem("ds","yt");
        //jsonData.addQueryItem("output","toolbar");
        jsonData.addQueryItem("hl","ja");
        jsonData.addQueryItem("hl","ko");
        jsonData.addQueryItem("hl","fr");
        jsonData.addQueryItem("hl","es");
        jsonData.addQueryItem("hl","ru");
        jsonData.addQueryItem("hl","zh-hans");
        jsonData.addQueryItem("hl","de");
        jsonData.addQueryItem("hl","en");
        if(IsKorean(p_text[0])== true){

            //jsonData.addQueryItem("hl","ko");//c220818
            qDebug() << "slot_textChanged---kor";
        }else{

            qDebug() << "slot_textChanged---eng";
        }
        //jsonData.addQueryItem("hl","kr&en");

        //network->request(HTTP_NETWORK_SUGGESTION, QString("http://suggestqueries.google.com/complete/search"), jsonData, false);
        network->request(HTTP_NETWORK_SUGGESTION, QString("http://google.com/complete/search"), jsonData, false);


    }else{
        if(le_search->hasFocus()){
            emit linker->signal_searchSuggestionsChanged(QJsonObject());
        }
    }
}




