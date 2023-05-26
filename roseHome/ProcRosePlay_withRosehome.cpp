#include "roseHome/ProcRosePlay_withRosehome.h"

#include "roseHome/ProcCommon_forRosehome.h"
#include "roseHome/ConvertData_rosehome.h"

#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"

#include <QDebug>

namespace roseHome {

    const QString CODE_ROSE___OK = "G0000";


    /**
    * @brief ProcRosePlay_withQobuz::ProcRosePlay_withQobuz
    * @param parent
    */
    ProcRosePlay_withRosehome::ProcRosePlay_withRosehome(QWidget *parent)
       : AbstractProcRosePlay(parent)
    {
        linker = Linker::getInstance();

    }


    /**
     * @brief 세션 정보를 요청함
     */
    void ProcRosePlay_withRosehome::request_get_session_info(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonData;

        QString url = "http://" + global.device.getDeviceIP() + ":" + global.port + "/get_rose_user_info";
        network->request(HttpRequestType::Get_Session_Info
                         , url
                         , jsonData
                         , true
                         , true);

    }


    // Rose 에게 Play 요청 about    ------------------------------------------------------------------------

    /**
     * @brief ProcRosePlay_withQobuz::slot_applyResult_tracks
     * @param list_track
     * @param jsonArray_toPlay
     */
    /*void ProcRosePlay_withRosehome::slot_applyResult_tracks(const QList<roseHome::TrackItemData>& list_track, const QJsonArray& jsonArray_toPlay){
        Q_UNUSED(list_track);

        int playType = this->get_rose_playType(this->curr_clickMode);
        //this->request_rose_totalPlay_set_queue(playType, jsonArray_toPlay, this->curr_shuffleMode);
    }*/


    // Rose 에게 Play 요청 about Album   ------------------------------------------------------------------------

    /**
     * @brief album_id로 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param album_id
     * @param clickMode
     */
    void ProcRosePlay_withRosehome::requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        this->album_id_check = album_id;
        this->jsonArr_tracks_toPlay = QJsonArray();
        this->pageCnt = 0;

