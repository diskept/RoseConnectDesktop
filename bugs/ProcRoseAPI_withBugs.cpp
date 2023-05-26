#include "ProcRoseAPI_withBugs.h"
#include "common/networkhttp.h"
#include "common/ProcJsonEasy.h"
#include "bugs/ConvertData_forBugs.h"
#include "ProcBugsAPI.h"
#include <QDate>//c221007_2
#include <QTimer>//c221007_2


#define print_debug() qDebug() << "\n" << "file_name: " << __FILE__ << "function_name: " << __FUNCTION__ << "line: " << __LINE__ << "\n";
namespace bugs {

    const QString CODE_ROSE___OK = "G0000";

    /**
     * @brief ProcRoseAPI_withBugs::ProcRoseAPI_withBugs
     * @param parent
     */
    ProcRoseAPI_withBugs::ProcRoseAPI_withBugs(QWidget *parent)
        : AbstractProcRosePlay(parent)
    {
        linker = Linker::getInstance();
    }


    /**
     * @brief ProcRoseAPI_withBugs::request_set_session_info
     * @param p_sessionInfo
     */
    void ProcRoseAPI_withBugs::request_set_session_info(const bugs::RoseSessionInfo_forBugs& p_sessionInfo){
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send = ConvertData_forBugs::getObjectJson_roseSessionInfo(p_sessionInfo);
        network->request(HttpRequestType::Set_Session_Info
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("bugs_set_session_info")
                         , jsonObj_send
                         , true
                         , true);
    }


    /**
     * @brief 세션 정보를 요청함
     */
    void ProcRoseAPI_withBugs::request_get_session_info(){

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
//        jsonObj_send.insert("dummy", "");
        network->request(HttpRequestType::Get_Session_Info
                         , QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("bugs_get_session_info")
                         , jsonObj_send
                         , true
                         , true);

    }





    // Rose 에게 Play 요청 about Album, PD's Album, My Album  ------------------------------------------------------------------------


    /**
     * @brief album_id로 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param album_id
     * @param clickMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byAlbumID(const int album_id, OptMorePopup::ClickMode clickMode){
        this->curr_clickMode = clickMode;

        ProcBugsAPI *proc = new ProcBugsAPI(this->parent);
        connect(proc, &ProcBugsAPI::completeReq_list_items_of_album, this, &ProcRoseAPI_withBugs::slot_applyResult_tracks);
        proc->request_bugs_getList_items_of_album(album_id);
    }


    /**
     * @brief album_id로 전체재생, 셔플재생 요청한다. AbstractImageDetailInfo::BtnClickMode 을 그대로 전달받아서 알아서 처리하도록 한다.
     * @details OptMorePopup의 Play_RightNow 모드로 재생된다.
     * @param album_id
     * @param btnClickMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byAlbumID(const int album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        this->curr_shuffleMode = AbstractProcRosePlay::PlayShuffleMode::JustPlay;
        if(btnClickMode == AbstractImageDetailInfo::PlayShuffle){
            this->curr_shuffleMode = AbstractProcRosePlay::PlayShuffleMode::ShufflePlay;
        }

        this->requestPlayRose_byAlbumID(album_id, OptMorePopup::ClickMode::Play_RightNow);
    }


    /**
     * @brief [Slot] Album, PD's Album, My Album의 내부 Track 목록에 대한 처리 -> 공용으로 사용
     * @param list_track
     * @param jsonArray_toPlay
     */
    void ProcRoseAPI_withBugs::slot_applyResult_tracks(const QList<bugs::TrackItemData>& list_track, const QJsonArray& jsonArray_toPlay){

        int playType = this->get_rose_playType(this->curr_clickMode);

        if(global.user_forBugs.isLogined()){//c221116_2
            print_debug();
        }else{

            print_debug();
            emit linker->signal_checkQueue(35, tr("Bugs type is currently logged out."));
            return;
        }
        if(list_track.size() <= 0)
        {
            return;
        }

        this->request_rose_bugsPlay_set_queue(playType, jsonArray_toPlay, this->curr_shuffleMode);
    }






