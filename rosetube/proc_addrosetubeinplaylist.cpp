#include "proc_addrosetubeinplaylist.h"
#include "widget/toastmsg.h"
#include "common/networkhttp.h"
#include "common/global.h"

const int HTTP_NETWORK_ADD_PLAYLIST = 89;
const int HTTP_NETWORK_NEW_PLAYLIST = 88;

/**
 * @brief Proc_addRosetubeInPlayList::Proc_addRosetubeInPlayListj : 로즈튜브 플레이리스트 담기 PROC
 * @details
 * 플레이리스트 선택, 플레이리스트명 입력 Dlg 이용해 담기 가능</br>
 * Dlg 이용없이 담기 가능
 * @param parent
 */
Proc_AddRosetubeInPlayList::Proc_AddRosetubeInPlayList(QWidget *parent)
    : QWidget(parent)
{
    setInit();
    setUIControl();
}

/**
 * @brief Proc_addRosetubeInPlayList::setInit : 초기 세팅
 */
void Proc_AddRosetubeInPlayList::setInit(){
    linker = Linker::getInstance();
}

/**
 * @brief Proc_addRosetubeInPlayList::setUIControl : UI 세팅
 */
void Proc_AddRosetubeInPlayList::setUIControl(){
    dlg_playList = new Dialog::Dialog_Playlist(Dialog::Dialog_Playlist::PlayListType::YOUTUBE, this);
    connect(dlg_playList, SIGNAL(signal_clickedPlaylist(int)), this, SIGNAL(signal_playlistClicked(int)));
}

/**
 * @brief Proc_addRosetubeInPlayList::setPlayUrl : 로즈튜브 재생 URL 세팅
 * @param p_playUrl
 * @note 플레이리스트 담기시 필요
 */
void Proc_AddRosetubeInPlayList::setPlayUrl(QString p_playUrl){
    playUrl = p_playUrl;
}

/**
 * @brief Proc_addRosetubeInPlayList::showDlgOfPlayList : 플레이리스트 선택 Dlg Show
 */
void Proc_AddRosetubeInPlayList::showDlgOfPlayList(){
    dlg_playList->requestMyPlayList();
    dlg_playList->show();
}


/**
 * @brief Proc_addRosetubeInPlayList::showDlgOfNewPlayList : 신규 플레이리스트 이름 입력 Dlg Show
 */
void Proc_AddRosetubeInPlayList::showDlgOfNewPlayList(){
    Dialog::Dialog_NewPlaylist *dlg_newPlaylist = new Dialog::Dialog_NewPlaylist(this);
    int tmp_result = dlg_newPlaylist->exec();
    if(tmp_result == QDialog::Accepted){
        emit signal_newPlaylistNameEdited(dlg_newPlaylist->getName());
    }
    delete dlg_newPlaylist;
}

/**
 * @brief Proc_addRosetubeInPlayList::requestAddVideoInPlaylist : 플레이리스트에 유튜브 추가
 * @param p_playlistno int 플레이리스트 넘버
 * @note 추가 할 유튜브 재생 url 세팅된 경우에 사용. {@link setPlayUrl(QString)}
 */
void Proc_AddRosetubeInPlayList::requestAddVideoInPlaylist(const int &p_playlistno){
    requestAddVideoInPlaylist(p_playlistno, playUrl);
}

/**
 * @brief Proc_addRosetubeInPlayList::requestAddVideoInPlaylist : 플레이리스트에 유튜브 추가
 * @param p_playlistno int 플레이리스트 넘버
 * @param p_playurl QString 유튜브 재생 url
 */
void Proc_AddRosetubeInPlayList::requestAddVideoInPlaylist(const int &p_playlistno, const QString &p_playurl){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("playlistno",QString::number(p_playlistno));
    params.addQueryItem("playurl",p_playurl);
    params.addQueryItem("username",global.user.getUsername());

    network->request(HTTP_NETWORK_ADD_PLAYLIST, QString("%1/playlist/track/insert")
                     .arg(global.legacy_mod), params, true,true);
}


