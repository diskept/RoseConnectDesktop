#include "apple/ProcRosePlay_withApple.h"

#include "apple/ConvertData_forApple.h"
#include "apple/ProcCommon_forApple.h"

#include "common/global.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"


namespace apple{

    /**
     * @brief ProcRosePlay_withApple::ProcRosePlay_withApple
     * @param parent
     */
    ProcRosePlay_withApple::ProcRosePlay_withApple(QWidget *parent)
        : AbstractProcRosePlay(parent)
    {

        this->linker = Linker::getInstance();
    }


    void ProcRosePlay_withApple::request_set_session_info(const apple::RoseSessionInfo_forApple &p_sessionInfo){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send = ConvertData::getObjectJson_roseSessionInfo(p_sessionInfo);

        network->request(HttpRequestType::Set_Session_Info
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("apple_music_set_session_info")
                         , jsonObj_send
                         , true
                         , true);
    }


    /**
     * @brief 세션 정보를 요청함
     */
    void ProcRosePlay_withApple::request_get_session_info(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;

        network->request(HttpRequestType::Get_Session_Info
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("apple_music_get_session_info")
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
    void ProcRosePlay_withApple::requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        ProcCommon *proc = new ProcCommon(this->parent);
        connect(proc, &ProcCommon::completeReq_tracks, this, &ProcRosePlay_withApple::slot_applyResult_tracks);
        proc->request_apple_get_album(album_id);
    }


    /**
     * @brief ProcRosePlay_withApple::slot_applyResult_tracks
     * @param list_track
     * @param jsonArray_toPlay
     */
    void ProcRosePlay_withApple::slot_applyResult_tracks(const QJsonArray& jsonArray_toPlay){

        int playType = this->get_rose_playType(this->curr_clickMode);
        this->request_rose_applePlay_set_queue(playType, jsonArray_toPlay, this->curr_shuffleMode);
    }


    void ProcRosePlay_withApple::requestPlayRose_byPlaylistID(const QString playlist_id, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        ProcCommon *proc = new ProcCommon(this->parent);
        connect(proc, &ProcCommon::completeReq_tracks, this, &ProcRosePlay_withApple::slot_applyResult_tracks);
        proc->request_apple_get_playlist(playlist_id);
    }


    // Rose 에게 Play 요청 about Tracks   ------------------------------------------------------------------------

    /**
     * @brief 트랙 목록(jsonArr_toPlayAll)에 대하여 clickMode 에 따라서 Rose에게 플레이 요청한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @param shuffleMode 기본값은 JustPlay.
     */
    void ProcRosePlay_withApple::requestPlayRose_byTracks(const QJsonArray& jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){

        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);

        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(jsonArr_toPlayAll, curr_index, clickMode);
        if(jsonArr_toPlayReal.count() > 0){
            this->request_rose_applePlay_set_queue(playType, jsonArr_toPlayReal, shuffleMode);
        }
    }


    void ProcRosePlay_withApple::request_rose_applePlay_set_queue(const int playType, const QJsonArray &jsonArr_appleModeItem, PlayShuffleMode shuffleMode){

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_appleModeItem.count();

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
        jsonObj_send.insert("playType", playType);
        jsonObj_send.insert("currentPosition", 0);

        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", -1);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }

        jsonObj_send.insert("tracks", jsonArr_appleModeItem);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());

        network->request(Rose_applePlay_set_queue,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("apple_music_set_queue"),
                         jsonObj_send,
                         true,
                         true);
    }


    /**
     * @brief ProcRosePlay_withApple::slot_responseHttp
     * @param p_id
     * @param p_jsonObj
     */
    void ProcRosePlay_withApple::slot_responseHttp(const int &p_id, const QJsonObject &p_jsonObj){

        QJsonDocument doc(p_jsonObj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        qDebug() << strJson;

        // 나중에 실패요인에 따라서 처리 보강
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){

            if(p_jsonObj.contains("checkQueue")){

                if(p_jsonObj.value("checkQueue").toBool()){
                    emit linker->signal_music_start();//c220411
                }
            }

            if(p_jsonObj.value("code").toString() == "G0000"){

                if(p_id == Get_Session_Info){
                    QString tmp_data = ProcJsonEasy::getString(p_jsonObj, "data", "{}");
                    QJsonDocument jsocDoc = QJsonDocument::fromJson(tmp_data.toUtf8());
                    QJsonObject jsonObj_data = jsocDoc.object();

                    apple::RoseSessionInfo_forApple sessionInfo = ConvertData::convertData_roseSessionInfo(jsonObj_data);

                    emit this->signal_completeReq_get_session_info(sessionInfo);
                }
            }
        }
    }
}
