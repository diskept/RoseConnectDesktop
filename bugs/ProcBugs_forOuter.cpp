#include "ProcBugs_forOuter.h"
#include <QDebug>
#include "ConvertData_forBugs.h"
#include "ProcBugsAPI.h"
#include "common/ProcJsonEasy.h"
#include "common/gscommon.h"


namespace bugs {

    /**
     * @brief 기본 생성자. 메모리 확인용
     */
    ProcBugs_forOuter::ProcBugs_forOuter(QWidget *parent)
        : QObject(parent)
    {
        this->parent = parent;
        if(this->flagShowDebug_funcName){
        }
    }


    /**
     * @brief 소멸자. 메모리 해제 체크확인용
     */
    ProcBugs_forOuter::~ProcBugs_forOuter(){
        if(this->flagShowDebug_funcName){
        }
    }









    // get data ------------------------------------------------------------------------------------------------------

    /**
     * @brief Json (Track) 데이터에서 track_id 추출해서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return track_id 반환
     */
    int ProcBugs_forOuter::extract_track_id_onTrackData(const QJsonObject& jsonObj_subAppCurrentData){
        bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.track_id;
    }

    /**
     * @brief Json (Video) 데이터에서 video_id 추출해서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return video_id 반환
     */
    int ProcBugs_forOuter::extract_video_id_onVideoData(const QJsonObject& jsonObj_subAppCurrentData){
        bugs::VideoItemData tmp_data = ConvertData_forBugs::make_videoData_fromBugsJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.mv_id;
    }


    /**
     * @brief Bugs Track 에 대하여 "좋아요" 여부 반환
     * @param jsonObj_subAppCurrentData
     * @return true | false
     */
    bool ProcBugs_forOuter::extract_track_likes_yn(const QJsonObject& jsonObj_subAppCurrentData){
        bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.likes_yn;
    }

    /**
     * @brief Bugs Track 에 대하여, 연관 Music Video 가 있는지 여부를 반환함.
     * @param jsonObj_subAppCurrentData
     * @return true | false
     */
    bool ProcBugs_forOuter::extract_track_has_musicVideo(const QJsonObject& jsonObj_subAppCurrentData){
        bugs::TrackItemData tmp_data = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);
        return tmp_data.list_mv_id.count()>0 ? true : false;
    }

    /**
     * @brief Bugs Track용 Json 데이터를 OptMorePopup용 HeaderData로 변환하여 반환
     * @param jsonObj_subAppCurrentData
     * @return OptMorePopup::HeaderData struct 반환
     */
    OptMorePopup::HeaderData ProcBugs_forOuter::getConvertOptHeaderData_of_track(const QJsonObject& jsonObj_subAppCurrentData){
        bugs::TrackItemData data_track = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);
        return ConvertData_forBugs::getConvertOptHeaderData(data_track);
    }

    /**
     * @brief Bugs Video용 Json 데이터를 OptMorePopup용 HeaderData로 변환하여 반환
     * @param jsonObj_subAppCurrentData
     * @return OptMorePopup::HeaderData struct 반환
     */
    OptMorePopup::HeaderData ProcBugs_forOuter::getConvertOptHeaderData_of_video(const QJsonObject& jsonObj_subAppCurrentData){
        bugs::VideoItemData data_track = ConvertData_forBugs::make_videoData_fromBugsJsonObj(jsonObj_subAppCurrentData);
        return ConvertData_forBugs::getConvertOptHeaderData(data_track);
    }



    /**
     * @brief Bugs Track용 Json 데이터를 활용하여, Album 상세페이지로 이동하기 위한 Json 만들어서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return
     */
    QJsonObject ProcBugs_forOuter::makeJsonObj_toMovePage_albumDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData){
        // Track 데이터로 변환
        bugs::TrackItemData data_track = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);

        if(data_track.album_id > 0){
            // Album 데이터 만들기
            bugs::AlbumItemData tmp_data_album;
            tmp_data_album.album_id = data_track.album_id;

            // 페이지 이동을 위한 JsonObj 세팅
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ALBUM_DETAIL);
            jsonObj_move.insert(KEY_DATA, ConvertData_forBugs::getObjectJson_albumData(tmp_data_album));
            return jsonObj_move;
        }
        else{
            return QJsonObject();
        }
    }


    /**
     * @brief Bugs Track용 Json 데이터를 활용하여, Artist 상세페이지로 이동하기 위한 Json 만들어서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return
     */
    QJsonObject ProcBugs_forOuter::makeJsonObj_toMovePage_artistDetail_fromTrack(const QJsonObject& jsonObj_subAppCurrentData){

        // Track 데이터로 변환
        bugs::TrackItemData data_track = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);

        if(data_track.list_artist_id.count()>0 && data_track.list_artist_id.first()>0){
            // Artist 데이터 만들기
            bugs::ArtistItemData tmp_data_artist;
            tmp_data_artist.artist_id = data_track.list_artist_id.first();

            // 페이지 이동을 위한 JsonObj 세팅
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);
            jsonObj_move.insert(KEY_DATA, ConvertData_forBugs::getObjectJson_artistData(tmp_data_artist));
            return jsonObj_move;
        }
        else{
            return QJsonObject();
        }
    }




    /**
     * @brief Bugs Video용 Json 데이터를 활용하여, Artist 상세페이지로 이동하기 위한 Json 만들어서 반환한다.
     * @param jsonObj_subAppCurrentData
     * @return
     */
    QJsonObject ProcBugs_forOuter::makeJsonObj_toMovePage_artistDetail_fromVideo(const QJsonObject &jsonObj_subAppCurrentData){
        // Video 데이터로 변환
        bugs::VideoItemData data_video = ConvertData_forBugs::make_videoData_fromBugsJsonObj(jsonObj_subAppCurrentData);

        if(data_video.list_artist_id.count()>0 && data_video.list_artist_id.first()>0){
            // Artist 데이터 만들기
            bugs::ArtistItemData tmp_data_artist;
            tmp_data_artist.artist_id = data_video.list_artist_id.first();

            // 페이지 이동을 위한 JsonObj 세팅
            QJsonObject jsonObj_move;
            jsonObj_move.insert(KEY_PAGE_CODE, PAGECODE_BUGS___ARTIST_DETAIL);
            jsonObj_move.insert(KEY_DATA, ConvertData_forBugs::getObjectJson_artistData(tmp_data_artist));
            return jsonObj_move;
        }
        else{
            return QJsonObject();
        }
    }





    /**
     * @brief Tidal Track에 대하여 Favoriate를 Add, Delete 처리 요청
     * @param curr_isFav
     * @param jsonObj_subAppCurrentData
     */
    void ProcBugs_forOuter::request_procFav_ofTrack(const bool curr_isFav, const QJsonObject& jsonObj_subAppCurrentData){
        bugs::TrackItemData data_track = ConvertData_forBugs::make_trackData_fromBugsJsonObj(jsonObj_subAppCurrentData);

        if(curr_isFav){
            // delete favorite (fav on -> off)
            ProcBugsAPI *proc = new ProcBugsAPI(this->parent);
            proc->request_bugs_deleteFavorite_track(data_track.track_id, QJsonObject());
        }
        else{
            // add favorite (fav off -> on)
            ProcBugsAPI *proc = new ProcBugsAPI(this->parent);
            proc->request_bugs_addFavorite_track(data_track.track_id, QJsonObject());
        }
    }


}