        roseHome::ProcCommon *proc = new roseHome::ProcCommon();
        connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &ProcRosePlay_withRosehome::slot_applyResult_albumTracks);
        proc->request_rose_get_album(this->album_id_check, pageCnt, 20);
    }


    void ProcRosePlay_withRosehome::requestPlayRose_byPlaylistID(const int playlist_id, OptMorePopup::ClickMode clickMode){

        this->curr_clickMode = clickMode;

        this->playlist_id_check = playlist_id;
        this->jsonArr_tracks_toPlay = QJsonArray();
        this->pageCnt = 0;

        roseHome::ProcCommon *proc = new roseHome::ProcCommon();
        connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &ProcRosePlay_withRosehome::slot_applyResult_playlsitTracks);
        proc->request_rose_get_playlist(this->playlist_id_check, pageCnt, 20);
    }


    void ProcRosePlay_withRosehome::slot_applyResult_albumTracks(const QList<roseHome::TrackItemData> &list_track, const QJsonArray &jsonArray_toPlay, const bool flag_last){

        Q_UNUSED(flag_last);
        int playType = this->get_rose_playType(this->curr_clickMode);

        if(this->pageCnt == 0 && list_track.size() <= 0)
        {
            return;
        }


        ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArray_toPlay);

        if(list_track.at(0).trackCount == this->jsonArr_tracks_toPlay.size()){
            QJsonObject tmpJson = QJsonObject();
            this->request_rose_tracks_set_queue(playType, tmpJson, this->jsonArr_tracks_toPlay, this->curr_shuffleMode);
        }
        else{
            this->pageCnt += 1;

            roseHome::ProcCommon *proc = new roseHome::ProcCommon();
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &ProcRosePlay_withRosehome::slot_applyResult_albumTracks);
            proc->request_rose_get_album(this->album_id_check, pageCnt, 20);
        }
    }


    void ProcRosePlay_withRosehome::slot_applyResult_playlsitTracks(const QList<roseHome::TrackItemData> &list_track, const QJsonArray &jsonArray_toPlay, const bool flag_last){

        Q_UNUSED(flag_last);
        int playType = this->get_rose_playType(this->curr_clickMode);

        if(this->pageCnt == 0 && list_track.size() <= 0)
        {
            return;
        }

        ProcJsonEasy::mergeJsonArray(this->jsonArr_tracks_toPlay, jsonArray_toPlay);

        if(list_track.at(0).trackCount == this->jsonArr_tracks_toPlay.size()){
            QJsonObject tmpJson = QJsonObject();
            this->request_rose_tracks_set_queue(playType, tmpJson, this->jsonArr_tracks_toPlay, this->curr_shuffleMode);
        }
        else{
            this->pageCnt += 1;

            roseHome::ProcCommon *proc = new roseHome::ProcCommon();
            connect(proc, &roseHome::ProcCommon::completeReq_list_tracks, this, &ProcRosePlay_withRosehome::slot_applyResult_playlsitTracks);
            proc->request_rose_get_playlist(this->playlist_id_check, pageCnt, 20);
        }
    }


    /**
     * @brief 트랙 목록(jsonArr_toPlayAll)에 대하여 clickMode 에 따라서 Rose에게 플레이 요청한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @param shuffleMode 기본값은 JustPlay.
     */
    void ProcRosePlay_withRosehome::requestPlayRose_byAlbum(const roseHome::AlbumItemData &data_album, const QJsonArray &jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){

        Q_UNUSED(data_album);

        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);

        QJsonObject album;

        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(jsonArr_toPlayAll, curr_index, clickMode);
        if(jsonArr_toPlayReal.count() > 0){
            this->request_rose_album_set_queue(playType, album, jsonArr_toPlayReal, shuffleMode);
        }
    }


    // Rose 에게 Play 요청 about Playlist   ------------------------------------------------------------------------

    /**
     * @brief 트랙 목록(jsonArr_toPlayAll)에 대하여 clickMode 에 따라서 Rose에게 플레이 요청한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @param shuffleMode 기본값은 JustPlay.
     */
    void ProcRosePlay_withRosehome::requestPlayRose_byPlaylist(const roseHome::PlaylistItemData &data_playlist, const QJsonArray &jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){

        Q_UNUSED(data_playlist);

        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);

        /*QString strplaylist = "";
        strplaylist += QString("{\"clientKey\":\"%1\",").arg(data_playlist.clientKey);
        strplaylist += QString("\"favorite\":%1,").arg(data_playlist.favorite);
        strplaylist += QString("\"favoriteTrack\":%1,").arg(data_playlist.favoriteTrack);
        strplaylist += QString("\"isRose\":%1,").arg(data_playlist.isRose);
        strplaylist += QString("\"ownerId\":%1,").arg(data_playlist.ownerId);
        strplaylist += QString("\"star\":%1,").arg(data_playlist.star);
        strplaylist += QString("\"thumbup\":%1,").arg(data_playlist.thumbup);
        strplaylist += QString("\"thumbupCount\":%1,").arg(data_playlist.thumbupCount);
        strplaylist += QString("\"trackCount\":%1").arg(data_playlist.trackCount);
        strplaylist += QString("\"type\":\"%1\"}").arg(data_playlist.type);

        QJsonObject playlist;
        playlist.insert("playlist", strplaylist);
        playlist.insert("type", "rose_playlist");*/

        QJsonObject playlist;

        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(jsonArr_toPlayAll, curr_index, clickMode);
        if(jsonArr_toPlayReal.count() > 0){
            this->request_rose_playlist_set_queue(playType, playlist, jsonArr_toPlayReal, shuffleMode);
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
    void ProcRosePlay_withRosehome::requestPlayRose_byTracks(const roseHome::TrackItemData &data_playlist, const QJsonArray &jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){

        Q_UNUSED(data_playlist);

        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);

        QJsonObject tracks;

        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(jsonArr_toPlayAll, curr_index, clickMode);

        if(jsonArr_toPlayReal.count() > 0){
            request_rose_tracks_set_queue(playType, tracks, jsonArr_toPlayReal, shuffleMode);
        }
    }


    // 실제 Http 요청 및 처리 파트   ------------------------------------------------------------------------

    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (음원)
     * @param playType
     * @param jsonObj_info
     * @param jsonArr_tracks
     * @param shuffleMode
     */
    void ProcRosePlay_withRosehome::request_rose_album_set_queue(const int playType, const QJsonObject &json_info, const QJsonArray &jsonArr_tracks, PlayShuffleMode shuffleMode){

        Q_UNUSED(json_info);
        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
        //jsonObj_send.insert("currentPosition", 0)
        //jsonObj_send.insert("playlist", json_info);
        jsonObj_send.insert("playType", playType);
        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", 0);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }
        jsonObj_send.insert("tracks", jsonArr_tracks);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());

        network->request(Rose_total_set_queue,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_play_list_add"),
                         jsonObj_send,
                         true,
                         true);
    }


    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (음원)
     * @param playType
     * @param jsonObj_info
     * @param jsonArr_tracks
     * @param shuffleMode
     */
    void ProcRosePlay_withRosehome::request_rose_playlist_set_queue(const int playType, const QJsonObject &json_info, const QJsonArray &jsonArr_tracks, PlayShuffleMode shuffleMode){

        Q_UNUSED(json_info);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
        //jsonObj_send.insert("currentPosition", 0)
        //jsonObj_send.insert("playlist", json_info);
        jsonObj_send.insert("playType", playType);
        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", 0);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }
        jsonObj_send.insert("tracks", jsonArr_tracks);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());

        network->request(Rose_total_set_queue,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_play_list_add"),
                         jsonObj_send,
                         true,
                         true);
    }


    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (음원)
     * @param playType
     * @param jsonObj_info
     * @param jsonArr_tracks
     * @param shuffleMode
     */
    void ProcRosePlay_withRosehome::request_rose_tracks_set_queue(const int playType, const QJsonObject &json_info, const QJsonArray &jsonArr_tracks, PlayShuffleMode shuffleMode){

        Q_UNUSED(json_info);

        if(global.Queue_track_count != 0) {
            print_debug();emit linker->signal_checkQueue(27, "");

            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_tracks.count();     // 220419 queue count
//
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
        //jsonObj_send.insert("currentPosition", 0);
        //jsonObj_send.insert("playlist", json_info);
        jsonObj_send.insert("playType", playType);
        // shuffle option
        if(jsonArr_tracks.size() < 2){
            jsonObj_send.insert("shuffle", -1);
        }
        else if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", 0);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }
        jsonObj_send.insert("tracks", jsonArr_tracks);        
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());

        network->request(Rose_total_set_queue,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_play_list_add"),
                         jsonObj_send,
                         true,
                         true);
    }


    /**
     * @brief ProcCommon::slot_responseHttp
     * @param p_id
     * @param p_jsonObj
     */
    void ProcRosePlay_withRosehome::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){
        //print_debug();
        //QJsonDocument doc(p_jsonObj);    QString strJson(doc.toJson(QJsonDocument::Compact));    qDebug() << "ProcRosePlay_withRosehome::slot_responseHttp = " << strJson;

        // 나중에 실패요인에 따라서 처리 보강
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            QString code = ProcJsonEasy::getString(p_jsonObj, "code");
            if(code == "G0000"){
                if(p_id == Rose_total_current_playlist_delete){
                    //emit this->completeReq_qobuzPlay_current_playlist_delete();
                }
                else if(p_id == Get_Session_Info){
                    global.user.setDataUser(p_jsonObj);

                    emit signal_completeReq_get_session_info();
                }
            }else if(p_id == Rose_total_set_queue){
                print_debug();//

            }
        }
        else{
            //emit this->signal_cannotRosePlay(tr("I requested the playback with Rose device, but an unknown error occurred."));
            //emit this->signal_cannotRosePlay(tr("Rose 기기로 재생요청을 하였으나, 알수없는 오류가 발생하였습니다."));
        }

        sender()->deleteLater();
    }
}