    /**
     * @brief playlist_id로 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param playlist_id
     * @param clickMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byMyAlbumID(const int playlist_id, OptMorePopup::ClickMode clickMode){
        this->curr_clickMode = clickMode;

        ProcBugsAPI *proc = new ProcBugsAPI(this->parent);
        connect(proc, &ProcBugsAPI::completeReq_list_items_of_my_album, this, &ProcRoseAPI_withBugs::slot_applyResult_tracks);
        proc->request_bugs_getList_items_of_my_album(playlist_id, 0, 100);          // 기본값. 바로재생일 때에는 최대 100곡만 지원
    }


    /**
     * @brief playlist_id로 전체재생, 셔플재생 요청한다. AbstractImageDetailInfo::BtnClickMode 을 그대로 전달받아서 알아서 처리하도록 한다.
     * @details OptMorePopup의 Play_RightNow 모드로 재생된다.
     * @param playlist_id
     * @param btnClickMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byMyAlbumID(const int playlist_id, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        this->curr_shuffleMode = AbstractProcRosePlay::PlayShuffleMode::JustPlay;
        if(btnClickMode == AbstractImageDetailInfo::PlayShuffle){
            this->curr_shuffleMode = AbstractProcRosePlay::PlayShuffleMode::ShufflePlay;
        }

        this->requestPlayRose_byMyAlbumID(playlist_id, OptMorePopup::ClickMode::Play_RightNow);
    }




    /**
     * @brief es_album_id로 재생요청한다. OptMorePopup::ClickMode를 그대로 전달받아서 알아서 처리하도록 한다.
     * @param es_album_id
     * @param clickMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byPD_AlbumID(const int es_album_id, OptMorePopup::ClickMode clickMode){
        this->curr_clickMode = clickMode;

        ProcBugsAPI *proc = new ProcBugsAPI(this->parent);
        connect(proc, &ProcBugsAPI::completeReq_list_items_of_pd_album, this, &ProcRoseAPI_withBugs::slot_applyResult_tracks);
        proc->request_bugs_getList_items_of_pd_album(es_album_id);
    }


    /**
     * @brief es_album_id로 전체재생, 셔플재생 요청한다. AbstractImageDetailInfo::BtnClickMode 을 그대로 전달받아서 알아서 처리하도록 한다.
     * @details OptMorePopup의 Play_RightNow 모드로 재생된다.
     * @param es_album_id
     * @param btnClickMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byPD_AlbumID(const int es_album_id, AbstractImageDetailInfo::BtnClickMode btnClickMode){
        this->curr_shuffleMode = AbstractProcRosePlay::PlayShuffleMode::JustPlay;
        if(btnClickMode == AbstractImageDetailInfo::PlayShuffle){
            this->curr_shuffleMode = AbstractProcRosePlay::PlayShuffleMode::ShufflePlay;
        }

        this->requestPlayRose_byPD_AlbumID(es_album_id, OptMorePopup::ClickMode::Play_RightNow);
    }




    // Rose 에게 Play 요청 about Tracks   ------------------------------------------------------------------------

    /**
     * @brief 트랙 목록(jsonArr_toPlayAll)에 대하여 clickMode 에 따라서 Rose에게 플레이 요청한다.
     * @param jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @param shuffleMode 기본값은 JustPlay.
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byTracks(const QJsonArray& p_jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){
        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);

        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(p_jsonArr_toPlayAll, curr_index, clickMode);
        if(jsonArr_toPlayReal.count() > 0){
            this->request_rose_bugsPlay_set_queue(playType, jsonArr_toPlayReal, shuffleMode);
        }
        else{
            print_debug();
        }
    }



    /**
     * @brief Video 목록(jsonArr_toPlayAll)에 대하여 clickMode 에 따라서 Rose에게 플레이 요청한다.
     * @param p_jsonArr_toPlayAll
     * @param curr_index
     * @param clickMode
     * @param shuffleMode
     */
    void ProcRoseAPI_withBugs::requestPlayRose_byVideos(const QJsonArray& p_jsonArr_toPlayAll, const int curr_index, OptMorePopup::ClickMode clickMode, PlayShuffleMode shuffleMode){
        this->curr_clickMode = clickMode;
        int playType = this->get_rose_playType(this->curr_clickMode);

        // needed to get JsonArray rearranged to play on the Rose
        QJsonArray jsonArr_toPlayReal = this->get_rearrangeJsonArray_toPlayData(p_jsonArr_toPlayAll, curr_index, clickMode);
        if(jsonArr_toPlayReal.count() > 0){
            this->request_rose_bugsPlay_set_queue_video(playType, jsonArr_toPlayReal, shuffleMode);
        }
    }



