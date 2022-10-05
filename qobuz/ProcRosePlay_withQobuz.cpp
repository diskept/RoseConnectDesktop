#include "qobuz/ProcRosePlay_withQobuz.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include <QDebug>


namespace qobuz {

    const QString CODE_ROSE___OK = "G0000";


    /**
     * @brief ProcRosePlay_withQobuz::ProcRosePlay_withQobuz
     * @param parent
     */
    ProcRosePlay_withQobuz::ProcRosePlay_withQobuz(QWidget *parent)
        : AbstractProcRosePlay(parent)
    {
        linker = Linker::getInstance();
    }


    /**
     * @brief ProcRosePlay_withQobuz::request_set_session_info
     * @param p_sessionInfo
     */
    void ProcRosePlay_withQobuz::request_set_session_info(const qobuz::RoseSessionInfo_forQobuz& p_sessionInfo){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
        //print_debug();
        //http://192.168.1.13:9283/qobuz_set_session_info

        QJsonObject jsonObj_send = ConvertData::getObjectJson_roseSessionInfo(p_sessionInfo);

        network->request(HttpRequestType::Set_Session_Info
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_session_info")
                         , jsonObj_send
                         , true
                         , true);
    }


    /**
     * @brief 세션 정보를 요청함
     */
    void ProcRosePlay_withQobuz::request_get_session_info(){

        //print_debug();
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //http://192.168.1.13:9283/qobuz_get_session_info

        QJsonObject jsonObj_send;
        //jsonObj_send.insert("dummy", "");
        network->request(HttpRequestType::Get_Session_Info
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_get_session_info")
                         , jsonObj_send
                         , true
                         , true);
    }



    // Rose 에게 Play 요청 about Album   ------------------------------------------------------------------------

    /**
     * @brief album_id로 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param album_id
     * @param clickMode
     */
    void ProcRosePlay_withQobuz::requestPlayRose_byAlbumID(const QString album_id, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        ProcCommon *proc = new ProcCommon(this->parent);
        connect(proc, &ProcCommon::completeReq_list_items_of_album, this, &ProcRosePlay_withQobuz::slot_applyResult_tracks);
        proc->request_qobuz_getList_items_of_album(album_id);
    }


    /**
     * @brief album_id로 전체재생, 셔플재생 요청한다. AbstractImageDetailInfo::BtnClickMode 을 그대로 전달받아서 알아서 처리하도록 한다.
     * @details OptMorePopup의 Play_RightNow 모드로 재생된다.
     * @param album_id
     * @param btnClickMode
     */
    void ProcRosePlay_withQobuz::requestPlayRose_byAlbumID(const QString album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode){

        this->curr_shuffleMode = ProcRosePlay_withQobuz::JustPlay;
        if(btnClickMode == AbstractImageDetailInfo::PlayShuffle){
            this->curr_shuffleMode = ProcRosePlay_withQobuz::ShufflePlay;
        }

        this->requestPlayRose_byAlbumID(album_id, OptMorePopup::ClickMode::Play_RightNow);
    }


    /**
     * @brief ProcRosePlay_withQobuz::slot_applyResult_tracks
     * @param list_track
     * @param jsonArray_toPlay
     */
    void ProcRosePlay_withQobuz::slot_applyResult_tracks(const QList<qobuz::TrackItemData>& list_track, const QJsonArray& jsonArray_toPlay){

        int playType = this->get_rose_playType(this->curr_clickMode);

        if(list_track.size() <= 0)
        {
            return;
        }

        this->request_rose_qobuzPlay_set_queue(playType, jsonArray_toPlay, this->curr_shuffleMode);
    }


    // Rose 에게 Play 요청 about Video   ------------------------------------------------------------------------

    /**
     * @brief video 1개애 대하여 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param jsonObj_toPlay
     * @param clickMode
     */
    void ProcRosePlay_withQobuz::requestPlayRose_video(const QJsonObject jsonObj_toPlay, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        int playType = this->get_rose_playType(this->curr_clickMode);
        this->request_rose_qobuzPlay_set_queue_video(playType, jsonObj_toPlay);
    }