/**
 * @brief Proc_addRosetubeInPlayList::requestNewVideoInPlaylist : 새 플레이리스트 추가 요청
 * @param p_name QString 플레이리스트명
 * @note 추가 할 유튜브 재생 url 세팅된 경우에 사용. {@link setPlayUrl(QString)}
 */
void Proc_AddRosetubeInPlayList::requestNewVideoInPlaylist(const QString &p_name){
        requestNewVideoInPlaylist(p_name, playUrl);
}

/**
 * @brief Proc_addRosetubeInPlayList::requestNewVideoInPlaylist : 새 플레이리스트 추가 요청
 * @param p_name QString 플레이리스트명
 * @param p_playurl QString 추가 할 유튜브 재생 url
 */
void Proc_AddRosetubeInPlayList::requestNewVideoInPlaylist(const QString &p_name, const QString p_playurl){
    NetworkHttp *network = new NetworkHttp(this);
    connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

    QUrlQuery params;
    params.addQueryItem("title",p_name);
    params.addQueryItem("type","youtube");
    params.addQueryItem("username",global.user.getUsername());
    params.addQueryItem("playurl", p_playurl);

    network->request(HTTP_NETWORK_NEW_PLAYLIST, QString("%1/playlist/new")
                     .arg(global.legacy_mod), params, true,true);
}

/**
 * @brief Proc_addRosetubeInPlayList::setResultOfAddPlayListTrack : 플레이리스트 담기 요청결과처리
 * @param p_jsonData QJsonObject response
 */
void Proc_AddRosetubeInPlayList::setResultOfAddPlayListTrack(const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";

    bool flagOk = false;
    QString tmp_errorMsg = "";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
            emit linker->signal_myPlaylistChanged();
        }else{
            if(p_jsonData.contains(jsonKey_message)){
                tmp_errorMsg = p_jsonData[jsonKey_message].toString();
            }
        }
    }

    // show Error
    if(!flagOk){
        if(tmp_errorMsg.isEmpty()){
            tmp_errorMsg = tr("Unable to add to Playlist.");
            //tmp_errorMsg = tr("플레이리스트에 추가할 수 없습니다.");
        }
        ToastMsg::show(this,"",tmp_errorMsg);
    }
}

/**
 * @brief Proc_addRosetubeInPlayList::setResultOfNewPlayListTrack : 플레이리스트 생성후 추가 요청결과처리
 * @param p_jsonData QJsonObject response
 */
void Proc_AddRosetubeInPlayList::setResultOfNewPlayListTrack(const QJsonObject &p_jsonData){
    const QString jsonKey_flagOk = "flagOk";
    const QString jsonKey_result = "result";
    const QString jsonKey_message = "message";

    bool flagOk = false;
    QString tmp_errorMsg = "";

    if(p_jsonData.contains(jsonKey_flagOk) && p_jsonData[jsonKey_flagOk].toBool()){
        if(p_jsonData.contains(jsonKey_result) && p_jsonData[jsonKey_result].toString().toLower()=="success"){
            flagOk = true;
            emit linker->signal_myPlaylistChanged();
        }else{
            if(p_jsonData.contains(jsonKey_message)){
                tmp_errorMsg = p_jsonData[jsonKey_message].toString();
            }
        }
    }

    // show Error
    if(!flagOk){
        if(tmp_errorMsg.isEmpty()){
            tmp_errorMsg = tr("Unable to add to Playlist.");
            //tmp_errorMsg = tr("플레이리스트에 추가할 수 없습니다.");
        }
        ToastMsg::show(this,"",tmp_errorMsg);
    }
}


/**
 * @brief Proc_addRosetubeInPlayList::slot_responseHttp : [슬롯] Http 요청 결과 처리
 * @param p_id
 * @param p_jsonObject
 */
void Proc_AddRosetubeInPlayList::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObject){
    switch (p_id){
        case HTTP_NETWORK_ADD_PLAYLIST :
        setResultOfAddPlayListTrack(p_jsonObject);
        break;
    case HTTP_NETWORK_NEW_PLAYLIST :
        setResultOfNewPlayListTrack(p_jsonObject);
        break;
    }

    sender()->deleteLater();
}