    /**
     * @brief Queue에서 Track 하나를 선택한 경우임 (재생을 위해)
     * @param currentPosition
     */
    void ProcRoseAPI_withBugs::request_setQueue_single(const int currentPosition, const QJsonObject& jsonObj_track){//cheon210916-total
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


        QJsonDocument doc(jsonObj_track);  QString strToJson(doc.toJson(QJsonDocument::Compact)); //qDebug() << "ProcRoseAPI_withBugs::request_setQueue_single---" << strToJson;
        QJsonObject jsonObj_send, tt;
        jsonObj_send.insert("data",strToJson);
        jsonObj_send.insert("currentPlayer", "BUGS");
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("remotePositionPlay", currentPosition);
        jsonObj_send.insert("queueMode", "TOTAL");//cheon211104-01
        qDebug() << "ProcRoseAPI_withBugs::request_setQueue_single----global.checkSum = " << global.checkSum;
        jsonObj_send.insert("checkSum", global.checkSum);

        QJsonDocument doc1(jsonObj_send);  QString strJson(doc1.toJson(QJsonDocument::Compact));  qDebug() << "ProcRoseAPI_withBugs::request_setQueue_single---" << strJson;
        qDebug() << "ProcRoseAPI_withBugs::request_setQueue_single----requst_str = " << QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set");
        //QString tmp_string = jsonObj_send["data"].toString();
        //print_debug();
        //qDebug() << "tmp_string -- : " << tmp_string;
        network->request(Rose_bugs_set_queue_single,

                         //QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("qobuz_set_queue.single"),
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_set"),
                         jsonObj_send,
                         true,
                         true);
    }



    /**
     * @brief Queue에서 Track들을 삭제 요청함
     * @param totalCnt - 현재의 Tidal Queue 데이터 개수
     * @param jsonArr_deletePosition
     * @param jsonArr_deleteArr
     */
    void ProcRoseAPI_withBugs::request_bugsPlay_current_playlist_delete(const int totalCnt, const QJsonArray& jsonArr_deletePosition, const QJsonArray& jsonArr_deleteArr){//cheon210916-delete

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

        network->request(Rose_bugsPlay_current_playlist_delete,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("total_queue_delete"),
                         jsonObj_send,
                         true,
                         true);

        /*
        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        // 모두 string 으로 보내야 함
        QJsonObject jsonObj_send;
        jsonObj_send.insert("totalCnt", totalCnt);
        jsonObj_send.insert("deletePosition", jsonArr_deletePosition);
        jsonObj_send.insert("deleteArr", jsonArr_deleteArr);

        network->request(Rose_bugsPlay_current_playlist_delete,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("bugsPlay_current_playlist_delete"),
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
    void ProcRoseAPI_withBugs::request_bugsPlay_current_playlist_delete(const int totalCnt, const int deletePosition, const QJsonObject& jsonObj_deleteObj){
        QJsonArray jsonArr_deletePositon;
        jsonArr_deletePositon.append(deletePosition);

        QJsonArray jsonArr_deleteArr;
        jsonArr_deleteArr.append(jsonObj_deleteObj);

        this->request_bugsPlay_current_playlist_delete(totalCnt, jsonArr_deletePositon, jsonArr_deleteArr);
    }



    // 실제 Http 요청 및 처리 파트   ------------------------------------------------------------------------

    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (음원)
     * @param playType
     * @param jsonArr_tidalModeItem
     * @param shuffleMode
     */
    void ProcRoseAPI_withBugs::request_rose_bugsPlay_set_queue(const int playType, const QJsonArray &jsonArr_tidalModeItem, PlayShuffleMode shuffleMode){

        if(global.Queue_track_count != 0) {

            print_debug();emit linker->signal_checkQueue(27, "");
            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_tidalModeItem.count();     // 220419 queue count

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));

        QJsonObject jsonObj_send;
        jsonObj_send.insert("currentPosition", 0);
        jsonObj_send.insert("playType", playType);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("tracks", jsonArr_tidalModeItem);            // 동영상일 때와는 key 값이 다름

        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", "0");
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", "1");
        }

