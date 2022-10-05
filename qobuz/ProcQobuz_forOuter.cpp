#include "qobuz/ProcQobuz_forOuter.h"

#include "qobuz/ConvertData_forQobuz.h"
#include "qobuz/ProcCommon_forQobuz.h"

#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"

#include <QDebug>
#include <QJsonDocument>


namespace qobuz {


    /**
     * @brief 기본 생성자. 메모리 확인용
     */
    ProcQobuz_forOuter::ProcQobuz_forOuter(QWidget *parent)
        : QObject(parent)
    {
        this->parent = parent;
        if(this->flagShowDebug_funcName){
        }
    }


    /**
     * @brief 소멸자. 메모리 해제 체크확인용
     */
    ProcQobuz_forOuter::~ProcQobuz_forOuter(){

        if(this->flagShowDebug_funcName){
        }
        this->deleteLater();
    }


    // get data ------------------------------------------------------------------------------------------------------

    /**
     * @brief Json (Track) 데이터에서 track_id 추출해서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return track_id 반환
     */
    int ProcQobuz_forOuter::extract_track_id_onTrackData(const QJsonObject& jsonObj_subAppCurrentData){

        //print_qobuz_func();
        qobuz::TrackItemData tmp_data = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.id;
    }


    /**
     * @brief Json (Video) 데이터에서 video_id 추출해서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return video_id 반환
     */
    int ProcQobuz_forOuter::extract_video_id_onVideoData(const QJsonObject& jsonObj_subAppCurrentData){

        qobuz::VideoItemData tmp_data = ConvertData::make_videoData_fromQobuzJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.id;
    }