    // Rose 에게 Play 요청 about Playlist   ------------------------------------------------------------------------

    /**
     * @brief Playlist의 UUID로 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param playlist_uuid
     * @param clickMode
     */
    void ProcRosePlay_withQobuz::requestPlayRose_byPlaylistID(const qobuz::PlaylistItemData& data_playlist, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        ProcCommon *proc = new ProcCommon(this->parent);

        if(data_playlist.tracks_count > 100){
            this->playlist_id_check = data_playlist.id;

            this->jsonArr_tracks_toPlay = QJsonArray();
            this->playlist_tracks_offset = 0;

            connect(proc, &ProcCommon::completeReq_list_items_of_playlist, this, &ProcRosePlay_withQobuz::slot_applyResult_items_of_playlist);
            proc->request_qobuz_getList_items_of_playlist(data_playlist.id, 100, 0);     // 트랙 수 만큼 전체 요청함 like App Proc
        }
        else{
            connect(proc, &ProcCommon::completeReq_list_items_of_playlist, this, &ProcRosePlay_withQobuz::slot_applyResult_items_of_playlist);
            proc->request_qobuz_getList_items_of_playlist(data_playlist.id, data_playlist.tracks_count, 0);     // 트랙 수 만큼 전체 요청함 like App Proc
        }
    }


    /**
     * @brief [Slot] Playlist의 Track들을 플레이한다. @see requestPlayRose_byPlaylistUUID
     * @param list_track
     * @param jsonArray_toPlay
     */
    void ProcRosePlay_withQobuz::slot_applyResult_items_of_playlist(const QList<qobuz::TrackItemData>& list_track, const QJsonArray& jsonArray_toPlay, const bool& flag_lastPage){

        Q_UNUSED(list_track);
        int playType = this->get_rose_playType(this->curr_clickMode);

        if(flag_lastPage == true){

            if(this->playlist_tracks_offset == 0 && list_track.size() <= 0)
            {
                return;
            }

            if(this->playlist_tracks_offset > 0){
                ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArray_toPlay);
                this->playlist_tracks_offset = this->jsonArr_tracks_toPlay.size();

                this->request_rose_qobuzPlay_set_queue(playType, this->jsonArr_tracks_toPlay, this->curr_shuffleMode);
            }
            else{
                this->request_rose_qobuzPlay_set_queue(playType, jsonArray_toPlay, this->curr_shuffleMode);
            }
        }
        else{
            ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArray_toPlay);
            this->playlist_tracks_offset = this->jsonArr_tracks_toPlay.size();