        network->request(Rose_bugs_set_queue,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("bugs_set_queue"),
                         jsonObj_send,
                         true,
                         true);
    }

    /**
     * @brief 로즈에게 재생목록 전달하여 플레이 요청 (동영상 N개)
     * @param playType
     * @param PlayShuffleMode shuffleMode
     * @param jsonObj_toPlay
     * @param shuffleMode - 2020-08-10 Android 3.3.4, Rose 3.4 기준으로 항상 shuffle=0 으로 전달하고 있어서, 기본값이 JustPlay임.
     */
    void ProcRoseAPI_withBugs::request_rose_bugsPlay_set_queue_video(const int playType, const QJsonArray &jsonArr_tidalModeItem, PlayShuffleMode shuffleMode){

        if(global.Queue_track_count != 0) {

            print_debug();emit linker->signal_checkQueue(27, "");
            return;
        }
        print_debug(); emit linker->signal_queuelist_mouse_trigger_menu_flag();
        global.Queue_track_count += jsonArr_tidalModeItem.count();

        NetworkHttp *network = new NetworkHttp;
        connect(network, SIGNAL(response(int,QJsonObject)), SLOT(slot_responseHttp(int,QJsonObject)));


        QJsonObject jsonObj_send;
        jsonObj_send.insert("currentPosition", 0);
        jsonObj_send.insert("playType", playType);
        jsonObj_send.insert("roseToken", global.device.getDeviceRoseToken());
        jsonObj_send.insert("mvs", jsonArr_tidalModeItem);

        // shuffle option
        if(shuffleMode == JustPlay){
            jsonObj_send.insert("shuffle", 0);
        }
        else if(shuffleMode == ShufflePlay){
            jsonObj_send.insert("shuffle", 1);
        }

        network->request(Rose_bugs_set_queue_video,
                         QString("http://%1:%2/%3").arg(global.device.getDeviceIP()).arg(global.port).arg("bugs_set_queue.video"),
                         jsonObj_send,
                         true,
                         true);

    }


    //-----------------------------------------------------------------------------------------------------------------------
    //
    // MARK : Http 요청 결과 처리
    //
    //-----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief ProcRoseAPI_withBugs::slot_responseHttp
     * @param p_id
     * @param p_jsonObj
     */
    void ProcRoseAPI_withBugs::slot_responseHttp(const int& p_id, const QJsonObject& p_jsonObj){
        print_debug();
        qDebug() << "p_id=" << p_id;
        QJsonDocument doc(p_jsonObj);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "ProcRoseAPI_withBugs::slot_responseHttp---" << strJson;

        switch (p_id) {
            case HttpRequestType::Set_Session_Info:     this->checkResult_set_session_info(p_jsonObj);       break;
            case HttpRequestType::Get_Session_Info:     this->checkResult_get_session_info(p_jsonObj);       break;
//            default :
        }

        // 나중에 실패요인에 따라서 처리 보강
        if(ProcJsonEasy::get_flagOk(p_jsonObj)){
            bool checkQueue = ProcJsonEasy::getBool(p_jsonObj, "checkQueue");//c1209
            if(checkQueue == false){//c1209
                linker = Linker::getInstance();
                if(global.queueTimerFlag){
                    //emit linker->signal_checkQueue(6, "");
                }
                //emit linker->signal_checkQueue(1, "");

            }else{
                linker = Linker::getInstance();
                //emit linker->signal_checkQueue(6, "");//c220411
                //if(global.queuelist_mouse_trigger_menu_flag)
                    emit linker->signal_music_start();//c220411

            }
            QString code = ProcJsonEasy::getString(p_jsonObj, "code");
            if(code == "G0000"){//c221007_2
                if(p_jsonObj.contains("data")){
                    QString tmp_data = ProcJsonEasy::getString(p_jsonObj, "data", "{}");
                    QJsonDocument jsocDoc = QJsonDocument::fromJson(tmp_data.toUtf8());
                    QJsonObject jsonObj_data = jsocDoc.object();
                    QJsonDocument doc(jsonObj_data);  QString strJson(doc.toJson(QJsonDocument::Compact));  qDebug() << "ProcRoseAPI_withBugs::slot_responseHttp--jsonObj_data-" << strJson;

                    QString BUGS_ProductDisplayEndDt = ProcJsonEasy::getString(jsonObj_data, "BUGS_ProductDisplayEndDt");//c221007_2
                    BUGS_ProductDisplayEndDt.chop(2);
                    /*
                                    qDebug() << "global.user_forBugs.getProduct_display_end_dt()=" << global.user_forBugs.getProduct_display_end_dt();
                                    qDebug() << "BUGS_ProductDisplayEndDt=" << BUGS_ProductDisplayEndDt;
                                    QString tt = BUGS_ProductDisplayEndDt;
                                    QDateTime e_date = QDateTime::fromString(tt,"yyyy.MM.dd HH:mm");
                                    qDebug() << "e_date=" << e_date;
                                    QDateTime c_date = QDateTime::currentDateTime();
                                    qDebug() << "c_date=" << c_date;
                                    if(e_date.isNull() || e_date < c_date){//c230426
                                        print_debug();
                                        slot_clickBtnLogout();
                                    }else{
                                        print_debug();
                                    }
                                    */

                    global.user_forBugs.setProduct_display_end_dt(BUGS_ProductDisplayEndDt);
                }
                if(p_id == Rose_bugsPlay_current_playlist_delete){
                    emit this->completeReq_bugsPlay_current_playlist_delete();
                }
            }else if(code == "SLEEP"){
                print_debug();
                emit linker->signal_checkQueue(24, "");

            }
            if(ProcJsonEasy::getBool(p_jsonObj, "checkQueue")==true){//cheon211107-01
                //print_debug();
                //qDebug()<< "checkQueue = " << ProcJsonEasy::getBool(p_jsonObj, "checkQueue");
            }else{
                //print_debug();
                //qDebug()<< "checkQueue = " << ProcJsonEasy::getBool(p_jsonObj, "checkQueue");
            }
        }


        sender()->deleteLater();

    }

    void ProcRoseAPI_withBugs::slot_clickBtnLogout(){//c221007_2
        // 로그아웃 처리 (PC 상태)
        ProcBugsAPI *proc = new ProcBugsAPI();
        proc->request_logout();

        // Rose 기기에 Bugs 로그아웃 처리
        ProcRoseAPI_withBugs *procRose = new ProcRoseAPI_withBugs();
        bugs::RoseSessionInfo_forBugs sessionInfo_init;
        procRose->request_set_session_info(sessionInfo_init);

        // 상태 변경
        //applyLoginSuccessState();
    }

    /**
     * @brief ProcRoseAPI_withBugs::checkResult_set_session_info
     * @param p_jsonObj
     */
    void ProcRoseAPI_withBugs::checkResult_set_session_info(const QJsonObject& p_jsonObj){
        Q_UNUSED(p_jsonObj);
        // donothing
    }


    /**
     * @brief ProcRoseAPI_withBugs::checkResult_get_session_info
     * @param p_jsonObj
     */
    void ProcRoseAPI_withBugs::checkResult_get_session_info(const QJsonObject& p_jsonObj){
        if(ProcJsonEasy::getString(p_jsonObj, "code") == CODE_ROSE___OK){
            QString tmp_data = ProcJsonEasy::getString(p_jsonObj, "data", "{}");
            QJsonDocument jsocDoc = QJsonDocument::fromJson(tmp_data.toUtf8());
            QJsonObject jsonObj_data = jsocDoc.object();

            bugs::RoseSessionInfo_forBugs sessionInfo = ConvertData_forBugs::convertData_roseSessionInfo(jsonObj_data);
            emit this->signal_completeReq_get_session_info(sessionInfo);
        }
    }

}