    /**
     * @brief Qobuz Track용 Json 데이터를 OptMorePopup용 HeaderData로 변환하여 반환
     * @param jsonObj_subAppCurrentData
     * @return OptMorePopup::HeaderData struct 반환
     */
    OptMorePopup::HeaderData ProcQobuz_forOuter::getConvertOptHeaderData_track(const QJsonObject& jsonObj_subAppCurrentData){

        qobuz::TrackItemData data_track = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj_subAppCurrentData);
        return ConvertData::getConvertOptHeaderData(data_track);
    }


    /**
     * @brief Qobuz Track용 Json 데이터를 활용하여, Album 상세페이지로 이동하기 위한 Json 만들어서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return
     */
    QJsonObject ProcQobuz_forOuter::makeJsonObj_toMovePage_albumDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData){

        // Track 데이터로 변환
        qobuz::TrackItemData data_track = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj_subAppCurrentData);

        if(data_track.album_id > 0){
            // Album 데이터 만들기
            qobuz::AlbumItemData tmp_data_album;
            tmp_data_album.id = data_track.album_id;

            // 페이지 이동을 위한 JsonObj 세팅
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ALBUM_DETAIL);
            jsonObj_move.insert(KEY_DATA, ConvertData::getObjectJson_albumData(tmp_data_album));
            return jsonObj_move;
        }
        else{
            return QJsonObject();
        }
    }


    /**
     * @brief ProcQobuz_forOuter::makeJsonObj_toMovePage_artistDetail_fromTrack
     * @param jsonObj_subAppCurrentData
     * @return
     */
    QJsonObject ProcQobuz_forOuter::makeJsonObj_toMovePage_artistDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData){

        // Track 데이터로 변환        
        qobuz::TrackItemData data_track = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj_subAppCurrentData);

        qobuz::ArtistItemData tmp_data_performer;
        if(jsonObj_subAppCurrentData.contains("performer")){
            QJsonObject jsonPar_artist = ProcJsonEasy::getJsonObject(jsonObj_subAppCurrentData, "performer");
            tmp_data_performer.id = (ProcJsonEasy::getInt(jsonPar_artist, "id"));
            tmp_data_performer.name = (ProcJsonEasy::getString(jsonPar_artist, "name"));
        }

        if(data_track.list_artist_id.count()>0 && data_track.list_artist_id.first()>0){
            // Artist 데이터 만들기
            qobuz::ArtistItemData tmp_data_artist;
            tmp_data_artist.id = data_track.list_artist_id.first();

            // 페이지 이동을 위한 JsonObj 세팅
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
            jsonObj_move.insert(KEY_DATA, ConvertData::getObjectJson_artistData(tmp_data_artist));
            return jsonObj_move;
        }
        else if(tmp_data_performer.id > 0){
            // 페이지 이동을 위한 JsonObj 세팅
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_Q_ARTIST_DETAIL);
            jsonObj_move.insert(KEY_DATA, ConvertData::getObjectJson_artistData(tmp_data_performer));
            return jsonObj_move;
        }
        else{
            return QJsonObject();
        }
    }


    // request ------------------------------------------------------------------------------------------------------

    /**
     * @brief Qobuz > Track 에 대해서, Favorite 정보를 요청.
     * @details 내부적으로 Fav ids 목록 요청 --> 해당 track_id 에 대해서 fav 인지 판별 --> signal 발생  (@see completeReq_flagFav_onTrack)
     * @param jsonObj_subAppCurrentData
     */
    void ProcQobuz_forOuter::request_flagFav_ofTrack(const QJsonObject& jsonObj_subAppCurrentData){

        if(this->flagShowDebug_funcName){
        }

        this->data_track = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj_subAppCurrentData);

        ProcCommon *proc = new ProcCommon(this->parent);
        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &ProcQobuz_forOuter::slot_qobuz_completeReq_listAll_myFavoritesIds___forTrack);
        proc->request_qobuz_getAll_favorites();
    }


    /**
     * @brief [Slot] request_qobuz_getAll_favorites 요청에 대한 처리
     * @see request_flagFav_ofTrack 함수와 관련된 Slot 임
     * @param p_jsonObj
     */
    void ProcQobuz_forOuter::slot_qobuz_completeReq_listAll_myFavoritesIds___forTrack(const QJsonObject& p_jsonObj){

        if(this->flagShowDebug_funcName){
        }

        // Favorite 여부 체크
        QVariantList list_ids = ProcJsonEasy::getJsonArray(p_jsonObj, "TRACK").toVariantList();
        QVariant var_track_id(this->data_track.id);

        emit this->completeReq_flagFav_onTrack(this->data_track.id, list_ids.contains(var_track_id));
    }


    /**
     * @brief Qobuz > Video 에 대해서, Favorite 정보를 요청.
     * @details 내부적으로 Fav ids 목록 요청 --> 해당 track_id 에 대해서 fav 인지 판별 --> signal 발생  (@see completeReq_flagFav_onVideo)
     * @param jsonObj_subAppCurrentData
     */
    void ProcQobuz_forOuter::request_flagFav_ofVideo(const QJsonObject& jsonObj_subAppCurrentData){

        if(this->flagShowDebug_funcName){
        }

        this->data_video = ConvertData::make_videoData_fromQobuzJsonObj(jsonObj_subAppCurrentData);

        ProcCommon *proc = new ProcCommon(this->parent);
        connect(proc, &qobuz::ProcCommon::completeReq_listAll_myFavoritesIds, this, &ProcQobuz_forOuter::slot_qobuz_completeReq_listAll_myFavoritesIds___forVideo);
        proc->request_qobuz_getAll_favorites();
    }


    /**
     * @brief [Slot] request_qobuz_getAll_favorites 요청에 대한 처리
     * @param p_jsonObj
     */
    void ProcQobuz_forOuter::slot_qobuz_completeReq_listAll_myFavoritesIds___forVideo(const QJsonObject& p_jsonObj){

        if(this->flagShowDebug_funcName){
        }

        // Favorite 여부 체크
        QVariantList list_ids = ProcJsonEasy::getJsonArray(p_jsonObj, "VIDEO").toVariantList();
        QVariant var_vedio_id(this->data_video.id);

        emit this->completeReq_flagFav_onVideo(this->data_video.id, list_ids.contains(var_vedio_id));
    }


    /**
     * @brief Track 을 선택한 사용자 플레이리스트에 담기 요청
     * @param playlist_uuid_target - 사용자가 선택한 플레이리스트의 Key
     * @param track_id - 트랙 Key
     */
    void ProcQobuz_forOuter::request_qobuz_add_track_in_myPlaylist(const QString playlist_uuid_target, const int track_id){

        ProcCommon *proc = new ProcCommon(this->parent);
        connect(proc, &ProcCommon::completeReq_add_items_in_myPlaylist, this, &ProcQobuz_forOuter::slot_done_addPlaylist);
        proc->request_qobuz_add_track_in_myPlaylist(playlist_uuid_target, track_id);
    }


    /**
     * @brief [Slot] request_qobuz_add_track_in_myPlaylist 함수와 관련된 Slot
     */
    void ProcQobuz_forOuter::slot_done_addPlaylist(){

       emit this->completeReq_add_track_in_myPlaylist();
    }


    /**
     * @brief Qobuz Track에 대하여 Favoriate를 Add, Delete 처리 요청
     * @param curr_isFav
     * @param jsonObj_subAppCurrentData
     */
    void ProcQobuz_forOuter::request_procFav_ofTrack(const bool curr_isFav, const QJsonObject& jsonObj_subAppCurrentData){

        Q_UNUSED(curr_isFav);
        qobuz::TrackItemData data_track = ConvertData::make_trackData_fromQobuzJsonObj(jsonObj_subAppCurrentData);

        if(curr_isFav){
            // delete favorite (fav on -> off)
            ProcCommon *proc = new ProcCommon(this->parent);
            //proc->request_qobuz_deleteFavorite_track(data_track.id);
            proc->request_qobuz_set_favorite("track", QString("%1").arg(data_track.id), !curr_isFav);
        }
        else{
            // add favorite (fav off -> on)
            ProcCommon *proc = new ProcCommon(this->parent);
            //proc->request_qobuz_addFavorite_track(data_track.id);
            proc->request_qobuz_set_favorite("track", QString("%1").arg(data_track.id), !curr_isFav);
        }
    }


    int ProcQobuz_forOuter::extract_track_id_onTrackData_Apple(const QJsonObject& jsonObj_subAppCurrentData){       //c220304

        //print_qobuz_func();
        qobuz::TrackItemData_apple tmp_data = ConvertData::make_trackData_fromAppleJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.id;
    }
}