            // request HTTP API
            ProcCommon *proc = new ProcCommon(this->parent);
            connect(proc, &ProcCommon::completeReq_list_items_of_playlist, this, &ProcRosePlay_withQobuz::slot_applyResult_items_of_playlist);
            proc->request_qobuz_getList_items_of_playlist(this->playlist_id_check, 100, this->playlist_tracks_offset);     // 트랙 수 만큼 전체 요청함 like App Proc
        }
    }


    // Rose 에게 Play 요청 about Tracks   ------------------------------------------------------------------------

    /**
     * @brief 트랙 목록(jsonArr_toPlayAll)에 대하여 clickMode 에 따라서 Rose에게 플레이 요청한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @param shuffleMode 기본값은 JustPlay.
     */
    void ProcRosePlay_withQobuz::requestPlayRose_byTracks(const QJsonArray& p_jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){

        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);
        //print_debug();
        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(p_jsonArr_toPlayAll, curr_index, clickMode);
        if(jsonArr_toPlayReal.count() > 0){
            this->request_rose_qobuzPlay_set_queue(playType, jsonArr_toPlayReal, shuffleMode);
        }
    }


    /**
         * @brief Queue에서 Track 하나를 선택한 경우임 (재생을 위해)
         * @param currentPosition
         * @param jsonObj_track
         */
    void ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track){//cheon210916-total

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //QJsonObject json_qobuzModeItem;


        // 모두 string 으로 보내야 함
        QJsonDocument doc(jsonObj_track);  QString strToJson(doc.toJson(QJsonDocument::Compact));
        QJsonObject jsonObj_send, tt;
        jsonObj_send.insert("data",strToJson);
        jsonObj_send.insert("currentPlayer", "QOBUZ");
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("remotePositionPlay", currentPosition);
        jsonObj_send.insert("queueMode", "TOTAL");//cheon211104-01

        jsonObj_send.insert("checkSum", global.checkSum);
        qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist----global.checkSum = " << global.checkSum;
        qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist----global.device.getDeviceRoseToken() = " << global.device.getDeviceRoseToken();

        //QJsonDocument doc1(jsonObj_send);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist---" << strJson;
        //qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
        //QString tmp_string = jsonObj_send["data"].toString();
        //print_debug();
        //qDebug() << "tmp_string -- : " << tmp_string;
        network->request(Rose_qobuzPlay_current_playlist,

                         //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set"),
                         jsonObj_send,
                         true,
                         true);
    }


    /*
    void ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonArray jsonArr_qobuzModeItem;
        jsonArr_qobuzModeItem.append(jsonObj_track);

        // 모두 string 으로 보내야 함
        QJsonObject jsonObj_send;
        jsonObj_send.insert("currentPosition", currentPosition);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("qobuzModeItem", jsonArr_qobuzModeItem);

        network->request(Rose_qobuzPlay_current_playlist,
                         //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuzPlay_current_playlist"),
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                         jsonObj_send,
                         true,
                         true);
    }
    */


    /**
     * @brief Queue에서 Track들을 삭제 요청함
     * @param totalCnt - 현재의 Qobuz Queue 데이터 개수
     * @param jsonArr_deletePosition
     * @param jsonArr_deleteArr
     */
    void ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr){//cheon210916-delete

        //print_debug();
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // 모두 string 으로 보내야 함
        QJsonObject jsonObj_send;

        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        qDebug() << "ProcRosePlay_withTidal::request_tidalPlay_current_playlist_delete----global.checkSum = " << global.checkSum;
        jsonObj_send.insert("checkSum", global.checkSum);
        jsonObj_send.insert("playType", "TOTAL");//cheon211107-01
        jsonObj_send.insert("indexArr", jsonArr_deletePosition);
        //jsonObj_send.insert("deleteArr", jsonArr_deleteArr);

        network->request(Rose_qobuzPlay_current_playlist_delete,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete"),
                         jsonObj_send,
                         true,
                         true);
        /*
            NetworkHttp *network = new NetworkHttp;
            connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));
    //print_debug();
            // 모두 string 으로 보내야 함
            QJsonObject jsonObj_send;
            jsonObj_send.insert("totalCnt", totalCnt);
            jsonObj_send.insert("deletePosition", jsonArr_deletePosition);
            jsonObj_send.insert("deleteArr", jsonArr_deleteArr);

            network->request(Rose_qobuzPlay_current_playlist_delete,
                             QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuzPlay_current_playlist_delete"),
                             jsonObj_send,
                             true,
                             true);
    */
    }


    /**
     * @brief Queue에서 Track들을 삭제 요청함 - 1개 Track
     * @param totalCnt
     * @param deletePosition
     * @param jsonObj_deleteObj
     */
    void ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj){

        QJsonArray jsonArr_deletePositon;
        jsonArr_deletePositon.append(deletePosition);
        //print_debug();
        QJsonArray jsonArr_deleteArr;
        jsonArr_deleteArr.append(jsonObj_deleteObj);

        this->request_qobuzPlay_current_playlist_delete(totalCnt, jsonArr_deletePositon, jsonArr_deleteArr);
    }


    // 실제 Http 요청 및 처리 파트   ------------------------------------------------------------------------

    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (음원)
     * @param playType
     * @param jsonArr_qobuzModeItem
     * @param shuffleMode
     */
    void ProcRosePlay_withQobuz::request_rose_qobuzPlay_set_queue(const int playType, const QJsonArray &jsonArr_qobuzModeItem, PlayShuffleMode shuffleMode){//cheon210916-total

        //print_debug();
        //qDebug() << "9. [QOBUZ][SEQ][FUNC] : ProcRosePlay_withQobuz.cpp -> ProcRosePlay_withQobuz::request_rose_qobuzPlay_set_queue(const int playType, const QJsonArray &jsonArr_qobuzModeItem, PlayShuffleMode shuffleMode)";
        //qDebug() << "[QOBUZ][SEQ][MSG] track information send Rose -> goto Nest step\n";

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_qobuzModeItem.count();

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
        jsonObj_send.insert("currentPosition", 0);
        jsonObj_send.insert("playType", playType);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());

        //qDebug() << ""

        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", 0);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }

        jsonObj_send.insert("tracks", jsonArr_qobuzModeItem);

        //QJsonDocument doc(jsonObj_send);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug("qobuz_set_queue : ");
        //qDebug() << strJson << "\n";

        network->request(Rose_qobuzPlay_set_queue,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue"),
                         jsonObj_send,
                         true,
                         true);
    }


    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (동영상 1개)
     * @details Qobuz 의 동영상 여러개 재생 요청은 아직 없나보다. 디자인엔 비디오도 playlist가 보이던데.
     * @param , PlayShuffleMode shuffleMode
     * @param jsonObj_toPlay
     * @param shuffleMode - 2020-08-10 Android 3.3.4, Rose 3.4 기준으로 항상 shuffle=0 으로 전달하고 있어서, 기본값이 JustPlay임.
     */
    void ProcRosePlay_withQobuz::request_rose_qobuzPlay_set_queue_video(const int playType, const QJsonObject& jsonObj_toPlay, PlayShuffleMode shuffleMode){

        //print_debug();
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonArray jsonArr_qobuzVideoItem;
        jsonArr_qobuzVideoItem.append(jsonObj_toPlay);           // 재생 요청 비디오 정보를 넣는다.

        QJsonObject jsonObj_send;
        jsonObj_send.insert("currentPosition", 0);
        jsonObj_send.insert("playType", playType);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("qobuzVedioItem", jsonArr_qobuzVideoItem);      // 음원일 때와는 key 값이 다름. Video 가 아니라 Vedio 오타... ㅡ,.ㅡ

        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", 0);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }

        network->request(Rose_qobuzPlay_set_queue_video,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuzPlay_set_queue.video"),
                         jsonObj_send,
                         true,
                         true);
    }


    /**
     * @brief ProcCommon::slot_responseHttp
     * @param p_id
     * @param p_jsonObj
     */
    void ProcRosePlay_withQobuz::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){

        Q_UNUSED(p_id);

        //print_debug();qDebug() << "p_id = " << p_id;
        QJsonDocument doc(p_jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  //qDebug() << "ProcRosePlay_withQobuz::slot_responseHttp---" << strJson;

        //qDebug() << "10. [QOBUZ][SEQ][FUNC] : ProcRosePlay_withQobuz.cpp -> ProcRosePlay_withQobuz::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj)";
        //qDebug() << "[QOBUZ][SEQ][MSG] qobuz_set_queue :: Response";
        //QJsonDocument doc(p_jsonObj);
        //QString strJson(doc.toJson(QJsonDocument::Compact));
        //qDebug() << strJson;

        // 나중에 실패요인에 따라서 처리 보강
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool checkQueue = ProcJsonEasy::getBool(p_jsonObj, "checkQueue");//c1209
            if(checkQueue == true){//c1209

                //emit linker->signal_checkQueue(6, "");//c220411
                //if(global.queuelist_mouse_trigger_menu_flag)
                    emit linker->signal_music_start();//c220411
            }
            QString code = ProcJsonEasy::getString(p_jsonObj, "code");
            if(code == "G0000"){
                if(p_id == Rose_qobuzPlay_current_playlist_delete){
                    emit this->completeReq_qobuzPlay_current_playlist_delete();
                }
                else if(p_id == Get_Session_Info){
                    QString tmp_data = ProcJsonEasy::getString(p_jsonObj, "data", "{}");
                    QJsonDocument jsocDoc = QJsonDocument::fromJson(tmp_data.toUtf8());
                    QJsonObject jsonObj_data = jsocDoc.object();

                    qobuz::RoseSessionInfo_forQobuz sessionInfo = ConvertData::convertData_roseSessionInfo(jsonObj_data);

                    emit this->signal_completeReq_get_session_info(sessionInfo);
                }
            }
            else if(code == "SLEEP"){
                print_debug();
                emit linker->signal_checkQueue(24, "");
                if(global.user_forQobuz.flag_login_wait == true)
                {
                    global.user_forQobuz.flag_login_wait = false;
                    //qobuz::RoseSessionInfo_forQobuz sessionInfo;
                    //emit this->signal_completeReq_get_session_info(sessionInfo);
                }
                emit this->signal_cannotRosePlay(tr("Please turn on the power."));
                //emit this->signal_cannotRosePlay(tr("Rose 전원을 켜주세요."));
            }
            if(ProcJsonEasy::getBool(p_jsonObj, "checkQueue")==true){//cheon211107-01
                print_debug();
                qDebug()<< "checkQueue = " << ProcJsonEasy::getBool(p_jsonObj, "checkQueue");
            }else{
                print_debug();
                qDebug()<< "checkQueue = " << ProcJsonEasy::getBool(p_jsonObj, "checkQueue");
            }
        }
        else{
            //            emit this->signal_cannotRosePlay(tr("I requested the playback with Rose device, but an unknown error occurred."));
            //            emit this->signal_cannotRosePlay(tr("Rose 기기로 재생요청을 하였으나, 알수없는 오류가 발생하였습니다."));
        }

        sender()->deleteLater();

    }


    /**
     * @brief ProcRoseAPI_withBugs::checkResult_set_session_info
     * @param p_jsonObj
     */
    void ProcRosePlay_withQobuz::checkResult_set_session_info(const QJsonObject& p_jsonObj){

        Q_UNUSED(p_jsonObj);
        // donothing
    }


    /**
     * @brief ProcRoseAPI_withBugs::checkResult_get_session_info
     * @param p_jsonObj
     */
    void ProcRosePlay_withQobuz::checkResult_get_session_info(const QJsonObject& p_jsonObj){

        if(ProcJsonEasy::getString(p_jsonObj, "code") == CODE_ROSE___OK){
            QString tmp_data = ProcJsonEasy::getString(p_jsonObj, "data", "{}");
            QJsonDocument jsocDoc = QJsonDocument::fromJson(tmp_data.toUtf8());
            QJsonObject jsonObj_data = jsocDoc.object();

            qobuz::RoseSessionInfo_forQobuz sessionInfo = ConvertData::convertData_roseSessionInfo(jsonObj_data);
            emit this->signal_completeReq_get_session_info(sessionInfo);
        }
    }


    /**
     * @brief Queue에서 Track 하나를 선택한 경우임 (재생을 위해)
     * @param currentPosition
     * @param jsonObj_track
     */
    void ProcRosePlay_withQobuz::request_applePlay_current_playlist(const int currentPosition, const QJsonObject& jsonObj_track){//c220304

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        //QJsonObject json_qobuzModeItem;


        // 모두 string 으로 보내야 함
        QJsonDocument doc(jsonObj_track);  QString strToJson(doc.toJson(QJsonDocument::Compact));
        QJsonObject jsonObj_send, tt;
        jsonObj_send.insert("data",strToJson);
        jsonObj_send.insert("currentPlayer", "APPLE_MUSIC");
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("remotePositionPlay", currentPosition);
        jsonObj_send.insert("queueMode", "TOTAL");//cheon211104-01

        jsonObj_send.insert("checkSum", global.checkSum);
        qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist----global.checkSum = " << global.checkSum;
        qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist----global.device.getDeviceRoseToken() = " << global.device.getDeviceRoseToken();

        //QJsonDocument doc1(jsonObj_send);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist---" << strJson;
        //qDebug() << "ProcRosePlay_withQobuz::request_qobuzPlay_current_playlist----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
        //QString tmp_string = jsonObj_send["data"].toString();
        //print_debug();
        //qDebug() << "tmp_string -- : " << tmp_string;
        network->request(Rose_qobuzPlay_current_playlist,
                         //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set"),
                         jsonObj_send,
                         true,
                         true);
    }
}
